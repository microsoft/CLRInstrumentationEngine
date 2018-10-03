#!/bin/bash

script_dir=$(dirname "$BASH_SOURCE")

print_install_instructions()
{
    echo "Docker must be installed to run this script."
    echo "To install docker, see here:"
    echo "https://docs.docker.com/engine/installation/"
}

usage()
{
    echo "Script for ClrInstrumentationEngine native binaries inside of a Docker Container."
    echo ""
    echo "Usage: $0 <DockerImage> [BuildArch] [BuildType] [additional build.sh params]"
    echo "DockerImage - An existing Docker image to use for the container build"
    echo "   Pass 'ubuntu' to use the image for building ubuntu linux"
    echo "   Pass 'alpine' to use the image for building alpine linux"
    echo "BuildArch can be: x64"
    echo "BuildType can be: Debug, Release"
    echo "All additional parameters will be passed to build.sh inside the container"

    print_install_instructions

    exit 1
}

check_prereqs()
{
    echo "Checking prerequisites..."

    # Check presence of Docker on path
    hash docker 2>/dev/null || { echo >&2 "Please install docker before running this script"; print_install_instructions; exit 1; }
}

clean()
{
    echo Cleaning previous output for the selected configuration
    rm -rf "$__BinDir"

    if [ $? -ne 0 ]
    then
        echo "ERROR: unable to remove directory '$__BinDir'."
        exit 1
    fi

    rm -rf "$__LogsDir/*Linux__${__BuildArch}__${__BuildType}.*"
}

write_commit_file()
{
    # if we are in a git enlistment then we want to write out a commit.txt
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

invoke_build()
{
     # Delegate to build.sh inside of the docker container

    echo "Commencing build inside $__DockerImage"

    docker inspect clrie-build &>/dev/null
    if [ $? -eq 0 ]; then
        docker stop -t 0 clrie-build
        docker rm clrie-build
    fi

    local LocalSrcPath=$EnlistmentRoot/src
    if [ -L "$LocalSrcPath" ]; then
       LocalSrcPath=$(readlink $LocalSrcPath)
    fi
    local docker_run_args="--name clrie-build -v $LocalSrcPath:/root/ClrInstrumentationEngine/src"

    local LocalToolsPath=$EnlistmentRoot/tools
    if [ -L "$LocalToolsPath" ]; then
       LocalToolsPath=$(readlink $LocalToolsPath)
    fi
    docker_run_args="$docker_run_args -v $LocalToolsPath:/root/ClrInstrumentationEngine/tools"

    local LocalIncPath=$EnlistmentRoot/inc
    if [ -L "$LocalIncPath" ]; then
       LocalIncPath=$(readlink $LocalIncPath)
    fi
    docker_run_args="$docker_run_args -v $LocalIncPath:/root/ClrInstrumentationEngine/inc"

    build_cmd="/root/ClrInstrumentationEngine/src/build.sh $__BuildArch $__BuildType clean $__UnprocessedBuildArgs"
    docker_run_args="$docker_run_args --net=host $__DockerImage $build_cmd"
    
    echo "executing: docker run $docker_run_args"
    docker run $docker_run_args
    local buildExitCode=$?
    local buildCopyError=0

    mkdir -p $EnlistmentRoot/out/Linux/bin
    if [ $? -ne 0 ]; then
        buildCopyError=1
        echo "ERROR: failed to create $EnlistmentRoot/out/Linux/bin."
    fi

    docker cp clrie-build:/root/ClrInstrumentationEngine/$__BinRelDir $EnlistmentRoot/out/Linux/bin
    if [ $? -ne 0 ]; then
        buildCopyError=2
        echo "ERROR: failed to copy build output from container."
    fi

    docker cp clrie-build:/root/ClrInstrumentationEngine/out/Logs $EnlistmentRoot/out
    if [ $? -ne 0 ]; then
        buildCopyError=3
        echo "ERROR: failed to copy build logs from container."
    fi

    docker rm clrie-build > /dev/null
    
    if [ $buildExitCode -ne 0 ]; then
        echo "docker build failed with exit code $buildExitCode"
        exit $buildExitCode
    fi

    if [ $buildCopyError -ne 0 ]; then
        exit $buildCopyError
    fi

    write_commit_file
}

# Set the root of the enlistment
pushd $script_dir/../../.. > /dev/null
EnlistmentRoot=$(pwd)
popd > /dev/null
if [ ! -f "$EnlistmentRoot/src/unix/docker/docker-build.sh" ]; then
    echo "ERROR: Unexpected directory structure. Couldn't find project root." >&2
    exit 1
fi
if [ ! -f "$EnlistmentRoot/src/build.sh" ]; then
    echo "ERROR: Unexpected directory structure. Couldn't find project root." >&2
    exit 1
fi

__BuildArch=x64
__BuildType=Debug

__DockerImage=$1
if [[ -z "$__DockerImage" || "$__DockerImage" == "-h" || "$__DockerImage" == "--help" || "$__DockerImage" == "-?" ]]; then
    usage
    exit 1
fi
if [ "$__DockerImage" == "ubuntu" ]; then
    __DockerImage=proddiagbuild.azurecr.io/clrie-build-ubuntu:latest
elif [ "$__DockerImage" == "alpine" ]; then
    __DockerImage=proddiagbuild.azurecr.io/clrie-build-alpine:latest
fi

shift
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
    arm)
        __BuildArch=arm
        ;;
    debug)
        __BuildType=Debug
        ;;
    release)
        __BuildType=Release
        ;;
    *)
        __UnprocessedBuildArgs="$__UnprocessedBuildArgs $i"
    esac
done

__BinRelDir=out/Linux/bin/$__BuildArch.$__BuildType
__BinDir="$EnlistmentRoot/$__BinRelDir"
__LogsDir="$EnlistmentRoot/out/Logs"

check_prereqs

clean

invoke_build

exit 0
