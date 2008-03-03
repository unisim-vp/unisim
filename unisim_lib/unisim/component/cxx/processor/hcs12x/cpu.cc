
#include <hcs12x.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {
	
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
	logger_import("logger_import", this)
	
{

	regA    = 0;
    regB    = 0;
    regX    = 0;
    regY    = 0;
    regSP   = 0;
    regPC   = 0;
    
    mmc = new MMC();
    ccr = new CCR_t();
    eb = new EB(this);

}

CPU::~CPU() 
{ 
	delete mmc; mmc = NULL;
	delete eb; eb = NULL;
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CPU::Setup()
{
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
}

void CPU::Stop(int ret)
{
}

void CPU::Sync()
{
}

//=====================================================================
//=             memory access reporting control interface methods     =
//=====================================================================

void CPU::RequiresMemoryAccessReporting(bool report)
{
}

void CPU::RequiresFinishedInstructionReporting(bool report)
{
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================
void CPU::Reset()
{
}

bool CPU::ReadMemory(physical_address_t addr, void *buffer, uint32_t size)
{
	return false;
}

bool CPU::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size)
{
	return false;
}


//=====================================================================
//=             CPURegistersInterface interface methods               =
//=====================================================================

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
Register *CPU::GetRegister(const char *name)
{
	return 0;
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
	// TODO
	next_addr = addr + 1;
	return string("");
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
    	return getSP();
	case 3:
    	return getPC();    
	}
}

void CPU::xb_setAddrRegValue(uint8_t rr,uint16_t val) {
    switch (rr) {
	case 0:
    	return setRegX(val);
	case 1:
    	return setRegY(val);
	case 2:
    	return setSP(val);
	case 3:
    	return setPC(val);    
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
	

void CPU::setRegA(int8_t val) { regA = val; }
int8_t CPU::getRegA() { return regA; }
    
void CPU::setRegB(int8_t val) { regB = val; }    
int8_t CPU::getRegB() { return regB; }
    
void CPU::setRegD(uint16_t val) { 
    // regD == regA:regB
    regA = (uint8_t) (val >> 8); 
    regB = (uint8_t) (val & 0x00ff); 
}    
    
uint16_t CPU::getRegD() { 
    // regD == regA:regB
    uint16_t tmp;
        
    tmp = (uint16_t) regA;
    tmp = (tmp << 8) | regB;
        
    return tmp; 
}

void CPU::setRegX(uint16_t val) { regX = val; }    
uint16_t CPU::getRegX() { return regX; }

void CPU::setRegY(uint16_t val) { regY = val; }    
uint16_t CPU::getRegY() { return regY; }

void CPU::setSP(uint16_t val) { regSP = val; }    
uint16_t CPU::getSP() { return regSP; }

void CPU::setPC(uint16_t val) { regPC = val; }    
uint16_t CPU::getPC() { return regPC; }

void CPU::setRegTMP(uint8_t index, uint16_t val) { regTMP[index] = val; }
uint16_t CPU::getRegTMP(uint8_t index) { return regTMP[index]; }

template <class T>
void EB::setter(uint8_t rr, T val) // setter function
	/* Legal "rr" value for setter and getter functions 
	 * 0x00:A; 0x01:B; 
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh; 0x23:CCRWord
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP  
	 */
{
	switch (rr) {
		case EBRegs::A: cpu->setRegA((uint8_t) val); break;
		case EBRegs::B: cpu->setRegB((uint8_t) val); break;
		case EBRegs::CCR: cpu->ccr->setCCRLow((uint8_t) val); break;
		case EBRegs::CCRL: cpu->ccr->setCCRLow((uint8_t) val); break;
		case EBRegs::CCRH: cpu->ccr->setCCRHigh((uint8_t) val); break;
		case EBRegs::CCRW: cpu->ccr->setCCR((uint16_t) val); break;
		case EBRegs::TMP1: cpu->setRegTMP(0, (uint16_t) val); break;
		case EBRegs::TMP2: cpu->setRegTMP(1, (uint16_t) val); break;
		case EBRegs::TMP3: cpu->setRegTMP(2, (uint16_t) val); break;
		case EBRegs::D: cpu->setRegD((uint16_t) val); break;
		case EBRegs::X: cpu->setRegX((uint16_t) val); break;
		case EBRegs::Y: cpu->setRegY((uint16_t) val); break;
		case EBRegs::SP: cpu->setSP((uint16_t) val); break;
	}
}

template <class T>
T EB::getter(uint8_t rr) // getter function
	/* Legal "rr" value for setter and getter functions 
	 * 0x00:A; 0x01:B; 
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh; 0x23:CCRWord
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP  
	 */
{
	switch (rr) {
		case EBRegs::A: return (uint8_t) cpu->getRegA(); break;
		case EBRegs::B: return (uint8_t) cpu->getRegB(); break;
		case EBRegs::CCR: return (uint8_t) cpu->ccr->getCCRLow(); break;
		case EBRegs::CCRL: return (uint8_t) cpu->ccr->getCCRLow(); break;
		case EBRegs::CCRH: return (uint8_t) cpu->ccr->getCCRHigh(); break;
		case EBRegs::CCRW: return (uint16_t) cpu->ccr->getCCR(); break;
		case EBRegs::TMP1: return (uint16_t) cpu->getRegTMP(0); break;
		case EBRegs::TMP2: return (uint16_t) cpu->getRegTMP(1); break;
		case EBRegs::TMP3: return (uint16_t) cpu->getRegTMP(2); break;
		case EBRegs::D: return (uint16_t) cpu->getRegD(); break;
		case EBRegs::X: return (uint16_t) cpu->getRegX(); break;
		case EBRegs::Y: return (uint16_t) cpu->getRegY(); break;
		case EBRegs::SP: return (uint16_t) cpu->getSP(); break;
	}
}

template <class T> 
void EB::exchange(uint8_t rrSrc, uint8_t rrDst) {
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
