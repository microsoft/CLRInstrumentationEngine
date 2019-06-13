// Copyright (c) Microsoft Corporation. All rights reserved.
//

using System;

namespace BasicManagedTest2
{
    class Program
    {
        static int Main(string[] args)
        {
            int error = 0;

            error += ArrayForeachTest();
            error += TinyMethodTest(8);

            return error;
        }

        static int ArrayForeachTest()
        {
            string result = "";
            string[] sArray = { "s1", "s2", "s3" };
            foreach (string s in sArray)
            {
                result += s;
            }

            if (result == "s1s2s3")
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }

        static int TinyMethodTest(int a)
        {
            if (a * 7 > 22)
                return (37 % 2 == 0) ? 1 : 0;
            else
                return 1;
        }
    }
}
