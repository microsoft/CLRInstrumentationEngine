using System;
using System.Runtime.InteropServices;

namespace Microsoft.InstrumentationEngine
{
    internal static class NativeMethods
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool IsWow64Process([In] IntPtr hProcess, [Out] out bool wow64Process);
    }
}
