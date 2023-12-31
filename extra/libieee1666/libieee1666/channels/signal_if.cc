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

#include "channels/signal_if.h"

namespace sc_core {

/////////////////////////////// sc_signal_in_if<bool> ////////////////////////////////////////
	
sc_signal_in_if<bool>::sc_signal_in_if()
{
}

// Disabled
sc_signal_in_if<bool>::sc_signal_in_if( const sc_signal_in_if<bool>& )
{
}

// Disabled
sc_signal_in_if<bool>& sc_signal_in_if<bool>::operator= ( const sc_signal_in_if<bool>& )
{
	return *this;
}

/////////////////////////////// sc_signal_in_if<sc_dt::sc_logic> ////////////////////////////////////////
	
sc_signal_in_if<sc_dt::sc_logic>::sc_signal_in_if()
{
}

// Disabled
sc_signal_in_if<sc_dt::sc_logic>::sc_signal_in_if( const sc_signal_in_if<sc_dt::sc_logic>& )
{
}

// Disabled
sc_signal_in_if<sc_dt::sc_logic>& sc_signal_in_if<sc_dt::sc_logic>::operator= ( const sc_signal_in_if<sc_dt::sc_logic>& )
{
	return *this;
}

} // end of namespace sc_core
