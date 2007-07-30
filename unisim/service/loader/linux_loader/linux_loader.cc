/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
 
/* This class has become templated so I will remove the code from here.... */
#if 0
#include <endian.h>
#include "plugins/loader/linux_loader.hh"
#include <string.h>
#include <iostream>

using namespace std;
using namespace full_system::plugins::loader;

template<typename T>
LinuxLoader::LinuxLoader(MemoryInterface<physical_address_t> *memory_interface,
			 ElfLoaderInterface<physical_address_t> *loader_interface,
			 endian_type _endianess,
			 physical_address_t _stack_base,
			 physical_address_t _max_environ) :
  loader(loader_interface), 
  mem(memory_interface), 
  stack_base(stack_base), 
  max_environ(_max_environ),
  endianness(_endianness) { }

LinuxLoader::~LinuxLoader() { }
  
template<typename T>
bool 
LinuxLoader::LoadProgram(const string& filename, int argc, char *argv[], char *envp[],
			      physical_address_t &entry_point, 
			      physical_address_t &stack_address,
			      physical_address_t &top_addr,
			      unsigned int &num_args,
			      physical_address_t &arg_address,
			      physical_address_t &data_base,
			      physical_address_t &data_size) {
  union {
    uint32_t val;
    uint8_t val8[4];
  } mem_val;

  /* use the elf loader to load the binary and set the entry point */
  loader->LoadProgram(filename, entry_point, top_addr, data_base, data_size);
  
  physical_address_t stack_ptr;
  stack_ptr = stack_base - max_environ;
  
  /*write argc to stack*/
  mem_val.val = argc;
  if(endianess == E_BIG_ENDIAN) {
#if BYTE_ORDER == BIG_ENDIAN
    /* nothing to do */
#else
    uint8_t swap;
    swap = mem_val.val8[0];
    mem_val.val8[0] = mem_val.val8[3];
    mem_val.val8[3] = swap;
    swap = mem_val.val8[1];
    mem_val.val8[1] = mem_val.val8[2];
    mem_val.val8[2] = swap;
#endif
  } else {
#if BYTE_ORDER == BIG_ENDIAN
    uint8_t swap;
    swap = mem_val.val8[0];
    mem_val.val8[0] = mem_val.val8[3];
    mem_val.val8[3] = swap;
    swap = mem_val.val8[1];
    mem_val.val8[1] = mem_val.val8[2];
    mem_val.val8[2] = swap;
#else
    /* nothing to do */
#endif
  }
  mem->WriteMemory(stack_ptr, mem_val.val8, 4);

  num_args = argc;
  stack_ptr += 4;
  
  /*skip stack_ptr past argv pointer array*/
  arg_address = stack_ptr;
  stack_ptr += (argc + 1) * 4;

  /*skip env pointer array*/
  physical_address_t env_address = stack_ptr;
  for(unsigned int i = 0; envp[i]; i++)
    stack_ptr += 4;
  stack_ptr += 4;
  
  /*write argv to stack*/
  for(unsigned int i = 0; i < argc; i++) {
    mem_val.val = stack_ptr;
    if(endianess == E_BIG_ENDIAN) {
#if BYTE_ORDER == BIG_ENDIAN
      /* nothing to do */
#else
      uint8_t swap;
      swap = mem_val.val8[0];
      mem_val.val8[0] = mem_val.val8[3];
      mem_val.val8[3] = swap;
      swap = mem_val.val8[1];
      mem_val.val8[1] = mem_val.val8[2];
      mem_val.val8[2] = swap;
#endif
    } else {
#if BYTE_ORDER == BIG_ENDIAN
      uint8_t swap;
      swap = mem_val.val8[0];
      mem_val.val8[0] = mem_val.val8[3];
      mem_val.val8[3] = swap;
      swap = mem_val.val8[1];
      mem_val.val8[1] = mem_val.val8[2];
      mem_val.val8[2] = swap;
#else
      /* nothing to do */
#endif
    }
    mem->WriteMemory(arg_address + i * 4, mem_val.val8, 4);
    mem->WriteMemory(stack_ptr, argv[i], strlen(argv[i]));
    stack_ptr += strlen(argv[i]) + 1;
  }
    
  /*write env to stack*/
  for(unsigned int i = 0; envp[i]; i++) {
    mem_val.val = stack_ptr;
    if(endianess == E_BIG_ENDIAN) {
#if BYTE_ORDER == BIG_ENDIAN
      /* nothing to do */
#else
      uint8_t swap;
      swap = mem_val.val8[0];
      mem_val.val8[0] = mem_val.val8[3];
      mem_val.val8[3] = swap;
      swap = mem_val.val8[1];
      mem_val.val8[1] = mem_val.val8[2];
      mem_val.val8[2] = swap;
#endif
    } else {
#if BYTE_ORDER == BIG_ENDIAN
      uint8_t swap;
      swap = mem_val.val8[0];
      mem_val.val8[0] = mem_val.val8[3];
      mem_val.val8[3] = swap;
      swap = mem_val.val8[1];
      mem_val.val8[1] = mem_val.val8[2];
      mem_val.val8[2] = swap;
#else
      /* nothing to do */
#endif
    }
    mem->WriteMemory(env_address + i * 4, mem_val.val8, 4);
    mem->WriteMemory(stack_ptr, envp[i], strlen(envp[i]));
    stack_ptr += strlen(envp[i]) + 1;
  }
    
  /*stack overflow*/
  if(stack_ptr + 4 >= stack_base) {
    cerr << "Environment overflow. Need to increase max_environ." << endl;;
    exit(1);
  }
  
  stack_address = arg_address - 4;

}
#endif
