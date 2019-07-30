#!/bin/bash
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

script_dir=$(dirname "$BASH_SOURCE")

# This script is used to delete the nuget.config file that was created by prep-credentials.sh

# Set the root of the enlistment
pushd $script_dir/../../.. > /dev/null
EnlistmentRoot=$(pwd)
popd > /dev/null

# check enlistment layout sanity
if [ ! -f "$EnlistmentRoot/src/unix/scripts/prep-credentials.sh" ]; then
    echo "##vso[task.logissue type=error]Unexpected directory structure (src/unix/scripts/prep-credentials.sh not found). Couldn't find project root." >&2
    echo "##vso[task.logissue type=error]Enlistment root='$EnlistmentRoot'" >&2
    exit 1
fi
if [ ! -f "$EnlistmentRoot/src/unix/dependencies/nuget.template.config" ]; then
    echo "##vso[task.logissue type=error]Unexpected directory structure (src/unix/dependencies/nuget.template.config not found). Couldn't find project root." >&2
    echo "##vso[task.logissue type=error]Enlistment root='$EnlistmentRoot'" >&2
    exit 1
fi

nugetConfigPath=$EnlistmentRoot/src/unix/dependencies/nuget.config

if [ -f "$nugetConfigPath" ]; then
    echo "Contents of $nugetConfigPath:" >&2
    cat $nugetConfigPath >&2
fi

# clean out existing file (if it exists)
if [ -f "$nugetConfigPath" ]; then
    echo "Removing '$nugetConfigPath'" >&2
    rm -f "$nugetConfigPath"
fi

if [ -f "$nugetConfigPath" ]; then
    echo "##vso[task.logissue type=error]Failed to remove '$nugetConfigPath'" >&2
    exit 1
fi