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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_FPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_FPU_HH__

#include <unisim/component/c++/processor/powerpc/floating.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/service/interfaces/logger.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/kernel/service/service.hh>
#include <map>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using std::map;
using unisim::service::interfaces::Logger;
using unisim::service::interfaces::Registers;
using unisim::util::debug::Register;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Object;

const uint32_t FPRF_FOR_QNAN = 0x11UL;
const uint32_t FPRF_FOR_NEGATIVE_INFINITY = 0x9UL;
const uint32_t FPRF_FOR_NEGATIVE_NORMAL = 0x8UL;
const uint32_t FPRF_FOR_NEGATIVE_DENORMAL = 0x18UL;
const uint32_t FPRF_FOR_NEGATIVE_ZERO = 0x12UL;
const uint32_t FPRF_FOR_POSITIVE_ZERO = 0x2UL;
const uint32_t FPRF_FOR_POSITIVE_DENORMAL = 0x14UL;
const uint32_t FPRF_FOR_POSITIVE_NORMAL = 0x4UL;
const uint32_t FPRF_FOR_POSITIVE_INFINITY = 0x5UL;

class FloatingPointRegisterInterface : public unisim::util::debug::Register
{
public:
	FloatingPointRegisterInterface(const char *name, SoftDouble *value);
	virtual ~FloatingPointRegisterInterface();
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	string name;
	SoftDouble *value;
};

template <class CONFIG>
class FPU :
	public Client<Logger>,
	public unisim::service::interfaces::Registers
{
public:
	ServiceImport<Logger> logger_import;
	
	FPU(const char *name, Object *parent = 0);
	virtual ~FPU();
	
	virtual unisim::util::debug::Register *GetRegister(const char *name);
	void Reset();
	inline uint32_t GetFPSCR() { return fpscr; }
	inline void SetFPSCR(uint32_t value) { fpscr = value & 0xfffff7ffUL; flags.setRoundingMode(GetFPSCR_RN() & 3); }

	/* Methods for reading FPSCR bits */
	inline uint32_t GetFPSCR_FX() { return ((fpscr >> 31) & 1); }
	inline uint32_t GetFPSCR_FEX() { return  ((fpscr >> 30) & 1); }
	inline uint32_t GetFPSCR_VX() { return  ((fpscr >> 29) & 1); }
	inline uint32_t GetFPSCR_OX() { return  ((fpscr >> 28) & 1); }
	inline uint32_t GetFPSCR_UX() { return  ((fpscr >> 27) & 1); }
	inline uint32_t GetFPSCR_ZX() { return  ((fpscr >> 26) & 1); }
	inline uint32_t GetFPSCR_XX() { return  ((fpscr >> 25) & 1); }
	inline uint32_t GetFPSCR_VXSNAN() { return  ((fpscr >> 24) & 1); }
	inline uint32_t GetFPSCR_VXISI() { return ((fpscr >> 23) & 1); }
	inline uint32_t GetFPSCR_VXIDI() { return ((fpscr >> 22) & 1); }
	inline uint32_t GetFPSCR_VXZDZ() { return ((fpscr >> 21) & 1); }
	inline uint32_t GetFPSCR_VXIMZ() { return ((fpscr >> 20) & 1); }
	inline uint32_t GetFPSCR_VXVC() { return ((fpscr >> 19) & 1); }
	inline uint32_t GetFPSCR_FR() { return ((fpscr >> 18) & 1); }
	inline uint32_t GetFPSCR_FI() { return ((fpscr >> 17) & 1); }
	inline uint32_t GetFPSCR_FPRF() { return ((fpscr >> 12) & 0x1f); }
	inline uint32_t GetFPSCR_FPCC() { return ((fpscr >> 12) & 0xf); }
	inline uint32_t GetFPSCR_VXSOFT() { return ((fpscr >> 10) & 1); }
	inline uint32_t GetFPSCR_VXSQRT() { return ((fpscr >> 9) & 1); }
	inline uint32_t GetFPSCR_VXCVI() { return ((fpscr >> 8) & 1); }
	inline uint32_t GetFPSCR_VE() { return ((fpscr >> 7) & 1); }
	inline uint32_t GetFPSCR_OE() { return ((fpscr >> 6) & 1); }
	inline uint32_t GetFPSCR_UE() { return ((fpscr >> 5) & 1); }
	inline uint32_t GetFPSCR_ZE() { return ((fpscr >> 4) & 1); }
	inline uint32_t GetFPSCR_XE() { return ((fpscr >> 3) & 1); }
	inline uint32_t GetFPSCR_NI() { return ((fpscr >> 2) & 1); }
	inline uint32_t GetFPSCR_RN() { return (fpscr & 3); }

	inline void SetFPSCR_FPRF(uint32_t value) { fpscr = (fpscr & 0xfffe0fffUL) | ((value & 0x1f) << 12); }
	
	/* Methods for selectively setting FPSCR bits */
	inline void SetFPSCR_FX() { fpscr = fpscr | 0x80000000UL; }
	inline void SetFPSCR_FEX() { fpscr = fpscr | 0x40000000UL; }
	inline void SetFPSCR_VX() { fpscr = fpscr | 0x20000000UL; }
	inline void SetFPSCR_OX() { fpscr = fpscr | 0x10000000UL; }
	inline void SetFPSCR_UX() { fpscr = fpscr | 0x08000000UL; }
	inline void SetFPSCR_ZX() { fpscr = fpscr | 0x04000000UL; }
	inline void SetFPSCR_XX() { fpscr = fpscr | 0x02000000UL; }
	inline void SetFPSCR_VXSNAN() { fpscr = fpscr | 0x01000000UL; }
	inline void SetFPSCR_VXISI() { fpscr = fpscr | 0x00800000UL; }
	inline void SetFPSCR_VXIDI() { fpscr = fpscr | 0x00400000UL; }
	inline void SetFPSCR_VXZDZ() { fpscr = fpscr | 0x00200000UL; }
	inline void SetFPSCR_VXIMZ() { fpscr = fpscr | 0x00100000UL; }
	inline void SetFPSCR_VXVC() { fpscr = fpscr | 0x00080000UL; }
	inline void SetFPSCR_FR() { fpscr = fpscr | 0x00040000UL; }
	inline void SetFPSCR_FI() { fpscr = fpscr | 0x00020000UL; }
	inline void SetFPSCR_FPCC(uint32_t c) { fpscr = (fpscr & 0xffff0fffUL) | ((c & 0xf) << 12); }
	inline void SetFPSCR_VXSOFT() { fpscr = fpscr | 0x00000400UL; }
	inline void SetFPSCR_VXSQRT() { fpscr = fpscr | 0x00000200UL; }
	inline void SetFPSCR_VXCVI() { fpscr = fpscr | 0x00000100UL; }
	inline void SetFPSCR_VE() { fpscr = fpscr | 0x00000080UL; }
	inline void SetFPSCR_OE() { fpscr = fpscr | 0x00000040UL; }
	inline void SetFPSCR_UE() { fpscr = fpscr | 0x00000020UL; }
	inline void SetFPSCR_ZE() { fpscr = fpscr | 0x00000010UL; }
	inline void SetFPSCR_XE() { fpscr = fpscr | 0x00000008UL; }
	inline void SetFPSCR_NI() { fpscr = fpscr | 0x00000004UL; }
	inline void SetFPSCR_RN(uint32_t mode) { fpscr = (fpscr & 0xfffffffcUL) | (mode & 0x3); flags.setRoundingMode(mode & 3); }
	
	/* Methods for selectively resetting READ_FPSCR bits */
	inline void ResetFPSCR_FX() { fpscr = fpscr & 0x7fffffffUL; }
	inline void ResetFPSCR_FEX() { fpscr = fpscr & 0xbfffffffUL; }
	inline void ResetFPSCR_VX() { fpscr = fpscr & 0xdfffffffUL; }
	inline void ResetFPSCR_OX() { fpscr = fpscr & 0xefffffffUL; }
	inline void ResetFPSCR_UX() { fpscr = fpscr & 0xf7ffffffUL; }
	inline void ResetFPSCR_ZX() { fpscr = fpscr & 0xfbffffffUL; }
	inline void ResetFPSCR_XX() { fpscr = fpscr & 0xfdffffffUL; }
	inline void ResetFPSCR_VXSNAN() { fpscr = fpscr & 0xfeffffffUL; }
	inline void ResetFPSCR_VXISI() { fpscr = fpscr & 0xff7fffffUL; }
	inline void ResetFPSCR_VXIDI() { fpscr = fpscr & 0xffbfffffUL; }
	inline void ResetFPSCR_VXZDZ() { fpscr = fpscr & 0xffdfffffUL; }
	inline void ResetFPSCR_VXIMZ() { fpscr = fpscr & 0xffefffffUL; }
	inline void ResetFPSCR_VXVC() { fpscr = fpscr & 0xfff7ffffUL; }
	inline void ResetFPSCR_FR() { fpscr = fpscr & 0xfffbffffUL; }
	inline void ResetFPSCR_FI() { fpscr = fpscr & 0xfffdffffUL; }
	inline void ResetFPSCR_VXSOFT() { fpscr = fpscr & 0xfffffbffUL; }
	inline void ResetFPSCR_VXSQRT() { fpscr = fpscr & 0xfffffdffUL; }
	inline void ResetFPSCR_VXCVI() { fpscr = fpscr & 0xfffffeffUL; }
	inline void ResetFPSCR_VE() { fpscr = fpscr & 0xffffff7fUL; }
	inline void ResetFPSCR_OE() { fpscr = fpscr & 0xffffffbfUL; }
	inline void ResetFPSCR_UE() { fpscr = fpscr & 0xffffffdfUL; }
	inline void ResetFPSCR_ZE() { fpscr = fpscr & 0xffffffefUL; }
	inline void ResetFPSCR_XE() { fpscr = fpscr & 0xfffffff7UL; }
	inline void ResetFPSCR_NI() { fpscr = fpscr & 0xfffffffbUL; }

	
	// Floating point computations
	void Fp32Add(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp32Sub(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp32Mul(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp32Div(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp32EstimateInv(unsigned int fd, unsigned int fa);
	void Fp32MulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp32MulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp32NegMulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp32NegMulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp64Add(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp64Sub(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp64Mul(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp64Div(unsigned int fd, unsigned int fa, unsigned int fb);
	void Fp64EstimateInvSqrt(unsigned int fd, unsigned int fa);
	void Fp64MulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp64MulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp64NegMulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp64NegMulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc);
	void Fp64Neg(unsigned int fd, unsigned int fa);
	void Fp64Abs(unsigned int fd, unsigned int fa);
	void Fp64NegAbs(unsigned int fd, unsigned int fa);
	uint32_t Fp64CompareOrdered(unsigned int fa, unsigned int fb);
	uint32_t Fp64CompareUnordered(unsigned int fa, unsigned int fb);
	void Fp64ToInt32(unsigned int fd, unsigned int fa);
	void Fp64ToInt32TowardZero(unsigned int fd, unsigned int fa);
	void Fp64Select(unsigned int fd, unsigned int fa, unsigned int fb, unsigned int fc);
	void Fp64ToFp32(unsigned int fd, unsigned int fa);
	void Fp64Move(unsigned int fd, unsigned int fa);
	uint64_t GetFp64(unsigned int n);
	void SetFp64(unsigned int n, uint64_t value);
	uint32_t GetFp32(unsigned int n);
	void SetFp32(unsigned int n, uint32_t value);
	
	void EnableException();
	void DisableException();
private:
	// Compute the FPSCR bits
	void ComputeFPSCR_FEX();
	void ComputeFPSCR_FPRF(const SoftFloat& result);
	void ComputeFPSCR_FPRF(const SoftDouble& result);
	void ComputeFPSCR_FI(const Flags& flags);
	void ComputeFPSCR_OX(const Flags& flags);
	void ComputeFPSCR_UX(const Flags& flags);
	void ComputeFPSCR_ZX(const Flags& flags);
	void ComputeFPSCR_VXSNAN(const Flags& flags);
	void ComputeFPSCR_VXISI(const Flags& flags);
	void ComputeFPSCR_VXIDI(const Flags& flags);
	void ComputeFPSCR_VXZDZ(const Flags& flags);
	void ComputeFPSCR_VXIMZ(const Flags& flags);
	void ComputeFPSCR_FR(const SoftFloat& result, const Flags& flags);
	void ComputeFPSCR_VXCVI(const Flags& flags);
	void ComputeFPSCR_FR(const SoftDouble& result, const Flags& flags);
	
	SoftDouble fpr[32];	/*< floating point registers (C++ objects implementing IEEE 754 floating point numbers) */
	uint32_t fpscr;		/*< floating point status and control register */
	Flags flags;
	bool exception_enabled;

	map<string, unisim::util::debug::Register *> registers_registry;

	//=====================================================================
	//=                             Debug stuff                           =
	//=====================================================================
	bool fp32_estimate_inv_warning;
	bool fp64_estimate_inv_sqrt_warning;
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
