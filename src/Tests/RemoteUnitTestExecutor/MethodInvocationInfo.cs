// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using System;

    [Serializable]
    public class MethodInvocationInfo
    {
        [NonSerialized]
        private object[] _arguments;

        public string MethodName { get; set; }

        public object[] Arguments
        {
            get => _arguments;
            set => _arguments = value;
        }
    }
}