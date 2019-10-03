﻿


// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//==============================================
//This file is autogenerated
//==============================================

namespace Microsoft.Diagnostics.Instrumentation.Extensions.Base
{
    using System;
    using System.Collections.Generic;
    /// <summary>
    /// This type if generic to avoid problems merging it to mscorlib
    /// </summary>
    /// <typeparam name="T">This generic attribute has no purpose</typeparam>

    internal class BCB<T>
    {
            public static int Lock0;
            public static int Lock1;
            public static int Lock2;
            public static int Lock3;
            public static int Lock4;
            public static int Lock5;
            public static int Lock6;

            internal static Dictionary<int, Func<object>>
                callbacks0 = new Dictionary<int, Func<object>>();
            internal static Dictionary<int, Func<object, object>>
                callbacks1 = new Dictionary<int, Func<object, object>>();
            internal static Dictionary<int, Func<object, object, object>>
                callbacks2 = new Dictionary<int, Func<object, object, object>>();
            internal static Dictionary<int, Func<object, object, object, object>>
                callbacks3 = new Dictionary<int, Func<object, object, object, object>>();
            internal static Dictionary<int, Func<object, object, object, object, object>>
                callbacks4 = new Dictionary<int, Func<object, object, object, object, object>>();
            internal static Dictionary<int, Func<object, object, object, object, object, object>>
                callbacks5 = new Dictionary<int, Func<object, object, object, object, object, object>>();
            internal static Dictionary<int, Func<object, object, object, object, object, object, object>>
                callbacks6 = new Dictionary<int, Func<object, object, object, object, object, object, object>>();
    }

}

//Underscore _System assembly will be treated as regular System namespace by ImportModule function of Intstrumentation Engine.
//This is done to avoid conflicts during compilation
namespace _System.Diagnostics
{
    using Microsoft.Diagnostics.Instrumentation.Extensions.Base;
    using System;

    /// <summary>
    /// This class defines public contract to register new callbacks
    /// </summary>
    public partial class DebuggerHiddenAttribute : Attribute
    {

        public static object ApplicationInsights_OnBegin(int methodId)
        {
            Func<object> func = null;
            if (BCB<int>.callbacks0.TryGetValue(methodId, out func) && (func != null))
                return func();
            else
                return null;
        }
        public static object ApplicationInsights_OnBegin(int methodId, object arg0)
        {
            Func<object, object> func = null;
            if (BCB<int>.callbacks1.TryGetValue(methodId, out func) && (func != null))
                return func(arg0);
            else
                return null;
        }
        public static object ApplicationInsights_OnBegin(int methodId, object arg0, object arg1)
        {
            Func<object, object, object> func = null;
            if (BCB<int>.callbacks2.TryGetValue(methodId, out func) && (func != null))
                return func(arg0, arg1);
            else
                return null;
        }
        public static object ApplicationInsights_OnBegin(int methodId, object arg0, object arg1, object arg2)
        {
            Func<object, object, object, object> func = null;
            if (BCB<int>.callbacks3.TryGetValue(methodId, out func) && (func != null))
                return func(arg0, arg1, arg2);
            else
                return null;
        }
        public static object ApplicationInsights_OnBegin(int methodId, object arg0, object arg1, object arg2, object arg3)
        {
            Func<object, object, object, object, object> func = null;
            if (BCB<int>.callbacks4.TryGetValue(methodId, out func) && (func != null))
                return func(arg0, arg1, arg2, arg3);
            else
                return null;
        }
        public static object ApplicationInsights_OnBegin(int methodId, object arg0, object arg1, object arg2, object arg3, object arg4)
        {
            Func<object, object, object, object, object, object> func = null;
            if (BCB<int>.callbacks5.TryGetValue(methodId, out func) && (func != null))
                return func(arg0, arg1, arg2, arg3, arg4);
            else
                return null;
        }
        public static object ApplicationInsights_OnBegin(int methodId, object arg0, object arg1, object arg2, object arg3, object arg4, object arg5)
        {
            Func<object, object, object, object, object, object, object> func = null;
            if (BCB<int>.callbacks6.TryGetValue(methodId, out func) && (func != null))
                return func(arg0, arg1, arg2, arg3, arg4, arg5);
            else
                return null;
        }
    }
}

