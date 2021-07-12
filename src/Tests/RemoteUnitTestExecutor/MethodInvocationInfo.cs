// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace RemoteUnitTestExecutor
{
    [Serializable]
    public class MethodInvocationInfo
    {
        [NonSerialized]
        public object[] Arguments;

        public string MethodName { get; set; }
    }
}
