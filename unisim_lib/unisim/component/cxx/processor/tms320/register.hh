/*
 *  Copyright (c) 2009,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_REGISTER_HH__

#include <inttypes.h>
#include <iosfwd>
#include <string>
#include <unisim/util/debug/register.hh>

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 96) || __GNUC__ >= 3)
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320 {

class Register
{
public:
	Register();

	inline void SetLo(uint32_t value) INLINE;
	inline uint32_t GetLo() const INLINE;
	inline void SetHi(uint8_t value) INLINE;
	inline uint8_t GetHi() const INLINE;
	void SetFromSinglePresicionFPFormat(uint32_t value);
	void SetFromShortFPFormat(uint16_t value);
	void Float(uint32_t value);
	uint32_t Fix(bool& overflow);
	void Add(const Register& reg, bool& overflow, bool& underflow);
	void Add(uint16_t imm, bool& overflow, bool& underflow);
	void Add(uint32_t imm, bool& overflow, bool& underflow);
	void Add(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow);
	void Add(const Register& reg, uint32_t imm, bool& overflow, bool& underflow);
	void Add(uint32_t imm, const Register& reg, bool& overflow, bool& underflow);
	void Add(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow);
	void Sub(const Register& reg, bool& overflow, bool& underflow);
	void Sub(uint16_t imm, bool& overflow, bool& underflow);
	void Sub(uint32_t imm, bool& overflow, bool& underflow);
	void Sub(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow);
	void Sub(const Register& reg, uint32_t imm, bool& overflow, bool& underflow);
	void Sub(uint32_t imm, const Register& reg, bool& overflow, bool& underflow);
	void Sub(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow);
	void Mpy(const Register& reg, bool& overflow, bool& underflow);
	void Mpy(uint16_t imm, bool& overflow, bool& underflow);
	void Mpy(uint32_t imm, bool& overflow, bool& underflow);
	void Mpy(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow);
	void Mpy(const Register& reg, uint32_t imm, bool& overflow, bool& underflow);
	void Mpy(uint32_t imm, const Register& reg, bool& overflow, bool& underflow);
	void Mpy(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow);
	friend std::ostream& operator << (std::ostream& os, const Register& reg);

	void SetLoWriteMask(uint32_t lo_write_mask);
private:
	uint32_t lo_write_mask; // write mask for the 32 LSBs
	uint32_t lo; // 32 LSB
	uint8_t hi;  // 8 MSB

	void Add(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow);
	void Sub(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow);
	void AddSub(bool add, uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow);
	void Mpy(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow);
};

class RegisterDebugInterface : public unisim::util::debug::Register
{
public:
	RegisterDebugInterface(const char *name, unisim::component::cxx::processor::tms320::Register *reg, bool extended_precision = false);
	virtual ~RegisterDebugInterface();
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	std::string name;
	bool extended_precision;
	unisim::component::cxx::processor::tms320::Register *reg;
};

class RegisterBitFieldDebugInterface : public unisim::util::debug::Register
{
public:
	RegisterBitFieldDebugInterface(const char *name, unisim::component::cxx::processor::tms320::Register *reg, unsigned int bit_offset, unsigned int bit_size = 1);
	virtual ~RegisterBitFieldDebugInterface();
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	std::string name;
	unisim::component::cxx::processor::tms320::Register *reg;
	unsigned int bit_offset;
	unsigned int bit_size;
};


inline void Register::SetLo(uint32_t value)
{
	lo = value & lo_write_mask;
}

inline uint32_t Register::GetLo() const
{
	return lo;
}

inline void Register::SetHi(uint8_t value)
{
	hi = value;
}

inline uint8_t Register::GetHi() const
{
	return hi;
}

} // end of namespace tms320
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_REGISTER_HH__