#!/bin/bash

# Copyright (c) Microsoft Corporation. All rights reserved.
#

invoke_remove()
{
    docker inspect $ContainerName &>/dev/null
    if [ $? -eq 0 ]; then
        docker stop -t 0 $ContainerName
        docker rm $ContainerName
    fi
}

ContainerName=$1

invoke_remove

exit 0
