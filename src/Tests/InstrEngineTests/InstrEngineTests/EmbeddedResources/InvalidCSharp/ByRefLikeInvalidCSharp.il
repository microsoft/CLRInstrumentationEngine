// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

.assembly extern System.Console { .publickeytoken = (B0 3F 5F 7F 11 D5 0A 3A ) }
.assembly extern System.Runtime { .publickeytoken = (B0 3F 5F 7F 11 D5 0A 3A ) }

.assembly InvalidCSharp { }

.class public auto ansi beforefieldinit ByRefLikeValidator extends Validator
{
    .method public hidebysig specialname rtspecialname instance void .ctor() cil managed
    {
        .maxstack  8
        ldarg.0
        call       instance void Validator::.ctor()
        nop
        ret
    }

    .method public hidebysig virtual instance void Validate() cil managed
    {
        .maxstack  4
        .locals init (valuetype [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler V_0)

        ldstr "BEGIN - Validating ByRefLikeInvalidCSharp..."
        call void [mscorlib]System.Console::WriteLine(string)

        /*
        ldloca.s   V_0
        call       bool [InvalidCSharp]Exec::BoxIsinstBranch()
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        */

        nop
        ldloca.s   V_0
        ldc.i4.s   18
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- GenericClass: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_0'
        dup
        brtrue.s   BRANCH_1
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_GenericClass_Invalid()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_0'
    BRANCH_1:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   22
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- GenericInterface: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_1'
        dup
        brtrue.s   BRANCH_2
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_GenericInterface_Invalid()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_1'
    BRANCH_2:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   22
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- GenericValueType: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_2'
        dup
        brtrue.s   BRANCH_3
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_GenericValueType_Invalid()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_2'
    BRANCH_3:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   22
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- GenericByRefLike: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_3'
        dup
        brtrue.s   BRANCH_4
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_GenericByRefLike_Invalid()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_3'
    BRANCH_4:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldstr      "Validate Casting:"
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   24
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- InstanceOf(object): "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        newobj     instance void [System.Runtime]System.Object::.ctor()
        call       bool [InvalidCSharp]Exec::InstanceOfT(object)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   22
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Cast To (object): "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_4'
        dup
        brtrue.s   BRANCH_5
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_CastToT()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_4'
    BRANCH_5:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.InvalidCastException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   23
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Unbox To (object): "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_5'
        dup
        brtrue.s   BRANCH_6
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_UnboxToT()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_5'
    BRANCH_6:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.InvalidCastException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldstr      "Validate Opcodes"
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   15
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Unbox Any: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        call       bool [InvalidCSharp]Exec::BoxUnboxAny()
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   16
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Box Branch: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        call       bool [InvalidCSharp]Exec::BoxBranch()
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   18
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Boxed Is Any: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        call       bool [InvalidCSharp]Exec::BoxIsinstUnboxAny()
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   21
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Boxed Is Branch: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        call       bool [InvalidCSharp]Exec::BoxIsinstBranch()
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldstr      "Validate Invalid Opcodes"
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   20
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Allocate Array: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_6'
        dup
        brtrue.s   BRANCH_7
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_AllocArrayOfT_Invalid()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_6'
    BRANCH_7:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   38
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Allocate Multi Dimensional Array: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_7'
        dup
        brtrue.s   BRANCH_8
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_AllocMultiDimArrayOfT_Invalid()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_7'
    BRANCH_8:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   37
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        IL_040d:  ldloca.s   V_0
        IL_040f:  ldstr      " -- Generic class with static field: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_8'
        dup
        brtrue.s   BRANCH_9
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_GenericClassWithStaticField_Invalid()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_8'
    BRANCH_9:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   19
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Box as object: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldarg.0
        ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_9'
        dup
        brtrue.s   BRANCH_10
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::Validate_BoxAsObject()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_9'
    BRANCH_10:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.InvalidProgramException>(class [System.Runtime]System.Action)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldstr      "Validate Inlining:"
        call       void [System.Console]System.Console::WriteLine(string)
        nop
        ldloca.s   V_0
        ldc.i4.s   20
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_0
        ldstr      " -- Avoid Inlining: "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_0
        ldc.i4.0
        call       bool [InvalidCSharp]Exec::TypeLoadExceptionAvoidsInline(bool)
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        nop
        ldloca.s   V_0
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       void [System.Console]System.Console::WriteLine(string)
        nop

        ldstr "END - Validating ByRefLikeInvalidCSharp..."
        call void [mscorlib]System.Console::WriteLine(string)

        ret
    }

    .class auto ansi serializable sealed nested private beforefieldinit '<>c' extends [System.Runtime]System.Object
    {
        .custom instance void [System.Runtime]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 

        .field public static initonly class ByRefLikeValidator/'<>c' '<>9'
        .field public static class [System.Runtime]System.Action '<>9__0_0'
        .field public static class [System.Runtime]System.Action '<>9__0_1'
        .field public static class [System.Runtime]System.Action '<>9__0_2'
        .field public static class [System.Runtime]System.Action '<>9__0_3'
        .field public static class [System.Runtime]System.Action '<>9__0_4'
        .field public static class [System.Runtime]System.Action '<>9__0_5'
        .field public static class [System.Runtime]System.Action '<>9__0_6'
        .field public static class [System.Runtime]System.Action '<>9__0_7'
        .field public static class [System.Runtime]System.Action '<>9__0_8'
        .field public static class [System.Runtime]System.Action '<>9__0_9'

        .method private hidebysig specialname rtspecialname static 
        void  .cctor() cil managed
        {
            .maxstack  8
            newobj     instance void ByRefLikeValidator/'<>c'::.ctor()
            stsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
            ret
        }

        .method public hidebysig specialname rtspecialname instance void  .ctor() cil managed
        {
            .maxstack  8
            ldarg.0
            call       instance void [System.Runtime]System.Object::.ctor()
            nop
            ret
        }

        .method assembly hidebysig instance void Validate_GenericClass_Invalid() cil managed
        {
            .maxstack  8
            nop
            call       string [InvalidCSharp]Exec::GenericClass_Invalid()
            pop
            ret
        }

        .method assembly hidebysig instance void Validate_GenericInterface_Invalid() cil managed
        {
            .maxstack  8
            nop
            call       string [InvalidCSharp]Exec::GenericInterface_Invalid()
            pop
            ret
        }

        .method assembly hidebysig instance void Validate_GenericValueType_Invalid() cil managed
        {
            .maxstack  8
            nop
            call       string [InvalidCSharp]Exec::GenericValueType_Invalid()
            pop
            ret
        }

        .method assembly hidebysig instance void Validate_GenericByRefLike_Invalid() cil managed
        {
            .maxstack  8
            nop
            call       string [InvalidCSharp]Exec::GenericByRefLike_Invalid()
            pop
            ret
        }

        .method assembly hidebysig instance void Validate_CastToT() cil managed
        {
            .maxstack  8
            nop
            newobj     instance void [System.Runtime]System.Object::.ctor()
            call       void [InvalidCSharp]Exec::CastToT(object)
            nop
            ret
        }

        .method assembly hidebysig instance void Validate_UnboxToT() cil managed
        {
            .maxstack  8
            nop
            newobj     instance void [System.Runtime]System.Object::.ctor()
            call       void [InvalidCSharp]Exec::UnboxToT(object)
            nop
            ret
        }

        .method assembly hidebysig instance void Validate_AllocArrayOfT_Invalid() cil managed
        {
            .maxstack  8
            nop
            call       void [InvalidCSharp]Exec::AllocArrayOfT_Invalid()
            nop
            ret
        }

        .method assembly hidebysig instance void Validate_AllocMultiDimArrayOfT_Invalid() cil managed
        {
            .maxstack  8
            nop
            call       void [InvalidCSharp]Exec::AllocMultiDimArrayOfT_Invalid()
            nop
            ret
        }

        .method assembly hidebysig instance void Validate_GenericClassWithStaticField_Invalid() cil managed
        {
            .maxstack  8
            nop
            call       string [InvalidCSharp]Exec::GenericClassWithStaticField_Invalid()
            pop
            ret
        }

        .method assembly hidebysig instance void Validate_BoxAsObject() cil managed
        {
            .maxstack  8
            nop
            call       object [InvalidCSharp]Exec::BoxAsObject()
            pop
            ret
        }
    }
}


//
// Begin invalid
//

.class public sequential ansi sealed beforefieldinit InvalidCSharp.GenericClass_Invalid`1<T>
    extends [System.Runtime]System.Object
{
    .method public hidebysig specialname rtspecialname
        instance void .ctor () cil managed
    {
        ldarg.0
        call instance void [System.Runtime]System.Object::.ctor()
        ret
    }
}

.class interface public auto ansi abstract InvalidCSharp.GenericInterface_Invalid`1<T>
{
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.GenericValueType_Invalid`1<T>
    extends [System.Runtime]System.ValueType
{
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.GenericByRefLike_Invalid`1<T>
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
}











//
// End invalid
//

.class public sequential ansi sealed beforefieldinit InvalidCSharp.GenericClass_Over`1<byreflike T>
    extends [System.Runtime]System.Object
{
    .method public hidebysig specialname rtspecialname
        instance void .ctor () cil managed
    {
        ldarg.0
        call instance void [System.Runtime]System.Object::.ctor()
        ret
    }
}

.class interface public auto ansi abstract InvalidCSharp.GenericInterface_Over`1<byreflike T>
{
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.GenericValueType_Over`1<byreflike T>
    extends [System.Runtime]System.ValueType
{
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.GenericByRefLike_Over`1<byreflike T>
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )

    .field public !T Field

    .method public hidebysig
        instance object BoxAsObject(!T) cil managed
    {
        ldarg.1
        box !T
        ret
    }

    .method public hidebysig
        instance bool BoxUnboxAny(!T) cil managed
    {
        ldarg.1
        // Begin sequence
            box !T
            unbox.any !T
        // End sequence
        pop
        ldc.i4.1
        ret
    }

    .method public hidebysig
        instance bool BoxBranch(!T) cil managed
    {
        ldarg.1
        // Begin sequence
            box !T
            brfalse.s NEXT_1
        // End sequence
        NEXT_1:

        ldarg.1
        // Begin sequence
            box !T
            brfalse NEXT_2
        // End sequence
        NEXT_2:

        ldarg.1
        // Begin sequence
            box !T
            brtrue.s NEXT_3
        // End sequence
        NEXT_3:

        ldarg.1
        // Begin sequence
            box !T
            brtrue NEXT_4
        // End sequence
        NEXT_4:

        ldc.i4.1
        ret
    }

    .method public hidebysig
        instance bool BoxBranch_WithSideEffects(!T&) cil managed
    {
        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            brfalse.s NEXT_1
        // End sequence
        NEXT_1:

        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            brfalse NEXT_2
        // End sequence
        NEXT_2:

        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            brtrue.s NEXT_3
        // End sequence
        NEXT_3:

        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            brtrue NEXT_4
        // End sequence
        NEXT_4:

        ldc.i4.1
        ret
    }

    .method public hidebysig
        instance bool BoxIsinstUnboxAny(!T) cil managed
    {
        ldarg.1
        // Begin sequence
            box !T
            isinst ByRefLikeType
            unbox.any !T
        // End sequence
        pop
        ldc.i4.1
        ret
    }

    .method public hidebysig
        instance bool BoxIsinstBranch(!T) cil managed
    {
        ldarg.1
        // Begin sequence
            box !T
            isinst ByRefLikeType
            brfalse.s NEXT_1
        // End sequence
        NEXT_1:

        ldarg.1
        // Begin sequence
            box !T
            isinst ByRefLikeType
            brfalse NEXT_2
        // End sequence
        NEXT_2:

        ldarg.1
        // Begin sequence
            box !T
            isinst ByRefLikeType
            brtrue.s NEXT_3
        // End sequence
        NEXT_3:

        ldarg.1
        // Begin sequence
            box !T
            isinst ByRefLikeType
            brtrue NEXT_4
        // End sequence
        NEXT_4:

        ldc.i4.1
        ret
    }

    .method public hidebysig
        instance bool BoxIsinstBranch_WithSideEffects(!T&) cil managed
    {
        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            isinst ByRefLikeType
            brfalse.s NEXT_1
        // End sequence
        NEXT_1:

        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            isinst ByRefLikeType
            brfalse NEXT_2
        // End sequence
        NEXT_2:

        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            isinst ByRefLikeType
            brtrue.s NEXT_3
        // End sequence
        NEXT_3:

        ldarg.1
        // Begin sequence
            ldobj !T // Side-effect
            box !T
            isinst ByRefLikeType
            brtrue NEXT_4
        // End sequence
        NEXT_4:

        ldc.i4.1
        ret
    }

    .method public hidebysig
        instance bool AllocArrayOfT() cil managed aggressiveinlining
    {
        ldc.i4.1
        newarr !T
        ldnull
        cgt.un
        ret
    }

    .method public hidebysig
        instance bool AllocMultiDimArrayOfT() cil managed
    {
        ldc.i4.1
        ldc.i4.1
        newobj instance void !T[0..., 0...]::.ctor(int32, int32)
        ldnull
        cgt.un
        ret
    }

    .method public hidebysig
        instance bool InstanceOfT(
            object o
        ) cil managed
    {
        ldarg.1
        isinst !T
        ldnull
        cgt.un
        ret
    }

    .method public hidebysig
        instance void CastToT(
            object o
        ) cil managed
    {
        ldarg.1
        castclass !T
        pop
        ret
    }

    .method public hidebysig
        instance void UnboxToT(
            object o
        ) cil managed
    {
        ldarg.1
        unbox.any !T
        pop
        ret
    }
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.GenericClass_WithStaticField`1<byreflike T>
    extends [System.Runtime]System.Object
{
    .method public hidebysig specialname rtspecialname
        instance void .ctor () cil managed
    {
        .locals init (
            [0] !T,
            [1] !T
        )

        ldarg.0
        call instance void [System.Runtime]System.Object::.ctor()

        ldloca.s 0
        initobj !T

        ldloc.0
        stsfld !0 class InvalidCSharp.GenericClass_WithStaticField`1<!T>::StaticField
        ldsfld !0 class InvalidCSharp.GenericClass_WithStaticField`1<!T>::StaticField
        stloc.1
        ret
    }

    .field public static !T StaticField
}

.class public sequential ansi sealed beforefieldinit ByRefLikeType
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
}

.class public auto ansi abstract sealed beforefieldinit Exec
    extends [System.Runtime]System.Object
{
    .method public hidebysig static
        string GenericClass() cil managed
    {
        ldstr "Generic Class"
        call void [mscorlib]System.Console::WriteLine(string)

        ldtoken class InvalidCSharp.GenericClass_Over`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }
    .method public hidebysig static
        string GenericInterface() cil managed
    {
        ldtoken class InvalidCSharp.GenericInterface_Over`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }
    .method public hidebysig static
        string GenericValueType() cil managed
    {
        ldtoken valuetype InvalidCSharp.GenericValueType_Over`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }
    .method public hidebysig static
        string GenericByRefLike() cil managed
    {
        ldtoken valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }

    .method public hidebysig static
        string GenericClass_Invalid() cil managed
    {
        ldtoken class InvalidCSharp.GenericClass_Invalid`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }
    .method public hidebysig static
        string GenericInterface_Invalid() cil managed
    {
        ldtoken class InvalidCSharp.GenericInterface_Invalid`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }
    .method public hidebysig static
        string GenericValueType_Invalid() cil managed
    {
        ldtoken valuetype InvalidCSharp.GenericValueType_Invalid`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }
    .method public hidebysig static
        string GenericByRefLike_Invalid() cil managed
    {
        ldtoken valuetype InvalidCSharp.GenericByRefLike_Invalid`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }

    .method public hidebysig static
        object BoxAsObject() cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        ldloc 0
        ldfld !0 valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::Field
        call instance object valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::BoxAsObject(!0)
        ret
    }

    .method public hidebysig static
        bool BoxUnboxAny() cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        ldloc 0
        ldfld !0 valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::Field
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::BoxUnboxAny(!0)
        ret
    }

    .method public hidebysig static
        bool BoxBranch() cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>,
            [1] bool
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>

        ldloca.s 0
        ldloc 0
        ldfld !0 valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::Field
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::BoxBranch(!0)
        pop

        ldloca.s 0
        ldloca.s 0
        ldflda !0 valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::Field
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::BoxBranch_WithSideEffects(!0&)

        ret
    }

    .method public hidebysig static
        bool BoxIsinstUnboxAny() cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        ldloc 0
        ldfld !0 valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::Field
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::BoxIsinstUnboxAny(!0)
        ret
    }

    .method public hidebysig static
        bool BoxIsinstBranch() cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>

        ldloca.s 0
        ldloc 0
        ldfld !0 valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::Field
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::BoxIsinstBranch(!0)
        pop

        ldloca.s 0
        ldloca.s 0
        ldflda !0 valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::Field
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::BoxIsinstBranch_WithSideEffects(!0&)

        ret
    }

    .method public hidebysig static
        void AllocArrayOfT_Invalid() cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::AllocArrayOfT()
        pop
        ret
    }

    .method public hidebysig static
        void AllocMultiDimArrayOfT_Invalid() cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::AllocMultiDimArrayOfT()
        pop
        ret
    }

    .method public hidebysig static
        string GenericClassWithStaticField_Invalid() cil managed
    {
        ldtoken class InvalidCSharp.GenericClass_WithStaticField`1<valuetype ByRefLikeType>
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        callvirt instance string [System.Runtime]System.Object::ToString()
        ret
    }

    .method public hidebysig static
        bool InstanceOfT(object) cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        ldarg.0
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::InstanceOfT(object)
        ret
    }

    .method public hidebysig static
        void CastToT(object) cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        ldarg.0
        call instance void valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::CastToT(object)
        ret
    }

    .method public hidebysig static
        void UnboxToT(object) cil managed
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldloca.s 0
        ldarg.0
        call instance void valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::UnboxToT(object)
        ret
    }

    .method public hidebysig static
        bool TypeLoadExceptionAvoidsInline(bool) cil managed noinlining
    {
        .locals init (
            [0] valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        )

        ldloca.s 0
        initobj valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>
        ldarg.0
        brfalse.s DONTCALL

        ldloca.s 0
        call instance bool valuetype InvalidCSharp.GenericByRefLike_Over`1<valuetype ByRefLikeType>::AllocArrayOfT()
        br.s DONE

        // Negate the input and return
        DONTCALL: ldarg.0
        ldc.i4.0
        ceq

        DONE: ret
    }
}