// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Emit;
using Microsoft.CodeAnalysis.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Net;
using System.Reflection;
using System.Runtime.InteropServices;

namespace InstrEngineTests
{
    /// <summary>
    /// Retrieve the target app's source from embedded resource and compile into
    /// debug/release version binary
    /// </summary>
    internal class TargetAppCompiler
    {
        private const string SourceExtension = "cs";
        private const string DynamicallyLinkedLibraryExtension = "dll";
        private const string ExecutableExtension = "exe";

#if NETCOREAPP
        private const string EntryPointAssemblyExtension = DynamicallyLinkedLibraryExtension;
#else
        private const string EntryPointAssemblyExtension = ExecutableExtension;
#endif

        private const string DebugBinarySuffix = "Debug";
        private const string ReleaseBinarySuffic = "Release";

        private const string X86BinarySuffix = "x86";
        private const string X64BinarySuffix = "x64";

        private const string EmbeddedResourcesPath = "InstrEngineTests.EmbeddedResources";

        private static string[] TestAppFilePrefixes = new string[] {
            "AddExceptionHandlerTests",
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
            "DynamicCodeTests",
            "MultiReturnTests"
        };

        private const string DynamicCodeAssemblyName = "DynamicCodeAssembly";

        internal static void ComplileCSharpTestCode(string directoryPath)
        {
            EmitResult result;
            foreach (string prefix in TestAppFilePrefixes)
            {
                string sourceCode = GetEmbeddedSourceFile(prefix);
                SourceText sourceText = SourceText.From(sourceCode);

                result = CompileTestAppPrefix(sourceText, directoryPath, prefix, isDebug: true, is64bit: false);
                Assert.IsTrue(result.Success, result.Diagnostics.Length > 0 ? result.Diagnostics[0].GetMessage() : null);

                result = CompileTestAppPrefix(sourceText, directoryPath, prefix, isDebug: true, is64bit: true);
                Assert.IsTrue(result.Success, result.Diagnostics.Length > 0 ? result.Diagnostics[0].GetMessage() : null);

                result = CompileTestAppPrefix(sourceText, directoryPath, prefix, isDebug: false, is64bit: false);
                Assert.IsTrue(result.Success, result.Diagnostics.Length > 0 ? result.Diagnostics[0].GetMessage() : null);

                result = CompileTestAppPrefix(sourceText, directoryPath, prefix, isDebug: false, is64bit: true);
                Assert.IsTrue(result.Success, result.Diagnostics.Length > 0 ? result.Diagnostics[0].GetMessage() : null);
            }

            SourceText dynamicCodeAssemblyText = SourceText.From(GetEmbeddedSourceFile(DynamicCodeAssemblyName));
            result = CompileAssembly(dynamicCodeAssemblyText, directoryPath, DynamicCodeAssemblyName);
            Assert.IsTrue(result.Success, result.Diagnostics.Length > 0 ? result.Diagnostics[0].GetMessage() : null);
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

        private static EmitResult CompileTestAppPrefix(SourceText sourceText, string path, string prefix, bool isDebug, bool is64bit)
        {
            return CompileAssembly(sourceText, path, GetAssemblyName(prefix, isDebug, is64bit), isDebug, is64bit, OutputKind.ConsoleApplication, EntryPointAssemblyExtension);
        }

        private static EmitResult CompileAssembly(
            SourceText sourceText,
            string directoryPath,
            string assemblyName,
            bool isDebug = false,
            bool? is64bit = null,
            OutputKind outputKind = OutputKind.DynamicallyLinkedLibrary,
            string extension = DynamicallyLinkedLibraryExtension)
        {
            IList<string> preprocessorSymbols = new List<string>();
            preprocessorSymbols.Add("TRACE");
            if (isDebug)
            {
                preprocessorSymbols.Add("DEBUG");
            }

            CSharpParseOptions parseOptions = CSharpParseOptions.Default
                .WithLanguageVersion(LanguageVersion.CSharp9)
                .WithPreprocessorSymbols(preprocessorSymbols);

            SyntaxTree syntaxTree = CSharpSyntaxTree.ParseText(sourceText, parseOptions);

            string sharedFrameworkDir = RuntimeEnvironment.GetRuntimeDirectory();
            MetadataReference[] references = new MetadataReference[]
            {
                MetadataReference.CreateFromFile(typeof(object).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(HttpWebRequest).Assembly.Location),
#if NETCOREAPP
                MetadataReference.CreateFromFile(typeof(Console).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(Uri).Assembly.Location),
                MetadataReference.CreateFromFile(Path.Combine(sharedFrameworkDir, "System.Runtime.dll"))
#endif
            };

            Platform platform = Platform.AnyCpu;
            if (is64bit.HasValue)
            {
                platform = is64bit.Value ? Platform.X64 : Platform.X86;
            }

            CSharpCompilationOptions compilationOptions = new CSharpCompilationOptions(outputKind)
                .WithWarningLevel(4)
                .WithOptimizationLevel(isDebug ? OptimizationLevel.Debug : OptimizationLevel.Release)
                .WithPlatform(platform);

            CSharpCompilation compilation = CSharpCompilation.Create(assemblyName)
                .WithReferences(references)
                .WithOptions(compilationOptions)
                .AddSyntaxTrees(syntaxTree);

            return compilation.Emit(Path.Combine(directoryPath, Path.ChangeExtension(assemblyName, extension)));
        }

        private static string GetBinaryFullPath(string path, string prefix, bool isDebug, bool is64bit)
        {
            return GetBinaryFullPath(path, GetAssemblyName(prefix, isDebug, is64bit));
        }

        private static string GetBinaryFullPath(string path, string assemblyName)
        {
            return Path.Combine(path, Path.ChangeExtension(assemblyName, EntryPointAssemblyExtension));
        }

        private static string GetAssemblyName(string prefix, bool isDebug, bool is64bit)
        {
            Assert.IsFalse(string.IsNullOrEmpty(prefix));

            return string.Format(CultureInfo.InvariantCulture, "{0}_{1}_{2}", prefix, isDebug ? DebugBinarySuffix : ReleaseBinarySuffic, is64bit ? X64BinarySuffix : X86BinarySuffix);
        }

        private static string GetCompilerOptions(bool isDebug, bool is64bit)
        {
            string platform = is64bit ? "x64" : "x86";
            string defines = isDebug ? "DEBUG;TRACE" : "TRACE";
            string debug = isDebug ? "full" : "pdbonly";
            string optimize = isDebug ? "-" : "+";

            return $"/platform:{platform} /define:{defines} /debug:{debug} /optimize{optimize}";
        }

        private static string GetEmbeddedSourceFile(string file, bool required = true)
        {
            Assert.IsNotNull(file);

            var fullPath = string.Format(CultureInfo.InvariantCulture, "{0}.{1}.{2}", EmbeddedResourcesPath, file, SourceExtension);
            var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(fullPath);

            if (required)
            {
                Assert.IsNotNull(stream, "Could not find embedded resource {0}", fullPath);
            }

            if (null == stream)
            {
                return null;
            }

            using (StreamReader reader = new StreamReader(stream))
            {
                return reader.ReadToEnd();
            }
        }
    }
}
