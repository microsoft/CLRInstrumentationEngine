

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0620 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for InstrumentationEngine.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0620 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __InstrumentationEngine_h__
#define __InstrumentationEngine_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IProfilerManager_FWD_DEFINED__
#define __IProfilerManager_FWD_DEFINED__
typedef interface IProfilerManager IProfilerManager;

#endif 	/* __IProfilerManager_FWD_DEFINED__ */


#ifndef __IProfilerManagerHost_FWD_DEFINED__
#define __IProfilerManagerHost_FWD_DEFINED__
typedef interface IProfilerManagerHost IProfilerManagerHost;

#endif 	/* __IProfilerManagerHost_FWD_DEFINED__ */


#ifndef __IProfilerManagerLogging_FWD_DEFINED__
#define __IProfilerManagerLogging_FWD_DEFINED__
typedef interface IProfilerManagerLogging IProfilerManagerLogging;

#endif 	/* __IProfilerManagerLogging_FWD_DEFINED__ */


#ifndef __IProfilerManagerLoggingHost_FWD_DEFINED__
#define __IProfilerManagerLoggingHost_FWD_DEFINED__
typedef interface IProfilerManagerLoggingHost IProfilerManagerLoggingHost;

#endif 	/* __IProfilerManagerLoggingHost_FWD_DEFINED__ */


#ifndef __IInstrumentationMethod_FWD_DEFINED__
#define __IInstrumentationMethod_FWD_DEFINED__
typedef interface IInstrumentationMethod IInstrumentationMethod;

#endif 	/* __IInstrumentationMethod_FWD_DEFINED__ */


#ifndef __IDataContainer_FWD_DEFINED__
#define __IDataContainer_FWD_DEFINED__
typedef interface IDataContainer IDataContainer;

#endif 	/* __IDataContainer_FWD_DEFINED__ */


#ifndef __IInstruction_FWD_DEFINED__
#define __IInstruction_FWD_DEFINED__
typedef interface IInstruction IInstruction;

#endif 	/* __IInstruction_FWD_DEFINED__ */


#ifndef __IExceptionSection_FWD_DEFINED__
#define __IExceptionSection_FWD_DEFINED__
typedef interface IExceptionSection IExceptionSection;

#endif 	/* __IExceptionSection_FWD_DEFINED__ */


#ifndef __IExceptionClause_FWD_DEFINED__
#define __IExceptionClause_FWD_DEFINED__
typedef interface IExceptionClause IExceptionClause;

#endif 	/* __IExceptionClause_FWD_DEFINED__ */


#ifndef __IEnumExceptionClauses_FWD_DEFINED__
#define __IEnumExceptionClauses_FWD_DEFINED__
typedef interface IEnumExceptionClauses IEnumExceptionClauses;

#endif 	/* __IEnumExceptionClauses_FWD_DEFINED__ */


#ifndef __IOperandInstruction_FWD_DEFINED__
#define __IOperandInstruction_FWD_DEFINED__
typedef interface IOperandInstruction IOperandInstruction;

#endif 	/* __IOperandInstruction_FWD_DEFINED__ */


#ifndef __IBranchInstruction_FWD_DEFINED__
#define __IBranchInstruction_FWD_DEFINED__
typedef interface IBranchInstruction IBranchInstruction;

#endif 	/* __IBranchInstruction_FWD_DEFINED__ */


#ifndef __ISwitchInstruction_FWD_DEFINED__
#define __ISwitchInstruction_FWD_DEFINED__
typedef interface ISwitchInstruction ISwitchInstruction;

#endif 	/* __ISwitchInstruction_FWD_DEFINED__ */


#ifndef __IInstructionGraph_FWD_DEFINED__
#define __IInstructionGraph_FWD_DEFINED__
typedef interface IInstructionGraph IInstructionGraph;

#endif 	/* __IInstructionGraph_FWD_DEFINED__ */


#ifndef __IMethodInfo_FWD_DEFINED__
#define __IMethodInfo_FWD_DEFINED__
typedef interface IMethodInfo IMethodInfo;

#endif 	/* __IMethodInfo_FWD_DEFINED__ */


#ifndef __IMethodInfo2_FWD_DEFINED__
#define __IMethodInfo2_FWD_DEFINED__
typedef interface IMethodInfo2 IMethodInfo2;

#endif 	/* __IMethodInfo2_FWD_DEFINED__ */


#ifndef __IAssemblyInfo_FWD_DEFINED__
#define __IAssemblyInfo_FWD_DEFINED__
typedef interface IAssemblyInfo IAssemblyInfo;

#endif 	/* __IAssemblyInfo_FWD_DEFINED__ */


#ifndef __IEnumAssemblyInfo_FWD_DEFINED__
#define __IEnumAssemblyInfo_FWD_DEFINED__
typedef interface IEnumAssemblyInfo IEnumAssemblyInfo;

#endif 	/* __IEnumAssemblyInfo_FWD_DEFINED__ */


#ifndef __IModuleInfo_FWD_DEFINED__
#define __IModuleInfo_FWD_DEFINED__
typedef interface IModuleInfo IModuleInfo;

#endif 	/* __IModuleInfo_FWD_DEFINED__ */


#ifndef __IModuleInfo2_FWD_DEFINED__
#define __IModuleInfo2_FWD_DEFINED__
typedef interface IModuleInfo2 IModuleInfo2;

#endif 	/* __IModuleInfo2_FWD_DEFINED__ */


#ifndef __IEnumModuleInfo_FWD_DEFINED__
#define __IEnumModuleInfo_FWD_DEFINED__
typedef interface IEnumModuleInfo IEnumModuleInfo;

#endif 	/* __IEnumModuleInfo_FWD_DEFINED__ */


#ifndef __IAppDomainInfo_FWD_DEFINED__
#define __IAppDomainInfo_FWD_DEFINED__
typedef interface IAppDomainInfo IAppDomainInfo;

#endif 	/* __IAppDomainInfo_FWD_DEFINED__ */


#ifndef __IEnumAppDomainInfo_FWD_DEFINED__
#define __IEnumAppDomainInfo_FWD_DEFINED__
typedef interface IEnumAppDomainInfo IEnumAppDomainInfo;

#endif 	/* __IEnumAppDomainInfo_FWD_DEFINED__ */


#ifndef __ILocalVariableCollection_FWD_DEFINED__
#define __ILocalVariableCollection_FWD_DEFINED__
typedef interface ILocalVariableCollection ILocalVariableCollection;

#endif 	/* __ILocalVariableCollection_FWD_DEFINED__ */


#ifndef __IType_FWD_DEFINED__
#define __IType_FWD_DEFINED__
typedef interface IType IType;

#endif 	/* __IType_FWD_DEFINED__ */


#ifndef __IAppDomainCollection_FWD_DEFINED__
#define __IAppDomainCollection_FWD_DEFINED__
typedef interface IAppDomainCollection IAppDomainCollection;

#endif 	/* __IAppDomainCollection_FWD_DEFINED__ */


#ifndef __ISignatureBuilder_FWD_DEFINED__
#define __ISignatureBuilder_FWD_DEFINED__
typedef interface ISignatureBuilder ISignatureBuilder;

#endif 	/* __ISignatureBuilder_FWD_DEFINED__ */


#ifndef __ITypeCreator_FWD_DEFINED__
#define __ITypeCreator_FWD_DEFINED__
typedef interface ITypeCreator ITypeCreator;

#endif 	/* __ITypeCreator_FWD_DEFINED__ */


#ifndef __IMethodLocal_FWD_DEFINED__
#define __IMethodLocal_FWD_DEFINED__
typedef interface IMethodLocal IMethodLocal;

#endif 	/* __IMethodLocal_FWD_DEFINED__ */


#ifndef __IMethodParameter_FWD_DEFINED__
#define __IMethodParameter_FWD_DEFINED__
typedef interface IMethodParameter IMethodParameter;

#endif 	/* __IMethodParameter_FWD_DEFINED__ */


#ifndef __IEnumMethodLocals_FWD_DEFINED__
#define __IEnumMethodLocals_FWD_DEFINED__
typedef interface IEnumMethodLocals IEnumMethodLocals;

#endif 	/* __IEnumMethodLocals_FWD_DEFINED__ */


#ifndef __IEnumMethodParameters_FWD_DEFINED__
#define __IEnumMethodParameters_FWD_DEFINED__
typedef interface IEnumMethodParameters IEnumMethodParameters;

#endif 	/* __IEnumMethodParameters_FWD_DEFINED__ */


#ifndef __ISingleRetDefaultInstrumentation_FWD_DEFINED__
#define __ISingleRetDefaultInstrumentation_FWD_DEFINED__
typedef interface ISingleRetDefaultInstrumentation ISingleRetDefaultInstrumentation;

#endif 	/* __ISingleRetDefaultInstrumentation_FWD_DEFINED__ */


#ifndef __IProfilerManager2_FWD_DEFINED__
#define __IProfilerManager2_FWD_DEFINED__
typedef interface IProfilerManager2 IProfilerManager2;

#endif 	/* __IProfilerManager2_FWD_DEFINED__ */


#ifndef __IInstrumentationMethodExceptionEvents_FWD_DEFINED__
#define __IInstrumentationMethodExceptionEvents_FWD_DEFINED__
typedef interface IInstrumentationMethodExceptionEvents IInstrumentationMethodExceptionEvents;

#endif 	/* __IInstrumentationMethodExceptionEvents_FWD_DEFINED__ */


#ifndef __IEnumInstructions_FWD_DEFINED__
#define __IEnumInstructions_FWD_DEFINED__
typedef interface IEnumInstructions IEnumInstructions;

#endif 	/* __IEnumInstructions_FWD_DEFINED__ */


#ifndef __IInstructionFactory_FWD_DEFINED__
#define __IInstructionFactory_FWD_DEFINED__
typedef interface IInstructionFactory IInstructionFactory;

#endif 	/* __IInstructionFactory_FWD_DEFINED__ */


#ifndef __IEnumAppMethodInfo_FWD_DEFINED__
#define __IEnumAppMethodInfo_FWD_DEFINED__
typedef interface IEnumAppMethodInfo IEnumAppMethodInfo;

#endif 	/* __IEnumAppMethodInfo_FWD_DEFINED__ */


#ifndef __ILocalVariableCollection2_FWD_DEFINED__
#define __ILocalVariableCollection2_FWD_DEFINED__
typedef interface ILocalVariableCollection2 ILocalVariableCollection2;

#endif 	/* __ILocalVariableCollection2_FWD_DEFINED__ */


#ifndef __IEnumTypes_FWD_DEFINED__
#define __IEnumTypes_FWD_DEFINED__
typedef interface IEnumTypes IEnumTypes;

#endif 	/* __IEnumTypes_FWD_DEFINED__ */


#ifndef __ISignatureParser_FWD_DEFINED__
#define __ISignatureParser_FWD_DEFINED__
typedef interface ISignatureParser ISignatureParser;

#endif 	/* __ISignatureParser_FWD_DEFINED__ */


#ifndef __ITokenType_FWD_DEFINED__
#define __ITokenType_FWD_DEFINED__
typedef interface ITokenType ITokenType;

#endif 	/* __ITokenType_FWD_DEFINED__ */


#ifndef __ICompositeType_FWD_DEFINED__
#define __ICompositeType_FWD_DEFINED__
typedef interface ICompositeType ICompositeType;

#endif 	/* __ICompositeType_FWD_DEFINED__ */


#ifndef __IGenericParameterType_FWD_DEFINED__
#define __IGenericParameterType_FWD_DEFINED__
typedef interface IGenericParameterType IGenericParameterType;

#endif 	/* __IGenericParameterType_FWD_DEFINED__ */


#ifndef __ISingleRetDefaultInstrumentation2_FWD_DEFINED__
#define __ISingleRetDefaultInstrumentation2_FWD_DEFINED__
typedef interface ISingleRetDefaultInstrumentation2 ISingleRetDefaultInstrumentation2;

#endif 	/* __ISingleRetDefaultInstrumentation2_FWD_DEFINED__ */


#ifndef __IInstrumentationMethodJitEvents_FWD_DEFINED__
#define __IInstrumentationMethodJitEvents_FWD_DEFINED__
typedef interface IInstrumentationMethodJitEvents IInstrumentationMethodJitEvents;

#endif 	/* __IInstrumentationMethodJitEvents_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"
#include "corprof.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MicrosoftInstrumentationEngine_LIBRARY_DEFINED__
#define __MicrosoftInstrumentationEngine_LIBRARY_DEFINED__

/* library MicrosoftInstrumentationEngine */
/* [uuid] */ 






































enum LoggingFlags
    {
        LoggingFlags_None	= 0,
        LoggingFlags_Errors	= 0x1,
        LoggingFlags_Trace	= 0x2,
        LoggingFlags_InstrumentationResults	= 0x4
    } ;

enum ILOrdinalOpcode
    {
        Cee_Nop	= 0,
        Cee_Break	= 0x1,
        Cee_Ldarg_0	= 0x2,
        Cee_Ldarg_1	= 0x3,
        Cee_Ldarg_2	= 0x4,
        Cee_Ldarg_3	= 0x5,
        Cee_Ldloc_0	= 0x6,
        Cee_Ldloc_1	= 0x7,
        Cee_Ldloc_2	= 0x8,
        Cee_Ldloc_3	= 0x9,
        Cee_Stloc_0	= 0xa,
        Cee_Stloc_1	= 0xb,
        Cee_Stloc_2	= 0xc,
        Cee_Stloc_3	= 0xd,
        Cee_Ldarg_S	= 0xe,
        Cee_Ldarga_S	= 0xf,
        Cee_Starg_S	= 0x10,
        Cee_Ldloc_S	= 0x11,
        Cee_Ldloca_S	= 0x12,
        Cee_Stloc_S	= 0x13,
        Cee_Ldnull	= 0x14,
        Cee_Ldc_I4_M1	= 0x15,
        Cee_Ldc_I4_0	= 0x16,
        Cee_Ldc_I4_1	= 0x17,
        Cee_Ldc_I4_2	= 0x18,
        Cee_Ldc_I4_3	= 0x19,
        Cee_Ldc_I4_4	= 0x1a,
        Cee_Ldc_I4_5	= 0x1b,
        Cee_Ldc_I4_6	= 0x1c,
        Cee_Ldc_I4_7	= 0x1d,
        Cee_Ldc_I4_8	= 0x1e,
        Cee_Ldc_I4_S	= 0x1f,
        Cee_Ldc_I4	= 0x20,
        Cee_Ldc_I8	= 0x21,
        Cee_Ldc_R4	= 0x22,
        Cee_Ldc_R8	= 0x23,
        Cee_Unused49	= 0x24,
        Cee_Dup	= 0x25,
        Cee_Pop	= 0x26,
        Cee_Jmp	= 0x27,
        Cee_Call	= 0x28,
        Cee_Calli	= 0x29,
        Cee_Ret	= 0x2a,
        Cee_Br_S	= 0x2b,
        Cee_Brfalse_S	= 0x2c,
        Cee_Brtrue_S	= 0x2d,
        Cee_Beq_S	= 0x2e,
        Cee_Bge_S	= 0x2f,
        Cee_Bgt_S	= 0x30,
        Cee_Ble_S	= 0x31,
        Cee_Blt_S	= 0x32,
        Cee_Bne_Un_S	= 0x33,
        Cee_Bge_Un_S	= 0x34,
        Cee_Bgt_Un_S	= 0x35,
        Cee_Ble_Un_S	= 0x36,
        Cee_Blt_Un_S	= 0x37,
        Cee_Br	= 0x38,
        Cee_Brfalse	= 0x39,
        Cee_Brtrue	= 0x3a,
        Cee_Beq	= 0x3b,
        Cee_Bge	= 0x3c,
        Cee_Bgt	= 0x3d,
        Cee_Ble	= 0x3e,
        Cee_Blt	= 0x3f,
        Cee_Bne_Un	= 0x40,
        Cee_Bge_Un	= 0x41,
        Cee_Bgt_Un	= 0x42,
        Cee_Ble_Un	= 0x43,
        Cee_Blt_Un	= 0x44,
        Cee_Switch	= 0x45,
        Cee_Ldind_I1	= 0x46,
        Cee_Ldind_U1	= 0x47,
        Cee_Ldind_I2	= 0x48,
        Cee_Ldind_U2	= 0x49,
        Cee_Ldind_I4	= 0x4a,
        Cee_Ldind_U4	= 0x4b,
        Cee_Ldind_I8	= 0x4c,
        Cee_Ldind_I	= 0x4d,
        Cee_Ldind_R4	= 0x4e,
        Cee_Ldind_R8	= 0x4f,
        Cee_Ldind_Ref	= 0x50,
        Cee_Stind_Ref	= 0x51,
        Cee_Stind_I1	= 0x52,
        Cee_Stind_I2	= 0x53,
        Cee_Stind_I4	= 0x54,
        Cee_Stind_I8	= 0x55,
        Cee_Stind_R4	= 0x56,
        Cee_Stind_R8	= 0x57,
        Cee_Add	= 0x58,
        Cee_Sub	= 0x59,
        Cee_Mul	= 0x5a,
        Cee_Div	= 0x5b,
        Cee_Div_Un	= 0x5c,
        Cee_Rem	= 0x5d,
        Cee_Rem_Un	= 0x5e,
        Cee_And	= 0x5f,
        Cee_Or	= 0x60,
        Cee_Xor	= 0x61,
        Cee_Shl	= 0x62,
        Cee_Shr	= 0x63,
        Cee_Shr_Un	= 0x64,
        Cee_Neg	= 0x65,
        Cee_Not	= 0x66,
        Cee_Conv_I1	= 0x67,
        Cee_Conv_I2	= 0x68,
        Cee_Conv_I4	= 0x69,
        Cee_Conv_I8	= 0x6a,
        Cee_Conv_R4	= 0x6b,
        Cee_Conv_R8	= 0x6c,
        Cee_Conv_U4	= 0x6d,
        Cee_Conv_U8	= 0x6e,
        Cee_Callvirt	= 0x6f,
        Cee_Cpobj	= 0x70,
        Cee_Ldobj	= 0x71,
        Cee_Ldstr	= 0x72,
        Cee_Newobj	= 0x73,
        Cee_Castclass	= 0x74,
        Cee_Isinst	= 0x75,
        Cee_Conv_R_Un	= 0x76,
        Cee_Unused58	= 0x77,
        Cee_Unused1	= 0x78,
        Cee_Unbox	= 0x79,
        Cee_Throw	= 0x7a,
        Cee_Ldfld	= 0x7b,
        Cee_Ldflda	= 0x7c,
        Cee_Stfld	= 0x7d,
        Cee_Ldsfld	= 0x7e,
        Cee_Ldsflda	= 0x7f,
        Cee_Stsfld	= 0x80,
        Cee_Stobj	= 0x81,
        Cee_Conv_Ovf_I1_Un	= 0x82,
        Cee_Conv_Ovf_I2_Un	= 0x83,
        Cee_Conv_Ovf_I4_Un	= 0x84,
        Cee_Conv_Ovf_I8_Un	= 0x85,
        Cee_Conv_Ovf_U1_Un	= 0x86,
        Cee_Conv_Ovf_U2_Un	= 0x87,
        Cee_Conv_Ovf_U4_Un	= 0x88,
        Cee_Conv_Ovf_U8_Un	= 0x89,
        Cee_Conv_Ovf_I_Un	= 0x8a,
        Cee_Conv_Ovf_U_Un	= 0x8b,
        Cee_Box	= 0x8c,
        Cee_Newarr	= 0x8d,
        Cee_Ldlen	= 0x8e,
        Cee_Ldelema	= 0x8f,
        Cee_Ldelem_I1	= 0x90,
        Cee_Ldelem_U1	= 0x91,
        Cee_Ldelem_I2	= 0x92,
        Cee_Ldelem_U2	= 0x93,
        Cee_Ldelem_I4	= 0x94,
        Cee_Ldelem_U4	= 0x95,
        Cee_Ldelem_I8	= 0x96,
        Cee_Ldelem_I	= 0x97,
        Cee_Ldelem_R4	= 0x98,
        Cee_Ldelem_R8	= 0x99,
        Cee_Ldelem_Ref	= 0x9a,
        Cee_Stelem_I	= 0x9b,
        Cee_Stelem_I1	= 0x9c,
        Cee_Stelem_I2	= 0x9d,
        Cee_Stelem_I4	= 0x9e,
        Cee_Stelem_I8	= 0x9f,
        Cee_Stelem_R4	= 0xa0,
        Cee_Stelem_R8	= 0xa1,
        Cee_Stelem_Ref	= 0xa2,
        Cee_Ldelem	= 0xa3,
        Cee_Stelem	= 0xa4,
        Cee_Unbox_Any	= 0xa5,
        Cee_Unused5	= 0xa6,
        Cee_Unused6	= 0xa7,
        Cee_Unused7	= 0xa8,
        Cee_Unused8	= 0xa9,
        Cee_Unused9	= 0xaa,
        Cee_Unused10	= 0xab,
        Cee_Unused11	= 0xac,
        Cee_Unused12	= 0xad,
        Cee_Unused13	= 0xae,
        Cee_Unused14	= 0xaf,
        Cee_Unused15	= 0xb0,
        Cee_Unused16	= 0xb1,
        Cee_Unused17	= 0xb2,
        Cee_Conv_Ovf_I1	= 0xb3,
        Cee_Conv_Ovf_U1	= 0xb4,
        Cee_Conv_Ovf_I2	= 0xb5,
        Cee_Conv_Ovf_U2	= 0xb6,
        Cee_Conv_Ovf_I4	= 0xb7,
        Cee_Conv_Ovf_U4	= 0xb8,
        Cee_Conv_Ovf_I8	= 0xb9,
        Cee_Conv_Ovf_U8	= 0xba,
        Cee_Unused50	= 0xbb,
        Cee_Unused18	= 0xbc,
        Cee_Unused19	= 0xbd,
        Cee_Unused20	= 0xbe,
        Cee_Unused21	= 0xbf,
        Cee_Unused22	= 0xc0,
        Cee_Unused23	= 0xc1,
        Cee_Refanyval	= 0xc2,
        Cee_Ckfinite	= 0xc3,
        Cee_Unused24	= 0xc4,
        Cee_Unused25	= 0xc5,
        Cee_Mkrefany	= 0xc6,
        Cee_Unused59	= 0xc7,
        Cee_Unused60	= 0xc8,
        Cee_Unused61	= 0xc9,
        Cee_Unused62	= 0xca,
        Cee_Unused63	= 0xcb,
        Cee_Unused64	= 0xcc,
        Cee_Unused65	= 0xcd,
        Cee_Unused66	= 0xce,
        Cee_Unused67	= 0xcf,
        Cee_Ldtoken	= 0xd0,
        Cee_Conv_U2	= 0xd1,
        Cee_Conv_U1	= 0xd2,
        Cee_Conv_I	= 0xd3,
        Cee_Conv_Ovf_I	= 0xd4,
        Cee_Conv_Ovf_U	= 0xd5,
        Cee_Add_Ovf	= 0xd6,
        Cee_Add_Ovf_Un	= 0xd7,
        Cee_Mul_Ovf	= 0xd8,
        Cee_Mul_Ovf_Un	= 0xd9,
        Cee_Sub_Ovf	= 0xda,
        Cee_Sub_Ovf_Un	= 0xdb,
        Cee_Endfinally	= 0xdc,
        Cee_Leave	= 0xdd,
        Cee_Leave_S	= 0xde,
        Cee_Stind_I	= 0xdf,
        Cee_Conv_U	= 0xe0,
        Cee_Unused26	= 0xe1,
        Cee_Unused27	= 0xe2,
        Cee_Unused28	= 0xe3,
        Cee_Unused29	= 0xe4,
        Cee_Unused30	= 0xe5,
        Cee_Unused31	= 0xe6,
        Cee_Unused32	= 0xe7,
        Cee_Unused33	= 0xe8,
        Cee_Unused34	= 0xe9,
        Cee_Unused35	= 0xea,
        Cee_Unused36	= 0xeb,
        Cee_Unused37	= 0xec,
        Cee_Unused38	= 0xed,
        Cee_Unused39	= 0xee,
        Cee_Unused40	= 0xef,
        Cee_Unused41	= 0xf0,
        Cee_Unused42	= 0xf1,
        Cee_Unused43	= 0xf2,
        Cee_Unused44	= 0xf3,
        Cee_Unused45	= 0xf4,
        Cee_Unused46	= 0xf5,
        Cee_Unused47	= 0xf6,
        Cee_Unused48	= 0xf7,
        Cee_Prefix7	= 0xf8,
        Cee_Prefix6	= 0xf9,
        Cee_Prefix5	= 0xfa,
        Cee_Prefix4	= 0xfb,
        Cee_Prefix3	= 0xfc,
        Cee_Prefix2	= 0xfd,
        Cee_Prefix1	= 0xfe,
        Cee_Prefixref	= 0xff,
        Cee_Arglist	= 0x100,
        Cee_Ceq	= 0x101,
        Cee_Cgt	= 0x102,
        Cee_Cgt_Un	= 0x103,
        Cee_Clt	= 0x104,
        Cee_Clt_Un	= 0x105,
        Cee_Ldftn	= 0x106,
        Cee_Ldvirtftn	= 0x107,
        Cee_Unused56	= 0x108,
        Cee_Ldarg	= 0x109,
        Cee_Ldarga	= 0x10a,
        Cee_Starg	= 0x10b,
        Cee_Ldloc	= 0x10c,
        Cee_Ldloca	= 0x10d,
        Cee_Stloc	= 0x10e,
        Cee_Localloc	= 0x10f,
        Cee_Unused57	= 0x110,
        Cee_Endfilter	= 0x111,
        Cee_Unaligned	= 0x112,
        Cee_Volatile	= 0x113,
        Cee_Tailcall	= 0x114,
        Cee_Initobj	= 0x115,
        Cee_Constrained	= 0x116,
        Cee_Cpblk	= 0x117,
        Cee_Initblk	= 0x118,
        Cee_Unused69	= 0x119,
        Cee_Rethrow	= 0x11a,
        Cee_Unused51	= 0x11b,
        Cee_Sizeof	= 0x11c,
        Cee_Refanytype	= 0x11d,
        Cee_Readonly	= 0x11e,
        Cee_Unused53	= 0x11f,
        Cee_Unused54	= 0x120,
        Cee_Unused55	= 0x121,
        Cee_Unused70	= 0x122,
        Cee_Count	= 0x123,
        Cee_Invalid	= 0xffff
    } ;

enum ILOperandType
    {
        ILOperandType_None	= 0,
        ILOperandType_Byte	= ( ILOperandType_None + 1 ) ,
        ILOperandType_Int	= ( ILOperandType_Byte + 1 ) ,
        ILOperandType_UShort	= ( ILOperandType_Int + 1 ) ,
        ILOperandType_Long	= ( ILOperandType_UShort + 1 ) ,
        ILOperandType_Single	= ( ILOperandType_Long + 1 ) ,
        ILOperandType_Double	= ( ILOperandType_Single + 1 ) ,
        ILOperandType_Token	= ( ILOperandType_Double + 1 ) ,
        ILOperandType_Switch	= ( ILOperandType_Token + 1 ) 
    } ;

enum ILOpcodeFlags
    {
        ILOpcodeFlag_None	= 0,
        ILOpcodeFlag_Meta	= 0x1,
        ILOpcodeFlag_Unused	= 0x2,
        ILOpcodeFlag_Branch	= 0x4
    } ;

enum InstructionGeneration
    {
        Generation_Original	= 0x1,
        Generation_Baseline	= 0x2,
        Generation_New	= 0x3
    } ;

enum InstructionTerminationType
    {
        TerminationType_FallThrough	= 0,
        TerminationType_Branch	= ( TerminationType_FallThrough + 1 ) ,
        TerminationType_ConditionalBranch	= ( TerminationType_Branch + 1 ) ,
        TerminationType_Throw	= ( TerminationType_ConditionalBranch + 1 ) ,
        TerminationType_Switch	= ( TerminationType_Throw + 1 ) ,
        TerminationType_Call	= ( TerminationType_Switch + 1 ) ,
        TerminationType_IndirectCall	= ( TerminationType_Call + 1 ) ,
        TerminationType_Return	= ( TerminationType_IndirectCall + 1 ) ,
        TerminationType_Trap	= ( TerminationType_Return + 1 ) 
    } ;

EXTERN_C const IID LIBID_MicrosoftInstrumentationEngine;

#ifndef __IProfilerManager_INTERFACE_DEFINED__
#define __IProfilerManager_INTERFACE_DEFINED__

/* interface IProfilerManager */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IProfilerManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3A09AD0A-25C6-4093-93E1-3F64EB160A9D")
    IProfilerManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetupProfilingEnvironment( 
            /* [in] */ BSTR bstrConfigPath[  ],
            /* [in] */ UINT numConfigPaths) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddRawProfilerHook( 
            /* [in] */ IUnknown *pUnkProfilerCallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveRawProfilerHook( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorProfilerInfo( 
            /* [out] */ IUnknown **ppCorProfiler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProfilerHost( 
            /* [out] */ IProfilerManagerHost **ppProfilerManagerHost) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLoggingInstance( 
            /* [out] */ IProfilerManagerLogging **ppLogging) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLoggingHost( 
            /* [in] */ IProfilerManagerLoggingHost *pLoggingHost) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainCollection( 
            /* [out] */ IAppDomainCollection **ppAppDomainCollection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateSignatureBuilder( 
            /* [out] */ ISignatureBuilder **ppSignatureBuilder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstrumentationMethod( 
            /* [in] */ REFGUID cslid,
            /* [out] */ IUnknown **ppUnknown) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveInstrumentationMethod( 
            /* [in] */ IInstrumentationMethod *pInstrumentationMethod) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddInstrumentationMethod( 
            /* [in] */ BSTR bstrModulePath,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrDescription,
            /* [in] */ BSTR bstrModule,
            /* [in] */ BSTR bstrClassGuid,
            /* [in] */ DWORD dwPriority,
            /* [out] */ IInstrumentationMethod **pInstrumentationMethod) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRuntimeType( 
            /* [out] */ COR_PRF_RUNTIME_TYPE *pRuntimeType) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProfilerManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfilerManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfilerManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfilerManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetupProfilingEnvironment )( 
            IProfilerManager * This,
            /* [in] */ BSTR bstrConfigPath[  ],
            /* [in] */ UINT numConfigPaths);
        
        HRESULT ( STDMETHODCALLTYPE *AddRawProfilerHook )( 
            IProfilerManager * This,
            /* [in] */ IUnknown *pUnkProfilerCallback);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveRawProfilerHook )( 
            IProfilerManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorProfilerInfo )( 
            IProfilerManager * This,
            /* [out] */ IUnknown **ppCorProfiler);
        
        HRESULT ( STDMETHODCALLTYPE *GetProfilerHost )( 
            IProfilerManager * This,
            /* [out] */ IProfilerManagerHost **ppProfilerManagerHost);
        
        HRESULT ( STDMETHODCALLTYPE *GetLoggingInstance )( 
            IProfilerManager * This,
            /* [out] */ IProfilerManagerLogging **ppLogging);
        
        HRESULT ( STDMETHODCALLTYPE *SetLoggingHost )( 
            IProfilerManager * This,
            /* [in] */ IProfilerManagerLoggingHost *pLoggingHost);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainCollection )( 
            IProfilerManager * This,
            /* [out] */ IAppDomainCollection **ppAppDomainCollection);
        
        HRESULT ( STDMETHODCALLTYPE *CreateSignatureBuilder )( 
            IProfilerManager * This,
            /* [out] */ ISignatureBuilder **ppSignatureBuilder);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstrumentationMethod )( 
            IProfilerManager * This,
            /* [in] */ REFGUID cslid,
            /* [out] */ IUnknown **ppUnknown);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveInstrumentationMethod )( 
            IProfilerManager * This,
            /* [in] */ IInstrumentationMethod *pInstrumentationMethod);
        
        HRESULT ( STDMETHODCALLTYPE *AddInstrumentationMethod )( 
            IProfilerManager * This,
            /* [in] */ BSTR bstrModulePath,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrDescription,
            /* [in] */ BSTR bstrModule,
            /* [in] */ BSTR bstrClassGuid,
            /* [in] */ DWORD dwPriority,
            /* [out] */ IInstrumentationMethod **pInstrumentationMethod);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeType )( 
            IProfilerManager * This,
            /* [out] */ COR_PRF_RUNTIME_TYPE *pRuntimeType);
        
        END_INTERFACE
    } IProfilerManagerVtbl;

    interface IProfilerManager
    {
        CONST_VTBL struct IProfilerManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfilerManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProfilerManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProfilerManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProfilerManager_SetupProfilingEnvironment(This,bstrConfigPath,numConfigPaths)	\
    ( (This)->lpVtbl -> SetupProfilingEnvironment(This,bstrConfigPath,numConfigPaths) ) 

#define IProfilerManager_AddRawProfilerHook(This,pUnkProfilerCallback)	\
    ( (This)->lpVtbl -> AddRawProfilerHook(This,pUnkProfilerCallback) ) 

#define IProfilerManager_RemoveRawProfilerHook(This)	\
    ( (This)->lpVtbl -> RemoveRawProfilerHook(This) ) 

#define IProfilerManager_GetCorProfilerInfo(This,ppCorProfiler)	\
    ( (This)->lpVtbl -> GetCorProfilerInfo(This,ppCorProfiler) ) 

#define IProfilerManager_GetProfilerHost(This,ppProfilerManagerHost)	\
    ( (This)->lpVtbl -> GetProfilerHost(This,ppProfilerManagerHost) ) 

#define IProfilerManager_GetLoggingInstance(This,ppLogging)	\
    ( (This)->lpVtbl -> GetLoggingInstance(This,ppLogging) ) 

#define IProfilerManager_SetLoggingHost(This,pLoggingHost)	\
    ( (This)->lpVtbl -> SetLoggingHost(This,pLoggingHost) ) 

#define IProfilerManager_GetAppDomainCollection(This,ppAppDomainCollection)	\
    ( (This)->lpVtbl -> GetAppDomainCollection(This,ppAppDomainCollection) ) 

#define IProfilerManager_CreateSignatureBuilder(This,ppSignatureBuilder)	\
    ( (This)->lpVtbl -> CreateSignatureBuilder(This,ppSignatureBuilder) ) 

#define IProfilerManager_GetInstrumentationMethod(This,cslid,ppUnknown)	\
    ( (This)->lpVtbl -> GetInstrumentationMethod(This,cslid,ppUnknown) ) 

#define IProfilerManager_RemoveInstrumentationMethod(This,pInstrumentationMethod)	\
    ( (This)->lpVtbl -> RemoveInstrumentationMethod(This,pInstrumentationMethod) ) 

#define IProfilerManager_AddInstrumentationMethod(This,bstrModulePath,bstrName,bstrDescription,bstrModule,bstrClassGuid,dwPriority,pInstrumentationMethod)	\
    ( (This)->lpVtbl -> AddInstrumentationMethod(This,bstrModulePath,bstrName,bstrDescription,bstrModule,bstrClassGuid,dwPriority,pInstrumentationMethod) ) 

#define IProfilerManager_GetRuntimeType(This,pRuntimeType)	\
    ( (This)->lpVtbl -> GetRuntimeType(This,pRuntimeType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProfilerManager_INTERFACE_DEFINED__ */


#ifndef __IProfilerManagerHost_INTERFACE_DEFINED__
#define __IProfilerManagerHost_INTERFACE_DEFINED__

/* interface IProfilerManagerHost */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IProfilerManagerHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BA9193B4-287F-4BF4-8E1B-00FCE33862EB")
    IProfilerManagerHost : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IProfilerManager *pProfilerManager) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProfilerManagerHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfilerManagerHost * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfilerManagerHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfilerManagerHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IProfilerManagerHost * This,
            /* [in] */ IProfilerManager *pProfilerManager);
        
        END_INTERFACE
    } IProfilerManagerHostVtbl;

    interface IProfilerManagerHost
    {
        CONST_VTBL struct IProfilerManagerHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfilerManagerHost_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProfilerManagerHost_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProfilerManagerHost_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProfilerManagerHost_Initialize(This,pProfilerManager)	\
    ( (This)->lpVtbl -> Initialize(This,pProfilerManager) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProfilerManagerHost_INTERFACE_DEFINED__ */


#ifndef __IProfilerManagerLogging_INTERFACE_DEFINED__
#define __IProfilerManagerLogging_INTERFACE_DEFINED__

/* interface IProfilerManagerLogging */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IProfilerManagerLogging;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9CFECED7-2123-4115-BF06-3693D1D19E22")
    IProfilerManagerLogging : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LogMessage( 
            /* [in] */ const WCHAR *wszMessage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogError( 
            /* [in] */ const WCHAR *wszError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogDumpMessage( 
            /* [in] */ const WCHAR *wszMessage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableDiagnosticLogToDebugPort( 
            /* [in] */ BOOL enable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLoggingFlags( 
            /* [out] */ enum LoggingFlags *pLoggingFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLoggingFlags( 
            /* [in] */ enum LoggingFlags loggingFlags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProfilerManagerLoggingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfilerManagerLogging * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfilerManagerLogging * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfilerManagerLogging * This);
        
        HRESULT ( STDMETHODCALLTYPE *LogMessage )( 
            IProfilerManagerLogging * This,
            /* [in] */ const WCHAR *wszMessage);
        
        HRESULT ( STDMETHODCALLTYPE *LogError )( 
            IProfilerManagerLogging * This,
            /* [in] */ const WCHAR *wszError);
        
        HRESULT ( STDMETHODCALLTYPE *LogDumpMessage )( 
            IProfilerManagerLogging * This,
            /* [in] */ const WCHAR *wszMessage);
        
        HRESULT ( STDMETHODCALLTYPE *EnableDiagnosticLogToDebugPort )( 
            IProfilerManagerLogging * This,
            /* [in] */ BOOL enable);
        
        HRESULT ( STDMETHODCALLTYPE *GetLoggingFlags )( 
            IProfilerManagerLogging * This,
            /* [out] */ enum LoggingFlags *pLoggingFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetLoggingFlags )( 
            IProfilerManagerLogging * This,
            /* [in] */ enum LoggingFlags loggingFlags);
        
        END_INTERFACE
    } IProfilerManagerLoggingVtbl;

    interface IProfilerManagerLogging
    {
        CONST_VTBL struct IProfilerManagerLoggingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfilerManagerLogging_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProfilerManagerLogging_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProfilerManagerLogging_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProfilerManagerLogging_LogMessage(This,wszMessage)	\
    ( (This)->lpVtbl -> LogMessage(This,wszMessage) ) 

#define IProfilerManagerLogging_LogError(This,wszError)	\
    ( (This)->lpVtbl -> LogError(This,wszError) ) 

#define IProfilerManagerLogging_LogDumpMessage(This,wszMessage)	\
    ( (This)->lpVtbl -> LogDumpMessage(This,wszMessage) ) 

#define IProfilerManagerLogging_EnableDiagnosticLogToDebugPort(This,enable)	\
    ( (This)->lpVtbl -> EnableDiagnosticLogToDebugPort(This,enable) ) 

#define IProfilerManagerLogging_GetLoggingFlags(This,pLoggingFlags)	\
    ( (This)->lpVtbl -> GetLoggingFlags(This,pLoggingFlags) ) 

#define IProfilerManagerLogging_SetLoggingFlags(This,loggingFlags)	\
    ( (This)->lpVtbl -> SetLoggingFlags(This,loggingFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProfilerManagerLogging_INTERFACE_DEFINED__ */


#ifndef __IProfilerManagerLoggingHost_INTERFACE_DEFINED__
#define __IProfilerManagerLoggingHost_INTERFACE_DEFINED__

/* interface IProfilerManagerLoggingHost */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IProfilerManagerLoggingHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("99F828EE-EA00-473C-A829-D400235C11C1")
    IProfilerManagerLoggingHost : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LogMessage( 
            /* [in] */ const WCHAR *wszMessage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogError( 
            /* [in] */ const WCHAR *wszError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogDumpMessage( 
            /* [in] */ const WCHAR *wszMessage) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProfilerManagerLoggingHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfilerManagerLoggingHost * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfilerManagerLoggingHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfilerManagerLoggingHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *LogMessage )( 
            IProfilerManagerLoggingHost * This,
            /* [in] */ const WCHAR *wszMessage);
        
        HRESULT ( STDMETHODCALLTYPE *LogError )( 
            IProfilerManagerLoggingHost * This,
            /* [in] */ const WCHAR *wszError);
        
        HRESULT ( STDMETHODCALLTYPE *LogDumpMessage )( 
            IProfilerManagerLoggingHost * This,
            /* [in] */ const WCHAR *wszMessage);
        
        END_INTERFACE
    } IProfilerManagerLoggingHostVtbl;

    interface IProfilerManagerLoggingHost
    {
        CONST_VTBL struct IProfilerManagerLoggingHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfilerManagerLoggingHost_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProfilerManagerLoggingHost_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProfilerManagerLoggingHost_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProfilerManagerLoggingHost_LogMessage(This,wszMessage)	\
    ( (This)->lpVtbl -> LogMessage(This,wszMessage) ) 

#define IProfilerManagerLoggingHost_LogError(This,wszError)	\
    ( (This)->lpVtbl -> LogError(This,wszError) ) 

#define IProfilerManagerLoggingHost_LogDumpMessage(This,wszMessage)	\
    ( (This)->lpVtbl -> LogDumpMessage(This,wszMessage) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProfilerManagerLoggingHost_INTERFACE_DEFINED__ */


#ifndef __IInstrumentationMethod_INTERFACE_DEFINED__
#define __IInstrumentationMethod_INTERFACE_DEFINED__

/* interface IInstrumentationMethod */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IInstrumentationMethod;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0D92A8D9-6645-4803-B94B-06A1C4F4E633")
    IInstrumentationMethod : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IProfilerManager *pProfilerManager) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnAppDomainCreated( 
            /* [in] */ IAppDomainInfo *pAppDomainInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnAppDomainShutdown( 
            /* [in] */ IAppDomainInfo *pAppDomainInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnAssemblyLoaded( 
            /* [in] */ IAssemblyInfo *pAssemblyInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnAssemblyUnloaded( 
            /* [in] */ IAssemblyInfo *pAssemblyInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnModuleLoaded( 
            /* [in] */ IModuleInfo *pModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnModuleUnloaded( 
            /* [in] */ IModuleInfo *pModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnShutdown( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShouldInstrumentMethod( 
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit,
            /* [out] */ BOOL *pbInstrument) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BeforeInstrumentMethod( 
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InstrumentMethod( 
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnInstrumentationComplete( 
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AllowInlineSite( 
            /* [in] */ IMethodInfo *pMethodInfoInlinee,
            /* [in] */ IMethodInfo *pMethodInfoCaller,
            /* [out] */ BOOL *pbAllowInline) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInstrumentationMethodVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInstrumentationMethod * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInstrumentationMethod * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInstrumentationMethod * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IInstrumentationMethod * This,
            /* [in] */ IProfilerManager *pProfilerManager);
        
        HRESULT ( STDMETHODCALLTYPE *OnAppDomainCreated )( 
            IInstrumentationMethod * This,
            /* [in] */ IAppDomainInfo *pAppDomainInfo);
        
        HRESULT ( STDMETHODCALLTYPE *OnAppDomainShutdown )( 
            IInstrumentationMethod * This,
            /* [in] */ IAppDomainInfo *pAppDomainInfo);
        
        HRESULT ( STDMETHODCALLTYPE *OnAssemblyLoaded )( 
            IInstrumentationMethod * This,
            /* [in] */ IAssemblyInfo *pAssemblyInfo);
        
        HRESULT ( STDMETHODCALLTYPE *OnAssemblyUnloaded )( 
            IInstrumentationMethod * This,
            /* [in] */ IAssemblyInfo *pAssemblyInfo);
        
        HRESULT ( STDMETHODCALLTYPE *OnModuleLoaded )( 
            IInstrumentationMethod * This,
            /* [in] */ IModuleInfo *pModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *OnModuleUnloaded )( 
            IInstrumentationMethod * This,
            /* [in] */ IModuleInfo *pModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *OnShutdown )( 
            IInstrumentationMethod * This);
        
        HRESULT ( STDMETHODCALLTYPE *ShouldInstrumentMethod )( 
            IInstrumentationMethod * This,
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit,
            /* [out] */ BOOL *pbInstrument);
        
        HRESULT ( STDMETHODCALLTYPE *BeforeInstrumentMethod )( 
            IInstrumentationMethod * This,
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit);
        
        HRESULT ( STDMETHODCALLTYPE *InstrumentMethod )( 
            IInstrumentationMethod * This,
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit);
        
        HRESULT ( STDMETHODCALLTYPE *OnInstrumentationComplete )( 
            IInstrumentationMethod * This,
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ BOOL isRejit);
        
        HRESULT ( STDMETHODCALLTYPE *AllowInlineSite )( 
            IInstrumentationMethod * This,
            /* [in] */ IMethodInfo *pMethodInfoInlinee,
            /* [in] */ IMethodInfo *pMethodInfoCaller,
            /* [out] */ BOOL *pbAllowInline);
        
        END_INTERFACE
    } IInstrumentationMethodVtbl;

    interface IInstrumentationMethod
    {
        CONST_VTBL struct IInstrumentationMethodVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInstrumentationMethod_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInstrumentationMethod_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInstrumentationMethod_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInstrumentationMethod_Initialize(This,pProfilerManager)	\
    ( (This)->lpVtbl -> Initialize(This,pProfilerManager) ) 

#define IInstrumentationMethod_OnAppDomainCreated(This,pAppDomainInfo)	\
    ( (This)->lpVtbl -> OnAppDomainCreated(This,pAppDomainInfo) ) 

#define IInstrumentationMethod_OnAppDomainShutdown(This,pAppDomainInfo)	\
    ( (This)->lpVtbl -> OnAppDomainShutdown(This,pAppDomainInfo) ) 

#define IInstrumentationMethod_OnAssemblyLoaded(This,pAssemblyInfo)	\
    ( (This)->lpVtbl -> OnAssemblyLoaded(This,pAssemblyInfo) ) 

#define IInstrumentationMethod_OnAssemblyUnloaded(This,pAssemblyInfo)	\
    ( (This)->lpVtbl -> OnAssemblyUnloaded(This,pAssemblyInfo) ) 

#define IInstrumentationMethod_OnModuleLoaded(This,pModuleInfo)	\
    ( (This)->lpVtbl -> OnModuleLoaded(This,pModuleInfo) ) 

#define IInstrumentationMethod_OnModuleUnloaded(This,pModuleInfo)	\
    ( (This)->lpVtbl -> OnModuleUnloaded(This,pModuleInfo) ) 

#define IInstrumentationMethod_OnShutdown(This)	\
    ( (This)->lpVtbl -> OnShutdown(This) ) 

#define IInstrumentationMethod_ShouldInstrumentMethod(This,pMethodInfo,isRejit,pbInstrument)	\
    ( (This)->lpVtbl -> ShouldInstrumentMethod(This,pMethodInfo,isRejit,pbInstrument) ) 

#define IInstrumentationMethod_BeforeInstrumentMethod(This,pMethodInfo,isRejit)	\
    ( (This)->lpVtbl -> BeforeInstrumentMethod(This,pMethodInfo,isRejit) ) 

#define IInstrumentationMethod_InstrumentMethod(This,pMethodInfo,isRejit)	\
    ( (This)->lpVtbl -> InstrumentMethod(This,pMethodInfo,isRejit) ) 

#define IInstrumentationMethod_OnInstrumentationComplete(This,pMethodInfo,isRejit)	\
    ( (This)->lpVtbl -> OnInstrumentationComplete(This,pMethodInfo,isRejit) ) 

#define IInstrumentationMethod_AllowInlineSite(This,pMethodInfoInlinee,pMethodInfoCaller,pbAllowInline)	\
    ( (This)->lpVtbl -> AllowInlineSite(This,pMethodInfoInlinee,pMethodInfoCaller,pbAllowInline) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInstrumentationMethod_INTERFACE_DEFINED__ */


#ifndef __IDataContainer_INTERFACE_DEFINED__
#define __IDataContainer_INTERFACE_DEFINED__

/* interface IDataContainer */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDataContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A4FDF66-FC5B-442D-8FAA-4137F023A4EA")
    IDataContainer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetDataItem( 
            /* [in] */ const GUID *componentId,
            /* [in] */ const GUID *objectGuid,
            /* [in] */ IUnknown *pDataItem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDataItem( 
            /* [in] */ const GUID *componentId,
            /* [in] */ const GUID *objectGuid,
            /* [out] */ IUnknown **ppDataItem) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDataContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDataContainer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDataContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDataContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetDataItem )( 
            IDataContainer * This,
            /* [in] */ const GUID *componentId,
            /* [in] */ const GUID *objectGuid,
            /* [in] */ IUnknown *pDataItem);
        
        HRESULT ( STDMETHODCALLTYPE *GetDataItem )( 
            IDataContainer * This,
            /* [in] */ const GUID *componentId,
            /* [in] */ const GUID *objectGuid,
            /* [out] */ IUnknown **ppDataItem);
        
        END_INTERFACE
    } IDataContainerVtbl;

    interface IDataContainer
    {
        CONST_VTBL struct IDataContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataContainer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDataContainer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDataContainer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDataContainer_SetDataItem(This,componentId,objectGuid,pDataItem)	\
    ( (This)->lpVtbl -> SetDataItem(This,componentId,objectGuid,pDataItem) ) 

#define IDataContainer_GetDataItem(This,componentId,objectGuid,ppDataItem)	\
    ( (This)->lpVtbl -> GetDataItem(This,componentId,objectGuid,ppDataItem) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDataContainer_INTERFACE_DEFINED__ */


#ifndef __IInstruction_INTERFACE_DEFINED__
#define __IInstruction_INTERFACE_DEFINED__

/* interface IInstruction */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IInstruction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E80D8434-2976-4242-8F3B-0C837C343F6C")
    IInstruction : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetOffset( 
            /* [out] */ DWORD *pdwOffset) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOriginalOffset( 
            /* [out] */ DWORD *pdwOffset) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOpCodeName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOpCode( 
            /* [out] */ enum ILOrdinalOpcode *pOpCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAlternateOrdinalOpcode( 
            /* [out] */ enum ILOrdinalOpcode *pOpCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstructionLength( 
            /* [out] */ DWORD *pdwLength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOpcodeFlags( 
            /* [out] */ enum ILOpcodeFlags *pFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOpcodeLength( 
            /* [out] */ DWORD *pdwLength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOperandType( 
            /* [out] */ enum ILOperandType *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOperandLength( 
            /* [out] */ DWORD *pdwLength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsNew( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsRemoved( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsBranch( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsSwitch( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsCallInstruction( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTerminationType( 
            /* [out] */ enum InstructionTerminationType *pTerminationType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsFallThrough( 
            /* [out] */ BOOL *pbIsFallThrough) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextInstruction( 
            /* [out] */ IInstruction **ppNextInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPreviousInstruction( 
            /* [out] */ IInstruction **ppPrevInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOriginalNextInstruction( 
            /* [out] */ IInstruction **ppNextInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOriginalPreviousInstruction( 
            /* [out] */ IInstruction **ppPrevInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstructionGeneration( 
            /* [out] */ enum InstructionGeneration *pInstructionGeneration) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInstruction * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInstruction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInstruction * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetOffset )( 
            IInstruction * This,
            /* [out] */ DWORD *pdwOffset);
        
        HRESULT ( STDMETHODCALLTYPE *GetOriginalOffset )( 
            IInstruction * This,
            /* [out] */ DWORD *pdwOffset);
        
        HRESULT ( STDMETHODCALLTYPE *GetOpCodeName )( 
            IInstruction * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetOpCode )( 
            IInstruction * This,
            /* [out] */ enum ILOrdinalOpcode *pOpCode);
        
        HRESULT ( STDMETHODCALLTYPE *GetAlternateOrdinalOpcode )( 
            IInstruction * This,
            /* [out] */ enum ILOrdinalOpcode *pOpCode);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructionLength )( 
            IInstruction * This,
            /* [out] */ DWORD *pdwLength);
        
        HRESULT ( STDMETHODCALLTYPE *GetOpcodeFlags )( 
            IInstruction * This,
            /* [out] */ enum ILOpcodeFlags *pFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetOpcodeLength )( 
            IInstruction * This,
            /* [out] */ DWORD *pdwLength);
        
        HRESULT ( STDMETHODCALLTYPE *GetOperandType )( 
            IInstruction * This,
            /* [out] */ enum ILOperandType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetOperandLength )( 
            IInstruction * This,
            /* [out] */ DWORD *pdwLength);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsNew )( 
            IInstruction * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsRemoved )( 
            IInstruction * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsBranch )( 
            IInstruction * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsSwitch )( 
            IInstruction * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsCallInstruction )( 
            IInstruction * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetTerminationType )( 
            IInstruction * This,
            /* [out] */ enum InstructionTerminationType *pTerminationType);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsFallThrough )( 
            IInstruction * This,
            /* [out] */ BOOL *pbIsFallThrough);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextInstruction )( 
            IInstruction * This,
            /* [out] */ IInstruction **ppNextInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetPreviousInstruction )( 
            IInstruction * This,
            /* [out] */ IInstruction **ppPrevInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetOriginalNextInstruction )( 
            IInstruction * This,
            /* [out] */ IInstruction **ppNextInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetOriginalPreviousInstruction )( 
            IInstruction * This,
            /* [out] */ IInstruction **ppPrevInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructionGeneration )( 
            IInstruction * This,
            /* [out] */ enum InstructionGeneration *pInstructionGeneration);
        
        END_INTERFACE
    } IInstructionVtbl;

    interface IInstruction
    {
        CONST_VTBL struct IInstructionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInstruction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInstruction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInstruction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInstruction_GetOffset(This,pdwOffset)	\
    ( (This)->lpVtbl -> GetOffset(This,pdwOffset) ) 

#define IInstruction_GetOriginalOffset(This,pdwOffset)	\
    ( (This)->lpVtbl -> GetOriginalOffset(This,pdwOffset) ) 

#define IInstruction_GetOpCodeName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetOpCodeName(This,pbstrName) ) 

#define IInstruction_GetOpCode(This,pOpCode)	\
    ( (This)->lpVtbl -> GetOpCode(This,pOpCode) ) 

#define IInstruction_GetAlternateOrdinalOpcode(This,pOpCode)	\
    ( (This)->lpVtbl -> GetAlternateOrdinalOpcode(This,pOpCode) ) 

#define IInstruction_GetInstructionLength(This,pdwLength)	\
    ( (This)->lpVtbl -> GetInstructionLength(This,pdwLength) ) 

#define IInstruction_GetOpcodeFlags(This,pFlags)	\
    ( (This)->lpVtbl -> GetOpcodeFlags(This,pFlags) ) 

#define IInstruction_GetOpcodeLength(This,pdwLength)	\
    ( (This)->lpVtbl -> GetOpcodeLength(This,pdwLength) ) 

#define IInstruction_GetOperandType(This,pType)	\
    ( (This)->lpVtbl -> GetOperandType(This,pType) ) 

#define IInstruction_GetOperandLength(This,pdwLength)	\
    ( (This)->lpVtbl -> GetOperandLength(This,pdwLength) ) 

#define IInstruction_GetIsNew(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsNew(This,pbValue) ) 

#define IInstruction_GetIsRemoved(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsRemoved(This,pbValue) ) 

#define IInstruction_GetIsBranch(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsBranch(This,pbValue) ) 

#define IInstruction_GetIsSwitch(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsSwitch(This,pbValue) ) 

#define IInstruction_GetIsCallInstruction(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsCallInstruction(This,pbValue) ) 

#define IInstruction_GetTerminationType(This,pTerminationType)	\
    ( (This)->lpVtbl -> GetTerminationType(This,pTerminationType) ) 

#define IInstruction_GetIsFallThrough(This,pbIsFallThrough)	\
    ( (This)->lpVtbl -> GetIsFallThrough(This,pbIsFallThrough) ) 

#define IInstruction_GetNextInstruction(This,ppNextInstruction)	\
    ( (This)->lpVtbl -> GetNextInstruction(This,ppNextInstruction) ) 

#define IInstruction_GetPreviousInstruction(This,ppPrevInstruction)	\
    ( (This)->lpVtbl -> GetPreviousInstruction(This,ppPrevInstruction) ) 

#define IInstruction_GetOriginalNextInstruction(This,ppNextInstruction)	\
    ( (This)->lpVtbl -> GetOriginalNextInstruction(This,ppNextInstruction) ) 

#define IInstruction_GetOriginalPreviousInstruction(This,ppPrevInstruction)	\
    ( (This)->lpVtbl -> GetOriginalPreviousInstruction(This,ppPrevInstruction) ) 

#define IInstruction_GetInstructionGeneration(This,pInstructionGeneration)	\
    ( (This)->lpVtbl -> GetInstructionGeneration(This,pInstructionGeneration) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInstruction_INTERFACE_DEFINED__ */


#ifndef __IExceptionSection_INTERFACE_DEFINED__
#define __IExceptionSection_INTERFACE_DEFINED__

/* interface IExceptionSection */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IExceptionSection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("42CE95A2-F814-4DCD-952F-68CE9801FDD3")
    IExceptionSection : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMethodInfo( 
            /* [out] */ IMethodInfo **ppMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExceptionClauses( 
            /* [out] */ IEnumExceptionClauses **ppEnumExceptionClauses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddExceptionClause( 
            /* [in] */ IExceptionClause *pExceptionClause) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveExceptionClause( 
            /* [in] */ IExceptionClause *pExceptionClause) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAllExceptionClauses( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddNewExceptionClause( 
            /* [in] */ DWORD flags,
            /* [in] */ IInstruction *pTryFirstInstruction,
            /* [in] */ IInstruction *pTryLastInstruction,
            /* [in] */ IInstruction *pHandlerFirstInstruction,
            /* [in] */ IInstruction *pHandlerLastInstruction,
            /* [in] */ IInstruction *pFilterLastInstruction,
            /* [in] */ mdToken handlerTypeToken,
            /* [out] */ IExceptionClause **ppExceptionClause) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IExceptionSectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IExceptionSection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IExceptionSection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IExceptionSection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodInfo )( 
            IExceptionSection * This,
            /* [out] */ IMethodInfo **ppMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetExceptionClauses )( 
            IExceptionSection * This,
            /* [out] */ IEnumExceptionClauses **ppEnumExceptionClauses);
        
        HRESULT ( STDMETHODCALLTYPE *AddExceptionClause )( 
            IExceptionSection * This,
            /* [in] */ IExceptionClause *pExceptionClause);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveExceptionClause )( 
            IExceptionSection * This,
            /* [in] */ IExceptionClause *pExceptionClause);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAllExceptionClauses )( 
            IExceptionSection * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddNewExceptionClause )( 
            IExceptionSection * This,
            /* [in] */ DWORD flags,
            /* [in] */ IInstruction *pTryFirstInstruction,
            /* [in] */ IInstruction *pTryLastInstruction,
            /* [in] */ IInstruction *pHandlerFirstInstruction,
            /* [in] */ IInstruction *pHandlerLastInstruction,
            /* [in] */ IInstruction *pFilterLastInstruction,
            /* [in] */ mdToken handlerTypeToken,
            /* [out] */ IExceptionClause **ppExceptionClause);
        
        END_INTERFACE
    } IExceptionSectionVtbl;

    interface IExceptionSection
    {
        CONST_VTBL struct IExceptionSectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExceptionSection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IExceptionSection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IExceptionSection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IExceptionSection_GetMethodInfo(This,ppMethodInfo)	\
    ( (This)->lpVtbl -> GetMethodInfo(This,ppMethodInfo) ) 

#define IExceptionSection_GetExceptionClauses(This,ppEnumExceptionClauses)	\
    ( (This)->lpVtbl -> GetExceptionClauses(This,ppEnumExceptionClauses) ) 

#define IExceptionSection_AddExceptionClause(This,pExceptionClause)	\
    ( (This)->lpVtbl -> AddExceptionClause(This,pExceptionClause) ) 

#define IExceptionSection_RemoveExceptionClause(This,pExceptionClause)	\
    ( (This)->lpVtbl -> RemoveExceptionClause(This,pExceptionClause) ) 

#define IExceptionSection_RemoveAllExceptionClauses(This)	\
    ( (This)->lpVtbl -> RemoveAllExceptionClauses(This) ) 

#define IExceptionSection_AddNewExceptionClause(This,flags,pTryFirstInstruction,pTryLastInstruction,pHandlerFirstInstruction,pHandlerLastInstruction,pFilterLastInstruction,handlerTypeToken,ppExceptionClause)	\
    ( (This)->lpVtbl -> AddNewExceptionClause(This,flags,pTryFirstInstruction,pTryLastInstruction,pHandlerFirstInstruction,pHandlerLastInstruction,pFilterLastInstruction,handlerTypeToken,ppExceptionClause) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IExceptionSection_INTERFACE_DEFINED__ */


#ifndef __IExceptionClause_INTERFACE_DEFINED__
#define __IExceptionClause_INTERFACE_DEFINED__

/* interface IExceptionClause */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IExceptionClause;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1D57EAF6-FCFE-4874-AA0E-C9D1DF714950")
    IExceptionClause : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFlags( 
            /* [out] */ DWORD *pFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTryFirstInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTryLastInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHandlerFirstInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHandlerLastInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFilterFirstInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExceptionHandlerType( 
            /* [out] */ mdToken *pToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFlags( 
            /* [in] */ DWORD flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTryFirstInstruction( 
            /* [in] */ IInstruction *pInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTryLastInstruction( 
            /* [in] */ IInstruction *pInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHandlerFirstInstruction( 
            /* [in] */ IInstruction *pInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHandlerLastInstruction( 
            /* [in] */ IInstruction *pInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFilterFirstInstruction( 
            /* [in] */ IInstruction *pInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetExceptionHandlerType( 
            /* [in] */ mdToken token) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IExceptionClauseVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IExceptionClause * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IExceptionClause * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IExceptionClause * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFlags )( 
            IExceptionClause * This,
            /* [out] */ DWORD *pFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetTryFirstInstruction )( 
            IExceptionClause * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetTryLastInstruction )( 
            IExceptionClause * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetHandlerFirstInstruction )( 
            IExceptionClause * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetHandlerLastInstruction )( 
            IExceptionClause * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetFilterFirstInstruction )( 
            IExceptionClause * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetExceptionHandlerType )( 
            IExceptionClause * This,
            /* [out] */ mdToken *pToken);
        
        HRESULT ( STDMETHODCALLTYPE *SetFlags )( 
            IExceptionClause * This,
            /* [in] */ DWORD flags);
        
        HRESULT ( STDMETHODCALLTYPE *SetTryFirstInstruction )( 
            IExceptionClause * This,
            /* [in] */ IInstruction *pInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *SetTryLastInstruction )( 
            IExceptionClause * This,
            /* [in] */ IInstruction *pInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *SetHandlerFirstInstruction )( 
            IExceptionClause * This,
            /* [in] */ IInstruction *pInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *SetHandlerLastInstruction )( 
            IExceptionClause * This,
            /* [in] */ IInstruction *pInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *SetFilterFirstInstruction )( 
            IExceptionClause * This,
            /* [in] */ IInstruction *pInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *SetExceptionHandlerType )( 
            IExceptionClause * This,
            /* [in] */ mdToken token);
        
        END_INTERFACE
    } IExceptionClauseVtbl;

    interface IExceptionClause
    {
        CONST_VTBL struct IExceptionClauseVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExceptionClause_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IExceptionClause_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IExceptionClause_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IExceptionClause_GetFlags(This,pFlags)	\
    ( (This)->lpVtbl -> GetFlags(This,pFlags) ) 

#define IExceptionClause_GetTryFirstInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetTryFirstInstruction(This,ppInstruction) ) 

#define IExceptionClause_GetTryLastInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetTryLastInstruction(This,ppInstruction) ) 

#define IExceptionClause_GetHandlerFirstInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetHandlerFirstInstruction(This,ppInstruction) ) 

#define IExceptionClause_GetHandlerLastInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetHandlerLastInstruction(This,ppInstruction) ) 

#define IExceptionClause_GetFilterFirstInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetFilterFirstInstruction(This,ppInstruction) ) 

#define IExceptionClause_GetExceptionHandlerType(This,pToken)	\
    ( (This)->lpVtbl -> GetExceptionHandlerType(This,pToken) ) 

#define IExceptionClause_SetFlags(This,flags)	\
    ( (This)->lpVtbl -> SetFlags(This,flags) ) 

#define IExceptionClause_SetTryFirstInstruction(This,pInstruction)	\
    ( (This)->lpVtbl -> SetTryFirstInstruction(This,pInstruction) ) 

#define IExceptionClause_SetTryLastInstruction(This,pInstruction)	\
    ( (This)->lpVtbl -> SetTryLastInstruction(This,pInstruction) ) 

#define IExceptionClause_SetHandlerFirstInstruction(This,pInstruction)	\
    ( (This)->lpVtbl -> SetHandlerFirstInstruction(This,pInstruction) ) 

#define IExceptionClause_SetHandlerLastInstruction(This,pInstruction)	\
    ( (This)->lpVtbl -> SetHandlerLastInstruction(This,pInstruction) ) 

#define IExceptionClause_SetFilterFirstInstruction(This,pInstruction)	\
    ( (This)->lpVtbl -> SetFilterFirstInstruction(This,pInstruction) ) 

#define IExceptionClause_SetExceptionHandlerType(This,token)	\
    ( (This)->lpVtbl -> SetExceptionHandlerType(This,token) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IExceptionClause_INTERFACE_DEFINED__ */


#ifndef __IEnumExceptionClauses_INTERFACE_DEFINED__
#define __IEnumExceptionClauses_INTERFACE_DEFINED__

/* interface IEnumExceptionClauses */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumExceptionClauses;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("85B0B99F-73D7-4C69-8659-BF6196F5264F")
    IEnumExceptionClauses : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IExceptionClause **rgelt,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumExceptionClauses **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumExceptionClausesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumExceptionClauses * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumExceptionClauses * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumExceptionClauses * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumExceptionClauses * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IExceptionClause **rgelt,
            /* [in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumExceptionClauses * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumExceptionClauses * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumExceptionClauses * This,
            /* [out] */ IEnumExceptionClauses **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumExceptionClauses * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumExceptionClausesVtbl;

    interface IEnumExceptionClauses
    {
        CONST_VTBL struct IEnumExceptionClausesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumExceptionClauses_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumExceptionClauses_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumExceptionClauses_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumExceptionClauses_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumExceptionClauses_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumExceptionClauses_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumExceptionClauses_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumExceptionClauses_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumExceptionClauses_INTERFACE_DEFINED__ */


#ifndef __IOperandInstruction_INTERFACE_DEFINED__
#define __IOperandInstruction_INTERFACE_DEFINED__

/* interface IOperandInstruction */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IOperandInstruction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1F014299-F383-46CE-B7A6-1982C85F9FEA")
    IOperandInstruction : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetOperandType( 
            /* [out] */ enum ILOperandType *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOperandValue( 
            /* [in] */ DWORD dwSize,
            /* [length_is][size_is][out] */ BYTE *pBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOperandValue( 
            /* [in] */ DWORD dwSize,
            /* [length_is][size_is][in] */ BYTE *pBytes) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IOperandInstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOperandInstruction * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOperandInstruction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOperandInstruction * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetOperandType )( 
            IOperandInstruction * This,
            /* [out] */ enum ILOperandType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetOperandValue )( 
            IOperandInstruction * This,
            /* [in] */ DWORD dwSize,
            /* [length_is][size_is][out] */ BYTE *pBytes);
        
        HRESULT ( STDMETHODCALLTYPE *SetOperandValue )( 
            IOperandInstruction * This,
            /* [in] */ DWORD dwSize,
            /* [length_is][size_is][in] */ BYTE *pBytes);
        
        END_INTERFACE
    } IOperandInstructionVtbl;

    interface IOperandInstruction
    {
        CONST_VTBL struct IOperandInstructionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOperandInstruction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOperandInstruction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOperandInstruction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOperandInstruction_GetOperandType(This,pType)	\
    ( (This)->lpVtbl -> GetOperandType(This,pType) ) 

#define IOperandInstruction_GetOperandValue(This,dwSize,pBytes)	\
    ( (This)->lpVtbl -> GetOperandValue(This,dwSize,pBytes) ) 

#define IOperandInstruction_SetOperandValue(This,dwSize,pBytes)	\
    ( (This)->lpVtbl -> SetOperandValue(This,dwSize,pBytes) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOperandInstruction_INTERFACE_DEFINED__ */


#ifndef __IBranchInstruction_INTERFACE_DEFINED__
#define __IBranchInstruction_INTERFACE_DEFINED__

/* interface IBranchInstruction */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBranchInstruction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("73728F9D-B4B5-4149-8396-A79C4726636E")
    IBranchInstruction : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsShortBranch( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExpandBranch( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBranchTarget( 
            /* [out] */ IInstruction **ppTarget) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTargetOffset( 
            /* [out] */ DWORD *pOffset) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBranchTarget( 
            /* [in] */ IInstruction *pTarget) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBranchInstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBranchInstruction * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBranchInstruction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBranchInstruction * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsShortBranch )( 
            IBranchInstruction * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *ExpandBranch )( 
            IBranchInstruction * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBranchTarget )( 
            IBranchInstruction * This,
            /* [out] */ IInstruction **ppTarget);
        
        HRESULT ( STDMETHODCALLTYPE *GetTargetOffset )( 
            IBranchInstruction * This,
            /* [out] */ DWORD *pOffset);
        
        HRESULT ( STDMETHODCALLTYPE *SetBranchTarget )( 
            IBranchInstruction * This,
            /* [in] */ IInstruction *pTarget);
        
        END_INTERFACE
    } IBranchInstructionVtbl;

    interface IBranchInstruction
    {
        CONST_VTBL struct IBranchInstructionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBranchInstruction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBranchInstruction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBranchInstruction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBranchInstruction_IsShortBranch(This,pbValue)	\
    ( (This)->lpVtbl -> IsShortBranch(This,pbValue) ) 

#define IBranchInstruction_ExpandBranch(This)	\
    ( (This)->lpVtbl -> ExpandBranch(This) ) 

#define IBranchInstruction_GetBranchTarget(This,ppTarget)	\
    ( (This)->lpVtbl -> GetBranchTarget(This,ppTarget) ) 

#define IBranchInstruction_GetTargetOffset(This,pOffset)	\
    ( (This)->lpVtbl -> GetTargetOffset(This,pOffset) ) 

#define IBranchInstruction_SetBranchTarget(This,pTarget)	\
    ( (This)->lpVtbl -> SetBranchTarget(This,pTarget) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBranchInstruction_INTERFACE_DEFINED__ */


#ifndef __ISwitchInstruction_INTERFACE_DEFINED__
#define __ISwitchInstruction_INTERFACE_DEFINED__

/* interface ISwitchInstruction */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ISwitchInstruction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("66B79035-4F18-4689-A16D-95AF469460A3")
    ISwitchInstruction : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBranchTarget( 
            /* [in] */ DWORD index,
            /* [out] */ IInstruction **ppTarget) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBranchTarget( 
            /* [in] */ DWORD index,
            /* [in] */ IInstruction *pTarget) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveBranchTargetAt( 
            /* [in] */ DWORD index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveBranchTarget( 
            /* [in] */ IInstruction *pTarget) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplaceBranchTarget( 
            /* [in] */ IInstruction *pOriginal,
            /* [in] */ IInstruction *pNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBranchCount( 
            /* [out] */ DWORD *pBranchCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBranchOffset( 
            /* [in] */ DWORD index,
            /* [out] */ DWORD *pdwOffset) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISwitchInstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISwitchInstruction * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISwitchInstruction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISwitchInstruction * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBranchTarget )( 
            ISwitchInstruction * This,
            /* [in] */ DWORD index,
            /* [out] */ IInstruction **ppTarget);
        
        HRESULT ( STDMETHODCALLTYPE *SetBranchTarget )( 
            ISwitchInstruction * This,
            /* [in] */ DWORD index,
            /* [in] */ IInstruction *pTarget);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveBranchTargetAt )( 
            ISwitchInstruction * This,
            /* [in] */ DWORD index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveBranchTarget )( 
            ISwitchInstruction * This,
            /* [in] */ IInstruction *pTarget);
        
        HRESULT ( STDMETHODCALLTYPE *ReplaceBranchTarget )( 
            ISwitchInstruction * This,
            /* [in] */ IInstruction *pOriginal,
            /* [in] */ IInstruction *pNew);
        
        HRESULT ( STDMETHODCALLTYPE *GetBranchCount )( 
            ISwitchInstruction * This,
            /* [out] */ DWORD *pBranchCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetBranchOffset )( 
            ISwitchInstruction * This,
            /* [in] */ DWORD index,
            /* [out] */ DWORD *pdwOffset);
        
        END_INTERFACE
    } ISwitchInstructionVtbl;

    interface ISwitchInstruction
    {
        CONST_VTBL struct ISwitchInstructionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISwitchInstruction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISwitchInstruction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISwitchInstruction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISwitchInstruction_GetBranchTarget(This,index,ppTarget)	\
    ( (This)->lpVtbl -> GetBranchTarget(This,index,ppTarget) ) 

#define ISwitchInstruction_SetBranchTarget(This,index,pTarget)	\
    ( (This)->lpVtbl -> SetBranchTarget(This,index,pTarget) ) 

#define ISwitchInstruction_RemoveBranchTargetAt(This,index)	\
    ( (This)->lpVtbl -> RemoveBranchTargetAt(This,index) ) 

#define ISwitchInstruction_RemoveBranchTarget(This,pTarget)	\
    ( (This)->lpVtbl -> RemoveBranchTarget(This,pTarget) ) 

#define ISwitchInstruction_ReplaceBranchTarget(This,pOriginal,pNew)	\
    ( (This)->lpVtbl -> ReplaceBranchTarget(This,pOriginal,pNew) ) 

#define ISwitchInstruction_GetBranchCount(This,pBranchCount)	\
    ( (This)->lpVtbl -> GetBranchCount(This,pBranchCount) ) 

#define ISwitchInstruction_GetBranchOffset(This,index,pdwOffset)	\
    ( (This)->lpVtbl -> GetBranchOffset(This,index,pdwOffset) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISwitchInstruction_INTERFACE_DEFINED__ */


#ifndef __IInstructionGraph_INTERFACE_DEFINED__
#define __IInstructionGraph_INTERFACE_DEFINED__

/* interface IInstructionGraph */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IInstructionGraph;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9165F2D1-2D6D-4B89-B2AB-2CACA66CAA48")
    IInstructionGraph : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMethodInfo( 
            /* [out] */ IMethodInfo **ppMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLastInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOriginalFirstInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOriginalLastInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUninstrumentedFirstInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUninstrumentedLastInstruction( 
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstructionAtOffset( 
            /* [in] */ DWORD offset,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstructionAtOriginalOffset( 
            /* [in] */ DWORD offset,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstructionAtUninstrumentedOffset( 
            /* [in] */ DWORD dwOffset,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InsertBefore( 
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InsertAfter( 
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InsertBeforeAndRetargetOffsets( 
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Replace( 
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ IInstruction *pInstructionOrig) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBaseline( 
            /* [in] */ LPCBYTE pCodeBase,
            /* [in] */ LPCBYTE pEndOfCode,
            /* [in] */ DWORD originalToBaselineCorIlMapSize,
            /* [size_is][in] */ COR_IL_MAP originalToBaselineCorIlMap[  ],
            /* [in] */ DWORD baselineSequencePointSize,
            /* [size_is][in] */ DWORD baselineSequencePointList[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HasBaselineBeenSet( 
            /* [out] */ BOOL *pHasBaselineBeenSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExpandBranches( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInstructionGraphVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInstructionGraph * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInstructionGraph * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInstructionGraph * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodInfo )( 
            IInstructionGraph * This,
            /* [out] */ IMethodInfo **ppMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstInstruction )( 
            IInstructionGraph * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastInstruction )( 
            IInstructionGraph * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetOriginalFirstInstruction )( 
            IInstructionGraph * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetOriginalLastInstruction )( 
            IInstructionGraph * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetUninstrumentedFirstInstruction )( 
            IInstructionGraph * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetUninstrumentedLastInstruction )( 
            IInstructionGraph * This,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructionAtOffset )( 
            IInstructionGraph * This,
            /* [in] */ DWORD offset,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructionAtOriginalOffset )( 
            IInstructionGraph * This,
            /* [in] */ DWORD offset,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructionAtUninstrumentedOffset )( 
            IInstructionGraph * This,
            /* [in] */ DWORD dwOffset,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *InsertBefore )( 
            IInstructionGraph * This,
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew);
        
        HRESULT ( STDMETHODCALLTYPE *InsertAfter )( 
            IInstructionGraph * This,
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew);
        
        HRESULT ( STDMETHODCALLTYPE *InsertBeforeAndRetargetOffsets )( 
            IInstructionGraph * This,
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew);
        
        HRESULT ( STDMETHODCALLTYPE *Replace )( 
            IInstructionGraph * This,
            /* [in] */ IInstruction *pInstructionOrig,
            /* [in] */ IInstruction *pInstructionNew);
        
        HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IInstructionGraph * This,
            /* [in] */ IInstruction *pInstructionOrig);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAll )( 
            IInstructionGraph * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBaseline )( 
            IInstructionGraph * This,
            /* [in] */ LPCBYTE pCodeBase,
            /* [in] */ LPCBYTE pEndOfCode,
            /* [in] */ DWORD originalToBaselineCorIlMapSize,
            /* [size_is][in] */ COR_IL_MAP originalToBaselineCorIlMap[  ],
            /* [in] */ DWORD baselineSequencePointSize,
            /* [size_is][in] */ DWORD baselineSequencePointList[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *HasBaselineBeenSet )( 
            IInstructionGraph * This,
            /* [out] */ BOOL *pHasBaselineBeenSet);
        
        HRESULT ( STDMETHODCALLTYPE *ExpandBranches )( 
            IInstructionGraph * This);
        
        END_INTERFACE
    } IInstructionGraphVtbl;

    interface IInstructionGraph
    {
        CONST_VTBL struct IInstructionGraphVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInstructionGraph_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInstructionGraph_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInstructionGraph_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInstructionGraph_GetMethodInfo(This,ppMethodInfo)	\
    ( (This)->lpVtbl -> GetMethodInfo(This,ppMethodInfo) ) 

#define IInstructionGraph_GetFirstInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetFirstInstruction(This,ppInstruction) ) 

#define IInstructionGraph_GetLastInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetLastInstruction(This,ppInstruction) ) 

#define IInstructionGraph_GetOriginalFirstInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetOriginalFirstInstruction(This,ppInstruction) ) 

#define IInstructionGraph_GetOriginalLastInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetOriginalLastInstruction(This,ppInstruction) ) 

#define IInstructionGraph_GetUninstrumentedFirstInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetUninstrumentedFirstInstruction(This,ppInstruction) ) 

#define IInstructionGraph_GetUninstrumentedLastInstruction(This,ppInstruction)	\
    ( (This)->lpVtbl -> GetUninstrumentedLastInstruction(This,ppInstruction) ) 

#define IInstructionGraph_GetInstructionAtOffset(This,offset,ppInstruction)	\
    ( (This)->lpVtbl -> GetInstructionAtOffset(This,offset,ppInstruction) ) 

#define IInstructionGraph_GetInstructionAtOriginalOffset(This,offset,ppInstruction)	\
    ( (This)->lpVtbl -> GetInstructionAtOriginalOffset(This,offset,ppInstruction) ) 

#define IInstructionGraph_GetInstructionAtUninstrumentedOffset(This,dwOffset,ppInstruction)	\
    ( (This)->lpVtbl -> GetInstructionAtUninstrumentedOffset(This,dwOffset,ppInstruction) ) 

#define IInstructionGraph_InsertBefore(This,pInstructionOrig,pInstructionNew)	\
    ( (This)->lpVtbl -> InsertBefore(This,pInstructionOrig,pInstructionNew) ) 

#define IInstructionGraph_InsertAfter(This,pInstructionOrig,pInstructionNew)	\
    ( (This)->lpVtbl -> InsertAfter(This,pInstructionOrig,pInstructionNew) ) 

#define IInstructionGraph_InsertBeforeAndRetargetOffsets(This,pInstructionOrig,pInstructionNew)	\
    ( (This)->lpVtbl -> InsertBeforeAndRetargetOffsets(This,pInstructionOrig,pInstructionNew) ) 

#define IInstructionGraph_Replace(This,pInstructionOrig,pInstructionNew)	\
    ( (This)->lpVtbl -> Replace(This,pInstructionOrig,pInstructionNew) ) 

#define IInstructionGraph_Remove(This,pInstructionOrig)	\
    ( (This)->lpVtbl -> Remove(This,pInstructionOrig) ) 

#define IInstructionGraph_RemoveAll(This)	\
    ( (This)->lpVtbl -> RemoveAll(This) ) 

#define IInstructionGraph_CreateBaseline(This,pCodeBase,pEndOfCode,originalToBaselineCorIlMapSize,originalToBaselineCorIlMap,baselineSequencePointSize,baselineSequencePointList)	\
    ( (This)->lpVtbl -> CreateBaseline(This,pCodeBase,pEndOfCode,originalToBaselineCorIlMapSize,originalToBaselineCorIlMap,baselineSequencePointSize,baselineSequencePointList) ) 

#define IInstructionGraph_HasBaselineBeenSet(This,pHasBaselineBeenSet)	\
    ( (This)->lpVtbl -> HasBaselineBeenSet(This,pHasBaselineBeenSet) ) 

#define IInstructionGraph_ExpandBranches(This)	\
    ( (This)->lpVtbl -> ExpandBranches(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInstructionGraph_INTERFACE_DEFINED__ */


#ifndef __IMethodInfo_INTERFACE_DEFINED__
#define __IMethodInfo_INTERFACE_DEFINED__

/* interface IMethodInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IMethodInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CC21A894-F4DF-4726-8318-D6C24C4985B1")
    IMethodInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetModuleInfo( 
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFullName( 
            /* [out] */ BSTR *pbstrFullName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstructions( 
            /* [out] */ IInstructionGraph **ppInstructionGraph) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocalVariables( 
            /* [out] */ ILocalVariableCollection **ppLocalVariables) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClassId( 
            /* [out] */ ClassID *pClassId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFunctionId( 
            /* [out] */ FunctionID *pFunctionID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMethodToken( 
            /* [out] */ mdToken *pToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGenericParameterCount( 
            /* [out] */ DWORD *pCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsStatic( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsPublic( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsPrivate( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsPropertyGetter( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsPropertySetter( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsFinalizer( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsConstructor( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsStaticConstructor( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParameters( 
            /* [out] */ IEnumMethodParameters **pMethodArgs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeclaringType( 
            /* [out] */ IType **ppType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetReturnType( 
            /* [out] */ IType **ppType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorSignature( 
            /* [in] */ DWORD cbBuffer,
            /* [size_is][length_is][out] */ BYTE *pCorSignature,
            /* [out] */ DWORD *pcbSignature) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocalVarSigToken( 
            /* [out] */ mdToken *pToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLocalVarSigToken( 
            /* [in] */ mdToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttributes( 
            /* [out] */ DWORD *pCorMethodAttr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRejitCodeGenFlags( 
            /* [out] */ DWORD *pRefitFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCodeRva( 
            /* [out] */ DWORD *pRva) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MethodImplFlags( 
            /* [out] */ UINT *pCorMethodImpl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRejitCodeGenFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExceptionSection( 
            /* [out] */ IExceptionSection **ppExceptionSection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInstructionFactory( 
            /* [out] */ IInstructionFactory **ppInstructionFactory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRejitCount( 
            /* [out] */ DWORD *pdwRejitCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxStack( 
            /* [out] */ DWORD *pMaxStack) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSingleRetDefaultInstrumentation( 
            /* [out] */ ISingleRetDefaultInstrumentation **ppSingleRetDefaultInstrumentation) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMethodInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMethodInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMethodInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMethodInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleInfo )( 
            IMethodInfo * This,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IMethodInfo * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetFullName )( 
            IMethodInfo * This,
            /* [out] */ BSTR *pbstrFullName);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructions )( 
            IMethodInfo * This,
            /* [out] */ IInstructionGraph **ppInstructionGraph);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocalVariables )( 
            IMethodInfo * This,
            /* [out] */ ILocalVariableCollection **ppLocalVariables);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassId )( 
            IMethodInfo * This,
            /* [out] */ ClassID *pClassId);
        
        HRESULT ( STDMETHODCALLTYPE *GetFunctionId )( 
            IMethodInfo * This,
            /* [out] */ FunctionID *pFunctionID);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodToken )( 
            IMethodInfo * This,
            /* [out] */ mdToken *pToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetGenericParameterCount )( 
            IMethodInfo * This,
            /* [out] */ DWORD *pCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsStatic )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPublic )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPrivate )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPropertyGetter )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPropertySetter )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsFinalizer )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsConstructor )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsStaticConstructor )( 
            IMethodInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetParameters )( 
            IMethodInfo * This,
            /* [out] */ IEnumMethodParameters **pMethodArgs);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeclaringType )( 
            IMethodInfo * This,
            /* [out] */ IType **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetReturnType )( 
            IMethodInfo * This,
            /* [out] */ IType **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorSignature )( 
            IMethodInfo * This,
            /* [in] */ DWORD cbBuffer,
            /* [size_is][length_is][out] */ BYTE *pCorSignature,
            /* [out] */ DWORD *pcbSignature);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocalVarSigToken )( 
            IMethodInfo * This,
            /* [out] */ mdToken *pToken);
        
        HRESULT ( STDMETHODCALLTYPE *SetLocalVarSigToken )( 
            IMethodInfo * This,
            /* [in] */ mdToken token);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributes )( 
            IMethodInfo * This,
            /* [out] */ DWORD *pCorMethodAttr);
        
        HRESULT ( STDMETHODCALLTYPE *GetRejitCodeGenFlags )( 
            IMethodInfo * This,
            /* [out] */ DWORD *pRefitFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetCodeRva )( 
            IMethodInfo * This,
            /* [out] */ DWORD *pRva);
        
        HRESULT ( STDMETHODCALLTYPE *MethodImplFlags )( 
            IMethodInfo * This,
            /* [out] */ UINT *pCorMethodImpl);
        
        HRESULT ( STDMETHODCALLTYPE *SetRejitCodeGenFlags )( 
            IMethodInfo * This,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetExceptionSection )( 
            IMethodInfo * This,
            /* [out] */ IExceptionSection **ppExceptionSection);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructionFactory )( 
            IMethodInfo * This,
            /* [out] */ IInstructionFactory **ppInstructionFactory);
        
        HRESULT ( STDMETHODCALLTYPE *GetRejitCount )( 
            IMethodInfo * This,
            /* [out] */ DWORD *pdwRejitCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetMaxStack )( 
            IMethodInfo * This,
            /* [out] */ DWORD *pMaxStack);
        
        HRESULT ( STDMETHODCALLTYPE *GetSingleRetDefaultInstrumentation )( 
            IMethodInfo * This,
            /* [out] */ ISingleRetDefaultInstrumentation **ppSingleRetDefaultInstrumentation);
        
        END_INTERFACE
    } IMethodInfoVtbl;

    interface IMethodInfo
    {
        CONST_VTBL struct IMethodInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMethodInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMethodInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMethodInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMethodInfo_GetModuleInfo(This,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleInfo(This,ppModuleInfo) ) 

#define IMethodInfo_GetName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetName(This,pbstrName) ) 

#define IMethodInfo_GetFullName(This,pbstrFullName)	\
    ( (This)->lpVtbl -> GetFullName(This,pbstrFullName) ) 

#define IMethodInfo_GetInstructions(This,ppInstructionGraph)	\
    ( (This)->lpVtbl -> GetInstructions(This,ppInstructionGraph) ) 

#define IMethodInfo_GetLocalVariables(This,ppLocalVariables)	\
    ( (This)->lpVtbl -> GetLocalVariables(This,ppLocalVariables) ) 

#define IMethodInfo_GetClassId(This,pClassId)	\
    ( (This)->lpVtbl -> GetClassId(This,pClassId) ) 

#define IMethodInfo_GetFunctionId(This,pFunctionID)	\
    ( (This)->lpVtbl -> GetFunctionId(This,pFunctionID) ) 

#define IMethodInfo_GetMethodToken(This,pToken)	\
    ( (This)->lpVtbl -> GetMethodToken(This,pToken) ) 

#define IMethodInfo_GetGenericParameterCount(This,pCount)	\
    ( (This)->lpVtbl -> GetGenericParameterCount(This,pCount) ) 

#define IMethodInfo_GetIsStatic(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsStatic(This,pbValue) ) 

#define IMethodInfo_GetIsPublic(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPublic(This,pbValue) ) 

#define IMethodInfo_GetIsPrivate(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPrivate(This,pbValue) ) 

#define IMethodInfo_GetIsPropertyGetter(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPropertyGetter(This,pbValue) ) 

#define IMethodInfo_GetIsPropertySetter(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPropertySetter(This,pbValue) ) 

#define IMethodInfo_GetIsFinalizer(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsFinalizer(This,pbValue) ) 

#define IMethodInfo_GetIsConstructor(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsConstructor(This,pbValue) ) 

#define IMethodInfo_GetIsStaticConstructor(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsStaticConstructor(This,pbValue) ) 

#define IMethodInfo_GetParameters(This,pMethodArgs)	\
    ( (This)->lpVtbl -> GetParameters(This,pMethodArgs) ) 

#define IMethodInfo_GetDeclaringType(This,ppType)	\
    ( (This)->lpVtbl -> GetDeclaringType(This,ppType) ) 

#define IMethodInfo_GetReturnType(This,ppType)	\
    ( (This)->lpVtbl -> GetReturnType(This,ppType) ) 

#define IMethodInfo_GetCorSignature(This,cbBuffer,pCorSignature,pcbSignature)	\
    ( (This)->lpVtbl -> GetCorSignature(This,cbBuffer,pCorSignature,pcbSignature) ) 

#define IMethodInfo_GetLocalVarSigToken(This,pToken)	\
    ( (This)->lpVtbl -> GetLocalVarSigToken(This,pToken) ) 

#define IMethodInfo_SetLocalVarSigToken(This,token)	\
    ( (This)->lpVtbl -> SetLocalVarSigToken(This,token) ) 

#define IMethodInfo_GetAttributes(This,pCorMethodAttr)	\
    ( (This)->lpVtbl -> GetAttributes(This,pCorMethodAttr) ) 

#define IMethodInfo_GetRejitCodeGenFlags(This,pRefitFlags)	\
    ( (This)->lpVtbl -> GetRejitCodeGenFlags(This,pRefitFlags) ) 

#define IMethodInfo_GetCodeRva(This,pRva)	\
    ( (This)->lpVtbl -> GetCodeRva(This,pRva) ) 

#define IMethodInfo_MethodImplFlags(This,pCorMethodImpl)	\
    ( (This)->lpVtbl -> MethodImplFlags(This,pCorMethodImpl) ) 

#define IMethodInfo_SetRejitCodeGenFlags(This,dwFlags)	\
    ( (This)->lpVtbl -> SetRejitCodeGenFlags(This,dwFlags) ) 

#define IMethodInfo_GetExceptionSection(This,ppExceptionSection)	\
    ( (This)->lpVtbl -> GetExceptionSection(This,ppExceptionSection) ) 

#define IMethodInfo_GetInstructionFactory(This,ppInstructionFactory)	\
    ( (This)->lpVtbl -> GetInstructionFactory(This,ppInstructionFactory) ) 

#define IMethodInfo_GetRejitCount(This,pdwRejitCount)	\
    ( (This)->lpVtbl -> GetRejitCount(This,pdwRejitCount) ) 

#define IMethodInfo_GetMaxStack(This,pMaxStack)	\
    ( (This)->lpVtbl -> GetMaxStack(This,pMaxStack) ) 

#define IMethodInfo_GetSingleRetDefaultInstrumentation(This,ppSingleRetDefaultInstrumentation)	\
    ( (This)->lpVtbl -> GetSingleRetDefaultInstrumentation(This,ppSingleRetDefaultInstrumentation) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMethodInfo_INTERFACE_DEFINED__ */


#ifndef __IMethodInfo2_INTERFACE_DEFINED__
#define __IMethodInfo2_INTERFACE_DEFINED__

/* interface IMethodInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IMethodInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CDF098F7-D04A-4B58-B46E-184C4F223E5F")
    IMethodInfo2 : public IMethodInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetOriginalLocalVariables( 
            /* [out] */ ILocalVariableCollection **ppLocalVariables) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMethodInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMethodInfo2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMethodInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMethodInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleInfo )( 
            IMethodInfo2 * This,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IMethodInfo2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetFullName )( 
            IMethodInfo2 * This,
            /* [out] */ BSTR *pbstrFullName);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructions )( 
            IMethodInfo2 * This,
            /* [out] */ IInstructionGraph **ppInstructionGraph);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocalVariables )( 
            IMethodInfo2 * This,
            /* [out] */ ILocalVariableCollection **ppLocalVariables);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassId )( 
            IMethodInfo2 * This,
            /* [out] */ ClassID *pClassId);
        
        HRESULT ( STDMETHODCALLTYPE *GetFunctionId )( 
            IMethodInfo2 * This,
            /* [out] */ FunctionID *pFunctionID);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodToken )( 
            IMethodInfo2 * This,
            /* [out] */ mdToken *pToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetGenericParameterCount )( 
            IMethodInfo2 * This,
            /* [out] */ DWORD *pCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsStatic )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPublic )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPrivate )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPropertyGetter )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsPropertySetter )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsFinalizer )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsConstructor )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsStaticConstructor )( 
            IMethodInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetParameters )( 
            IMethodInfo2 * This,
            /* [out] */ IEnumMethodParameters **pMethodArgs);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeclaringType )( 
            IMethodInfo2 * This,
            /* [out] */ IType **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetReturnType )( 
            IMethodInfo2 * This,
            /* [out] */ IType **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorSignature )( 
            IMethodInfo2 * This,
            /* [in] */ DWORD cbBuffer,
            /* [size_is][length_is][out] */ BYTE *pCorSignature,
            /* [out] */ DWORD *pcbSignature);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocalVarSigToken )( 
            IMethodInfo2 * This,
            /* [out] */ mdToken *pToken);
        
        HRESULT ( STDMETHODCALLTYPE *SetLocalVarSigToken )( 
            IMethodInfo2 * This,
            /* [in] */ mdToken token);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributes )( 
            IMethodInfo2 * This,
            /* [out] */ DWORD *pCorMethodAttr);
        
        HRESULT ( STDMETHODCALLTYPE *GetRejitCodeGenFlags )( 
            IMethodInfo2 * This,
            /* [out] */ DWORD *pRefitFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetCodeRva )( 
            IMethodInfo2 * This,
            /* [out] */ DWORD *pRva);
        
        HRESULT ( STDMETHODCALLTYPE *MethodImplFlags )( 
            IMethodInfo2 * This,
            /* [out] */ UINT *pCorMethodImpl);
        
        HRESULT ( STDMETHODCALLTYPE *SetRejitCodeGenFlags )( 
            IMethodInfo2 * This,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetExceptionSection )( 
            IMethodInfo2 * This,
            /* [out] */ IExceptionSection **ppExceptionSection);
        
        HRESULT ( STDMETHODCALLTYPE *GetInstructionFactory )( 
            IMethodInfo2 * This,
            /* [out] */ IInstructionFactory **ppInstructionFactory);
        
        HRESULT ( STDMETHODCALLTYPE *GetRejitCount )( 
            IMethodInfo2 * This,
            /* [out] */ DWORD *pdwRejitCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetMaxStack )( 
            IMethodInfo2 * This,
            /* [out] */ DWORD *pMaxStack);
        
        HRESULT ( STDMETHODCALLTYPE *GetSingleRetDefaultInstrumentation )( 
            IMethodInfo2 * This,
            /* [out] */ ISingleRetDefaultInstrumentation **ppSingleRetDefaultInstrumentation);
        
        HRESULT ( STDMETHODCALLTYPE *GetOriginalLocalVariables )( 
            IMethodInfo2 * This,
            /* [out] */ ILocalVariableCollection **ppLocalVariables);
        
        END_INTERFACE
    } IMethodInfo2Vtbl;

    interface IMethodInfo2
    {
        CONST_VTBL struct IMethodInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMethodInfo2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMethodInfo2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMethodInfo2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMethodInfo2_GetModuleInfo(This,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleInfo(This,ppModuleInfo) ) 

#define IMethodInfo2_GetName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetName(This,pbstrName) ) 

#define IMethodInfo2_GetFullName(This,pbstrFullName)	\
    ( (This)->lpVtbl -> GetFullName(This,pbstrFullName) ) 

#define IMethodInfo2_GetInstructions(This,ppInstructionGraph)	\
    ( (This)->lpVtbl -> GetInstructions(This,ppInstructionGraph) ) 

#define IMethodInfo2_GetLocalVariables(This,ppLocalVariables)	\
    ( (This)->lpVtbl -> GetLocalVariables(This,ppLocalVariables) ) 

#define IMethodInfo2_GetClassId(This,pClassId)	\
    ( (This)->lpVtbl -> GetClassId(This,pClassId) ) 

#define IMethodInfo2_GetFunctionId(This,pFunctionID)	\
    ( (This)->lpVtbl -> GetFunctionId(This,pFunctionID) ) 

#define IMethodInfo2_GetMethodToken(This,pToken)	\
    ( (This)->lpVtbl -> GetMethodToken(This,pToken) ) 

#define IMethodInfo2_GetGenericParameterCount(This,pCount)	\
    ( (This)->lpVtbl -> GetGenericParameterCount(This,pCount) ) 

#define IMethodInfo2_GetIsStatic(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsStatic(This,pbValue) ) 

#define IMethodInfo2_GetIsPublic(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPublic(This,pbValue) ) 

#define IMethodInfo2_GetIsPrivate(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPrivate(This,pbValue) ) 

#define IMethodInfo2_GetIsPropertyGetter(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPropertyGetter(This,pbValue) ) 

#define IMethodInfo2_GetIsPropertySetter(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsPropertySetter(This,pbValue) ) 

#define IMethodInfo2_GetIsFinalizer(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsFinalizer(This,pbValue) ) 

#define IMethodInfo2_GetIsConstructor(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsConstructor(This,pbValue) ) 

#define IMethodInfo2_GetIsStaticConstructor(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsStaticConstructor(This,pbValue) ) 

#define IMethodInfo2_GetParameters(This,pMethodArgs)	\
    ( (This)->lpVtbl -> GetParameters(This,pMethodArgs) ) 

#define IMethodInfo2_GetDeclaringType(This,ppType)	\
    ( (This)->lpVtbl -> GetDeclaringType(This,ppType) ) 

#define IMethodInfo2_GetReturnType(This,ppType)	\
    ( (This)->lpVtbl -> GetReturnType(This,ppType) ) 

#define IMethodInfo2_GetCorSignature(This,cbBuffer,pCorSignature,pcbSignature)	\
    ( (This)->lpVtbl -> GetCorSignature(This,cbBuffer,pCorSignature,pcbSignature) ) 

#define IMethodInfo2_GetLocalVarSigToken(This,pToken)	\
    ( (This)->lpVtbl -> GetLocalVarSigToken(This,pToken) ) 

#define IMethodInfo2_SetLocalVarSigToken(This,token)	\
    ( (This)->lpVtbl -> SetLocalVarSigToken(This,token) ) 

#define IMethodInfo2_GetAttributes(This,pCorMethodAttr)	\
    ( (This)->lpVtbl -> GetAttributes(This,pCorMethodAttr) ) 

#define IMethodInfo2_GetRejitCodeGenFlags(This,pRefitFlags)	\
    ( (This)->lpVtbl -> GetRejitCodeGenFlags(This,pRefitFlags) ) 

#define IMethodInfo2_GetCodeRva(This,pRva)	\
    ( (This)->lpVtbl -> GetCodeRva(This,pRva) ) 

#define IMethodInfo2_MethodImplFlags(This,pCorMethodImpl)	\
    ( (This)->lpVtbl -> MethodImplFlags(This,pCorMethodImpl) ) 

#define IMethodInfo2_SetRejitCodeGenFlags(This,dwFlags)	\
    ( (This)->lpVtbl -> SetRejitCodeGenFlags(This,dwFlags) ) 

#define IMethodInfo2_GetExceptionSection(This,ppExceptionSection)	\
    ( (This)->lpVtbl -> GetExceptionSection(This,ppExceptionSection) ) 

#define IMethodInfo2_GetInstructionFactory(This,ppInstructionFactory)	\
    ( (This)->lpVtbl -> GetInstructionFactory(This,ppInstructionFactory) ) 

#define IMethodInfo2_GetRejitCount(This,pdwRejitCount)	\
    ( (This)->lpVtbl -> GetRejitCount(This,pdwRejitCount) ) 

#define IMethodInfo2_GetMaxStack(This,pMaxStack)	\
    ( (This)->lpVtbl -> GetMaxStack(This,pMaxStack) ) 

#define IMethodInfo2_GetSingleRetDefaultInstrumentation(This,ppSingleRetDefaultInstrumentation)	\
    ( (This)->lpVtbl -> GetSingleRetDefaultInstrumentation(This,ppSingleRetDefaultInstrumentation) ) 


#define IMethodInfo2_GetOriginalLocalVariables(This,ppLocalVariables)	\
    ( (This)->lpVtbl -> GetOriginalLocalVariables(This,ppLocalVariables) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMethodInfo2_INTERFACE_DEFINED__ */


#ifndef __IAssemblyInfo_INTERFACE_DEFINED__
#define __IAssemblyInfo_INTERFACE_DEFINED__

/* interface IAssemblyInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAssemblyInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("110FE5BA-57CD-4308-86BE-487478ABE2CD")
    IAssemblyInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainInfo( 
            /* [out] */ IAppDomainInfo **ppAppDomainInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleCount( 
            /* [out] */ ULONG *pcModuleInfos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModules( 
            /* [in] */ ULONG cModuleInfos,
            /* [out] */ IEnumModuleInfo **pModuleInfos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleById( 
            /* [in] */ ModuleID moduleId,
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleByMvid( 
            /* [in] */ GUID *pMvid,
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModulesByName( 
            /* [in] */ BSTR pszModuleName,
            /* [out] */ IEnumModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleForMethod( 
            /* [in] */ mdToken token,
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleForType( 
            /* [in] */ BSTR pszTypeName,
            /* [in] */ mdToken tkResolutionScope,
            /* [out] */ mdToken *pTkTypeDef) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetManifestModule( 
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPublicKey( 
            /* [in] */ ULONG cbBytes,
            /* [length_is][size_is][out] */ BYTE *pbBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPublicKeySize( 
            /* [out] */ ULONG *pcbBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPublicKeyToken( 
            /* [out] */ BSTR *pbstrPublicKeyToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetID( 
            /* [out] */ AssemblyID *pAssemblyId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetaDataToken( 
            /* [out] */ DWORD *pdwToken) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAssemblyInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAssemblyInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAssemblyInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAssemblyInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainInfo )( 
            IAssemblyInfo * This,
            /* [out] */ IAppDomainInfo **ppAppDomainInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleCount )( 
            IAssemblyInfo * This,
            /* [out] */ ULONG *pcModuleInfos);
        
        HRESULT ( STDMETHODCALLTYPE *GetModules )( 
            IAssemblyInfo * This,
            /* [in] */ ULONG cModuleInfos,
            /* [out] */ IEnumModuleInfo **pModuleInfos);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleById )( 
            IAssemblyInfo * This,
            /* [in] */ ModuleID moduleId,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleByMvid )( 
            IAssemblyInfo * This,
            /* [in] */ GUID *pMvid,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModulesByName )( 
            IAssemblyInfo * This,
            /* [in] */ BSTR pszModuleName,
            /* [out] */ IEnumModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleForMethod )( 
            IAssemblyInfo * This,
            /* [in] */ mdToken token,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleForType )( 
            IAssemblyInfo * This,
            /* [in] */ BSTR pszTypeName,
            /* [in] */ mdToken tkResolutionScope,
            /* [out] */ mdToken *pTkTypeDef);
        
        HRESULT ( STDMETHODCALLTYPE *GetManifestModule )( 
            IAssemblyInfo * This,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetPublicKey )( 
            IAssemblyInfo * This,
            /* [in] */ ULONG cbBytes,
            /* [length_is][size_is][out] */ BYTE *pbBytes);
        
        HRESULT ( STDMETHODCALLTYPE *GetPublicKeySize )( 
            IAssemblyInfo * This,
            /* [out] */ ULONG *pcbBytes);
        
        HRESULT ( STDMETHODCALLTYPE *GetPublicKeyToken )( 
            IAssemblyInfo * This,
            /* [out] */ BSTR *pbstrPublicKeyToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetID )( 
            IAssemblyInfo * This,
            /* [out] */ AssemblyID *pAssemblyId);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IAssemblyInfo * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataToken )( 
            IAssemblyInfo * This,
            /* [out] */ DWORD *pdwToken);
        
        END_INTERFACE
    } IAssemblyInfoVtbl;

    interface IAssemblyInfo
    {
        CONST_VTBL struct IAssemblyInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAssemblyInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAssemblyInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAssemblyInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAssemblyInfo_GetAppDomainInfo(This,ppAppDomainInfo)	\
    ( (This)->lpVtbl -> GetAppDomainInfo(This,ppAppDomainInfo) ) 

#define IAssemblyInfo_GetModuleCount(This,pcModuleInfos)	\
    ( (This)->lpVtbl -> GetModuleCount(This,pcModuleInfos) ) 

#define IAssemblyInfo_GetModules(This,cModuleInfos,pModuleInfos)	\
    ( (This)->lpVtbl -> GetModules(This,cModuleInfos,pModuleInfos) ) 

#define IAssemblyInfo_GetModuleById(This,moduleId,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleById(This,moduleId,ppModuleInfo) ) 

#define IAssemblyInfo_GetModuleByMvid(This,pMvid,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleByMvid(This,pMvid,ppModuleInfo) ) 

#define IAssemblyInfo_GetModulesByName(This,pszModuleName,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModulesByName(This,pszModuleName,ppModuleInfo) ) 

#define IAssemblyInfo_GetModuleForMethod(This,token,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleForMethod(This,token,ppModuleInfo) ) 

#define IAssemblyInfo_GetModuleForType(This,pszTypeName,tkResolutionScope,pTkTypeDef)	\
    ( (This)->lpVtbl -> GetModuleForType(This,pszTypeName,tkResolutionScope,pTkTypeDef) ) 

#define IAssemblyInfo_GetManifestModule(This,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetManifestModule(This,ppModuleInfo) ) 

#define IAssemblyInfo_GetPublicKey(This,cbBytes,pbBytes)	\
    ( (This)->lpVtbl -> GetPublicKey(This,cbBytes,pbBytes) ) 

#define IAssemblyInfo_GetPublicKeySize(This,pcbBytes)	\
    ( (This)->lpVtbl -> GetPublicKeySize(This,pcbBytes) ) 

#define IAssemblyInfo_GetPublicKeyToken(This,pbstrPublicKeyToken)	\
    ( (This)->lpVtbl -> GetPublicKeyToken(This,pbstrPublicKeyToken) ) 

#define IAssemblyInfo_GetID(This,pAssemblyId)	\
    ( (This)->lpVtbl -> GetID(This,pAssemblyId) ) 

#define IAssemblyInfo_GetName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetName(This,pbstrName) ) 

#define IAssemblyInfo_GetMetaDataToken(This,pdwToken)	\
    ( (This)->lpVtbl -> GetMetaDataToken(This,pdwToken) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAssemblyInfo_INTERFACE_DEFINED__ */


#ifndef __IEnumAssemblyInfo_INTERFACE_DEFINED__
#define __IEnumAssemblyInfo_INTERFACE_DEFINED__

/* interface IEnumAssemblyInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumAssemblyInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71001B79-B50A-4103-9D19-FFCF9F6CE1E9")
    IEnumAssemblyInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IAssemblyInfo **rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumAssemblyInfo **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumAssemblyInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumAssemblyInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumAssemblyInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumAssemblyInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumAssemblyInfo * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IAssemblyInfo **rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumAssemblyInfo * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumAssemblyInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumAssemblyInfo * This,
            /* [out] */ IEnumAssemblyInfo **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumAssemblyInfo * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumAssemblyInfoVtbl;

    interface IEnumAssemblyInfo
    {
        CONST_VTBL struct IEnumAssemblyInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumAssemblyInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumAssemblyInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumAssemblyInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumAssemblyInfo_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumAssemblyInfo_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumAssemblyInfo_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumAssemblyInfo_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumAssemblyInfo_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumAssemblyInfo_INTERFACE_DEFINED__ */


#ifndef __IModuleInfo_INTERFACE_DEFINED__
#define __IModuleInfo_INTERFACE_DEFINED__

/* interface IModuleInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IModuleInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0BD963B1-FD87-4492-A417-152F3D0C9CBC")
    IModuleInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetModuleName( 
            /* [out] */ BSTR *pbstrModuleName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFullPath( 
            /* [out] */ BSTR *pbstrFullPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyInfo( 
            /* [out] */ IAssemblyInfo **ppAssemblyInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainInfo( 
            /* [out] */ IAppDomainInfo **ppAppDomainInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetaDataImport( 
            /* [out] */ IUnknown **ppMetaDataImport) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetaDataAssemblyImport( 
            /* [out] */ IUnknown **ppMetaDataAssemblyImport) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetaDataEmit( 
            /* [out] */ IUnknown **ppMetaDataEmit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetaDataAssemblyEmit( 
            /* [out] */ IUnknown **ppMetaDataAssemblyEmit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleID( 
            /* [out] */ ModuleID *pModuleId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMVID( 
            /* [out] */ GUID *pguidMvid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsILOnly( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsMscorlib( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsDynamic( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsNgen( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsWinRT( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIs64bit( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetImageBase( 
            /* [out] */ LPCBYTE *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorHeader( 
            /* [in] */ DWORD cbValue,
            /* [length_is][size_is][out] */ BYTE *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEntrypointToken( 
            /* [out] */ DWORD *pdwEntrypointToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleVersion( 
            /* [in] */ DWORD cbValue,
            /* [length_is][size_is][out] */ BYTE *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RequestRejit( 
            /* [in] */ mdToken methodToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateTypeFactory( 
            /* [out] */ ITypeCreator **ppTypeFactory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMethodInfoById( 
            /* [in] */ FunctionID functionID,
            /* [out] */ IMethodInfo **ppMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMethodInfoByToken( 
            /* [in] */ mdToken token,
            /* [out] */ IMethodInfo **ppMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ImportModule( 
            /* [in] */ IUnknown *pSourceModuleMetadataImport,
            /* [in] */ LPCBYTE *pSourceImage) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IModuleInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IModuleInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IModuleInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IModuleInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleName )( 
            IModuleInfo * This,
            /* [out] */ BSTR *pbstrModuleName);
        
        HRESULT ( STDMETHODCALLTYPE *GetFullPath )( 
            IModuleInfo * This,
            /* [out] */ BSTR *pbstrFullPath);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyInfo )( 
            IModuleInfo * This,
            /* [out] */ IAssemblyInfo **ppAssemblyInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainInfo )( 
            IModuleInfo * This,
            /* [out] */ IAppDomainInfo **ppAppDomainInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataImport )( 
            IModuleInfo * This,
            /* [out] */ IUnknown **ppMetaDataImport);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataAssemblyImport )( 
            IModuleInfo * This,
            /* [out] */ IUnknown **ppMetaDataAssemblyImport);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataEmit )( 
            IModuleInfo * This,
            /* [out] */ IUnknown **ppMetaDataEmit);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataAssemblyEmit )( 
            IModuleInfo * This,
            /* [out] */ IUnknown **ppMetaDataAssemblyEmit);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleID )( 
            IModuleInfo * This,
            /* [out] */ ModuleID *pModuleId);
        
        HRESULT ( STDMETHODCALLTYPE *GetMVID )( 
            IModuleInfo * This,
            /* [out] */ GUID *pguidMvid);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsILOnly )( 
            IModuleInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsMscorlib )( 
            IModuleInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsDynamic )( 
            IModuleInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsNgen )( 
            IModuleInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsWinRT )( 
            IModuleInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIs64bit )( 
            IModuleInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetImageBase )( 
            IModuleInfo * This,
            /* [out] */ LPCBYTE *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorHeader )( 
            IModuleInfo * This,
            /* [in] */ DWORD cbValue,
            /* [length_is][size_is][out] */ BYTE *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetEntrypointToken )( 
            IModuleInfo * This,
            /* [out] */ DWORD *pdwEntrypointToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleVersion )( 
            IModuleInfo * This,
            /* [in] */ DWORD cbValue,
            /* [length_is][size_is][out] */ BYTE *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *RequestRejit )( 
            IModuleInfo * This,
            /* [in] */ mdToken methodToken);
        
        HRESULT ( STDMETHODCALLTYPE *CreateTypeFactory )( 
            IModuleInfo * This,
            /* [out] */ ITypeCreator **ppTypeFactory);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodInfoById )( 
            IModuleInfo * This,
            /* [in] */ FunctionID functionID,
            /* [out] */ IMethodInfo **ppMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodInfoByToken )( 
            IModuleInfo * This,
            /* [in] */ mdToken token,
            /* [out] */ IMethodInfo **ppMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ImportModule )( 
            IModuleInfo * This,
            /* [in] */ IUnknown *pSourceModuleMetadataImport,
            /* [in] */ LPCBYTE *pSourceImage);
        
        END_INTERFACE
    } IModuleInfoVtbl;

    interface IModuleInfo
    {
        CONST_VTBL struct IModuleInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IModuleInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IModuleInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IModuleInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IModuleInfo_GetModuleName(This,pbstrModuleName)	\
    ( (This)->lpVtbl -> GetModuleName(This,pbstrModuleName) ) 

#define IModuleInfo_GetFullPath(This,pbstrFullPath)	\
    ( (This)->lpVtbl -> GetFullPath(This,pbstrFullPath) ) 

#define IModuleInfo_GetAssemblyInfo(This,ppAssemblyInfo)	\
    ( (This)->lpVtbl -> GetAssemblyInfo(This,ppAssemblyInfo) ) 

#define IModuleInfo_GetAppDomainInfo(This,ppAppDomainInfo)	\
    ( (This)->lpVtbl -> GetAppDomainInfo(This,ppAppDomainInfo) ) 

#define IModuleInfo_GetMetaDataImport(This,ppMetaDataImport)	\
    ( (This)->lpVtbl -> GetMetaDataImport(This,ppMetaDataImport) ) 

#define IModuleInfo_GetMetaDataAssemblyImport(This,ppMetaDataAssemblyImport)	\
    ( (This)->lpVtbl -> GetMetaDataAssemblyImport(This,ppMetaDataAssemblyImport) ) 

#define IModuleInfo_GetMetaDataEmit(This,ppMetaDataEmit)	\
    ( (This)->lpVtbl -> GetMetaDataEmit(This,ppMetaDataEmit) ) 

#define IModuleInfo_GetMetaDataAssemblyEmit(This,ppMetaDataAssemblyEmit)	\
    ( (This)->lpVtbl -> GetMetaDataAssemblyEmit(This,ppMetaDataAssemblyEmit) ) 

#define IModuleInfo_GetModuleID(This,pModuleId)	\
    ( (This)->lpVtbl -> GetModuleID(This,pModuleId) ) 

#define IModuleInfo_GetMVID(This,pguidMvid)	\
    ( (This)->lpVtbl -> GetMVID(This,pguidMvid) ) 

#define IModuleInfo_GetIsILOnly(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsILOnly(This,pbValue) ) 

#define IModuleInfo_GetIsMscorlib(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsMscorlib(This,pbValue) ) 

#define IModuleInfo_GetIsDynamic(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsDynamic(This,pbValue) ) 

#define IModuleInfo_GetIsNgen(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsNgen(This,pbValue) ) 

#define IModuleInfo_GetIsWinRT(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsWinRT(This,pbValue) ) 

#define IModuleInfo_GetIs64bit(This,pbValue)	\
    ( (This)->lpVtbl -> GetIs64bit(This,pbValue) ) 

#define IModuleInfo_GetImageBase(This,pbValue)	\
    ( (This)->lpVtbl -> GetImageBase(This,pbValue) ) 

#define IModuleInfo_GetCorHeader(This,cbValue,pbValue)	\
    ( (This)->lpVtbl -> GetCorHeader(This,cbValue,pbValue) ) 

#define IModuleInfo_GetEntrypointToken(This,pdwEntrypointToken)	\
    ( (This)->lpVtbl -> GetEntrypointToken(This,pdwEntrypointToken) ) 

#define IModuleInfo_GetModuleVersion(This,cbValue,pbValue)	\
    ( (This)->lpVtbl -> GetModuleVersion(This,cbValue,pbValue) ) 

#define IModuleInfo_RequestRejit(This,methodToken)	\
    ( (This)->lpVtbl -> RequestRejit(This,methodToken) ) 

#define IModuleInfo_CreateTypeFactory(This,ppTypeFactory)	\
    ( (This)->lpVtbl -> CreateTypeFactory(This,ppTypeFactory) ) 

#define IModuleInfo_GetMethodInfoById(This,functionID,ppMethodInfo)	\
    ( (This)->lpVtbl -> GetMethodInfoById(This,functionID,ppMethodInfo) ) 

#define IModuleInfo_GetMethodInfoByToken(This,token,ppMethodInfo)	\
    ( (This)->lpVtbl -> GetMethodInfoByToken(This,token,ppMethodInfo) ) 

#define IModuleInfo_ImportModule(This,pSourceModuleMetadataImport,pSourceImage)	\
    ( (This)->lpVtbl -> ImportModule(This,pSourceModuleMetadataImport,pSourceImage) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IModuleInfo_INTERFACE_DEFINED__ */


#ifndef __IModuleInfo2_INTERFACE_DEFINED__
#define __IModuleInfo2_INTERFACE_DEFINED__

/* interface IModuleInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IModuleInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4200c448-7ede-4e61-ae67-b017d3021f12")
    IModuleInfo2 : public IModuleInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetIsFlatLayout( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResolveRva( 
            /* [in] */ DWORD rva,
            /* [out] */ LPCBYTE *ppbResolvedAddress) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IModuleInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IModuleInfo2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IModuleInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IModuleInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleName )( 
            IModuleInfo2 * This,
            /* [out] */ BSTR *pbstrModuleName);
        
        HRESULT ( STDMETHODCALLTYPE *GetFullPath )( 
            IModuleInfo2 * This,
            /* [out] */ BSTR *pbstrFullPath);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyInfo )( 
            IModuleInfo2 * This,
            /* [out] */ IAssemblyInfo **ppAssemblyInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainInfo )( 
            IModuleInfo2 * This,
            /* [out] */ IAppDomainInfo **ppAppDomainInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataImport )( 
            IModuleInfo2 * This,
            /* [out] */ IUnknown **ppMetaDataImport);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataAssemblyImport )( 
            IModuleInfo2 * This,
            /* [out] */ IUnknown **ppMetaDataAssemblyImport);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataEmit )( 
            IModuleInfo2 * This,
            /* [out] */ IUnknown **ppMetaDataEmit);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetaDataAssemblyEmit )( 
            IModuleInfo2 * This,
            /* [out] */ IUnknown **ppMetaDataAssemblyEmit);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleID )( 
            IModuleInfo2 * This,
            /* [out] */ ModuleID *pModuleId);
        
        HRESULT ( STDMETHODCALLTYPE *GetMVID )( 
            IModuleInfo2 * This,
            /* [out] */ GUID *pguidMvid);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsILOnly )( 
            IModuleInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsMscorlib )( 
            IModuleInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsDynamic )( 
            IModuleInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsNgen )( 
            IModuleInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsWinRT )( 
            IModuleInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIs64bit )( 
            IModuleInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetImageBase )( 
            IModuleInfo2 * This,
            /* [out] */ LPCBYTE *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorHeader )( 
            IModuleInfo2 * This,
            /* [in] */ DWORD cbValue,
            /* [length_is][size_is][out] */ BYTE *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetEntrypointToken )( 
            IModuleInfo2 * This,
            /* [out] */ DWORD *pdwEntrypointToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleVersion )( 
            IModuleInfo2 * This,
            /* [in] */ DWORD cbValue,
            /* [length_is][size_is][out] */ BYTE *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *RequestRejit )( 
            IModuleInfo2 * This,
            /* [in] */ mdToken methodToken);
        
        HRESULT ( STDMETHODCALLTYPE *CreateTypeFactory )( 
            IModuleInfo2 * This,
            /* [out] */ ITypeCreator **ppTypeFactory);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodInfoById )( 
            IModuleInfo2 * This,
            /* [in] */ FunctionID functionID,
            /* [out] */ IMethodInfo **ppMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodInfoByToken )( 
            IModuleInfo2 * This,
            /* [in] */ mdToken token,
            /* [out] */ IMethodInfo **ppMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ImportModule )( 
            IModuleInfo2 * This,
            /* [in] */ IUnknown *pSourceModuleMetadataImport,
            /* [in] */ LPCBYTE *pSourceImage);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsFlatLayout )( 
            IModuleInfo2 * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *ResolveRva )( 
            IModuleInfo2 * This,
            /* [in] */ DWORD rva,
            /* [out] */ LPCBYTE *ppbResolvedAddress);
        
        END_INTERFACE
    } IModuleInfo2Vtbl;

    interface IModuleInfo2
    {
        CONST_VTBL struct IModuleInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IModuleInfo2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IModuleInfo2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IModuleInfo2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IModuleInfo2_GetModuleName(This,pbstrModuleName)	\
    ( (This)->lpVtbl -> GetModuleName(This,pbstrModuleName) ) 

#define IModuleInfo2_GetFullPath(This,pbstrFullPath)	\
    ( (This)->lpVtbl -> GetFullPath(This,pbstrFullPath) ) 

#define IModuleInfo2_GetAssemblyInfo(This,ppAssemblyInfo)	\
    ( (This)->lpVtbl -> GetAssemblyInfo(This,ppAssemblyInfo) ) 

#define IModuleInfo2_GetAppDomainInfo(This,ppAppDomainInfo)	\
    ( (This)->lpVtbl -> GetAppDomainInfo(This,ppAppDomainInfo) ) 

#define IModuleInfo2_GetMetaDataImport(This,ppMetaDataImport)	\
    ( (This)->lpVtbl -> GetMetaDataImport(This,ppMetaDataImport) ) 

#define IModuleInfo2_GetMetaDataAssemblyImport(This,ppMetaDataAssemblyImport)	\
    ( (This)->lpVtbl -> GetMetaDataAssemblyImport(This,ppMetaDataAssemblyImport) ) 

#define IModuleInfo2_GetMetaDataEmit(This,ppMetaDataEmit)	\
    ( (This)->lpVtbl -> GetMetaDataEmit(This,ppMetaDataEmit) ) 

#define IModuleInfo2_GetMetaDataAssemblyEmit(This,ppMetaDataAssemblyEmit)	\
    ( (This)->lpVtbl -> GetMetaDataAssemblyEmit(This,ppMetaDataAssemblyEmit) ) 

#define IModuleInfo2_GetModuleID(This,pModuleId)	\
    ( (This)->lpVtbl -> GetModuleID(This,pModuleId) ) 

#define IModuleInfo2_GetMVID(This,pguidMvid)	\
    ( (This)->lpVtbl -> GetMVID(This,pguidMvid) ) 

#define IModuleInfo2_GetIsILOnly(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsILOnly(This,pbValue) ) 

#define IModuleInfo2_GetIsMscorlib(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsMscorlib(This,pbValue) ) 

#define IModuleInfo2_GetIsDynamic(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsDynamic(This,pbValue) ) 

#define IModuleInfo2_GetIsNgen(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsNgen(This,pbValue) ) 

#define IModuleInfo2_GetIsWinRT(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsWinRT(This,pbValue) ) 

#define IModuleInfo2_GetIs64bit(This,pbValue)	\
    ( (This)->lpVtbl -> GetIs64bit(This,pbValue) ) 

#define IModuleInfo2_GetImageBase(This,pbValue)	\
    ( (This)->lpVtbl -> GetImageBase(This,pbValue) ) 

#define IModuleInfo2_GetCorHeader(This,cbValue,pbValue)	\
    ( (This)->lpVtbl -> GetCorHeader(This,cbValue,pbValue) ) 

#define IModuleInfo2_GetEntrypointToken(This,pdwEntrypointToken)	\
    ( (This)->lpVtbl -> GetEntrypointToken(This,pdwEntrypointToken) ) 

#define IModuleInfo2_GetModuleVersion(This,cbValue,pbValue)	\
    ( (This)->lpVtbl -> GetModuleVersion(This,cbValue,pbValue) ) 

#define IModuleInfo2_RequestRejit(This,methodToken)	\
    ( (This)->lpVtbl -> RequestRejit(This,methodToken) ) 

#define IModuleInfo2_CreateTypeFactory(This,ppTypeFactory)	\
    ( (This)->lpVtbl -> CreateTypeFactory(This,ppTypeFactory) ) 

#define IModuleInfo2_GetMethodInfoById(This,functionID,ppMethodInfo)	\
    ( (This)->lpVtbl -> GetMethodInfoById(This,functionID,ppMethodInfo) ) 

#define IModuleInfo2_GetMethodInfoByToken(This,token,ppMethodInfo)	\
    ( (This)->lpVtbl -> GetMethodInfoByToken(This,token,ppMethodInfo) ) 

#define IModuleInfo2_ImportModule(This,pSourceModuleMetadataImport,pSourceImage)	\
    ( (This)->lpVtbl -> ImportModule(This,pSourceModuleMetadataImport,pSourceImage) ) 


#define IModuleInfo2_GetIsFlatLayout(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsFlatLayout(This,pbValue) ) 

#define IModuleInfo2_ResolveRva(This,rva,ppbResolvedAddress)	\
    ( (This)->lpVtbl -> ResolveRva(This,rva,ppbResolvedAddress) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IModuleInfo2_INTERFACE_DEFINED__ */


#ifndef __IEnumModuleInfo_INTERFACE_DEFINED__
#define __IEnumModuleInfo_INTERFACE_DEFINED__

/* interface IEnumModuleInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumModuleInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683b3d0b-5cab-49ac-9242-c7de190c7764")
    IEnumModuleInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IModuleInfo **rgelt,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumModuleInfo **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumModuleInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumModuleInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumModuleInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumModuleInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumModuleInfo * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IModuleInfo **rgelt,
            /* [in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumModuleInfo * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumModuleInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumModuleInfo * This,
            /* [out] */ IEnumModuleInfo **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumModuleInfo * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumModuleInfoVtbl;

    interface IEnumModuleInfo
    {
        CONST_VTBL struct IEnumModuleInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumModuleInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumModuleInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumModuleInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumModuleInfo_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumModuleInfo_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumModuleInfo_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumModuleInfo_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumModuleInfo_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumModuleInfo_INTERFACE_DEFINED__ */


#ifndef __IAppDomainInfo_INTERFACE_DEFINED__
#define __IAppDomainInfo_INTERFACE_DEFINED__

/* interface IAppDomainInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAppDomainInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A81A5232-4693-47E9-A74D-BB4C71164659")
    IAppDomainInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainId( 
            /* [out] */ AppDomainID *pAppDomainId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsSystemDomain( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsSharedDomain( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsDefaultDomain( 
            /* [out] */ BOOL *pbValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblies( 
            /* [out] */ IEnumAssemblyInfo **ppAssemblyInfos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModules( 
            /* [out] */ IEnumModuleInfo **ppModuleInfos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyInfoById( 
            /* [in] */ AssemblyID assemblyID,
            /* [out] */ IAssemblyInfo **ppAssemblyInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyInfosByName( 
            /* [in] */ BSTR pszAssemblyName,
            /* [out] */ IEnumAssemblyInfo **ppAssemblyInfos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleCount( 
            /* [out] */ ULONG *pcModuleInfos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleInfoById( 
            /* [in] */ ModuleID moduleId,
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleInfosByMvid( 
            /* [in] */ GUID mvid,
            /* [out] */ IEnumModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleInfosByName( 
            /* [in] */ BSTR pszModuleName,
            /* [out] */ IEnumModuleInfo **ppModuleInfo) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAppDomainInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAppDomainInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAppDomainInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAppDomainInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainId )( 
            IAppDomainInfo * This,
            /* [out] */ AppDomainID *pAppDomainId);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsSystemDomain )( 
            IAppDomainInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsSharedDomain )( 
            IAppDomainInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsDefaultDomain )( 
            IAppDomainInfo * This,
            /* [out] */ BOOL *pbValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IAppDomainInfo * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblies )( 
            IAppDomainInfo * This,
            /* [out] */ IEnumAssemblyInfo **ppAssemblyInfos);
        
        HRESULT ( STDMETHODCALLTYPE *GetModules )( 
            IAppDomainInfo * This,
            /* [out] */ IEnumModuleInfo **ppModuleInfos);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyInfoById )( 
            IAppDomainInfo * This,
            /* [in] */ AssemblyID assemblyID,
            /* [out] */ IAssemblyInfo **ppAssemblyInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyInfosByName )( 
            IAppDomainInfo * This,
            /* [in] */ BSTR pszAssemblyName,
            /* [out] */ IEnumAssemblyInfo **ppAssemblyInfos);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleCount )( 
            IAppDomainInfo * This,
            /* [out] */ ULONG *pcModuleInfos);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleInfoById )( 
            IAppDomainInfo * This,
            /* [in] */ ModuleID moduleId,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleInfosByMvid )( 
            IAppDomainInfo * This,
            /* [in] */ GUID mvid,
            /* [out] */ IEnumModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleInfosByName )( 
            IAppDomainInfo * This,
            /* [in] */ BSTR pszModuleName,
            /* [out] */ IEnumModuleInfo **ppModuleInfo);
        
        END_INTERFACE
    } IAppDomainInfoVtbl;

    interface IAppDomainInfo
    {
        CONST_VTBL struct IAppDomainInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAppDomainInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAppDomainInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAppDomainInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAppDomainInfo_GetAppDomainId(This,pAppDomainId)	\
    ( (This)->lpVtbl -> GetAppDomainId(This,pAppDomainId) ) 

#define IAppDomainInfo_GetIsSystemDomain(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsSystemDomain(This,pbValue) ) 

#define IAppDomainInfo_GetIsSharedDomain(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsSharedDomain(This,pbValue) ) 

#define IAppDomainInfo_GetIsDefaultDomain(This,pbValue)	\
    ( (This)->lpVtbl -> GetIsDefaultDomain(This,pbValue) ) 

#define IAppDomainInfo_GetName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetName(This,pbstrName) ) 

#define IAppDomainInfo_GetAssemblies(This,ppAssemblyInfos)	\
    ( (This)->lpVtbl -> GetAssemblies(This,ppAssemblyInfos) ) 

#define IAppDomainInfo_GetModules(This,ppModuleInfos)	\
    ( (This)->lpVtbl -> GetModules(This,ppModuleInfos) ) 

#define IAppDomainInfo_GetAssemblyInfoById(This,assemblyID,ppAssemblyInfo)	\
    ( (This)->lpVtbl -> GetAssemblyInfoById(This,assemblyID,ppAssemblyInfo) ) 

#define IAppDomainInfo_GetAssemblyInfosByName(This,pszAssemblyName,ppAssemblyInfos)	\
    ( (This)->lpVtbl -> GetAssemblyInfosByName(This,pszAssemblyName,ppAssemblyInfos) ) 

#define IAppDomainInfo_GetModuleCount(This,pcModuleInfos)	\
    ( (This)->lpVtbl -> GetModuleCount(This,pcModuleInfos) ) 

#define IAppDomainInfo_GetModuleInfoById(This,moduleId,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleInfoById(This,moduleId,ppModuleInfo) ) 

#define IAppDomainInfo_GetModuleInfosByMvid(This,mvid,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleInfosByMvid(This,mvid,ppModuleInfo) ) 

#define IAppDomainInfo_GetModuleInfosByName(This,pszModuleName,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleInfosByName(This,pszModuleName,ppModuleInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAppDomainInfo_INTERFACE_DEFINED__ */


#ifndef __IEnumAppDomainInfo_INTERFACE_DEFINED__
#define __IEnumAppDomainInfo_INTERFACE_DEFINED__

/* interface IEnumAppDomainInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumAppDomainInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C2A3E353-08BB-4A13-851E-07B1BB4AD57C")
    IEnumAppDomainInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IAppDomainInfo **rgelt,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumAppDomainInfo **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumAppDomainInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumAppDomainInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumAppDomainInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumAppDomainInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumAppDomainInfo * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IAppDomainInfo **rgelt,
            /* [in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumAppDomainInfo * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumAppDomainInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumAppDomainInfo * This,
            /* [out] */ IEnumAppDomainInfo **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumAppDomainInfo * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumAppDomainInfoVtbl;

    interface IEnumAppDomainInfo
    {
        CONST_VTBL struct IEnumAppDomainInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumAppDomainInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumAppDomainInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumAppDomainInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumAppDomainInfo_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumAppDomainInfo_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumAppDomainInfo_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumAppDomainInfo_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumAppDomainInfo_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumAppDomainInfo_INTERFACE_DEFINED__ */


#ifndef __ILocalVariableCollection_INTERFACE_DEFINED__
#define __ILocalVariableCollection_INTERFACE_DEFINED__

/* interface ILocalVariableCollection */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ILocalVariableCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("353F806F-6563-40E0-8EBE-B93A58C0145F")
    ILocalVariableCollection : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorSignature( 
            /* [out] */ ISignatureBuilder **ppSignature) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pdwCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddLocal( 
            /* [in] */ IType *pType,
            /* [optional][out] */ DWORD *pIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplaceSignature( 
            /* [in] */ const BYTE *pSignature,
            /* [in] */ DWORD dwSigSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitSignature( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ILocalVariableCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILocalVariableCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILocalVariableCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILocalVariableCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            ILocalVariableCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorSignature )( 
            ILocalVariableCollection * This,
            /* [out] */ ISignatureBuilder **ppSignature);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            ILocalVariableCollection * This,
            /* [out] */ DWORD *pdwCount);
        
        HRESULT ( STDMETHODCALLTYPE *AddLocal )( 
            ILocalVariableCollection * This,
            /* [in] */ IType *pType,
            /* [optional][out] */ DWORD *pIndex);
        
        HRESULT ( STDMETHODCALLTYPE *ReplaceSignature )( 
            ILocalVariableCollection * This,
            /* [in] */ const BYTE *pSignature,
            /* [in] */ DWORD dwSigSize);
        
        HRESULT ( STDMETHODCALLTYPE *CommitSignature )( 
            ILocalVariableCollection * This);
        
        END_INTERFACE
    } ILocalVariableCollectionVtbl;

    interface ILocalVariableCollection
    {
        CONST_VTBL struct ILocalVariableCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILocalVariableCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILocalVariableCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILocalVariableCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILocalVariableCollection_Initialize(This)	\
    ( (This)->lpVtbl -> Initialize(This) ) 

#define ILocalVariableCollection_GetCorSignature(This,ppSignature)	\
    ( (This)->lpVtbl -> GetCorSignature(This,ppSignature) ) 

#define ILocalVariableCollection_GetCount(This,pdwCount)	\
    ( (This)->lpVtbl -> GetCount(This,pdwCount) ) 

#define ILocalVariableCollection_AddLocal(This,pType,pIndex)	\
    ( (This)->lpVtbl -> AddLocal(This,pType,pIndex) ) 

#define ILocalVariableCollection_ReplaceSignature(This,pSignature,dwSigSize)	\
    ( (This)->lpVtbl -> ReplaceSignature(This,pSignature,dwSigSize) ) 

#define ILocalVariableCollection_CommitSignature(This)	\
    ( (This)->lpVtbl -> CommitSignature(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILocalVariableCollection_INTERFACE_DEFINED__ */


#ifndef __IType_INTERFACE_DEFINED__
#define __IType_INTERFACE_DEFINED__

/* interface IType */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6FC96859-ED89-4D9F-A7C9-1DAD7EC35F67")
    IType : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddToSignature( 
            /* [in] */ ISignatureBuilder *pSignatureBuilder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorElementType( 
            /* [out] */ CorElementType *pCorType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsPrimitive( 
            /* [out] */ BOOL *pIsPrimitive) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsArray( 
            /* [out] */ BOOL *pIsArray) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsClass( 
            /* [out] */ BOOL *pIsClass) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IType * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IType * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddToSignature )( 
            IType * This,
            /* [in] */ ISignatureBuilder *pSignatureBuilder);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorElementType )( 
            IType * This,
            /* [out] */ CorElementType *pCorType);
        
        HRESULT ( STDMETHODCALLTYPE *IsPrimitive )( 
            IType * This,
            /* [out] */ BOOL *pIsPrimitive);
        
        HRESULT ( STDMETHODCALLTYPE *IsArray )( 
            IType * This,
            /* [out] */ BOOL *pIsArray);
        
        HRESULT ( STDMETHODCALLTYPE *IsClass )( 
            IType * This,
            /* [out] */ BOOL *pIsClass);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IType * This,
            /* [out] */ BSTR *pbstrName);
        
        END_INTERFACE
    } ITypeVtbl;

    interface IType
    {
        CONST_VTBL struct ITypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IType_AddToSignature(This,pSignatureBuilder)	\
    ( (This)->lpVtbl -> AddToSignature(This,pSignatureBuilder) ) 

#define IType_GetCorElementType(This,pCorType)	\
    ( (This)->lpVtbl -> GetCorElementType(This,pCorType) ) 

#define IType_IsPrimitive(This,pIsPrimitive)	\
    ( (This)->lpVtbl -> IsPrimitive(This,pIsPrimitive) ) 

#define IType_IsArray(This,pIsArray)	\
    ( (This)->lpVtbl -> IsArray(This,pIsArray) ) 

#define IType_IsClass(This,pIsClass)	\
    ( (This)->lpVtbl -> IsClass(This,pIsClass) ) 

#define IType_GetName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetName(This,pbstrName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IType_INTERFACE_DEFINED__ */


#ifndef __IAppDomainCollection_INTERFACE_DEFINED__
#define __IAppDomainCollection_INTERFACE_DEFINED__

/* interface IAppDomainCollection */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAppDomainCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C79F6730-C5FB-40C4-B528-0A0248CA4DEB")
    IAppDomainCollection : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainCount( 
            /* [out] */ DWORD *pdwCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainById( 
            /* [in] */ AppDomainID appDomainId,
            /* [out] */ IAppDomainInfo **ppAppDomainInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainIDs( 
            /* [in] */ DWORD cAppDomains,
            /* [out] */ DWORD *pcActual,
            /* [length_is][size_is][out] */ AppDomainID *AppDomainIDs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAppDomains( 
            /* [out] */ IEnumAppDomainInfo **ppEnumAppDomains) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyInfoById( 
            /* [in] */ AssemblyID assemblyID,
            /* [out] */ IAssemblyInfo **ppAssemblyInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleInfoById( 
            /* [in] */ ModuleID moduleID,
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModuleInfosByMvid( 
            /* [in] */ GUID mvid,
            /* [out] */ IEnumModuleInfo **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMethodInfoById( 
            /* [in] */ FunctionID functionID,
            /* [out] */ IMethodInfo **ppMethodInfo) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAppDomainCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAppDomainCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAppDomainCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAppDomainCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainCount )( 
            IAppDomainCollection * This,
            /* [out] */ DWORD *pdwCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainById )( 
            IAppDomainCollection * This,
            /* [in] */ AppDomainID appDomainId,
            /* [out] */ IAppDomainInfo **ppAppDomainInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainIDs )( 
            IAppDomainCollection * This,
            /* [in] */ DWORD cAppDomains,
            /* [out] */ DWORD *pcActual,
            /* [length_is][size_is][out] */ AppDomainID *AppDomainIDs);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomains )( 
            IAppDomainCollection * This,
            /* [out] */ IEnumAppDomainInfo **ppEnumAppDomains);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyInfoById )( 
            IAppDomainCollection * This,
            /* [in] */ AssemblyID assemblyID,
            /* [out] */ IAssemblyInfo **ppAssemblyInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleInfoById )( 
            IAppDomainCollection * This,
            /* [in] */ ModuleID moduleID,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetModuleInfosByMvid )( 
            IAppDomainCollection * This,
            /* [in] */ GUID mvid,
            /* [out] */ IEnumModuleInfo **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodInfoById )( 
            IAppDomainCollection * This,
            /* [in] */ FunctionID functionID,
            /* [out] */ IMethodInfo **ppMethodInfo);
        
        END_INTERFACE
    } IAppDomainCollectionVtbl;

    interface IAppDomainCollection
    {
        CONST_VTBL struct IAppDomainCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAppDomainCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAppDomainCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAppDomainCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAppDomainCollection_GetAppDomainCount(This,pdwCount)	\
    ( (This)->lpVtbl -> GetAppDomainCount(This,pdwCount) ) 

#define IAppDomainCollection_GetAppDomainById(This,appDomainId,ppAppDomainInfo)	\
    ( (This)->lpVtbl -> GetAppDomainById(This,appDomainId,ppAppDomainInfo) ) 

#define IAppDomainCollection_GetAppDomainIDs(This,cAppDomains,pcActual,AppDomainIDs)	\
    ( (This)->lpVtbl -> GetAppDomainIDs(This,cAppDomains,pcActual,AppDomainIDs) ) 

#define IAppDomainCollection_GetAppDomains(This,ppEnumAppDomains)	\
    ( (This)->lpVtbl -> GetAppDomains(This,ppEnumAppDomains) ) 

#define IAppDomainCollection_GetAssemblyInfoById(This,assemblyID,ppAssemblyInfo)	\
    ( (This)->lpVtbl -> GetAssemblyInfoById(This,assemblyID,ppAssemblyInfo) ) 

#define IAppDomainCollection_GetModuleInfoById(This,moduleID,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetModuleInfoById(This,moduleID,ppModuleInfo) ) 

#define IAppDomainCollection_GetModuleInfosByMvid(This,mvid,ppEnum)	\
    ( (This)->lpVtbl -> GetModuleInfosByMvid(This,mvid,ppEnum) ) 

#define IAppDomainCollection_GetMethodInfoById(This,functionID,ppMethodInfo)	\
    ( (This)->lpVtbl -> GetMethodInfoById(This,functionID,ppMethodInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAppDomainCollection_INTERFACE_DEFINED__ */


#ifndef __ISignatureBuilder_INTERFACE_DEFINED__
#define __ISignatureBuilder_INTERFACE_DEFINED__

/* interface ISignatureBuilder */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ISignatureBuilder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F574823E-4863-4013-A4EA-C6D9943246E6")
    ISignatureBuilder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ DWORD value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSignedInt( 
            /* [in] */ LONG value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddToken( 
            /* [in] */ mdToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddElementType( 
            /* [in] */ CorElementType type) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddData( 
            /* [in] */ const BYTE *pData,
            /* [in] */ DWORD cbSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSignature( 
            /* [in] */ ISignatureBuilder *pSignature) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ DWORD *pcbSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorSignature( 
            /* [in] */ DWORD cbBuffer,
            /* [size_is][length_is][out] */ BYTE *pCorSignature,
            /* [out] */ DWORD *pcbSignature) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorSignaturePtr( 
            /* [out] */ const BYTE **ppSignature) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISignatureBuilderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISignatureBuilder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISignatureBuilder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISignatureBuilder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISignatureBuilder * This,
            /* [in] */ DWORD value);
        
        HRESULT ( STDMETHODCALLTYPE *AddSignedInt )( 
            ISignatureBuilder * This,
            /* [in] */ LONG value);
        
        HRESULT ( STDMETHODCALLTYPE *AddToken )( 
            ISignatureBuilder * This,
            /* [in] */ mdToken token);
        
        HRESULT ( STDMETHODCALLTYPE *AddElementType )( 
            ISignatureBuilder * This,
            /* [in] */ CorElementType type);
        
        HRESULT ( STDMETHODCALLTYPE *AddData )( 
            ISignatureBuilder * This,
            /* [in] */ const BYTE *pData,
            /* [in] */ DWORD cbSize);
        
        HRESULT ( STDMETHODCALLTYPE *AddSignature )( 
            ISignatureBuilder * This,
            /* [in] */ ISignatureBuilder *pSignature);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISignatureBuilder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            ISignatureBuilder * This,
            /* [out] */ DWORD *pcbSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorSignature )( 
            ISignatureBuilder * This,
            /* [in] */ DWORD cbBuffer,
            /* [size_is][length_is][out] */ BYTE *pCorSignature,
            /* [out] */ DWORD *pcbSignature);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorSignaturePtr )( 
            ISignatureBuilder * This,
            /* [out] */ const BYTE **ppSignature);
        
        END_INTERFACE
    } ISignatureBuilderVtbl;

    interface ISignatureBuilder
    {
        CONST_VTBL struct ISignatureBuilderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISignatureBuilder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISignatureBuilder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISignatureBuilder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISignatureBuilder_Add(This,value)	\
    ( (This)->lpVtbl -> Add(This,value) ) 

#define ISignatureBuilder_AddSignedInt(This,value)	\
    ( (This)->lpVtbl -> AddSignedInt(This,value) ) 

#define ISignatureBuilder_AddToken(This,token)	\
    ( (This)->lpVtbl -> AddToken(This,token) ) 

#define ISignatureBuilder_AddElementType(This,type)	\
    ( (This)->lpVtbl -> AddElementType(This,type) ) 

#define ISignatureBuilder_AddData(This,pData,cbSize)	\
    ( (This)->lpVtbl -> AddData(This,pData,cbSize) ) 

#define ISignatureBuilder_AddSignature(This,pSignature)	\
    ( (This)->lpVtbl -> AddSignature(This,pSignature) ) 

#define ISignatureBuilder_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISignatureBuilder_GetSize(This,pcbSize)	\
    ( (This)->lpVtbl -> GetSize(This,pcbSize) ) 

#define ISignatureBuilder_GetCorSignature(This,cbBuffer,pCorSignature,pcbSignature)	\
    ( (This)->lpVtbl -> GetCorSignature(This,cbBuffer,pCorSignature,pcbSignature) ) 

#define ISignatureBuilder_GetCorSignaturePtr(This,ppSignature)	\
    ( (This)->lpVtbl -> GetCorSignaturePtr(This,ppSignature) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISignatureBuilder_INTERFACE_DEFINED__ */


#ifndef __ITypeCreator_INTERFACE_DEFINED__
#define __ITypeCreator_INTERFACE_DEFINED__

/* interface ITypeCreator */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITypeCreator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C6D612FA-B550-48E3-8859-DE440CF66627")
    ITypeCreator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FromSignature( 
            /* [in] */ DWORD cbBuffer,
            /* [in] */ const BYTE *pCorSignature,
            /* [out] */ IType **ppType,
            /* [optional][out] */ DWORD *pdwSigSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FromCorElement( 
            /* [in] */ CorElementType type,
            /* [out] */ IType **ppType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FromToken( 
            /* [in] */ CorElementType type,
            /* [in] */ mdToken token,
            /* [out] */ IType **ppType) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITypeCreatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITypeCreator * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITypeCreator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITypeCreator * This);
        
        HRESULT ( STDMETHODCALLTYPE *FromSignature )( 
            ITypeCreator * This,
            /* [in] */ DWORD cbBuffer,
            /* [in] */ const BYTE *pCorSignature,
            /* [out] */ IType **ppType,
            /* [optional][out] */ DWORD *pdwSigSize);
        
        HRESULT ( STDMETHODCALLTYPE *FromCorElement )( 
            ITypeCreator * This,
            /* [in] */ CorElementType type,
            /* [out] */ IType **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *FromToken )( 
            ITypeCreator * This,
            /* [in] */ CorElementType type,
            /* [in] */ mdToken token,
            /* [out] */ IType **ppType);
        
        END_INTERFACE
    } ITypeCreatorVtbl;

    interface ITypeCreator
    {
        CONST_VTBL struct ITypeCreatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITypeCreator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITypeCreator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITypeCreator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITypeCreator_FromSignature(This,cbBuffer,pCorSignature,ppType,pdwSigSize)	\
    ( (This)->lpVtbl -> FromSignature(This,cbBuffer,pCorSignature,ppType,pdwSigSize) ) 

#define ITypeCreator_FromCorElement(This,type,ppType)	\
    ( (This)->lpVtbl -> FromCorElement(This,type,ppType) ) 

#define ITypeCreator_FromToken(This,type,token,ppType)	\
    ( (This)->lpVtbl -> FromToken(This,type,token,ppType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITypeCreator_INTERFACE_DEFINED__ */


#ifndef __IMethodLocal_INTERFACE_DEFINED__
#define __IMethodLocal_INTERFACE_DEFINED__

/* interface IMethodLocal */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IMethodLocal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F8C007DB-0D35-4726-9EDC-781590E30688")
    IMethodLocal : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [out] */ IType **ppType) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMethodLocalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMethodLocal * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMethodLocal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMethodLocal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IMethodLocal * This,
            /* [out] */ IType **ppType);
        
        END_INTERFACE
    } IMethodLocalVtbl;

    interface IMethodLocal
    {
        CONST_VTBL struct IMethodLocalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMethodLocal_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMethodLocal_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMethodLocal_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMethodLocal_GetType(This,ppType)	\
    ( (This)->lpVtbl -> GetType(This,ppType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMethodLocal_INTERFACE_DEFINED__ */


#ifndef __IMethodParameter_INTERFACE_DEFINED__
#define __IMethodParameter_INTERFACE_DEFINED__

/* interface IMethodParameter */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IMethodParameter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("26255678-9F51-433F-89B1-51B978EB4C2B")
    IMethodParameter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [out] */ IType **ppType) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMethodParameterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMethodParameter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMethodParameter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMethodParameter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IMethodParameter * This,
            /* [out] */ IType **ppType);
        
        END_INTERFACE
    } IMethodParameterVtbl;

    interface IMethodParameter
    {
        CONST_VTBL struct IMethodParameterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMethodParameter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMethodParameter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMethodParameter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMethodParameter_GetType(This,ppType)	\
    ( (This)->lpVtbl -> GetType(This,ppType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMethodParameter_INTERFACE_DEFINED__ */


#ifndef __IEnumMethodLocals_INTERFACE_DEFINED__
#define __IEnumMethodLocals_INTERFACE_DEFINED__

/* interface IEnumMethodLocals */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumMethodLocals;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C4440146-7E2D-4B1A-8F69-D6E4817D7295")
    IEnumMethodLocals : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IMethodLocal **rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumMethodLocals **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumMethodLocalsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumMethodLocals * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumMethodLocals * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumMethodLocals * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumMethodLocals * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IMethodLocal **rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumMethodLocals * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumMethodLocals * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumMethodLocals * This,
            /* [out] */ IEnumMethodLocals **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumMethodLocals * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumMethodLocalsVtbl;

    interface IEnumMethodLocals
    {
        CONST_VTBL struct IEnumMethodLocalsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumMethodLocals_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumMethodLocals_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumMethodLocals_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumMethodLocals_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumMethodLocals_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumMethodLocals_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumMethodLocals_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumMethodLocals_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumMethodLocals_INTERFACE_DEFINED__ */


#ifndef __IEnumMethodParameters_INTERFACE_DEFINED__
#define __IEnumMethodParameters_INTERFACE_DEFINED__

/* interface IEnumMethodParameters */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumMethodParameters;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2DBC9FAB-93BD-4733-82FA-EA3B3D558A0B")
    IEnumMethodParameters : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IMethodParameter **rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumMethodParameters **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumMethodParametersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumMethodParameters * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumMethodParameters * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumMethodParameters * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumMethodParameters * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IMethodParameter **rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumMethodParameters * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumMethodParameters * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumMethodParameters * This,
            /* [out] */ IEnumMethodParameters **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumMethodParameters * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumMethodParametersVtbl;

    interface IEnumMethodParameters
    {
        CONST_VTBL struct IEnumMethodParametersVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumMethodParameters_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumMethodParameters_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumMethodParameters_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumMethodParameters_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumMethodParameters_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumMethodParameters_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumMethodParameters_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumMethodParameters_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumMethodParameters_INTERFACE_DEFINED__ */


#ifndef __ISingleRetDefaultInstrumentation_INTERFACE_DEFINED__
#define __ISingleRetDefaultInstrumentation_INTERFACE_DEFINED__

/* interface ISingleRetDefaultInstrumentation */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ISingleRetDefaultInstrumentation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2ED40F43-E51A-41A6-91FC-6FA9163C62E9")
    ISingleRetDefaultInstrumentation : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IInstructionGraph *pInstructionGraph) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplySingleRetDefaultInstrumentation( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISingleRetDefaultInstrumentationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISingleRetDefaultInstrumentation * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISingleRetDefaultInstrumentation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISingleRetDefaultInstrumentation * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            ISingleRetDefaultInstrumentation * This,
            /* [in] */ IInstructionGraph *pInstructionGraph);
        
        HRESULT ( STDMETHODCALLTYPE *ApplySingleRetDefaultInstrumentation )( 
            ISingleRetDefaultInstrumentation * This);
        
        END_INTERFACE
    } ISingleRetDefaultInstrumentationVtbl;

    interface ISingleRetDefaultInstrumentation
    {
        CONST_VTBL struct ISingleRetDefaultInstrumentationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISingleRetDefaultInstrumentation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISingleRetDefaultInstrumentation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISingleRetDefaultInstrumentation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISingleRetDefaultInstrumentation_Initialize(This,pInstructionGraph)	\
    ( (This)->lpVtbl -> Initialize(This,pInstructionGraph) ) 

#define ISingleRetDefaultInstrumentation_ApplySingleRetDefaultInstrumentation(This)	\
    ( (This)->lpVtbl -> ApplySingleRetDefaultInstrumentation(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISingleRetDefaultInstrumentation_INTERFACE_DEFINED__ */


#ifndef __IProfilerManager2_INTERFACE_DEFINED__
#define __IProfilerManager2_INTERFACE_DEFINED__

/* interface IProfilerManager2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IProfilerManager2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DCB0764D-E18F-4F9A-91E8-6A40FCFE6775")
    IProfilerManager2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE DisableProfiling( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyMetadata( 
            /* [in] */ IModuleInfo *pMethodInfo) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProfilerManager2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfilerManager2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfilerManager2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfilerManager2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *DisableProfiling )( 
            IProfilerManager2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyMetadata )( 
            IProfilerManager2 * This,
            /* [in] */ IModuleInfo *pMethodInfo);
        
        END_INTERFACE
    } IProfilerManager2Vtbl;

    interface IProfilerManager2
    {
        CONST_VTBL struct IProfilerManager2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfilerManager2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProfilerManager2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProfilerManager2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProfilerManager2_DisableProfiling(This)	\
    ( (This)->lpVtbl -> DisableProfiling(This) ) 

#define IProfilerManager2_ApplyMetadata(This,pMethodInfo)	\
    ( (This)->lpVtbl -> ApplyMetadata(This,pMethodInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProfilerManager2_INTERFACE_DEFINED__ */


#ifndef __IInstrumentationMethodExceptionEvents_INTERFACE_DEFINED__
#define __IInstrumentationMethodExceptionEvents_INTERFACE_DEFINED__

/* interface IInstrumentationMethodExceptionEvents */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IInstrumentationMethodExceptionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8310B758-6642-46AD-9423-DDA5F9E278AE")
    IInstrumentationMethodExceptionEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherEnter( 
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ UINT_PTR objectId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherLeave( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionSearchCatcherFound( 
            /* [in] */ IMethodInfo *pMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterEnter( 
            /* [in] */ IMethodInfo *pMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterLeave( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionEnter( 
            /* [in] */ IMethodInfo *pMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionLeave( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionThrown( 
            /* [in] */ UINT_PTR thrownObjectId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyEnter( 
            /* [in] */ IMethodInfo *pMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyLeave( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionEnter( 
            /* [in] */ IMethodInfo *pMethodInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionLeave( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInstrumentationMethodExceptionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInstrumentationMethodExceptionEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInstrumentationMethodExceptionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionCatcherEnter )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ IMethodInfo *pMethodInfo,
            /* [in] */ UINT_PTR objectId);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionCatcherLeave )( 
            IInstrumentationMethodExceptionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionSearchCatcherFound )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ IMethodInfo *pMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionSearchFilterEnter )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ IMethodInfo *pMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionSearchFilterLeave )( 
            IInstrumentationMethodExceptionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionSearchFunctionEnter )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ IMethodInfo *pMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionSearchFunctionLeave )( 
            IInstrumentationMethodExceptionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionThrown )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ UINT_PTR thrownObjectId);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionUnwindFinallyEnter )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ IMethodInfo *pMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionUnwindFinallyLeave )( 
            IInstrumentationMethodExceptionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionUnwindFunctionEnter )( 
            IInstrumentationMethodExceptionEvents * This,
            /* [in] */ IMethodInfo *pMethodInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ExceptionUnwindFunctionLeave )( 
            IInstrumentationMethodExceptionEvents * This);
        
        END_INTERFACE
    } IInstrumentationMethodExceptionEventsVtbl;

    interface IInstrumentationMethodExceptionEvents
    {
        CONST_VTBL struct IInstrumentationMethodExceptionEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInstrumentationMethodExceptionEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInstrumentationMethodExceptionEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInstrumentationMethodExceptionEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInstrumentationMethodExceptionEvents_ExceptionCatcherEnter(This,pMethodInfo,objectId)	\
    ( (This)->lpVtbl -> ExceptionCatcherEnter(This,pMethodInfo,objectId) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionCatcherLeave(This)	\
    ( (This)->lpVtbl -> ExceptionCatcherLeave(This) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionSearchCatcherFound(This,pMethodInfo)	\
    ( (This)->lpVtbl -> ExceptionSearchCatcherFound(This,pMethodInfo) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionSearchFilterEnter(This,pMethodInfo)	\
    ( (This)->lpVtbl -> ExceptionSearchFilterEnter(This,pMethodInfo) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionSearchFilterLeave(This)	\
    ( (This)->lpVtbl -> ExceptionSearchFilterLeave(This) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionSearchFunctionEnter(This,pMethodInfo)	\
    ( (This)->lpVtbl -> ExceptionSearchFunctionEnter(This,pMethodInfo) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionSearchFunctionLeave(This)	\
    ( (This)->lpVtbl -> ExceptionSearchFunctionLeave(This) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionThrown(This,thrownObjectId)	\
    ( (This)->lpVtbl -> ExceptionThrown(This,thrownObjectId) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionUnwindFinallyEnter(This,pMethodInfo)	\
    ( (This)->lpVtbl -> ExceptionUnwindFinallyEnter(This,pMethodInfo) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionUnwindFinallyLeave(This)	\
    ( (This)->lpVtbl -> ExceptionUnwindFinallyLeave(This) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionUnwindFunctionEnter(This,pMethodInfo)	\
    ( (This)->lpVtbl -> ExceptionUnwindFunctionEnter(This,pMethodInfo) ) 

#define IInstrumentationMethodExceptionEvents_ExceptionUnwindFunctionLeave(This)	\
    ( (This)->lpVtbl -> ExceptionUnwindFunctionLeave(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInstrumentationMethodExceptionEvents_INTERFACE_DEFINED__ */


#ifndef __IEnumInstructions_INTERFACE_DEFINED__
#define __IEnumInstructions_INTERFACE_DEFINED__

/* interface IEnumInstructions */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumInstructions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A4A827A-046D-4927-BD90-CE9607607280")
    IEnumInstructions : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IInstruction **rgelt,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumInstructions **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumInstructionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumInstructions * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumInstructions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumInstructions * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumInstructions * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IInstruction **rgelt,
            /* [in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumInstructions * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumInstructions * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumInstructions * This,
            /* [out] */ IEnumInstructions **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumInstructions * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumInstructionsVtbl;

    interface IEnumInstructions
    {
        CONST_VTBL struct IEnumInstructionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumInstructions_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumInstructions_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumInstructions_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumInstructions_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumInstructions_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumInstructions_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumInstructions_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumInstructions_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumInstructions_INTERFACE_DEFINED__ */


#ifndef __IInstructionFactory_INTERFACE_DEFINED__
#define __IInstructionFactory_INTERFACE_DEFINED__

/* interface IInstructionFactory */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IInstructionFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CF059876-C5CA-4EBF-ACB9-9C58009CE31A")
    IInstructionFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateByteOperandInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ BYTE operand,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateUShortOperandInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ USHORT operand,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateIntOperandInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ INT32 operand,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLongOperandInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ INT64 operand,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateFloatOperandInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ float operand,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDoubleOperandInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ double operand,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateTokenOperandInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ mdToken operand,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBranchInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ IInstruction *pBranchTarget,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateSwitchInstruction( 
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ DWORD cBranchTargets,
            /* [length_is][size_is][in] */ IInstruction **ppBranchTargets,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLoadConstInstruction( 
            /* [in] */ int value,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLoadLocalInstruction( 
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLoadLocalAddressInstruction( 
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateStoreLocalInstruction( 
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLoadArgInstruction( 
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLoadArgAddressInstruction( 
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DecodeInstructionByteStream( 
            /* [in] */ DWORD cbMethod,
            /* [in] */ LPCBYTE instructionBytes,
            /* [out] */ IInstructionGraph **ppInstructionGraph) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInstructionFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInstructionFactory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInstructionFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInstructionFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateByteOperandInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ BYTE operand,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateUShortOperandInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ USHORT operand,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateIntOperandInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ INT32 operand,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLongOperandInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ INT64 operand,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateFloatOperandInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ float operand,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDoubleOperandInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ double operand,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateTokenOperandInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ mdToken operand,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBranchInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ IInstruction *pBranchTarget,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateSwitchInstruction )( 
            IInstructionFactory * This,
            /* [in] */ enum ILOrdinalOpcode opcode,
            /* [in] */ DWORD cBranchTargets,
            /* [length_is][size_is][in] */ IInstruction **ppBranchTargets,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLoadConstInstruction )( 
            IInstructionFactory * This,
            /* [in] */ int value,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLoadLocalInstruction )( 
            IInstructionFactory * This,
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLoadLocalAddressInstruction )( 
            IInstructionFactory * This,
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateStoreLocalInstruction )( 
            IInstructionFactory * This,
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLoadArgInstruction )( 
            IInstructionFactory * This,
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLoadArgAddressInstruction )( 
            IInstructionFactory * This,
            /* [in] */ USHORT index,
            /* [out] */ IInstruction **ppInstruction);
        
        HRESULT ( STDMETHODCALLTYPE *DecodeInstructionByteStream )( 
            IInstructionFactory * This,
            /* [in] */ DWORD cbMethod,
            /* [in] */ LPCBYTE instructionBytes,
            /* [out] */ IInstructionGraph **ppInstructionGraph);
        
        END_INTERFACE
    } IInstructionFactoryVtbl;

    interface IInstructionFactory
    {
        CONST_VTBL struct IInstructionFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInstructionFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInstructionFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInstructionFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInstructionFactory_CreateInstruction(This,opcode,ppInstruction)	\
    ( (This)->lpVtbl -> CreateInstruction(This,opcode,ppInstruction) ) 

#define IInstructionFactory_CreateByteOperandInstruction(This,opcode,operand,ppInstruction)	\
    ( (This)->lpVtbl -> CreateByteOperandInstruction(This,opcode,operand,ppInstruction) ) 

#define IInstructionFactory_CreateUShortOperandInstruction(This,opcode,operand,ppInstruction)	\
    ( (This)->lpVtbl -> CreateUShortOperandInstruction(This,opcode,operand,ppInstruction) ) 

#define IInstructionFactory_CreateIntOperandInstruction(This,opcode,operand,ppInstruction)	\
    ( (This)->lpVtbl -> CreateIntOperandInstruction(This,opcode,operand,ppInstruction) ) 

#define IInstructionFactory_CreateLongOperandInstruction(This,opcode,operand,ppInstruction)	\
    ( (This)->lpVtbl -> CreateLongOperandInstruction(This,opcode,operand,ppInstruction) ) 

#define IInstructionFactory_CreateFloatOperandInstruction(This,opcode,operand,ppInstruction)	\
    ( (This)->lpVtbl -> CreateFloatOperandInstruction(This,opcode,operand,ppInstruction) ) 

#define IInstructionFactory_CreateDoubleOperandInstruction(This,opcode,operand,ppInstruction)	\
    ( (This)->lpVtbl -> CreateDoubleOperandInstruction(This,opcode,operand,ppInstruction) ) 

#define IInstructionFactory_CreateTokenOperandInstruction(This,opcode,operand,ppInstruction)	\
    ( (This)->lpVtbl -> CreateTokenOperandInstruction(This,opcode,operand,ppInstruction) ) 

#define IInstructionFactory_CreateBranchInstruction(This,opcode,pBranchTarget,ppInstruction)	\
    ( (This)->lpVtbl -> CreateBranchInstruction(This,opcode,pBranchTarget,ppInstruction) ) 

#define IInstructionFactory_CreateSwitchInstruction(This,opcode,cBranchTargets,ppBranchTargets,ppInstruction)	\
    ( (This)->lpVtbl -> CreateSwitchInstruction(This,opcode,cBranchTargets,ppBranchTargets,ppInstruction) ) 

#define IInstructionFactory_CreateLoadConstInstruction(This,value,ppInstruction)	\
    ( (This)->lpVtbl -> CreateLoadConstInstruction(This,value,ppInstruction) ) 

#define IInstructionFactory_CreateLoadLocalInstruction(This,index,ppInstruction)	\
    ( (This)->lpVtbl -> CreateLoadLocalInstruction(This,index,ppInstruction) ) 

#define IInstructionFactory_CreateLoadLocalAddressInstruction(This,index,ppInstruction)	\
    ( (This)->lpVtbl -> CreateLoadLocalAddressInstruction(This,index,ppInstruction) ) 

#define IInstructionFactory_CreateStoreLocalInstruction(This,index,ppInstruction)	\
    ( (This)->lpVtbl -> CreateStoreLocalInstruction(This,index,ppInstruction) ) 

#define IInstructionFactory_CreateLoadArgInstruction(This,index,ppInstruction)	\
    ( (This)->lpVtbl -> CreateLoadArgInstruction(This,index,ppInstruction) ) 

#define IInstructionFactory_CreateLoadArgAddressInstruction(This,index,ppInstruction)	\
    ( (This)->lpVtbl -> CreateLoadArgAddressInstruction(This,index,ppInstruction) ) 

#define IInstructionFactory_DecodeInstructionByteStream(This,cbMethod,instructionBytes,ppInstructionGraph)	\
    ( (This)->lpVtbl -> DecodeInstructionByteStream(This,cbMethod,instructionBytes,ppInstructionGraph) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInstructionFactory_INTERFACE_DEFINED__ */


#ifndef __IEnumAppMethodInfo_INTERFACE_DEFINED__
#define __IEnumAppMethodInfo_INTERFACE_DEFINED__

/* interface IEnumAppMethodInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumAppMethodInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("541A45B7-D194-47EE-9231-AB69D27D1D59")
    IEnumAppMethodInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IEnumAppMethodInfo **rgelt,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumAppMethodInfo **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumAppMethodInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumAppMethodInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumAppMethodInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumAppMethodInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumAppMethodInfo * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IEnumAppMethodInfo **rgelt,
            /* [in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumAppMethodInfo * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumAppMethodInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumAppMethodInfo * This,
            /* [out] */ IEnumAppMethodInfo **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumAppMethodInfo * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumAppMethodInfoVtbl;

    interface IEnumAppMethodInfo
    {
        CONST_VTBL struct IEnumAppMethodInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumAppMethodInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumAppMethodInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumAppMethodInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumAppMethodInfo_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumAppMethodInfo_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumAppMethodInfo_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumAppMethodInfo_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumAppMethodInfo_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumAppMethodInfo_INTERFACE_DEFINED__ */


#ifndef __ILocalVariableCollection2_INTERFACE_DEFINED__
#define __ILocalVariableCollection2_INTERFACE_DEFINED__

/* interface ILocalVariableCollection2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ILocalVariableCollection2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("61657FE7-BFBB-4B60-BBA7-1D3C326FA470")
    ILocalVariableCollection2 : public ILocalVariableCollection
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetEnum( 
            /* [out] */ IEnumMethodLocals **ppEnumMethodLocals) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ILocalVariableCollection2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILocalVariableCollection2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILocalVariableCollection2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILocalVariableCollection2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            ILocalVariableCollection2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorSignature )( 
            ILocalVariableCollection2 * This,
            /* [out] */ ISignatureBuilder **ppSignature);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            ILocalVariableCollection2 * This,
            /* [out] */ DWORD *pdwCount);
        
        HRESULT ( STDMETHODCALLTYPE *AddLocal )( 
            ILocalVariableCollection2 * This,
            /* [in] */ IType *pType,
            /* [optional][out] */ DWORD *pIndex);
        
        HRESULT ( STDMETHODCALLTYPE *ReplaceSignature )( 
            ILocalVariableCollection2 * This,
            /* [in] */ const BYTE *pSignature,
            /* [in] */ DWORD dwSigSize);
        
        HRESULT ( STDMETHODCALLTYPE *CommitSignature )( 
            ILocalVariableCollection2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetEnum )( 
            ILocalVariableCollection2 * This,
            /* [out] */ IEnumMethodLocals **ppEnumMethodLocals);
        
        END_INTERFACE
    } ILocalVariableCollection2Vtbl;

    interface ILocalVariableCollection2
    {
        CONST_VTBL struct ILocalVariableCollection2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILocalVariableCollection2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILocalVariableCollection2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILocalVariableCollection2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILocalVariableCollection2_Initialize(This)	\
    ( (This)->lpVtbl -> Initialize(This) ) 

#define ILocalVariableCollection2_GetCorSignature(This,ppSignature)	\
    ( (This)->lpVtbl -> GetCorSignature(This,ppSignature) ) 

#define ILocalVariableCollection2_GetCount(This,pdwCount)	\
    ( (This)->lpVtbl -> GetCount(This,pdwCount) ) 

#define ILocalVariableCollection2_AddLocal(This,pType,pIndex)	\
    ( (This)->lpVtbl -> AddLocal(This,pType,pIndex) ) 

#define ILocalVariableCollection2_ReplaceSignature(This,pSignature,dwSigSize)	\
    ( (This)->lpVtbl -> ReplaceSignature(This,pSignature,dwSigSize) ) 

#define ILocalVariableCollection2_CommitSignature(This)	\
    ( (This)->lpVtbl -> CommitSignature(This) ) 


#define ILocalVariableCollection2_GetEnum(This,ppEnumMethodLocals)	\
    ( (This)->lpVtbl -> GetEnum(This,ppEnumMethodLocals) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILocalVariableCollection2_INTERFACE_DEFINED__ */


#ifndef __IEnumTypes_INTERFACE_DEFINED__
#define __IEnumTypes_INTERFACE_DEFINED__

/* interface IEnumTypes */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumTypes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5618BD13-12FC-4198-A39D-8ED60265AAC6")
    IEnumTypes : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IType **rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumTypes **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ DWORD *pLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumTypesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumTypes * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumTypes * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumTypes * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumTypes * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IType **rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumTypes * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumTypes * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumTypes * This,
            /* [out] */ IEnumTypes **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumTypes * This,
            /* [out] */ DWORD *pLength);
        
        END_INTERFACE
    } IEnumTypesVtbl;

    interface IEnumTypes
    {
        CONST_VTBL struct IEnumTypesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumTypes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumTypes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumTypes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumTypes_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumTypes_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumTypes_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumTypes_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#define IEnumTypes_GetCount(This,pLength)	\
    ( (This)->lpVtbl -> GetCount(This,pLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumTypes_INTERFACE_DEFINED__ */


#ifndef __ISignatureParser_INTERFACE_DEFINED__
#define __ISignatureParser_INTERFACE_DEFINED__

/* interface ISignatureParser */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ISignatureParser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33BD020E-372B-40F9-A735-4B4017ED56AC")
    ISignatureParser : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParseMethodSignature( 
            /* [in] */ const BYTE *pSignature,
            /* [in] */ ULONG cbSignature,
            /* [optional][out] */ ULONG *pCallingConvention,
            /* [optional][out] */ IType **ppReturnType,
            /* [optional][out] */ IEnumTypes **ppEnumParameterTypes,
            /* [optional][out] */ ULONG *pcGenericTypeParameters,
            /* [optional][out] */ ULONG *pcbRead) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseLocalVarSignature( 
            /* [in] */ const BYTE *pSignature,
            /* [in] */ ULONG cbSignature,
            /* [optional][out] */ ULONG *pCallingConvention,
            /* [optional][out] */ IEnumTypes **ppEnumTypes,
            /* [optional][out] */ ULONG *pcbRead) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseTypeSequence( 
            /* [in] */ const BYTE *pBuffer,
            /* [in] */ ULONG cbBuffer,
            /* [in] */ ULONG cTypes,
            /* [optional][out] */ IEnumTypes **ppEnumTypes,
            /* [optional][out] */ ULONG *pcbRead) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISignatureParserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISignatureParser * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISignatureParser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISignatureParser * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseMethodSignature )( 
            ISignatureParser * This,
            /* [in] */ const BYTE *pSignature,
            /* [in] */ ULONG cbSignature,
            /* [optional][out] */ ULONG *pCallingConvention,
            /* [optional][out] */ IType **ppReturnType,
            /* [optional][out] */ IEnumTypes **ppEnumParameterTypes,
            /* [optional][out] */ ULONG *pcGenericTypeParameters,
            /* [optional][out] */ ULONG *pcbRead);
        
        HRESULT ( STDMETHODCALLTYPE *ParseLocalVarSignature )( 
            ISignatureParser * This,
            /* [in] */ const BYTE *pSignature,
            /* [in] */ ULONG cbSignature,
            /* [optional][out] */ ULONG *pCallingConvention,
            /* [optional][out] */ IEnumTypes **ppEnumTypes,
            /* [optional][out] */ ULONG *pcbRead);
        
        HRESULT ( STDMETHODCALLTYPE *ParseTypeSequence )( 
            ISignatureParser * This,
            /* [in] */ const BYTE *pBuffer,
            /* [in] */ ULONG cbBuffer,
            /* [in] */ ULONG cTypes,
            /* [optional][out] */ IEnumTypes **ppEnumTypes,
            /* [optional][out] */ ULONG *pcbRead);
        
        END_INTERFACE
    } ISignatureParserVtbl;

    interface ISignatureParser
    {
        CONST_VTBL struct ISignatureParserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISignatureParser_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISignatureParser_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISignatureParser_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISignatureParser_ParseMethodSignature(This,pSignature,cbSignature,pCallingConvention,ppReturnType,ppEnumParameterTypes,pcGenericTypeParameters,pcbRead)	\
    ( (This)->lpVtbl -> ParseMethodSignature(This,pSignature,cbSignature,pCallingConvention,ppReturnType,ppEnumParameterTypes,pcGenericTypeParameters,pcbRead) ) 

#define ISignatureParser_ParseLocalVarSignature(This,pSignature,cbSignature,pCallingConvention,ppEnumTypes,pcbRead)	\
    ( (This)->lpVtbl -> ParseLocalVarSignature(This,pSignature,cbSignature,pCallingConvention,ppEnumTypes,pcbRead) ) 

#define ISignatureParser_ParseTypeSequence(This,pBuffer,cbBuffer,cTypes,ppEnumTypes,pcbRead)	\
    ( (This)->lpVtbl -> ParseTypeSequence(This,pBuffer,cbBuffer,cTypes,ppEnumTypes,pcbRead) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISignatureParser_INTERFACE_DEFINED__ */


#ifndef __ITokenType_INTERFACE_DEFINED__
#define __ITokenType_INTERFACE_DEFINED__

/* interface ITokenType */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITokenType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77655B33-1B29-4285-9F2D-FF9526E3A0AA")
    ITokenType : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetToken( 
            /* [out] */ mdToken *pToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOwningModule( 
            /* [out] */ IModuleInfo **ppModuleInfo) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITokenTypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITokenType * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITokenType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITokenType * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetToken )( 
            ITokenType * This,
            /* [out] */ mdToken *pToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetOwningModule )( 
            ITokenType * This,
            /* [out] */ IModuleInfo **ppModuleInfo);
        
        END_INTERFACE
    } ITokenTypeVtbl;

    interface ITokenType
    {
        CONST_VTBL struct ITokenTypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITokenType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITokenType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITokenType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITokenType_GetToken(This,pToken)	\
    ( (This)->lpVtbl -> GetToken(This,pToken) ) 

#define ITokenType_GetOwningModule(This,ppModuleInfo)	\
    ( (This)->lpVtbl -> GetOwningModule(This,ppModuleInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITokenType_INTERFACE_DEFINED__ */


#ifndef __ICompositeType_INTERFACE_DEFINED__
#define __ICompositeType_INTERFACE_DEFINED__

/* interface ICompositeType */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ICompositeType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("06B9FD79-0386-4CF3-93DD-A23E95EBC225")
    ICompositeType : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRelatedType( 
            /* [out] */ IType **ppType) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICompositeTypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompositeType * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompositeType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompositeType * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetRelatedType )( 
            ICompositeType * This,
            /* [out] */ IType **ppType);
        
        END_INTERFACE
    } ICompositeTypeVtbl;

    interface ICompositeType
    {
        CONST_VTBL struct ICompositeTypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompositeType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompositeType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompositeType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompositeType_GetRelatedType(This,ppType)	\
    ( (This)->lpVtbl -> GetRelatedType(This,ppType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompositeType_INTERFACE_DEFINED__ */


#ifndef __IGenericParameterType_INTERFACE_DEFINED__
#define __IGenericParameterType_INTERFACE_DEFINED__

/* interface IGenericParameterType */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IGenericParameterType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1D5C1393-DC7E-4FEF-8A9D-A3DAF7A55C6E")
    IGenericParameterType : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPosition( 
            /* [out] */ ULONG *pPosition) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IGenericParameterTypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGenericParameterType * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGenericParameterType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGenericParameterType * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPosition )( 
            IGenericParameterType * This,
            /* [out] */ ULONG *pPosition);
        
        END_INTERFACE
    } IGenericParameterTypeVtbl;

    interface IGenericParameterType
    {
        CONST_VTBL struct IGenericParameterTypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGenericParameterType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGenericParameterType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGenericParameterType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGenericParameterType_GetPosition(This,pPosition)	\
    ( (This)->lpVtbl -> GetPosition(This,pPosition) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGenericParameterType_INTERFACE_DEFINED__ */


#ifndef __ISingleRetDefaultInstrumentation2_INTERFACE_DEFINED__
#define __ISingleRetDefaultInstrumentation2_INTERFACE_DEFINED__

/* interface ISingleRetDefaultInstrumentation2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ISingleRetDefaultInstrumentation2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7A88FF19-F3A1-4C43-89DB-61DF376441B5")
    ISingleRetDefaultInstrumentation2 : public ISingleRetDefaultInstrumentation
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBranchTargetInstruction( 
            /* [out] */ IInstruction **pInstruction) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISingleRetDefaultInstrumentation2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISingleRetDefaultInstrumentation2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISingleRetDefaultInstrumentation2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISingleRetDefaultInstrumentation2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            ISingleRetDefaultInstrumentation2 * This,
            /* [in] */ IInstructionGraph *pInstructionGraph);
        
        HRESULT ( STDMETHODCALLTYPE *ApplySingleRetDefaultInstrumentation )( 
            ISingleRetDefaultInstrumentation2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBranchTargetInstruction )( 
            ISingleRetDefaultInstrumentation2 * This,
            /* [out] */ IInstruction **pInstruction);
        
        END_INTERFACE
    } ISingleRetDefaultInstrumentation2Vtbl;

    interface ISingleRetDefaultInstrumentation2
    {
        CONST_VTBL struct ISingleRetDefaultInstrumentation2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISingleRetDefaultInstrumentation2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISingleRetDefaultInstrumentation2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISingleRetDefaultInstrumentation2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISingleRetDefaultInstrumentation2_Initialize(This,pInstructionGraph)	\
    ( (This)->lpVtbl -> Initialize(This,pInstructionGraph) ) 

#define ISingleRetDefaultInstrumentation2_ApplySingleRetDefaultInstrumentation(This)	\
    ( (This)->lpVtbl -> ApplySingleRetDefaultInstrumentation(This) ) 


#define ISingleRetDefaultInstrumentation2_GetBranchTargetInstruction(This,pInstruction)	\
    ( (This)->lpVtbl -> GetBranchTargetInstruction(This,pInstruction) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISingleRetDefaultInstrumentation2_INTERFACE_DEFINED__ */


#ifndef __IInstrumentationMethodJitEvents_INTERFACE_DEFINED__
#define __IInstrumentationMethodJitEvents_INTERFACE_DEFINED__

/* interface IInstrumentationMethodJitEvents */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IInstrumentationMethodJitEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9B028F9E-E2E0-4A61-862B-A4E1158657D0")
    IInstrumentationMethodJitEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE JitStarted( 
            /* [in] */ FunctionID functionID,
            /* [in] */ BOOL isRejit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE JitComplete( 
            /* [in] */ FunctionID functionID,
            /* [in] */ BOOL isRejit,
            /* [in] */ HRESULT jitResult) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IInstrumentationMethodJitEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInstrumentationMethodJitEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInstrumentationMethodJitEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInstrumentationMethodJitEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *JitStarted )( 
            IInstrumentationMethodJitEvents * This,
            /* [in] */ FunctionID functionID,
            /* [in] */ BOOL isRejit);
        
        HRESULT ( STDMETHODCALLTYPE *JitComplete )( 
            IInstrumentationMethodJitEvents * This,
            /* [in] */ FunctionID functionID,
            /* [in] */ BOOL isRejit,
            /* [in] */ HRESULT jitResult);
        
        END_INTERFACE
    } IInstrumentationMethodJitEventsVtbl;

    interface IInstrumentationMethodJitEvents
    {
        CONST_VTBL struct IInstrumentationMethodJitEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInstrumentationMethodJitEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInstrumentationMethodJitEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInstrumentationMethodJitEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInstrumentationMethodJitEvents_JitStarted(This,functionID,isRejit)	\
    ( (This)->lpVtbl -> JitStarted(This,functionID,isRejit) ) 

#define IInstrumentationMethodJitEvents_JitComplete(This,functionID,isRejit,jitResult)	\
    ( (This)->lpVtbl -> JitComplete(This,functionID,isRejit,jitResult) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInstrumentationMethodJitEvents_INTERFACE_DEFINED__ */

#endif /* __MicrosoftInstrumentationEngine_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


