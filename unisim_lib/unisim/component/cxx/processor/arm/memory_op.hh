/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr),
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_MEMORY_OP_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_MEMORY_OP_HH__

#include <vector>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  class MemoryOp {
  public:
    typedef enum {
      READ,
      USER_READ,
      WRITE,
      PREFETCH
    } type_t;
	
    MemoryOp();
    ~MemoryOp();
	
    void SetRead(uint32_t address, uint32_t size, bool read_signed);
    void SetWrite(uint32_t address, uint32_t size, uint32_t value);
    void SetPrefetch(uint32_t address);
    void SetDestReg(uint32_t dst) { target_reg = dst; }
    void SetDestUserReg(uint32_t dst) { target_reg = dst; type = USER_READ; }
    type_t GetType() const { return type; }
    uint32_t GetAddress() const { return address; }
    uint32_t GetSize() const { return size; }
    uint32_t GetTargetReg() const { return target_reg; }
    uint32_t GetWriteValue() const { return write_value; }
    bool IsSigned() const { return read_signed; }
	
    static MemoryOp* alloc() {
      if (freepool.empty())
        return new MemoryOp();
      MemoryOp* res = freepool.back();
      freepool.pop_back();
      return res;
    }
    static void release( MemoryOp* memop ) {
      freepool.push_back( memop );
    }
    
  private:
    uint32_t  address;
    type_t    type;
    uint32_t  size;
    uint32_t  target_reg;
    uint32_t  write_value;
    bool      read_signed;
    
    static std::vector<MemoryOp*> freepool;
  };

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_MEMORY_OP_HH__
