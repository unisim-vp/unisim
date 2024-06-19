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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_BREAKPOINT_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_BREAKPOINT_TCC__

#include <unisim/service/debug/nodejs/breakpoint.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::MakeInteger;

//////////////////////////////// BreakpointWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *BreakpointWrapper<CONFIG>::CLASS_NAME = "Breakpoint";

template <typename CONFIG>
const uint32_t BreakpointWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> BreakpointWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
BreakpointWrapper<CONFIG>::BreakpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG>& _processor_wrapper, unisim::util::debug::Breakpoint<ADDRESS> *_breakpoint, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, breakpoint(_breakpoint)
	, event_bridge(_nodejs, _breakpoint, processor_wrapper.Recv())
	, shadow_object()
{
}

template <typename CONFIG>
BreakpointWrapper<CONFIG>::~BreakpointWrapper()
{
}

template <typename CONFIG>
void BreakpointWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void BreakpointWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void BreakpointWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "XXXX" method
	//object_template->Set(isolate, "xxxx", unisim::util::nodejs::CreateFunctionTemplate<BreakpointWrapper<CONFIG>, &BreakpointWrapper<CONFIG>::XXXX>(isolate));
	// add "enable" method
	object_template->Set(isolate, "enable", unisim::util::nodejs::CreateFunctionTemplate<BreakpointWrapper<CONFIG>, &BreakpointWrapper<CONFIG>::Enable>(isolate));
	// add "disable" method
	object_template->Set(isolate, "disable", unisim::util::nodejs::CreateFunctionTemplate<BreakpointWrapper<CONFIG>, &BreakpointWrapper<CONFIG>::Disable>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> BreakpointWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> object_template;
	
	if(cached_object_template.IsEmpty())
	{
		// create object template
		object_template = unisim::util::nodejs::CreateObjectTemplate(isolate);
		// fill object template
		FillObjectTemplate(isolate, object_template);
		// cache object template
		cached_object_template.Reset(isolate, object_template);
	}
	else
	{
		object_template = cached_object_template.Get(isolate);
	}
	
	return handle_scope.Escape(object_template);
}

template <typename CONFIG>
template <typename T>
v8::Local<v8::Object> BreakpointWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "yyyy" read-only property
	//object->DefineOwnProperty(
	//	this->GetIsolate()->GetCurrentContext(),
	//	v8::String::NewFromUtf8Literal(this->GetIsolate(), "yyyy", v8::NewStringType::kInternalized),
	//	v8::String::NewFromUtf8(this->GetIsolate(), "zzzz").ToLocalChecked(),
	//	v8::ReadOnly
	//).ToChecked();
	// add "address" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "address", v8::NewStringType::kInternalized),
		MakeInteger(this->GetIsolate(), breakpoint->GetAddress()),
		v8::ReadOnly
 	).ToChecked();
	
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> BreakpointWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
unisim::util::debug::Breakpoint<typename CONFIG::ADDRESS> *BreakpointWrapper<CONFIG>::GetBreakpoint() const
{
	return breakpoint;
}

//template <typename CONFIG>
//void BreakpointWrapper<CONFIG>::XXXX(const v8::FunctionCallbackInfo<v8::Value>& args)
//{
//	v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
//}

template <typename CONFIG>
void BreakpointWrapper<CONFIG>::Enable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(true);
}

template <typename CONFIG>
void BreakpointWrapper<CONFIG>::Disable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(false);
}

//////////////////////////////// SubProgramBreakpointWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *SubProgramBreakpointWrapper<CONFIG>::CLASS_NAME = "SubProgramBreakpoint";

template <typename CONFIG>
const uint32_t SubProgramBreakpointWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> SubProgramBreakpointWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
SubProgramBreakpointWrapper<CONFIG>::SubProgramBreakpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG>& _processor_wrapper, unisim::util::debug::SubProgramBreakpoint<ADDRESS> *_subprogram_breakpoint, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, subprogram_breakpoint(_subprogram_breakpoint)
	, event_bridge(_nodejs, _subprogram_breakpoint, processor_wrapper.Recv())
	, shadow_object()
{
}

template <typename CONFIG>
SubProgramBreakpointWrapper<CONFIG>::~SubProgramBreakpointWrapper()
{
}

template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "XXXX" method
	//object_template->Set(isolate, "xxxx", unisim::util::nodejs::CreateFunctionTemplate<SubProgramBreakpointWrapper<CONFIG>, &SubProgramBreakpointWrapper<CONFIG>::XXXX>(isolate));
	// add "enable" method
	object_template->Set(isolate, "enable", unisim::util::nodejs::CreateFunctionTemplate<SubProgramBreakpointWrapper<CONFIG>, &SubProgramBreakpointWrapper<CONFIG>::Enable>(isolate));
	// add "disable" method
	object_template->Set(isolate, "disable", unisim::util::nodejs::CreateFunctionTemplate<SubProgramBreakpointWrapper<CONFIG>, &SubProgramBreakpointWrapper<CONFIG>::Disable>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> SubProgramBreakpointWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> object_template;
	
	if(cached_object_template.IsEmpty())
	{
		// create object template
		object_template = unisim::util::nodejs::CreateObjectTemplate(isolate);
		// fill object template
		FillObjectTemplate(isolate, object_template);
		// cache object template
		cached_object_template.Reset(isolate, object_template);
	}
	else
	{
		object_template = cached_object_template.Get(isolate);
	}
	
	return handle_scope.Escape(object_template);
}

template <typename CONFIG>
template <typename T>
v8::Local<v8::Object> SubProgramBreakpointWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "yyyy" read-only property
	//object->DefineOwnProperty(
	//	this->GetIsolate()->GetCurrentContext(),
	//	v8::String::NewFromUtf8Literal(this->GetIsolate(), "yyyy", v8::NewStringType::kInternalized),
	//	v8::String::NewFromUtf8(this->GetIsolate(), "zzzz").ToLocalChecked(),
	//	v8::ReadOnly
	//).ToChecked();
	// add "file" read-only property
	unisim::util::debug::SubProgram<ADDRESS> const *subprogram = subprogram_breakpoint->GetSubProgram();
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "file", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), subprogram->GetFilename()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	// add "name" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "name", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), subprogram->GetName()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> SubProgramBreakpointWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
unisim::util::debug::SubProgramBreakpoint<typename CONFIG::ADDRESS> *SubProgramBreakpointWrapper<CONFIG>::GetSubProgramBreakpoint() const
{
	return subprogram_breakpoint;
}

//template <typename CONFIG>
//void SubProgramBreakpointWrapper<CONFIG>::XXXX(const v8::FunctionCallbackInfo<v8::Value>& args)
//{
//	v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
//}

template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::Enable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(true);
}

template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::Disable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(false);
}

//////////////////////////////// SourceCodeBreakpointWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME = "SourceCodeBreakpoint";

template <typename CONFIG>
const uint32_t SourceCodeBreakpointWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> SourceCodeBreakpointWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
SourceCodeBreakpointWrapper<CONFIG>::SourceCodeBreakpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG>& _processor_wrapper, unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *_source_code_breakpoint, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, source_code_breakpoint(_source_code_breakpoint)
	, event_bridge(_nodejs, _source_code_breakpoint, processor_wrapper.Recv())
	, shadow_object()
{
}

template <typename CONFIG>
SourceCodeBreakpointWrapper<CONFIG>::~SourceCodeBreakpointWrapper()
{
}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "XXXX" method
	//object_template->Set(isolate, "xxxx", unisim::util::nodejs::CreateFunctionTemplate<SourceCodeBreakpointWrapper<CONFIG>, &SourceCodeBreakpointWrapper<CONFIG>::XXXX>(isolate));
	// add "enable" method
	object_template->Set(isolate, "enable", unisim::util::nodejs::CreateFunctionTemplate<SourceCodeBreakpointWrapper<CONFIG>, &SourceCodeBreakpointWrapper<CONFIG>::Enable>(isolate));
	// add "disable" method
	object_template->Set(isolate, "disable", unisim::util::nodejs::CreateFunctionTemplate<SourceCodeBreakpointWrapper<CONFIG>, &SourceCodeBreakpointWrapper<CONFIG>::Disable>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> SourceCodeBreakpointWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> object_template;
	
	if(cached_object_template.IsEmpty())
	{
		// create object template
		object_template = unisim::util::nodejs::CreateObjectTemplate(isolate);
		// fill object template
		FillObjectTemplate(isolate, object_template);
		// cache object template
		cached_object_template.Reset(isolate, object_template);
	}
	else
	{
		object_template = cached_object_template.Get(isolate);
	}
	
	return handle_scope.Escape(object_template);
}

template <typename CONFIG>
template <typename T>
v8::Local<v8::Object> SourceCodeBreakpointWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = Super::template MakeObject<T>(object_template);
	// add "yyyy" read-only property
	//object->DefineOwnProperty(
	//	this->GetIsolate()->GetCurrentContext(),
	//	v8::String::NewFromUtf8Literal(this->GetIsolate(), "yyyy", v8::NewStringType::kInternalized),
	//	v8::String::NewFromUtf8(this->GetIsolate(), "zzzz").ToLocalChecked(),
	//	v8::ReadOnly
	//).ToChecked();
	// add "file" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "file", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), source_code_breakpoint->GetFilename().c_str()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
#if 0
	unisim::util::debug::SourceCodeLocation const& source_code_location = source_code_breakpoint->GetSourceCodeLocation();
	v8::Local<v8::Object> prop_loc = v8::Object::New(this->GetIsolate());
	// add "loc.source_code_filename" read-only property
	prop_loc->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "source_code_filename", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), source_code_location.GetSourceCodeFilename().c_str()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	unsigned lineno = source_code_location.GetLineNo();
	// add "loc.lineno" read-only property
	prop_loc->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "lineno", v8::NewStringType::kInternalized),
		v8::Integer::NewFromUnsigned(this->GetIsolate(), lineno),
		v8::ReadOnly
 	).ToChecked();
	// add "loc.colno" read-only property
	unsigned colno = source_code_location.GetColNo();
	if(colno)
	{
		prop_loc->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "colno", v8::NewStringType::kInternalized),
			v8::Integer::NewFromUnsigned(this->GetIsolate(), colno),
			v8::ReadOnly
		).ToChecked();
	}
	// add "loc" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "loc", v8::NewStringType::kInternalized),
		prop_loc,
		v8::ReadOnly
 	).ToChecked();
#else
	// add "loc" read-only property
	SourceCodeLocationWrapper<CONFIG> *source_code_location_wrapper = new SourceCodeLocationWrapper<CONFIG>(this->nodejs, source_code_breakpoint->GetSourceCodeLocation());
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "loc", v8::NewStringType::kInternalized),
		source_code_location_wrapper->MakeObject(),
		v8::ReadOnly
 	).ToChecked();
#endif
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> SourceCodeBreakpointWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
unisim::util::debug::SourceCodeBreakpoint<typename CONFIG::ADDRESS> *SourceCodeBreakpointWrapper<CONFIG>::GetSourceCodeBreakpoint() const
{
	return source_code_breakpoint;
}

//template <typename CONFIG>
//void SourceCodeBreakpointWrapper<CONFIG>::XXXX(const v8::FunctionCallbackInfo<v8::Value>& args)
//{
//	v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
//}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::Enable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(true);
}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::Disable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	event_bridge.Trap(false);
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_BREAKPOINT_TCC__
