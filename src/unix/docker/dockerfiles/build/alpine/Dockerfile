# This is the base image that coreclr uses to build its alpine/linux-musl release
# REFERENCE: https://github.com/dotnet/coreclr/blob/v2.1.0-preview2-26406-04/buildpipeline/pipelines.json
FROM microsoft/dotnet-buildtools-prereqs:alpine-3.6-3148f11-20171119021156

# Add Mono
# We need mono to get the package for the Core CLR PAL. We're sticking with Mono over .NET CLI for now
# as .NET CLI is not guaranteed to be available on all new platforms (it wasn't on linux-arm while that work was happening)
# and there is no `dotnet nuget` command to acquire a single package
RUN apk add --no-cache mono --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing && \
    apk add --no-cache --virtual=.build-dependencies ca-certificates && \
    cert-sync /etc/ssl/certs/ca-certificates.crt && \
    apk del .build-dependencies

RUN apk update
RUN apk add libxml2-dev
