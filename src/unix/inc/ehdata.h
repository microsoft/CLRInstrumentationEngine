// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

// TODO: Remove
typedef	__int32	PMFN;					// Image relative offset of Member Function


#if defined(_WIN64)
#define _EH_PTR64 __ptr64
#else
#define _EH_PTR64
#endif

#ifndef WANT_NO_TYPES
typedef struct PMD
{
#if defined(_WIN64)	 /*IFSTRIP=IGN*/
	__int32		mdisp;		// Offset of intended data within base
	__int32		pdisp;		// Displacement to virtual base pointer
	__int32		vdisp;		// Index within vbTable to offset of base
#else
	ptrdiff_t	mdisp;		// Offset of intended data within base
	ptrdiff_t	pdisp;		// Displacement to virtual base pointer
	ptrdiff_t	vdisp;		// Index within vbTable to offset of base
#endif
	} PMD;

typedef struct TypeDescriptor
{
#if defined(_WIN64) || defined(_RTTI) /*IFSTRIP=IGN*/
	const void * _EH_PTR64	pVFTable;	// Field overloaded by RTTI
#else
	DWORD	hash;			// Hash value computed from type's decorated name
#endif
	void * _EH_PTR64			spare;		// reserved, possible for RTTI
	char	name[];			// The decorated name of the type; 0 terminated.
	} TypeDescriptor;

typedef const struct _s_CatchableType {
	unsigned int	properties;				// Catchable Type properties (Bit field)
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			pType;					// Image relative offset of TypeDescriptor
#else
	TypeDescriptor * _EH_PTR64 pType;		// Pointer to the type descriptor for this type
#endif
	PMD 			thisDisplacement;		// Pointer to instance of catch type within
											//		thrown object.
	int				sizeOrOffset;			// Size of simple-type object or offset into
											//  buffer of 'this' pointer for catch object
	PMFN			copyFunction;			// Copy constructor or CC-closure
} CatchableType;
typedef const struct _s_CatchableTypeArray {
	int	nCatchableTypes;
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			arrayOfCatchableTypes[];	// Image relative offset of Catchable Types
#else
	CatchableType	* _EH_PTR64 arrayOfCatchableTypes[];
#endif
	} CatchableTypeArray;
#endif // WANT_NO_TYPES

typedef const struct _s_ThrowInfo {
	unsigned int	attributes;			// Throw Info attributes (Bit field)
	PMFN			pmfnUnwind;			// Destructor to call when exception
										// has been handled or aborted.
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			pForwardCompat;		// Image relative offset of Forward compatibility frame handler
	__int32			pCatchableTypeArray;// Image relative offset of CatchableTypeArray
#else
#if defined(__cplusplus)
	int	(__cdecl* _EH_PTR64 pForwardCompat)(...);	// Forward compatibility frame handler
#else
	int	(__cdecl* _EH_PTR64 pForwardCompat)();	// Forward compatibility frame handler
#endif
	CatchableTypeArray	* _EH_PTR64 pCatchableTypeArray;	// Pointer to list of pointers to types.
#endif
} ThrowInfo;
