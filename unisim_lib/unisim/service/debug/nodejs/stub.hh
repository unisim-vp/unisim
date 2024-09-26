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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_STUB_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_STUB_HH__

#include <unisim/service/debug/nodejs/fwd.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// StubWrapper<> /////////////////////////////////

template <typename CONFIG>
struct StubWrapper : ObjectWrapper<CONFIG>
{
	typedef ObjectWrapper<CONFIG> Super;
	typedef StubWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	StubWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG> *processor_wrapper, Stub<CONFIG> *stub, std::size_t size = 0);
	virtual ~StubWrapper();
	unisim::util::debug::Stub<ADDRESS> *GetStub() const;
	void Trigger(typename unisim::util::debug::Stub<ADDRESS>::Parameters& parameters);
	void GetProcessor(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetSubProgram(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void On(const v8::FunctionCallbackInfo<v8::Value>& args);
	void RemoveListener(const v8::FunctionCallbackInfo<v8::Value>& args);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Super::template MakeObject<This>(); }
	static void Help(std::ostream& stream);
private:
	ProcessorWrapper<CONFIG> *processor_wrapper;
	unisim::util::debug::Stub<ADDRESS> *stub;
	v8::Global<v8::Function> function;
	bool stub_set;
	v8::Local<v8::Value> SetRemoveError();
	bool Update(bool has_listeners);
};

//////////////////////////////////// Stub<> ////////////////////////////////////

template <typename CONFIG>
struct Stub : unisim::util::debug::Stub<typename CONFIG::ADDRESS>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	Stub(NodeJS<CONFIG>& nodejs, const unisim::util::debug::SubProgram<ADDRESS> *subprogram);
	void Bind(StubWrapper<CONFIG> *stub_wrapper);
	virtual void Run(typename unisim::util::debug::Stub<ADDRESS>::Parameters& parameters);
private:
	StubWrapper<CONFIG> *stub_wrapper;
};

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_STUB_HH__
