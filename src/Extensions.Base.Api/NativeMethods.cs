// Copyright (c) Microsoft Corporation. All rights reserved.
//

namespace _System.Diagnostics
{
    using Microsoft.Diagnostics.Instrumentation.Extensions.Base;
    using System;

    /// <summary>
    /// This class defines public contract to register new callbacks
    /// </summary>
    public partial class DebuggerHiddenAttribute : Attribute
    {
        public static void ApplicationInsights_Attach(long modulePath, long name, long description, long module, long classGuid, int priority)
        {
            //this method body will be replaced with the call to the correponding native method
            return;
        }
    }
}