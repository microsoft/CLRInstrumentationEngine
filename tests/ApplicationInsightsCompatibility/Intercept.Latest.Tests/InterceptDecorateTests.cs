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
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule)]
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule32)]
        public void InstrumentationEngineDoNotAddUnnecessaryCustomAttributes32()
        {
            TestEngine.ExecuteTest<CustomAttributesTestBase>(run32Bit: true);
        }

        [TestMethod]
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule)]
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule64)]
        public void InstrumentationEngineDoNotAddUnnecessaryCustomAttributes64()
        {
            TestEngine.ExecuteTest<CustomAttributesTestBase>(run32Bit: false);
        }
    }
}
