# This image is based on the public Ubuntu 16.04 LTS image
FROM ubuntu:16.04

# Get Mono
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF
RUN echo "deb http://download.mono-project.com/repo/debian wheezy main" | tee /etc/apt/sources.list.d/mono-xamarin.list
RUN apt-get update
RUN apt-get install -y mono-devel
RUN apt-get install -y mono-complete

# Dependencies
RUN apt-get install -y cmake clang-3.5 libunwind8 libunwind8-dev uuid-dev

# Add git
RUN apt-get install -y git

# Add unzip
RUN apt-get install -y unzip

# Add libxml2
RUN apt-get install -y libxml2-dev

# Add curl
RUN apt-get install libcurl3
