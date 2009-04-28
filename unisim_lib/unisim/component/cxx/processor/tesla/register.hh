/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_HH__

#include <bitset>
#include <iosfwd>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

//=====================================================================
//=                               Enums                               =
//=====================================================================


enum DataType
{
	DT_U8 = 0,
	DT_S8 = 1,
	DT_U16 = 2,
	DT_S16 = 3,
	DT_U64 = 4,
	DT_U128 = 5,
	DT_U32 = 6,
	DT_S32 = 7,
	DT_F32,		// Not in Tesla ISA
	DT_NONE,	//
	DT_UNKNOWN	//
};

enum ConvType
{
	CT_U16 = 0,
	CT_U32 = 1,
	CT_U8 = 2,
	
	CT_S16 = 4,
	CT_S32 = 5,
	CT_S8 = 6,
	CT_NONE = 7	// Not in Decuda
};

enum RoundingMode
{
	RM_RN = 0,
	RM_RD = 1,
	RM_RU = 2,
	RM_RZ = 3
};

enum SMType
{
	SM_U8 = 0,
	SM_U16 = 1,
	SM_S16 = 2,
	SM_U32 = 3
};

// Does not match any ISA field
// ... except b32.
enum RegType
{
	RT_U16 = 0,
	RT_U32 = 1,
	RT_U64,		// 2 regs, Not in ISA
};

enum AbsSat
{
	AS_NONE = 0,
	AS_SAT = 1,
	AS_ABS = 2,
	AS_SSAT = 3
};

inline SMType MvSizeToSMType(uint32_t mv_size);
inline RegType CvtTypeToRT(ConvType ct);
inline DataType RegTypeToDataType(RegType rt);
inline DataType MvSizeToDataType(uint32_t mv_size);
inline DataType CvtTypeToDataType(ConvType ct);
inline size_t DataTypeSize(DataType dt);
inline DataType SMTypeToDataType(SMType st);

template <class CONFIG>
struct VectorAddress;

// Inherit from valarray<CONFIG::reg_t>??
template <class CONFIG>
struct VectorRegister
{
	static unsigned int const WARP_SIZE = CONFIG::WARP_SIZE;
	typedef typename CONFIG::reg_t reg_t;
	VectorRegister();
	VectorRegister(uint32_t val);
	VectorRegister(VectorAddress<CONFIG> const & addr);
	void NegateFP32();
	void Write(VectorRegister<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask);
	void Write16(VectorRegister<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask, int hi);
	
	void WriteLane(uint32_t val, unsigned int lane);
	uint32_t ReadLane(unsigned int lane) const;
	void WriteFloat(float val, unsigned int lane);
	float ReadFloat(unsigned int lane) const;
	void WriteSimfloat(typename CONFIG::float_t val, unsigned int lane);
	typename CONFIG::float_t ReadSimfloat(unsigned int lane) const;
	VectorRegister<CONFIG> Split(int hilo) const;
	void DumpFloat(std::ostream & os);
	
	uint32_t operator[] (unsigned int lane) const;
	uint32_t & operator[] (unsigned int lane);

	reg_t v[WARP_SIZE];
};

template <class CONFIG>
std::ostream & operator << (std::ostream & os, VectorRegister<CONFIG> const & r);

template <class CONFIG>
struct VectorAddress
{
	static unsigned int const WARP_SIZE = CONFIG::WARP_SIZE;
	typedef typename CONFIG::address_t address_t;
	
	VectorAddress();
	VectorAddress(address_t addr);
	VectorAddress(VectorRegister<CONFIG> const & vr);

	void Write(VectorAddress<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask);
	
	void Reset();
	address_t operator[] (unsigned int lane) const;
	address_t & operator[] (unsigned int lane);
	
	VectorAddress<CONFIG> & operator+=(VectorAddress<CONFIG> const & other);
	
	void Increment(DataType dt, size_t imm, std::bitset<CONFIG::WARP_SIZE> mask);
	
	address_t v[WARP_SIZE];
};

template<class CONFIG>
VectorAddress<CONFIG> operator+(VectorAddress<CONFIG> const & a, VectorAddress<CONFIG> const & b);

template<class CONFIG>
VectorAddress<CONFIG> operator*(unsigned int factor, VectorAddress<CONFIG> const & addr);

template <class CONFIG>
std::ostream & operator << (std::ostream & os, VectorAddress<CONFIG> const & r);

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
