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

#ifndef __UNISIM_UTIL_DEBUG_HOOK_HH__
#define __UNISIM_UTIL_DEBUG_HOOK_HH__

#include <unisim/util/debug/source_code_location.hh>
#include <unisim/util/debug/data_object.hh>

namespace unisim {
namespace util {
namespace debug {

////////////////////////////// declarations ///////////////////////////////////

template <typename ADDRESS>
class Hook
{
public:
	typedef unsigned int Type;
	
	Hook(Type _type) : type(_type), id(next_id++), ref_count(0) {}
	virtual ~Hook() {}

	Type GetType() const { return type; }
	unsigned int GetId() const { return id; }
	
	virtual void Run() = 0;

	virtual void Print(std::ostream& stream) const = 0;
	
	void Catch() const { ref_count++; }
	void Release() const { if(ref_count && (--ref_count == 0)) delete this; }
private:
	Type type;
	unsigned int id;
	static unsigned int next_id;
	mutable unsigned int ref_count;
protected:
	static Type AllocateCustomHookType() { static Type next_hook_type = 0; return next_hook_type++; }
};

template <typename ADDRESS>
std::ostream& operator << (std::ostream& stream, const Hook<ADDRESS>& hook)
{
	hook.Print(stream);
	return stream;
}

template <typename ADDRESS>
unsigned int Hook<ADDRESS>::next_id;

template <typename ADDRESS, typename T>
class CustomHook : public Hook<ADDRESS>
{
public:
	static const typename Hook<ADDRESS>::Type TYPE;
	
	static bool IsInstanceOf(const Hook<ADDRESS> *hook) { return hook->GetType() == TYPE; }
	
	CustomHook() : Hook<ADDRESS>(TYPE) {}
};

template <typename ADDRESS, typename T>
const typename Hook<ADDRESS>::Type CustomHook<ADDRESS, T>::TYPE = Hook<ADDRESS>::AllocateCustomHookType();

template <typename ADDRESS>
class AddressHook : public CustomHook<ADDRESS, AddressHook<ADDRESS> >
{
public:
	AddressHook(ADDRESS _addr) : addr(_addr) {}
	
	ADDRESS GetAddress() const { return addr; }

	virtual void Print(std::ostream& stream) const { stream << "Address hook at @0x" << std::hex << addr << std::dec; }
private:
	ADDRESS addr;
};

template <typename ADDRESS>
class SourceCodeHook : public CustomHook<ADDRESS, SourceCodeHook<ADDRESS> >
{
public:
	SourceCodeHook(const unisim::util::debug::SourceCodeLocation& _source_code_location) : source_code_location(_source_code_location), filename() {}
	SourceCodeHook(const unisim::util::debug::SourceCodeLocation& _source_code_location, const char *_filename) : source_code_location(_source_code_location), filename(_filename) {}
	SourceCodeHook(const unisim::util::debug::SourceCodeLocation& _source_code_location, const std::string& _filename) : source_code_location(_source_code_location), filename(_filename) {}
	
	const unisim::util::debug::SourceCodeLocation& GetSourceCodeLocation() const { return source_code_location; }
	const std::string& GetFilename() const { return filename; }
	
	virtual void Print(std::ostream& stream) const { stream << "Source code hook at " << source_code_location << " in File \"" << filename << "\""; }
	
private:
	SourceCodeLocation source_code_location;
	std::string filename;
};

template <typename ADDRESS>
class SubProgramHook : public CustomHook<ADDRESS, SubProgramHook<ADDRESS> >
{
public:
	SubProgramHook(const SubProgram<ADDRESS> *_subprogram) : subprogram(_subprogram) {}
	
	const SubProgram<ADDRESS> *GetSubProgram() const { return subprogram; }
	
	virtual void Print(std::ostream& stream) const { stream << "Source code hook at " << subprogram->GetName() << " in File \"" << subprogram->GetFilename() << "\""; }

private:
	const SubProgram<ADDRESS> *subprogram;
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_HOOK_HH__
