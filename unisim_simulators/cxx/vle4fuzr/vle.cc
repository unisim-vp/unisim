/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *  
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "vle.hh"
#include "top_vle.hh"
#include <unisim/util/endian/endian.hh>
#include <iostream>

namespace vle {
namespace concrete {
  
  Processor::Processor()
  {     
    for (unsigned reg = 0; reg < 32; ++reg)
      reg_values[reg] = U32();
  }

  void
  Processor::ProcessException( char const* msg )
  {
    std::cerr << "exception(" << msg << ")\n";
    throw TODO();
  }

  Processor::RegView const*
  Processor::get_reg(char const* id, uintptr_t size)
  {
    if (regname("gpr",id,size))
      {
        static struct : public RegView
        {
          void write( EmuProcessor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetGPR(id, unisim::util::endian::ByteSwap(*(uint32_t*)bytes)); }
          void read( EmuProcessor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetGPR(id); }
        } _;
        return &_;
      }

    std::cerr << "Register '" << std::string(id,size) << "' not found.\n";
    return 0;
  }

  int
  Processor::emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count )
  {
    if (timeout)
      {
        std::cerr << "Error: timeout unimplemented." << timeout << std::endl;
        throw 0;
      }
    
    // std::cerr << "until: " << until << std::endl;
    // std::cerr << "count: " << count << std::endl;
    
    this->Branch(begin);
    this->bblock = true;
  
    while (this->nia != until)
      {
        /* go to the next instruction */
        this->cia = this->nia;
        
        uint32_t insn_addr = this->cia = this->nia, insn_length = 0;
        CodeType insn;
        insn = this->Fetch(insn_addr);

        uint32_t insn_idx = insn_addr/2,
          insn_tag = insn_idx / Processor::OPPAGESIZE, insn_offset = insn_idx % Processor::OPPAGESIZE;
        
        InsnPage& page = insn_cache[insn_tag];
        Operation* op = page.ops[insn_offset];
        if (op and op->GetEncoding() == insn)
          { insn_length = op->GetLength() / 8; }
        else
          {
            delete op;
            static Decoder decoder;
            decoder.NCDecode(insn_addr, insn);
          }
        
        // Monitor
        if (unlikely(this->disasm))
          {
            op->disasm(this, std::cerr << std::hex << insn_addr << ": " );
            std::cerr << std::endl;
          }

        insn_hooks(insn_addr, insn_length);
        
        this->nia = this->cia + insn_length;
        
        /* execute the instruction */
        if (not op->execute(this))
          {
            std::cerr << "Something went wrong with instruction execution.\n";
            throw TODO();
          }
      }
    
    //  std::cerr << "Stopped: current=0x" << std::hex << current_insn_addr << ", next: " << std::hex << next_insn_addr << std::dec << std::endl;
    return 0;
  }
  
} // end of namespace concrete
} // end of namespace vle

