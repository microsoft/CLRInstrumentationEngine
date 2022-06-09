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
        IL_007f:  dup
        IL_0080:  brtrue.s   IL_0099
        IL_0082:  pop
        IL_0083:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_0088:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_0'()
        IL_008e:  newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        IL_0093:  dup
        IL_0094:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_0'
        IL_0099:  call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        IL_009e:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_00a3:  nop
        IL_00a4:  ldloca.s   V_0
        IL_00a6:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_00ab:  call       void [System.Console]System.Console::WriteLine(string)
        IL_00b0:  nop
        IL_00b1:  ldloca.s   V_0
        IL_00b3:  ldc.i4.s   22
        IL_00b5:  ldc.i4.1
        IL_00b6:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32, int32)
        IL_00bb:  ldloca.s   V_0
        IL_00bd:  ldstr      " -- GenericInterface: "
        IL_00c2:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_00c7:  nop
        IL_00c8:  ldloca.s   V_0
        IL_00ca:  ldarg.0
        IL_00cb:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_1'
        IL_00d0:  dup
        IL_00d1:  brtrue.s   IL_00ea
        IL_00d3:  pop
        IL_00d4:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_00d9:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_1'()
        IL_00df:  newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_1'
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
        brtrue.s   BRANCH
        pop
        ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_2'()
        newobj     instance void [System.Runtime]System.Action::.ctor(object, native int)
        dup
        stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_2'
    BRANCH:
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
        call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                   int32)
        IL_015d:  ldloca.s   V_0
        IL_015f:  ldstr      " -- GenericByRefLike: "
        IL_0164:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_0169:  nop
        IL_016a:  ldloca.s   V_0
        IL_016c:  ldarg.0
        IL_016d:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_3'
        IL_0172:  dup
        IL_0173:  brtrue.s   IL_018c
        IL_0175:  pop
        IL_0176:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_017b:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_3'()
        IL_0181:  newobj     instance void [System.Runtime]System.Action::.ctor(object,
                                                                                native int)
        IL_0186:  dup
        IL_0187:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_3'
        IL_018c:  call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        IL_0191:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_0196:  nop
        IL_0197:  ldloca.s   V_0
        IL_0199:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_019e:  call       void [System.Console]System.Console::WriteLine(string)
        IL_01a3:  nop
        IL_01a4:  ldstr      "Validate Casting:"
        IL_01a9:  call       void [System.Console]System.Console::WriteLine(string)
        IL_01ae:  nop
        IL_01af:  ldloca.s   V_0
        IL_01b1:  ldc.i4.s   24
        IL_01b3:  ldc.i4.1
        IL_01b4:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_01b9:  ldloca.s   V_0
        IL_01bb:  ldstr      " -- InstanceOf(object): "
        IL_01c0:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_01c5:  nop
        IL_01c6:  ldloca.s   V_0
        IL_01c8:  newobj     instance void [System.Runtime]System.Object::.ctor()
        IL_01cd:  call       bool [InvalidCSharp]InvalidCSharp.Exec::InstanceOfT(object)
        IL_01d2:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_01d7:  nop
        IL_01d8:  ldloca.s   V_0
        IL_01da:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_01df:  call       void [System.Console]System.Console::WriteLine(string)
        IL_01e4:  nop
        IL_01e5:  ldloca.s   V_0
        IL_01e7:  ldc.i4.s   22
        IL_01e9:  ldc.i4.1
        IL_01ea:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_01ef:  ldloca.s   V_0
        IL_01f1:  ldstr      " -- Cast To (object): "
        IL_01f6:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_01fb:  nop
        IL_01fc:  ldloca.s   V_0
        IL_01fe:  ldarg.0
        IL_01ff:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_4'
        IL_0204:  dup
        IL_0205:  brtrue.s   IL_021e
        IL_0207:  pop
        IL_0208:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_020d:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_4'()
        IL_0213:  newobj     instance void [System.Runtime]System.Action::.ctor(object,
                                                                                native int)
        IL_0218:  dup
        IL_0219:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_4'
        IL_021e:  call       instance bool Validator::ValidateException<class [System.Runtime]System.InvalidCastException>(class [System.Runtime]System.Action)
        IL_0223:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_0228:  nop
        IL_0229:  ldloca.s   V_0
        IL_022b:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_0230:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0235:  nop
        IL_0236:  ldloca.s   V_0
        IL_0238:  ldc.i4.s   23
        IL_023a:  ldc.i4.1
        IL_023b:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_0240:  ldloca.s   V_0
        IL_0242:  ldstr      " -- Unbox To (object): "
        IL_0247:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_024c:  nop
        IL_024d:  ldloca.s   V_0
        IL_024f:  ldarg.0
        IL_0250:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_5'
        IL_0255:  dup
        IL_0256:  brtrue.s   IL_026f
        IL_0258:  pop
        IL_0259:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_025e:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_5'()
        IL_0264:  newobj     instance void [System.Runtime]System.Action::.ctor(object,
                                                                                native int)
        IL_0269:  dup
        IL_026a:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_5'
        IL_026f:  call       instance bool Validator::ValidateException<class [System.Runtime]System.InvalidCastException>(class [System.Runtime]System.Action)
        IL_0274:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_0279:  nop
        IL_027a:  ldloca.s   V_0
        IL_027c:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_0281:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0286:  nop
        IL_0287:  ldstr      "Validate Opcodes"
        IL_028c:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0291:  nop
        IL_0292:  ldloca.s   V_0
        IL_0294:  ldc.i4.s   15
        IL_0296:  ldc.i4.1
        IL_0297:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_029c:  ldloca.s   V_0
        IL_029e:  ldstr      " -- Unbox Any: "
        IL_02a3:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_02a8:  nop
        IL_02a9:  ldloca.s   V_0
        IL_02ab:  call       bool [InvalidCSharp]InvalidCSharp.Exec::BoxUnboxAny()
        IL_02b0:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_02b5:  nop
        IL_02b6:  ldloca.s   V_0
        IL_02b8:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_02bd:  call       void [System.Console]System.Console::WriteLine(string)
        IL_02c2:  nop
        IL_02c3:  ldloca.s   V_0
        IL_02c5:  ldc.i4.s   16
        IL_02c7:  ldc.i4.1
        IL_02c8:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_02cd:  ldloca.s   V_0
        IL_02cf:  ldstr      " -- Box Branch: "
        IL_02d4:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_02d9:  nop
        IL_02da:  ldloca.s   V_0
        IL_02dc:  call       bool [InvalidCSharp]InvalidCSharp.Exec::BoxBranch()
        IL_02e1:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_02e6:  nop
        IL_02e7:  ldloca.s   V_0
        IL_02e9:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_02ee:  call       void [System.Console]System.Console::WriteLine(string)
        IL_02f3:  nop
        IL_02f4:  ldloca.s   V_0
        IL_02f6:  ldc.i4.s   18
        IL_02f8:  ldc.i4.1
        IL_02f9:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_02fe:  ldloca.s   V_0
        IL_0300:  ldstr      " -- Boxed Is Any: "
        IL_0305:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_030a:  nop
        IL_030b:  ldloca.s   V_0
        IL_030d:  call       bool [InvalidCSharp]InvalidCSharp.Exec::BoxIsinstUnboxAny()
        IL_0312:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_0317:  nop
        IL_0318:  ldloca.s   V_0
        IL_031a:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_031f:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0324:  nop
        IL_0325:  ldloca.s   V_0
        IL_0327:  ldc.i4.s   21
        IL_0329:  ldc.i4.1
        IL_032a:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_032f:  ldloca.s   V_0
        IL_0331:  ldstr      " -- Boxed Is Branch: "
        IL_0336:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_033b:  nop
        IL_033c:  ldloca.s   V_0
        IL_033e:  call       bool [InvalidCSharp]InvalidCSharp.Exec::BoxIsinstBranch()
        IL_0343:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_0348:  nop
        IL_0349:  ldloca.s   V_0
        IL_034b:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_0350:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0355:  nop
        IL_0356:  ldstr      "Validate Invalid Opcodes"
        IL_035b:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0360:  nop
        IL_0361:  ldloca.s   V_0
        IL_0363:  ldc.i4.s   20
        IL_0365:  ldc.i4.1
        IL_0366:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_036b:  ldloca.s   V_0
        IL_036d:  ldstr      " -- Allocate Array: "
        IL_0372:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_0377:  nop
        IL_0378:  ldloca.s   V_0
        IL_037a:  ldarg.0
        IL_037b:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_6'
        IL_0380:  dup
        IL_0381:  brtrue.s   IL_039a
        IL_0383:  pop
        IL_0384:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_0389:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_6'()
        IL_038f:  newobj     instance void [System.Runtime]System.Action::.ctor(object,
                                                                                native int)
        IL_0394:  dup
        IL_0395:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_6'
        IL_039a:  call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        IL_039f:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_03a4:  nop
        IL_03a5:  ldloca.s   V_0
        IL_03a7:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_03ac:  call       void [System.Console]System.Console::WriteLine(string)
        IL_03b1:  nop
        IL_03b2:  ldloca.s   V_0
        IL_03b4:  ldc.i4.s   38
        IL_03b6:  ldc.i4.1
        IL_03b7:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_03bc:  ldloca.s   V_0
        IL_03be:  ldstr      " -- Allocate Multi Dimensional Array: "
        IL_03c3:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_03c8:  nop
        IL_03c9:  ldloca.s   V_0
        IL_03cb:  ldarg.0
        IL_03cc:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_7'
        IL_03d1:  dup
        IL_03d2:  brtrue.s   IL_03eb
        IL_03d4:  pop
        IL_03d5:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_03da:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_7'()
        IL_03e0:  newobj     instance void [System.Runtime]System.Action::.ctor(object,
                                                                                native int)
        IL_03e5:  dup
        IL_03e6:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_7'
        IL_03eb:  call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        IL_03f0:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_03f5:  nop
        IL_03f6:  ldloca.s   V_0
        IL_03f8:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_03fd:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0402:  nop
        IL_0403:  ldloca.s   V_0
        IL_0405:  ldc.i4.s   37
        IL_0407:  ldc.i4.1
        IL_0408:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_040d:  ldloca.s   V_0
        IL_040f:  ldstr      " -- Generic class with static field: "
        IL_0414:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_0419:  nop
        IL_041a:  ldloca.s   V_0
        IL_041c:  ldarg.0
        IL_041d:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_8'
        IL_0422:  dup
        IL_0423:  brtrue.s   IL_043c
        IL_0425:  pop
        IL_0426:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_042b:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_8'()
        IL_0431:  newobj     instance void [System.Runtime]System.Action::.ctor(object,
                                                                                native int)
        IL_0436:  dup
        IL_0437:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_8'
        IL_043c:  call       instance bool Validator::ValidateException<class [System.Runtime]System.TypeLoadException>(class [System.Runtime]System.Action)
        IL_0441:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_0446:  nop
        IL_0447:  ldloca.s   V_0
        IL_0449:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_044e:  call       void [System.Console]System.Console::WriteLine(string)
        IL_0453:  nop
        IL_0454:  ldloca.s   V_0
        IL_0456:  ldc.i4.s   19
        IL_0458:  ldc.i4.1
        IL_0459:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_045e:  ldloca.s   V_0
        IL_0460:  ldstr      " -- Box as object: "
        IL_0465:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_046a:  nop
        IL_046b:  ldloca.s   V_0
        IL_046d:  ldarg.0
        IL_046e:  ldsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_9'
        IL_0473:  dup
        IL_0474:  brtrue.s   IL_048d
        IL_0476:  pop
        IL_0477:  ldsfld     class ByRefLikeValidator/'<>c' ByRefLikeValidator/'<>c'::'<>9'
        IL_047c:  ldftn      instance void ByRefLikeValidator/'<>c'::'<Validate>b__0_9'()
        IL_0482:  newobj     instance void [System.Runtime]System.Action::.ctor(object,
                                                                                native int)
        IL_0487:  dup
        IL_0488:  stsfld     class [System.Runtime]System.Action ByRefLikeValidator/'<>c'::'<>9__0_9'
        IL_048d:  call       instance bool Validator::ValidateException<class [System.Runtime]System.InvalidProgramException>(class [System.Runtime]System.Action)
        IL_0492:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_0497:  nop
        IL_0498:  ldloca.s   V_0
        IL_049a:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_049f:  call       void [System.Console]System.Console::WriteLine(string)
        IL_04a4:  nop
        IL_04a5:  ldstr      "Validate Inlining:"
        IL_04aa:  call       void [System.Console]System.Console::WriteLine(string)
        IL_04af:  nop
        IL_04b0:  ldloca.s   V_0
        IL_04b2:  ldc.i4.s   20
        IL_04b4:  ldc.i4.1
        IL_04b5:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(int32,
                                                                                                                                    int32)
        IL_04ba:  ldloca.s   V_0
        IL_04bc:  ldstr      " -- Avoid Inlining: "
        IL_04c1:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(string)
        IL_04c6:  nop
        IL_04c7:  ldloca.s   V_0
        IL_04c9:  ldc.i4.0
        IL_04ca:  call       bool [InvalidCSharp]InvalidCSharp.Exec::TypeLoadExceptionAvoidsInline(bool)
        IL_04cf:  call       instance void [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<bool>(!!0)
        IL_04d4:  nop
        IL_04d5:  ldloca.s   V_0
        IL_04d7:  call       instance string [System.Runtime]System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()
        IL_04dc:  call       void [System.Console]System.Console::WriteLine(string)
        IL_04e1:  nop
        IL_04e2:  ret
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

        .method assembly hidebysig instance void 
        '<Validate>b__0_0'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       string [InvalidCSharp]InvalidCSharp.Exec::GenericClass_Invalid()
            IL_0006:  pop
            IL_0007:  ret
        }

        .method assembly hidebysig instance void '<Validate>b__0_1'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       string [InvalidCSharp]InvalidCSharp.Exec::GenericInterface_Invalid()
            IL_0006:  pop
            IL_0007:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_2'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       string [InvalidCSharp]InvalidCSharp.Exec::GenericValueType_Invalid()
            IL_0006:  pop
            IL_0007:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_3'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       string [InvalidCSharp]InvalidCSharp.Exec::GenericByRefLike_Invalid()
            IL_0006:  pop
            IL_0007:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_4'() cil managed
        {
            // Code size       13 (0xd)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  newobj     instance void [System.Runtime]System.Object::.ctor()
            IL_0006:  call       void [InvalidCSharp]InvalidCSharp.Exec::CastToT(object)
            IL_000b:  nop
            IL_000c:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_5'() cil managed
        {
            // Code size       13 (0xd)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  newobj     instance void [System.Runtime]System.Object::.ctor()
            IL_0006:  call       void [InvalidCSharp]InvalidCSharp.Exec::UnboxToT(object)
            IL_000b:  nop
            IL_000c:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_6'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       void [InvalidCSharp]InvalidCSharp.Exec::AllocArrayOfT_Invalid()
            IL_0006:  nop
            IL_0007:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_7'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       void [InvalidCSharp]InvalidCSharp.Exec::AllocMultiDimArrayOfT_Invalid()
            IL_0006:  nop
            IL_0007:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_8'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       string [InvalidCSharp]InvalidCSharp.Exec::GenericClassWithStaticField_Invalid()
            IL_0006:  pop
            IL_0007:  ret
        }

        .method assembly hidebysig instance void 
        '<Validate>b__0_9'() cil managed
        {
            // Code size       8 (0x8)
            .maxstack  8
            IL_0000:  nop
            IL_0001:  call       object [InvalidCSharp]InvalidCSharp.Exec::BoxAsObject()
            IL_0006:  pop
            IL_0007:  ret
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