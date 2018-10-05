// Copyright (c) Microsoft Corporation. All rights reserved.
// 

using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace InstrEngineTests.HttpMethodTests
{
    class Program
    {
        static int error = 0;

        static int Main(string[] args)
        {
            var req = HttpWebRequest.CreateHttp("http://windows.com").GetResponseAsync();

            req.Wait();

            new StreamReader(req.Result.GetResponseStream()).ReadToEnd();

            return 0;
        }
    }
}
