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

#ifndef __IEEE1666_KERNEL_PORT_H__
#define __IEEE1666_KERNEL_PORT_H__

#include <ieee1666/kernel/fwd.h>
#include <ieee1666/kernel/object.h>
#include <ieee1666/kernel/interface.h>

namespace sc_core {

class sc_port_base : public sc_object
{
};

template <class IF>
class sc_port_b : public sc_port_base
{
public:
	void operator() ( IF& );
	void operator() ( sc_port_b<IF>& );
	virtual void bind( IF& );
	virtual void bind( sc_port_b<IF>& );
	int size() const;
	IF* operator-> ();
	const IF* operator-> () const;
	IF* operator[] ( int );
	const IF* operator[] ( int ) const;
	virtual sc_interface* get_interface();
	virtual const sc_interface* get_interface() const;
protected:
	virtual void before_end_of_elaboration();
	virtual void end_of_elaboration();
	virtual void start_of_simulation();
	virtual void end_of_simulation();
	explicit sc_port_b( int , sc_port_policy );
	sc_port_b( const char* , int , sc_port_policy );
	virtual ~sc_port_b();
private:
	// Disabled
	sc_port_b();
	sc_port_b( const sc_port_b<IF>& );
	sc_port_b<IF>& operator = ( const sc_port_b<IF>& );
};

template <class IF, int N = 1, sc_port_policy P = SC_ONE_OR_MORE_BOUND>
class sc_port : public sc_port_b<IF>
{
public:
	sc_port();
	explicit sc_port( const char* );
	virtual ~sc_port();
	virtual const char* kind() const;
private:
	// Disabled
	sc_port( const sc_port<IF,N,P>& );
	sc_port<IF,N,P>& operator= ( const sc_port<IF,N,P>& );
};

//////////////////////////////////// sc_port_b<> /////////////////////////////////////////////

template <class IF>
void sc_port_b<IF>::operator() ( IF& )
{
}

template <class IF>
void sc_port_b<IF>::operator() ( sc_port_b<IF>& )
{
}

template <class IF>
void sc_port_b<IF>::bind( IF& )
{
}

template <class IF>
void sc_port_b<IF>::bind( sc_port_b<IF>& )
{
}

template <class IF>
int sc_port_b<IF>::size() const
{
}

template <class IF>
IF* sc_port_b<IF>::operator-> ()
{
}

template <class IF>
const IF* sc_port_b<IF>::operator-> () const
{
}

template <class IF>
IF* sc_port_b<IF>::operator[] ( int )
{
}

template <class IF>
const IF* sc_port_b<IF>::operator[] ( int ) const
{
}

template <class IF>
sc_interface* sc_port_b<IF>::get_interface()
{
}

template <class IF>
const sc_interface* sc_port_b<IF>::get_interface() const
{
}

template <class IF>
void sc_port_b<IF>::before_end_of_elaboration()
{
}

template <class IF>
void sc_port_b<IF>::end_of_elaboration()
{
}

template <class IF>
void sc_port_b<IF>::start_of_simulation()
{
}

template <class IF>
void sc_port_b<IF>::end_of_simulation()
{
}

template <class IF>
sc_port_b<IF>::sc_port_b( int , sc_port_policy )
{
}

template <class IF>
sc_port_b<IF>::sc_port_b( const char* , int , sc_port_policy )
{
}

template <class IF>
sc_port_b<IF>::~sc_port_b()
{
}

template <class IF>
sc_port_b<IF>::sc_port_b()
{
}

template <class IF>
sc_port_b<IF>::sc_port_b( const sc_port_b<IF>& )
{
}

template <class IF>
sc_port_b<IF>& sc_port_b<IF>::operator = ( const sc_port_b<IF>& )
{
}

//////////////////////////////////// sc_port<> /////////////////////////////////////////////

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::sc_port()
	: sc_port_b<IF>(N, P)
{
}

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::sc_port( const char* )
{
}

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::~sc_port()
{
}

template <class IF, int N, sc_port_policy P>
const char* sc_port<IF, N, P>::kind() const
{
}

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::sc_port( const sc_port<IF,N,P>& )
{
}

template <class IF, int N, sc_port_policy P>
sc_port<IF,N,P>& sc_port<IF, N, P>::operator= ( const sc_port<IF,N,P>& )
{
}

} // end of namespace sc_core

#endif
