// Copyright (c) Microsoft Corporation. All rights reserved.
// 

using System.Runtime.InteropServices;

namespace InstrEngineTests
{
    internal class NativeMethods
    {
        [DllImport("kernel32.dll")]
        public static extern int GetComPlusPackageInstallStatus();

        [DllImport("kernel32.dll")]
        public static extern int SetComPlusPackageInstallStatus(int flag);
    }
}
