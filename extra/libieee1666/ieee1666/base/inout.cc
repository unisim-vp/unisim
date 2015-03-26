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

#include <ieee1666/base/inout.h>

namespace sc_core {

//////////////////////////////// sc_inout<bool> /////////////////////////////////////////////

sc_inout<bool>::sc_inout()
{
}

sc_inout<bool>::sc_inout( const char* )
{
}

sc_inout<bool>::~sc_inout()
{
}

void sc_inout<bool>::initialize( const bool& )
{
}

void sc_inout<bool>::initialize( const sc_signal_in_if<bool>& )
{
}

void sc_inout<bool>::end_of_elaboration()
{
}

const bool& sc_inout<bool>::read() const
{
}

sc_inout<bool>::operator const bool& () const
{
}

void sc_inout<bool>::write( const bool& )
{
}

sc_inout<bool>& sc_inout<bool>::operator= ( const bool& )
{
}

sc_inout<bool>& sc_inout<bool>::operator= ( const sc_signal_in_if<bool>& )
{
}

sc_inout<bool>& sc_inout<bool>::operator= ( const sc_port< sc_signal_in_if<bool>, 1>& )
{
}

sc_inout<bool>& sc_inout<bool>::operator= ( const sc_port< sc_signal_inout_if<bool>, 1>& )
{
}

sc_inout<bool>& sc_inout<bool>::operator= ( const sc_inout<bool>& )
{
}

const sc_event& sc_inout<bool>::default_event() const
{
}

const sc_event& sc_inout<bool>::value_changed_event() const
{
}

const sc_event& sc_inout<bool>::posedge_event() const
{
}

const sc_event& sc_inout<bool>::negedge_event() const
{
}

bool sc_inout<bool>::event() const
{
}

bool sc_inout<bool>::posedge() const
{
}

bool sc_inout<bool>::negedge() const
{
}

sc_event_finder& sc_inout<bool>::value_changed() const
{
}

sc_event_finder& sc_inout<bool>::pos() const
{
}

sc_event_finder& sc_inout<bool>::neg() const
{
}

const char* sc_inout<bool>::kind() const
{
}

sc_inout<bool>::sc_inout( const sc_inout<bool>& )
{
}

template <>
inline void sc_trace<bool>( sc_trace_file*, const sc_inout<bool>&, const std::string& )
{
}

} // end of namespace sc_core
