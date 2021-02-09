﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Linq;
using System.Reflection;

namespace TestAppRunner
{
    /// <summary>
    /// Purpose is to bootstrap assemblies that cannot be run directly. For example,
    /// test apps that are compiled by Microsoft.CodeAnalysis do not have runtimeconfig.json
    /// files so they cannot be run by dotnet.exe directly.
    /// </summary>
    class Program
    {
        static int Main(string[] args)
        {
            if (args.Length == 0)
            {
                throw new InvalidOperationException("Expected at least one argument with name of assembly to execute.");
            }

            string assemblyPath = args[0];

            using FileStream assemblyStream = new FileStream(assemblyPath, FileMode.Open, FileAccess.Read);
            using BinaryReader assemblyReader = new BinaryReader(assemblyStream);
            byte[] assemblyData = assemblyReader.ReadBytes((int)assemblyStream.Length);

            Assembly assembly = AppDomain.CurrentDomain.Load(assemblyData);

            if (null == assembly.EntryPoint)
            {
                throw new InvalidOperationException("Specified assembly does not have an entry point.");
            }

            // Pass remaining arguments to assembly entry point.
            string[] entryPointArgs = args.Skip(1).ToArray();

            // Create a delegate for the entry point and invoke it. This allows
            // direct invocation of the entry point and avoids putting more frames
            // on the callstack (important for exception tests).
            // NOTE: All current test app entry points only return void or int. If one is
            // added that returns Task, then the delegate creation and TestAppRunner entry
            // point need to be updated to handle async method invocation.
            if (assembly.EntryPoint.ReturnType == typeof(void))
            {
                Action<string[]> entryPointDelegate = (Action<string[]>)assembly.EntryPoint
                    .CreateDelegate(typeof(Action<string[]>));

                entryPointDelegate(entryPointArgs);

                return 0;
            }
            else
            {
                Func<string[], int> entryPointDelegate = (Func<string[], int>)assembly.EntryPoint
                    .CreateDelegate(typeof(Func<string[], int>));

                return entryPointDelegate(entryPointArgs);
            }
            
        }
    }
}
