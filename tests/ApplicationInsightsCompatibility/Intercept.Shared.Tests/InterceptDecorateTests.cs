// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Intercept.Tests
{
    using Microsoft.Diagnostics.Instrumentation.Extensions.Intercept;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;
    using System;
    using System.Diagnostics;
    using System.Net;

    [TestClass]
    public class InterceptDecorateTests
    {
        private class SimpleDecorate : TestBase
        {
            public override void TestPreInitialize()
            {
                Trace.WriteLine("Preinitialized executed");
                InterceptAssemblyRedirector.EnsureInitialize();
            }

            public override void TestInitialize()
            {
                Decorator.InitializeExtension(Environment.CurrentDirectory);
#pragma warning disable CS0612 // Type or member is obsolete
                Decorator.Decorate(
                    "System",
                    "System.dll",
                    "System.Net.HttpWebRequest.GetResponse",
                    0,
                    this.OnBegin,
                    this.OnEnd,
                    this.OnException);
#pragma warning restore CS0612
            }

            public override void Execute()
            {
                // Calls the method WebRequest.GetResponse which was decorated
                WebRequest.Create(new Uri("http://bing.com")).GetResponse();

                // Make sure begin and end methods are invoked
                AssertExtensions.InvokedMethodsAreEqual(this.TestResult.InvokedMethods, new[] { "OnBegin", "OnEnd" });

                AssertExtensions.ClearInvokedMethods(this.TestResult.InvokedMethods);

                try
                {
                    // Calling method to fail
                    WebRequest.Create(new Uri("http://SomeRandomBadUri.com")).GetResponse();
                }
                catch
                {
                }

                // Make sure begin and end methods are invoked
                AssertExtensions.InvokedMethodsAreEqual(this.TestResult.InvokedMethods,new[] { "OnBegin", "OnException" });

            }

            private object OnBegin(object thisObj)
            {
                this.TestResult.InvokedMethods.Add(new MethodInvocationInfo { MethodName = "OnBegin" });
                return null;
            }

            public object OnEnd(object context, object retValue, object thisObj)
            {
                MethodInvocationInfo info = new MethodInvocationInfo { MethodName = "OnEnd" };
                info.AddArgument(context);
                info.AddArgument(retValue);
                info.AddArgument(thisObj);
                TestResult.InvokedMethods.Add(info);

                return retValue;
            }

            public void OnException(object context, Exception exc, object thisObj)
            {
                MethodInvocationInfo info = new MethodInvocationInfo { MethodName = "OnException" };
                info.AddArgument(context);
                info.AddArgument(exc);
                info.AddArgument(thisObj);
                TestResult.InvokedMethods.Add(info);
            }
        }

        [TestMethod]
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_AgentInterceptModule, Constants.InterceptVersion)]
        public void InterceptCallsBeginEndOnExceptionCallbacksOfHttpGetResponse32()
        {
            TestEngine.ExecuteTest<SimpleDecorate>(run32Bit: true);
        }

        [TestMethod]
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_AgentInterceptModule, Constants.InterceptVersion)]
        public void InterceptCallsBeginEndOnExceptionCallbacksOfHttpGetResponse64()
        {
            TestEngine.ExecuteTest<SimpleDecorate>(run32Bit: false);
        }
    }
}
