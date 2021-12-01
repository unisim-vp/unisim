/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FRAME_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_FRAME_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
class DWARF_Register : public unisim::service::interfaces::Register
{
public:
	DWARF_Register(unisim::service::interfaces::Register *reg, unisim::util::endian::endian_type endianness);
	virtual const char *GetName() const;
	virtual const char *GetDescription() const;
	virtual int GetSize() const;
	virtual void Copy(const unisim::service::interfaces::Register *reg) = 0;
	virtual bool Load(unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size) = 0;
protected:
	unisim::service::interfaces::Register *reg;
	unisim::util::endian::endian_type endianness;
};

template <class MEMORY_ADDR, class VALUE_TYPE>
class DWARF_CopyRegister : public DWARF_Register<MEMORY_ADDR>
{
public:
	DWARF_CopyRegister(unisim::service::interfaces::Register *reg, unisim::util::endian::endian_type endianness);
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual void Copy(const unisim::service::interfaces::Register *from);
	virtual bool Load(unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size);
private:
	VALUE_TYPE value;
};

template <class MEMORY_ADDR>
class DWARF_PassThroughRegister : public DWARF_Register<MEMORY_ADDR>
{
public:
	DWARF_PassThroughRegister(unisim::service::interfaces::Register *reg, unisim::util::endian::endian_type endianness);
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual void Copy(const unisim::service::interfaces::Register *from);
	virtual bool Load(unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size);
};

template <class MEMORY_ADDR>
class DWARF_RegSet
{
public:
	DWARF_RegSet(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num);
	~DWARF_RegSet();
	bool LoadArchRegs();
	void Copy(const DWARF_RegSet& o);
	DWARF_Register<MEMORY_ADDR> *DefRegister(unsigned int reg_num, unisim::service::interfaces::Register *reg);
	DWARF_Register<MEMORY_ADDR> *DefPassThroughRegister(unsigned int reg_num, unisim::service::interfaces::Register *reg);
	void UndefRegister(unsigned int reg_num);
	template <typename T> bool ReadRegister(unsigned int reg_num, T& reg_value) const;
	template <typename T> bool WriteRegister(unsigned int reg_num, T value);
	void DefProgramCounterRegister(DWARF_Register<MEMORY_ADDR> *ret_addr_reg);
	DWARF_Register<MEMORY_ADDR> *GetRegister(unsigned int reg_num) const;
	unisim::service::interfaces::Register *GetProgramCounterRegister() const;
	bool LoadRegister(unsigned int reg_num, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size);
	void Print(std::ostream& os) const;
private:
	DWARF_RegisterNumberMapping *reg_num_mapping;
	unisim::util::endian::endian_type endianness;
	typedef std::map<unsigned int, DWARF_Register<MEMORY_ADDR> *> RegSet;
	RegSet reg_set;
	DWARF_Register<MEMORY_ADDR> *program_counter;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegSet<MEMORY_ADDR>& dw_reg_set);

template <class MEMORY_ADDR>
class DWARF_Frame
{
public:
	DWARF_Frame(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num);
	~DWARF_Frame();
	
	bool LoadArchRegs();
	bool ComputeCFA(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame);
	bool Unwind(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame, unsigned int dw_ret_addr_reg_num);
	MEMORY_ADDR ReadCFA() const;
	unisim::service::interfaces::Register *GetRegister(unsigned int reg_num) const; 
	unisim::service::interfaces::Register *GetProgramCounterRegister() const;
	template <typename T> bool ReadRegister(unsigned int reg_num, T& reg_value) const;
	const DWARF_RegSet<MEMORY_ADDR>& GetRegSet() const;
	unsigned int GetProcessorNumber() const;
private:
	const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	unsigned int prc_num;
	bool has_sp_reg_num;
	unsigned int sp_reg_num;
	unsigned int dw_ret_addr_reg_num;
	unisim::util::endian::endian_type endianness;
	unsigned int address_size;
	DW_CFA_Specification dw_cfa_spec;
	DW_CFA_RegRuleOffsetSpecification dw_cfa_reg_rule_offset_spec;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	MEMORY_ADDR cfa;
	DWARF_RegSet<MEMORY_ADDR> dw_reg_set;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
