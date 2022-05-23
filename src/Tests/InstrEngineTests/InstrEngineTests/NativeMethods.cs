// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Globalization;
using System.Runtime.InteropServices;

namespace InstrEngineTests
{
    internal class NativeMethods
    {

        // Disable warning because kernel32.dll has special loading logic in the kernel
#pragma warning disable CA5392 // Use DefaultDllImportSearchPaths attribute for P/Invokes
        [DllImport("kernel32.dll")]
        public static extern int GetComPlusPackageInstallStatus();

        [DllImport("kernel32.dll")]
        public static extern bool SetComPlusPackageInstallStatus(int flag);

#pragma warning restore CA5392 // Use DefaultDllImportSearchPaths attribute for P/Invokes
    }
}
