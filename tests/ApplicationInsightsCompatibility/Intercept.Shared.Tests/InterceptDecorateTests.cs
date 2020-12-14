// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace InstrEngineTests
{
    using System;
    using ApplicationInsightsCompatibility;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;
    using Microsoft.Diagnostics.Instrumentation.Extensions.Intercept;
    using System.Net;
    using Intercept.Shared.Tests;
    using System.Diagnostics;
    using Intercept.Tests;

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
                TestResult.InvokedMethods.Add(new MethodInvocationInfo
                    {
                        MethodName = "OnEnd",
                        Arguments = new[] { context, retValue, thisObj }
                    });

                return retValue;
            }

            public void OnException(object context, Exception exc, object thisObj)
            {
                TestResult.InvokedMethods.Add(new MethodInvocationInfo
                    {
                        MethodName = "OnException",
                        Arguments = new[] { context, exc, thisObj }
                    });
            }
        }

        [TestMethod]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineProfilerModuleName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineDefaultMethodModuleName, ".")]
        [DeploymentItem("..\\..\\AnyCPU\\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(".\\Microsoft.AI.Agent.Intercept.dll", Constants.InterceptVersion)]
        [DeploymentItem("Microsoft.VisualStudio.QualityTools.UnitTestFramework.dll", ".")]
        public void InterceptCallsBeginEndOnExceptionCallbacksOfHttpGetResponse()
        {
            TestEngine.ExecuteTest<SimpleDecorate>();
        }
    }
}
