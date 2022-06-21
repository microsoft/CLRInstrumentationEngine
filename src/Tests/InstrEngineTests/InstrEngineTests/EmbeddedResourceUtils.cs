// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Security;
using System.Text;
using System.Threading.Tasks;

namespace InstrEngineTests
{

    /// <summary>
    /// A temporary file that was extracted from an embedded resource. The file will
    /// be placed in a folder under the test's profile directory.
    /// </summary>
    internal interface IEmbeddedResourceFile
    {
        /// <summary>
        /// Path to the extracted file.
        /// </summary>
        string FilePath { get; }

        /// <summary>
        /// The original name of the resource.
        /// </summary>

        string ResourceName { get; }
    }

    internal class EmbeddedResourceUtils
    {
        private const string EmbeddedResourcesPath = "InstrEngineTests.EmbeddedResources";
        private const string IL_EmbeddedResourcesPath = EmbeddedResourcesPath + ".InvalidCSharp";
        private static readonly object ExtractionLock = new object();

        /// <inheritdoc/>
        internal class EmbeddedResourceFile : IEmbeddedResourceFile
        {
            private FileInfo _tempFile;

            /// <inheritdoc/> 
            public string FilePath => _tempFile.FullName;

            /// <inheritdoc/>
            public string ResourceName { get; }

            public EmbeddedResourceFile(FileInfo tempFile, string resourceName)
            {
                _tempFile = tempFile;
                ResourceName = resourceName;
            }

            internal void CopyFromStream(Stream stream)
            {
                using (var writeStream = _tempFile.Open(FileMode.CreateNew, FileAccess.Write, FileShare.Read))
                {
                    stream.CopyTo(writeStream);
                }
            }
        }

        public static string ReadEmbeddedResourceFile(string file, bool required = true)
        {
            Stream stream = GetEmbeddedResource(file, required: required);
            if (stream == null)
            {
                return null;
            }

            using (StreamReader reader = new StreamReader(stream))
            {
                return reader.ReadToEnd();
            }
        }

        /// <summary>
        /// Looks for the given embedded resource, and extracts it to the test assets folder.
        /// If the file was previously extracted, it is not overwritten.
        /// </summary>
        /// <param name="resourceName">The name of the resource.</param>
        /// <param name="required">True if an error should occur when the resource is not found.</param>
        /// <returns>An IEmbeddedResourceFile that has info about the extracted file.</returns>
        public static IEmbeddedResourceFile GetTestResourceFile(string resourceName, bool required = true, bool isIntermediateLanguage = false)
        {
            // Don't allow parallel tests to overwrite extracted files.
            lock (ExtractionLock)
            {
                Stream stream = GetEmbeddedResource(resourceName, required, isIntermediateLanguage);
                if (stream == null)
                {
                    return null;
                }
                using (stream)
                {
                    DirectoryInfo resourceDirectory = new DirectoryInfo(PathUtils.GetResourcesFolder());
                    resourceDirectory.Create();

                    string tempFileName = Path.Combine(PathUtils.GetResourcesFolder(), resourceName);
                    FileInfo fileInfo = new FileInfo(tempFileName);
                    var embeddedResource = new EmbeddedResourceFile(fileInfo, resourceName);
                    try
                    {
                        if (!fileInfo.Exists)
                        {
                            embeddedResource.CopyFromStream(stream);
                        }

                        return embeddedResource;
                    }
                    catch (Exception e)
                    {
                        Assert.Fail(e.Message);
                    }
                }

                return null;
            }
        }

        /// <summary>
        /// Deletes all test resource files that have been previously extracted.
        /// </summary>
        public static void CleanTestResourceFiles()
        {
            lock (ExtractionLock)
            {
                DirectoryInfo resourceDirectory = new DirectoryInfo(PathUtils.GetResourcesFolder());
                if (resourceDirectory.Exists)
                {
                    foreach (FileInfo f in resourceDirectory.EnumerateFiles())
                    {
                        f.Delete();
                    }
                }
            }
        }

        private static Stream GetEmbeddedResource(string fileName, bool required = true, bool isIntermediateLanguage = false)
        {
            Assert.IsNotNull(fileName);

            string resourcesPath = isIntermediateLanguage ? IL_EmbeddedResourcesPath : EmbeddedResourcesPath;

            var fullPath = FormattableString.Invariant($"{resourcesPath}.{fileName}");

            var stream = typeof(EmbeddedResourceUtils).Assembly.GetManifestResourceStream(fullPath);

            if (required)
            {
                Assert.IsNotNull(stream, "Could not find embedded resource {0}", fullPath);
            }

            return stream;
        }
    }
}
