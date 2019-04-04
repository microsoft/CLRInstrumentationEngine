# Configuring the CLR Instrumentation Engine

See [Environment Variables](environment_variables.md) for details on how to enable the Instrumentation Engine.

The CLR Instrumentation Engine follows a declarative model where profiler extension owners declare their existence through environment variables. These environment variables tell the Engine the paths to *.config, which provide the metadata and relative path to your Instrumentation Method implementation (dll).

### Config File

The format of the *.config file is:
```
<InstrumentationEngineConfiguration>
    <InstrumentationMethod>
        <Name>Squid Instrumentation</Name>
        <Description>Dynamically make squids swim</Description>
        <Module>SeafoodInstrumentation.dll</Module>
        <ClassGuid>{12341234-1234-1234-1234-123412341234}</ClassGuid>
        <Priority>50</Priority>
    </InstrumentationMethod>
</InstrumentationEngineConfiguration>
```

| Variable | Description |
|-|-|
Name|The name of the instrumentation method.
Description|A short description of its application.
Module|The **relative path from the config file** to the implementation dll.
ClassGuid|The CLSID of the implementation module.
Priority|Integer ordering for InstrumentationMethod consumption. Higher priority applies earlier.

### Config Environment Variable

The CLR Instrumentation Engine checks for environment variables with these prefixes:
* 64bit: `MicrosoftInstrumentationEngine_ConfigPath64_`
* 32bit: `MicrosoftInstrumentationEngine_ConfigPath32_`

The value of these variables should be a **full path** to the *.config file.

### Testing Custom Instrumentation Method

In order to test your InstrumentationMethod, make sure to disable Signature Validation by setting the following environment variable and value.

`MicrosoftInstrumentationEngine_DisableCodeSignatureValidation = 1`

The Instrumentation Engine leverages WinTrust libraries to validate code signatures. See CSignatureValidator for in-depth details.