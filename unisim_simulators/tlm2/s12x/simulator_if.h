#ifndef _SIMULATOR_IF_H_
#define _SIMULATOR_IF_H_

#ifdef uint64_to_double
#undef uint64_to_double
#endif

#define EXPORT_FCNS

#if defined(_WIN32) || defined(_WIN64)

#ifdef EXPORT_FCNS
#define EXPORTED_FUNCTION __declspec(dllexport)
#else
#define EXPORTED_FUNCTION __declspec(dllimport)
#endif

#define DLL_PUBLIC
#define DLL_LOCAL

#else

#define EXPORTED_FUNCTION
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif

#endif


#ifdef __cplusplus
#include <simulator.hh>
#endif

#ifdef __cplusplus
extern "C" {
#endif

EXPORTED_FUNCTION  void   createObject(int argc, char **argv) DLL_PUBLIC;
EXPORTED_FUNCTION  int computeWrapper(int inVal) DLL_PUBLIC;
EXPORTED_FUNCTION  void   deleteObject() DLL_PUBLIC;

#ifdef __cplusplus
}
#endif

#endif
