// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using System;
    using System.Collections.Generic;

    [Serializable]
    public class MethodInvocationInfo
    {
        [NonSerialized]
        private List<object> _arguments = new List<object>();

        public string MethodName { get; set; }

        public IEnumerable<object> Arguments => _arguments;

        public void AddArgument(object value)
        {
            _arguments.Add(value);
        }
    }
}