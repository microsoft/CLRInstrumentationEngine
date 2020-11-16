using System;
using System.Runtime.CompilerServices;

namespace InstrEngineTests.EmbeddedResources
{
    public class Program
    {
        static void Main(string[] args)
        {
            MultiIfTest(5);
            MultiSwitchTest(5);
            MultiExceptionTest(5);
        }

        private static int MultiExceptionTest(int v)
        {
            try
            {
                if (v == 5)
                {
                    return 1;
                }
                else if (v == 3)
                {
                    throw new InvalidOperationException("That was not valid");
                }
                else
                {
                    Console.WriteLine("Here is a string");
                }
            }
            catch (InvalidOperationException e)
            {
                return e.HResult;
            }
            finally
            {
                Console.WriteLine("Entered Finally clause");
            }

            return 0;
        }

        private static int MultiSwitchTest(int v)
        {
            switch (v)
            {
                case 1:
                    Console.WriteLine("Return 1");
                    return 1;
                case 2:
                    Console.WriteLine("Return 2");
                    return 2;
                case 3:
                    Console.WriteLine("Return 3");
                    return 3;
                default:
                    return 5;
            }
        }

        private static int MultiIfTest(int v)
        {
            if (v == 1)
            {
                return 1;
            }
            if (v == 2)
            {
                return 2;
            }
            else if (v == 10)
            {
                return 3;
            }

            return 4;
        }
    }
}
