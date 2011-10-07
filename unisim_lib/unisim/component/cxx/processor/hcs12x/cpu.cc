/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/cxx/processor/hcs12x/hcs12x.hh>
#include <iostream>
#include <stdlib.h>
#include "unisim/util/debug/simple_register.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

using std::cout;

using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;

template void EBLB::setter<uint8_t>(uint8_t rr, uint8_t val);
template void EBLB::setter<uint16_t>(uint8_t rr, uint16_t val);
template uint8_t EBLB::getter<uint8_t>(uint8_t rr);
template uint16_t EBLB::getter<uint16_t>(uint8_t rr);
template void EBLB::exchange<uint8_t>(uint8_t rrSrc, uint8_t rrDst);
template void EBLB::exchange<uint16_t>(uint8_t rrSrc, uint8_t rrDst);

CPU::CPU(const char *name, Object *parent):
	Object(name, parent),
	Client<DebugControl<service_address_t> >(name, parent),
	Client<MemoryAccessReporting<service_address_t> >(name, parent),
	Service<MemoryAccessReportingControl>(name, parent),
	Service<Disassembly<service_address_t> >(name, parent),
	Service<Registers>(name, parent),
	Service<Memory<service_address_t> >(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	Client<SymbolTableLookup<service_address_t> >(name, parent),
	Client<TrapReporting>(name, parent),
	queueCurrentAddress(0xFFFE),
	queueFirst(-1),
	queueNElement(0),
	disasm_export("disasm_export", this),
	registers_export("registers_export", this),
	memory_export("memory_export", this),
	memory_access_reporting_control_export("memory_access_reporting_control_export", this),
	debug_control_import("debug_control_import", this),
	memory_access_reporting_import("memory_access_reporting_import", this),
	memory_import("memory_import", this),
	symbol_table_lookup_import("symbol-table-lookup-import",  this),
	logger(0),
	trap_reporting_import("trap_reporting_import", this),
	cpu_cycle(0),
	bus_cycle(0),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all),
	verbose_setup(false),
	param_verbose_setup("verbose-setup", this, verbose_setup),
	verbose_step(false),
	param_verbose_step("verbose-step", this, verbose_step),
	verbose_dump_regs_start(false),
	param_verbose_dump_regs_start("verbose-dump-regs-start", this, verbose_dump_regs_start),
	verbose_dump_regs_end(false),
	param_verbose_dump_regs_end("verbose-dump-regs-end", this, verbose_dump_regs_end),
	verbose_exception(false),
	param_verbose_exception("verbose-exception", this, verbose_exception),
	trace_enable(false),
	param_trace_enable("trace-enable", this, trace_enable),
	requires_memory_access_reporting(true),
	param_requires_memory_access_reporting("requires-memory-access-reporting", this, requires_memory_access_reporting),
	requires_finished_instruction_reporting(true),
	param_requires_finished_instruction_reporting("requires-finished-instruction-reporting", this, requires_finished_instruction_reporting),
	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled),
	asynchronous_interrupt(false),
	maskableIbit_interrupt(false),
	nonMaskableXIRQ_interrupt(false),
	nonMaskableAccessError_interrupt(false),
	nonMascableSWI_interrupt(false),
	trap_interrupt(false),
	reset(false),
	syscall_interrupt(false),
	spurious_interrupt(false),
	instruction_counter(0),
	cycles_counter(0),
	data_load_counter(0),
	data_store_counter(0),
	max_inst((uint64_t) -1),
	stat_instruction_counter("instruction-counter", this, instruction_counter),
	stat_cycles_counter("cycles-counter", this, cycles_counter),
	stat_load_counter("data-load-counter", this, data_load_counter),
	stat_store_counter("data-store-counter", this, data_store_counter),
	param_max_inst("max-inst",this,max_inst),

	trap_on_instruction_counter(-1),
	param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter)


{
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_cycles_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_load_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_store_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
    ccr = new CCR_t(&ccrReg);

    eblb = new EBLB(this);

	logger = new unisim::kernel::logger::Logger(*this);

	char buf[80];

	sprintf(buf, "A");
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &regA);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>(buf, this, regA, "Accumulator register A"));

	sprintf(buf, "B");
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &regB);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>(buf, this, regB, "Accumulator register B"));

	sprintf(buf, "D");
	registers_registry[buf] = new ConcatenatedRegister<uint16_t,uint8_t>(buf, &regA, &regB);
	extended_registers_registry.push_back(new ConcatenatedRegisterView<uint16_t,uint8_t>(buf, this,  &regA, &regB, "Accumulator register D"));

	sprintf(buf, "X");
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &regX);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>(buf, this, regX, "Index register X"));

	sprintf(buf, "Y");
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &regY);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>(buf, this, regY, "Index register Y"));

	sprintf(buf, "SP");
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &regSP);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>(buf, this, regSP, "Stack Pointer SP"));

	sprintf(buf, "PC");
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &regPC);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>(buf, this, regPC, "Program counter PC"));

	sprintf(buf, "%s", ccr->GetName());
	registers_registry[buf] = ccr;
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>(buf, this, ccrReg, "CCR"));

	unisim::util::debug::Register *ccrl = ccr->GetLowRegister();
	sprintf(buf, "%s", ccrl->GetName());
	registers_registry[buf] = ccrl;
	extended_registers_registry.push_back(new TimeBaseRegisterView(buf, this, ccrReg, TimeBaseRegisterView::TB_LOW, "CCR LOW"));

	unisim::util::debug::Register *ccrh = ccr->GetHighRegister();
	sprintf(buf, "%s", ccrh->GetName());
	registers_registry[buf] = ccrh;
	extended_registers_registry.push_back(new TimeBaseRegisterView(buf, this, ccrReg, TimeBaseRegisterView::TB_HIGH, "CCR HIGH"));

}

CPU::~CPU()
{
	if (eblb) { delete eblb; eblb = NULL;}
	// ccr will be release while releasing registers_registry
	//if (ccr) { delete ccr; ccr = NULL;}

	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	if (logger) { delete logger; logger = NULL;}
}

void CPU::SetEntryPoint(address_t cpu_address)
{
	setRegPC(cpu_address);
}


void CPU::Reset()
{
	regA = 0;
	regB = 0;
	regX = 0;
	regY = 0;
	regSP = 0;
	regPC = 0;
	regTMP[0] = 0;
	regTMP[1] = 0;
	regTMP[2] = 0;
	lastPC = 0;

	ccr->reset();
	for (unsigned int i=0; i < QUEUE_SIZE; i++) queueBuffer[i] = 0;
}

//=====================================================================
//=                    execution handling methods                     =
//=====================================================================


void CPU::Stop(int ret)
{
	exit(ret);
}

void CPU::Sync()
{

}

void CPU::OnBusCycle()
{
}

uint8_t CPU::Step()
{
	address_t 	current_pc;

	uint8_t 	buffer[MAX_INS_SIZE];

	Operation 	*op;
	uint8_t	opCycles = 0;

	current_pc = getRegPC();

	try
	{

		VerboseDumpRegsStart();

		if(debug_enabled && verbose_step)
			*logger << DebugInfo << "Starting step at PC = 0x" << std::hex << current_pc << std::dec << std::endl << EndDebugInfo;

		if(debug_control_import) {
			DebugControl<service_address_t>::DebugCommand dbg_cmd;

			do {
				if(debug_enabled && verbose_step)
					*logger << DebugInfo << "Fetching debug command (PC = 0x" << std::hex << current_pc << std::dec << ")"
						<< std::endl << EndDebugInfo;

				dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);

				if(dbg_cmd == DebugControl<service_address_t>::DBG_STEP) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_STEP command (PC = 0x"
							<< std::hex << current_pc << std::dec << ")"
							<< std::endl << EndDebugInfo;
					break;
				}
				if(dbg_cmd == DebugControl<service_address_t>::DBG_SYNC) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_SYNC command (PC = 0x"
							<< std::hex << current_pc << std::dec << ")"
							<< std::endl << EndDebugInfo;
					Sync();
					continue;
				}

				if(dbg_cmd == DebugControl<service_address_t>::DBG_KILL) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_KILL command (PC = 0x"
							<< std::hex << current_pc << std::dec << ")"
							<< std::endl << EndDebugInfo;
					Stop(0);
				}
				if(dbg_cmd == DebugControl<service_address_t>::DBG_RESET) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_RESET command (PC = 0x"
							<< std::hex << current_pc << std::dec << ")"
							<< std::endl << EndDebugInfo;
				}
			} while(1);
		}

		if(requires_memory_access_reporting) {
			if(memory_access_reporting_import) {
				if(debug_enabled && verbose_step)
					*logger << DebugInfo
						<< "Reporting memory access for fetch at address 0x"
						<< std::hex << current_pc << std::dec
						<< std::endl << EndDebugInfo;

				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<service_address_t>::MAT_READ, MemoryAccessReporting<service_address_t>::MT_INSN, current_pc, MAX_INS_SIZE);
			}
		}


		if(debug_enabled && verbose_step)
		{
			*logger << DebugInfo
				<< "Fetching (reading) instruction at address 0x"
				<< std::hex << current_pc << std::dec
				<< std::endl << EndDebugInfo;
		}

		queueFetch(current_pc, buffer, MAX_INS_SIZE);
		CodeType 	insn( buffer, MAX_INS_SIZE*8);

		/* Decode current PC */
		if(debug_enabled && verbose_step)
		{
			stringstream ctstr;
			ctstr << insn;
			*logger << DebugInfo
				<< "Decoding instruction at 0x"
				<< std::hex << current_pc << std::dec
				<< " (0x" << std::hex << ctstr.str() << std::dec << ")"
				<< std::endl << EndDebugInfo;
		}

		op = this->Decode(current_pc, insn);
		lastPC = current_pc;
                unsigned int insn_length = op->GetLength();
                if (insn_length % 8) throw "InternalError";
		setRegPC(current_pc + (insn_length/8));

		queueFlush(insn_length/8);

		/* Execute instruction */

		if (trace_enable) {
			stringstream disasm_str;
			stringstream ctstr;

			op->disasm(disasm_str);

			ctstr << op->GetEncoding();

			*logger << DebugInfo << GetSimulatedTime() << " ms: "
				<< "PC = 0x" << std::hex << current_pc << std::dec << " : "
				<< GetFunctionFriendlyName(current_pc) << " : "
				<< disasm_str.str()
				<< " : (0x" << std::hex << ctstr.str() << std::dec << " ) " << EndDebugInfo	<< std::endl;

		}

		if (debug_enabled && verbose_step) {
			stringstream disasm_str;
			stringstream ctstr;

			op->disasm(disasm_str);

			ctstr << op->GetEncoding();
			*logger << DebugInfo << GetSimulatedTime() << "ms: "
				<< "Executing instruction "
				<< disasm_str.str()
				<< " at PC = 0x" << std::hex << current_pc << std::dec
				<< " (0x" << std::hex << ctstr.str() << std::dec << ") , Instruction Counter = " << instruction_counter
				<< "  " << EndDebugInfo	<< std::endl;
		}

		op->execute(this);

		opCycles = op->getCycles();

		cycles_counter += opCycles;

		VerboseDumpRegsEnd();

		instruction_counter++;

		RegistersInfo();

		if ((trap_reporting_import) && (instruction_counter == trap_on_instruction_counter)) {
			trap_reporting_import->ReportTrap();
		}

		if(requires_finished_instruction_reporting)
			if(memory_access_reporting_import)
				memory_access_reporting_import->ReportFinishedInstruction(getRegPC());

		if(HasAsynchronousInterrupt())
		{
			throw AsynchronousException();
		}

	}
	catch (AsynchronousException& exc) { HandleException(exc); }
	catch (NonMaskableAccessErrorInterrupt& exc) { HandleException(exc); }
	catch (NonMaskableSWIInterrupt& exc) { HandleException(exc); }
	catch (TrapException& exc) { HandleException(exc); }
	catch (SysCallInterrupt& exc) { HandleException(exc); }
	catch (SpuriousInterrupt& exc) { HandleException(exc); }
	catch(Exception& e)
	{
		if(debug_enabled && verbose_step)
			*logger << DebugError << "uncaught processor exception :" << e.what() << std::endl << EndDebugError;
		Stop(1);
	}

	if (instruction_counter >= max_inst) Stop(0);

	return opCycles;
}

//=====================================================================
//=              HCS12X (CPU12X) Queue Handling                       =
//=====================================================================

uint8_t* CPU::queueFetch(address_t addr, uint8_t* ins, uint8_t nByte)
{

	if (nByte > QUEUE_SIZE) return NULL;

	if (addr != queueCurrentAddress)
	{
		queueFill(addr, 0, QUEUE_SIZE);
		queueFirst = 0;
		queueNElement = QUEUE_SIZE;
		queueCurrentAddress = addr;
	}
	else if (nByte > queueNElement)
	{
		queueFill(addr+queueNElement, (queueFirst+queueNElement) % QUEUE_SIZE, QUEUE_SIZE-queueNElement);
		queueNElement = QUEUE_SIZE;
	}

	for (uint8_t i=0; i < nByte; i++)
	{
		ins[i] = queueBuffer[(queueFirst + i) % QUEUE_SIZE];
	}

	return ins;
}

void CPU::queueFill(address_t addr, int position, uint8_t nByte)
{
	uint8_t buff[QUEUE_SIZE];

	MMC_DATA mmc_data;

	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = buff;
	mmc_data.data_size = nByte;

	BusRead(addr, &mmc_data, sizeof(MMC_DATA));

	for (uint8_t i=0; i<nByte; i++)
	{
		queueBuffer[position] = buff[i];
		position = (position + 1) % QUEUE_SIZE;
	}

}

void CPU::queueFlush(uint8_t nByte)
{
	queueFirst = (queueFirst + nByte) % QUEUE_SIZE;
	queueNElement = queueNElement - nByte;
	queueCurrentAddress = queueCurrentAddress + nByte;
}

//=====================================================================
//=                    Exception handling methods                     =
//=====================================================================

// compute return address, save the CPU registers and then set I/X bit before the interrupt handling began
void CPU::PrepareInterrupt() {
	/*
	 * *** Save context sequence for CPU12X ***
	 *
	 * - The instruction queue is refilled
	 * - The return address is calculated
	 * - Save the return address and CPU registers as follow:
	 *    . M(SP+8) <= RTNH:RTNL
	 *    . M(SP+6) <= YH:YL
	 *    . M(SP+4) <= XH:XL
	 *    . M(SP+2) <= B:A
	 *    . M(SP)   <= CCRH:CCRL
	 */

	setRegSP(getRegSP()-2);
	memWrite16(getRegSP(), getRegPC());
	setRegSP(getRegSP()-2);
	memWrite16(getRegSP(), getRegY());
	setRegSP(getRegSP()-2);
	memWrite16(getRegSP(), getRegX());
	setRegSP(getRegSP()-2);
	memWrite16(getRegSP(), getRegD());
	setRegSP(getRegSP()-2);
	memWrite16(getRegSP(), ccr->getCCR());

}

// AsynchronousException
void CPU::HandleException(const AsynchronousException& exc)
{

	uint8_t newIPL = ccr->getIPL();
	address_t asyncVector = GetIntVector(newIPL);

	asynchronous_interrupt = false;

	if (CONFIG::HAS_RESET && HasReset())
		HandleResetException(asyncVector);

	if (CONFIG::HAS_NON_MASKABLE_XIRQ_INTERRUPT && HasNonMaskableXIRQInterrupt() && (ccr->getX() == 0))
		HandleNonMaskableXIRQException(asyncVector, newIPL);

	if (CONFIG::HAS_MASKABLE_IBIT_INTERRUPT && HasMaskableIbitInterrup() && /*(ccr->getX() == 0) &&*/ (ccr->getI() == 0))
		HandleMaskableIbitException(asyncVector, newIPL);

	// It is necessary to call AckAsynchronousInterrupt() if XIRQ and I-bit interrupt are masked
	AckAsynchronousInterrupt();
}

void CPU::AckAsynchronousInterrupt()
{
	if (CONFIG::HAS_RESET)  asynchronous_interrupt |= reset;
	if (CONFIG::HAS_NON_MASKABLE_XIRQ_INTERRUPT)  asynchronous_interrupt |= nonMaskableXIRQ_interrupt;
	if (CONFIG::HAS_MASKABLE_IBIT_INTERRUPT) asynchronous_interrupt |= maskableIbit_interrupt;

}

void CPU::ReqAsynchronousInterrupt()
{
	asynchronous_interrupt = true;
}

// Hardware and Software reset
void CPU::HandleResetException(address_t resetVector)
{

	AckReset();

	this->Reset();
	// clear U-bit for CPU12XV2
	ccr->clrIPL();
	SetEntryPoint(memRead16(resetVector));

}

void CPU::AckReset()
{
	reset = false;
	AckAsynchronousInterrupt();
}

void CPU::ReqReset()
{
	reset = true;
}

// NonMaskable XIRQ (X bit) interrupts
void CPU::HandleNonMaskableXIRQException(address_t xirqVector, uint8_t newIPL)
{
	AckXIRQInterrupt();

	PrepareInterrupt();

	ccr->setI();
	// clear U-bit for CPU12XV2
	ccr->setIPL(newIPL);
	ccr->setX();

	SetEntryPoint(memRead16(xirqVector));

}

void CPU::AckXIRQInterrupt()
{
	nonMaskableXIRQ_interrupt = false;
	AckAsynchronousInterrupt();
}

void CPU::ReqXIRQInterrupt()
{
	if (ccr->getX() == 0) nonMaskableXIRQ_interrupt = true;
}

// Maskable (I bit) interrupt
void CPU::HandleMaskableIbitException(address_t ibitVector, uint8_t newIPL)
{
	AckIbitInterrupt();

	if (newIPL > ccr->getIPL()) {
		PrepareInterrupt();

		ccr->setI();
		// clear U-bit for CPU12XV2
		ccr->setIPL(newIPL);

		SetEntryPoint(memRead16(ibitVector));
	}

}

void CPU::AckIbitInterrupt()
{
	maskableIbit_interrupt = false;
	AckAsynchronousInterrupt();
}

void CPU::ReqIbitInterrupt()
{
	uint8_t iBit = ccr->getI();

	if (iBit == 0) maskableIbit_interrupt = true;
}

// Unimplemented opcode trap
void CPU::HandleException(const TrapException& exc)
{
	ReqTrapInterrupt();

	uint8_t newIPL = ccr->getIPL();
	address_t trapVector = GetIntVector(newIPL);

	PrepareInterrupt();

	ccr->setI();
	// clear U-bit for CPU12XV2

	SetEntryPoint(memRead16(trapVector));

	AckTrapInterrupt();
}

void CPU::AckTrapInterrupt()
{
	trap_interrupt = false;
}

void CPU::ReqTrapInterrupt()
{
	trap_interrupt = true;
}

// A software interrupt instruction (SWI) or BDM vector request
void CPU::HandleException(const NonMaskableSWIInterrupt& exc)
{
	ReqSWIInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t swiVector = GetIntVector(newIPL);

	PrepareInterrupt();

	ccr->setI();
	// clear U-bit for CPU12XV2

	SetEntryPoint(memRead16(swiVector));

	AckSWIInterrupt();
}

void CPU::AckSWIInterrupt()
{
	nonMascableSWI_interrupt = false;
}

void CPU::ReqSWIInterrupt()
{
	nonMascableSWI_interrupt = true;
}

// A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)
void CPU::HandleException(const SysCallInterrupt& exc)
{
	ReqSysInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t sysCallVector = GetIntVector(newIPL);

	PrepareInterrupt();

	ccr->setI();
	// clear U-bit for CPU12XV2

	SetEntryPoint(memRead16(sysCallVector));

	AckSysInterrupt();
}

void CPU::AckSysInterrupt()
{
	syscall_interrupt = false;
}

void CPU::ReqSysInterrupt()
{
	syscall_interrupt = true;
}

// A spurious interrupt
void CPU::HandleException(const SpuriousInterrupt& exc)
{
	ReqSpuriousInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t spuriousVector = GetIntVector(newIPL);

	PrepareInterrupt();

	ccr->setI();
	// clear U-bit for CPU12XV2

	SetEntryPoint(memRead16(spuriousVector));

	AckSpuriousInterrupt();
}

void CPU::AckSpuriousInterrupt()
{
	spurious_interrupt = false;
}

void CPU::ReqSpuriousInterrupt()
{
	spurious_interrupt = true;
}

// NonMaskable Access Error interrupts
void CPU::HandleException(const NonMaskableAccessErrorInterrupt& exc)
{
	ReqAccessErrorInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t accessErrorVector = GetIntVector(newIPL);

	SetEntryPoint(memRead16(accessErrorVector));

	AckAccessErrorInterrupt();

}

void CPU::AckAccessErrorInterrupt()
{
	nonMaskableAccessError_interrupt = false;
	AckAsynchronousInterrupt();
}

void CPU::ReqAccessErrorInterrupt()
{
	nonMaskableAccessError_interrupt = true;
	ReqAsynchronousInterrupt();
}


// ======================================================
// =    Transfer/Exchange: Registers setters/getters    =
// ======================================================

template <class T>
void EBLB::setter(uint8_t rr, T val) // setter function
	/* Legal "rr" value for setter and getter functions
	 * 0x00:A; 0x01:B;
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh; 0x23:CCRWord
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP
	 */
{
	switch (rr) {
		case EBLBRegs::A: {
			cpu->setRegA((uint8_t) val);
		} break;
		case EBLBRegs::B: {
			cpu->setRegB((uint8_t) val);
		} break;
		case EBLBRegs::CCR: {
			cpu->ccr->setCCRLow((uint8_t) val);
		} break;
		case EBLBRegs::CCRL: {
			cpu->ccr->setCCRLow((uint8_t) val);
		} break;
		case EBLBRegs::CCRH: {
			cpu->ccr->setCCRHigh((uint8_t) val);
		} break;
		case EBLBRegs::CCRW: {
			cpu->ccr->setCCR((uint16_t) val);
		} break;
		case EBLBRegs::TMP1: {
			cpu->setRegTMP(0, (uint16_t) val);
		} break;
		case EBLBRegs::TMP2: {
			cpu->setRegTMP(1, (uint16_t) val);
		} break;
		case EBLBRegs::TMP3: {
			cpu->setRegTMP(2, (uint16_t) val);
		} break;
		case EBLBRegs::D: {
			cpu->setRegD((uint16_t) val);
		} break;
		case EBLBRegs::X: {
			cpu->setRegX((uint16_t) val);
		} break;
		case EBLBRegs::Y: {
			cpu->setRegY((uint16_t) val);
		} break;
		case EBLBRegs::SP: {
			cpu->setRegSP((uint16_t) val);
		} break;
		default:;
	}
}

template <class T>
T EBLB::getter(uint8_t rr) // getter function
	/* Legal "rr" value for setter and getter functions
	 * 0x00:A; 0x01:B;
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh; 0x23:CCRWord
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP
	 */
{
	switch (rr) {
		case EBLBRegs::A: {
			return (uint8_t) cpu->getRegA();
		} break;
		case EBLBRegs::B: {
			return (uint8_t) cpu->getRegB();
		} break;
		case EBLBRegs::CCR: {
			return (uint8_t) cpu->ccr->getCCRLow();
		} break;
		case EBLBRegs::CCRL: {
			return (uint8_t) cpu->ccr->getCCRLow();
		} break;
		case EBLBRegs::CCRH: {
			return (uint8_t) cpu->ccr->getCCRHigh();
		} break;
		case EBLBRegs::CCRW: {
			return (uint16_t) cpu->ccr->getCCR();
		} break;
		case EBLBRegs::TMP1: {
			return (uint16_t) cpu->getRegTMP(0);
		} break;
		case EBLBRegs::TMP2: {
			return (uint16_t) cpu->getRegTMP(1);
		} break;
		case EBLBRegs::TMP3: {
			return (uint16_t) cpu->getRegTMP(2);
		} break;
		case EBLBRegs::D: {
			return (uint16_t) cpu->getRegD();
		} break;
		case EBLBRegs::X: {
			return (uint16_t) cpu->getRegX();
		} break;
		case EBLBRegs::Y: {
			return (uint16_t) cpu->getRegY();
		} break;
		case EBLBRegs::SP: {
			return (uint16_t) cpu->getRegSP();
		} break;
		default:;
	}
	throw std::runtime_error("Internal error");
}

template <class T>
void EBLB::exchange(uint8_t rrSrc, uint8_t rrDst) {
	T tmp = getter<T>(rrSrc);
	setter<T>(rrSrc, getter<T>(rrDst));
	setter<T>(rrDst, tmp);
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CPU::BeginSetup() {

	/* check verbose settings */
	if(debug_enabled && verbose_all) {
		verbose_setup = true;
		verbose_step = true;
		verbose_dump_regs_start = true;
		verbose_dump_regs_end = true;
	}
	if(debug_enabled && verbose_all) {
		*logger << DebugInfo
			<< "verbose-all = true"
			<< std::endl << EndDebugInfo;
	} else {
		if(debug_enabled && verbose_setup)
			*logger << DebugInfo
				<< "verbose-setup = true"
				<< std::endl << EndDebugInfo;
		if(debug_enabled && verbose_step)
			*logger << DebugInfo
				<< "verbose-step = true"
				<< std::endl << EndDebugInfo;
		if(debug_enabled && verbose_dump_regs_start)
			*logger << DebugInfo
				<< "verbose-dump-regs-end = true"
				<< std::endl << EndDebugInfo;
		if(debug_enabled && verbose_dump_regs_end)
			*logger << DebugInfo
				<< "verbose-dump-regs-start = true"
				<< std::endl << EndDebugInfo;
	}

	/* setting debugging registers */
	if(verbose_setup)
		*logger << DebugInfo
			<< "Initializing debugging registers"
			<< std::endl << EndDebugInfo;

	return true;
}

bool CPU::Setup(ServiceExportBase *srv_export) {

	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}

	return true;
}

bool CPU::EndSetup() {
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

/**************************************************************/
/* Verbose methods (protected)                          START */
/**************************************************************/

inline INLINE
bool CPU::VerboseSetup() {
	return debug_enabled && verbose_setup;
}

inline INLINE
bool CPU::VerboseStep() {
	return debug_enabled && verbose_step;
}

inline INLINE
void CPU::VerboseDumpRegs() {

	*logger << "\t- A" << " = 0x" << std::hex << getRegA() << std::dec;
	*logger << "\t- B" << " = 0x" << std::hex << getRegB() << std::dec;
	*logger << "\t- D" << " = 0x" << std::hex << getRegD() << std::dec;
	*logger << std::endl;
	*logger << "\t- X" << " = 0x" << std::hex << getRegX() << std::dec;
	*logger << "\t- Y" << " = 0x" << std::hex << getRegY() << std::dec;
	*logger << std::endl;
	*logger << "\t- SP" << " = 0x" << std::hex << getRegSP() << std::dec;
	*logger << "\t- PC" << " = 0x" << std::hex << getRegPC() << std::dec;
	*logger << std::endl;

}

inline INLINE
void CPU::VerboseDumpRegsStart() {
	if(debug_enabled && verbose_dump_regs_start) {
		*logger << DebugInfo
			<< "Register dump before starting instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

inline INLINE
void CPU::VerboseDumpRegsEnd() {
	if(debug_enabled && verbose_dump_regs_end) {
		*logger << DebugInfo
			<< "Register dump at the end of instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

inline INLINE
void CPU::RegistersInfo() {

	if (debug_enabled) {
		cout << "CPU:: Registers Info " << std::endl;
		cout << std::hex << "CCR=0x" << ccr->getCCR() << "  PC=0x" << getRegPC() << "  SP=0x" << getRegSP() << "\n";
		cout << "D  =0x" << getRegD() << "  X =0x" << getRegX() << "  Y =0x" << getRegY() << std::dec << "\n";
	}
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool CPU::ReadMemory(service_address_t addr, void *buffer, uint32_t size)
{
	if (memory_import) {
		return memory_import->ReadMemory(addr, (uint8_t *) buffer, size);
	}

	return false;
}

bool CPU::WriteMemory(service_address_t addr, const void *buffer, uint32_t size)
{
	if (size == 0) {
		return true;
	}

	if (memory_import) {
		return memory_import->WriteMemory(addr, (uint8_t *) buffer, size);
	}

	return false;
}

//=====================================================================
//=             CPURegistersInterface interface methods               =
//=====================================================================

string CPU::GetObjectFriendlyName(service_address_t addr)
{
	stringstream sstr;

	const Symbol<service_address_t> *symbol = NULL;

	if (symbol_table_lookup_import) {
		symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<service_address_t>::SYM_OBJECT);
	}

	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

string CPU::GetFunctionFriendlyName(service_address_t addr)
{
	stringstream sstr;

	const Symbol<service_address_t> *symbol = NULL;

	if (symbol_table_lookup_import) {
		symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<service_address_t>::SYM_FUNC);
	}

	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
Register* CPU::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;

}

//=====================================================================
//=                   DebugDisasmInterface methods                    =
//=====================================================================

/**
 * Returns a string with the disassembling of the instruction found
 *   at address addr.
 *
 * @param addr The address of the instruction to disassemble.
 * @param next_addr The address following the requested instruction.
 * @return The disassembling of the requested instruction address.
 */
string CPU::Disasm(service_address_t service_addr, service_address_t &next_addr)
{
	Operation *op = NULL;

	uint8_t 	buffer[CodeType::capacity];

	ReadMemory(service_addr, buffer, CodeType::capacity);
	CodeType 	insn( buffer, CodeType::capacity*8 );

	op = this->Decode(service_addr, insn);

	stringstream disasmBuffer;
	op->disasm(disasmBuffer);

        unsigned int insn_length = op->GetLength();
        if (insn_length % 8) throw "InternalError";
	next_addr = service_addr + (insn_length/8);

	return disasmBuffer.str();
}

} // end namespace
}
}
}
} // end namespace hcs12x
