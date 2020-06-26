/////////////////////////////////
//
// Library   : Analyzer/Assembly
// Unit      : Instructions
// File      : DecoderInterface.h
// Author    : Franck Vedrine
// Copyright : CEA LIST - 2013-2020
//
// Description :
//   Definition of a generic C Interface to make the connection
//   between the binary static analysis and the interpretation
//   of the instructions.
//

#pragma once

#include "Analyzer/Assembly/DynamicInterpreters/Interface/InstructionCallBack.h"

#if defined(_WIN32) && defined(_USRDLL)
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif // APE_EXPORTS
#else
#define DLL_API
#endif // _USRDLL

#ifdef _MSC_VER
#pragma pack(push,4)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DefineStartGenericFunctions 
#define DefineEndGenericFunctions 
#define DefineDecoderPrefix(x) decoder_##x
#include "Analyzer/Assembly/DynamicInterpreters/Interface/DecoderInterface.inch"
#undef DefineDecoderPrefix
#undef DefineStartGenericFunctions
#undef DefineEndGenericFunctions

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif

