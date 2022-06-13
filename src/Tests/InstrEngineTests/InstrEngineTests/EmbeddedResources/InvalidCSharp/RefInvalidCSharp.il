// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

.assembly extern mscorlib {}
.assembly extern System.Runtime { .publickeytoken = (B0 3F 5F 7F 11 D5 0A 3A ) }

.assembly InvalidCSharp { }

.class private auto ansi sealed beforefieldinit Microsoft.CodeAnalysis.EmbeddedAttribute
       extends [System.Runtime]System.Attribute
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    .custom instance void Microsoft.CodeAnalysis.EmbeddedAttribute::.ctor() = ( 01 00 00 00 ) 

    .method public hidebysig specialname rtspecialname instance void .ctor() cil managed
    {
        .maxstack  8
        ldarg.0
        call       instance void [System.Runtime]System.Attribute::.ctor()
        nop
        ret
    }
}

.class private auto ansi sealed beforefieldinit System.Runtime.CompilerServices.NullableContextAttribute
       extends [System.Runtime]System.Attribute
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    .custom instance void Microsoft.CodeAnalysis.EmbeddedAttribute::.ctor() = ( 01 00 00 00 ) 
    .custom instance void [System.Runtime]System.AttributeUsageAttribute::.ctor(valuetype [System.Runtime]System.AttributeTargets) = ( 01 00 4C 14 00 00 02 00 54 02 0D 41 6C 6C 6F 77   // ..L.....T..Allow
                                                                                                                                        4D 75 6C 74 69 70 6C 65 00 54 02 09 49 6E 68 65   // Multiple.T..Inhe
                                                                                                                                        72 69 74 65 64 00 )                               // rited.

    .field public initonly uint8 Flag

    .method public hidebysig specialname rtspecialname instance void  .ctor(uint8 A_1) cil managed
    {
        .maxstack  8
        ldarg.0
        call       instance void [System.Runtime]System.Attribute::.ctor()
        nop
        ldarg.0
        ldarg.1
        stfld      uint8 System.Runtime.CompilerServices.NullableContextAttribute::Flag
        ret
    }
}

.class public auto ansi beforefieldinit RefFieldValidator
       extends Validator
{
    .custom instance void System.Runtime.CompilerServices.NullableContextAttribute::.ctor(uint8) = ( 01 00 01 00 00 ) 
    .custom instance void System.Runtime.CompilerServices.NullableAttribute::.ctor(uint8) = ( 01 00 00 00 00 ) 

    .class auto ansi serializable sealed nested public beforefieldinit '<>c'
       extends [System.Runtime]System.Object
    {
        .custom instance void [System.Runtime]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 

        .custom instance void System.Runtime.CompilerServices.NullableAttribute::.ctor(uint8) = ( 01 00 00 00 00 ) 

        .field public static initonly class RefFieldValidator/'<>c' '<>9'
        .field public static class [System.Runtime]System.Action Action_0
        .field public static class [System.Runtime]System.Action Action_1
        .field public static class [System.Runtime]System.Action Action_2
        .field public static class [System.Runtime]System.Action Action_3
        .field public static class [System.Runtime]System.Action Action_4
        .field public static class [System.Runtime]System.Action Action_5

        .method public hidebysig specialname rtspecialname static void .cctor() cil managed
        {
            .maxstack  8
            newobj     instance void RefFieldValidator/'<>c'::.ctor()
            stsfld     class RefFieldValidator/'<>c' RefFieldValidator/'<>c'::'<>9'
            ret
        }

        .method public hidebysig specialname rtspecialname instance void .ctor() cil managed
        {
            .maxstack  8
            ldarg.0
            call       instance void [System.Runtime]System.Object::.ctor()
            nop
            ret
        }

        .method public hidebysig instance void Validate_InvalidStructWithRefField_Fails() cil managed
        {
            .maxstack  1
            .locals init (class [System.Runtime]System.Type V_0)
            nop
            ldtoken    [InvalidCSharp]InvalidCSharp.InvalidStructWithRefField
            call       class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
            stloc.0
            ret
        }

        .method public hidebysig instance void Validate_InvalidRefFieldAlignment_Fails() cil managed
        {
            .maxstack  1
            .locals init (class [System.Runtime]System.Type V_0)
            nop
            ldtoken    [InvalidCSharp]InvalidCSharp.InvalidRefFieldAlignment
            call       class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
            stloc.0
            ret
        }

        .method public hidebysig instance void Validate_InvalidObjectRefRefFieldOverlap_Fails() cil managed
        {
            .maxstack  1
            .locals init (class [System.Runtime]System.Type V_0)
            nop
            ldtoken    [InvalidCSharp]InvalidCSharp.InvalidObjectRefRefFieldOverlap
            call       class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
            stloc.0
            ret
        }

        .method public hidebysig instance void Validate_IntPtrRefFieldOverlap_Fails() cil managed
        {
            .maxstack  2
            .locals init (valuetype [InvalidCSharp]InvalidCSharp.IntPtrRefFieldOverlap V_0,
                    valuetype [InvalidCSharp]InvalidCSharp.IntPtrRefFieldOverlap V_1)
            nop
            ldloca.s   V_1
            initobj    [InvalidCSharp]InvalidCSharp.IntPtrRefFieldOverlap
            ldloca.s   V_1
            ldsfld     native int [System.Runtime]System.IntPtr::Zero
            stfld      native int [InvalidCSharp]InvalidCSharp.IntPtrRefFieldOverlap::Field
            ldloc.1
            stloc.0
            ret
        }

        .method public hidebysig instance void Validate_IntPtrOverlapWithInnerFieldType_Fails() cil managed
        {
            .maxstack  2
            .locals init (valuetype [InvalidCSharp]InvalidCSharp.IntPtrOverlapWithInnerFieldType V_0,
                    valuetype [InvalidCSharp]InvalidCSharp.InnerValidByRef& V_1,
                    valuetype [InvalidCSharp]InvalidCSharp.IntPtrOverlapWithInnerFieldType V_2)
            nop
            ldloca.s   V_2
            initobj    [InvalidCSharp]InvalidCSharp.IntPtrOverlapWithInnerFieldType
            ldloca.s   V_2
            ldsfld     native int [System.Runtime]System.IntPtr::Zero
            stfld      native int [InvalidCSharp]InvalidCSharp.IntPtrOverlapWithInnerFieldType::Field
            ldloc.2
            stloc.0
            ldloca.s   V_0
            ldflda     valuetype [InvalidCSharp]InvalidCSharp.InnerValidByRef [InvalidCSharp]InvalidCSharp.IntPtrOverlapWithInnerFieldType::Invalid
            stloc.1
            ret
        }

        .method public hidebysig instance void Validate_RefFieldValidatorExplicit_Fails() cil managed
        {
            .maxstack  2
            .locals init (valuetype RefFieldValidator/Explicit V_0,
                    valuetype RefFieldValidator/Explicit V_1)
            nop
            ldloca.s   V_1
            initobj    RefFieldValidator/Explicit
            ldloca.s   V_1
            call       valuetype [System.Runtime]System.Guid [System.Runtime]System.Guid::NewGuid()
            stfld      valuetype [System.Runtime]System.Guid RefFieldValidator/Explicit::Guid
            ldloc.1
            stloc.0
            ret
        }
    }

    .class explicit ansi sealed nested private beforefieldinit Explicit extends [System.Runtime]System.ValueType
    {
        .custom instance void System.Runtime.CompilerServices.NullableContextAttribute::.ctor(uint8) = ( 01 00 00 00 00 ) 
        .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = ( 01 00 00 00 ) 
        .custom instance void [System.Runtime]System.ObsoleteAttribute::.ctor(string,
                                                                            bool) = ( 01 00 52 54 79 70 65 73 20 77 69 74 68 20 65 6D   // ..RTypes with em
                                                                                        62 65 64 64 65 64 20 72 65 66 65 72 65 6E 63 65   // bedded reference
                                                                                        73 20 61 72 65 20 6E 6F 74 20 73 75 70 70 6F 72   // s are not suppor
                                                                                        74 65 64 20 69 6E 20 74 68 69 73 20 76 65 72 73   // ted in this vers
                                                                                        69 6F 6E 20 6F 66 20 79 6F 75 72 20 63 6F 6D 70   // ion of your comp
                                                                                        69 6C 65 72 2E 01 00 00 )                         // iler....

        .field [0] public valuetype [System.Runtime]System.Span`1<uint8> Bytes
        .field [0] public valuetype [System.Runtime]System.Guid Guid
    }

    .method public hidebysig specialname rtspecialname instance void .ctor() cil managed
    {
        .maxstack  8
        ldarg.0
        call instance void Validator::.ctor()
        nop
        ret
    }

    .method public hidebysig virtual instance void Validate() cil managed
    {
        // Responsible for calling everything in RefFieldValidator.cs
        .maxstack  8

        ldarg.0
        call instance void RefFieldValidator::Validate_Create_RefField()

        ldarg.0
        call instance void RefFieldValidator::Validate_Create_RefStructField()

        ldarg.0
        call instance void RefFieldValidator::Validate_Create_TypedReferenceRefField()

        ldarg.0
        call instance void RefFieldValidator::Validate_Invalid_RefField_Fails()

        ldarg.0
        call instance void RefFieldValidator::Validate_RefStructWithRefField_Load()

        ret
    }

    .method private hidebysig instance void  Validate_Create_TypedReferenceRefField() cil managed
    {
        .maxstack  3
        .locals init (class RefFieldValidator V_0)
        nop
        newobj     instance void RefFieldValidator::.ctor()
        stloc.0
        ldarg.0
        ldloc.0
        ldc.i4.1
        call       instance void RefFieldValidator::Create_TypedReferenceRefField_Worker(class RefFieldValidator, int32)
        nop
        ret
    }

    .method private hidebysig instance void  Validate_Create_RefStructField() cil managed
    {
        .maxstack  3
        .locals init (string V_0)
        nop
        ldarg.0
        ldloc.0
        ldc.i4.1
        call       instance void RefFieldValidator::Create_RefStructField_Worker(string, int32)
        nop
        ret
    }

    .method private hidebysig instance void  Validate_Create_RefField() cil managed
    {
        .maxstack  3
        .locals init (string V_0)
        nop
        ldarg.0
        ldloc.0
        ldc.i4.1
        call       instance void RefFieldValidator::Create_RefField_Worker(string, int32)
        nop
        ret
    }

    .method private hidebysig instance void  Create_TypedReferenceRefField_Worker(class RefFieldValidator v, int32 depth) cil managed noinlining
    {
        .maxstack  4
        .locals init (valuetype [InvalidCSharp]InvalidCSharp.WithTypedReferenceField`1<class RefFieldValidator> V_0,
                bool V_1)
        nop
        ldarg.2
        ldc.i4.5
        ceq
        stloc.1
        ldloc.1
        brfalse.s  BRANCH
        nop
        br.s       RETURN
    BRANCH:
        ldarga.s   v
        newobj     instance void valuetype [InvalidCSharp]InvalidCSharp.WithTypedReferenceField`1<class RefFieldValidator>::.ctor(!0&)
        stloc.0
        ldarg.0
        ldarg.1
        ldarg.2
        ldc.i4.1
        add
        call       instance void RefFieldValidator::Create_TypedReferenceRefField_Worker(class RefFieldValidator, int32)
        nop
        ldloca.s   V_0
        call       instance class [System.Runtime]System.Type valuetype [InvalidCSharp]InvalidCSharp.WithTypedReferenceField`1<class RefFieldValidator>::GetFieldType()
        pop
        ldloca.s   V_0
        ldarg.1
        call       instance bool valuetype [InvalidCSharp]InvalidCSharp.WithTypedReferenceField`1<class RefFieldValidator>::ConfirmFieldInstance(!0)
        pop
        RETURN:  ret
    }

    .method private hidebysig instance void  Create_RefStructField_Worker(string str, int32 depth) cil managed noinlining
    {
        .maxstack  5
        .locals init (valuetype [InvalidCSharp]InvalidCSharp.WithRefField V_0,
                valuetype [InvalidCSharp]InvalidCSharp.WithRefStructField V_1,
                bool V_2,
                valuetype [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler V_3)
        nop
        ldarg.2
        ldc.i4.5
        ceq
        stloc.2
        ldloc.2
        brfalse.s  BRANCH
        nop
        br.s       RETURN
    BRANCH:
        ldarga.s   str
        newobj     instance void [InvalidCSharp]InvalidCSharp.WithRefField::.ctor(string&)
        stloc.0
        ldloca.s   V_0
        newobj     instance void [InvalidCSharp]InvalidCSharp.WithRefStructField::.ctor(valuetype [InvalidCSharp]InvalidCSharp.WithRefField&)
        stloc.1
        ldarg.0
        ldarg.1
        ldloca.s   V_3
        ldc.i4.1
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_3
        ldstr      " "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_3
        ldarg.2
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<int32>(!!0)
        nop
        ldloca.s   V_3
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       string [System.Runtime]System.String::Concat(string, string)
        ldarg.2
        ldc.i4.1
        add
        call       instance void RefFieldValidator::Create_RefStructField_Worker(string, int32)
        nop
        ldloca.s   V_1
        ldloca.s   V_0
        call       instance bool [InvalidCSharp]InvalidCSharp.WithRefStructField::ConfirmFieldInstance(valuetype [InvalidCSharp]InvalidCSharp.WithRefField&)
        pop
    RETURN:
        ret
    }

    .method private hidebysig instance void  Create_RefField_Worker(string str, int32 depth) cil managed noinlining
    {
        .maxstack  5
        .locals init (valuetype [InvalidCSharp]InvalidCSharp.WithRefField V_0,
                string V_1,
                bool V_2,
                valuetype [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler V_3)
        nop
        ldarg.2
        ldc.i4.5
        ceq
        stloc.2
        ldloc.2
        brfalse.s  BRANCH
        nop
        br.s       RETURN
    BRANCH:
        ldarga.s   str
        newobj     instance void [InvalidCSharp]InvalidCSharp.WithRefField::.ctor(string&)
        stloc.0
        ldarg.1
        call       valuetype [System.Runtime]System.ReadOnlySpan`1<char> [System.Runtime]System.String::op_Implicit(string)
        newobj     instance void [System.Runtime]System.String::.ctor(valuetype [System.Runtime]System.ReadOnlySpan`1<char>)
        stloc.1
        ldarg.0
        ldarg.1
        ldloca.s   V_3
        ldc.i4.1
        ldc.i4.1
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        ldloca.s   V_3
        ldstr      " "
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        nop
        ldloca.s   V_3
        ldarg.2
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<int32>(!!0)
        nop
        ldloca.s   V_3
        call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        call       string [System.Runtime]System.String::Concat(string, string)
        ldarg.2
        ldc.i4.1
        add
        call       instance void RefFieldValidator::Create_RefField_Worker(string, int32)
        nop
        ldloca.s   V_0
        ldloc.1
        call       instance bool [InvalidCSharp]InvalidCSharp.WithRefField::ConfirmFieldInstance(string)
        pop
        ldloca.s   V_0
        ldarg.1
        call       instance bool [InvalidCSharp]InvalidCSharp.WithRefField::ConfirmFieldInstance(string)
        pop
    RETURN:
        ret
    }

    .method public hidebysig instance void  Validate_RefStructWithRefField_Load() cil managed
    {
        .maxstack  8
        .locals init (class [System.Runtime]System.Type V_0)
        nop
        ldtoken    [InvalidCSharp]InvalidCSharp.WithRefField
        call       class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
        stloc.0
        ret
    }

    .method public hidebysig instance void  Validate_Invalid_RefField_Fails() cil managed
    {
        .maxstack  3

        nop
        ldarg.0
        ldsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_0
        dup
        brtrue.s   VALIDATE1
        pop
        ldsfld     class RefFieldValidator/'<>c' RefFieldValidator/'<>c'::'<>9'
        ldftn      instance void RefFieldValidator/'<>c'::Validate_InvalidStructWithRefField_Fails()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_0
    VALIDATE1:
        call instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        pop
        ldarg.0
        ldsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_1
        dup
        brtrue.s   VALIDATE2
        pop
        ldsfld     class RefFieldValidator/'<>c' RefFieldValidator/'<>c'::'<>9'
        ldftn      instance void RefFieldValidator/'<>c'::Validate_InvalidRefFieldAlignment_Fails()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_1
    VALIDATE2:
        call instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        pop
        ldarg.0
        ldsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_2
        dup
        brtrue.s   VALIDATE3
        pop
        ldsfld     class RefFieldValidator/'<>c' RefFieldValidator/'<>c'::'<>9'
        ldftn      instance void RefFieldValidator/'<>c'::Validate_InvalidObjectRefRefFieldOverlap_Fails()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_2
    VALIDATE3:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        pop
        ldarg.0
        ldsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_3
        dup
        brtrue.s   VALIDATE4
        pop
        ldsfld     class RefFieldValidator/'<>c' RefFieldValidator/'<>c'::'<>9'
        ldftn      instance void RefFieldValidator/'<>c'::Validate_IntPtrRefFieldOverlap_Fails()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_3
    VALIDATE4:
        call instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        pop
        ldarg.0
        ldsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_4
        dup
        brtrue.s   VALIDATE5
        pop
        ldsfld     class RefFieldValidator/'<>c' RefFieldValidator/'<>c'::'<>9'
        ldftn      instance void RefFieldValidator/'<>c'::Validate_IntPtrOverlapWithInnerFieldType_Fails()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_4
    VALIDATE5:
        call instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        pop
        ldarg.0
        ldsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_5
        dup
        brtrue.s   VALIDATE6
        pop
        ldsfld     class RefFieldValidator/'<>c' RefFieldValidator/'<>c'::'<>9'
        ldftn      instance void RefFieldValidator/'<>c'::Validate_RefFieldValidatorExplicit_Fails()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action RefFieldValidator/'<>c'::Action_5
    VALIDATE6:
        call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        pop

        ret
    }
}

.class private auto ansi sealed beforefieldinit System.Runtime.CompilerServices.NullableAttribute extends [System.Runtime]System.Attribute
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    .custom instance void Microsoft.CodeAnalysis.EmbeddedAttribute::.ctor() = ( 01 00 00 00 ) 
    .custom instance void [System.Runtime]System.AttributeUsageAttribute::.ctor(valuetype [System.Runtime]System.AttributeTargets) = ( 01 00 84 6B 00 00 02 00 54 02 0D 41 6C 6C 6F 77
                                                                                                                                     4D 75 6C 74 69 70 6C 65 00 54 02 09 49 6E 68 65
                                                                                                                                     72 69 74 65 64 00 )

    .field public initonly uint8[] NullableFlags

    .method public hidebysig specialname rtspecialname 
        instance void  .ctor(uint8[] A_1) cil managed
    {
        .maxstack  8
        ldarg.0
        call       instance void [System.Runtime]System.Attribute::.ctor()
        nop
        ldarg.0
        ldarg.1
        stfld      uint8[] System.Runtime.CompilerServices.NullableAttribute::NullableFlags
        ret
    }

    .method public hidebysig specialname rtspecialname 
        instance void  .ctor(uint8 A_1) cil managed
    {
        .maxstack  8
        ldarg.0
        call       instance void [System.Runtime]System.Attribute::.ctor()
        nop
        ldarg.0
        ldc.i4.1
        newarr     [System.Runtime]System.Byte
        dup
        ldc.i4.0
        ldarg.1
        stelem.i1
        stfld      uint8[] System.Runtime.CompilerServices.NullableAttribute::NullableFlags
        ret
    }
}

.class public abstract auto ansi beforefieldinit Validator extends [System.Runtime]System.Object
{
    .method family hidebysig specialname rtspecialname instance void  .ctor() cil managed
    {
        .maxstack  8
        ldarg.0
        call instance void [System.Runtime]System.Object::.ctor()
        nop
        ret
    }

    .method public hidebysig newslot abstract virtual instance void Validate() cil managed
    {
    }

    .method family hidebysig instance bool ValidateException<([System.Runtime]System.Exception) T>(class [System.Runtime]System.Action a) cil managed
    {
        .custom instance void System.Runtime.CompilerServices.NullableContextAttribute::.ctor(uint8) = ( 01 00 01 00 00 ) 
        .param type T 
        .custom instance void System.Runtime.CompilerServices.NullableAttribute::.ctor(uint8) = ( 01 00 00 00 00 ) 

        .maxstack  1
        .locals init (bool V_0)
        nop
        .try
        {
            nop
            ldarg.1
            callvirt instance void [System.Runtime]System.Action::Invoke()
            nop
            nop
            leave.s BREAK
        }
        catch !!T 
        {
            pop
            nop
            ldc.i4.1
            stloc V_0
            leave.s RETURN
        }

    BREAK:
        ldc.i4.0
        stloc V_0
        br.s RETURN

    RETURN:
        ldloc V_0
        ret
    }
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.InvalidStructWithRefField
    extends [System.Runtime]System.ValueType
{
    // Type requires IsByRefLikeAttribute to be valid.
    .field public string& Invalid

    .method public hidebysig specialname rtspecialname
        instance void TestFieldInstance (
            string&
        ) cil managed
    {
        ldarg.0
        ldarg.1
        stfld string& InvalidCSharp.WithRefField::Invalid
        ret
    }
}

.class public explicit ansi sealed beforefieldinit InvalidCSharp.InvalidRefFieldAlignment
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field [0] public int16 Field
    .field [2] public int32& Invalid
}

.class public explicit ansi sealed beforefieldinit InvalidCSharp.InvalidObjectRefRefFieldOverlap
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field [0] public object Field
    .field [0] public int32& Invalid
}

// This is invalid metadata and is unable to be loaded.
// -  [field sig] (0x80131815 (VER_E_FIELD_SIG))
//
// .class public sequential ansi sealed beforefieldinit InvalidCSharp.InvalidStructWithStaticRefField
//     extends [System.Runtime]System.ValueType
// {
//     .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
//         01 00 00 00
//     )
//     .field public static string& Invalid
// }

.class public explicit ansi sealed beforefieldinit InvalidCSharp.IntPtrRefFieldOverlap
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field [0] public native int Field
    .field [0] public int32& Invalid
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.InnerValidByRef
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field public int32& Field
    .field public int32 Size
}

.class public explicit ansi sealed beforefieldinit InvalidCSharp.IntPtrOverlapWithInnerFieldType
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field [0] public native int Field
    // This field's valid Type would illegally overlap this type's first field using a precise GC.
    .field [0] public valuetype InvalidCSharp.InnerValidByRef Invalid
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.WithRefField
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field public string& Str

    .method public hidebysig specialname rtspecialname
        instance void .ctor (
            string&
        ) cil managed
    {
        ldarg.0
        ldarg.1
        stfld string& InvalidCSharp.WithRefField::Str
        ret
    }

    .method public hidebysig
        instance bool ConfirmFieldInstance (
            string
        ) cil managed
    {
        ldarg.0
        ldfld string& InvalidCSharp.WithRefField::Str
        ldind.ref
        ldarg.1
        ceq
        ret
    }
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.WithRefStructField
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field public valuetype InvalidCSharp.WithRefField& Field

    .method public hidebysig specialname rtspecialname
        instance void .ctor (
            valuetype InvalidCSharp.WithRefField&
        ) cil managed
    {
        ldarg.0
        ldarg.1
        stfld valuetype InvalidCSharp.WithRefField& InvalidCSharp.WithRefStructField::Field
        ret
    }

    .method public hidebysig
        instance bool ConfirmFieldInstance (
            valuetype InvalidCSharp.WithRefField&
        ) cil managed
    {
        ldarg.0
        ldfld valuetype InvalidCSharp.WithRefField& InvalidCSharp.WithRefStructField::Field
        ldind.ref
        ldarg.1
        ldind.ref
        ceq
        ret
    }
}

.class public sequential ansi sealed beforefieldinit InvalidCSharp.WithTypedReferenceField`1<T>
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field public typedref Field

    .method public hidebysig specialname rtspecialname
        instance void .ctor (
            !T&
        ) cil managed
    {
        ldarg.0
        ldarg.1
        mkrefany !T
        stfld typedref valuetype InvalidCSharp.WithTypedReferenceField`1<!T>::Field
        ret
    }

    .method public hidebysig
        instance class [System.Runtime]System.Type GetFieldType () cil managed
    {
        ldarg.0
        ldfld typedref valuetype InvalidCSharp.WithTypedReferenceField`1<!T>::Field
        refanytype
        call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle )
        ret
    }

    .method public hidebysig
        instance bool ConfirmFieldInstance (
            !T
        ) cil managed
    {
        ldarg.0
        ldfld typedref valuetype InvalidCSharp.WithTypedReferenceField`1<!T>::Field
        refanyval !T
        ldind.ref
        ldarg.1
        ceq
        ret
    }
}
