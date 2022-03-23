// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CompiledCode
{
    static public class CallForwarder
    {
        public static void ForwardCallCompiled(Action a)
        {
            a();
        }
    }
}