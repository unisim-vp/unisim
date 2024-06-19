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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_HOOK_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_HOOK_HH__

#include <unisim/service/debug/nodejs/fwd.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// HookWrapper<> /////////////////////////////////

template <typename CONFIG>
struct HookWrapper : ObjectWrapper<CONFIG>
{
	typedef ObjectWrapper<CONFIG> Super;
	typedef HookWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;

	HookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG>& processor_wrapper, unisim::util::debug::Hook<ADDRESS> *hook, std::size_t size = 0);
	virtual ~HookWrapper();
	unisim::util::debug::Hook<ADDRESS> *GetHook() const;
	void Trigger();
	void On(const v8::FunctionCallbackInfo<v8::Value>& args);
	void RemoveListener(const v8::FunctionCallbackInfo<v8::Value>& args);
	virtual void Finalize();
	static void Cleanup();
protected:
	template <typename T> v8::Local<v8::Object> MakeObject(v8::Local<v8::ObjectTemplate> object_template);
	static void FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template);
private:
	ProcessorWrapper<CONFIG>& processor_wrapper;
	unisim::util::debug::Hook<ADDRESS> *hook;
	typedef std::list<v8::Global<v8::Function> > Functions;
	Functions functions;
	typedef std::vector<v8::Global<v8::Promise::Resolver> > Resolvers;
	Resolvers resolvers;
	bool hook_set;
	v8::Global<v8::Object> shadow_object;
	static v8::Global<v8::ObjectTemplate> cached_object_template;
	static v8::Local<v8::ObjectTemplate> MakeObjectTemplate(v8::Isolate *isolate);
	v8::Local<v8::Value> SetRemoveError();
	template <v8::Maybe<bool> (v8::Promise::Resolver::*SETTLE_METHOD)(v8::Local<v8::Context>, v8::Local<v8::Value>)>
	void Settle(v8::Local<v8::Value> value);
	void Resolve(v8::Local<v8::Value> value);
	void Reject(v8::Local<v8::Value> value);
	bool Update(bool has_listeners);
};

///////////////////////////// AddressHookWrapper<> /////////////////////////////

template <typename CONFIG>
struct AddressHookWrapper : HookWrapper<CONFIG>
{
	typedef HookWrapper<CONFIG> Super;
	typedef AddressHookWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	AddressHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG>& processor_wrapper, AddressHook<CONFIG> *hook, std::size_t size = 0);
	v8::Local<v8::Object> MakeObject();
	static void Cleanup();
protected:
	template <typename T> v8::Local<v8::Object> MakeObject(v8::Local<v8::ObjectTemplate> object_template);
	static void FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template);
private:
	AddressHook<CONFIG> *address_hook;
	static v8::Global<v8::ObjectTemplate> cached_object_template;
	static v8::Local<v8::ObjectTemplate> MakeObjectTemplate(v8::Isolate *isolate);
};

/////////////////////////// SourceCodeHookWrapper<> ////////////////////////////

template <typename CONFIG>
struct SourceCodeHookWrapper : HookWrapper<CONFIG>
{
	typedef HookWrapper<CONFIG> Super;
	typedef SourceCodeHookWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	SourceCodeHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG>& processor_wrapper, SourceCodeHook<CONFIG> *hook, std::size_t size = 0);
	v8::Local<v8::Object> MakeObject();
	static void Cleanup();
protected:
	template <typename T> v8::Local<v8::Object> MakeObject(v8::Local<v8::ObjectTemplate> object_template);
	static void FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template);
private:
	SourceCodeHook<CONFIG> *source_code_hook;
	static v8::Global<v8::ObjectTemplate> cached_object_template;
	static v8::Local<v8::ObjectTemplate> MakeObjectTemplate(v8::Isolate *isolate);
};

/////////////////////////// SubProgramHookWrapper<> ////////////////////////////

template <typename CONFIG>
struct SubProgramHookWrapper : HookWrapper<CONFIG>
{
	typedef HookWrapper<CONFIG> Super;
	typedef SubProgramHookWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	SubProgramHookWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG>& processor_wrapper, SubProgramHook<CONFIG> *hook, std::size_t size = 0);
	v8::Local<v8::Object> MakeObject();
	static void Cleanup();
protected:
	template <typename T> v8::Local<v8::Object> MakeObject(v8::Local<v8::ObjectTemplate> object_template);
	static void FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template);
private:
	SubProgramHook<CONFIG> *subprogram_hook;
	static v8::Global<v8::ObjectTemplate> cached_object_template;
	static v8::Local<v8::ObjectTemplate> MakeObjectTemplate(v8::Isolate *isolate);
};

//////////////////////////////// AddressHook<> /////////////////////////////////

template <typename CONFIG>
struct AddressHook : unisim::util::debug::AddressHook<typename CONFIG::ADDRESS>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	AddressHook(ADDRESS addr);
	void Bind(AddressHookWrapper<CONFIG> *hook_wrapper);
	virtual void Run();
private:
	AddressHookWrapper<CONFIG> *hook_wrapper;
};

/////////////////////////////// SourceCodeHook<> ///////////////////////////////

template <typename CONFIG>
struct SourceCodeHook : unisim::util::debug::SourceCodeHook<typename CONFIG::ADDRESS>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	SourceCodeHook(const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename);
	void Bind(SourceCodeHookWrapper<CONFIG> *hook_wrapper);
	virtual void Run();
private:
	SourceCodeHookWrapper<CONFIG> *hook_wrapper;
};

/////////////////////////////// SubProgramHook<> ///////////////////////////////

template <typename CONFIG>
struct SubProgramHook : unisim::util::debug::SubProgramHook<typename CONFIG::ADDRESS>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	SubProgramHook(const unisim::util::debug::SubProgram<ADDRESS> *subprogram);
	void Bind(SubProgramHookWrapper<CONFIG> *hook_wrapper);
	virtual void Run();
private:
	SubProgramHookWrapper<CONFIG> *hook_wrapper;
};

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_HOOK_HH__
