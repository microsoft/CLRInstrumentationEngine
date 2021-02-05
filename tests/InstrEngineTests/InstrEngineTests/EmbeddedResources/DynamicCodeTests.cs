﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Reflection;
using System.Reflection.Emit;

namespace DynamicCodeTests
{
    public class Program
    {

        const string CallForwarderSource = @"using System;

namespace CompiledCode
{
    static public class CallForwarder
    {
        public static void ForwardCallCompiled(Action a)
        {
            a();
        }
    }
}";

        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                throw new ArgumentException("Expected One Argument");
            }
            switch (args[0])
            {
                case "Compiled":
                    CallCompiledAssembly();
                    break;
                case "Emitted":
                    CallEmittedAssembly();
                    break;
                default:
                    throw new ArgumentException(args[0]);
            }
        }

        private static void CallCompiledAssembly()
        {
            string assemblyPath = Path.Combine(
                Path.GetDirectoryName(Assembly.GetEntryAssembly().Location),
                "DynamicCodeAssembly.dll");

            using FileStream assemblyStream = new FileStream(assemblyPath, FileMode.Open, FileAccess.Read);
            using BinaryReader assemblyReader = new BinaryReader(assemblyStream);
            byte[] assemblyData = assemblyReader.ReadBytes((int)assemblyStream.Length);

            Assembly compiledAssembly = AppDomain.CurrentDomain.Load(assemblyData);

            Type t = compiledAssembly.GetType("CompiledCode.CallForwarder");
            MethodInfo mi = t.GetMethod("ForwardCallCompiled", new Type[] { typeof(System.Action) });
            mi.Invoke(null, new object[] { new Action(CallHelloFromCompiledSource) });
        }

        private static void CallHelloFromCompiledSource()
        {
            // redirect to ensure that Jit Compilation does not occur until this method is called.
            CallHelloFromCompiledSource2();
        }

        private static void CallHelloFromCompiledSource2()
        {
            Console.WriteLine("Hello from Compiled Source");
        }

        static void CallEmittedAssembly()
        {
            AssemblyName aname = new AssemblyName("DynamicEmitAssembly.dll");
            AssemblyBuilder builder = AssemblyBuilder.DefineDynamicAssembly(aname, AssemblyBuilderAccess.Run);
            ModuleBuilder mBuilder = builder.DefineDynamicModule("DynamicEmitAssembly.dll");
            TypeBuilder tbuilder = mBuilder.DefineType("EmittedCode.CallForwarder", TypeAttributes.Public | TypeAttributes.Class);
            MethodBuilder methodBuilder = tbuilder.DefineMethod("ForwardCallEmitted", MethodAttributes.Public | MethodAttributes.Static, CallingConventions.Standard, typeof(void), new Type[] { typeof(System.Action) });
            ILGenerator il = methodBuilder.GetILGenerator();
            il.Emit(OpCodes.Ldarg_0);
            il.EmitCall(OpCodes.Callvirt, typeof(Action).GetMethod("Invoke"), new Type[0]);
            il.Emit(OpCodes.Ret);
            Type t = tbuilder.CreateType();

            MethodInfo mi = t.GetMethod("ForwardCallEmitted");

            mi.Invoke(null, new object[] { new Action(CallHelloFromEmittedAssembly) });
        }

        private static void CallHelloFromEmittedAssembly()
        {
            // redirect to ensure that Jit Compilation does not occur until this method is called.
            CallHelloFromEmittedAssembly2();
        }

        private static void CallHelloFromEmittedAssembly2()
        {
            Console.WriteLine("Hello from Emitted Source");
        }
    }
}
