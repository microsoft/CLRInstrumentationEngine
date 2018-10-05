// Copyright (c) Microsoft Corporation. All rights reserved.
// 

using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.IO.Compression;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace Microsoft.VisualStudio.ProductionDiagnostics.BuildTasks
{
    public class ZipArchiveTask :
        Task
    {
        [Required]
        public ITaskItem[] Files { get; set; }

        [Required]
        public string OutputPath { get; set; }

        public override bool Execute()
        {
            if (String.IsNullOrEmpty(OutputPath))
            {
                Log.LogError("Parameter 'OutputPath' was not specified.");
                return false;
            }

            Log.LogMessage("Number of items to archive: {0}", Files.Length);

            bool success = true;
            try
            {
                using (FileStream zipToOpen = new FileStream(OutputPath, FileMode.Create))
                using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Update))
                {
                    foreach (ITaskItem fileItem in Files)
                    {
                        string destinationFileName = new FileInfo(fileItem.ItemSpec).Name;
                        string destinationFolder = fileItem.GetMetadata("Destination");
                        string type = fileItem.GetMetadata("Type");
                        if (!string.IsNullOrEmpty(destinationFolder))
                        {
                            destinationFileName  = Path.Combine(destinationFolder, destinationFileName);
                        }

                        // Create directories
                        if (!string.IsNullOrEmpty(type) && type.Equals("Directory"))
                        {
                            ZipArchiveEntry entry = archive.CreateEntry(destinationFileName.TrimEnd('/') + '/');
                        }
                        else
                        {
                            using (Stream fs = File.Open(fileItem.ItemSpec, FileMode.Open, FileAccess.Read, FileShare.Read))
                            {
                                ZipArchiveEntry entry = archive.CreateEntry(destinationFileName);
                                using (Stream es = entry.Open())
                                    fs.CopyTo(es);
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Log.LogErrorFromException(ex);
                success = false;
            }

            return success;
        }
    }
}
