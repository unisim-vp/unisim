#include "xb.hh"
#include <cassert>
#include <ostream>
#include <cstring>
#line 52 "xb.isa"

using namespace std;
#line 9 "xb.cc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace hcs12x { namespace XB {
unsigned int const CodeType::maxsize;
std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {
	char const* xrepr = "0123456789abcdef";
	char const* sep = "";
	for( unsigned int idx = 0; idx < _ct.size; ++idx, sep = " " ) {
		uint32_t byte = _ct.str[idx];
		_sink << sep << xrepr[byte/16] << xrepr[byte%16];
	}
	return _sink;
}
Operation::Operation(CodeType const& _code, uint16_t _addr, const char *_name)
:
encoding(_code),
addr(_addr),
name(_name)
{
}

Operation::~Operation()
{
}

#line 62 "xb.isa"
uint16_t
#line 35 "xb.cc"
Operation::getEAddr(
#line 62 "xb.isa"
CPU *
#line 39 "xb.cc"
#line 62 "xb.isa"
cpu
#line 42 "xb.cc"
)
{
#line 62 "xb.isa"
	{ assert( false ); return 0; }
#line 47 "xb.cc"
}
#line 58 "xb.isa"
void
#line 51 "xb.cc"
Operation::disasm(
#line 58 "xb.isa"
ostream&
#line 55 "xb.cc"
#line 58 "xb.isa"
sink
#line 58 "xb.cc"
)
{
#line 58 "xb.isa"
	{
		sink << "?";
	}
#line 65 "xb.cc"
}
#line 56 "xb.isa"
uint8_t
#line 69 "xb.cc"
Operation::getCycles()
{
#line 56 "xb.isa"
	{ return 1; }
#line 74 "xb.cc"
}
class OpXb_auto : public Operation
{
public:
	OpXb_auto(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t p;
	uint32_t nnnn;
	virtual
#line 62 "xb.isa"
	uint16_t
#line 86 "xb.cc"
	getEAddr(
#line 62 "xb.isa"
	CPU *
#line 90 "xb.cc"
#line 62 "xb.isa"
	cpu
#line 93 "xb.cc"
	);
	virtual
#line 58 "xb.isa"
	void
#line 98 "xb.cc"
	disasm(
#line 58 "xb.isa"
	ostream&
#line 102 "xb.cc"
#line 58 "xb.isa"
	sink
#line 105 "xb.cc"
	);
private:
};

class OpXb_5bit_cst : public Operation
{
public:
	OpXb_5bit_cst(CodeType const& code, uint16_t addr);
	uint32_t rr;
	int32_t nnnnn;
	virtual
#line 62 "xb.isa"
	uint16_t
#line 119 "xb.cc"
	getEAddr(
#line 62 "xb.isa"
	CPU *
#line 123 "xb.cc"
#line 62 "xb.isa"
	cpu
#line 126 "xb.cc"
	);
	virtual
#line 58 "xb.isa"
	void
#line 131 "xb.cc"
	disasm(
#line 58 "xb.isa"
	ostream&
#line 135 "xb.cc"
#line 58 "xb.isa"
	sink
#line 138 "xb.cc"
	);
private:
};

class OpXb_9_cst_z0 : public Operation
{
public:
	OpXb_9_cst_z0(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t s;
	int32_t n8;
	virtual
#line 62 "xb.isa"
	uint16_t
#line 153 "xb.cc"
	getEAddr(
#line 62 "xb.isa"
	CPU *
#line 157 "xb.cc"
#line 62 "xb.isa"
	cpu
#line 160 "xb.cc"
	);
	virtual
#line 58 "xb.isa"
	void
#line 165 "xb.cc"
	disasm(
#line 58 "xb.isa"
	ostream&
#line 169 "xb.cc"
#line 58 "xb.isa"
	sink
#line 172 "xb.cc"
	);
private:
};

class OpXb_16bit_cst_z1 : public Operation
{
public:
	OpXb_16bit_cst_z1(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t s;
	int32_t n16;
	virtual
#line 62 "xb.isa"
	uint16_t
#line 187 "xb.cc"
	getEAddr(
#line 62 "xb.isa"
	CPU *
#line 191 "xb.cc"
#line 62 "xb.isa"
	cpu
#line 194 "xb.cc"
	);
	virtual
#line 58 "xb.isa"
	void
#line 199 "xb.cc"
	disasm(
#line 58 "xb.isa"
	ostream&
#line 203 "xb.cc"
#line 58 "xb.isa"
	sink
#line 206 "xb.cc"
	);
private:
};

class OpXb_acc_offset : public Operation
{
public:
	OpXb_acc_offset(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t aa;
	virtual
#line 62 "xb.isa"
	uint16_t
#line 220 "xb.cc"
	getEAddr(
#line 62 "xb.isa"
	CPU *
#line 224 "xb.cc"
#line 62 "xb.isa"
	cpu
#line 227 "xb.cc"
	);
	virtual
#line 58 "xb.isa"
	void
#line 232 "xb.cc"
	disasm(
#line 58 "xb.isa"
	ostream&
#line 236 "xb.cc"
#line 58 "xb.isa"
	sink
#line 239 "xb.cc"
	);
private:
};

class OpXb_accD_offset_idx_ind : public Operation
{
public:
	OpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr);
	uint32_t rr;
	virtual
#line 62 "xb.isa"
	uint16_t
#line 252 "xb.cc"
	getEAddr(
#line 62 "xb.isa"
	CPU *
#line 256 "xb.cc"
#line 62 "xb.isa"
	cpu
#line 259 "xb.cc"
	);
	virtual
#line 58 "xb.isa"
	void
#line 264 "xb.cc"
	disasm(
#line 58 "xb.isa"
	ostream&
#line 268 "xb.cc"
#line 58 "xb.isa"
	sink
#line 271 "xb.cc"
	);
private:
};

#line 62 "xb.isa"
uint16_t
#line 278 "xb.cc"
OpXb_auto::getEAddr(
#line 62 "xb.isa"
CPU *
#line 282 "xb.cc"
#line 62 "xb.isa"
cpu
#line 285 "xb.cc"
)
{
#line 126 "xb.isa"
	{

		address_t regOldVal = cpu->xb_getAddrRegValue(rr);
		address_t regNewVal;

		short val = nnnn;
		// if (val > 7) then decrement else increment
		if (val < 8) // increment index register
		{
			regNewVal = regOldVal + val + 1;
			} else { // decrement index register
			regNewVal = (int16_t) regOldVal + val - 16;
		}

		cpu->xb_setAddrRegValue(rr,regNewVal);
		if (p == 0) {
			// Auto pre-
			return regNewVal;
			} else {
			// Auto post-
			return regOldVal;
		}
	}
#line 312 "xb.cc"
}
#line 58 "xb.isa"
void
#line 316 "xb.cc"
OpXb_auto::disasm(
#line 58 "xb.isa"
ostream&
#line 320 "xb.cc"
#line 58 "xb.isa"
sink
#line 323 "xb.cc"
)
{
#line 150 "xb.isa"
	{

		string	regLabel = CPU::xb_getAddrRegLabel(rr);
		char	sign = '+';
		short  val = nnnn;

		if (val > 7) {
			val = val - 16;
			} else {
			val = val + 1;
		}

		if (val < 0) {
			sign = '-';
			val = -1*val;
		}

		sink << std::dec << val << ",";
		if (p == 0) {
			sink << sign << regLabel;
			} else {
			sink << regLabel << sign;
		}
	}
#line 351 "xb.cc"
}

static Operation *DecodeOpXb_auto(CodeType const& code, uint16_t addr)
{
	return new OpXb_auto(code, addr);
}

#line 62 "xb.isa"
uint16_t
#line 361 "xb.cc"
OpXb_5bit_cst::getEAddr(
#line 62 "xb.isa"
CPU *
#line 365 "xb.cc"
#line 62 "xb.isa"
cpu
#line 368 "xb.cc"
)
{
#line 67 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + nnnnn;

		return addr;

		//    return cpu->xb_getAddrRegValue(rr) + nnnnn;
	}
#line 380 "xb.cc"
}
#line 58 "xb.isa"
void
#line 384 "xb.cc"
OpXb_5bit_cst::disasm(
#line 58 "xb.isa"
ostream&
#line 388 "xb.cc"
#line 58 "xb.isa"
sink
#line 391 "xb.cc"
)
{
#line 76 "xb.isa"
	{
		sink << std::dec << nnnnn << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 398 "xb.cc"
}

static Operation *DecodeOpXb_5bit_cst(CodeType const& code, uint16_t addr)
{
	return new OpXb_5bit_cst(code, addr);
}

// TODO: 20080417 - a revoir
#line 62 "xb.isa"
uint16_t
#line 409 "xb.cc"
OpXb_9_cst_z0::getEAddr(
#line 62 "xb.isa"
CPU *
#line 413 "xb.cc"
#line 62 "xb.isa"
cpu
#line 416 "xb.cc"
)
{
#line 83 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + n8;

		return addr;

		//	return cpu->xb_getAddrRegValue(rr) + n8;
	}
#line 428 "xb.cc"
}
#line 58 "xb.isa"
void
#line 432 "xb.cc"
OpXb_9_cst_z0::disasm(
#line 58 "xb.isa"
ostream&
#line 436 "xb.cc"
#line 58 "xb.isa"
sink
#line 439 "xb.cc"
)
{
#line 92 "xb.isa"
	{

		sink << std::dec << n8 << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 447 "xb.cc"
}

static Operation *DecodeOpXb_9_cst_z0(CodeType const& code, uint16_t addr)
{
	return new OpXb_9_cst_z0(code, addr);
}

#line 62 "xb.isa"
uint16_t
#line 457 "xb.cc"
OpXb_16bit_cst_z1::getEAddr(
#line 62 "xb.isa"
CPU *
#line 461 "xb.cc"
#line 62 "xb.isa"
cpu
#line 464 "xb.cc"
)
{
#line 99 "xb.isa"
	{

		physical_address_t addr = cpu->mmc->getPhysicalAddress(cpu->xb_getAddrRegValue(rr) + n16, MEMORY::EXTENDED, WO_GLOBAL_ADDRESS);

		if (s==0) {
			// Constant offset 16-bit signed
			return addr;
			} else {
			// 16-bit offset indexed-indirect

			return cpu->memRead16(addr);
		}
	}
#line 481 "xb.cc"
}
#line 58 "xb.isa"
void
#line 485 "xb.cc"
OpXb_16bit_cst_z1::disasm(
#line 58 "xb.isa"
ostream&
#line 489 "xb.cc"
#line 58 "xb.isa"
sink
#line 492 "xb.cc"
)
{
#line 113 "xb.isa"
	{
		if (s==0) {
			// constant offset 16-bit signed
			sink << std::dec << n16 << "," << CPU::xb_getAddrRegLabel(rr);
			} else {
			// 16-bit offset index-indirect
			sink << "[" << std::dec << n16 << "," << CPU::xb_getAddrRegLabel(rr) << "]";
		}
	}
#line 505 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1(code, addr);
}

#line 62 "xb.isa"
uint16_t
#line 515 "xb.cc"
OpXb_acc_offset::getEAddr(
#line 62 "xb.isa"
CPU *
#line 519 "xb.cc"
#line 62 "xb.isa"
cpu
#line 522 "xb.cc"
)
{
#line 178 "xb.isa"
	{
		uint16_t addrRegVal = cpu->xb_getAddrRegValue(rr);
		uint16_t accRegVal = cpu->xb_getAccRegValue(aa);

		return addrRegVal + accRegVal;
	}
#line 532 "xb.cc"
}
#line 58 "xb.isa"
void
#line 536 "xb.cc"
OpXb_acc_offset::disasm(
#line 58 "xb.isa"
ostream&
#line 540 "xb.cc"
#line 58 "xb.isa"
sink
#line 543 "xb.cc"
)
{
#line 185 "xb.isa"
	{
		sink << CPU::xb_getAccRegLabel(aa) << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 550 "xb.cc"
}

static Operation *DecodeOpXb_acc_offset(CodeType const& code, uint16_t addr)
{
	return new OpXb_acc_offset(code, addr);
}

#line 62 "xb.isa"
uint16_t
#line 560 "xb.cc"
OpXb_accD_offset_idx_ind::getEAddr(
#line 62 "xb.isa"
CPU *
#line 564 "xb.cc"
#line 62 "xb.isa"
cpu
#line 567 "xb.cc"
)
{
#line 192 "xb.isa"
	{

		return cpu->memRead16(cpu->mmc->getPhysicalAddress(cpu->getRegD() + cpu->xb_getAddrRegValue(rr), MEMORY::EXTENDED, WO_GLOBAL_ADDRESS));
	}
#line 575 "xb.cc"
}
#line 58 "xb.isa"
void
#line 579 "xb.cc"
OpXb_accD_offset_idx_ind::disasm(
#line 58 "xb.isa"
ostream&
#line 583 "xb.cc"
#line 58 "xb.isa"
sink
#line 586 "xb.cc"
)
{
#line 197 "xb.isa"
	{
		sink << "[D," << CPU::xb_getAddrRegLabel(rr) << "]";
	}
#line 593 "xb.cc"
}

static Operation *DecodeOpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr)
{
	return new OpXb_accD_offset_idx_ind(code, addr);
}

OpXb_auto::OpXb_auto(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_auto")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 6) & 0x3ULL);
		p = ((_subword_ >> 4) & 0x1ULL);
		nnnn = ((_subword_ >> 0) & 0xfULL);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_5bit_cst::OpXb_5bit_cst(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_5bit_cst")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 6) & 0x3ULL);
		nnnnn = (((int32_t((_subword_ >> 0) & 0x1fLL)) << 27) >> 27);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_9_cst_z0::OpXb_9_cst_z0(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_9_cst_z0")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
		s = ((_subword_ >> 0) & 0x1ULL);
	}
	_code_.pop( 1 );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		n8 = (((int32_t((_subword_ >> 0) & 0xffLL)) << 24) >> 24);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_16bit_cst_z1::OpXb_16bit_cst_z1(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_16bit_cst_z1")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
		s = ((_subword_ >> 0) & 0x1ULL);
	}
	_code_.pop( 1 );
	{
		uint16_t _subword_ = (uint16_t( _code_.str[0] ) << 8) | (uint16_t( _code_.str[1] ) << 0);
		n16 = (((int32_t((_subword_ >> 0) & 0xffffLL)) << 16) >> 16);
	}
	_code_.pop( 2 );
	this->encoding.size -= _code_.size;
}

OpXb_acc_offset::OpXb_acc_offset(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_acc_offset")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
		aa = ((_subword_ >> 0) & 0x3ULL);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_accD_offset_idx_ind::OpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_accD_offset_idx_ind")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

DecodeTableEntry::DecodeTableEntry(CodeType const& opcode, CodeType const& opcode_mask, Operation *(*decode)(CodeType const&, uint16_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

Decoder::Decoder()
{
	little_endian = false;
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe7" ), 1 ), CodeType( (uint8_t*)( "\xe7" ), 1 ), DecodeOpXb_accD_offset_idx_ind));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe4" ), 1 ), CodeType( (uint8_t*)( "\xe4" ), 1 ), DecodeOpXb_acc_offset));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe2" ), 1 ), CodeType( (uint8_t*)( "\xe6" ), 1 ), DecodeOpXb_16bit_cst_z1));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe0" ), 1 ), CodeType( (uint8_t*)( "\xe6" ), 1 ), DecodeOpXb_9_cst_z0));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\x00" ), 1 ), CodeType( (uint8_t*)( "\x20" ), 1 ), DecodeOpXb_5bit_cst));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\x20" ), 1 ), CodeType( (uint8_t*)( "\x20" ), 1 ), DecodeOpXb_auto));
}

Decoder::~Decoder()
{
}

Operation *Decoder::NCDecode(uint16_t addr, CodeType const& code)
{
	Operation *operation;
	unsigned int count = decode_table.size();
	unsigned int idx;
	for(idx = 0; idx < count; idx++)
	{
		if( code.match( decode_table[idx].opcode, decode_table[idx].opcode_mask) )
		{
			operation = decode_table[idx].decode(code, addr);
			return operation;
		}
	}
	operation = new Operation(code, addr, "???");
	return operation;
}

void Decoder::SetLittleEndian()
{
	little_endian = true;
}

void Decoder::SetBigEndian()
{
	little_endian = false;
}

} } } } } }
