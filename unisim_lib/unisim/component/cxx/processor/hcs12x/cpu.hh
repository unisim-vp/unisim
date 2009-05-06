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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CPU_HH__

#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif


#include "unisim/kernel/service/service.hh"

#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/logger.hh"

#include "unisim/util/debug/register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/util/endian/endian.hh"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>


#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/ccr.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/exception.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;

using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Logger;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;

using unisim::util::debug::Register;

using unisim::util::arithmetic::Add8;
using unisim::util::arithmetic::Add16;
using unisim::util::arithmetic::Sub8;
using unisim::util::arithmetic::Sub16;

using unisim::util::endian::endian_type;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

using std::string;
using std::stringstream;
using std::map;
using std::ostream;
using std::vector;

//#define MAX_INS_SIZE	CodeType::maxsize;
#define MAX_INS_SIZE	8
#define QUEUE_SIZE		MAX_INS_SIZE

class CPU;

	/* *******************************************************************
	 * ********  Used for                                   **************
	 * ********    Exchange/Transfer EB:  post-byte         **************
	 * ********    Loop Primitive    LB:  post-byte         **************
	 * ******************************************************************/
class EBLB {
public:

	class EBLBMSLSRegs {
		public:
		enum { A=0x0, B=0x1, CCR=0x2, TMPx=0x3, D=0x4, X=0x5, Y=0x6, SP=0x7 };
	};

	class EBLBRegs {
		public:
		enum {A=0x0, B=0x1, CCR=0x20, CCRL=0x21, CCRH=0x22, CCRW=0x23, TMP1=0x30, TMP2=0x31, TMP3=0x32, D=0x4, X=0x5, Y=0x6, SP=0x7};
	};

	EBLB(CPU *cpu) { this->cpu = cpu; }
	~EBLB() { cpu = NULL; }

	static string getRegName(uint8_t num) {
		switch (num) {
			case EBLBRegs::A: return "A"; break;
			case EBLBRegs::B: return "B"; break;
			case EBLBRegs::CCR: return "CCR"; break;
			case EBLBRegs::CCRL: return "CCRL"; break;
			case EBLBRegs::CCRH: return "CCRH"; break;
			case EBLBRegs::CCRW: return "CCRW"; break;
			case EBLBRegs::TMP1: return "TMP1"; break;
			case EBLBRegs::TMP2: return "TMP2"; break;
			case EBLBRegs::TMP3: return "TMP3"; break;
			case EBLBRegs::D: return "D"; break;
			case EBLBRegs::X: return "X"; break;
			case EBLBRegs::Y: return "Y"; break;
			case EBLBRegs::SP: return "SP"; break;
			default: return "?";
		}
	}

	static uint8_t getRegSize(uint8_t num) {
		switch (num) {
			case EBLBRegs::A: return 8; break;
			case EBLBRegs::B: return 8; break;
			case EBLBRegs::CCR: return 8; break;
			case EBLBRegs::CCRL: return 8; break;
			case EBLBRegs::CCRH: return 8; break;
			case EBLBRegs::CCRW: return 16; break;
			case EBLBRegs::TMP1: return 16; break;
			case EBLBRegs::TMP2: return 16; break;
			case EBLBRegs::TMP3: return 16; break;
			case EBLBRegs::D: return 16; break;
			case EBLBRegs::X: return 16; break;
			case EBLBRegs::Y: return 16; break;
			case EBLBRegs::SP: return 16; break;
			default: return 0;
		}
	}

	/* Legal "rr" value for setter and getter functions
	 * 0x00:A; 0x01:B;
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh;
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP
	 */
	template <class T>	void setter(uint8_t rr, T val); // setter function
	template <class T>	T getter(uint8_t rr); // getter function
	template <class T> void exchange(uint8_t rrSrc, uint8_t rrDst);

private:
	CPU *cpu;

};	/***********   END EBLB  **********/


class CPU : public Decoder,
	public Client<DebugControl<service_address_t> >,
	public Client<MemoryAccessReporting<service_address_t> >,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<service_address_t> >,
    public Service<Registers>,
	public Service<Memory<service_address_t> >,
	public Client<Memory<service_address_t> >,
	public Client<SymbolTableLookup<service_address_t> >,
	public Client<Logger>,
	public Client<TrapReporting >

{
public:

	void queueFlush(uint8_t nByte); // flush is called after prefetch() to advance the queue cursor (first pointer)
	uint8_t* queueFetch(address_t addr, uint8_t* ins, uint8_t nByte);

private:
	address_t	queueCurrentAddress;
	uint8_t		queueBuffer[QUEUE_SIZE];

	int		queueFirst, queueNElement;

	void queueFill(address_t addr, int position, uint8_t nByte);

public:

	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================

	ServiceExport<Disassembly<service_address_t> > disasm_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<Memory<service_address_t> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<DebugControl<service_address_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<service_address_t> > memory_access_reporting_import;
//	ServiceImport<SymbolTableLookup<service_address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<service_address_t> > memory_import;
	ServiceImport<Logger> logger_import;

	ServiceImport<TrapReporting > trap_reporting_import;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();

	void SetEntryPoint(address_t cpu_address);

	//==========================================
	//=     ISA - MEMORY ACCESS ROUTINES       =
	//==========================================

	inline uint8_t memRead8(address_t logicalAddress, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);
	inline void memWrite8(address_t logicalAddress,uint8_t val, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);

	inline uint16_t memRead16(address_t logicalAddress, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);
	inline void memWrite16(address_t logicalAddress,uint16_t val, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);

	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================

	void OnBusCycle();
	uint8_t Step();	// Return the number of cpu cycles consumed by the operation
	virtual void Stop(int ret);
	virtual void Sync();

	virtual void Reset();

	/* TODO:
	 * Stop All Clocks and puts the device in standby mode.
	 * Asserting the RESET, XIRQ, or IRQ signals ends standby mode.
	 */
	virtual void Sleep() = 0;

	/* TODO:
	 * Enter a wait state for an integer number of bus clock cycle
	 * Only CPU12 clocks are stopped
	 * Wait for not masked interrupt
	 */
	virtual void Wait() = 0;

	/*
	 * The CPU issues a signal that tells the interrupt module to drive
	 * the vector address of the highest priority pending exception onto the system address bus
	 * (the CPU12 does not provide this address)
	 *
	 * Priority is as follow: reset => sw-interrupt => hw-interrupt => spurious interrupt
	 *
	 * If (RAM_ACC_VIOL | SYS || SWI || TRAP) return IVBR;
	 * Else return INT_Vector
	 */
	virtual address_t GetIntVector(uint8_t &ipl) = 0;

	//=====================================================================
	//=                    Interface with outside                         =
	//=====================================================================
	inline bool HasAsynchronousInterrupt() const { return asynchronous_interrupt; }

	inline bool HasMaskableIbitInterrup() const { return maskableIbit_interrupt; }
	inline bool HasNonMaskableXIRQInterrupt() const { return nonMaskableXIRQ_interrupt; }
	inline bool HasNonMaskableAccessErrorInterrupt() const { return nonMaskableAccessError_interrupt; }
	inline bool HasNonMaskableSWIInterrupt() const { return nonMascableSWI_interrupt; }
	inline bool HasTrapInterrupt() const { return trap_interrupt; }
	inline bool HasReset() const { return reset; }
	inline bool HasSysCallInterrupt() const { return syscall_interrupt; }
	inline bool HasSpuriousInterrupt() const { return spurious_interrupt; }

	//=====================================================================
	//=                    Exception handling methods                     =
	//=====================================================================

	// compute return address, save the CPU registers and then set I/X bit before the interrupt handling began
	void PrepareInterrupt();

	// Asynchronous Interrupts (including Resets, I-bit, XIRQ, IRQ)
	void HandleException(const AsynchronousException& exc);

	// Hardware and Software reset (including COP, clock monitor, and pin)
	void HandleResetException(address_t resetVector);

	// Non-maskable (X bit) interrupts
	void HandleNonMaskableXIRQException(address_t xirqVector, uint8_t newIPL);

	// Maskable (I bit) interrupt
	void HandleMaskableIbitException(address_t ibitVector, uint8_t newIPL);

	// A software interrupt instruction (SWI) or BDM vector request
	void HandleException(const NonMaskableSWIInterrupt& exc);

	// Unimplemented opcode trap
	void HandleException(const TrapException& exc);

	// A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)
	void HandleException(const SysCallInterrupt& exc);

	// A spurious interrupt
	void HandleException(const SpuriousInterrupt& exc);

	// Non-maskable Access Error interrupts
	void HandleException(const NonMaskableAccessErrorInterrupt& exc);

	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================

	void AckAsynchronousInterrupt();
	void AckIbitInterrupt();
	void AckXIRQInterrupt();
	void AckAccessErrorInterrupt();
	void AckSWIInterrupt();
	void AckTrapInterrupt();
	void AckReset();
	void AckSysInterrupt();
	void AckSpuriousInterrupt();

	//=====================================================================
	//=                    Hardware interrupt request                     =
	//=====================================================================

	void ReqAsynchronousInterrupt();
	void ReqIbitInterrupt();
	void ReqXIRQInterrupt();
	void ReqAccessErrorInterrupt();
	void ReqSWIInterrupt();
	void ReqTrapInterrupt();
	void ReqReset();
	void ReqSysInterrupt();
	void ReqSpuriousInterrupt();

	//======================================================================
	//=                  Registers Acces Routines                          =
	//======================================================================
    inline void    setRegA(uint8_t val);
    inline uint8_t getRegA();

    inline void    setRegB(uint8_t val);
    inline uint8_t getRegB();

    inline void    setRegD(uint16_t val); // regD == regA:regB
    inline uint16_t getRegD();

    inline void    setRegX(uint16_t val);
    inline uint16_t getRegX();

    inline void    setRegY(uint16_t val);
    inline uint16_t getRegY();

    inline void    setRegSP(uint16_t val);
    inline uint16_t getRegSP();

    inline void    setRegPC(uint16_t val);
    inline uint16_t getRegPC();

    inline void    setRegTMP(uint8_t index, uint16_t val);
    inline uint16_t getRegTMP(uint8_t index);

	/********************************************************************
	 * *******  Used for Indexed Operations XB: Postbyte Code  **********
	 * ******************************************************************/
	static string xb_getAddrRegLabel(uint8_t rr) {
		switch (rr) {
    	case 0:
       		return "X";
    	case 1:
   	    	return "Y";
		case 2:
       		return "SP";
    	case 3:
   	    	return "PC";
		}
	}

	static string xb_getAccRegLabel(uint8_t rr) {
		switch (rr) {
    	case 0:
       		return "A";
    	case 1:
   	    	return "B";
		case 2:
       		return "D";
       	default:
       		return "unknown"; // rr=11 see accumulator D offset indexed-indirect
		}
	}

	inline uint16_t xb_getAddrRegValue(uint8_t rr);
	inline void xb_setAddrRegValue(uint8_t rr,uint16_t val);

	inline uint16_t xb_getAccRegValue(uint8_t rr);
	/*************  END  XB  ***************/

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool Setup();
	virtual void OnDisconnect();

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report) ;

	inline void MonitorStore(address_t ea, uint32_t size);
	inline void MonitorLoad(address_t ea, uint32_t size);

	inline void ReportTrap();

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             bus interface methods                              =
	//=====================================================================
	virtual void BusWrite(address_t addr, const void *buffer, uint32_t size) = 0;
	virtual void BusRead(address_t addr, void *buffer, uint32_t size) = 0;

	//=====================================================================
	//=             CPURegistersInterface interface methods               =
	//=====================================================================

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
    virtual Register *GetRegister(const char *name);

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
    virtual string Disasm(service_address_t addr, service_address_t &next_addr);

	//=====================================================================
	//=                   Debugging methods                               =
	//=====================================================================

    inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	inline bool IsVerboseException() const { return logger_import && CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception); }

	address_t getLastPC() {return lastPC; }

//protected:
    class CCR_t *ccr;
	class EBLB	*eblb;

protected:
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================

	uint64_t cpu_cycle_time; //!< CPU cycle time in ps
	uint64_t voltage;        //!< CPU voltage in mV
	uint64_t bus_cycle_time; //!< Front side bus cycle time in ps
	uint64_t cpu_cycle;      //!< Number of cpu cycles
	uint64_t bus_cycle;      //!< Number of front side bus cycles

	//utils attributes
	bool verbose_all;
	bool verbose_setup;
	bool verbose_step;
	bool verbose_dump_regs_start;
	bool verbose_dump_regs_end;
	bool verbose_exception;

		// verbose methods
	inline bool VerboseSetup() GCC_INLINE;
	inline bool VerboseStep() GCC_INLINE;
	inline void VerboseDumpRegs() GCC_INLINE;
	inline void VerboseDumpRegsStart() GCC_INLINE;
	inline void VerboseDumpRegsEnd() GCC_INLINE;

	inline void RegistersInfo() GCC_INLINE;

	/** indicates if the memory accesses require to be reported */
	bool requires_memory_access_reporting;
	/** indicates if the finished instructions require to be reported */
	bool requires_finished_instruction_reporting;

private:
	uint8_t		regA, regB;
    uint16_t    regX, regY, regSP, regPC;
    uint16_t	regTMP[3];

    address_t lastPC;

	//=====================================================================
	//=                   68HCS12X interrupt signals                      =
	//=====================================================================

	bool asynchronous_interrupt;
	bool maskableIbit_interrupt;	// I-Bit maskable interrupts => IVBR + 0x0012-0x00F2 (113 interrupts)
	bool nonMaskableXIRQ_interrupt;	// X-Bit (XIRQ) maskable interrupt => IVBR + 0x00F4
	bool nonMaskableAccessError_interrupt; // Memory Access Error Interrupt
	bool nonMascableSWI_interrupt;	// (SWI) => IVBR + 0x00F6
									// non maskable software interrupt request or background debug mode vector request
	bool trap_interrupt;			// non maskable unimplemented opcode => IVBR + 0x00F8
	bool reset;						// Hardware and Software interrupt =>  0xFFFA-0xFFFE
	bool syscall_interrupt;			// SYS call interrupt =>
	bool spurious_interrupt;		// Spurious interrupt => IVBR + 0x0010 (default interrupt)

	// Registers map
	map<string, Register *> registers_registry;

	/** the instruction counter */
	uint64_t instruction_counter;
	uint64_t	max_inst;
	Parameter<uint64_t>	   param_max_inst;

};

// ==============================================
// =          MEMORY ACCESS ROUTINES            =
// ==============================================

inline void CPU::MonitorLoad(address_t ea, uint32_t size)
{
	// Memory access reporting
	if(requires_memory_access_reporting && memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<service_address_t>::MAT_READ, MemoryAccessReporting<service_address_t>::MT_DATA, ea, size);
	}
}

inline void CPU::MonitorStore(address_t ea, uint32_t size)
{
	// Memory access reporting
	if(requires_memory_access_reporting && memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<service_address_t>::MAT_WRITE, MemoryAccessReporting<service_address_t>::MT_DATA, ea, size);
	}
}

inline void CPU::ReportTrap() {
	if (CONFIG::DEBUG_ENABLE && trap_reporting_import) {
		std::cerr << "*** CPU12 ReprotTrap *** 0x" << std::hex << getLastPC() << std::dec << std::endl;
		trap_reporting_import->ReportTrap();
	}

}

inline uint8_t CPU::memRead8(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal) {

	uint8_t data;
	MMC_DATA mmc_data;

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 1;

	BusRead(logicalAddress, &mmc_data, sizeof(MMC_DATA));

	MonitorLoad(logicalAddress, sizeof(data));

	return data;
}

inline uint16_t CPU::memRead16(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal) {

	uint16_t data;
	MMC_DATA mmc_data;

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 2;

	BusRead(logicalAddress, &mmc_data, sizeof(MMC_DATA));

	data = BigEndian2Host(data);

	MonitorLoad(logicalAddress, sizeof(data));

	return data;
}

inline void CPU::memWrite8(address_t logicalAddress,uint8_t val, ADDRESS::MODE type, bool isGlobal) {

	MMC_DATA mmc_data;

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &val;
	mmc_data.data_size = 1;

	BusWrite( logicalAddress, &mmc_data, sizeof(MMC_DATA));

	MonitorStore(logicalAddress, sizeof(val));

}

inline void CPU::memWrite16(address_t logicalAddress,uint16_t val, ADDRESS::MODE type, bool isGlobal) {

	MMC_DATA mmc_data;

	val = Host2BigEndian(val);

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &val;
	mmc_data.data_size = 2;

	BusWrite( logicalAddress, &mmc_data, sizeof(MMC_DATA));

	MonitorStore(logicalAddress, sizeof(val));

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


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

