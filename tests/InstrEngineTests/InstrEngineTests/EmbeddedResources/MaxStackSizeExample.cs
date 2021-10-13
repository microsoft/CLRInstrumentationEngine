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
            GetHdiPolicyValidationRequest(new HdiJobMetadata()
            {
                ExperimentId = String.Empty,
                File = String.Empty,
                ModuleInfo = new ModuleInfoDto
                {
                    ModuleId = String.Empty
                },
                Args = new List<string>()
            });

            return 0;
        }

        public static ValidationRequestDto GetHdiPolicyValidationRequest(HdiJobMetadata job)
        {
            ValidationRequestDto validationRequest = new ValidationRequestDto()
            {
                ExperimentName = job.ExperimentId ?? throw new ArgumentNullException(String.Empty),
                ExecutionContext = new ExecutionContextDto()
                {
                    ModuleInfo = new ModuleInfoDto
                    {
                        SnapshotIds = new List<string>() { job.ModuleInfo.ModuleId },
                        Executable = job.File.ToLower().EndsWith(String.Empty) ? String.Empty : String.Empty,
                        Arguments = string.Join(string.Empty, job.Args)
                    }
                },
            };

            return validationRequest;
        }

        public class ValidationRequestDto
        {
            public string RunId { get; set; }

            public string ParentRunId { get; set; }

            public string ExperimentName { get; set; }

            public ExecutionContextDto ExecutionContext { get; set; }
        }

        public class HdiJobMetadata
        {
            public ModuleInfoDto ModuleInfo { get; set; }

            public string ExperimentId { get; set; }

            public string File { get; set; }

            public List<string> Args { get; set; }
        }

        public class ExecutionContextDto
        {
            public ModuleInfoDto ModuleInfo { get; set; }
        }

        public class ModuleInfoDto
        {
            public string ModuleId { get; set; }

            public List<string> SnapshotIds { get; set; }

            public string Executable { get; set; }

            public string Arguments { get; set; }
        }
    }
}
