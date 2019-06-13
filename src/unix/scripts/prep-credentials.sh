#!/bin/bash

# Copyright (c) Microsoft Corporation. All rights reserved.
#

script_dir=$(dirname "$BASH_SOURCE")

# This script is used to place a PAT in a nuget.config to be used in linux environments to get private packages.
# This script expects the "PersonalAuthenticationToken" environment variable. It must contain a PAT to use to access the nuget packages source.

# Set the root of the enlistment
pushd $script_dir/../../.. > /dev/null
EnlistmentRoot=$(pwd)
popd > /dev/null

# check enlistment layout sanity
if [ ${#PersonalAuthenticationToken} != 52 ]; then
    echo "##vso[task.logissue type=error]PersonalAuthenticationToken not valid." >&2
    echo "##vso[task.logissue type=error]\$PersonalAuthenticationToken's length=${#PersonalAuthenticationToken}, expecting 52. \$PersonalAuthenticationToken='$PersonalAuthenticationToken'" >&2
    exit 1
fi
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

# clean out existing file (if it exists)
if [ -f "$nugetConfigPath" ]; then
    echo "##vso[task.logissue type=warning]Removing '$nugetConfigPath'" >&2
    rm -f "$nugetConfigPath"
fi

# copy the template and replace substitution parameters
cat "$EnlistmentRoot/src/unix/dependencies/nuget.template.config" | sed "s/{PersonalAuthenticationToken}/$PersonalAuthenticationToken/" > "$nugetConfigPath"

if [ -f "$nugetConfigPath" ]; then
    echo "Contents of $nugetConfigPath:" >&2
    cat $nugetConfigPath
fi
if [ ! -f "$nugetConfigPath" ]; then
    echo "##vso[task.logissue type=error]Failed to write '$nugetConfigPath'" >&2
    exit 1
fi