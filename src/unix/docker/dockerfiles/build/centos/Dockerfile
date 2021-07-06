FROM centos:7

RUN yum install -y \
        centos-release-scl \
        epel-release \
    && \
    yum install -y \
        llvm-toolset-7-clang-4.0.1-1.el7 \
        libuuid-devel \
        libxml2-devel \
        libunwind-devel \
        jq \
        which \
        make \
        wget

RUN rpm -Uvh https://packages.microsoft.com/config/centos/7/packages-microsoft-prod.rpm && \
	yum install -y dotnet-sdk-2.1 \
	jq      

RUN yum install -y https://repo.ius.io/ius-release-el7.rpm && \
    yum install -y git224

RUN wget -O cmake-install.sh https://github.com/Kitware/CMake/releases/download/v3.20.2/cmake-3.20.2-Linux-x86_64.sh && \
    bash ./cmake-install.sh --skip-license --exclude-subdir --prefix=/usr/local && \
    rm ./cmake-install.sh        
