# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

FROM mcr.microsoft.com/dotnet-buildtools/prereqs:alpine-3.13-20211214164113-8a6f4f3

WORKDIR /

# Install .NET Core 2.1 SDK
# REFERENCE: https://github.com/dotnet/dotnet-docker/blob/master/2.1/sdk/alpine3.10/amd64/Dockerfile

# Disable the invariant mode (set in base image)
RUN apk add --no-cache icu-libs
ENV DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=false \
    LC_ALL=en_US.UTF-8 \
    LANG=en_US.UTF-8

# Install .NET Core SDK
ENV DOTNET_SDK_VERSION 2.1.805

RUN apk add --no-cache --virtual .build-deps \
        openssl \
    && wget -O dotnet.tar.gz https://dotnetcli.blob.core.windows.net/dotnet/Sdk/$DOTNET_SDK_VERSION/dotnet-sdk-$DOTNET_SDK_VERSION-linux-musl-x64.tar.gz \
    && dotnet_sha512='6eb76f405c339d8d662ad152b9cd9d0a77957f5538d44bbdc0d2c66f5923bd3064ea9fddcd20e2c2e4a7a051f97e0e99fa0a30ca1649f2c0c6ddbfaaf11831d2' \
    && echo "$dotnet_sha512  dotnet.tar.gz" | sha512sum -c - \
    && mkdir -p /usr/share/dotnet \
    && tar -C /usr/share/dotnet -xzf dotnet.tar.gz \
    && ln -s /usr/share/dotnet/dotnet /usr/bin/dotnet \
    && rm dotnet.tar.gz \
    && apk del .build-deps

# Enable correct mode for dotnet watch (only mode supported in a container)
ENV DOTNET_USE_POLLING_FILE_WATCHER=true \
    # Skip extraction of XML docs - generally not useful within an image/container - helps performance
    NUGET_XMLDOC_MODE=skip

# Trigger first run experience by running arbitrary cmd to populate local package cache
RUN dotnet help

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