// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace BasicManagedTest
{
    class Program
    {
        static int Main(string[] args)
        {
            int error = 0;

            IfTest();
            WhileTest();
            ForTest();
            SwitchTest();

            error += SwitchTest2();

            return error;
        }

        static void IfTest()
        {
            int x = 0;
            if (x == 3)
            {
                System.Console.WriteLine(x.ToString());
            }
            else
            {
                System.Console.WriteLine(x.ToString());
            }
        }

        static void WhileTest()
        {
            int x = 0;
            while (x < 3)
            {
                System.Console.WriteLine(x.ToString());
                x++;
            }
        }

        static void ForTest()
        {
            for (int i = 0; i < 3; i++)
            {
                System.Console.WriteLine(i.ToString());
            }
        }

        static void SwitchTest()
        {
            int x = 3;

            switch (x)
            {
                case 0:
                    System.Console.WriteLine(x.ToString());
                    break;
                case 1:
                    System.Console.WriteLine(x.ToString());
                    break;
                case 2:
                    System.Console.WriteLine(x.ToString());
                    break;
                case 3:
                    System.Console.WriteLine(x.ToString());
                    break;
                default:
                    System.Console.WriteLine("default case");
                    break;
            }
        }

        static int SwitchTest2()
        {
            int error = 0;
            int x = 5;
            switch(x)
            {
                case 2:
                case 4:
                    System.Console.WriteLine(x.ToString());
                    error += 1;
                    break;
                case 5:
                    System.Console.WriteLine(x.ToString());
                    break;
                case 10:
                case 11:
                    System.Console.WriteLine(x.ToString());
                    error += 1;
                    break;
                default:
                    System.Console.WriteLine(x.ToString());
                    error += 1;
                    break;
            }

            return error;
        }
    }
}
