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

#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/symbol_table_build.hh"
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
#include <unisim/component/cxx/processor/hcs12x/mmc.hh>
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
	public Client<DebugControl<physical_address_t> >,
	public Client<MemoryAccessReporting<physical_address_t> >,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<physical_address_t> >,
    public Service<Registers>,
	public Service<Memory<physical_address_t> >,
	public Client<Memory<physical_address_t> >,
	public Client<SymbolTableLookup<physical_address_t> >,
	public Client<Logger> 
{
public:

	void queueFlush(uint8_t nByte); // flush is called after prefetch() to advance the queue cursor (first pointer)
	uint8_t* queueFetch(physical_address_t addr, uint8_t* ins, uint8_t nByte);
	
private:
	physical_address_t	queueCurrentAddress;
	uint8_t		queueBuffer[QUEUE_SIZE];
	
	int		queueFirst, queueNElement;
	
	void queueFill(physical_address_t addr, int position, uint8_t nByte);
	
public:

	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<physical_address_t> > disasm_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<DebugControl<physical_address_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<physical_address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<physical_address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceImport<Logger> logger_import;
	
	
	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();
	void SetEntryPoint(uint8_t page, address_t cpu_address);

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool Setup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================
	
	void OnBusCycle();
	// step() Return the number of cpu cycles consumed by the operation
	uint8_t Step();
	virtual void Stop(int ret);
	virtual void Sync();
	
	
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

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report) ;

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================
	virtual void Reset();
	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

	/* ******** MEMORY ACCESS ROUTINES ******* */
	uint8_t memRead8(physical_address_t addr);
	void memWrite8(physical_address_t addr,uint8_t val);
	uint16_t memRead16(physical_address_t addr);
	void memWrite16(physical_address_t addr,uint16_t val);
	
	/* ******** END MEM ACCESS ROUTINES ****** */

	//=====================================================================
	//=             bus interface methods                              =
	//=====================================================================
	virtual void BusWrite(physical_address_t addr, const void *buffer, uint32_t size) = 0;
	virtual void BusRead(physical_address_t addr, void *buffer, uint32_t size) = 0;
	
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
    virtual string Disasm(physical_address_t addr, physical_address_t &next_addr);

	//=====================================================================
	//=                   Debugging methods                               =
	//=====================================================================

    inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	inline bool IsVerboseException() const { return logger_import && CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception); }

	//=====================================================================
	//=                    Interface with outside                         =
	//=====================================================================

	inline bool HasHardReset() const { return hard_reset; }
	inline bool HasSoftReset() const { return soft_reset; }
	inline bool HasNonMaskableXIRQInterrupt() const { return nonMaskableXIRQ_interrupt; }
	inline bool HasMaskableInterrupt() const { return maskable_interrupt; }
	inline bool HasAsynchronousInterrupt() const { return asynchronous_interrupt; }

	//=====================================================================
	//=                    Exception handling methods                     =
	//=====================================================================

	// Hardware and Software reset (including COP, clock monitor, and pin)
	void HandleException(const ResetException& exc);

	// Unimplemented opcode trap
	void HandleException(const TrapException& exc);

	// A software interrupt instruction (SWI) or BDM vector request 
	void HandleException(const SoftwareInterrupt& exc);

	// A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)
	void HandleException(const SysCallInterrupt& exc);

	// Non-maskable (X bit) interrupts
	void HandleException(const NonMaskableXIRQInterrupt& exc);

	// Maskable (I bit) interrupt
	void HandleException(const MaskableInterrupt& exc);

	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================

	void AckHardReset();
	void AckSoftReset();
	void AckNonMaskableAccessErrorInterrupt();
	void AckMaskableInterrupt();
	void AckAsynchronousInterrupt();

	//=====================================================================
	//=                    Hardware interrupt request                     =
	//=====================================================================
	
	void ReqHardReset();
	void ReqSoftReset();
	void ReqNonMaskableAccessErrorInterrupt();
	void ReqMaskableInterrupt();
	void ReqAsynchronousInterrupt();

	//======================================================================
	//=                  Registers Acces Routines                          =
	//======================================================================
    void    setRegA(uint8_t val);
    uint8_t getRegA();
    
    void    setRegB(uint8_t val);
    uint8_t getRegB();
    
    void    setRegD(uint16_t val); // regD == regA:regB
    uint16_t getRegD();

    void    setRegX(uint16_t val);
    uint16_t getRegX();

    void    setRegY(uint16_t val);
    uint16_t getRegY();

    void    setRegSP(uint16_t val);
    uint16_t getRegSP();

    void    setRegPC(uint16_t val);
    uint16_t getRegPC();

    void    setRegTMP(uint8_t index, uint16_t val);
    uint16_t getRegTMP(uint8_t index);

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

	uint16_t xb_getAddrRegValue(uint8_t rr);
	void xb_setAddrRegValue(uint8_t rr,uint16_t val);	

	uint16_t xb_getAccRegValue(uint8_t rr);
	/*************  END  XB  ***************/

	//=====================================================================
	//=              Interrupt Vectors and Access Routines                =
	//=====================================================================

	static const uint8_t IVBR_DEFAULT_VALUE				= 0xFF;		// IVBR is only one. 

	static const address_t IVBR_ADDRESS					= 0x0121;	// S12XINT: Address of the Interrupt Vector Base Register

//	static const address_t TRAP_VECTOR_ADDRESS 			= 0xFFF8;	// Shared interrupt vector for traps ($FFF8:$FFF9)
//	static const address_t SYS_VECTOR_ADDRESS			= 0xFF12;	// System call interrupt vector is $FF12:$FF13


	void setIVBR(uint8_t val) {	memWrite8((physical_address_t&) IVBR_ADDRESS, val); }
	uint8_t getIVBR() { return memRead8((physical_address_t) IVBR_ADDRESS); }

	address_t get_SysReset_Vector() { return 0xFFFE; }
	address_t get_IllegalAccessReset_Vector() { return 0xFFFE; }
	address_t get_ClockMonitorReset_Vector() { return 0xFFFC; }
	address_t get_COPWatchdogReset_Vector() { return 0xFFFA; } 

	address_t get_Trap_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xF8; } // Shared interrupt vector for traps ($FFF8:$FFF9)
	address_t get_SWI_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xF6 ; }
	address_t get_XIRQ_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xF4 ; }
	address_t get_IRQEN_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xF2 ; }
	address_t get_RTI_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xF0 ; } // Real Time Interrupt Vector
	address_t get_ECT_Ch0_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xEE ; } // Enhanced Capture Timer Channel 0 
	address_t get_ECT_Ch1_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xEC ; } // Enhanced Capture Timer Channel 1
	address_t get_ECT_Ch2_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xEA ; } // Enhanced Capture Timer Channel 2
	address_t get_ECT_Ch3_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xE8 ; } // Enhanced Capture Timer Channel 3
	address_t get_ECT_Ch4_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xE6 ; } // Enhanced Capture Timer Channel 4
	address_t get_ECT_Ch5_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xE4 ; } // Enhanced Capture Timer Channel 5
	address_t get_ECT_Ch6_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xE2 ; } // Enhanced Capture Timer Channel 6
	address_t get_ECT_Ch7_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xE0 ; } // Enhanced Capture Timer Channel 7
	address_t get_ECT_Overflow_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xDE ; } // Enhanced capture Timer Overflow
	address_t get_PAcc_A_Overflow_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xDC ; } // Pulse Accumulator A Overflow
	address_t get_PAcc_Input_edge_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xDA ; } // Pulse Accumulator Input Edge
	address_t get_SPI0_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xD8 ; } 
	address_t get_SCI0_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xD6 ; }
	address_t get_SCI1_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xD4 ; }
	address_t get_ATD0_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xD2 ; }
	address_t get_ATD1_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xD0 ; }
	address_t get_PortJ_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xCE ; }
	address_t get_PortH_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xCC ; }
	address_t get_MDC_Underflow_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xCA ; } // Modulus Down Counter Underflow
	address_t get_PAcc_B_Overflow_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xC8 ; } // Pulse Accumulator B Overflow
	address_t get_CRGPLL_Lock_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xC6 ; }
	address_t get_CRG_SelfClockMode_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xC4 ; }
	address_t get_IIC0_Bus_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xC0 ; }
	address_t get_SPI1_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xBE ; }
	address_t get_SPI2_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xBC ; }
	address_t get_EEPROM_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xBA ; }
	address_t get_FLASH_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xB8 ; }
	address_t get_CAN0_WakeUp_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xB6 ; }
	address_t get_CAN0_Errors_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xB4 ; }
	address_t get_CAN0_Receive_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xB2 ; }
	address_t get_CAN0_Transmit_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xB0 ; }
	address_t get_CAN1_WakeUp_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xAE ; }
	address_t get_CAN1_Errors_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xAC ; }
	address_t get_CAN1_Receive_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xAA ; }
	address_t get_CAN1_Transmit_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xA8 ; }
	address_t get_CAN2_WakeUp_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xA6 ; }
	address_t get_CAN2_Errors_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xA4 ; }
	address_t get_CAN2_Receive_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xA2 ; }
	address_t get_CAN2_Transmit_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0xA0 ; }
	address_t get_CAN3_WakeUp_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x9E ; }
	address_t get_CAN3_Errors_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x9C ; }
	address_t get_CAN3_Receive_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x9A ; }
	address_t get_CAN3_Transmit_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x98 ; }
	address_t get_CAN4_WakeUp_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x96 ; }
	address_t get_CAN4_Errors_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x94 ; }
	address_t get_CAN4_Receive_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x92 ; }
	address_t get_CAN4_Transmit_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x90 ; }
	address_t get_PortP_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x8E ; }
	address_t get_PWM_Shutdown_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x8C ; }
	address_t get_SCI2_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x8A ; }
	address_t get_SCI3_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x88 ; }
	address_t get_SCI4_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x86 ; }
	address_t get_SCI5_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x84 ; }
	address_t get_IIC1_Bus_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x82 ; }
	address_t get_LVI_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x80 ; }
	address_t get_API_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x7E ; }
	address_t get_PIT_Ch0_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x7A ; } // Periodic Interrupt Timer Channel 0
	address_t get_PIT_Ch1_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x78 ; } // Periodic Interrupt Timer Channel 1
	address_t get_PIT_Ch2_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x76 ; } // Periodic Interrupt Timer Channel 2
	address_t get_PIT_Ch3_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x74 ; } // Periodic Interrupt Timer Channel 3
	address_t get_XGATE_SW_Trigger0_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x72 ; }
	address_t get_XGATE_SW_Trigger1_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x70 ; }
	address_t get_XGATE_SW_Trigger2_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x6E ; }
	address_t get_XGATE_SW_Trigger3_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x6C ; }
	address_t get_XGATE_SW_Trigger4_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x6A ; }
	address_t get_XGATE_SW_Trigger5_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x68 ; }
	address_t get_XGATE_SW_Trigger6_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x66 ; }
	address_t get_XGATE_SW_Trigger7_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x64 ; }
	address_t get_XGATE_SW_Error_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x62 ; }
	address_t get_S12X_RAV_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x60 ; } // S12XCPU RAM Access Violation

	address_t get_Sys_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x12; } // System call interrupt vector is $FF12:$FF13
	address_t get_Sputious_Vector() { return /*((address_t) getIVBR() << 8)*/ ivbr_value + 0x10 ; } // Spurious interrupt
	
	
//protected:
    class MMC	*mmc;
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

	address_t	ivbr_value; // Interrupt Vector Base Register Value
	
	//=====================================================================
	//=                   68HCS12X interrupt signals                      =
	//=====================================================================
	
	bool soft_reset;						//!< Soft reset signal
	bool hard_reset;						//!< Hard reset signal
	bool nonMaskableXIRQ_interrupt;			//!< Non maskable access error Interrupt signal
	bool maskable_interrupt;				//!< Maskable interrupt signal 
	
	bool asynchronous_interrupt;          //!< summary of all hardware interrupt signals

	// Registers map
	map<string, Register *> registers_registry;       

	/** the instruction counter */
	uint64_t instruction_counter;

};



} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

