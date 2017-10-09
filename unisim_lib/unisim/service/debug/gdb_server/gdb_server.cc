/*
 *  Copyright (c) 2007,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/service/debug/gdb_server/gdb_server.hh>

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

/////////////////////////////// GDBServerBase /////////////////////////////////

bool GDBServerBase::killed = false;

GDBServerBase::GDBServerBase(const char *_name, unisim::kernel::service::Object *_parent)
	: unisim::kernel::service::Object(_name, _parent)
{
}

GDBServerBase::~GDBServerBase()
{
}

void GDBServerBase::SigInt()
{
	killed = true;
}

//////////////////////////////// GDBRegister //////////////////////////////////

GDBRegister::GDBRegister()
	: name()
	, bitsize(0)
	, arch_regs()
	, endian(GDB_LITTLE_ENDIAN)
	, reg_num(0)
	, type()
	, group()
{
}

GDBRegister::GDBRegister(const std::string& reg_name, int reg_bitsize, GDBEndian reg_endian, unsigned int _reg_num, const std::string& _type, const std::string& _group)
	: name(reg_name)
	, bitsize(reg_bitsize)
	, arch_regs()
	, endian(reg_endian)
	, reg_num(_reg_num)
	, type(_type)
	, group(_group)
{
}

void GDBRegister::SetRegisterInterface(unsigned int prc_num, unisim::service::interfaces::Register *reg)
{
	if(prc_num >= arch_regs.size())
	{
		arch_regs.resize(prc_num + 1);
	}
	
	arch_regs[prc_num] = reg;
}

bool GDBRegister::SetValue(unsigned int prc_num, const std::string& hex)
{
	int i;
	unsigned int len = hex.length();
	unsigned int pos = 0;
	int size = bitsize / 8;
	uint8_t value[size];

	if(endian == GDB_BIG_ENDIAN)
	{
#if BYTE_ORDER == BIG_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;
	
			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;
	
			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}

	unisim::service::interfaces::Register *reg = arch_regs[prc_num];
	if(reg)	reg->SetValue(&value);
	return reg != 0;
}

bool GDBRegister::SetValue(unsigned int prc_num, const void *buffer)
{
	unisim::service::interfaces::Register *reg = arch_regs[prc_num];
	if(reg) reg->SetValue(buffer);
	return reg != 0;
}

bool GDBRegister::GetValue(unsigned int prc_num, std::string& hex) const
{
	int i;
	int size = bitsize / 8;
	uint8_t value[size];
	
	hex.erase();
	
	memset(value, 0, size);

	unisim::service::interfaces::Register *reg = arch_regs[prc_num];
	if(reg)
	{
		reg->GetValue(&value);
		
		char c[2];
		c[1] = 0;

		if(endian == GDB_BIG_ENDIAN)
		{
#if BYTE_ORDER == BIG_ENDIAN
			for(i = 0; i < size; i++)
#else
			for(i = size - 1; i >= 0; i--)
#endif
			{
				c[0] = Nibble2HexChar(value[i] >> 4);
				hex += c;
				c[0] = Nibble2HexChar(value[i] & 0xf);
				hex += c;
			}
		}
		else
		{
#if BYTE_ORDER == LITTLE_ENDIAN
			for(i = 0; i < size; i++)
#else
			for(i = size - 1; i >= 0; i--)
#endif
			{
				c[0] = Nibble2HexChar(value[i] >> 4);
				hex += c;
				c[0] = Nibble2HexChar(value[i] & 0xf);
				hex += c;
			}
		}
	}
	else
	{
		for(i = 0; i < size; i++)
		{
			hex += 'x';
			hex += 'x';
		}
	}
	
	return reg != 0;
}

bool GDBRegister::GetValue(unsigned int prc_num, void *buffer) const
{
	int size = bitsize / 8;
	memset(buffer, 0, size);
	unisim::service::interfaces::Register *reg = arch_regs[prc_num];
	if(reg)
		reg->GetValue(buffer);
	return reg != 0;
}

std::ostream& GDBRegister::ToXML(std::ostream& os, unsigned int _reg_num) const
{
	os << "<reg name=\"" << name << "\" bitsize=\"" << bitsize << "\"";
	if(reg_num != _reg_num)
	{
		os << " regnum=\"" << reg_num << "\"";
	}
	if(!type.empty())
	{
		os << " type=\"" << type << "\"";
	}
	if(!group.empty())
	{
		os << " group=\"" << group << "\"";
	}
	os << "/>";
	
	return os;
}

///////////////////////////////// GDBFeature //////////////////////////////////

GDBFeature::GDBFeature(const std::string& _name, unsigned int _id)
	: name(_name)
	, id(_id)
	, gdb_registers()
{
}
void GDBFeature::AddRegister(const GDBRegister *gdb_register)
{
	gdb_registers.push_back(gdb_register);
}

unsigned int GDBFeature::GetId() const
{
	return id;
}

std::ostream& GDBFeature::ToXML(std::ostream& os, std::string req_filename) const
{
	std::stringstream feature_filename_sstr;
	feature_filename_sstr << "feature" << id << ".xml";
	std::string feature_filename(feature_filename_sstr.str());

	if(req_filename == feature_filename)
	{
		os << "<feature name=\"" << name << "\"";

		unsigned int n = gdb_registers.size();
		if(n > 0)
		{
			os << ">";
			
			unsigned int i;
			unsigned int reg_num = (unsigned int) -1;
			for(i = 0; i < n; i++)
			{
				const GDBRegister *gdb_reg = gdb_registers[i];
				if(gdb_reg)
				{
					gdb_reg->ToXML(os, reg_num);
					
					reg_num = gdb_reg->GetRegNum() + 1;
				}
			}
			
			os << "</feature>";
		}
		else
		{
			os << "/>";
		}
	}
	
	return os;
}

std::ostream& operator << (std::ostream& os, const GDBServerError& gdb_server_error)
{
	switch(gdb_server_error)
	{
		case GDB_SERVER_ERROR_EXPECTING_HEX              : os << "expecting hexadecimal value"; break;
		case GDB_SERVER_ERROR_EXPECTING_COMMA            : os << "expecting ','"; break;
		case GDB_SERVER_ERROR_EXPECTING_COLON            : os << "expecting ':'"; break;
		case GDB_SERVER_ERROR_EXPECTING_SEMICOLON        : os << "expecting ';'"; break;
		case GDB_SERVER_ERROR_EXPECTING_ASSIGNMENT       : os << "expecting '='"; break;
		case GDB_SERVER_ERROR_GARBAGE                    : os << "got garbage (extra characters)"; break;
		case GDB_SERVER_ERROR_EXPECTING_THREAD_ID        : os << "expecting thread id"; break;
		case GDB_SERVER_ERROR_INVALID_THREAD_ID          : os << "invalid thread id"; break;
		case GDB_SERVER_ERROR_EXPECTING_OPERATION        : os << "expecting operation"; break;
		case GDB_SERVER_ERROR_INVALID_OPERATION          : os << "invalid operation"; break;
		case GDB_SERVER_ERROR_CANT_DEBUG_PROCESSOR       : os << "can't debug processor"; break;
		case GDB_SERVER_ERROR_UNKNOWN_REGISTER           : os << "unknown register"; break;
		case GDB_SERVER_ERROR_CANT_READ_REGISTER         : os << "can't read register"; break;
		case GDB_SERVER_ERROR_CANT_WRITE_REGISTER        : os << "can't write register"; break;
		case GDB_SERVER_ERROR_CANT_READ_MEMORY           : os << "can't read memory"; break;
		case GDB_SERVER_ERROR_CANT_WRITE_MEMORY          : os << "can't write memory"; break;
		case GDB_SERVER_ERROR_MALFORMED_BINARY_DATA      : os << "malformed binary data"; break;
		case GDB_SERVER_CANT_SET_BREAKPOINT_WATCHPOINT   : os << "can't set breakpoint/watchpoint"; break;
		case GDB_SERVER_CANT_REMOVE_BREAKPOINT_WATCHPOINT: os << "can't remove breakpoint/watchpoint"; break;
		case GDB_SERVER_ERROR_EXPECTING_ACTION           : os << "expecting action"; break;
		default                                          : os << "?"; break;
	}
	
	return os;
}

std::ostream& operator << (std::ostream& os, const GDBServerAction& gdb_server_action)
{
	switch(gdb_server_action)
	{
		case GDB_SERVER_NO_ACTION      : os << "no action"; break;
		case GDB_SERVER_ACTION_STEP    : os << "step"; break;
		case GDB_SERVER_ACTION_CONTINUE: os << "continue"; break;
		default                        : os << "?"; break;
	}
	
	return os;
}

std::ostream& operator << (std::ostream& os, const GDBMode& gdb_mode)
{
	switch(gdb_mode)
	{
		case GDB_MODE_SINGLE_THREAD: os << "SINGLE_THREAD"; break;
		case GDB_MODE_MULTI_THREAD: os << "MULTI_THREAD"; break;
		default          : os << "?"; break;
	}
	
	return os;
}

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace service {

//////////////////// Variable<GDBWaitConnectionMode> //////////////////////////

using unisim::service::debug::gdb_server::GDBWaitConnectionMode;
using unisim::service::debug::gdb_server::GDB_WAIT_CONNECTION_NEVER;
using unisim::service::debug::gdb_server::GDB_WAIT_CONNECTION_STARTUP_ONLY;
using unisim::service::debug::gdb_server::GDB_WAIT_CONNECTION_ALWAYS;

template <> Variable<GDBWaitConnectionMode>::Variable(const char *_name, Object *_object, GDBWaitConnectionMode& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
	AddEnumeratedValue("never");
	AddEnumeratedValue("startup-only");
	AddEnumeratedValue("always");
}

template <>
const char *Variable<GDBWaitConnectionMode>::GetDataTypeName() const
{
	return "gdb-wait-connection-mode";
}

template <>
unsigned int Variable<GDBWaitConnectionMode>::GetBitSize() const
{
	return 2;
}

template <> Variable<GDBWaitConnectionMode>::operator bool () const { return *storage != GDB_WAIT_CONNECTION_NEVER; }
template <> Variable<GDBWaitConnectionMode>::operator long long () const { return *storage; }
template <> Variable<GDBWaitConnectionMode>::operator unsigned long long () const { return *storage; }
template <> Variable<GDBWaitConnectionMode>::operator double () const { return (double)(*storage); }
template <> Variable<GDBWaitConnectionMode>::operator string () const
{
	switch(*storage)
	{
		case GDB_WAIT_CONNECTION_NEVER: return std::string("never");
		case GDB_WAIT_CONNECTION_STARTUP_ONLY: return std::string("startup-only");
		case GDB_WAIT_CONNECTION_ALWAYS: return std::string("always");
	}
	return std::string("?");
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (bool value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (long long value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch(value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch(value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (double value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (const char *value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		if(std::string(value) == std::string("never")) tmp = GDB_WAIT_CONNECTION_NEVER;
		else if(std::string(value) == std::string("startup-only")) tmp = GDB_WAIT_CONNECTION_STARTUP_ONLY;
		else if(std::string(value) == std::string("always")) tmp = GDB_WAIT_CONNECTION_ALWAYS;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<GDBWaitConnectionMode>;

//////////////////////////// Variable<GDBMode> ////////////////////////////////

using unisim::service::debug::gdb_server::GDBMode;
using unisim::service::debug::gdb_server::GDB_MODE_SINGLE_THREAD;
using unisim::service::debug::gdb_server::GDB_MODE_MULTI_THREAD;

template <> Variable<GDBMode>::Variable(const char *_name, Object *_object, GDBMode& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
	AddEnumeratedValue("single-thread");
	AddEnumeratedValue("multi-thread");
}

template <>
const char *Variable<GDBMode>::GetDataTypeName() const
{
	return "gdb-mode";
}

template <>
unsigned int Variable<GDBMode>::GetBitSize() const
{
	return 2;
}

template <> Variable<GDBMode>::operator bool () const { return *storage != GDB_MODE_SINGLE_THREAD; }
template <> Variable<GDBMode>::operator long long () const { return *storage; }
template <> Variable<GDBMode>::operator unsigned long long () const { return *storage; }
template <> Variable<GDBMode>::operator double () const { return (double)(*storage); }
template <> Variable<GDBMode>::operator string () const
{
	switch(*storage)
	{
		case GDB_MODE_SINGLE_THREAD: return std::string("single-thread");
		case GDB_MODE_MULTI_THREAD: return std::string("multi-thread");
	}
	return std::string("?");
}

template <> VariableBase& Variable<GDBMode>::operator = (bool value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (long long value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch(value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch(value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (double value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (const char *value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		if(std::string(value) == std::string("single-thread")) tmp = GDB_MODE_SINGLE_THREAD;
		else if(std::string(value) == std::string("multi-thread")) tmp = GDB_MODE_MULTI_THREAD;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<GDBMode>;

} // end of service namespace
} // end of kernel namespace
} // end of unisim namespace
