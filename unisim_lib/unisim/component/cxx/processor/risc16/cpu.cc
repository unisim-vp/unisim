/*
 *  Copyright (c) 2007-2015,
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
 * Authors: Réda Nouacer (reda.nouacer@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/component/cxx/processor/risc16/cpu.hh>
#include <sstream>
#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace risc16 {

CPU::CPU(const char *name, Object *_parent)
	: Object(name, _parent, "16-bit RISC processor for teaching")
	, Decoder()
	, unisim::kernel::service::Service<MemoryAccessReportingControl>(name, _parent)
	, unisim::kernel::service::Service<Memory<uint64_t> > (name, _parent)
	, unisim::kernel::service::Service<Disassembly<uint64_t> >(name, _parent)
	, unisim::kernel::service::Service<Registers>(name, _parent)
	, unisim::kernel::service::Client<DebugControl<uint64_t> >(name, _parent)
	, unisim::kernel::service::Client<MemoryAccessReporting<uint64_t> >(name, _parent)
	, unisim::kernel::service::Client<Memory<uint64_t> > (name, _parent)
	, memory_export("memory-export", this)
	, memory_access_reporting_control_export("memory-access-reporting-control-export", this)
	, disasm_export("disasm_export", this)
	, registers_export("registers_export", this)
	, debug_control_import("debug-control-import", this)
	, memory_access_reporting_import("memory-access-reporting-import", this)
	, memory_import("memory-import", this)
	, trap_reporting_import("trap_reporting_import", this)
	, cia(0)
	, nia(0)
	, gpr()
	, requires_memory_access_reporting(false)
	, requires_finished_instruction_reporting(false)
	, logger(*this)
{
	Reset();
	
	for (unsigned int i= 0; i < 16; i++)
	{
		std::stringstream strm;
		strm << "r" << i;
		string name = strm.str();
		registers_registry[name] = new unisim::util::debug::SimpleRegister<uint16_t>(name.c_str(), &gpr[i]);
		extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>(name.c_str(), this, gpr[i], "General Purpose Register"));
	}
}

CPU::~CPU()
{
	unsigned int n = extended_registers_registry.size();
	for(unsigned int i = 0; i < n; i++)
	{
		delete extended_registers_registry[i];
	}
	
	std::map<std::string, unisim::service::interfaces::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}
}

uint16_t CPU::fetch(uint16_t addr)
{
	if(requires_memory_access_reporting)
	{
		if(memory_access_reporting_import)
		{
			memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, 2 * addr, INS_SIZE);
		}
	}
	
	return mem_read(addr);
}

void CPU::step_instruction()
{
	if(debug_control_import)
	{
		DebugControl<uint64_t>::DebugCommand dbg_cmd;

		do
		{
			dbg_cmd = debug_control_import->FetchDebugCommand(2 * cia);

			if(dbg_cmd == DebugControl<uint64_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<uint64_t>::DBG_SYNC) continue;
			if(dbg_cmd == DebugControl<uint64_t>::DBG_KILL) Stop(0);
		}
		while(1);
	}

	uint16_t instruction;
	Operation *operation;
	char disasm_buffer[265];

	instruction = fetch(cia);
	operation = Decode(2 * cia, instruction);

	operation->disasm(cia, disasm_buffer);

	nia = cia + 1;

	operation->execute((CPU *) this);

	if(requires_finished_instruction_reporting)
	{
		if(memory_access_reporting_import)
		{
			memory_access_reporting_import->ReportFinishedInstruction(2 * cia, 2 * nia);
		}
	}

	cia = nia;
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CPU::BeginSetup()
{
	Reset();

	if(!memory_access_reporting_import)
	{
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}

	return true;
}

bool CPU::Setup(ServiceExportBase *srv_export)
{
	return true;
}

bool CPU::EndSetup()
{
	return true;
}

void CPU::OnDisconnect()
{

}

//=====================================================================
//=             memory access reporting control interface methods     =
//=====================================================================

void CPU::RequiresMemoryAccessReporting(bool report)
{
	requires_memory_access_reporting = report;
}

void CPU::RequiresFinishedInstructionReporting(bool report)
{
	requires_finished_instruction_reporting = report;
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

void CPU::Reset()
{
	cia = 0;
	for(unsigned int i= 0; i < 16; i++) gpr[i] = 0;
}

bool CPU::ReadMemory(uint64_t addr, void *buffer, uint32_t size)
{
	return memory_import ? memory_import->ReadMemory(addr, buffer, size) : false;
}

bool CPU::WriteMemory(uint64_t addr, const void *buffer, uint32_t size)
{
	return memory_import ? memory_import->WriteMemory(addr, buffer, size) : false;
}

string CPU::Disasm(uint64_t addr, uint64_t &next_addr)
{
	uint16_t instruction;

	ReadMemory(addr, &instruction, INS_SIZE);
	instruction = unisim::util::endian::BigEndian2Host(instruction);

	Operation *operation = Decode(addr, instruction);

	char disasm_buffer[265];
	operation->disasm(addr, disasm_buffer);

	std::stringstream strm;
	strm << "0x" << std::hex << operation->GetEncoding() << std::dec << " " << disasm_buffer;

	next_addr = addr + 2;

	return strm.str();
}

unisim::service::interfaces::Register *CPU::GetRegister(const char *name)
{
	map<string, unisim::service::interfaces::Register *>::iterator reg_iter = registers_registry.find(name);
	if(reg_iter != registers_registry.end())
	{
		return (*reg_iter).second;
	}

	return 0;
}

/** Scan available registers for the Registers interface
 * 
 *  Allows clients of the Registers interface to scan available
 * register by providing a suitable RegisterScanner interface.
 */
void
CPU::ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner )
{
  // TODO:
}



} // end of namespace risc16
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
