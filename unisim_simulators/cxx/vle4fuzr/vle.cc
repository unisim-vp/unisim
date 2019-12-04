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
#include "top_vle_concrete.hh"
#include "top_vle_branch.hh"
#include <unisim/util/endian/endian.hh>
#include <iostream>

namespace vle {
  void
  PPCBase::ProcessException( char const* msg )
  {
    std::cerr << "exception(" << msg << ")\n";
    throw TODO();
  }
namespace branch {
  Processor::Processor( ActionNode& root, uint32_t addr, uint32_t length )
    : path(&root), reg_values(), cia(addr), nia(addr+length), has_branch(false)
  {}
  
} // end of namespace branch
namespace concrete {
  
  Processor::Processor()
    : reg_values(), cia(), nia()
  {}

  Processor::RegView const*
  Processor::get_reg(char const* id, uintptr_t size)
  {
    if (regname("gpr",id,size))
      {
        static struct : public RegView
        {
          void write( EmuProcessor& proc, int id, uint64_t value ) const { Self(proc).SetGPR(id, value); }
          void read( EmuProcessor& proc, int id, uint64_t* value ) const { *value = Self(proc).GetGPR(id); }
        } _;
        return &_;
      }

    if (regname("lr",id,size))
      {
        static struct : public RegView
        {
          void write( EmuProcessor& proc, int id, uint64_t value ) const { Self(proc).GetLR() = value; }
          void read( EmuProcessor& proc, int id, uint64_t* value ) const { *value = Self(proc).GetLR(); }
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
        
        /* Process exceptions */
        
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
            op = page.ops[insn_offset] = decoder.NCDecode(insn_addr, insn);

            {
              static branch::Decoder bdecoder;
              auto bop = bdecoder.NCDecode( insn_addr, insn );
          
              branch::ActionNode root;
              for (bool end = false; not end;)
                {
                  branch::Processor bp( root, insn_addr, insn_length );
                  bop->execute( &bp );
                  op->branch.update( bp.has_branch, bp.nia );
                  end = bp.path->close();
                }
    
              delete bop;
            }
            
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

