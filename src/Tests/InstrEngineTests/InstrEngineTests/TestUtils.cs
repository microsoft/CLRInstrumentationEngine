// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace InstrEngineTests
{
    internal class TestUtils
    {
        public static bool IsArmProcess
        {
            get
            {
                switch (RuntimeInformation.ProcessArchitecture)
                {
                    case Architecture.Arm:
                    case Architecture.Arm64:
                        return true;
                    case Architecture.X86:
                    case Architecture.X64:
                        return false;
                    default:
                        Assert.Fail($"Unsupported Architecture {RuntimeInformation.ProcessArchitecture}");
                        return false;
                }
            }
        }
    }
}
