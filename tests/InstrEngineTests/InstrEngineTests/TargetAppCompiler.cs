// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Emit;
using Microsoft.CodeAnalysis.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Net;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace InstrEngineTests
{
    /// <summary>
    /// Retrieve the target app's source from embedded resource and compile into
    /// debug/release version binary
    /// </summary>
    internal class TargetAppCompiler
    {
#if NETCOREAPP
        private const string EntryPointAssemblyExtension = "dll";
        private const OutputKind EntryPointAssemblyOutputKind = OutputKind.DynamicallyLinkedLibrary;
#else
        private const string EntryPointAssemblyExtension = "exe";
        private const OutputKind EntryPointAssemblyOutputKind = OutputKind.ConsoleApplication;
#endif

        private const string DebugBinarySuffix = "Debug";
        private const string ReleaseBinarySuffic = "Release";

        private const string X86BinarySuffix = "x86";
        private const string X64BinarySuffix = "x64";

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

        internal static void ComplileCSharpTestCode(string path)
        {
            foreach (string prefix in TestAppFilePrefixes)
            {
                string sourceCode = GetEmbeddedFile("InstrEngineTests.EmbeddedResources", prefix + ".cs");
                SourceText sourceText = SourceText.From(sourceCode);

                GenerateExecutable(sourceText, path, prefix, isDebug: true, is64bit: false);
                GenerateExecutable(sourceText, path, prefix, isDebug: true, is64bit: true);
                GenerateExecutable(sourceText, path, prefix, isDebug: false, is64bit: false);
                GenerateExecutable(sourceText, path, prefix, isDebug: false, is64bit: true);
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

        private static void GenerateExecutable(SourceText sourceText, string path, string prefix, bool isDebug, bool is64bit)
        {
            EmitResult result = Compile(sourceText, path, prefix, isDebug, is64bit);
            Assert.IsTrue(result.Success);
            CopyAdditionalFiles(path, prefix, isDebug, is64bit);
        }

        private static EmitResult Compile(SourceText sourceText, string path, string prefix, bool isDebug, bool is64bit)
        {
            CSharpParseOptions parseOptions = CSharpParseOptions.Default
                .WithLanguageVersion(LanguageVersion.CSharp9);

            SyntaxTree syntaxTree = CSharpSyntaxTree.ParseText(sourceText, parseOptions);

            string sharedFrameworkDir = RuntimeEnvironment.GetRuntimeDirectory();
            MetadataReference[] references = new MetadataReference[]
            {
                MetadataReference.CreateFromFile(typeof(object).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(HttpWebRequest).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(SourceText).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(CSharpCompilation).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(ImmutableArray<>).Assembly.Location),
                MetadataReference.CreateFromFile(Path.Combine(sharedFrameworkDir, "netstandard.dll")),
#if NETCOREAPP
                MetadataReference.CreateFromFile(typeof(Console).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(Uri).Assembly.Location),
                MetadataReference.CreateFromFile(Path.Combine(sharedFrameworkDir, "System.Runtime.dll"))
#endif
            };

            CSharpCompilationOptions compilationOptions = new CSharpCompilationOptions(EntryPointAssemblyOutputKind)
                .WithWarningLevel(4)
                .WithOptimizationLevel(isDebug ? OptimizationLevel.Debug : OptimizationLevel.Release)
                .WithPlatform(is64bit ? Platform.X64 : Platform.X86);

            string assemblyName = GetAssemblyName(prefix, isDebug, is64bit);
            CSharpCompilation compilation = CSharpCompilation.Create(assemblyName)
                .WithReferences(references)
                .WithOptions(compilationOptions)
                .AddSyntaxTrees(syntaxTree);

            return compilation.Emit(GetBinaryFullPath(path, prefix, isDebug, is64bit));
        }

        private static void CopyAdditionalFiles(string path, string prefix, bool isDebug, bool is64bit)
        {
#if !NETCOREAPP
            string appConfig = GetEmbeddedFile("InstrEngineTests.EmbeddedResources", prefix + ".config", required: false);
            if (!string.IsNullOrEmpty(appConfig))
            {
                File.WriteAllText(Path.Combine(path, GetAppConfigFileName(prefix, isDebug, is64bit)), appConfig);
            }
#endif
        }

        private static string GetBinaryFullPath(string path, string prefix, bool isDebug, bool is64bit)
        {
            return Path.Combine(path, Path.ChangeExtension(GetFileName(prefix, isDebug, is64bit), EntryPointAssemblyExtension));
        }

        private static string GetAssemblyName(string prefix, bool isDebug, bool is64bit)
        {
            return GetFileName(prefix, isDebug, is64bit);
        }

        private static string GetAppConfigFileName(string prefix, bool isDebug, bool is64bit)
        {
            return Path.ChangeExtension(GetFileName(prefix, isDebug, is64bit), "exe.config");
        }

        private static string GetFileName(string prefix, bool isDebug, bool is64bit)
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

        private static string GetEmbeddedFile(string resourcePath, string path, bool required = true)
        {
            Assert.IsNotNull(path);
            Assert.IsNotNull(resourcePath);

            var fullPath = string.Format(CultureInfo.InvariantCulture, "{0}.{1}", resourcePath, path);
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
