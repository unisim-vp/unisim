/*
 *  Copyright (c) 2023,
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
 
#ifndef __UNISIM_UTIL_OSTREAM_OSTREAM_HH__
#define __UNISIM_UTIL_OSTREAM_OSTREAM_HH__

#include <iostream>
#include <sstream>

namespace unisim {
namespace util {
namespace ostream {

// Convert something into a string using the ostream << operator
template <typename T>
std::string ToString(const T& v)
{
	std::ostringstream sstr;
	sstr << v;
	return sstr.str();
}

// Scope (automatic save and restore) printing format modifications of a ostream
template <typename OSTREAM = std::ostream>
struct Scope
{
	Scope(OSTREAM& _stream)
		: stream(_stream)
		, width(stream.width())
		, fill(stream.fill())
		, precision(stream.precision())
		, flags(stream.flags())
	{
	}
	
	~Scope()
	{
		stream.width(width);
		stream.fill(fill);
		stream.precision(precision);
		stream.flags(flags);
	}
	
private:
	OSTREAM& stream;
	std::streamsize width;
	typename OSTREAM::char_type fill;
	std::streamsize precision;
	std::ios_base::fmtflags flags;
};

typedef Scope<std::ostream> OutputStreamScope;
typedef Scope<std::wostream> WideOutputStreamScope;

// Scope (automatic save and restore) printing format modifications of a ostream
template <typename OSTREAM = std::ostream>
struct FScope
{
	FScope(OSTREAM& _stream)
		: stream(_stream)
		, flags(stream.flags())
	{
	}
	
	~FScope()
	{
		stream.flags(flags);
	}
	
private:
	OSTREAM& stream;
	std::ios_base::fmtflags flags;
};

typedef Scope<std::ostream> OutputStreamFScope;
typedef Scope<std::wostream> WideOutputStreamFScope;

} // end of namespace ostream
} // end of namespace util
} // end of namespace unisim

#endif
