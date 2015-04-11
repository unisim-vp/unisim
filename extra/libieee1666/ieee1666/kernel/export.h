/*
 *  Copyright (c) 2014,
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

#ifndef __IEEE1666_KERNEL_EXPORT_H__
#define __IEEE1666_KERNEL_EXPORT_H__

#include <ieee1666/kernel/fwd.h>
#include <ieee1666/kernel/object.h>
#include <ieee1666/kernel/interface.h>

namespace sc_core {

class sc_export_base : public sc_object
{
protected:
	friend class sc_kernel;
	
	virtual void before_end_of_elaboration();
	virtual void end_of_elaboration();
	virtual void start_of_simulation();
	virtual void end_of_simulation();
};

template<class IF>
class sc_export : public sc_export_base
{
public:
	sc_export();
	explicit sc_export( const char* );
	virtual ~sc_export();
	virtual const char* kind() const;
	void operator() ( IF& );
	virtual void bind( IF& );
	operator IF& ();
	operator const IF& () const;
	IF* operator-> ();
	const IF* operator-> () const;
	virtual sc_interface* get_interface();
	virtual const sc_interface* get_interface() const;
protected:
};

//////////////////////////////////// sc_export<> /////////////////////////////////////////////

template<class IF>
sc_export<IF>::sc_export()
{
}

template<class IF>
sc_export<IF>::sc_export( const char* )
{
}

template<class IF>
sc_export<IF>::~sc_export()
{
}

template<class IF>
const char* sc_export<IF>::kind() const
{
	return "sc_export";
}

template<class IF>
void sc_export<IF>::operator() ( IF& )
{
}

template<class IF>
void sc_export<IF>::bind( IF& )
{
}

template<class IF>
sc_export<IF>::operator IF& ()
{
}

template<class IF>
sc_export<IF>::operator const IF& () const
{
}

template<class IF>
IF* sc_export<IF>::operator-> ()
{
}

template<class IF>
const IF* sc_export<IF>::operator-> () const
{
}

template<class IF>
sc_interface* sc_export<IF>::get_interface()
{
}

template<class IF>
const sc_interface* sc_export<IF>::get_interface() const
{
}

} // end of namespace sc_core

#endif
