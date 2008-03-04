#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CPU_HH__

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
#include <inttypes.h>


#include <inttypes.h>
#include <iostream>
#include <unisim/component/cxx/processor/hcs12x/mmc.hh>



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
using std::string;
using std::stringstream;
using std::map;
using std::ostream;
using std::vector;


typedef uint16_t address_t;
typedef uint32_t physical_address_t;
typedef uint16_t reg_t;

class CPU;

/* I think it's better to declare the CCR as uint16_t and then use mask to set/get each bit */
 
class CCR_t
{
public:
	uint8_t getC();
	void 	setC(uint8_t val);            

	uint8_t getV();
	void 	setV(uint8_t val);            

	uint8_t getZ();
	void 	setZ(uint8_t val);            

	uint8_t getN();
	void 	setN(uint8_t val);            

	uint8_t getI();
	void 	setI(uint8_t val);            

	uint8_t getH();
	void 	setH(uint8_t val);            

	uint8_t getX();
	void 	setX(uint8_t val);            

	uint8_t getS();
	void 	setS(uint8_t val);            

	uint8_t getIPL();
	void 	setIPL(uint8_t val);            

	uint8_t getCCRLow();	
	void setCCRLow(uint8_t val);

	uint16_t getCCR();
	void setCCR(uint16_t val);
	
	uint8_t getCCRHigh();
	void setCCRHigh(uint8_t val);
	
private:		
#if BYTEORDER==LITTLE_ENDIAN
	uint8_t c : 1;
	uint8_t v : 1;
	uint8_t z : 1;
	uint8_t n : 1;
	uint8_t i : 1;
	uint8_t h : 1;
	uint8_t x : 1;
	uint8_t s : 1;
	uint8_t ipl : 3;
#else
	uint8_t ipl : 3;
	uint8_t s : 1;
	uint8_t x : 1;
	uint8_t h : 1;
	uint8_t i : 1;
	uint8_t n : 1;
	uint8_t z : 1;
	uint8_t v : 1;
	uint8_t c : 1;
#endif

}; // end class CCR_t 

	/********************************************************************
	 *********  Used for Exchange/Transfer EB:  post-byte  **************
	 * ******************************************************************/
class EB {
public:

	class EBMSLSRegs {
		public:
		enum { A=0x0, B=0x1, CCR=0x2, TMPx=0x3, D=0x4, X=0x5, Y=0x6, SP=0x7 };
	};
	
	class EBRegs {
		public:
		enum {A=0x0, B=0x1, CCR=0x20, CCRL=0x21, CCRH=0x22, CCRW=0x23, TMP1=0x30, TMP2=0x31, TMP3=0x32, D=0x4, X=0x5, Y=0x6, SP=0x7};
	};

	EB(CPU *cpu) { this->cpu = cpu; }
	~EB() { cpu = NULL; }
	 
	static string getRegName(uint8_t num) {
		switch (num) {
			case EBRegs::A: return "A"; break;
			case EBRegs::B: return "B"; break;
			case EBRegs::CCR: return "CCR"; break;
			case EBRegs::CCRL: return "CCRL"; break;
			case EBRegs::CCRH: return "CCRH"; break;
			case EBRegs::CCRW: return "CCRW"; break;
			case EBRegs::TMP1: return "TMP1"; break;
			case EBRegs::TMP2: return "TMP2"; break;
			case EBRegs::TMP3: return "TMP3"; break;
			case EBRegs::D: return "D"; break;
			case EBRegs::X: return "X"; break;
			case EBRegs::Y: return "Y"; break;
			case EBRegs::SP: return "SP"; break;
			default: return "?";			
		}
	}
	
	static uint8_t getRegSize(uint8_t num) { 
		switch (num) {
			case EBRegs::A: return 8; break;
			case EBRegs::B: return 8; break;
			case EBRegs::CCR: return 8; break;
			case EBRegs::CCRL: return 8; break;
			case EBRegs::CCRH: return 8; break;
			case EBRegs::CCRW: return 16; break;
			case EBRegs::TMP1: return 16; break;
			case EBRegs::TMP2: return 16; break;
			case EBRegs::TMP3: return 16; break;
			case EBRegs::D: return 16; break;
			case EBRegs::X: return 16; break;
			case EBRegs::Y: return 16; break;
			case EBRegs::SP: return 16; break;
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
	
};	/***********   END EB  **********/

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
	
	//utils attributes
	bool verbose_all;
	bool verbose_setup;
	
	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool Setup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================
	
	void OnBusCycle();
	void Step();
	virtual void Stop(int ret);
	virtual void Sync();

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

	virtual void BusWrite(physical_address_t addr, const uint8_t *buffer, uint32_t size) = 0;
	virtual void BusRead(physical_address_t addr, uint8_t *buffer, uint32_t size) = 0;


	//======================================================================
	//=                  Registers Acces Routines                          =
	//======================================================================
    void    setRegA(int8_t val);
    int8_t getRegA();
    
    void    setRegB(int8_t val);
    int8_t getRegB();
    
    void    setRegD(uint16_t val); // regD == regA:regB
    uint16_t getRegD();

    void    setRegX(uint16_t val);
    uint16_t getRegX();

    void    setRegY(uint16_t val);
    uint16_t getRegY();

    void    setSP(uint16_t val);
    uint16_t getSP();

    void    setPC(uint16_t val);
    uint16_t getPC();

    void    setRegTMP(uint8_t index, uint16_t val);
    uint16_t getRegTMP(uint8_t index);

	/********************************************************************
	 * *******  Used for Indexed Operations XB: Postbyte Code  **********
	 * ******************************************************************/
	static char* xb_getAddrRegLabel(uint8_t rr) {
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

	static char* xb_getAccRegLabel(uint8_t rr) {
		switch (rr) {
    	case 0:
       		return "A";
    	case 1:
   	    	return "B";
		case 2:
       		return "D";
       	default:
       		return "unknown";	
		}
	}

	uint16_t xb_getAddrRegValue(uint8_t rr);
	void xb_setAddrRegValue(uint8_t rr,uint16_t val);	

	uint16_t xb_getAccRegValue(uint8_t rr);
	/*************  END  XB  ***************/
	 
    class CCR_t *ccr;   
    class MMC	*mmc;
	class EB	*eb;
	
private:
    int8_t      regA, regB;
    uint16_t    regX, regY, regSP, regPC;
    uint16_t	regTMP[3];

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

