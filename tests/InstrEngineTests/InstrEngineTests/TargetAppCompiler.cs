// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.CodeDom.Compiler;
using System.Globalization;
using System.IO;
using System.Reflection;

namespace InstrEngineTests
{
    /// <summary>
    /// Retrieve the target app's source from embedded resource and compile into
    /// debug/release version binary
    /// </summary>
    internal class TargetAppCompiler
    {
        private const string DebugBinarySuffix = "Debug";
        private const string ReleaseBinarySuffic = "Release";

        private const string X86BinarySuffix = "x86";
        private const string X64BinarySuffix = "x64";

        private static string[] TestAppFilePrefixes = new string[] {
            "InjectToMscorlibTest",
            "BasicManagedTests",
            "BasicManagedTests2",
            "ExceptionTests",
            "InstruOperationsTests",
            "InstrumentationMethodLoggingTests",
            "MethodReplacementTests",
            "LocalTests",
            "RuntimeExceptionCallbacks",
            "HttpMethodTests",
            "DynamicCodeTests" };

        internal static void ComplileCSharpTestCode(string path)
        {
            foreach (string prefix in TestAppFilePrefixes)
            {
                using (CodeDomProvider codeProvider = CodeDomProvider.CreateProvider("CSharp"))
                {

                    string sourceCode = GetEmbeddedSourceFile(Assembly.GetExecutingAssembly(), "InstrEngineTests.EmbeddedResources", prefix + ".cs");

                    CompilerParameters debugX86Parameters = CreateCompilerParameters(path, prefix, isDebug: true, is64bit: false);
                    CompilerResults results = codeProvider.CompileAssemblyFromSource(debugX86Parameters, sourceCode);
                    Assert.IsFalse(results.Errors.HasErrors, results.Errors.Count > 0 ? "Compile error:" + results.Errors[0].ErrorText : null);

                    CompilerParameters debugX64Parameters = CreateCompilerParameters(path, prefix, isDebug: true, is64bit: true);
                    results = codeProvider.CompileAssemblyFromSource(debugX64Parameters, sourceCode);
                    Assert.IsFalse(results.Errors.HasErrors, results.Errors.Count > 0 ? "Compile error:" + results.Errors[0].ErrorText : null);

                    CompilerParameters releaseX86Parameters = CreateCompilerParameters(path, prefix, isDebug: false, is64bit: false);
                    results = codeProvider.CompileAssemblyFromSource(releaseX86Parameters, sourceCode);
                    Assert.IsFalse(results.Errors.HasErrors, results.Errors.Count > 0 ? "Compile error:" + results.Errors[0].ErrorText : null);

                    CompilerParameters releaseX64Parameters = CreateCompilerParameters(path, prefix, isDebug: false, is64bit: true);
                    results = codeProvider.CompileAssemblyFromSource(releaseX64Parameters, sourceCode);
                    Assert.IsFalse(results.Errors.HasErrors, results.Errors.Count > 0 ? "Compile error:" + results.Errors[0].ErrorText : null);
                }
            }
        }

        internal static void DeleteExistingBinary(string path)
        {
            foreach (string prefix in TestAppFilePrefixes)
            {
                string debugX86BinaryPath = GetBinaryFullPath(path, prefix, isDebug: true, is64bit: false);
                if (File.Exists(debugX86BinaryPath))
                {
                    File.Delete(debugX86BinaryPath);
                }

                string debugX64BinaryPath = GetBinaryFullPath(path, prefix, isDebug: true, is64bit: true);
                if (File.Exists(debugX64BinaryPath))
                {
                    File.Delete(debugX64BinaryPath);
                }

                string releaseX86BinaryPath = GetBinaryFullPath(path, prefix, isDebug: false, is64bit: false);
                if (File.Exists(releaseX86BinaryPath))
                {
                    File.Delete(releaseX86BinaryPath);
                }

                string releaseX64BinaryPath = GetBinaryFullPath(path, prefix, isDebug: false, is64bit: true);
                if (File.Exists(releaseX64BinaryPath))
                {
                    File.Delete(releaseX64BinaryPath);
                }
            }
        }

        private static CompilerParameters CreateCompilerParameters(string path, string prefix, bool isDebug, bool is64bit)
        {
            CompilerParameters parameters = new CompilerParameters();
            parameters.GenerateExecutable = true;
            parameters.WarningLevel = 4;
            parameters.IncludeDebugInformation = true;
            parameters.ReferencedAssemblies.Add("System.dll");
            parameters.OutputAssembly = GetBinaryFullPath(path, prefix, isDebug, is64bit);
            parameters.CompilerOptions = GetCompilerOptions(isDebug, is64bit);
            return parameters;
        }

        internal static string GetBinaryFullPath(string path, string prefix, bool isDebug, bool is64bit)
        {
            Assert.IsFalse(string.IsNullOrEmpty(prefix));

            string fullName = string.Format(CultureInfo.InvariantCulture, "{0}_{1}_{2}.exe", prefix, isDebug ? DebugBinarySuffix : ReleaseBinarySuffic, is64bit ? X64BinarySuffix : X86BinarySuffix);
            return Path.Combine(path, fullName);
        }

        private static string GetCompilerOptions(bool isDebug, bool is64bit)
        {
            string platform = is64bit ? "x64" : "x86";
            string defines = isDebug ? "DEBUG;TRACE" : "TRACE";
            string debug = isDebug ? "full" : "pdbonly";
            string optimize = isDebug ? "-" : "+";

            return $"/platform:{platform} /define:{defines} /debug:{debug} /optimize{optimize}";
        }

        private static string GetEmbeddedSourceFile(Assembly assembly, string resourcePath, string path)
        {
            Assert.IsNotNull(path);
            Assert.IsNotNull(resourcePath);

            var fullPath = string.Format(CultureInfo.InvariantCulture, "{0}.{1}", resourcePath, path);
            var stream = assembly.GetManifestResourceStream(fullPath);

            Assert.IsNotNull(stream, "Could not find embedded resource {0}", fullPath);
            using (StreamReader reader = new StreamReader(stream))
            {
                return reader.ReadToEnd();
            }
        }
    }
}
