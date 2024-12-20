# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# This image is based on the public Ubuntu 18.04 LTS image
FROM mcr.microsoft.com/mirror/docker/library/ubuntu:18.04@sha256:152dc042452c496007f07ca9127571cb9c29697f42acbfad72324b2bb2e43c98

# REFERENCE: https://github.com/dotnet/dotnet-docker/blob/main/src/sdk/6.0/jammy/amd64/Dockerfile

ENV \
    # Do not generate certificate
    DOTNET_GENERATE_ASPNET_CERTIFICATE=false \
    # Do not show first run text
    DOTNET_NOLOGO=true \
    # .NET 6 SDK version
    DOTNET_6_SDK_VERSION=6.0.427 \
    # .NET 7 SDK version
    DOTNET_7_SDK_VERSION=7.0.410 \
    # Enable correct mode for dotnet watch (only mode supported in a container)
    DOTNET_USE_POLLING_FILE_WATCHER=true \
    # Skip extraction of XML docs - generally not useful within an image/container - helps performance
    NUGET_XMLDOC_MODE=skip

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        libicu60 \
        # Needed for "apt-add-repository" later in Dockerfile
        software-properties-common \
        wget \
    && rm -rf /var/lib/apt/lists/*

RUN \
    # Install .NET 6 SDK
    curl -fSL --output dotnet.tar.gz https://builds.dotnet.microsoft.com/dotnet/Sdk/$DOTNET_6_SDK_VERSION/dotnet-sdk-$DOTNET_6_SDK_VERSION-linux-x64.tar.gz \
    && dotnet_sha512='a9cd1e5ccc3c5d847aca2ef21dd145f61c6b18c4e75a3c2fc9aed592c6066d511b8b658c54c2cd851938fe5aba2386e5f6f51005f6406b420110c0ec408a8401' \
    && echo "$dotnet_sha512  dotnet.tar.gz" | sha512sum -c - \
    && mkdir -p /usr/share/dotnet \
    && tar -oxzf dotnet.tar.gz -C /usr/share/dotnet \
    && rm dotnet.tar.gz \
    # Install .NET 7 SDK
    && curl -fSL --output dotnet.tar.gz https://builds.dotnet.microsoft.com/dotnet/Sdk/$DOTNET_7_SDK_VERSION/dotnet-sdk-$DOTNET_7_SDK_VERSION-linux-x64.tar.gz \
    && dotnet_sha512='20b8e02979328e4c4a14493f7791ed419aabd0175233db80cd60e2c004b829b3e8301281ea86b27ba818372473accf5a6d553e5354c54917c8e84d25f5855caa' \
    && echo "$dotnet_sha512  dotnet.tar.gz" | sha512sum -c - \
    && mkdir -p /usr/share/dotnet \
    && tar -oxzf dotnet.tar.gz -C /usr/share/dotnet \
    && rm dotnet.tar.gz \
    # Make dotnet symlink
    && ln -s /usr/share/dotnet/dotnet /usr/bin/dotnet \
    # Trigger first run experience by running arbitrary cmd
    && dotnet help

RUN echo 'export DOTNET_ROOT=$HOME/.dotnet' >> ~/.bashrc
RUN echo 'export DOTNET_EXE=$DOTNET_ROOT/dotnet' >> ~/.bashrc
RUN echo 'export PATH=$DOTNET_ROOT:$DOTNET_ROOT/tools:$PATH' >> ~/.bashrc

# get the signing key for the kitware (cmake) repository
RUN curl https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null

# Add the repository (ubuntu 18.04) to get the newest version of cmake
# NOTE: the repository will need to be changed if we update the version of ubuntu
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'

# CLR Instrumentation Engine Build Prerequisites
# CMake will be retrieved from the kitware repo
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        clang-3.9 \
        cmake \
        git \
        libxml2-dev \
        make \
        jq

# Git now checks for repository ownership
RUN git config --global --add safe.directory /root/ClrInstrumentationEngine/out/Linux/Intermediate/x64.Debug/_deps/googletest-src
RUN git config --global --add safe.directory /root/ClrInstrumentationEngine