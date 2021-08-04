#!/bin/bash
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

script_dir=`dirname $0`

print_install_instructions()
{
    if [ "$OSName" == "Linux" ]; then
        echo ""
        echo "To prepare a machine for building ClrInstrumentationEngine. Install the following:"
        echo "  # Install dotnet"
        echo "  Follow the instructions here: https://docs.microsoft.com/dotnet/core/linux-prerequisites?tabs=netcore2x"
        echo ""
        echo "  # Install required packages"
        echo "  sudo apt-get install cmake clang-4.0 libunwind8 libunwind8-dev uuid-dev"
        echo ""
    elif [ "$OSName" == "Darwin" ]; then
        echo ""
        echo "See the following for instructions on preparing a machine for building ClrInstrumentationEngine:"
        echo "https://microsoft.sharepoint.com/teams/DD_VSPlat/Diagnostics/_layouts/15/WopiFrame.aspx?sourcedoc={57593dcb-cb9c-4761-9981-a048d9896671}&action=edit&wd=target%283%2E%20Feature%20Design%2FMDD%2FXPlat%25%20%20%20%2020CLR%2Eone%7C282BD224%2D3E32%2D4984%2DB3F3%2D4325AB3F4ACC%2FBuilding%20vsdbg%20on%20MacOS%7C6B4DE44E%2DA2D8%2D4BD9%2D8B33%2D60AC8951F474%2F%29"
        echo ""
    fi
}

usage()
{
    echo "Script for building ClrInstrumentationEngine binaries on non Microsoft"
    echo "Windows platforms."
    echo ""
    echo "Usage: $0 [BuildArch] [BuildType] [clean] [verbose] [clangx.y]"
    echo "BuildArch can be: x64"
    echo "BuildType can be: Debug, Release"
    echo "clean - optional argument to force a clean build."
    echo "verbose - optional argument to enable verbose build output."
    echo "clangx.y - optional argument to build using clang version x.y."

    print_install_instructions
    exit 1
}

setup_dirs()
{
    echo Setting up directories for build

    mkdir -p "$__ClrInstrumentationEngineDir"
    mkdir -p "$__LogsDir"
    mkdir -p "$__IntermediatesDir"
}

# Performs "clean build" type actions (deleting and remaking directories)

clean()
{
    echo Cleaning previous output for the selected configuration
    rm -rf "$__BinDir"

    if [ $? -ne 0 ]
    then
        echo "ERROR: unable to remove directory '$__BinDir'."
        exit 1
    fi

    rm -rf "$__IntermediatesDir"
    if [ $? -ne 0 ]
    then
        echo "ERROR: unable to remove directory '$__IntermediatesDir'."
        exit 1
    fi

    rm -rf "$__LogsDir/*${__BuildOS}__${__BuildArch}__${__BuildType}.*"
}

# Check the system to ensure the right pre-reqs are in place

check_prereqs()
{
    echo "Checking pre-requisites..."

    get_cmake

    # Check for dotnet
    hash dotnet 2>/dev/null || { echo >&2 "Please install dotnet before running this script"; print_install_instructions; exit 1; }

    # make sure the linker we are using is at least gnu binutils ld >= 2.24 when on linux
    if [ "$OSName" == "Linux" ]; then
        ldMinorVersion="$(ld -v | grep GNU | cut -d'.' -f2)"
        if [ $ldMinorVersion -lt 24 ]; then
            echo ""
            echo >&2 "Error: The system linker 'ld' must be at least GNU ld 2.24"
            echo >&2 "The system linker 'ld' on this system is version '$(ld -v)'"
            echo >&2 "For instructions on installing 2.24 see https://microsoft.sharepoint.com/teams/DD_VSPlat/Diagnostics/_layouts/OneNote.aspx?id=%2Fteams%2FDD_VSPlat%2FDiagnostics%2FShared%20Documents%2FDebugger%2FOneNote%2FDebugger&wd=target%283.%20Feature%20Design%2FMDD%2FXPlat%20CLR.one%7C282BD224-3E32-4984-B3F3-4325AB3F4ACC%2FInstalling%20GNU%20ld%202.24%7CC8834EFB-6CC6-4C80-943C-2C24DE2413E9%2F%29"
            echo ""
            exit 1
        fi
    fi
}

locate_llvm_exec()
{
    if which "$llvm_prefix$1$desired_llvm_version" > /dev/null 2>&1
    then
    echo "$(which $llvm_prefix$1$desired_llvm_version)"
    elif which "$llvm_prefix$1" > /dev/null 2>&1
    then
    echo "$(which $llvm_prefix$1)"
    else
    exit 1
    fi
}


locate_google_test()
{
    manifest="$EnlistmentRoot/src/unix/docker/context/components/googletest/cgmanifest.json"
    __GoogleTestUrl=$(cat $manifest | jq '.Registrations[0].Component.git.RepositoryUrl' | sed 's/^\"\s*//;s/\"*$//')
    __GoogleTestTag=$(cat $manifest | jq '.Registrations[0].Component.git.Tag' | sed 's/^\"\s*//;s/\"*$//')
}

get_cmake()
{
    cmakeLocation=cmake

    #if cmake is not on this machine just download it.
    hash cmake 2>/dev/null || {
         $cmakeLocation=download_cmake
         echo cmakeLocation
         return; 
    }

    # get the current version of cmake, and see if it is high enough
    cmakeVersion=$(cmake --version)
    cmakeVersion=$(echo $cmakeVersion | sed 's/^.*[^0-9]\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/g')
    echo "found cmake version $cmakeVersion"
    cmakeMajor=$(echo $cmakeVersion | sed 's/^.*[^0-9]\([0-9]*\)\..*$/\1'/)
    cmakeMinor=$(echo $cmakeVersion | sed 's/^.*[^0-9]*\.\([0-9]*\)\..*$/\1'/)
    if (( $cmakeMajor > 3 )); then
        if (( $cmakeMinor >= 14 )); then
            # found sufficient cmake, continue.
            echo $cmakeLocation
            return
        fi
    fi

    echo "Insufficient cmake version. Ensure cmake version 3.14 or later is installed"
    exit 1
}


locate_build_tools()
{
    desired_llvm_major_version=$__ClangMajorVersion
    desired_llvm_minor_version=$__ClangMinorVersion
    if [ $OSName == "FreeBSD" ]; then
        desired_llvm_version="$desired_llvm_major_version$desired_llvm_minor_version"
    elif [ $OSName == "OpenBSD" ]; then
        desired_llvm_version=""
    elif [ $OSName == "NetBSD" ]; then
        desired_llvm_version=""
    else
      desired_llvm_version="-$desired_llvm_major_version.$desired_llvm_minor_version"
    fi

    llvm_ar="$(locate_llvm_exec ar)"
    [[ $? -eq 0 ]] || { echo "Unable to locate llvm-ar"; exit 1; }
    llvm_link="$(locate_llvm_exec link)"
    [[ $? -eq 0 ]] || { echo "Unable to locate llvm-link"; exit 1; }
    llvm_nm="$(locate_llvm_exec nm)"
    [[ $? -eq 0 ]] || { echo "Unable to locate llvm-nm"; exit 1; }
    if [ $OSName = "Linux" -o $OSName = "FreeBSD" -o $OSName = "OpenBSD" -o $OSName = "NetBSD" ]; then
      llvm_objdump="$(locate_llvm_exec objdump)"
      [[ $? -eq 0 ]] || { echo "Unable to locate llvm-objdump"; exit 1; }
    fi

    cmake_extra_defines=
    if [[ -n "$LLDB_LIB_DIR" ]]; then
        cmake_extra_defines="$cmake_extra_defines -DWITH_LLDB_LIBS=$LLDB_LIB_DIR"
    fi

    if [[ -n "$LLDB_INCLUDE_DIR" ]]; then
        cmake_extra_defines="$cmake_extra_defines -DWITH_LLDB_INCLUDES=$LLDB_INCLUDE_DIR"
    fi

    if [[ $__BuildOS == "Linux" ]]; then
        linux_id_file="/etc/os-release"
    else
        echo "Build only supports Linux."
    fi

    if [[ -e $linux_id_file ]]; then
        source $linux_id_file
        cmake_extra_defines="$cmake_extra_defines -DCLR_CMAKE_LINUX_ID=$ID"
    fi

    locate_google_test
    $echo "found google version '$__GoogleTestTag' test at '$__GoogleTestUrl'"
}

invoke_build()
{
    # All set to commence the build

    echo "Commencing build for $__BuildOS.$__BuildArch.$__BuildType"
    cd "$__IntermediatesDir"

    locate_build_tools

    # Regenerate the CMake solution
    cmake \
      "-DCMAKE_AR=$llvm_ar" \
      "-DCMAKE_LINKER=$llvm_link" \
      "-DCMAKE_NM=$llvm_nm" \
      "-DCMAKE_OBJDUMP=$llvm_objdump" \
      "-DCMAKE_BUILD_TYPE=$__BuildType" \
      "-DCMAKE_INSTALL_PREFIX=$__ClrInstrumentationEngineDir" \
      "-DCORECLR_PAL_ROOT=$PALRoot" \
      "-DREPOSITORY_ROOT=$EnlistmentRoot" \
      "-DINTERMEDIATES_DIR=$__IntermediatesDir" \
      "-DENGINEBINARIES_DIR=$__ClrInstrumentationEngineBinDir" \
      "-DGOOGLE_TEST_URL=$__GoogleTestUrl" \
      "-DGOOGLE_TEST_TAG=$__GoogleTestTag" \
      $cmake_extra_defines \
      "$EnlistmentRoot/src"

    # Check that the makefiles were created.
    if [ ! -f "$__IntermediatesDir/Makefile" ]; then
        echo "Failed to generate native component build project!"
        exit 1
    fi

    # Get the number of processors available to the scheduler
    # Other techniques such as `nproc` only get the number of
    # processors available to a single process.
    if [ `uname` = "FreeBSD" ]; then
        NumProc=`sysctl hw.ncpu | awk '{ print $2+1 }'`
    else
        NumProc=$(($(getconf _NPROCESSORS_ONLN)+1))
    fi

    echo "Executing make install -j $NumProc $__UnprocessedBuildArgs"

    #make install -j $NumProc $__UnprocessedBuildArgs > >(tee $__LogsDir/make__${__BuildOS}__${__BuildArch}__${__BuildType}.log) 2> >(tee $__LogsDir/make__${__BuildOS}__${__BuildArch}__${__BuildType}.err >&2)
    make -j $NumProc $__UnprocessedBuildArgs > >(tee $__LogsDir/make__${__BuildOS}__${__BuildArch}__${__BuildType}.log) 2> >(tee $__LogsDir/make__${__BuildOS}__${__BuildArch}__${__BuildType}.err >&2)
    if [ $? != 0 ]; then
        # If stdout (handle 1) is a terminal, change the output color to red
        [ -t 1 ] && echo -e "\033[0;31m"
        # Dump the errors
        cat $__LogsDir/make__${__BuildOS}__${__BuildArch}__${__BuildType}.err
        # Turn the output color back to normal
        [ -t 1 ] && echo -e "\033[0;0m"
        echo "Make failed. Standard output can be found at --"
        echo "   cat $__LogsDir/make__${__BuildOS}__${__BuildArch}__${__BuildType}.log"
        echo ""
        exit 1
    fi

    echo "Running tests"
    ctest
}

write_commit_file()
{
    # if we in a git enlistment then we want to write out a commit.txt
    # file. Don't make this a fatal error if we in a mapped directory instead
    # of a git enlistment since we only really need this for the offical build.
    if [ -d "$EnlistmentRoot/.git" ]
    then
        git -C "$EnlistmentRoot" rev-list HEAD --max-count=1 > $__BinDir/commit.txt
        if [ $? -ne 0 ]; then
            echo "ERROR: Unable to determine current commit."
            exit 1
        fi
    fi
}

restore_build_dependencies()
{
    # Check if $PALRoot/lib is a directory instead of a link
    if [[ -d $PALRoot/lib ]] && [[ ! -h $PALRoot/lib ]]; then
        # If UseLocalPAL.sh has been run then we don't need to restore dependencies
        echo "Using local CoreCLR PAL instead of package version. Skipping dotnet restore."
        return
    fi

    # NOTE: This file is generated on Windows, so remove both \r and \n
    local Version_CoreCLR_PAL=$(tr -d '\r\n' < $script_dir/unix/dependencies/CoreCLRPAL.Version.txt)

    if [ -z "$Version_CoreCLR_PAL" ]; then
        echo "ERROR: Unexpected format for $script_dir/unix/dependencies/dependencies.list. CoreCLR_PAL version was not defined."
        exit 1
    fi

    case $__BuildOS in
        Linux)
            _ToolNugetRuntimeId=linux
            if [ -e /etc/os-release ]; then
                source /etc/os-release
                if [[ $ID == "alpine" ]]; then
                    _ToolNugetRuntimeId=linux-musl
                fi
            fi
            ;;
        OSX)
            _ToolNugetRuntimeId=osx
            ;;
        *)
            echo "Error: Unexpected Build OS: $__BuildOS."
	    exit 1
            ;;
    esac
    _ToolNugetRuntimeId=$_ToolNugetRuntimeId-$__BuildArch

    CoreClrPALLinkTarget=$__PackagesDir/$__CoreCLRPALPackageId/$Version_CoreCLR_PAL/runtimes/$_ToolNugetRuntimeId

    if [ -h $PALRoot/lib ]; then
        CurrentCoreClrPALLinkTarget="$(readlink "$PALRoot/lib")"
        if [ "$CurrentCoreClrPALLinkTarget" == "$CoreClrPALLinkTarget" ]; then
            echo "CoreCLR package already restored. Skipping nuget restore."
            return
        fi
    fi

    rm -rf $PALRoot
    if [ $? -ne 0 ]; then
        echo "ERROR: Unable to remove directory $PALRoot."
        exit 1
    fi

    # Restore NuGet packages
    dotnet restore "$EnlistmentRoot/src/Dependencies/NativeDependencies.csproj" \
        --configfile "$__NuGetConfigPath" \
        --packages "$__PackagesDir"

    if [ $? -ne 0 ]; then
        echo "Error: Unable to restore package with dotnet."
        exit 1
    fi

    if [ ! -d $CoreClrPALLinkTarget ]; then
        echo "ERROR: Package restore did not create expected directory '$CoreClrPALLinkTarget'."
        exit 1
    fi

    mkdir -p $PALRoot
    if [ $? -ne 0 ]; then
        echo "ERROR: Unable to create $PALRoot."
        exit 1
    fi

    cp -r $__PackagesDir/$__CoreCLRPALPackageId/$Version_CoreCLR_PAL/inc $PALRoot/inc
    if [ $? -ne 0 ]; then
        echo "ERROR: Unable to copy PALRoot/inc contents."
        exit 1
    fi

    cp -r $CoreClrPALLinkTarget $PALRoot/lib
    if [ $? -ne 0 ]; then
        echo "ERROR: Unable to copy ALRoot/lib contents."
        exit 1
    fi
}

# Set default clang version
set_clang_path_and_version()
{
    if [[ $__ClangMajorVersion == 0 && $__ClangMinorVersion == 0 ]]; then
        # If the version is not specified, search for one

        local ClangVersion=""
        for ver in "3.6" "3.9" "4.0"; do
            hash "clang-$ver" 2>/dev/null
            if [ $? == 0 ]; then
               ClangVersion=$ver
            fi
        done

        if [ -z "$ClangVersion" ]; then
            # If the 'clang-<ver>' commands weren't installed, scrape version info from the 'clang' command
            hash clang 2>/dev/null
            if [ $? != 0 ]; then
                print_install_instructions
                exit 1
            fi
            ClangVersion=$(clang --version | head -n 1 | grep -o -E "[[:digit:]].[[:digit:]].[[:digit:]]" | uniq)
        fi

        local ClangVersionArray=(${ClangVersion//./ })
        __ClangMajorVersion="${ClangVersionArray[0]}"
        __ClangMinorVersion="${ClangVersionArray[1]}"

        if hash "clang-$__ClangMajorVersion.$__ClangMinorVersion" 2>/dev/null
            then
                export CC="$(command -v clang-$__ClangMajorVersion.$__ClangMinorVersion)"
        elif hash clang 2>/dev/null
            then
                export CC="$(command -v clang)"
        else
            echo "Unable to find Clang compiler"
            exit 1
        fi

        if hash "clang++-$__ClangMajorVersion.$__ClangMinorVersion" 2>/dev/null
            then
                export CXX="$(command -v clang++-$__ClangMajorVersion.$__ClangMinorVersion)"
        elif hash clang 2>/dev/null
            then
                export CXX="$(command -v clang++)"
        else
            echo "Unable to find clang++ compiler"
            exit 1
        fi
    else
        # If the version is specified, we require the strongly-versioned executables

        hash "clang-$__ClangMajorVersion.$__ClangMinorVersion" 2>/dev/null
        if [ $? != 0 ]; then
            echo "Specified clang version ($__ClangMajorVersion.$__ClangMinorVersion) was not found"
            exit 1
        fi
        export CC="$(command -v clang-$__ClangMajorVersion.$__ClangMinorVersion)"

        hash "clang++-$__ClangMajorVersion.$__ClangMinorVersion" 2>/dev/null
        if [ $? != 0 ]; then
            echo "Specified clang++ version ($__ClangMajorVersion.$__ClangMinorVersion) was not found"
            exit 1
        fi
        export CXX="$(command -v clang++-$__ClangMajorVersion.$__ClangMinorVersion)"
    fi

    # Clang 3.6 is required at minimum
    if ! [[ "$__ClangMajorVersion" -gt "3" || ( $__ClangMajorVersion == 3 && $__ClangMinorVersion -gt 5 ) ]]; then
        echo "Please install clang 3.6 or later"
        exit 1
    fi

    if [ ! -f "$CC" ]; then
        echo "clang path $CC does not exist"
        exit 1
    fi

    if [ ! -f "$CXX" ]; then
        echo "clang++ path $CXX does not exist"
        exit 1
    fi
}

# Set the root of the enlisment
pushd $script_dir/.. > /dev/null
EnlistmentRoot=$(pwd)
popd > /dev/null
if [ ! -f "$EnlistmentRoot/src/build.sh" ]; then
    echo "ERROR: Unexpected directory structure. Couldn't find project root." >&2
    exit 1
fi

__BuildArch=x64
# Use uname to determine what the OS is.
OSName=$(uname -s)
case $OSName in
    Linux)
        __BuildOS=Linux
        ;;

    Darwin)
        __BuildOS=OSX
        ;;

    FreeBSD)
        __BuildOS=FreeBSD
        echo "FreeBSD is currently not supported" >&2
        exit 1
        ;;

    OpenBSD)
        __BuildOS=OpenBSD
        echo "OpenBSD is currently not supported" >&2
        exit 1
        ;;

    NetBSD)
        __BuildOS=NetBSD
        echo "NetBSD is currently not supported" >&2
        exit 1
        ;;

    *)
        echo "Unsupported OS $OSName detected" >&2
        exit 1
        ;;
esac
__BuildType=Debug

# Set the various build properties here so that CMake and MSBuild can pick them up
__PackagesDir="$EnlistmentRoot/out/packages"
__LogsDir="$EnlistmentRoot/out/Logs"
__UnprocessedBuildArgs=
__MSBCleanBuildArgs=
__CleanBuild=false
__VerboseBuild=false
__ClangMajorVersion=0
__ClangMinorVersion=0
__NuGetConfigPath="$EnlistmentRoot/src/unix/dependencies/nuget.config"
# Package name is used in file system and "dotnet restore" will restore packages using lowercase characters in the filesystem.
__CoreCLRPALPackageId="microsoft.visualstudio.debugger.coreclrpal"

for i in "$@"
    do
        lowerI="$(echo $i | awk '{print tolower($0)}')"
        case $lowerI in
        -?|-h|--help)
        usage
        exit 1
        ;;
        x64)
        __BuildArch=x64
        ;;
        debug)
        __BuildType=Debug
        ;;
        release)
        __BuildType=Release
        ;;
        clean)
        __CleanBuild=1
        ;;
        verbose)
        __VerboseBuild=1
        ;;
        clang3.9)
        __ClangMajorVersion=3
        __ClangMinorVersion=9
        ;;
        clang4.0)
        __ClangMajorVersion=4
        __ClangMinorVersion=0
        ;;
        build-only|no-clr-restore)
        # These modes are now always on, so ignore them if they are passed, but do nothing.
        ;;
        *)
        __UnprocessedBuildArgs="$__UnprocessedBuildArgs $i"
    esac
done

# Set the remaining variables based upon the determined build configuration
OSOutRoot=$EnlistmentRoot/out/$__BuildOS
PALRoot=$OSOutRoot/CoreCLRPAL
__BinDir="$OSOutRoot/bin/$__BuildArch.$__BuildType"
__ClrInstrumentationEngineDir="$__BinDir/ClrInstrumentationEngine"
__IntermediatesDir="$OSOutRoot/Intermediate/$__BuildArch.$__BuildType"
HandshakeRoot="$__IntermediatesDir/vsdbg/handshake"

# Configure environment if we are doing a clean build.
if [ $__CleanBuild == 1 ]; then
    clean
fi

# Configure environment if we are doing a verbose build
if [ $__VerboseBuild == 1 ]; then
    export VERBOSE=1
fi

set_clang_path_and_version

# Make the directories necessary for build if they don't exist

setup_dirs

# Check prereqs.

check_prereqs

restore_build_dependencies

pwd

invoke_build

write_commit_file

#copy binaries to output

find $__IntermediatesDir -name "*.so" -exec cp {} $__ClrInstrumentationEngineDir ";"

# Build complete

echo "Repo successfully built."
echo "Product binaries are available at $__ClrInstrumentationEngineDir"
exit 0
