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
#include <unisim/util/debug/dwarf/machine_state.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
class DWARF_RegisterRef
{
public:
	DWARF_RegisterRef(const DWARF_Frame<MEMORY_ADDR> *dw_frame, unsigned int dw_reg_num);
	DWARF_Register<MEMORY_ADDR> *GetRegister() const;
private:
	const DWARF_Frame<MEMORY_ADDR> *dw_frame;
	unsigned int dw_reg_num;
};

template <class MEMORY_ADDR>
class DWARF_Register
{
public:
	DWARF_Register(unisim::service::interfaces::Register *reg);
	virtual ~DWARF_Register();
	virtual const char *GetName() const;
	virtual int GetSize() const;
	virtual bool GetValue(void *buffer) const;
	virtual bool SetValue(const void *buffer);
	bool GetValue(uint8_t& val) const;
	bool SetValue(const uint8_t& val);
	bool GetValue(uint16_t& val) const;
	bool SetValue(const uint16_t& val);
	bool GetValue(uint32_t& val) const;
	bool SetValue(const uint32_t& val);
	bool GetValue(uint64_t& val) const;
	bool SetValue(const uint64_t& val);
	void Print(std::ostream& os) const;
	unsigned int GetRegisterNumber() const;
	bool Snap();
	bool Commit();

private:
	template <typename T> bool GetTypedValue(T& val) const;
	template <typename T> bool SetTypedValue(T& val) const;
protected:
	unisim::service::interfaces::Register *reg;
	std::vector<uint8_t> snapshot;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Register<MEMORY_ADDR>& reg);

template <class MEMORY_ADDR>
class DWARF_RenameRegister : public DWARF_Register<MEMORY_ADDR>
{
public:
	DWARF_RenameRegister(unisim::service::interfaces::Register *reg, const DWARF_RegisterRef<MEMORY_ADDR>& dw_reg_ref);
	virtual int GetSize() const;
	virtual bool GetValue(void *buffer) const;
	virtual bool SetValue(const void *buffer);
private:
	DWARF_RegisterRef<MEMORY_ADDR> dw_reg_ref;
};

template <class MEMORY_ADDR>
class DWARF_ValueRegister : public DWARF_Register<MEMORY_ADDR>
{
public:
	DWARF_ValueRegister(unisim::service::interfaces::Register *reg, int size, const uint64_t& value);
	virtual int GetSize() const;
	virtual bool GetValue(void *buffer) const;
	virtual bool SetValue(const void *buffer);
private:
	int size;
	uint64_t value;
};

template <class MEMORY_ADDR>
class DWARF_SpilledRegister : public DWARF_Register<MEMORY_ADDR>
{
public:
	DWARF_SpilledRegister(unisim::service::interfaces::Register *reg, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size, unisim::util::endian::endian_type endianness);
	virtual bool GetValue(void *buffer) const;
	virtual bool SetValue(const void *buffer);
private:
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	MEMORY_ADDR addr;
	unsigned int size;
	unisim::util::endian::endian_type endianness;
};

template <class MEMORY_ADDR>
class DWARF_RegSet
{
public:
	DWARF_RegSet(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num);
	~DWARF_RegSet();
	bool LoadArchRegs();
	void DefRegister(unsigned int dw_reg_num, DWARF_Register<MEMORY_ADDR> *dw_reg);
	void UndefRegister(unsigned int dw_reg_num);
	template <typename T> bool ReadRegister(unsigned int dw_reg_num, T& reg_value) const;
	template <typename T> bool WriteRegister(unsigned int dw_reg_num, T value);
	bool ReadProgramCounterRegister(MEMORY_ADDR& pc) const;
	void DefProgramCounterRegister(DWARF_Register<MEMORY_ADDR> *pc_reg);
	DWARF_Register<MEMORY_ADDR> *GetRegister(unsigned int reg_num) const;
	DWARF_Register<MEMORY_ADDR> *GetProgramCounterRegister() const;
	void Print(std::ostream& os) const;
	bool Commit();
private:
	DWARF_RegisterNumberMapping *dw_reg_num_mapping;
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
	DWARF_Frame(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num);
	DWARF_Frame(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, const DWARF_Handler<MEMORY_ADDR> *dw_handler, const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, unsigned int dw_ret_addr_reg_num);
	~DWARF_Frame();
	
	bool LoadArchRegs();
	bool ComputeCFA(const DWARF_Handler<MEMORY_ADDR> *dw_handler, const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame);
	bool Unwind();
	MEMORY_ADDR ReadCFA() const;
	DWARF_Register<MEMORY_ADDR> *GetRegister(unsigned int reg_num) const; 
	DWARF_Register<MEMORY_ADDR> *GetProgramCounterRegister() const;
	template <typename T> bool ReadRegister(unsigned int reg_num, T& reg_value) const;
	template <typename T> bool WriteRegister(unsigned int dw_reg_num, T value);
	unsigned int GetRegisterSize(unsigned int reg_num) const;
	const char *GetRegisterName(unsigned int reg_num) const;
	bool ReadProgramCounterRegister(MEMORY_ADDR& pc) const;
	const DWARF_RegSet<MEMORY_ADDR>& GetRegSet() const;
	const DWARF_MachineState<MEMORY_ADDR> *GetMachineState() const;
	unsigned int GetProcessorNumber() const;
	const DWARF_Frame<MEMORY_ADDR> *GetInnerFrame() const;
	template <class VALUE_TYPE> bool Load(MEMORY_ADDR addr, VALUE_TYPE& value, unsigned int read_size, unisim::util::endian::endian_type endianness, unsigned int addr_space) const;
	bool Commit();
private:
	const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state;
	unsigned int prc_num;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	MEMORY_ADDR cfa;
	mutable DWARF_RegSet<MEMORY_ADDR> dw_reg_set;
	const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_RegisterNumberMapping *dw_reg_num_mapping;
	const DWARF_CFIRow<MEMORY_ADDR> *cfi_row;
	unsigned int dw_ret_addr_reg_num;
	bool has_sp_reg_num;
	unsigned int sp_reg_num;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
