// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using System;

    [Serializable]
    public class MethodInvocationInfo
    {
        public string MethodName { get; set; }

        [NonSerialized] public object[] Arguments;
    }
}