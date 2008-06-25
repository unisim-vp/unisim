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
#include "unisim/util/debug/simple_register.hh"

#define LOCATION Function << __FUNCTION__ << File <<  __FILE__ << Line << __LINE__


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

using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;
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
	Client<DebugControl<physical_address_t> >(name, parent),
	Client<MemoryAccessReporting<physical_address_t> >(name, parent),
	Service<MemoryAccessReportingControl>(name, parent),
	Service<Disassembly<physical_address_t> >(name, parent),
	Service<Registers>(name, parent),
	Service<Memory<physical_address_t> >(name, parent),
	Client<Memory<physical_address_t> >(name, parent),
	Client<SymbolTableLookup<physical_address_t> >(name, parent),
	Client<Logger>(name, parent),
	disasm_export("disasm_export", this),
	registers_export("registers_export", this),
	memory_access_reporting_control_export("memory_access_reporting_control_export", this),
	memory_export("memory_export", this),
	debug_control_import("debug_control_import", this),
	memory_access_reporting_import("memory_access_reporting_import", this),
	symbol_table_lookup_import("symbol_table_lookup_import", this),
	memory_import("memory_import", this),
	logger_import("logger_import", this),
//	itcm_logger_import("itcm_logger_import", this),
//	dtcm_logger_import("dtcm_logger_import", this),
	requires_memory_access_reporting(true),
	requires_finished_instruction_reporting(true),
//	default_endianess(E_BIG_ENDIAN),
//	param_default_endianess("default-endianess", this, default_endianess),
//	verbose_all(true),
	verbose_all(false),
	verbose_exception(false),
//	param_verbose_all("verbose-all", this, verbose_all),
	verbose_setup(false),
//	param_verbose_setup("verbose-setup", this, verbose_setup),
	verbose_step(true),
//	param_verbose_step("verbose-step", this, verbose_step),
//	verbose_step_insn(false),
//	param_verbose_step_insn("verbose-step-insn", this, verbose_step_insn),
	verbose_dump_regs_start(true),
//	param_verbose_dump_regs_start("verbose-dump-regs-start", this, verbose_dump_regs_start),
	verbose_dump_regs_end(true),
//	param_verbose_dump_regs_end("verbose-dump-regs-end", this, verbose_dump_regs_end),
//	memory_interface(_memory_interface),
	instruction_counter(0),
//	running(true),
	flash_mode(false)
	
{
	setRegA(0x00);
	setRegB(0x00);	
    setRegX(0x0000);
    setRegY(0x0000);

	//TODO: intitialize segment register 
//    setRegSP(0xFE00);

    ccr = new CCR_t();
    
    mmc = new MMC(CONFIG::GLOBAL_RESET_PAGE, CONFIG::RAM_RESET_PAGE, CONFIG::EEPROM_RESET_PAGE, CONFIG::FLASH_RESET_PAGE, CONFIG::DIRECT_RESET_PAGE);
  
    eblb = new EBLB(this);

}

CPU::~CPU() 
{ 
	delete mmc; mmc = NULL;
	delete eblb; eblb = NULL;
	delete ccr; ccr = NULL;
}

void CPU::SetStartAddress(uint16_t page, address_t entry_point)
{
    setRegPC(entry_point);
    if (flash_mode) {
    	mmc->setPpage((uint8_t) page);
    }
}

void CPU::SetFlashMode(bool mode)
{
	flash_mode = mode;
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CPU::Setup()
{
	/* check verbose settings */
	if(CONFIG::DEBUG_ENABLE && verbose_all) {
		verbose_setup = true;
		verbose_step = true;
		verbose_dump_regs_start = true;
		verbose_dump_regs_end = true;
	}
	if(CONFIG::DEBUG_ENABLE && verbose_all && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "verbose-all = true"
			<< Endl << EndDebugInfo;
	} else {
		if(CONFIG::DEBUG_ENABLE && verbose_setup && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-setup = true"
				<< Endl << EndDebugInfo;
		if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-step = true"
				<< Endl << EndDebugInfo;
		if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_start && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-dump-regs-end = true"
				<< Endl << EndDebugInfo;
		if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_end && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-dump-regs-start = true"
				<< Endl << EndDebugInfo;
	}
	
	/* setting debugging registers */
	if(verbose_setup && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Initializing debugging registers"
			<< Endl << EndDebugInfo;

	registers_registry["A"] = new SimpleRegister<uint8_t>("A", &regA);
	registers_registry["B"] = new SimpleRegister<uint8_t>("B", &regB);
	registers_registry["D"] = new ConcatenatedRegister<uint16_t,uint8_t>("D", &regA, &regB);
	registers_registry["X"] = new SimpleRegister<uint16_t>("X", &regX);
	registers_registry["Y"] = new SimpleRegister<uint16_t>("Y", &regY);	
	registers_registry["SP"] = new SimpleRegister<uint16_t>("SP", &regSP);
	registers_registry["PC"] = new SimpleRegister<uint16_t>("PC", &regPC);
	registers_registry["CCR"] = new SimpleRegister<CCR_t>("CCR", ccr);		// ici il faut une <class BitFieldRegister>

	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}
	
//	if(statistics_import) {
//		statistics_import->AddSource(Object::GetName(), statistics_id);
//		statistics_import->AddStatistic(statistics_id, "instruction-counter", 
//				&instruction_counter);
//	}
	
	return true;
}

void CPU::OnDisconnect()
{
}

//=====================================================================
//=                    execution handling methods                     =
//=====================================================================

void CPU::OnBusCycle()
{
}

void CPU::Step()
{
	address_t 	current_pc;
	physical_address_t physical_pc;

	uint8_t 	buffer[CodeType::maxsize]; 
	Operation 	*op;

	current_pc = getRegPC();
	physical_pc = current_pc;
	
	if (flash_mode) {
		physical_pc = mmc->getPhysicalAddress(current_pc, MEMORY::FLASH, WO_GLOBAL_ADDRESS);
	}

	VerboseDumpRegsStart();
	
	if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Starting step at PC = 0x"
			<< Hex << physical_pc << Dec
			<< Endl << EndDebugInfo;
	
	if(debug_control_import) {
		DebugControl<physical_address_t>::DebugCommand dbg_cmd;

		do {
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Fetching debug command (PC = 0x"
					<< Hex << physical_pc << Dec << ")"
					<< Endl << EndDebugInfo;
			dbg_cmd = debug_control_import->FetchDebugCommand(physical_pc);
	
			if(dbg_cmd == DebugControl<physical_address_t>::DBG_STEP) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_STEP command (PC = 0x"
						<< Hex << physical_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				break;
			}
			if(dbg_cmd == DebugControl<physical_address_t>::DBG_SYNC) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_SYNC command (PC = 0x"
						<< Hex << physical_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				Sync();
				continue;
			}

			if(dbg_cmd == DebugControl<physical_address_t>::DBG_KILL) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_KILL command (PC = 0x"
						<< Hex << physical_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				Stop(0);
			}
			if(dbg_cmd == DebugControl<physical_address_t>::DBG_RESET) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_RESET command (PC = 0x"
						<< Hex << physical_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				// TODO : memory_interface->Reset(); 
			}
		} while(1);
	}

	if(requires_memory_access_reporting) {
		if(memory_access_reporting_import) {
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Reporting memory acces for fetch at address 0x"
					<< Hex << physical_pc << Dec
					<< Endl << EndDebugInfo;
/*
			uint32_t insn_size;
			if(GetCPSR_T())
				insn_size = 2;
			else
				insn_size = 4;
				
			memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ, 
					MemoryAccessReporting<address_t>::MT_INSN, 
					physical_pc, insn_size);
*/
		}
	}


	if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
	{
		(*logger_import) << DebugInfo << LOCATION
			<< "Fetching (reading) instruction at address 0x"
			<< Hex << physical_pc << Dec
			<< Endl << EndDebugInfo;
	}

	BusRead(physical_pc, buffer, CodeType::maxsize);
	CodeType 	insn( buffer, CodeType::maxsize);
	
	/* Decode current PC */
	if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
	{
		stringstream ctstr;
		ctstr << insn;
		(*logger_import) << DebugInfo << LOCATION
			<< "Decoding instruction at 0x"
			<< Hex << physical_pc << Dec
			<< " (0x" << Hex << ctstr.str() << Dec << ")"
			<< Endl << EndDebugInfo;
	}

	op = this->Decode(current_pc, insn);
	setRegPC(current_pc+op->GetEncoding().size);
		
	/* Execute instruction */

	if(logger_import) {
		stringstream disasm_str;
		stringstream ctstr;
		
		op->disasm(disasm_str);
		
		ctstr << insn;
		(*logger_import) << DebugInfo << LOCATION
			<< "Executing instruction "
			<< disasm_str.str()
			<< " at 0x" << Hex << physical_pc << Dec
			<< " (0x" << Hex << ctstr.str() << Dec << ", " << instruction_counter << ")"
			<< Endl << EndDebugInfo;
	}

	try 
	{

		op->execute(this);
		
	//	/* perform the memory load/store operations */
	//	PerformLoadStoreAccesses();
		VerboseDumpRegsEnd();
		
		instruction_counter++;

		RegistersInfo();
	
	/*		
		if(requires_finished_instruction_reporting)
			if(memory_access_reporting_import)
				memory_access_reporting_import->ReportFinishedInstruction(GetGPR(PC_reg));
	*/	

		if(requires_finished_instruction_reporting)
			if(memory_access_reporting_import)
				memory_access_reporting_import->ReportFinishedInstruction(getRegPC());


		if(HasAsynchronousInterrupt())
		{
			if(CONFIG::HAS_HARD_RESET && HasHardReset()) throw ResetException();
			if(CONFIG::HAS_SOFT_RESET && HasSoftReset()) throw ResetException();
			if(CONFIG::HAS_NON_MASKABLE_XIRQ_INTERRUPT && HasNonMaskableXIRQInterrupt()) throw NonMaskableXIRQInterrupt();
			if(CONFIG::HAS_MASKABLE_INTERRUPT && HasMaskableInterrupt()) throw MaskableInterrupt();
			
		}
	}
	catch(ResetException& exc) { HandleException(exc); }
	catch(NonMaskableXIRQInterrupt& exc) { HandleException(exc); }
	catch(MaskableInterrupt& exc) { HandleException(exc); }
	catch(Exception& e)
	{
		if(logger_import)
			(*logger_import) << DebugError << "uncaught processor exception :" << e.what() << Endl << EndDebugError;
		Stop(1);
	}
	
}

void CPU::Stop(int ret)
{
	exit(ret);
}

void CPU::Sync()
{

}


//=====================================================================
//=                    Exception handling methods                     =
//=====================================================================

void CPU::ReqAsynchronousInterrupt()
{
	asynchronous_interrupt = true;
}

void CPU::AckAsynchronousInterrupt()
{
	asynchronous_interrupt = false;
	
	if(CONFIG::HAS_SOFT_RESET) asynchronous_interrupt |= soft_reset;
	if(CONFIG::HAS_HARD_RESET) asynchronous_interrupt |= hard_reset;
	if(CONFIG::HAS_NON_MASKABLE_XIRQ_INTERRUPT) asynchronous_interrupt |= nonMaskableXIRQ_interrupt;
	if(CONFIG::HAS_MASKABLE_INTERRUPT) asynchronous_interrupt |= maskable_interrupt;

}

// Hardware and Software reset
void CPU::HandleException(const ResetException& exc)
{
	// TODO:
}

void CPU::ReqHardReset()
{
	hard_reset = true;
	ReqAsynchronousInterrupt();
}

void CPU::AckHardReset()
{
	hard_reset = false;
	AckAsynchronousInterrupt();
}

void CPU::ReqSoftReset()
{
	soft_reset = true;
	ReqAsynchronousInterrupt();
}

void CPU::AckSoftReset()
{
	soft_reset = false;
	AckAsynchronousInterrupt();
}

// Unimplemented opcode trap 
void CPU::HandleException(const TrapException& exc)
{
	// TODO
}

// A software interrupt instruction (SWI) or BDM vector request 
void CPU::HandleException(const SoftwareInterrupt& exc)
{
	// TODO
}

// A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)
void CPU::HandleException(const SysCallInterrupt& exc)
{
	// TODO
}

// Non-maskable (X bit) interrupts
void CPU::HandleException(const NonMaskableXIRQInterrupt& exc)
{
	// TODO
}

void CPU::ReqNonMaskableAccessErrorInterrupt()
{
	nonMaskableXIRQ_interrupt = true;
	// TODO
}

void CPU::AckNonMaskableAccessErrorInterrupt()
{
	nonMaskableXIRQ_interrupt = false;
	// TODO
}

// Maskable (I bit) interrupt
void CPU::HandleException(const MaskableInterrupt& exc)
{
	// TODO
}

void CPU::ReqMaskableInterrupt()
{
	maskable_interrupt = true;
	// TODO
}

void CPU::AckMaskableInterrupt()
{
	maskable_interrupt = false;
	// TODO
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
	return CONFIG::DEBUG_ENABLE && verbose_setup && logger_import;
}

inline INLINE
bool CPU::VerboseStep() {
	return CONFIG::DEBUG_ENABLE && verbose_step && logger_import;
}

inline INLINE
void CPU::VerboseDumpRegs() {

	(*logger_import) << "\t- A" << " = 0x" << Hex << getRegA() << Dec;
	(*logger_import) << "\t- B" << " = 0x" << Hex << getRegB() << Dec;
	(*logger_import) << "\t- D" << " = 0x" << Hex << getRegD() << Dec;
	(*logger_import) << Endl;
	(*logger_import) << "\t- X" << " = 0x" << Hex << getRegX() << Dec;
	(*logger_import) << "\t- Y" << " = 0x" << Hex << getRegY() << Dec;
	(*logger_import) << Endl;
	(*logger_import) << "\t- SP" << " = 0x" << Hex << getRegSP() << Dec;
	(*logger_import) << "\t- PC" << " = 0x" << Hex << getRegPC() << Dec;
	(*logger_import) << Endl;

}

inline INLINE
void CPU::VerboseDumpRegsStart() {
	if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_start && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Register dump before starting instruction execution: " << Endl;
		VerboseDumpRegs();
		(*logger_import) << EndDebugInfo;
	}
}

inline INLINE
void CPU::VerboseDumpRegsEnd() {
	if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_end && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Register dump at the end of instruction execution: " << Endl;
		VerboseDumpRegs();
		(*logger_import) << EndDebugInfo;
	}
}

inline INLINE
void CPU::RegistersInfo() {

	if (CONFIG::REGISTERS_INFO) {
		cout << "CCR=0x" << std::hex << ccr->getCCR() << "  PC=0x" << getRegPC() << "  SP=0x" << getRegSP() << "\n";
		cout << "D  =0x" << getRegD()     << "  X =0x" << getRegX() << "  Y =0x" << getRegY() << "\n";
	}
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================
void CPU::Reset()
{
	//TODO
	bus_cycle = 0;
	cpu_cycle = 0;
	instruction_counter = 0;

	soft_reset = false;
	hard_reset = false;
	nonMaskableXIRQ_interrupt = false;
	maskable_interrupt = false;
	
	asynchronous_interrupt = false;
}

bool CPU::ReadMemory(physical_address_t addr, void *buffer, uint32_t size)
{
	if (memory_import) {
		return memory_import->ReadMemory(addr, (uint8_t *) buffer, size);
	}
	
	return false;
}

bool CPU::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size)
{
	if (memory_import) {
		return memory_import->WriteMemory(addr, (uint8_t *) buffer, size);
	}
	
	return false;
}

/* ********** MEMORY ACCESS ROUTINES ******* */

uint8_t CPU::memRead8(physical_address_t addr) {

	uint8_t data;
	BusRead(addr, &data, 1);
	return data;
}

uint16_t CPU::memRead16(physical_address_t addr) {

	uint16_t data;
	BusRead(addr, &data, 2);

	return BigEndian2Host(data);
}

void CPU::memWrite8(physical_address_t addr, uint8_t val) {

	BusWrite( addr, &val, 1); 
}

void CPU::memWrite16(physical_address_t addr, uint16_t val) {

	val = Host2BigEndian(val);
	
	BusWrite(addr, &val, 2);
}

/* ********** END MEM ACCESS ROUTINES ****** */

//=====================================================================
//=             CPURegistersInterface interface methods               =
//=====================================================================

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
string CPU::Disasm(physical_address_t addr, physical_address_t &next_addr)
{
	Operation *op = NULL;
	uint8_t 	buffer[CodeType::maxsize];
	 
	ReadMemory(addr, buffer, CodeType::maxsize);
	CodeType 	insn( buffer, CodeType::maxsize);
	
	op = this->Decode(addr, insn);
	
	
	op = Decode(addr);
	stringstream disasmBuffer;
	op->disasm(disasmBuffer);

	next_addr = addr + op->GetEncoding().size;
	return disasmBuffer.str();
}

	//======================================================================
	//=                  Registers Acces Routines                          =
	//======================================================================
	
uint16_t CPU::xb_getAddrRegValue(uint8_t rr) {
    switch (rr) {
	case 0:
    	return getRegX(); break;
	case 1:
    	return getRegY(); break;
	case 2:
    	return getRegSP(); break;
	case 3:
    	return getRegPC(); break;
	}
}

void CPU::xb_setAddrRegValue(uint8_t rr,uint16_t val) {
    switch (rr) {
	case 0:
    	return setRegX(val); break;
	case 1:
    	return setRegY(val); break;
	case 2:
    	return setRegSP(val); break;
	case 3:
    	return setRegPC(val); break;
	}
}


uint16_t CPU::xb_getAccRegValue(uint8_t rr) {
    switch (rr) {
	case 0:
    	return getRegA(); break;
	case 1:
    	return getRegB(); break;
	case 2:
    	return getRegD(); break;
    default:
    	return 0; // ! or throw an exception 	
	}
}
	

void CPU::setRegA(uint8_t val) { regA = val; }
uint8_t CPU::getRegA() { return regA; }
    
void CPU::setRegB(uint8_t val) { regB = val; }    
uint8_t CPU::getRegB() { return regB; }
    
void CPU::setRegD(uint16_t val) { 
    // regD == regA:regB
	
	setRegA((uint8_t) (val >> 8));
	setRegB((uint8_t) (val & 0x00FF));

}    
    
uint16_t CPU::getRegD() { 
    // regD == regA:regB
	uint16_t val = (((uint16_t) getRegA()) << 8) | getRegB();
	return val;
}

void CPU::setRegX(uint16_t val) { regX = val; }    
uint16_t CPU::getRegX() { return regX; }

void CPU::setRegY(uint16_t val) { regY = val; }    
uint16_t CPU::getRegY() { return regY; }

void CPU::setRegSP(uint16_t val) { regSP = val; }    
uint16_t CPU::getRegSP() { return regSP; }

void CPU::setRegPC(uint16_t val) { regPC = val; }    
uint16_t CPU::getRegPC() { return regPC; }

void CPU::setRegTMP(uint8_t index, uint16_t val) { regTMP[index] = val; }
uint16_t CPU::getRegTMP(uint8_t index) { return regTMP[index]; }

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
}

template <class T> 
void EBLB::exchange(uint8_t rrSrc, uint8_t rrDst) {
	T tmp = getter<T>(rrSrc);
	setter<T>(rrSrc, getter<T>(rrDst));
	setter<T>(rrDst, tmp);
}


} // end namespace
}
}
}
} // end namespace hcs12x
