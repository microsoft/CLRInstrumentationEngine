// Copyright (c) Microsoft Corporation. All rights reserved.
//

using System;
using System.Collections.Generic;
using System.Text;

namespace InstruOperationsTests
{
    public class Program
    {
        static int Main(string[] args)
        {
            return InjectToMscorlib("0");
        }

        private static int InjectToMscorlib(string value)
        {
            //this will be replaced by new method
            return 42;
        }
    }
}

namespace System.Diagnostics.Instrumentation
{

    public class A<T>
    {
        static List<int> values = new List<int>();

        public void Add(int value)
        {
            values.Add(value);
        }

        public int GetSumValue()
        {
            int result = 0;
            foreach(int i in values)
            {
                result += i;
            }
            return result;
        }

    }
}

namespace _System.Diagnostics
{

    public class DebuggerHiddenAttribute : Attribute
    {
        public static int MethodToCallInstead(string value)
        {
            System.Diagnostics.Instrumentation.A<string> classA = new System.Diagnostics.Instrumentation.A<string>();
            classA.Add(42);
            classA.Add(-42);
            return classA.GetSumValue();
        }

        public void NoLocalVariablesMethod()
        {
        }

    }
}

