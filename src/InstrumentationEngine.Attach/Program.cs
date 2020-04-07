// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.CommandLine;
using System.CommandLine.Builder;
using System.CommandLine.Parsing;
using System.Threading.Tasks;

namespace Microsoft.InstrumentationEngine
{
    internal sealed class Program
    {
        internal static Task<int> Main(string[] args)
        {
            var parser = new CommandLineBuilder()
                .AddCommand(AttachCommandHandler.GetCommand())
                .UseDefaults()
                .Build();

            return parser.InvokeAsync(args);
        }
    }
}
