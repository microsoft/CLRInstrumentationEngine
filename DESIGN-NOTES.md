# Instrumentation Engine design notes

1. [Introduction](#introduction)
2. [Components and Interfaces](#components)
3. [Profiler Manager](#profiler-manager)
4. [Profiler Callbacks](#profiler-callbacks)
5. [Infrastructure Interfaces](#infrastructure)
6. [Abstractions](#abstractions)
    * [DataItems](#dataitems)
    * [Instruction Graph](#instruction-graph)
    * [Method Info](#method)
    * [Module Info](#module)
    * [Assembly Info](#assembly)
    * [AppDomain Info](#appdomain)
    * [Local Variables](#local-var)
7. [Single Ret and Other Automatic Transformations](#transformations)
8. [Metadata](#metadata)
9. [Rejit](#rejit)

## <a name="introduction">Introduction</a>

IL Rewriting instrumentation is a fundamental technique used by diagnostic tools for monitoring applications or extracting information out of a process. This allows a diagnostic tool the opportunity to modify or replace the IL (intermediate language) form of a method before it is jit compiled and executed.

Within the CLR, IL Rewriting is enabled using the [ICorProfiler](https://msdn.microsoft.com/library/ms233177.aspx) apis. Examples of thing clients do with these apis are:

1. Modify the IL for a method
2. Add or remove metadata to an application at module load time
3. Receive a callback on method entry or exit
4. Manipulate optimizations applied to be applied during the jit compilation

Furthermore, the CLR allows a client to request a new jit compilation pass via “Rejit” allowing the IL to be manipulated on-the-fly as users modify dials or the instrumentation scenario changes.

One long standing limitation of the profiler approach to diagnostics is the profiler api only allows a single profiler to exist in a process. This means any multiplexing of the functionality must be performed by the profiler client. This has proven to be problematic as different scenarios need to be consumers of the profiler functionality against the same application.

Instrumentation Engine allows multiple profilers co-exist in the same process. It also simplifies some basic scenarios providing higher level interfaces.

## <a name="components">Component and Interface Design Points</a>

One of the goals of this design is to allow for consumers of the new instrumentation engine to be implemented in separate dlls. This allows them to be implemented by separate teams, and shared between different products. Achieving these goals will require a binary compatibility contract. Instrumentation Engine uses a form of Reg-Free COM that reuses the interface concept including IUnknown but does not bring in com baggage such as apartment threading or marshaling. This nicely matches the existing model of ICorProfiler.

Sticking with the Intellitrace terminology, these instrumentation components called "Instrumentation Methods" which will essentially be free threaded com objects loaded by direct call to the class factory using the com server exports (DllGetClassObject etc…). This has the advantage of requiring a simple vtable dispatch for each cross component call keeping overhead to a minimum.

Since ICorProfiler reserves the right to callback on multiple threads simultaneously, the instrumentation methods will need to act as free-threaded objects and take responsibility for their own locking. This is nothing new, as ICorProfiler has always had that requirement.

In order to allow for xcopy deploy without admin rights, the registration of the instrumentation components is done via an object configuration model using xml files. This is similar to how intellitrace manages instrumentation methods today with one key difference: the instrumentation objects will likely be packaged in their own dlls. Therefore, for MSI based installations, a safe admin writable folder will contain the dlls, and the configuration file will describe the objects that are contained within that dll. For xcopy deployment, the sub folder containing the instrumentation methods will be a sub-folder of the xcopy payload and the xml config will be contained in the same folder as the monitoring agent.

Example Configuration (NOTE: this is just an example, not a finished design):

``` xml
<InstrumentationEngineConfiguration>
  <InstrumentationMethod>
    <Name>Seafood Instrumentation</Name>
    <Description>Dynamically make squids swim</Description>
    <Module>SeafoodInstrumentation.dll</Module>
    <ClassGuid>{1234-12-123-124354}</ClassGuid>
    <Priority>50</Priority>
  </InstrumentationMethod>
</InstrumentationEngineConfiguration>
```

Alternative way of registering and unregistering of Instrumentation Methods is by calling the method `AddInstrumentationMethod` from the code. This allows late-binding scenarios when certain features are enabled and disabled from the code dynamically:

```
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AddInstrumentationMethod(
    _In_ BSTR bstrModulePath,
    _In_ BSTR bstrName,
    _In_ BSTR bstrDescription,
    _In_ BSTR bstrModule,
    _In_ BSTR bstrClassGuid,
    _In_ DWORD dwPriority,
    _Out_ IInstrumentationMethod** ppInstrumentationMethod)

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemoveInstrumentationMethod(
    _In_ IInstrumentationMethod* pInstrumentationMethod)
```

## <a name="profiler-manager">Profiler Manager</a>

The idea behind the high level instrumentation engine is to allow instrumentation methods to coexist in an instrumentation scenario while maintaining a higher level api for manipulating IL than the ICorProfiler apis allow. The Profiler manager is a new object that is responsible for loading up the various instrumentation methods, querying them about what profiler flags to set, multiplexing the events to the various instrumentation methods, and managing the IL graph that allows them to coexist.

In order to allow for reuse across products, Profiler manager will live in a dll `MicrosoftInstrumentationEngine_x(86|64).dll`.

To use Profiler manager, the host application (MMA, Intellitrace, code coverage etc..) should set up the process up for profiling using the normal ICorProfiler environment variables:

```
COR_ENABLE_PROFILING=1
COR_PROFILER={324F817A-7420-4E6D-B3C1-143FBED6D855}
COR_PROFILER_PATH=<full path of the profiler DLL>\MicrosoftInstrumentationEngine_x(86|64).dll
```

The host can either set the clsid directly to ProfilerManager, or implement the callback interfaces themselves and forward the calls to an instance of ProfilerManager.

## <a name="profiler-callbacks">Profiler Callbacks and Application State</a>

Many profiler consumers will need callbacks when interesting events happen in the target process. The list of known callbacks used by our current scenarios are:

1. Module load
    + This is used to add new metadata to the assembly
    + Parameters:
        + Module info (Assembly name, strong name, path,  Appdomain)
        + IMetadataImport / IMetadataExport
2. Before Jit Complete
    + This is the primary callback where IL rewriting occurs
    + Parameters
        + Method Info
        + IL Graph
3. On Rejit
    + This occurs the next time a specific function is called after a rejit request is made and like Before Jit Complete, is used to rewrite method il.
    + Parameters
        + Method Info
        * IL Graph
4. OnClassLoad
    * Intellitrace uses this callback which occurs every time a new class loads to update the type system. This callback MUST be optional as it incurs high overhead
    * Parameters:
        * Method Info
        * Class Info?
        * IMetadataImport
5. Function Enter / Function Exit
    * These callbacks are issued every time a function is called and every time a function returns. Intellitrace and Production BPs don’t use these, but Tofino does. Note that these also incur large runtime overhead and use a naked calling convention making it difficult to multiplex
6. Inline tracking
    * This allows components to track where a method has been inlined. Intellitrace turns inlining off globally today. Tofino allows inlining to occur.
    * Inlining greatly complicates the way the profiler works. However, the increase in performance is likely worth building something into the high level structures to make it easy to go from a metadata token to its inline sites to allow consumers to easily rejit all of the parent function to undo inlining.

Instrumentation Methods implements the ICorProfilerCallback interfaces (2, 3, 4, 5) to receive notifications of these events from the profiler manager. The profiler manager will expose the higher level api to allow the instrumentation methods to co-exist and make writing them easier. For instance, when receiving a jit callback, instrumentation methods can query the ProfilerManager to map MethodId to an instance of IMethodInfo and use that to get the instruction graph, examine metadata etc…

## <a name="infrastructure">Infrastructure Interfaces</a>

### InstrumentationMethod object and IInstrumentationMethod interface
Apart from implementing the interesting ICorProfilerCallback interfaces, InstrumentationMethods also implement IInstrumentationMethod. This interface contains an InstrumentationMethod specific Initialize call that receives the ProfilerManager instance from which the raw ICorProfilerInfo can be accessed as well as the higher level api.

Instrumentation Methods should call `IProfilerManager::SetEventMask` to specify which events it would like to receive. This function takes the clsid of the InstrumentationMethod and the flags. Note that just like with the clr, this can be called to enable / disable callbacks anytime. The ProfilerManager will obtain the event flags from each instrumentation method and enable the union of them. However, an InstrumentationMethod will only receive the events for which is subscribes.

In the current ICorProfiler design, there can only be one single function enter, function exit, and function tail call callback function. This is no longer the case with this new model. Instrumentation methods that wish to receive these callbacks can do so enabling the event mask on the Profiler Manager and then setting the callbacks passing the clsid of the instrumentation method. The ProfilerManager will multiplex the callbacks to each of the interesting calls. Similarly, each instrumentation method can register for the FunctionIDMapper  to allow them to disable the callback for a particular method. If no instrumentation method one states that the callback should occur, it is disabled. However, if at least one leaves it enabled, the callback will still occur but will filter out instrumentation methods that do not wish to receive it.

>In typical implementations, is this even possible for function enter / function exit / tailcall? Since these must be naked, it may not be possible for these to be multiplexed as registers will need to be saved, and the callbacks may depend on the value of those registers (example: value sitting in Eax, saved by multiplex implementation). One possible solution is to redefine the interface to not be naked, take more parameters, and and allow the values of the original registers to be obtained by the callbacks. Need to find out how profilers that actually use these calls use them. Intellitrace and production bps don’t currently consume these. MMA does.
Note that the function enter/exit callbacks actually aren’t required as it is possible to achieve the same functionality by instrumenting the prolog and epilog. My current thoughts on this is the ProfilerManager should NOT expose this functionality. The profiler host may choose to pass it to a single implementation however.

For scenarios involving IL Rewriting, the Instrumentation Methods will be given the opportunity to rewrite IL using the IL graph at two different callbacks: right before the CLR jits the method the first time, and after a rejit is request and the method is called again.  The instrumentation methods will be called in priority order first, and will perform their il manipulations using the il graph (see interface description below). Once the instrumentation methods have finished, the il, exception ranges, and localvar sigs will be rendered back to a byte stream and given the snapins to manipulate. Finally, the actual il method body will be given to the clr to finish the rewrite.

## <a name="abstractions">Abstractions</a>

### <a name="dataitems">Data Items</a>
One design point that has proven to be very useful in other diagnostic code base is the concept of data items and data containers. In the Data Container model, most system owned object derives from a hash table base class that implements the IDataContainer interface that holds instances of IDataItem. This allows consumers of the API to extend objects and add data within the object that makes sense.
For example, If Intellitrace wants to extend the module info object, they can do so by placing an extension object into the data item collection for each module. This allows them to extend the object with Intellitrace specific data without having to create their own collection that maps ModuleInfo to ModuleInfoIntellitraceExtension.

Given that the object structure is loosely based on COM, it makes sense to use object guids as the key. I.E. the intellitrace instrumentation method will have a guid, as will the data item object they wish to store. So, the key would be the combination of IntellitraceInstrumentationMethod guid and the IntellitraceModuleItemExtension guid.

``` cpp
IDataContainer : IUnknown
{
    // Set an instance of a data item for a component.
    // This object will be add-ref'd and
    // continue to exist until someone sets
    // it again passing null, or the data container
    // is destroyed
    HRESULT SetDataItem(REFGUID componentId, REFGUID objectGuid, IUknown* pDataItem);

    // return an addref'd pointer to a data item previously
    // stored with SetDataItem.
    // componentId is the guid of the object storing item.
    // (CoClass of instrumentation method);
    // objectGuid is a class guid of the object being stored
    // returns the data item if it exists
    HRESULT GetDataItem(REFGUID componentId, REFGUID objectGuid, IUknown** ppDataItem);
}
```

Sometimes, it is necessary for data items to need to do cleanup when their corresponding data container is destroyed. To enable this, there is an IDataItem interface with an OnClosed method that is called if the data item implements it. It is optional however.

``` cpp
IDataItem : IUnknown
{
    HRESULT OnDataContainerDestroyed();
}
```

**Example:**

If a consumer wants to store custom information about a module, they would do so by creating an object to store the data, and then calling the SetDataItem method of the IDataContainer interface on the IModuleInfo interface. Example Data Item extending IModuleInfo:

``` cpp
class __declspec(uuid("2056974A-2245-4d9c-B225-AB99ADB05EE9"))
MyModuleDataItem : public IDataItem
{
// …
    HRESULT GetMethodDefTokenOfInstrumentationCall() { return _token };
// …
}

HRESULT MyInstrumentationMethod::OnModuleLoad(IModuleInfo* pModuleInfo)
{
    // .... emit metadata, store token

    CComPtr<IUnknown> pMyDataItem = new MyModuleDataItem(token);
    IfFailRet(pModuleInfo->SetDatItem(__uuidof(this), __uuidof(MyModuleDataItem), pMyDataItem));

    // ... other code

    return S_OK;
}

HRESULT MyInstrumentationMethod::OnBeforeJitComplete(IMethodInfo* pMethodInfo)
{
    CComPtr<IModuleInfo> pModuleInfo;
    IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));

    CComPtr<IUnknown> punkMyDataItem;
    IfFailRet(pModuleInfo->GetDatItem(__uuidof(this),
    __uuidof(MyModuleDataItem), &punkMyDataItem));

    MyModuleDataItem* pMyDataItem = (MyModuleDataItem*)punkMyDataItem;

    methodDefTk methdDef;
    pMyDataItem->GetToken(&methdDef);

    return S_OK;
}
```

### <a name="instruction-graph">Instruction Graph</a>

The instruction graph is a high level representation of the instructions in a method. It hangs off the MethodInfo object and is the core of how instrumentation happens in the profiler host model. Consumers of the instruction graph can insert, modify and remove instances of ILInstruction and its derived classes into the graph. The exception ranges also hang off the MethodInfo object and can be updated to reflect the new graph. The Cor IL Map is automatically generated from the modified offsets after the instrumentation methods have executed.

IInstructionGraph  interface contains methods to consume and manipulate the graph.


### <a name="method">Method Info</a>

MethodInfo is an abstraction over a method. It contains the profiler runtime method id, the method token, a pointer the module, an instruction graph, a collection of method args that is used to manipulate the local var sig for the method, a representation of the method header (Fat/Tiny) and references to all of the exception regions in a method.  The IL rewriting events, OnBeforeJitComplete and OnRejit, in the high level api receive an instance of this object.


### <a name="module">Module Info</a>

Module info is an abstraction over a loaded managed module in the target process. It references the AssemblyInfo that contains the module, the containing appdomain, raw interface pointers to metadata import and export, and other helpers to make working with modules and metadata more efficient or easier.

***NOTE:*** Intellitrace’s definition of ModuleInfo has the type system hanging off of it. The purpose of the type system is to allow native code to extract the necessary target application information at each intellitrace instrumentation point in order to store it into the collection plan. My current thoughts on the type system is it is intellitrace specific and should not be exposed in this API.

### <a name="assembly">Assembly Info</a>

AssemblyInfo represents an assembly loaded in an AppDomain which is made up of a collection of modules.

### <a name="appdomain">AppDomain Info</a>

This object was originally a single object in Intellitrace called AppDomainCollection. I think I would rather have an AppDomainInfo object for each Appdomain with the ability so search the appdomain collection.

### <a name="local-var">Local Variables</a>

Local variables in the CLR are described by the LocalVarSig blob which is a compressed description of the local variable slots and their types. The CLR provides helpers to convert from this compressed blob to the corresponding tokens which can use IMetadataImport.

In Intellitrace, Local Variable manipulation is performed using the local variable collection that hangs off of MethodInfo. Unfortunately, it is tightly coupled to the type system which I wasn’t currently planning to carry forward in this api.


## <a name="transformations">Single Ret and Other Automatic Transformations</a>

Almost all instrumentation models perform an il transformation on methods where multiple return points are replaced with a single return everything else jmps to. This is what makes epilog instrumentation possible. If configured to do so, and an instrumentation method returns that it needs to instrument a method, this standard transformation will be made by the ProfilerManager using the instruction graph before any instrumentation methods run. This centralizes one of the more difficult profiler operations while ensuring the transformation is visible in the instruction graph.

Furthermore, almost all instrumented methods are converted to the CLR’s fat header to allow for exception ranges and more code than the tiny header allows for. The ProfilerManager will also perform that transformation before calling the instrumentation methods.

## <a name="metadata">Metadata</a>

In this proposal, metadata is exposed using the standard public metadata apis: IMetaDataImport and IMetaDataEmit. Instances of these are available off the IModuleInfo interface. However, there are helper methods on ModuleInfo that allow for mapping of runtime ids such as MethodId to metadata tokens and / or structures

Today, metadata can only be modified during the module load callback except for local variable signatures.

## <a name="rejit">Rejit</a>

Rejit is a feature of the CLR that allows a profiler to request that a previously jitted function needs to be rejitted so its instrumentation can be adjusted or removed.
ProfilerManager will have an API to allow a profiler to request a rejit on a module and method. After the method is called, the instrumentation methods will each receive the instrumentation callback again. At the beginning of the rejit, the instruction graph reflect the original state of the method.

Once the instrumentation methods have finished their instrumentation, the IL and IL Map will be rendered to bytes and passed to the snapins using the normal rejit methods (GetReJITParameters). Note that like all clr callbacks, the passed clr interfaces will be wrapped by the profiler manager to ensure the updated il stream and il map are exposed. This is especially important in rejit as clients cannot call a second time and receive the updated il stream after it is set.