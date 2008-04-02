
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
//	param_verbose_all("verbose-all", this, verbose_all),
	verbose_setup(false),
//	param_verbose_setup("verbose-setup", this, verbose_setup),
	verbose_step(false),
//	param_verbose_step("verbose-step", this, verbose_step),
//	verbose_step_insn(false),
//	param_verbose_step_insn("verbose-step-insn", this, verbose_step_insn),
	verbose_dump_regs_start(false),
//	param_verbose_dump_regs_start("verbose-dump-regs-start", this, verbose_dump_regs_start),
	verbose_dump_regs_end(false),
//	param_verbose_dump_regs_end("verbose-dump-regs-end", this, verbose_dump_regs_end),
//	memory_interface(_memory_interface),
	instruction_counter(0)
//	running(true)
	
{
/*
#if BYTE_ORDER == LITTLE_ENDIAN
	regB = (uint8_t *) &regD;
	regA = (uint8_t *) &regD+1;
#endif
#if BYTE_ORDER == BIG_ENDIAN
	regA = (uint8_t *) &regD;
	regB = (uint8_t *) &regD+1;	
#endif
*/
	setRegA(0x00);
	setRegB(0x00);	
//    setRegD(0x0000);
    setRegX(0x0000);
    setRegY(0x0000);
    setRegSP(0xFE00);
    setRegPC(0x8000);
    
    // temporary declaration. my be in a config file or defined as constants
    uint8_t gpage=0, rpage=0, epage=0, ppage=0, direct=0;
    
    mmc = new MMC(gpage, rpage, epage, ppage, direct);
  
    ccr = new CCR_t();
    eblb = new EBLB(this);

}

CPU::~CPU() 
{ 
	delete mmc; mmc = NULL;
	delete eblb; eblb = NULL;
	delete ccr; ccr = NULL;
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
	uint16_t 	current_pc;

	uint8_t 	buffer[CodeType::maxsize]; 
	Operation 	*op;


	current_pc = getRegPC();
	

	VerboseDumpRegsStart();
	
	if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Starting step at PC = 0x"
			<< Hex << current_pc << Dec
			<< Endl << EndDebugInfo;
	
	if(debug_control_import) {
		DebugControl<physical_address_t>::DebugCommand dbg_cmd;

		do {
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Fetching debug command (PC = 0x"
					<< Hex << current_pc << Dec << ")"
					<< Endl << EndDebugInfo;
			dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);
	
			if(dbg_cmd == DebugControl<physical_address_t>::DBG_STEP) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_STEP command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				break;
			}
			if(dbg_cmd == DebugControl<physical_address_t>::DBG_SYNC) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_SYNC command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				Sync();
				continue;
			}

			if(dbg_cmd == DebugControl<physical_address_t>::DBG_KILL) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_KILL command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				Stop(0);
			}
			if(dbg_cmd == DebugControl<physical_address_t>::DBG_RESET) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_RESET command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
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
					<< Hex << current_pc << Dec
					<< Endl << EndDebugInfo;
/*
			uint32_t insn_size;
			if(GetCPSR_T())
				insn_size = 2;
			else
				insn_size = 4;
				
			memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ, 
					MemoryAccessReporting<address_t>::MT_INSN, 
					current_pc, insn_size);
*/
		}
	}


	if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
	{
		(*logger_import) << DebugInfo << LOCATION
			<< "Fetching (reading) instruction at address 0x"
			<< Hex << current_pc << Dec
			<< Endl << EndDebugInfo;
	}

	// ReadInsn(current_pc, insn);
	BusRead(current_pc, buffer, CodeType::maxsize);
	CodeType 	insn( buffer, CodeType::maxsize);
	
	/* Decode current PC */
	if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
	{
		stringstream ctstr;
		ctstr << insn;
		(*logger_import) << DebugInfo << LOCATION
			<< "Decoding instruction at 0x"
			<< Hex << current_pc << Dec
			<< " (0x" << Hex << ctstr.str() << Dec << ")"
			<< Endl << EndDebugInfo;
	}

	op = this->Decode(current_pc, insn);
	
	/* Execute instruction */

	if(logger_import) {
		stringstream disasm_str;
		stringstream ctstr;
		
		op->disasm(disasm_str);
		
		ctstr << insn;
		(*logger_import) << DebugInfo << LOCATION
			<< "Executing instruction "
			<< disasm_str.str()
			<< " at 0x" << Hex << current_pc << Dec
			<< " (0x" << Hex << ctstr.str() << Dec << ", " << instruction_counter << ")"
			<< Endl << EndDebugInfo;
	}

	setRegPC(current_pc+op->GetEncoding().size);
	op->execute(this);
		
//	/* perform the memory load/store operations */
//	PerformLoadStoreAccesses();
	VerboseDumpRegsEnd();
	
	instruction_counter++;

/*		
	if(requires_finished_instruction_reporting)
		if(memory_access_reporting_import)
			memory_access_reporting_import->ReportFinishedInstruction(GetGPR(PC_reg));
*/	
	
}

void CPU::Stop(int ret)
{
	exit(ret);
}

void CPU::Sync()
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

//=====================================================================
//=             memory interface methods                              =
//=====================================================================
void CPU::Reset()
{
	//TODO
}

bool CPU::ReadMemory(physical_address_t addr, void *buffer, uint32_t size)
{
	if (memory_import) {
		return memory_import->ReadMemory(addr, (uint8_t *) buffer, size);
	}
//	BusRead(addr, (uint8_t *) buffer, size);
	
	return false;
}

bool CPU::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size)
{
	if (memory_import) {
		return memory_import->WriteMemory(addr, (uint8_t *) buffer, size);
	}

//	BusWrite(addr, (uint8_t *) buffer, size);
	
	return false;
}

/* ********** MEMORY ACCESS ROUTINES ******* */

uint8_t CPU::memRead8(address_t logicalAddress, MEMORY::MAP type) {

	physical_address_t addr = mmc->getPhysicalAddress(logicalAddress, type);

	uint8_t data;
	BusRead(addr, &data, 1);
	return data;
	
}

uint16_t CPU::memRead16(address_t logicalAddress, MEMORY::MAP type) {

    physical_address_t addr = mmc->getPhysicalAddress(logicalAddress, type);

	uint16_t data;
	BusRead(addr, &data, 2);

	return BigEndian2Host(data);

}

void CPU::memWrite8(address_t logicalAddress, uint8_t val, MEMORY::MAP type) {

	physical_address_t addr = mmc->getPhysicalAddress(logicalAddress, type);

	BusWrite( addr, &val, 1); 

}

void CPU::memWrite16(address_t logicalAddress, uint16_t val, MEMORY::MAP type) {

    physical_address_t addr = mmc->getPhysicalAddress(logicalAddress, type);

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
    	return getRegX();
	case 1:
    	return getRegY();
	case 2:
    	return getRegSP();
	case 3:
    	return getRegPC();    
	}
}

void CPU::xb_setAddrRegValue(uint8_t rr,uint16_t val) {
    switch (rr) {
	case 0:
    	return setRegX(val);
	case 1:
    	return setRegY(val);
	case 2:
    	return setRegSP(val);
	case 3:
    	return setRegPC(val);    
	}
}


uint16_t CPU::xb_getAccRegValue(uint8_t rr) {
    switch (rr) {
	case 0:
    	return getRegA();
	case 1:
    	return getRegB();
	case 2:
    	return getRegD();
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

	uint16_t value = Host2BigEndian(val);
	
	setRegA((uint8_t) (value >> 8));
	setRegB((uint8_t) (value & 0x00FF));

}    
    
uint16_t CPU::getRegD() { 
    // regD == regA:regB
	uint16_t val = (((uint16_t) getRegA()) << 8) | getRegB();
    return BigEndian2Host(val); 
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
		case EBLBRegs::A: cpu->setRegA((uint8_t) val); break;
		case EBLBRegs::B: cpu->setRegB((uint8_t) val); break;
		case EBLBRegs::CCR: cpu->ccr->setCCRLow((uint8_t) val); break;
		case EBLBRegs::CCRL: cpu->ccr->setCCRLow((uint8_t) val); break;
		case EBLBRegs::CCRH: cpu->ccr->setCCRHigh((uint8_t) val); break;
		case EBLBRegs::CCRW: cpu->ccr->setCCR((uint16_t) val); break;
		case EBLBRegs::TMP1: cpu->setRegTMP(0, (uint16_t) val); break;
		case EBLBRegs::TMP2: cpu->setRegTMP(1, (uint16_t) val); break;
		case EBLBRegs::TMP3: cpu->setRegTMP(2, (uint16_t) val); break;
		case EBLBRegs::D: cpu->setRegD((uint16_t) val); break;
		case EBLBRegs::X: cpu->setRegX((uint16_t) val); break;
		case EBLBRegs::Y: cpu->setRegY((uint16_t) val); break;
		case EBLBRegs::SP: cpu->setRegSP((uint16_t) val); break;
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
		case EBLBRegs::A: return (uint8_t) cpu->getRegA(); break;
		case EBLBRegs::B: return (uint8_t) cpu->getRegB(); break;
		case EBLBRegs::CCR: return (uint8_t) cpu->ccr->getCCRLow(); break;
		case EBLBRegs::CCRL: return (uint8_t) cpu->ccr->getCCRLow(); break;
		case EBLBRegs::CCRH: return (uint8_t) cpu->ccr->getCCRHigh(); break;
		case EBLBRegs::CCRW: return (uint16_t) cpu->ccr->getCCR(); break;
		case EBLBRegs::TMP1: return (uint16_t) cpu->getRegTMP(0); break;
		case EBLBRegs::TMP2: return (uint16_t) cpu->getRegTMP(1); break;
		case EBLBRegs::TMP3: return (uint16_t) cpu->getRegTMP(2); break;
		case EBLBRegs::D: return (uint16_t) cpu->getRegD(); break;
		case EBLBRegs::X: return (uint16_t) cpu->getRegX(); break;
		case EBLBRegs::Y: return (uint16_t) cpu->getRegY(); break;
		case EBLBRegs::SP: return (uint16_t) cpu->getRegSP(); break;
	}
}

template <class T> 
void EBLB::exchange(uint8_t rrSrc, uint8_t rrDst) {
	T tmp = getter<T>(rrSrc);
	setter<T>(rrSrc, getter<T>(rrDst));
	setter<T>(rrDst, tmp);
}

uint8_t CCR_t::getC() { return c;};
void 	CCR_t::setC(uint8_t val) { c == val;};            

uint8_t CCR_t::getV() { return v;};
void 	CCR_t::setV(uint8_t val) { v == val;};            

uint8_t CCR_t::getZ() { return z;};
void 	CCR_t::setZ(uint8_t val) { z == val;};            

uint8_t CCR_t::getN() { return n;};
void 	CCR_t::setN(uint8_t val) { n == val;};            

uint8_t CCR_t::getI() { return i;};
void 	CCR_t::setI(uint8_t val) { i == val;};            

uint8_t CCR_t::getH() { return h;};
void 	CCR_t::setH(uint8_t val) { h == val;};            

uint8_t CCR_t::getX() { return x;};
void 	CCR_t::setX(uint8_t val) { x == val;};            

uint8_t CCR_t::getS() { return s;};
void 	CCR_t::setS(uint8_t val) { s == val;};            

uint8_t CCR_t::getIPL() { return ipl;};
void 	CCR_t::setIPL(uint8_t val) { ipl == val;};            

uint8_t CCR_t::getCCRLow() {
	uint8_t val = 0;
	val |= getC();
	val |= getV() << 1;
	val |= getZ() << 2;
	val |= getN() << 3;
	val |= getI() << 4;
	val |= getH() << 5;
	val |= getX() << 6;
	val |= getS() << 7;
	
	return val;
};	
void CCR_t::setCCRLow(uint8_t val) {
	setC(val & 0x01);
	setV(val & 0x02);
	setZ(val & 0x04);
	setN(val & 0x08);
	setI(val & 0x10);
	setH(val & 0x20);
	setX(val & 0x40);
	setS(val & 0x80);
};

uint16_t CCR_t::getCCR() {
	uint16_t val = 0;
	val |= getC();
	val |= getV() << 1;
	val |= getZ() << 2;
	val |= getN() << 3;
	val |= getI() << 4;
	val |= getH() << 5;
	val |= getX() << 6;
	val |= getS() << 7;
	val |= getIPL() << 8;
	
	return val;

};

void CCR_t::setCCR(uint16_t val) {
	setC((uint8_t) val & 0x01);
	setV((uint8_t) val & 0x02);
	setZ((uint8_t) val & 0x04);
	setN((uint8_t) val & 0x08);
	setI((uint8_t) val & 0x10);
	setH((uint8_t) val & 0x20);
	setX((uint8_t) val & 0x40);
	setS((uint8_t) val & 0x80);
	setIPL((uint8_t) ((val & 0x0700) >> 8));
};
	
uint8_t CCR_t::getCCRHigh() {
	return getIPL();
};

void CCR_t::setCCRHigh(uint8_t val) {
	setIPL(val);
};
	

} // end namespace
}
}
}
} // end namespace hcs12x
