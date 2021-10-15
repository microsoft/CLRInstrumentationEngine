// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace MaxStackSizeExample
{
    class Program
    {
        static int Main(string[] args)
        {
            GetRequest(new Job()
            {
                Id = String.Empty,
                File = String.Empty,
                Info = new Info
                {
                    Id = String.Empty
                },
                Args = new List<string>()
            });

            MethodWithExceptionFilter();

            return 0;
        }

        public static Request GetRequest(Job job)
        {
            Request request = new Request()
            {
                Name = job.Id ?? throw new ArgumentNullException(String.Empty),
                Context = new ContextDto()
                {
                    Info = new Info
                    {
                        SnapshotIds = new List<string>() { job.Info.Id },
                        Executable = job.File.ToLower().EndsWith(String.Empty) ? String.Empty : String.Empty,
                        Arguments = string.Join(string.Empty, job.Args)
                    }
                },
            };

            return request;
        }

        public static void MethodWithExceptionFilter()
        {
            try
            {
                try
                {
                    throw new Exception("ab");
                }
                catch (Exception e) when (e.Message.Contains("a") && e.Message.Contains("b") && e.Message.Contains("c") && Other(1, 2, 3, 4))
                {
                    Console.WriteLine(e.Message);
                }
            }
            catch
            {
            }
        }

        public class Request
        {
            public string RunId { get; set; }

            public string ParentRunId { get; set; }

            public string Name { get; set; }

            public ContextDto Context { get; set; }
        }

        public class Job
        {
            public Info Info { get; set; }

            public string Id { get; set; }

            public string File { get; set; }

            public List<string> Args { get; set; }
        }

        public class ContextDto
        {
            public Info Info { get; set; }
        }

        public class Info
        {
            public string Id { get; set; }

            public List<string> SnapshotIds { get; set; }

            public string Executable { get; set; }

            public string Arguments { get; set; }
        }

        public static bool Other(int a, int b, int c, int d)
        {
            return a == b && c == d;
        }
    }
}
