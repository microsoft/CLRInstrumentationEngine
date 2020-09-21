# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# This image is based on the public Ubuntu 18.04 LTS image
FROM ubuntu:18.04

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        curl

# Install .NET Core 2.1 SDK
# https://www.microsoft.com/net/download/linux-package-manager/ubuntu18-04/sdk-2.1.403
RUN curl -SL --output packages-microsoft-prod.deb https://packages.microsoft.com/config/ubuntu/18.04/packages-microsoft-prod.deb \
    && dpkg -i packages-microsoft-prod.deb \
    && rm packages-microsoft-prod.deb

RUN apt-get install -y --no-install-recommends apt-transport-https \
    && apt-get update \
    && apt-get install -y --no-install-recommends dotnet-sdk-2.1

# Trigger first run experience by running arbitrary cmd to populate local package cache
RUN dotnet help

# CLR Instrumentation Engine Build Prerequisites
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        clang-3.9 \
        cmake \
        git \
        libunwind8-dev \
        libxml2-dev \
        make \
        uuid-dev