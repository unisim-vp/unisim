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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_DATA_OBJECT_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_DATA_OBJECT_HH__

#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/stack_frame.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>
#include <unisim/service/interfaces/stubbing.hh>
#include <unisim/service/interfaces/hooking.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
#include <unisim/service/interfaces/debug_timing.hh>
#include <unisim/util/nodejs/nodejs.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/debug/fetch_stmt_event.hh>
#include <unisim/util/debug/source_code_breakpoint.hh>
#include <unisim/util/debug/subprogram_breakpoint.hh>
#include <unisim/util/ieee754/ieee754.hh>
#include <unisim/util/json/json.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/debug/nodejs/fwd.hh>

#include <string>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// DataObjectWrapper<> //////////////////////////////

template <typename CONFIG>
struct DataObjectWrapper : ObjectWrapper<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	
	struct SetOptions
	{
		bool flatten = false;
	};
	
	struct GetOptions
	{
		bool flatten = false;
	};
	
	typedef ObjectWrapper<CONFIG> Super;
	typedef DataObjectWrapper<CONFIG> This;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	DataObjectWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG> *processor_wrapper, unisim::util::debug::DataObjectRef<ADDRESS> data_object, std::size_t size = 0);
	DataObjectWrapper(NodeJS<CONFIG>& nodejs, ProcessorWrapper<CONFIG> *processor_wrapper = 0, const std::string& expr = std::string(), std::size_t size = 0);
	void Set(v8::Local<v8::Value> value, const SetOptions& options = SetOptions());
	v8::Local<v8::Value> Get(const GetOptions& options = GetOptions());
	void SetCb(const v8::FunctionCallbackInfo<v8::Value>& args);
	void GetCb(const v8::FunctionCallbackInfo<v8::Value>& args);
	void GetExpression(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	template <bool (unisim::util::debug::DataObjectRef<ADDRESS>::*METHOD)() const> void GetIsSomething(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetProcessor(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetBitSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetCVUnqualifiedType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetEndian(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetExists(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetIsOptimizedOut(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	ProcessorWrapper<CONFIG> *GetProcessorWrapper() const { return processor_wrapper; }
	unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE> *GetProcessor() const { return processor_wrapper ? processor_wrapper->GetProcessor() : 0; }
	const std::string& GetExpression() const { return expression; }
	const unisim::util::debug::DataObjectRef<ADDRESS>& GetDataObject() const { return data_object; }
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Super::template MakeObject<This>(); }
	static void Help(std::ostream& stream);
private:
	ProcessorWrapper<CONFIG> *processor_wrapper;
	std::string expression;
	unisim::util::debug::DataObjectRef<ADDRESS> data_object;
	void Update();
};

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_DATA_OBJECT_HH__
