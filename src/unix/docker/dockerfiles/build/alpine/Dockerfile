# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

FROM mcr.microsoft.com/dotnet-buildtools/prereqs:alpine-3.13@sha256:1fdd7085cf4497b31fa1192f35c8e8cd7e9da343ebe4d969bd4ce977790a8a99

# Install .NET SDK
# REFERENCE: https://github.com/dotnet/dotnet-docker/blob/main/src/sdk/6.0/alpine3.20/amd64/Dockerfile

ENV \
    # Do not generate certificate
    DOTNET_GENERATE_ASPNET_CERTIFICATE=false \
    # Do not show first run text
    DOTNET_NOLOGO=true \
    # SDK version
    DOTNET_SDK_VERSION=6.0.427 \
    # Disable the invariant mode (set in base image)
    DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=false \
    # Enable correct mode for dotnet watch (only mode supported in a container)
    DOTNET_USE_POLLING_FILE_WATCHER=true \
    # Skip extraction of XML docs - generally not useful within an image/container - helps performance
    NUGET_XMLDOC_MODE=skip

RUN apk add --upgrade --no-cache \
    curl \
    icu-libs

RUN wget -O dotnet.tar.gz https://builds.dotnet.microsoft.com/dotnet/Sdk/$DOTNET_SDK_VERSION/dotnet-sdk-$DOTNET_SDK_VERSION-linux-musl-x64.tar.gz \
    && dotnet_sha512='38e63bc2e94b5dfbaa5ffcc31e96eaaf9889a86ae03b2bba72ed73434d79857d56566345c65a20c7a5e62f444b8f13a3ed6a3e7e568a3c34c837cfcecd1ca68f' \
    && echo "$dotnet_sha512  dotnet.tar.gz" | sha512sum -c - \
    && mkdir -p /usr/share/dotnet \
    && tar -oxzf dotnet.tar.gz -C /usr/share/dotnet \
    && rm dotnet.tar.gz \
    && ln -s /usr/share/dotnet/dotnet /usr/bin/dotnet \
    # Trigger first run experience by running arbitrary cmd
    && dotnet help

# CLR Instrumentation Engine Build Prerequisites
RUN apk update \
    && apk add --no-cache libxml2-dev

# Add jq to make it easer to read json in shell scripts
RUN apk add --no-cache jq

# Install newer version of cmake. We need to run make commands inside the directory
# that it is extracted to, so WORKDIR is the easiest context to use.
WORKDIR /usr/share/cmakeinst

# Copy manifest json to the target so that we can extract the download url and
# version of cmake
COPY "components/cmake/cgmanifest.json" /usr/share/cmakeinst
RUN cmakeurl=$(cat cgmanifest.json | jq '.Registrations[0].Component.other.DownloadUrl' | sed 's/^\"\s*//;s/\"*$//') \
    && cmakever=$(cat cgmanifest.json | jq '.Registrations[0].Component.other.Version' | sed 's/^\"\s*//;s/\"*$//') \
    && cmakesha256=$(cat cgmanifest.json | jq '.Registrations[0].Component.other.SHA256CHECK' | sed 's/^\"\s*//;s/\"*$//') \
    && echo found cmake version "$cmakever" at url "$cmakeurl" \
    && wget -O cmake.tar.gz $cmakeurl \
    && echo "$cmakesha256 cmake.tar.gz" | sha256sum -c - \
    && tar -xzf cmake.tar.gz \
    && cd cmake-$cmakever \
    && ./bootstrap \
    && make \
    && make install 

# Delete the cmake installer it is no longer needed.
WORKDIR /
RUN rm -rf /usr/share/cmakeinst