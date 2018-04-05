/*
 *  Copyright (c) 2018,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <arch.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/likely/likely.hh>

Arch::Arch()
  : unisim::kernel::service::Object("e5500", 0)
  , unisim::kernel::service::Service< unisim::service::interfaces::Registers >("e5500", 0)
  , unisim::kernel::service::Service< unisim::service::interfaces::MemoryInjection<uint64_t> >("e5500", 0)
  , unisim::kernel::service::Service< unisim::service::interfaces::Memory<uint64_t> >("e5500", 0)
  , unisim::kernel::service::Service< unisim::service::interfaces::Disassembly<uint64_t> >("e5500", 0)
  , unisim::kernel::service::Service< unisim::service::interfaces::MemoryAccessReportingControl >("e5500", 0)
  , unisim::kernel::service::Client< unisim::service::interfaces::DebugYielding >("e5500", 0)
  , unisim::kernel::service::Client< unisim::service::interfaces::TrapReporting >("e5500", 0)
  , unisim::kernel::service::Client< unisim::service::interfaces::MemoryAccessReporting<uint64_t> >("e5500", 0)
  , registers_export("registers-export", this)
  , memory_injection_export("memory-injection-export", this)
  , memory_export("memory-export", this)
  , disasm_export("disasm-export", this)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , requires_memory_access_reporting(false)
  , requires_fetch_instruction_reporting(false)
  , requires_commit_instruction_reporting(false)
  , debug_yielding_import("debug-yielding-import", this)
  , trap_reporting_import("trap-reporting-import", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , xer(0)
  , cr(0)
  , msr(0)
  , fpscr(0)
  , insn_count(0)
  , time_base(0)
    // , linux_os(0)
{
  for (int idx = 0; idx < 32; ++idx)
    {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::powerpc::GPRPrint(idx);
      regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &gprs[idx]);
    }

  struct { char const* name; uint64_t* reg; } dedicated_registers[] = {
    { "cr", &cr.value },
    { "cia", &cia },
    { "pc", &cia },
  };

  for (int idx = sizeof(dedicated_registers)/sizeof(dedicated_registers[0]); --idx >= 0;)
    regmap[dedicated_registers[idx].name] = new unisim::util::debug::SimpleRegister<uint64_t>(dedicated_registers[idx].name, dedicated_registers[idx].reg);
}
  
Arch::~Arch()
{
  for (auto && reg : regmap)
    delete reg.second;
}

void
Arch::RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report )
{
  switch (type) {
  case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
  case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
  case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
  default: throw 0;
  }
}

bool
Arch::ReadMemory(uint64_t addr, void* buffer, unsigned size )
{
  memory.read( (uint8_t*)buffer, addr, size );
  return true;
}

bool
Arch::WriteMemory(uint64_t addr, void const* buffer, unsigned size)
{
  memory.write( addr, (uint8_t*)buffer, size );
  return true;
}
  
unisim::service::interfaces::Register*
Arch::GetRegister(char const* name)
{
  auto reg = regmap.find( name );
  return (reg == regmap.end()) ? 0 : reg->second;
}
  
void
Arch::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
  for (int idx = 0; idx < 32; ++idx)
    {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::powerpc::GPRPrint(idx);
      scanner.Append( GetRegister( regname.str().c_str() ) );
    }
    
  for (int idx = 0; idx < 32; ++idx)
    {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::powerpc::FPRPrint(idx);
      scanner.Append( GetRegister( regname.str().c_str() ) );
    }
}
  
bool
Arch::InjectReadMemory(uint64_t addr, void* buffer, unsigned size)
{
  memory.read( (uint8_t*)buffer, addr, size );
  return true;
}

bool
Arch::InjectWriteMemory(uint64_t addr, void const* buffer, unsigned size)
{
  memory.write( addr, (uint8_t*)buffer, size );
  return true;
}

// Implementation of ExecuteSystemCall
void
Arch::ThrowException( SystemCallInterrupt::SystemCall const& )
{
  if (not linux_os)
    { throw std::logic_error( "No linux OS emulation connected" ); }
  linux_os->ExecuteSystemCall( gprs[0] );
}

bool
Arch::InstructionFetch(uint64_t addr, uint32_t& insn)
{
  uint8_t bytes[4];
  memory.read( &bytes[0], addr, 4 );
  
  insn = (uint32_t(bytes[0] << 24) | uint32_t(bytes[1] << 16) | uint32_t(bytes[2] << 8) | uint32_t(bytes[3] << 0));
  
  return true;
}

Arch::Operation*
Arch::fetch()
{
  U64 addr = this->cia;
  
  if (unlikely(requires_fetch_instruction_reporting and memory_access_reporting_import))
    memory_access_reporting_import->ReportFetchInstruction( addr );

  if (debug_yielding_import)
    debug_yielding_import->DebugYield();

  uint32_t insn = 0;
  if (unlikely(not InstructionFetch(addr, insn)))
    throw 0;
  
  Operation* operation = decoder.Decode(addr, insn);

  nia = cia + 4;

  return operation;
}

std::string
Arch::Disasm(ADDRESS addr, ADDRESS& next_addr)
{
  uint32_t insn = 0;
  if (unlikely(not InstructionFetch(addr, insn)))
    throw 0;
  
  std::ostringstream buf;
  decoder.Decode(addr, insn)->disasm( buf );
  next_addr = addr + 4;
  
  return buf.str();
}

void
Arch::commit()
{
  if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
    memory_access_reporting_import->ReportCommitInstruction(cia, 4);

  cia = nia;
  insn_count += 1;
  time_base += 2;
}

bool
Arch::MoveFromSPR( unsigned id, U64& value )
{
  U64 val;
  switch (id)
    {
    default: return false;
    case 268: val = time_base; break;
    }
  value = val;
  return true;
}

bool
Arch::Fp32Load(unsigned id, U64 addr)
{
  Flags flags;
  flags.setRoundingMode(fpscr.Get<FPSCR::RN>());
  fprs[id].convertAssign(SoftFloat(SoftFloat::FromRawBits,IntLoad<U32>( addr )), flags);
  return true;
}

bool
Arch::Fp32Store(unsigned id, U64 addr)
{
  Flags flags;
  flags.setZeroRound();
  IntStore( addr, U32(SoftFloat(fprs[id], flags).queryRawBits()) );
  return true;
}

bool
Arch::FpStoreLSW(unsigned id, U64 addr)
{
  IntStore( addr, U32(fprs[id].queryRawBits()) );
  return true;
}

U64
UnsignedMultiplyHigh( U64 lop, U64 rop )
{
  U64 lhi = U64(lop >> 32), llo = U64(U32(lop)), rhi = U64(rop >> 32), rlo = U64(U32(rop));
  U64 hihi( lhi*rhi ), hilo( lhi*rlo), lohi( llo*rhi ), lolo( llo*rlo );
  return (((lolo >> 32) + U64(U32(hilo)) + U64(U32(lohi))) >> 32) + (hilo >> 32) + (lohi >> 32) + hihi;
}

S64
SignedMultiplyHigh( S64 lop, S64 rop )
{
  bool lsign = (lop < S64(0)), rsign = (rop < S64(0));
  U64 ulop = lsign ? -lop : lop, urop = rsign ? -rop : rop;
  
  if (lop < S64(0)) { ulop = -lop; lsign = true; } else { ulop = lop; lsign = false; }
  if (rop < S64(0)) { urop = -rop; rsign = true; } else { urop = rop; rsign = false; }
  
  U64 uhi = UnsignedMultiplyHigh(ulop, urop), ulo = ulop*urop;
  
  return S64( (lsign xor rsign) ? (ulo == 0) ? -uhi : ~uhi : uhi );
}

