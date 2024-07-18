/*
 *  Copyright (c) 2020,
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

#ifndef __UNISIM_UTIL_DEBUG_STUB_HH__
#define __UNISIM_UTIL_DEBUG_STUB_HH__

#include <unisim/util/debug/subprogram.hh>
#include <unisim/util/debug/data_object.hh>
#include <map>

namespace unisim {
namespace util {
namespace debug {

////////////////////////////// declarations ///////////////////////////////////

template <typename ADDRESS>
class Stub
{
public:
	typedef unisim::util::debug::DataObjectRef<ADDRESS> Parameter;
	typedef unisim::util::debug::DataObjectRef<ADDRESS> ReturnValue;
	
	class Parameters
	{
	public:
		Parameter& operator [] (unsigned int index) const { return GetParameter(index); }
		
		virtual unsigned int Length() const = 0;
		virtual Parameter& GetParameter(unsigned int index) const = 0;
		virtual ReturnValue& GetReturnValue() const = 0;
	private:
	};
	
	Stub(const SubProgram<ADDRESS> *_subprogram) : subprogram(_subprogram), ref_count(0) { subprogram->Catch(); }
	virtual ~Stub() { subprogram->Release(); }
	
	const SubProgram<ADDRESS> *GetSubProgram() const { return subprogram; }
	
	virtual void Run(Parameters& parameters) = 0;

	void Catch() const { ref_count++; }
	void Release() const { if(ref_count && (--ref_count == 0)) delete this; }
private:
	const SubProgram<ADDRESS> *subprogram;
	mutable unsigned int ref_count;
};

template <typename ADDRESS>
std::ostream& operator << (std::ostream& stream, const Stub<ADDRESS>& stub)
{
	return stream << "Stub at " << stub.GetSubProgram()->GetName() << " in File \"" << stub.GetSubProgram()->GetFilename() << "\"";
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_STUB_HH__
