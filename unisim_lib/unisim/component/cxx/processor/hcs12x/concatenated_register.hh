/*
 *  Copyright (c) 2008,
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
 * Authors: Reda NOUACER (reda.nouacer@cea.fr)
 */
 

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONCATENATED_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONCATENATED_REGISTER_HH__

#include <unisim/util/debug/register.hh>
#include <string>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using std::string;
using unisim::util::debug::Register;

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
class ConcatenatedRegister : public Register
{
public:
	ConcatenatedRegister(const char *name, SUB_REGISTER_TYPE *_regHigh, SUB_REGISTER_TYPE *_regLow);
	virtual ~ConcatenatedRegister();
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	string name;
//	REGISTER_TYPE *value;
	SUB_REGISTER_TYPE *regHigh;
	SUB_REGISTER_TYPE *regLow;
};

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegister<REGISTER_TYPE, SUB_REGISTER_TYPE>::ConcatenatedRegister(const char *_name, SUB_REGISTER_TYPE *_regHigh, SUB_REGISTER_TYPE *_regLow) :
	name(_name),
	regHigh(_regHigh),
	regLow(_regLow)
{
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegister<REGISTER_TYPE, SUB_REGISTER_TYPE>::~ConcatenatedRegister()
{
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
const char *ConcatenatedRegister<REGISTER_TYPE, SUB_REGISTER_TYPE>::GetName() const
{
	return name.c_str();
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
void ConcatenatedRegister<REGISTER_TYPE, SUB_REGISTER_TYPE>::GetValue(void *buffer) const
{

	*(REGISTER_TYPE *) buffer =  (REGISTER_TYPE ) ((REGISTER_TYPE ) *regHigh << (sizeof(SUB_REGISTER_TYPE)*8)) | *regLow;
	*(REGISTER_TYPE *) buffer = LittleEndian2Host(*(REGISTER_TYPE *) buffer);

}


template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
void ConcatenatedRegister<REGISTER_TYPE, SUB_REGISTER_TYPE>::SetValue(const void *buffer)
{
	REGISTER_TYPE value = Host2LittleEndian(*(REGISTER_TYPE *) buffer);
	
	*(SUB_REGISTER_TYPE *) regHigh = (SUB_REGISTER_TYPE) (value) >> sizeof(SUB_REGISTER_TYPE);
	*(SUB_REGISTER_TYPE *) regLow = (SUB_REGISTER_TYPE) (value);	
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
int ConcatenatedRegister<REGISTER_TYPE, SUB_REGISTER_TYPE>::GetSize() const
{
	return sizeof(REGISTER_TYPE);
}

// ***********************************************

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
class ConcatenatedRegisterView : public unisim::kernel::service::VariableBase
{
public:

	ConcatenatedRegisterView(const char *name, unisim::kernel::service::Object *owner, SUB_REGISTER_TYPE *_regHigh, SUB_REGISTER_TYPE *_regLow, const char *description);
	virtual ~ConcatenatedRegisterView();
	virtual const char *GetDataTypeName() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator std::string () const;
	virtual unisim::kernel::service::VariableBase& operator = (bool value);
	virtual unisim::kernel::service::VariableBase& operator = (long long value);
	virtual unisim::kernel::service::VariableBase& operator = (unsigned long long value);
	virtual unisim::kernel::service::VariableBase& operator = (double value);
	virtual unisim::kernel::service::VariableBase& operator = (const char * value);
private:
	SUB_REGISTER_TYPE* regHigh;
	SUB_REGISTER_TYPE* regLow;
};


template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::ConcatenatedRegisterView(const char *name, unisim::kernel::service::Object *owner, SUB_REGISTER_TYPE *_regHigh, SUB_REGISTER_TYPE *_regLow, const char *description) :
	unisim::kernel::service::VariableBase(name, owner, unisim::kernel::service::VariableBase::VAR_REGISTER, description)
	, regHigh(_regHigh)
	, regLow(_regLow)
{

}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::~ConcatenatedRegisterView() {

}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
const char *ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::GetDataTypeName() const {
	switch(sizeof(SUB_REGISTER_TYPE))
	{
	case 1: return "16-bit concatenated register";
	case 2: return "32-bit concatenated register";
	case 4: return "64-bit concatenated register";
	default: throw std::runtime_error("Internal error");
	}
	return "?";
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>:: operator bool () const {
	return *regHigh || *regLow;
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator long long () const {
	switch(sizeof(SUB_REGISTER_TYPE))
	{
	case 1: return (long long) (int16_t) (((uint16_t) *regHigh) << 8) | ((uint16_t) *regLow);
	case 2: return (long long) (int32_t) (((uint32_t) *regHigh) << 16) | ((uint32_t) *regLow);
	case 4: return (long long) (int64_t) (((uint64_t) *regHigh) << 32) | ((uint64_t) *regLow);
	default: throw std::runtime_error("Internal error");
	}
	return 0;
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator unsigned long long () const {
	switch(sizeof(SUB_REGISTER_TYPE))
	{
	case 1: return (unsigned long long) (((uint16_t) *regHigh) << 8) | ((uint16_t) *regLow);
	case 2: return (unsigned long long) (((uint32_t) *regHigh) << 16) | ((uint32_t) *regLow);
	case 4: return (unsigned long long) (((uint64_t) *regHigh) << 32) | ((uint64_t) *regLow);
	default: throw std::runtime_error("Internal error");
	}
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator double () const {
	switch(sizeof(SUB_REGISTER_TYPE))
	{
	case 1: return (double) (int16_t) ((((uint16_t) *regHigh) << 8) | ((uint16_t) *regLow));
	case 2: return (double) (int32_t) ((((uint32_t) *regHigh) << 16) | ((uint32_t) *regLow));
	case 4: return (double) (int64_t) ((((uint64_t) *regHigh) << 32) | ((uint64_t) *regLow));
	default: throw std::runtime_error("Internal error");
	}
	return 0;
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator std::string () const {
	std::stringstream sstr;
	sstr << "0x" << std::hex;
	sstr.fill('0');
	sstr.width(2 * sizeof(SUB_REGISTER_TYPE));
	sstr << (uint64_t) *regHigh;
	sstr.width(2 * sizeof(SUB_REGISTER_TYPE));
	sstr << (uint64_t) *regLow;
	return sstr.str();
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
unisim::kernel::service::VariableBase& ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator = (bool value) {
	if(IsMutable())
	{
		*regHigh = 0;
		*regLow = value ? 1 : 0;
		NotifyListeners();
	}
	return *this;
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
unisim::kernel::service::VariableBase& ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator = (long long value) {
	if(IsMutable())
	{
		*regHigh = value >> (8 * sizeof(SUB_REGISTER_TYPE));
		*regLow = value;
		NotifyListeners();
	}
	return *this;
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
unisim::kernel::service::VariableBase& ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator = (unsigned long long value) {
	if(IsMutable())
	{
		*regHigh = value >> (8 * sizeof(SUB_REGISTER_TYPE));
		*regLow = value;
		NotifyListeners();
	}
	return *this;
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
unisim::kernel::service::VariableBase& ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator = (double value) {
	if(IsMutable())
	{
		*regHigh = (int64_t) value >> (8 * sizeof(SUB_REGISTER_TYPE));
		*regLow = (int64_t) value;
		NotifyListeners();
	}
	return *this;
}

template <class REGISTER_TYPE, class SUB_REGISTER_TYPE>
unisim::kernel::service::VariableBase& ConcatenatedRegisterView<REGISTER_TYPE, SUB_REGISTER_TYPE>::operator = (const char * value) {
	if(IsMutable())
	{
		uint64_t v = (uint64_t) (strcmp(value, "true") == 0) ? 1 : ((strcmp(value, "false") == 0) ? 0 : strtoull(value, 0, 0));
		*regHigh = v >> (8 * sizeof(SUB_REGISTER_TYPE));
		*regLow = v;
		NotifyListeners();
	}
	return *this;
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
