// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

.assembly extern mscorlib {}
.assembly extern System.Runtime { .publickeytoken = (B0 3F 5F 7F 11 D5 0A 3A ) }

.assembly InvalidCSharp { }

.class Program
{
    .method static void ValidateBoth() cil managed
    {
        .entrypoint
        .maxstack 8

        newobj     instance void RefFieldValidator::.ctor()
        callvirt   instance void Validator::Validate()

        newobj     instance void ByRefLikeValidator::.ctor()
        callvirt   instance void Validator::Validate()

        ret
    }
}
