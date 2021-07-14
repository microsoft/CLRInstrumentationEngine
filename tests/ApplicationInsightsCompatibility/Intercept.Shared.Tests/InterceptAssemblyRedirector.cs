// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Diagnostics.Instrumentation.Extensions.Intercept;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace Intercept.Tests
{
    internal static class InterceptAssemblyRedirector
    {
        private static bool initialized = false;

        static InterceptAssemblyRedirector()
        {
            AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(CurrentDomain_AssemblyResolve);
        }

        public static void EnsureInitialize()
        {
            if (initialized)
                return;

            try
            {
                TryLoadIntercept();
                initialized = true;
            }
            catch (FileLoadException)
            {
            }
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static void TryLoadIntercept()
        {
            Decorator.IsHostEnabled();
        }

        public static Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            var requestedAssembly = new AssemblyName(args.Name);

            if (!requestedAssembly.Name.Contains("Intercept"))
                return null;

            string path = Path.Combine(Directory.GetCurrentDirectory(), requestedAssembly.Version.ToString(), "Microsoft.AI.Agent.Intercept.dll");
            Assert.IsTrue(File.Exists(path), "Test set up is incorrect. Make sure you copied Microsoft.AI.Agent.Intercept.dll to the versioned folder to avoid versions conflict.");

            return Assembly.LoadFrom(path);
        }
    }
}
