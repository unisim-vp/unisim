/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#ifndef __VLE4FUZR_ARM_HH__
#define __VLE4FUZR_ARM_HH__

#include <emu.hh>
#include <unisim/component/cxx/processor/arm/vmsav7/cpu.hh>
#include <unisim/component/cxx/processor/arm/isa_arm32.hh>

struct ArmProcessor
  : public Processor
  , public unisim::component::cxx::processor::arm::vmsav7::CPU
{
  typedef unisim::component::cxx::processor::arm::vmsav7::CPU CPU;

  ArmProcessor( char const* name, bool is_thumb );
  ~ArmProcessor();

  static ArmProcessor& Self( Processor& proc ) { return dynamic_cast<ArmProcessor&>( proc ); }
  
  Processor::RegView const* get_reg(int regid) override
  {
    switch (regid)
      {
      case 0: /* UC_ARM_REG_INVALID */
        break;
      case 1: /* UC_ARM_REG_APSR */
        static struct : public RegView
        {
          static uint32_t apsr_mask() { return 0xf80f0000; }
          virtual void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetCPSR(*(uint32_t*)bytes, apsr_mask()); }
          virtual void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetCPSR() & apsr_mask(); }
        } _;
        return &_;
        break;
      case 2: /* UC_ARM_REG_APSR_NZCV */
        break;
      case 3: /* UC_ARM_REG_CPSR */
        break;
      case 4: /* UC_ARM_REG_FPEXC */
        break;
      case 5: /* UC_ARM_REG_FPINST */
        break;
      case 6: /* UC_ARM_REG_FPSCR */
        break;
      case 7: /* UC_ARM_REG_FPSCR_NZCV */
        break;
      case 8: /* UC_ARM_REG_FPSID */
        break;
      case 9: /* UC_ARM_REG_ITSTATE */
        break;
      case 10: /* UC_ARM_REG_LR, UC_ARM_REG_R14 */
      case 11: /* UC_ARM_REG_PC, UC_ARM_REG_R15 */
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetGPR(id-10+14, *(uint32_t*)bytes); }
            void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetGPR(id-10+14); }
          } _;
          return &_;
        } break;
      case 12: /* UC_ARM_REG_SP, UC_ARM_REG_R13 */
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetGPR(13, *(uint32_t*)bytes); }
            void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetGPR(13); }
          } _;
          return &_;
        } break;
      case 13: /* UC_ARM_REG_SPSR */
        break;
      case 14: /* UC_ARM_REG_D0 */
        break;
      case 15: /* UC_ARM_REG_D1 */
        break;
      case 16: /* UC_ARM_REG_D2 */
        break;
      case 17: /* UC_ARM_REG_D3 */
        break;
      case 18: /* UC_ARM_REG_D4 */
        break;
      case 19: /* UC_ARM_REG_D5 */
        break;
      case 20: /* UC_ARM_REG_D6 */
        break;
      case 21: /* UC_ARM_REG_D7 */
        break;
      case 22: /* UC_ARM_REG_D8 */
        break;
      case 23: /* UC_ARM_REG_D9 */
        break;
      case 24: /* UC_ARM_REG_D10 */
        break;
      case 25: /* UC_ARM_REG_D11 */
        break;
      case 26: /* UC_ARM_REG_D12 */
        break;
      case 27: /* UC_ARM_REG_D13 */
        break;
      case 28: /* UC_ARM_REG_D14 */
        break;
      case 29: /* UC_ARM_REG_D15 */
        break;
      case 30: /* UC_ARM_REG_D16 */
        break;
      case 31: /* UC_ARM_REG_D17 */
        break;
      case 32: /* UC_ARM_REG_D18 */
        break;
      case 33: /* UC_ARM_REG_D19 */
        break;
      case 34: /* UC_ARM_REG_D20 */
        break;
      case 35: /* UC_ARM_REG_D21 */
        break;
      case 36: /* UC_ARM_REG_D22 */
        break;
      case 37: /* UC_ARM_REG_D23 */
        break;
      case 38: /* UC_ARM_REG_D24 */
        break;
      case 39: /* UC_ARM_REG_D25 */
        break;
      case 40: /* UC_ARM_REG_D26 */
        break;
      case 41: /* UC_ARM_REG_D27 */
        break;
      case 42: /* UC_ARM_REG_D28 */
        break;
      case 43: /* UC_ARM_REG_D29 */
        break;
      case 44: /* UC_ARM_REG_D30 */
        break;
      case 45: /* UC_ARM_REG_D31 */
        break;
      case 46: /* UC_ARM_REG_FPINST2 */
        break;
      case 47: /* UC_ARM_REG_MVFR0 */
        break;
      case 48: /* UC_ARM_REG_MVFR1 */
        break;
      case 49: /* UC_ARM_REG_MVFR2 */
        break;
      case 50: /* UC_ARM_REG_Q0 */
        break;
      case 51: /* UC_ARM_REG_Q1 */
        break;
      case 52: /* UC_ARM_REG_Q2 */
        break;
      case 53: /* UC_ARM_REG_Q3 */
        break;
      case 54: /* UC_ARM_REG_Q4 */
        break;
      case 55: /* UC_ARM_REG_Q5 */
        break;
      case 56: /* UC_ARM_REG_Q6 */
        break;
      case 57: /* UC_ARM_REG_Q7 */
        break;
      case 58: /* UC_ARM_REG_Q8 */
        break;
      case 59: /* UC_ARM_REG_Q9 */
        break;
      case 60: /* UC_ARM_REG_Q10 */
        break;
      case 61: /* UC_ARM_REG_Q11 */
        break;
      case 62: /* UC_ARM_REG_Q12 */
        break;
      case 63: /* UC_ARM_REG_Q13 */
        break;
      case 64: /* UC_ARM_REG_Q14 */
        break;
      case 65: /* UC_ARM_REG_Q15 */
        break;
      case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78:
        /* UC_ARM_REG_R0,  UC_ARM_REG_R1,  UC_ARM_REG_R2,  UC_ARM_REG_R3,  UC_ARM_REG_R4,  UC_ARM_REG_R5, UC_ARM_REG_R6, UC_ARM_REG_R7,
         * UC_ARM_REG_R8,  UC_ARM_REG_R9,  UC_ARM_REG_R10, UC_ARM_REG_R11, UC_ARM_REG_R12,
         * UC_ARM_REG_SB=R9, UC_ARM_REG_SL=R10, UC_ARM_REG_FP=R11 */
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetGPR(id - 66, *(uint32_t*)bytes); }
            void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetGPR(id - 66); }
          } _;
          return &_;
        } break;
      case 79: /* UC_ARM_REG_S0 */
        break;
      case 80: /* UC_ARM_REG_S1 */
        break;
      case 81: /* UC_ARM_REG_S2 */
        break;
      case 82: /* UC_ARM_REG_S3 */
        break;
      case 83: /* UC_ARM_REG_S4 */
        break;
      case 84: /* UC_ARM_REG_S5 */
        break;
      case 85: /* UC_ARM_REG_S6 */
        break;
      case 86: /* UC_ARM_REG_S7 */
        break;
      case 87: /* UC_ARM_REG_S8 */
        break;
      case 88: /* UC_ARM_REG_S9 */
        break;
      case 89: /* UC_ARM_REG_S10 */
        break;
      case 90: /* UC_ARM_REG_S11 */
        break;
      case 91: /* UC_ARM_REG_S12 */
        break;
      case 92: /* UC_ARM_REG_S13 */
        break;
      case 93: /* UC_ARM_REG_S14 */
        break;
      case 94: /* UC_ARM_REG_S15 */
        break;
      case 95: /* UC_ARM_REG_S16 */
        break;
      case 96: /* UC_ARM_REG_S17 */
        break;
      case 97: /* UC_ARM_REG_S18 */
        break;
      case 98: /* UC_ARM_REG_S19 */
        break;
      case 99: /* UC_ARM_REG_S20 */
        break;
      case 100: /* UC_ARM_REG_S21 */
        break;
      case 101: /* UC_ARM_REG_S22 */
        break;
      case 102: /* UC_ARM_REG_S23 */
        break;
      case 103: /* UC_ARM_REG_S24 */
        break;
      case 104: /* UC_ARM_REG_S25 */
        break;
      case 105: /* UC_ARM_REG_S26 */
        break;
      case 106: /* UC_ARM_REG_S27 */
        break;
      case 107: /* UC_ARM_REG_S28 */
        break;
      case 108: /* UC_ARM_REG_S29 */
        break;
      case 109: /* UC_ARM_REG_S30 */
        break;
      case 110: /* UC_ARM_REG_S31 */
        break;
      case 111: /* UC_ARM_REG_C1_C0_2 */
        break;
      case 112: /* UC_ARM_REG_C13_C0_2 */
        break;
      case 113: /* UC_ARM_REG_C13_C0_3 */
        break;
      case 114: /* UC_ARM_REG_ENDING */
        break;
      }
    std::cerr << "Register: " << regid << " not found.\n";
    return 0;
  }
  
  virtual void Sync() override { throw 0; }
  virtual void ResetMemory() override { throw 0; }
  virtual bool ExternalReadMemory( uint32_t addr, void* buffer, uint32_t size ) override
  {
    throw 0;
  }
  virtual bool ExternalWriteMemory( uint32_t addr, void const* buffer, uint32_t size ) override
  {
    throw 0;
  }
  virtual bool PhysicalWriteMemory( uint32_t addr, uint32_t paddr, uint8_t const* buffer, uint32_t size, uint32_t attrs ) override
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end())
      pi = mem_allocate(0, addr, (pages.size() ? (--pi)->base : 0)-1);
    else if (pi->hi() <= addr)
      pi = mem_allocate(pi->hi(), addr, (--pi != pages.end() ? pi->base : 0)-1);
    
    if (pi == pages.end())
      return false;
    uint32_t pos = addr - pi->base;
    std::copy(&buffer[0], &buffer[size], pi->at(pos));
    return true;
  }
  
  virtual bool PhysicalReadMemory( uint32_t addr, uint32_t paddr, uint8_t* buffer, uint32_t size, uint32_t attrs ) override
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end())
      pi = mem_allocate(0, addr, (pages.size() ? (--pi)->base : 0)-1);
    else if (pi->hi() <= addr)
      pi = mem_allocate(pi->hi(), addr, (--pi != pages.end() ? pi->base : 0)-1);
    
    if (pi == pages.end())
      return false;
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }
  virtual bool PhysicalFetchMemory( uint32_t addr, uint32_t paddr, uint8_t* buffer, uint32_t size, uint32_t attrs ) override
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end())
      pi = mem_allocate(0, addr, (pages.size() ? (--pi)->base : 0)-1);
    else if (pi->hi() <= addr)
      pi = mem_allocate(pi->hi(), addr, (--pi != pages.end() ? pi->base : 0)-1);
    
    if (pi == pages.end())
      return false;
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }
  
  int emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count )
  {
    if (timeout)
      {
        std::cerr << "Error: timeout unimplemented." << timeout << std::endl;
        throw 0;
      }
    
    // std::cerr << "until: " << until << std::endl;
    // std::cerr << "count: " << count << std::endl;

    Branch(begin, B_DBG);

    while (next_insn_addr != until)
      {
        /* Instruction boundary next_insn_addr becomes current_insn_addr */
        uint32_t insn_addr = this->current_insn_addr = this->next_insn_addr, insn_length = 0;
          
        try {
          if (cpsr.Get( unisim::component::cxx::processor::arm::T ))
            {
              /* Thumb state */
              unisim::component::cxx::processor::arm::isa::thumb::CodeType insn;
              ReadInsn(insn_addr, insn);
    
              /* Decode current PC */
              unisim::component::cxx::processor::arm::isa::thumb::Operation<CPU>* op = thumb_decoder.Decode(insn_addr, insn);

              /* update PC register value before execution */
              insn_length = op->GetLength() / 8;
                
              for (auto h : hooks[Hook::CODE])
                {
                  if (h->bound_check(next_insn_addr))
                    h->cb<Hook::cb_code>()(this, insn_addr, insn_length);
                }
                
              this->gpr[15] = insn_addr + 4;
              this->next_insn_addr = insn_addr + insn_length;
                
              /* Execute instruction */
              asm volatile( "thumb_operation_execute:" );
              op->execute( *this );
                
              this->ITAdvance();
            }
          else
            {
              /* Arm32 state */
              unisim::component::cxx::processor::arm::isa::arm32::CodeType insn;
              ReadInsn(insn_addr, insn);
      
              /* Decode current PC */
              unisim::component::cxx::processor::arm::isa::arm32::Operation<CPU>* op = arm32_decoder.Decode(insn_addr, insn);
    
              /* update PC register value before execution */
              insn_length = op->GetLength() / 8;

              for (auto h : hooks[Hook::CODE])
                {
                  if (h->bound_check(next_insn_addr))
                    h->cb<Hook::cb_code>()(this, insn_addr, insn_length);
                }
              
              this->gpr[15] = insn_addr + 8;
              this->next_insn_addr = insn_addr + insn_length;
                
              /* Execute instruction */
              asm volatile( "arm32_operation_execute:" );
              op->execute( *this );
              //op->profile(profile);
            }
            
          // if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
          //   memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, insn_length);
            
          instruction_counter++; /* Instruction regularly finished */
        }
          
        //         catch (SVCException const& svexc) {
        //           /* Resuming execution, since SVC exceptions are explicitly
        //    * requested from regular instructions. ITState will be updated as
        //    * needed by TakeSVCException (as done in the ARM spec). */
        //   if (unlikely( requires_commit_instruction_reporting and memory_access_reporting_import ))
        //     memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, insn_length);

        //   instruction_counter++; /* Instruction regularly finished */
    
        //   this->TakeSVCException();
        // }
  
        // catch (DataAbortException const& daexc) {
        //   /* Abort execution, and take processor to data abort handler */
    
        //   if (unlikely(trap_reporting_import))
        //     trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
    
        //   this->TakeDataOrPrefetchAbortException(true); // TakeDataAbortException
        // }
  
        // catch (PrefetchAbortException const& paexc) {
        //   /* Abort execution, and take processor to prefetch abort handler */
    
        //   if (unlikely(trap_reporting_import))
        //     trap_reporting_import->ReportTrap( *this, "Prefetch Abort Exception" );
    
        //   this->TakeDataOrPrefetchAbortException(false); // TakePrefetchAbortException
        // }
  
        // catch (UndefInstrException const& undexc) {
        //   /* Abort execution, and take processor to undefined handler */
    
        //   if (unlikely(trap_reporting_import))
        //     trap_reporting_import->ReportTrap( *this, "Undefined Exception" );
    
        //   this->TakeUndefInstrException();
        // }
  
        catch (unisim::component::cxx::processor::arm::Exception const& exc)
          {
            throw 0;
          // logger << DebugError << "Unimplemented exception (" << exc.what() << ")"
          //        << " pc: " << std::hex << current_insn_addr << std::dec
          //        << EndDebugError;
          // this->Stop(-1);
          }
      }
    
    //  std::cerr << "Stopped: current=0x" << std::hex << current_insn_addr << ", next: " << std::hex << next_insn_addr << std::dec << std::endl;
    return 0;
  }
  
};

#endif /* __VLE4FUZR_ARM_HH__ */

