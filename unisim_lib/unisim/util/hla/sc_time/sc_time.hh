/*
 *  Copyright (c) 2022,
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

#ifndef __UNISIM_UTIL_HLA_SC_TIME_SC_TIME_HH__
#define __UNISIM_UTIL_HLA_SC_TIME_SC_TIME_HH__

#include <systemc>
#include <sstream>
#include <unisim/util/unicode/unicode.hh>

namespace unisim {
namespace util {
namespace hla {

template <>
struct TimeAdapters<sc_core::sc_time, uint64_t>
{
	static uint64_t ToScalar(const sc_core::sc_time& time) { return time.value(); }
	static bool IsNull(const sc_core::sc_time& time) { return time == sc_core::SC_ZERO_TIME; }
	static sc_core::sc_time Epsilon() { return sc_core::sc_get_time_resolution(); }
	static std::string ToString(const sc_core::sc_time& time) { return time.to_string(); }
	static sc_core::sc_time FromString(const std::string& str)
	{
		std::istringstream sstr(str);
		double time_value;
		std::string time_unit;
		
		if((sstr >> time_value) && (sstr >> time_unit))
		{
			if(time_unit.compare("s") == 0) return sc_core::sc_time(time_value, sc_core::SC_SEC);
			else if(time_unit.compare("ms") == 0) return sc_core::sc_time(time_value, sc_core::SC_MS);
			else if(time_unit.compare("us") == 0) return sc_core::sc_time(time_value, sc_core::SC_US);
			else if(time_unit.compare("ns") == 0) return sc_core::sc_time(time_value, sc_core::SC_NS);
			else if(time_unit.compare("ps") == 0) return sc_core::sc_time(time_value, sc_core::SC_PS);
			else if(time_unit.compare("fs") == 0) return sc_core::sc_time(time_value, sc_core::SC_FS);
		}
		
		return sc_core::SC_ZERO_TIME;
	}
};

template <>
struct TimeAdapters<sc_core::sc_time, double>
{
	static uint64_t ToScalar(const sc_core::sc_time& time) { return time.to_seconds(); }
	static bool IsNull(const sc_core::sc_time& time) { return time == sc_core::SC_ZERO_TIME; }
	static sc_core::sc_time Epsilon() { return sc_core::sc_get_time_resolution(); }
	static std::string ToString(const sc_core::sc_time& time) { return time.to_string(); }
	static sc_core::sc_time FromString(const std::string& str)
	{
		std::istringstream sstr(str);
		double time_value;
		std::string time_unit;
		
		if((sstr >> time_value) && (sstr >> time_unit))
		{
			if(time_unit.compare("s") == 0) return sc_core::sc_time(time_value, sc_core::SC_SEC);
			else if(time_unit.compare("ms") == 0) return sc_core::sc_time(time_value, sc_core::SC_MS);
			else if(time_unit.compare("us") == 0) return sc_core::sc_time(time_value, sc_core::SC_US);
			else if(time_unit.compare("ns") == 0) return sc_core::sc_time(time_value, sc_core::SC_NS);
			else if(time_unit.compare("ps") == 0) return sc_core::sc_time(time_value, sc_core::SC_PS);
			else if(time_unit.compare("fs") == 0) return sc_core::sc_time(time_value, sc_core::SC_FS);
		}
		
		return sc_core::SC_ZERO_TIME;
	}
};

} // end of namespace hla
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_HLA_SC_TIME_SC_TIME_HH__

