// Copyright (c) Microsoft Corporation. All rights reserved.
//

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ProfilerCallbacksTests
{
    [TestClass]
    public class ExtensionsBaseLimitations
    {
        [TestMethod]
        public void ExtensionsBaseDoNotHaveReferences()
        {
            var extAsm = typeof(_System.Diagnostics.DebuggerHiddenAttribute).Assembly;
            foreach (var asm in extAsm.GetReferencedAssemblies())
            {
                Assert.AreEqual("mscorlib", asm.Name);
            }
            Assert.AreEqual(extAsm.GetReferencedAssemblies().Length, 1);
        }

        [TestMethod]
        public void ExtensionsBaseHasNoPublicClassesExceptInterface()
        {
            var extModule = typeof(_System.Diagnostics.DebuggerHiddenAttribute).Module;
            foreach (var type in extModule.GetTypes())
            {
                if (type.IsPublic)
                {
                    Assert.AreEqual("_System.Diagnostics.DebuggerHiddenAttribute", type.FullName);
                }
            }
        }

        [TestMethod]
        public void ExtensionsBaseHasNoStaticFieldInNonGenericClasses()
        {
            var extModule = typeof(_System.Diagnostics.DebuggerHiddenAttribute).Module;
            foreach (var type in extModule.GetTypes())
            {
                if (!type.IsGenericType)
                {
                    foreach (var field in type.GetFields())
                    {
                        Assert.IsTrue(field.IsStatic == false, "Type: " + type.FullName + " should not have static field " + field.Name);
                    }
                }
            }

        }

    }
}
