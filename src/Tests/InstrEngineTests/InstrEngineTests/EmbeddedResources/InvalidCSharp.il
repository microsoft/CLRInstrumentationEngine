// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

.assembly extern mscorlib {}
.assembly extern System.Runtime { .publickeytoken = (B0 3F 5F 7F 11 D5 0A 3A ) }

.assembly InvalidCSharp { }

.method static void Main() cil managed
{
    .maxstack 8
    .entrypoint

    .locals init (int32 currentErrorCount, int32 expectedErrorCount)

    ldc.i4 5 // Come fix this for number of expected errors
    stloc expectedErrorCount

    ldc.i4 0
    stloc currentErrorCount

    //////////////////////////////////////
    // InvalidStructWithRefField /////////
    //////////////////////////////////////
    .try
    {
        call void Test_InvalidStructWithRefField()
        leave.s _CONTINUE1
    }
    catch [mscorlib]System.TypeLoadException
    {
        ldloc currentErrorCount
        ldc.i4 1
        add
        stloc currentErrorCount
        leave.s _CONTINUE1
    }

_CONTINUE1:

    //////////////////////////////////////
    // InvalidRefFieldAlignment //////////
    //////////////////////////////////////
    .try
    {
        call void Test_InvalidRefFieldAlignment()
        leave.s _CONTINUE2
    }
    catch [mscorlib]System.TypeLoadException
    {
        ldloc currentErrorCount
        ldc.i4 1
        add
        stloc currentErrorCount
        leave.s _CONTINUE2
    }

_CONTINUE2:

    //////////////////////////////////////
    // InvalidObjectRefRefFieldOverlap ///
    //////////////////////////////////////
    .try
    {
        call void Test_InvalidObjectRefRefFieldOverlap()
        leave.s _CONTINUE3
    }
    catch [mscorlib]System.TypeLoadException
    {
        ldloc currentErrorCount
        ldc.i4 1
        add
        stloc currentErrorCount
        leave.s _CONTINUE3
    }

_CONTINUE3:

    //////////////////////////////////////
    // IntPtrRefFieldOverlap /////////////
    //////////////////////////////////////
    .try
    {
        call void Test_IntPtrRefFieldOverlap()
        leave.s _CONTINUE4
    }
    catch [mscorlib]System.TypeLoadException
    {
        ldloc currentErrorCount
        ldc.i4 1
        add
        stloc currentErrorCount
        leave.s _CONTINUE4
    }

_CONTINUE4:

    //////////////////////////////////////
    // IntPtrOverlapWithInnerFieldType ///
    //////////////////////////////////////
    .try
    {
        call void Test_IntPtrOverlapWithInnerFieldType() // This is suspicious...seems that the erroring isn't because of the field overlap, but because of the second field's type
        leave.s _CONTINUE5
    }
    catch [mscorlib]System.TypeLoadException
    {
        ldloc currentErrorCount
        ldc.i4 1
        add
        stloc currentErrorCount
        leave.s _CONTINUE5
    }

_CONTINUE5:


    call void Test_WithRefField()


    /*
    call void Test_InvalidRefFieldAlignment()
    call void Test_InvalidObjectRefRefFieldOverlap()
    call void Test_InnerValidByRef()
    call void Test_IntPtrOverlapWithInnerFieldType()
    call void Test_WithRefField()
    call void Test_WithRefStructField()
    call void Test_WithTypedReferenceField()

    ldstr "We made it to the end!"
    call void [mscorlib]System.Console::WriteLine(string)
    */


    // Need to check that the error counter is the expected number
    ldloc currentErrorCount
    ldloc expectedErrorCount

    beq SUCCESS
      ldstr      "Incorrect Error Count"
      newobj     instance void [System.Runtime]System.Exception::.ctor(string)
      throw

SUCCESS:
    ldstr "SUCCESS!"
    call void [mscorlib]System.Console::WriteLine(string)
    ret
}

.method static void PrintMethodName(string methodName)
{
    ldstr "--"
    call void [mscorlib]System.Console::Write(string)

    ldarg methodName
    call void [mscorlib]System.Console::WriteLine(string)

    ret
}

.method static void Test_InvalidStructWithRefField()
{
    .maxstack 2

    ldstr "Test_InvalidStructWithRefField"
    call void PrintMethodName(string)

    //.locals init (string testString, valuetype InvalidCSharp.InvalidStructWithRefField invalidStructWithRefField)

    .locals init (valuetype InvalidCSharp.InvalidStructWithRefField invalidStructWithRefField)

    ret
}

.method static void Test_InvalidRefFieldAlignment()
{
    .maxstack 2
    .locals init (valuetype InvalidCSharp.InvalidRefFieldAlignment invalidRefFieldAlignmentClass)
    ret
}

.method static void Test_InvalidObjectRefRefFieldOverlap()
{
    .maxstack 2
    .locals init (valuetype InvalidCSharp.InvalidObjectRefRefFieldOverlap invalidObjectRefRefFieldOverlapClass)
    ret
}

.method static void Test_InnerValidByRef()
{
    .maxstack 2

    ldstr "Test_InnerValidByRef"
    call void PrintMethodName(string)

    ret
}

.method static void Test_IntPtrRefFieldOverlap()
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
    ldloc V_1
    stloc V_0
    ret








    //.maxstack 2
    //.locals init (valuetype InvalidCSharp.IntPtrRefFieldOverlap intPtrRefFieldOverlapClass)
    //ret
}

.method static void Test_IntPtrOverlapWithInnerFieldType()
{
    .maxstack 2
    .locals init (valuetype InvalidCSharp.IntPtrOverlapWithInnerFieldType intPtrOverlapWithInnerFieldTypeClass)
    ret
}

.method static void Test_WithRefField()
{
    .maxstack 8

    ldstr "Test_WithRefField"
    call void PrintMethodName(string)

    .locals init (string testString, int32 testInt, valuetype InvalidCSharp.WithRefField withRefFieldClass)

    ldstr "Test"
    stloc testString

    ldc.i4 4
    stloc testInt

    ldloca withRefFieldClass
    ldloc testString
    ldloca testInt
    call instance void InvalidCSharp.WithRefField::.ctor(string, int32 &)

    ldloca withRefFieldClass
    ldloc testString
    call instance bool InvalidCSharp.WithRefField::ConfirmFieldInstance(string)
    call void [mscorlib]System.Console::WriteLine(bool) // Probably isn't necessary

    ret
}

.method static void Test_WithRefStructField()
{
    .maxstack 2

    ldstr "Test_WithRefStructField"
    call void PrintMethodName(string)

    .locals init (string testString, valuetype InvalidCSharp.WithRefField withRefFieldClass, valuetype InvalidCSharp.WithRefStructField withRefStructFieldClass)

    ldstr "Test"
    stloc testString

    ldloca withRefFieldClass
    ldloca testString
    call instance void InvalidCSharp.WithRefField::.ctor(string &)

    ldloca withRefStructFieldClass
    ldloca withRefFieldClass
    call instance void InvalidCSharp.WithRefStructField::.ctor(valuetype InvalidCSharp.WithRefField &)

    ldloca withRefStructFieldClass
    ldloca withRefFieldClass
    call instance bool InvalidCSharp.WithRefStructField::ConfirmFieldInstance(valuetype InvalidCSharp.WithRefField &)
    call void [mscorlib]System.Console::WriteLine(bool) // Probably isn't necessary

    ret
}

.method static void Test_WithTypedReferenceField()
{
    .maxstack 2

    ldstr "Test_WithTypedReferenceField"
    call void PrintMethodName(string)

    ret
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

// TESTED
.class public sequential ansi sealed beforefieldinit InvalidCSharp.WithRefField
    extends [System.Runtime]System.ValueType
{
    .custom instance void [System.Runtime]System.Runtime.CompilerServices.IsByRefLikeAttribute::.ctor() = (
        01 00 00 00
    )
    .field [0] public native int Str
    .field [0] public int32& Int

    .method public hidebysig specialname rtspecialname
        instance void .ctor (
            string, int32&
        ) cil managed
    {
        //ldarg.0
        //ldarg.1
        //stfld string InvalidCSharp.WithRefField::Str

        ldarg.0
        ldsfld native int [System.Runtime]System.IntPtr::Zero
        stfld native int InvalidCSharp.WithRefField::Str

        ldarg.0
        ldarg.2
        stfld int32& InvalidCSharp.WithRefField::Int

        ldarg.0
        ldfld native int InvalidCSharp.WithRefField::Str
        pop
        //ldind.ref
        //call void [mscorlib]System.Console::WriteLine(native int)

        ldarg.0
        ldfld int32& InvalidCSharp.WithRefField::Int
        ldind.i4
        call void [mscorlib]System.Console::WriteLine(int32)


        ret
    }

    .method public hidebysig
        instance bool ConfirmFieldInstance (
            string
        ) cil managed
    {
        ldarg.0
        ldfld string InvalidCSharp.WithRefField::Str
        //ldind.ref
        ldarg.1
        ceq
        ret
    }
}

// TESTED
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
