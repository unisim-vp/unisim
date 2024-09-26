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
v8::Local<v8::FunctionTemplate> BreakpointWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> breakpoint_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = breakpoint_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "address", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetAddress>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "DebugEvent"
	breakpoint_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<DebugEventWrapper<CONFIG> >());
	
	return handle_scope.Escape(breakpoint_function_template);
}

// Breakpoint() => Breakpoint
// Breakpoint(processor: Processor, addr : Number) => Breakpoint
template <typename CONFIG>
void BreakpointWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = 0;
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, addr : Number)"; } };
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1];
		ADDRESS addr = 0;
		if((!arg1->IsNumber() && !arg1->IsBigInt()) || !ToInt(args.GetIsolate(), arg1, addr))
		{
			nodejs.Throw(nodejs.Error(Synopsis().str() + " expects a number for 'addr'"));
			return;
		}
		
		breakpoint = processor_wrapper->GetProcessor()->CreateBreakpoint(addr);
	}
	
	BreakpointWrapper<CONFIG> *breakpoint_wrapper = new BreakpointWrapper<CONFIG>(nodejs, processor_wrapper, breakpoint);
	breakpoint_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
BreakpointWrapper<CONFIG>::BreakpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::Breakpoint<ADDRESS> *_breakpoint, std::size_t size)
	: Super(_nodejs, _processor_wrapper, _breakpoint, size ? size : sizeof(*this))
	, breakpoint(_breakpoint)
{
}

template <typename CONFIG>
BreakpointWrapper<CONFIG>::~BreakpointWrapper()
{
}

template <typename CONFIG>
unisim::util::debug::Breakpoint<typename CONFIG::ADDRESS> *BreakpointWrapper<CONFIG>::GetBreakpoint() const
{
	return breakpoint;
}

template <typename CONFIG>
void BreakpointWrapper<CONFIG>::GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(breakpoint) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), breakpoint->GetAddress()));
}

template <typename CONFIG>
void BreakpointWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/breakpoint.h>
	;
}

//////////////////////////////// SubProgramBreakpointWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *SubProgramBreakpointWrapper<CONFIG>::CLASS_NAME = "SubProgramBreakpoint";

template <typename CONFIG>
const uint32_t SubProgramBreakpointWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> SubProgramBreakpointWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> subprogram_breakpoint_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = subprogram_breakpoint_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "subprogram", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetSubProgram>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "DebugEvent"
	subprogram_breakpoint_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<DebugEventWrapper<CONFIG> >());
	
	return handle_scope.Escape(subprogram_breakpoint_function_template);
}

// SubProgramBreakpoint() => SubProgramBreakpoint
// SubProgramBreakpoint(processor: Processor, subprogram: SubProgram) => SubProgramBreakpoint
template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_breakpoint = 0;
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, subprogram: SubProgram)"; } };
	
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1];
		SubProgramWrapper<CONFIG> *subprogram_wrapper = 0;
		if(!arg1->IsObject() || !(subprogram_wrapper = SubProgramWrapper<CONFIG>::GetInstance(arg1)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a SubProgram for 'subprogram'"));
			return;
		}
		
		subprogram_breakpoint = processor_wrapper->GetProcessor()->CreateSubProgramBreakpoint(subprogram_wrapper->GetSubProgram());
	}
	
	SubProgramBreakpointWrapper<CONFIG> *subprogram_breakpoint_wrapper = new SubProgramBreakpointWrapper<CONFIG>(nodejs, processor_wrapper, subprogram_breakpoint);
	
	subprogram_breakpoint_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
SubProgramBreakpointWrapper<CONFIG>::SubProgramBreakpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::SubProgramBreakpoint<ADDRESS> *_subprogram_breakpoint, std::size_t size)
	: Super(_nodejs, _processor_wrapper, _subprogram_breakpoint, size ? size : sizeof(*this))
	, subprogram_breakpoint(_subprogram_breakpoint)
{
}

template <typename CONFIG>
SubProgramBreakpointWrapper<CONFIG>::~SubProgramBreakpointWrapper()
{
}

template <typename CONFIG>
unisim::util::debug::SubProgramBreakpoint<typename CONFIG::ADDRESS> *SubProgramBreakpointWrapper<CONFIG>::GetSubProgramBreakpoint() const
{
	return subprogram_breakpoint;
}

template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::GetSubProgram(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram_breakpoint)
	{
		const unisim::util::debug::SubProgram<ADDRESS> *subprogram = subprogram_breakpoint->GetSubProgram();
		if(subprogram)
		{
			SubProgramWrapper<CONFIG> *subprogram_wrapper = SubProgramWrapper<CONFIG>::Wrap(this->nodejs, subprogram);
			info.GetReturnValue().Set(subprogram_wrapper->MakeObject());
		}
	}
}

template <typename CONFIG>
void SubProgramBreakpointWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/subprogram_breakpoint.h>
	;
}

//////////////////////////////// SourceCodeBreakpointWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *SourceCodeBreakpointWrapper<CONFIG>::CLASS_NAME = "SourceCodeBreakpoint";

template <typename CONFIG>
const uint32_t SourceCodeBreakpointWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> SourceCodeBreakpointWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> source_code_breakpoint_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = source_code_breakpoint_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "file", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetFile>, 0 },
		{ "loc" , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetLoc> , 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Inherit from "DebugEvent"
	source_code_breakpoint_function_template->Inherit(nodejs.template GetCtorFunctionTemplate<DebugEventWrapper<CONFIG> >());

	return handle_scope.Escape(source_code_breakpoint_function_template);
}

// SourceCodeBreakpoint() => SourceCodeBreakpoint
// SourceCodeBreakpoint(processor: Processor, file : string, loc : (Object|string)) => SourceCodeBreakpoint
template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = 0;
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, file : string, loc : (Object|string))"; } };
		
		v8::Local<v8::Value> arg0 = args[0]; // processor
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1]; // file
		std::string file;
		if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, file))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a string for 'file'"));
			return;
		}
		v8::Local<v8::Value> arg2 = args[2]; // loc
		
		unisim::util::debug::SourceCodeLocation source_code_location;
		if(!nodejs.ArgToSourceCodeLocation(arg2, Synopsis().str(), "loc", source_code_location)) return;
		
		source_code_breakpoint = processor_wrapper->GetProcessor()->CreateSourceCodeBreakpoint(source_code_location, nodejs.LocateFile(file));
	}
	
	SourceCodeBreakpointWrapper<CONFIG> *source_code_breakpoint_wrapper = new SourceCodeBreakpointWrapper<CONFIG>(nodejs, processor_wrapper, source_code_breakpoint);
	source_code_breakpoint_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
SourceCodeBreakpointWrapper<CONFIG>::SourceCodeBreakpointWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *_source_code_breakpoint, std::size_t size)
	: Super(_nodejs, _processor_wrapper, _source_code_breakpoint, size ? size : sizeof(*this))
	, source_code_breakpoint(_source_code_breakpoint)
{
}

template <typename CONFIG>
SourceCodeBreakpointWrapper<CONFIG>::~SourceCodeBreakpointWrapper()
{
}

template <typename CONFIG>
unisim::util::debug::SourceCodeBreakpoint<typename CONFIG::ADDRESS> *SourceCodeBreakpointWrapper<CONFIG>::GetSourceCodeBreakpoint() const
{
	return source_code_breakpoint;
}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::GetFile(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(source_code_breakpoint) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), source_code_breakpoint->GetFilename().c_str()).ToLocalChecked());
}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::GetLoc(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(source_code_breakpoint)
	{
		SourceCodeLocationWrapper<CONFIG> *source_code_location_wrapper = new SourceCodeLocationWrapper<CONFIG>(this->nodejs, source_code_breakpoint->GetSourceCodeLocation());
		info.GetReturnValue().Set(source_code_location_wrapper->MakeObject());
	}
}

template <typename CONFIG>
void SourceCodeBreakpointWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/source_code_breakpoint.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_BREAKPOINT_TCC__
