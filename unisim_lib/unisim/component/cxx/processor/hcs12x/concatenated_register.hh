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
	*(REGISTER_TYPE *) buffer = (REGISTER_TYPE ) (*regHigh << sizeof(SUB_REGISTER_TYPE)) | *regLow ;
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

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
