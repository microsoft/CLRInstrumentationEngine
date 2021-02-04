using System;

namespace CompiledCode
{
    static public class CallForwarder
    {
        public static void ForwardCallCompiled(Action a)
        {
            a();
        }
    }
}