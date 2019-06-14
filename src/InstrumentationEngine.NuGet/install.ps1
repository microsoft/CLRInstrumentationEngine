# Copyright (c) Microsoft Corporation. All rights reserved.
#

param($installPath, $toolsPath, $package, $project)

if ([System.IO.File]::Exists($project.FullName))
{
	function MarkFilesAsCopyToOutputRecursive($item)
	{
		$item.ProjectItems | ForEach-Object { MarkItemASCopyToOutputDirectory($_) }
	}

	function MarkItemASCopyToOutputDirectory($item)
	{
		Try
		{
			#if it's a file - mark it to copy if newer
			$item.Properties.Item("CopyToOutputDirectory").Value = 2
		}
		Catch
		{
			#if it's a folder - process it
			MarkFilesAsCopyToOutputRecursive($item)
		}
	}

	MarkFilesAsCopyToOutputRecursive($project.ProjectItems.Item("Microsoft.InstrumentationEngine"))
}