// Copyright (c) Microsoft Corporation. All rights reserved.
// 

namespace RawProfilerHook.Tests
{
    using System.Diagnostics;

    internal static class ProcessExtensions
    {
        internal static bool IsModuleLoaded(this Process process, string moduleNameMarker)
        {
            var moduleFound = false;
            foreach (ProcessModule module in process.Modules)
            {
                moduleFound = module.FileName.Contains(moduleNameMarker);
                if (true == moduleFound)
                {
                    break;
                }
            }

            return moduleFound;
        }
    }
}
