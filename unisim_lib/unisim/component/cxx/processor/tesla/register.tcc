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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_TCC__

#include <unisim/component/cxx/processor/tesla/register.hh>
#include <cassert>
#include <ostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {



inline SMType MvSizeToSMType(uint32_t mv_size)
{
	switch(mv_size) {
	case 0:
		return SM_U16;
	case 1:
		return SM_U32;
	case 2:
		return SM_U8;
	default:
		assert(false);
		throw "";
	}
}

inline RegType CvtTypeToRT(ConvType ct)
{
	switch(ct) {
	case CT_U16:
	case CT_S16:
	case CT_U8:	// hack!
	case CT_S8:
		return RT_U16;
	case CT_U32:
	case CT_S32:
	case CT_NONE:
		return RT_U32;
	default:
		assert(false);
		throw "";
	}
}

inline DataType RegTypeToDataType(RegType rt)
{
	switch(rt) {
	case RT_U16:
		return DT_U16;
	case RT_U32:
		return DT_U32;
	default:
		assert(false);
	}
}

inline DataType MvSizeToDataType(uint32_t mv_size)
{
	switch(mv_size) {
	case 0:
		return DT_U16;
	case 1:
		return DT_U32;
	case 2:
		return DT_U8;
	default:
		assert(false);
		throw "";
	}
}

inline DataType CvtTypeToDataType(ConvType ct)
{
	switch(ct) {
	case CT_U16:
		return DT_U16;
	case CT_S16:
		return DT_S16;
	case CT_U8:
		return DT_U8;
	case CT_S8:
		return DT_S8;
	case CT_U32:
	case CT_NONE:
		return DT_U32;
	case CT_S32:
		return DT_S32;
	default:
		assert(false);
		throw "";
	}
}

inline size_t DataTypeSize(DataType dt)
{
	switch(dt) {
	case DT_U8:
	case DT_S8:
		return 1;
	case DT_U16:
	case DT_S16:
		return 2;
	case DT_U32:
	case DT_S32:
	case DT_F32:
		return 4;
	case DT_U64:
		return 8;
	case DT_U128:
		return 16;
	default:
		assert(false);
		throw "";
	}
}

inline DataType SMTypeToDataType(SMType st)
{
	switch(st) {
	case SM_U8:
		return DT_U8;
	case SM_U16:
		return DT_U16;
	case SM_S16:
		return DT_S16;
	case SM_U32:
		return DT_U32;
	default:
		assert(false);
		throw "";
	}
}


template <class CONFIG>
VectorRegister<CONFIG>::VectorRegister()
{
	SetScalar(false);
	SetStrided(false);
	SetScalar16(0, false);
	SetScalar16(1, false);
	SetStrided16(0, false);
	SetStrided16(1, false);
}

template <class CONFIG>
VectorRegister<CONFIG>::VectorRegister(uint32_t val)
{
	SetScalar(true);
	//SetStrided(true);
	//SetScalar16(0, true);
	//SetScalar16(1, true);
	//SetStrided16(0, true);
	//SetStrided16(1, true);
	std::fill(v, v + WARP_SIZE, val);
}

template <class CONFIG>
VectorRegister<CONFIG>::VectorRegister(VectorAddress<CONFIG> const & addr)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		WriteLane(addr[i], i);
	}
	SetScalar(addr.IsScalar());
	SetStrided(addr.IsStrided());
	SetScalar16(0, addr.IsScalar());
	SetScalar16(1, addr.IsScalar());
	SetStrided16(0, false);
	SetStrided16(1, false);
}

template <class CONFIG>
void VectorRegister<CONFIG>::Write(VectorRegister<CONFIG> const & vec, bitset<CONFIG::WARP_SIZE> mask)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			WriteLane(vec[i], i);
		}
	}
	if(mask == ~bitset<CONFIG::WARP_SIZE>(0)) {
		SetScalar(vec.IsScalar());
		SetStrided(vec.IsStrided());
		SetScalar16(false, vec.IsScalar16(false));
		SetStrided16(false, vec.IsStrided16(false));
		SetScalar16(true, vec.IsScalar16(true));
		SetStrided16(true, vec.IsStrided16(true));
	}
	else if(mask != 0) {
		SetScalar(false);
		SetStrided(false);
		SetScalar16(false, false);
		SetStrided16(false, false);
		SetScalar16(true, false);
		SetStrided16(true, false);
		
	}
}

template <class CONFIG>
void VectorRegister<CONFIG>::Write16(VectorRegister<CONFIG> const & vec,
	bitset<CONFIG::WARP_SIZE> mask, int hi)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			if(hi) {
				v[i] = (v[i] & 0x0000ffff) | (vec[i] << 16);
			}
			else {
				v[i] = (v[i] & 0xffff0000) | (vec[i] & 0x0000ffff);
			}
		}
	}
	SetScalar(false);
	SetStrided(false);
	if(mask == ~bitset<CONFIG::WARP_SIZE>(0)) {
		SetScalar16(hi, vec.IsScalar16(false));
		SetStrided16(hi, vec.IsStrided16(false));
	}
	else if(mask != 0) {
		SetScalar16(hi, false);
		SetStrided16(hi, false);
	}
}

template <class CONFIG>
void VectorRegister<CONFIG>::WriteLane(uint32_t val, unsigned int lane)
{
	assert(lane >= 0 && lane < WARP_SIZE);
	v[lane] = val;
}

template <class CONFIG>
uint32_t VectorRegister<CONFIG>::ReadLane(unsigned int lane) const
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
void VectorRegister<CONFIG>::WriteFloat(float val, unsigned int lane)
{
	// TODO: endianness
	union { float f; uint32_t u; } caster;
	caster.f = val;
	WriteLane(caster.u, lane);
}

template <class CONFIG>
float VectorRegister<CONFIG>::ReadFloat(unsigned int lane) const
{
	// TODO: endianness
	union { float f; uint32_t u; } caster;
	caster.u = ReadLane(lane);
	return caster.f;
}

template <class CONFIG>
void VectorRegister<CONFIG>::WriteSimfloat(typename CONFIG::float_t val, unsigned int lane)
{
	WriteLane(val.queryValue(), lane);
}

template <class CONFIG>
typename CONFIG::float_t VectorRegister<CONFIG>::ReadSimfloat(unsigned int lane) const
{
	return typename CONFIG::float_t(ReadLane(lane));
}

template <class CONFIG>
VectorRegister<CONFIG> VectorRegister<CONFIG>::Split(int hilo) const
{
	VectorRegister<CONFIG> vr;
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(hilo) {
			// high part
			vr[i] = (v[i] >> 16);
		}
		else {
			// low part
			vr[i] = (v[i] & 0x0000ffff);
		}
	}
	vr.SetScalar(scalar16[hilo]);
	vr.SetScalar16(0, scalar16[hilo]);
	vr.SetStrided(strided16[hilo]);
	vr.SetStrided16(0, strided16[hilo]);
	return vr;
}

template <class CONFIG>
uint32_t VectorRegister<CONFIG>::operator[] (unsigned int lane) const
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
uint32_t & VectorRegister<CONFIG>::operator[] (unsigned int lane)
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
uint16_t VectorRegister<CONFIG>::Read16(unsigned int lane, bool hi) const
{
	if(hi) {
		return v[lane] >> 16;
	}
	else {
		return v[lane] & 0xffff;
	}
}

template <class CONFIG>
void VectorRegister<CONFIG>::DumpFloat(std::ostream & os)
{
	os << "(";
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE-1; ++i)
	{
		os << ReadFloat(i) << ", ";
	}
	os << ReadFloat(CONFIG::WARP_SIZE-1);
	os << ")";
}

template <class CONFIG>
bool VectorRegister<CONFIG>::CheckScalar() const
{
	assert(CONFIG::WARP_SIZE >= 1);
	uint32_t ref = v[0];
	for(unsigned int i = 1; i != CONFIG::WARP_SIZE; ++i) {
		if(ref != v[i]) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
bool VectorRegister<CONFIG>::CheckScalar16(bool hi) const
{
	assert(CONFIG::WARP_SIZE >= 1);
	uint16_t ref = Read16(0, hi);
	for(unsigned int i = 1; i != CONFIG::WARP_SIZE; ++i) {
		if(ref != Read16(i, hi)) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
bool VectorRegister<CONFIG>::CheckStrided() const
{
	assert(CONFIG::WARP_SIZE >= 2);
	uint32_t base = v[0];
	int32_t stride = v[1] - base;
	for(unsigned int i = 2; i != CONFIG::WARP_SIZE; ++i)
	{
		if(v[i] != base + i * stride) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
bool VectorRegister<CONFIG>::CheckStrided16(bool hi) const
{
	assert(CONFIG::WARP_SIZE >= 2);
	uint16_t base = Read16(0, hi);
	int16_t stride = Read16(1, hi) - base;
	for(unsigned int i = 2; i != CONFIG::WARP_SIZE; ++i)
	{
		if(Read16(i, hi) != base + i * stride) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
std::ostream & operator << (std::ostream & os, VectorRegister<CONFIG> const & r)
{
	os << "(";
	os << std::hex;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE-1; ++i)
	{
		os << r[i] << ", ";
	}
	os << r[CONFIG::WARP_SIZE-1];
	os << std::dec;
	os << ")";
	return os;
}

template <class CONFIG>
VectorAddress<CONFIG>::VectorAddress()
{
	SetScalar(false);
	SetStrided(false);
}

template <class CONFIG>
VectorAddress<CONFIG>::VectorAddress(VectorAddress<CONFIG>::address_t addr)
{
	std::fill(v, v + WARP_SIZE, addr);
	SetScalar(true);
	SetStrided(true);
}

template <class CONFIG>
VectorAddress<CONFIG>::VectorAddress(VectorRegister<CONFIG> const & vr)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		v[i] = vr[i];
	}
	SetScalar(vr.IsScalar());
	SetStrided(vr.IsStrided());
}

template <class CONFIG>
void VectorAddress<CONFIG>::Write(VectorAddress<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		if(mask[i])
			v[i] = vec[i];
	}
}

template <class CONFIG>
void VectorAddress<CONFIG>::Reset()
{
	std::fill(v, v + WARP_SIZE, 0);
	SetScalar(false);
	SetStrided(false);
}

template <class CONFIG>
void VectorAddress<CONFIG>::Increment(DataType dt, size_t imm, std::bitset<CONFIG::WARP_SIZE> mask)
{
	size_t inc = DataTypeSize(dt) * imm;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		if(mask[i])
			v[i] += inc;
	}
}

template <class CONFIG>
typename VectorAddress<CONFIG>::address_t VectorAddress<CONFIG>::operator[] (unsigned int lane) const
{
	assert(lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
typename VectorAddress<CONFIG>::address_t & VectorAddress<CONFIG>::operator[] (unsigned int lane)
{
	assert(lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
VectorAddress<CONFIG> & VectorAddress<CONFIG>::operator+=(VectorAddress<CONFIG> const & other)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		v[i] += other[i];
	}
	return *this;
}

template<class CONFIG>
VectorAddress<CONFIG> operator+(VectorAddress<CONFIG> const & a, VectorAddress<CONFIG> const & b)
{
	VectorAddress<CONFIG> dest;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		dest[i] = a[i] + b[i];
	}
	return dest;
}

template<class CONFIG>
VectorAddress<CONFIG> operator*(unsigned int factor, VectorAddress<CONFIG> const & addr)
{
	VectorAddress<CONFIG> dest;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		dest[i] = factor * addr[i];
	}
	return dest;
}

template <class CONFIG>
std::ostream & operator << (std::ostream & os, VectorAddress<CONFIG> const & r)
{
	os << "(";
	os << std::hex;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE-1; ++i)
	{
		os << r[i] << ", ";
	}
	os << r[CONFIG::WARP_SIZE-1];
	os << std::dec;
	os << ")";
	return os;
}



} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
