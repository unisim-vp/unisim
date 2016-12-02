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

#ifndef __IEEE1666_CHANNELS_FIFO_H__
#define __IEEE1666_CHANNELS_FIFO_H__

#include "core/prim_channel.h"
#include "core/port.h"
#include "core/event.h"
#include "channels/fwd.h"
#include "channels/fifo_if.h"
#include <iostream>

namespace sc_core {

template <class T>
class sc_fifo : public sc_fifo_in_if<T>, public sc_fifo_out_if<T>, public sc_prim_channel
{
public:
	explicit sc_fifo( int size_ = 16 );
	explicit sc_fifo( const char* name_, int size_ = 16);
	virtual ~sc_fifo();
	virtual void register_port( sc_port_base&, const char* );
	virtual void read( T& );
	virtual T read();
	virtual bool nb_read( T& );
	operator T ();
	virtual void write( const T& );
	virtual bool nb_write( const T& );
	sc_fifo<T>& operator= ( const T& );
	virtual const sc_event& data_written_event() const;
	virtual const sc_event& data_read_event() const;
	virtual int num_available() const;
	virtual int num_free() const;
	virtual void print( std::ostream& = std::cout ) const;
	virtual void dump( std::ostream& = std::cout ) const;
	virtual const char* kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_fifo( const sc_fifo<T>& );
	sc_fifo<T>& operator= ( const sc_fifo<T>& );
};

template <class T>
inline std::ostream& operator<< ( std::ostream&, const sc_fifo<T>& );

//////////////////////////////////// sc_fifo<> /////////////////////////////////////////////


template <class T>
sc_fifo<T>::sc_fifo( int size_ )
{
}

template <class T>
sc_fifo<T>::sc_fifo( const char* name_, int size_)
{
}

template <class T>
sc_fifo<T>::~sc_fifo()
{
}

template <class T>
void sc_fifo<T>::register_port( sc_port_base&, const char* )
{
}

template <class T>
void sc_fifo<T>::read( T& )
{
}

template <class T>
T sc_fifo<T>::read()
{
}

template <class T>
bool sc_fifo<T>::nb_read( T& )
{
}

template <class T>
sc_fifo<T>::operator T ()
{
}

template <class T>
void sc_fifo<T>::write( const T& )
{
}

template <class T>
bool sc_fifo<T>::nb_write( const T& )
{
}

template <class T>
sc_fifo<T>& sc_fifo<T>::operator= ( const T& )
{
}

template <class T>
const sc_event& sc_fifo<T>::data_written_event() const
{
}

template <class T>
const sc_event& sc_fifo<T>::data_read_event() const
{
}

template <class T>
int sc_fifo<T>::num_available() const
{
}

template <class T>
int sc_fifo<T>::num_free() const
{
}

template <class T>
void sc_fifo<T>::print( std::ostream& ) const
{
}

template <class T>
void sc_fifo<T>::dump( std::ostream& ) const
{
}

template <class T>
const char* sc_fifo<T>::kind() const
{
}

template <class T>
void sc_fifo<T>::update()
{
}

template <class T>
sc_fifo<T>::sc_fifo( const sc_fifo<T>& )
{
}

template <class T>
sc_fifo<T>& sc_fifo<T>::operator= ( const sc_fifo<T>& )
{
}

template <class T>
inline std::ostream& operator<< ( std::ostream&, const sc_fifo<T>& )
{
}

} // end of namespace sc_core

#endif
