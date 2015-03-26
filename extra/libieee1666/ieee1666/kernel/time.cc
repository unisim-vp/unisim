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

#include <ieee1666/kernel/time.h>

namespace sc_core {

//////////////////////////////////// sc_time /////////////////////////////////////

sc_time::sc_time()
{
}

sc_time::sc_time( double , sc_time_unit )
{
}

sc_time::sc_time( const sc_time& )
{
}

sc_time& sc_time::operator= ( const sc_time& )
{
}

sc_dt::uint64 sc_time::value() const
{
}

double sc_time::to_double() const
{
}

double sc_time::to_seconds() const
{
}

const std::string sc_time::to_string() const
{
}

bool sc_time::operator== ( const sc_time& ) const
{
}

bool sc_time::operator!= ( const sc_time& ) const
{
}

bool sc_time::operator< ( const sc_time& ) const
{
}

bool sc_time::operator<= ( const sc_time& ) const
{
}

bool sc_time::operator> ( const sc_time& ) const
{
}

bool sc_time::operator>= ( const sc_time& ) const
{
}

sc_time& sc_time::operator+= ( const sc_time& )
{
}

sc_time& sc_time::operator-= ( const sc_time& )
{
}

sc_time& sc_time::operator*= ( double )
{
}

sc_time& sc_time::operator/= ( double )
{
}

void sc_time::print( std::ostream& ) const
{
}

//////////////////////////////// global functions /////////////////////////////////

const sc_time operator+ ( const sc_time&, const sc_time& )
{
}

const sc_time operator- ( const sc_time&, const sc_time& )
{
}

const sc_time operator* ( const sc_time&, double )
{
}

const sc_time operator* ( double, const sc_time& )
{
}

const sc_time operator/ ( const sc_time&, double )
{
}

double operator/ ( const sc_time&, const sc_time& )
{
}

std::ostream& operator<< ( std::ostream&, const sc_time& )
{
}

void sc_set_time_resolution( double, sc_time_unit )
{
}

sc_time sc_get_time_resolution()
{
}

const sc_time& sc_max_time()
{
}

} // end of namespace sc_core
