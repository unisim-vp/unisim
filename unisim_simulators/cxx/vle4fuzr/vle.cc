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
#include "top_vle_branch.hh"
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <unisim/util/endian/endian.hh>
#include <iostream>

namespace vle {

namespace branch {
  Processor::Processor( ActionNode& root, uint32_t addr, uint32_t length )
    : path(&root), reg_values(), cia(addr), nia(addr+length), has_branch(false)
  {}
} // end of namespace branch

namespace concrete {

  Processor::Processor()
    : decoder(), current_operation(), reg_values(), cia(), nia()
  {}

  Processor::RegView const*
  Processor::get_reg(char const* id, uintptr_t size, int regid)
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

    if (regname("pc",id,size))
      {
        static struct : public RegView
        {
          void write( EmuProcessor& proc, int id, uint64_t value ) const { Self(proc).DebugBranch(value); }
          void read( EmuProcessor& proc, int id, uint64_t* value ) const { *value = Self(proc).GetCIA(); }
        } _;
        return &_;
      }

    std::cerr << "Register '" << std::string(id,size) << "' not found.\n";
    return 0;
  }

  char const*
  Processor::get_asm()
  {
    if (current_operation)
      {
        std::ostringstream buf;
        current_operation->disasm(this, buf);
        std::cerr << std::endl;
        asmbuf = buf.str();
      }
    else
      asmbuf = "?";
    return asmbuf.c_str();
  }

  namespace {
    template <class Instruction>
    void
    ComputeBranchInfo(Instruction* op)
    {
      if (not op->stat.branch.startupdate())
        return; // Already computed

      uint32_t insn_addr = op->GetAddr(), insn = op->GetEncoding();
      unsigned insn_length = op->GetLength() / 8;

      static branch::Decoder bdecoder;
      auto bop = bdecoder.NCDecode( insn_addr, insn );

      branch::ActionNode root;
      for (bool end = false; not end;)
        {
          branch::Processor bp( root, insn_addr, insn_length );
          bop->execute( &bp );
          op->stat.branch.update( bp.has_branch, bp.nia, op->stat.branch.NoHint );
          end = bp.path->close();
        }

      delete bop;
    }
  }

  void
  Processor::run( uint64_t begin, uint64_t until, uint64_t count )
  {
    this->Branch(begin);

    do
      {
        // Go to the next instruction
        uint32_t insn_addr = this->cia = this->nia;

        // Fetch
        CodeType insn = this->Fetch(insn_addr);

        // Decode
        Operation* op = current_operation = decoder.Decode(insn_addr, insn);

        ComputeBranchInfo(op);

        // Monitor
        if (unlikely(this->disasm))
          {
            op->disasm(this, std::cerr << std::hex << insn_addr << ": " );
            std::cerr << std::endl;
          }

        unsigned insn_length = op->GetLength() / 8;
        insn_hooks(insn_addr, insn_length);

        this->nia = this->cia + insn_length;

        /* execute the instruction */
        if (not op->execute(this))
          {
            /* Process exceptions */
            std::cerr << "Pending exceptions.\n";
            abort("ProcessorException()");
          }

        if (debug_branch != uint64_t(-1))
          {
            this->bblock = true;
            Branch(debug_branch);
            DebugBranch(-1);
          }
        else
          this->bblock = (op->stat.branch.has_jump());
      }
    while (this->nia != until and --count != 0);
  }

} // end of namespace concrete
} // end of namespace vle

