// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace InstrEngineTests
{
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Reflection;
    using ApplicationInsightsCompatibility;
    using Intercept.Tests;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;

    [TestClass]
    public class CustomAttributesTest
    {
        private class CustomAttributesTestBase : TestBase
        {
            public override void Execute()
            {
                var declaredMscorlib = Assembly.ReflectionOnlyLoadFrom((typeof(object)).Assembly.Location);
                var declaredAttributes = CustomAttributeData.GetCustomAttributes(declaredMscorlib).Select(attr => attr.AttributeType.FullName.ToString(CultureInfo.InvariantCulture)).ToArray<string>();
                var runtimeAttributes = CustomAttributeData.GetCustomAttributes((typeof(object)).Assembly).Select(attr => attr.AttributeType.FullName.ToString(CultureInfo.InvariantCulture)).ToArray<string>();

                Assert.AreEqual(declaredAttributes.Length, runtimeAttributes.Length);

                foreach (var a in runtimeAttributes)
                {
                    Assert.IsTrue(declaredAttributes.Contains(a), $"Unexpected attribute {a}");
                }

                HashSet<string> uniqueNames = new HashSet<string>();
                string duplicatedAttributes = "";
                foreach (var a in runtimeAttributes)
                {
                    if (!a.Contains("InternalsVisibleTo"))
                    {
                       if (!uniqueNames.Add(a))
                        {
                            duplicatedAttributes += a + ", ";
                        }
                    }
                }
                Assert.AreEqual(string.Empty, duplicatedAttributes);
            }
        }

        [TestMethod]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineProfilerModuleName32, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineDefaultMethodModuleName32, ".")]
        [DeploymentItem("..\\..\\AnyCPU\\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(".\\Microsoft.AI.Agent.Intercept.dll", Constants.InterceptVersion)]
        [DeploymentItem("Microsoft.VisualStudio.TestPlatform.TestFramework.dll", ".")]
        [DeploymentItem("Microsoft.VisualStudio.TestPlatform.TestFramework.Extensions.dll", ".")]
        public void InstrumentationEngineDoNotAddUnnecessaryCustomAttributes32()
        {
            TestEngine.ExecuteTest<CustomAttributesTestBase>(run32Bit: true);
        }

        [TestMethod]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineProfilerModuleName64, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineDefaultMethodModuleName64, ".")]
        [DeploymentItem("..\\..\\AnyCPU\\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(".\\Microsoft.AI.Agent.Intercept.dll", Constants.InterceptVersion)]
        [DeploymentItem("Microsoft.VisualStudio.TestPlatform.TestFramework.dll", ".")]
        [DeploymentItem("Microsoft.VisualStudio.TestPlatform.TestFramework.Extensions.dll", ".")]
        public void InstrumentationEngineDoNotAddUnnecessaryCustomAttributes64()
        {
            TestEngine.ExecuteTest<CustomAttributesTestBase>(run32Bit: false);
        }
    }
}
