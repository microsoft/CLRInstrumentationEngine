// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InstrEngineTests
{
    internal class TestAppAssembler
    {
        internal static bool AssembleFile(IEmbeddedResourceFile embeddedResource, string directoryPath, string assemblyName, bool isDebug, bool is64Bit, bool isExe = false)
        {
            ProcessStartInfo psi = new ProcessStartInfo();
            IEmbeddedResourceFile ilasm = EmbeddedResourceUtils.GetTestResourceFile("ilasm.exe");
            Assert.IsFalse(ilasm == null, "Could not find ilasm.exe");
            if (ilasm == null)
            {
                return false;
            }

            psi.FileName = ilasm.FilePath;
            psi.RedirectStandardError = true;
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            string debugSwitch = isDebug ? "/DEBUG=OPT" : string.Empty;
            string bitSwitch = is64Bit ? "/X64" : "/32BITPREFERRED";
            if (TestUtils.IsArmProcess)
            {
                bitSwitch = is64Bit ? "/ARM64" : "/ARM";
            }
            string outputType = isExe ? "/EXE" : "/DLL";
            string extension = isExe ? "exe" : "dll";
            string outputFile = Path.Combine(directoryPath, FormattableString.Invariant($"{assemblyName}.{extension}"));
            psi.Arguments = FormattableString.Invariant($"{debugSwitch} {bitSwitch} {outputType} /OUTPUT=\"{outputFile}\" \"{embeddedResource.FilePath}\"");
            Process p = Process.Start(psi);
            p.WaitForExit();
            string stdOut = p.StandardOutput.ReadToEnd();
            string stdErr = p.StandardError.ReadToEnd();
            Assert.IsTrue(p.ExitCode == 0, FormattableString.Invariant($"Assembly failed for '{embeddedResource.FilePath}' : {stdOut} \n\n {stdErr}"));
            return p.ExitCode == 0;
        }

        internal static string AssembleFiles(List<IEmbeddedResourceFile> embeddedResources, string directoryPath, string assemblyName, bool isDebug, bool is64Bit, bool isExe = false)
        {
            ProcessStartInfo psi = new ProcessStartInfo();
            IEmbeddedResourceFile ilasm = EmbeddedResourceUtils.GetTestResourceFile("ilasm.exe");
            Assert.IsFalse(ilasm == null, "Could not find ilasm.exe");
            if (ilasm == null)
            {
                return null;
            }

            psi.FileName = ilasm.FilePath;
            psi.RedirectStandardError = true;
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            string debugSwitch = isDebug ? "/DEBUG=OPT" : string.Empty;
            string bitSwitch = is64Bit ? "/X64" : "/32BITPREFERRED";
            if (TestUtils.IsArmProcess)
            {
                bitSwitch = is64Bit ? "/ARM64" : "/ARM";
            }
            string outputType = isExe ? "/EXE" : "/DLL";
            string extension = isExe ? "exe" : "dll";
            string outputFile = Path.Combine(directoryPath, FormattableString.Invariant($"{assemblyName}.{extension}"));

            string[] embeddedResourceFilePaths = embeddedResources.Select(x => x.FilePath).ToArray();
            string ilFileNames = string.Join("\" \"", embeddedResourceFilePaths);

            psi.Arguments = FormattableString.Invariant($"{debugSwitch} {bitSwitch} {outputType} /OUTPUT=\"{outputFile}\" \"{ilFileNames}\"");
            Process p = Process.Start(psi);
            p.WaitForExit(5000);
            string stdOut = p.StandardOutput.ReadToEnd();
            string stdErr = p.StandardError.ReadToEnd();
            Assert.IsTrue(p.ExitCode == 0, FormattableString.Invariant($"Assembly failed for '{ilFileNames}' : {stdOut} \n\n {stdErr}"));
            if (p.ExitCode == 0)
            {
                return outputFile;
            }

            return null;
        }
    }
}
