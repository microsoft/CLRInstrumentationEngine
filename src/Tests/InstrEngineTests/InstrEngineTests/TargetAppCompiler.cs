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
using System.Linq;
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
        private const string DynamicallyLinkedLibraryExtension = "dll";

#if NETCOREAPP
        private const string EntryPointAssemblyExtension = DynamicallyLinkedLibraryExtension;
#else
        private const string ExecutableExtension = "exe";
        private const string EntryPointAssemblyExtension = ExecutableExtension;
#endif

        private const string DebugBinarySuffix = "Debug";
        private const string ReleaseBinarySuffic = "Release";

        private const string X86BinarySuffix = "x86";
        private const string X64BinarySuffix = "x64";

        private static string[] EmbeddedCSharpPrefixes = new string[] {
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
            "MultiReturnTests",
            "MaxStackSizeExample"
        };

        private static string[] EmbeddedILPrefixes = new string[]
        {
            "ByRefLikeInvalidCSharp",
            "RefFieldInvalidCSharp"
        };

        private const string DynamicCodeAssemblyName = "DynamicCodeAssembly";

        private static readonly bool[] DebugChoice = { true, false };
        private static readonly bool[] Is64BitChoice = { true, false };

        internal static void CompileTestCode(string directoryPath)
        {
            CompileCSharpTestCode(directoryPath);
            AssembleILTestCode(directoryPath);
        }

        private static void CompileCSharpTestCode(string directoryPath)
        {
            EmitResult result;
            foreach (string prefix in EmbeddedCSharpPrefixes)
            {
                string sourceCode = EmbeddedResourceUtils.ReadEmbeddedResourceFile(FormattableString.Invariant($"{prefix}.cs"));
                SourceText sourceText = SourceText.From(sourceCode);
                foreach (bool isDebug in DebugChoice)
                {
                    foreach (bool is64Bit in Is64BitChoice)
                    {
                        result = CompileTestAppPrefix(sourceText, directoryPath, prefix, isDebug, is64Bit);
                        Assert.IsTrue(result.Success, result.Diagnostics.Length > 0 ? result.Diagnostics[0].GetMessage() : null);
                    }
                }
            }

            SourceText dynamicCodeAssemblyText = SourceText.From(EmbeddedResourceUtils.ReadEmbeddedResourceFile(FormattableString.Invariant($"{DynamicCodeAssemblyName}.cs")));
            result = CompileAssembly(dynamicCodeAssemblyText, directoryPath, DynamicCodeAssemblyName);
            Assert.IsTrue(result.Success, result.Diagnostics.Length > 0 ? result.Diagnostics[0].GetMessage() : null);
        }

        private static void AssembleILTestCode(string directoryPath)
        {
            foreach (string prefix in EmbeddedILPrefixes)
            {
                IEmbeddedResourceFile embeddedResource = EmbeddedResourceUtils.GetTestResourceFile(FormattableString.Invariant($"{prefix}.il"));
                foreach (bool isDebug in DebugChoice)
                {
                    foreach (bool is64Bit in Is64BitChoice)
                    {
                        Assert.IsTrue(TestAppAssembler.AssembleFile(embeddedResource, directoryPath, GetAssemblyName(prefix, isDebug, is64Bit), isDebug, is64Bit));
                    }
                }
            }
        }

        internal static void DeleteExistingBinary(string path)
        {
            foreach (string prefix in EmbeddedCSharpPrefixes.Union(EmbeddedILPrefixes))
            {
                foreach (bool isDebug in DebugChoice)
                {
                    foreach (bool is64Bit in Is64BitChoice)
                    {
                        string filePath = GetBinaryFullPath(path, prefix, isDebug, is64Bit);
                        if (File.Exists(filePath))
                        {
                            File.Delete(filePath);
                        }
                    }
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

            // Attempt to enable /debug+ to allow profiler friendly optimizations
            MethodInfo withDebugPlusModeMethod = typeof(CSharpCompilationOptions)
                .GetMethod("WithDebugPlusMode", BindingFlags.NonPublic | BindingFlags.Instance);
            Assert.IsNotNull(withDebugPlusModeMethod);
            compilationOptions = (CSharpCompilationOptions)withDebugPlusModeMethod.Invoke(
                compilationOptions,
                new object[] { true });

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
    }
}
