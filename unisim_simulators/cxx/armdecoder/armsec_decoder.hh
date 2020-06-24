/////////////////////////////////
//
// Library   : Static Analysis
// Unit      : armsec
// File      : armsec_decoder.h
// Copyright : CEA LIST - 2020
//
// Description :
//   Definition of a generic C Interface to manipulate abstract domains
//   = interval, disjunctions, ...
//

#pragma once

#include "memsec_callback.hh"

#if defined(_WIN32)
#define DLL_CDECL __cdecl
#else
#define DLL_CDECL 
#endif

#if defined(_WIN32) && defined(_USRDLL)
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif // DLL_EXPORTS
#else
#define DLL_API
#endif // _USRDLL

#ifdef _MSC_VER
#pragma pack(push,4)
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_MSC_VER) && !defined(__cplusplus)
#define inline __inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SAL_VERSION

#define _In_ 
#define _Out_ 
#define _Inout_ 
#define _In_opt_ 

#endif

typedef struct _InterpretParameters InterpretParameters;
typedef struct _MemoryModel MemoryModel;
typedef struct _MemoryModelFunctions MemoryModelFunctions;
struct _Processor;

typedef struct _TargetAddresses {
   uint64_t* addresses;
   int addresses_array_size;
   int addresses_length;

   uint64_t* (*realloc_addresses)(uint64_t* old_addresses, int old_size,
         int* new_size, void* address_container);
   void* address_container;
} TargetAddresses;

struct _DecisionVector;

DLL_API struct _Processor* create_processor();
DLL_API void set_domain_functions(struct _Processor* processor, struct _DomainElementFunctions* functionTable);
DLL_API struct _DomainElementFunctions* get_domain_functions(struct _Processor* aprocessor);
DLL_API void initialize_memory(struct _Processor* processor, MemoryModel* memory,
      MemoryModelFunctions* memory_functions, InterpretParameters* parameters);
DLL_API void processor_set_verbose(struct _Processor* processor);
DLL_API void free_processor(struct _Processor* processor);

DLL_API int processor_get_registers_number(struct _Processor* processor);
DLL_API int processor_get_register_index(struct _Processor* processor,
      const char* name);
DLL_API const char* processor_get_register_name(struct _Processor* processor,
      int register_index);
DLL_API struct _DecisionVector* processor_create_decision_vector(struct _Processor* processor);
DLL_API struct _DecisionVector* processor_clone_decision_vector(struct _DecisionVector* decision_vector);
DLL_API void processor_filter_decision_vector(struct _DecisionVector* decision_vector, uint64_t target);
DLL_API void processor_free_decision_vector(struct _DecisionVector* decision_vector);
DLL_API bool processor_next_targets(struct _Processor* processor, char* instruction_buffer,
      size_t buffer_size, uint64_t address, TargetAddresses* target_addresses,
      MemoryModel* memory, MemoryModelFunctions* memory_functions,
      struct _DecisionVector* decision_vector, InterpretParameters* parameters);

DLL_API bool processor_interpret(struct _Processor* processor, char* instruction_buffer,
      size_t buffer_size, uint64_t* address, uint64_t target_address,
      MemoryModel* memory, MemoryModelFunctions* memory_functions,
      struct _DecisionVector* decision_vector, InterpretParameters* parameters);

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif


