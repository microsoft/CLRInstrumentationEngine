// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace InstrEngineTests
{
    using System.Collections.Generic;
    using System.Linq;
    using System.Reflection;
    using ApplicationInsightsCompatibility;
    using Intercept.Tests;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;

    [TestClass]
    public class CustomAttributesTest
    {
        public class CustomAttributesTestBase : TestBase
        {
            public override void Execute()
            {
                var declaredMscorlib = Assembly.ReflectionOnlyLoadFrom((typeof(object)).Assembly.Location);
                var declaredAttributes = CustomAttributeData.GetCustomAttributes(declaredMscorlib).Select(attr => attr.AttributeType.FullName.ToString()).ToArray<string>();
                var runtimeAttributes = CustomAttributeData.GetCustomAttributes((typeof(object)).Assembly).Select(attr => attr.AttributeType.FullName.ToString()).ToArray<string>();

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
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineProfilerModuleName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineDefaultMethodModuleName, ".")]
        [DeploymentItem("..\\..\\AnyCPU\\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(".\\Microsoft.AI.Agent.Intercept.dll", Constants.InterceptVersion)]
        [DeploymentItem("Microsoft.VisualStudio.QualityTools.UnitTestFramework.dll", ".")]
        public void InstrumentationEngineDoNotAddUnnecessaryCustomAttributes()
        {
            TestEngine.ExecuteTest<CustomAttributesTestBase>();
        }
    }
}
