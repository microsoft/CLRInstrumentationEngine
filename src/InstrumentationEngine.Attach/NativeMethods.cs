// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;

namespace Microsoft.InstrumentationEngine
{
    internal static class NativeMethods
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Security", "CA5392:Use DefaultDllImportSearchPaths attribute for P/Invokes", Justification = "Function exists in kernel32.dll, which has special loading logic controlled by the kernel.")]
        public static extern bool IsWow64Process([In] IntPtr hProcess, [Out] out bool wow64Process);
    }
}
