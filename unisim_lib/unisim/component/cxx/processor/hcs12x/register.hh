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
 

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_REGISTER_HH__

#include <unisim/kernel/variable/variable.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

//=============================================================================
//=                  CallBackObject and  TCallBack<TYPE>                      =
//=============================================================================

struct CallBackObject
{
	virtual ~CallBackObject() {}

	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length) { return false; }

	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length) {	return false; }

};

template <typename TYPE>
class TCallBack
{
public:
	typedef bool (CallBackObject::*cbwrite)(unsigned int offset, const void*, unsigned int size);
	typedef bool (CallBackObject::*cbread)(unsigned int offset, const void*, unsigned int size);
	
	TCallBack(CallBackObject *owner, unsigned int offset, cbwrite _write, cbread _read)
          : m_owner(owner), m_offset(offset), write(_write), read(_read)
	{}

	bool Write(TYPE const& storage) { return write and (m_owner->*write)(m_offset, &storage, sizeof (TYPE)); }

	bool Read(TYPE& storage) { return read and (m_owner->*read)(m_offset, &storage, sizeof (TYPE)); }
private:
	CallBackObject *m_owner;
	unsigned int m_offset;

	cbwrite write;
	cbread read;
};

template <class TYPE>
class RegisterView : public unisim::kernel::variable::Register<TYPE>
{
public:
	typedef unisim::kernel::variable::Register<TYPE> Super;
	
	RegisterView(const char *name, unisim::kernel::Object *owner, TYPE& storage, const char *description = NULL)
	  : Super(name, owner, storage, description)
	  , m_callback( 0 )
	{}
	
	~RegisterView()
	{
	  delete m_callback;
	}
	
	typedef TCallBack<TYPE> TCB;
	void setCallBack(CallBackObject *owner, unsigned int offset, typename TCB::cbwrite _write, typename TCB::cbread _read)
	{
		if (m_callback) delete m_callback;
		m_callback = new TCB(owner, offset, _write, _read);
	}

	virtual void Set( TYPE const& value ) { if (not WriteBack(value)) Super::Set( value ); }
	virtual const TYPE& Get() const { TYPE& curr_value = this->storage->Get(); ReadBack(curr_value); return curr_value; }

	using Super::operator=;

protected:
	bool WriteBack(TYPE const& storage)
	{
		bool status = (m_callback and m_callback->Write(storage));
		if (status) this->NotifyListeners();
		return status;
	}

	bool ReadBack(TYPE& storage) const
	{
		return (m_callback and m_callback->Read(storage));
	}

private:
	TCB* m_callback;
};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
