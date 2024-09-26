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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_SOURCE_CODE_LOCATION_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_SOURCE_CODE_LOCATION_TCC__

#include <unisim/service/debug/nodejs/source_code_location.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::ToString;
using unisim::util::ostream::ToString;
using unisim::util::nodejs::MakeInteger;
using unisim::util::nodejs::ToInt;

//////////////////////////////// SourceCodeLocationWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *SourceCodeLocationWrapper<CONFIG>::CLASS_NAME = "SourceCodeLocation";

template <typename CONFIG>
const uint32_t SourceCodeLocationWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> SourceCodeLocationWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> source_code_location_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = source_code_location_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "sourceCodeFilename", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetSourceCodeFilename>, 0 },
		{ "lineno"            , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetLineNo            >, 0 },
		{ "colno"             , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetColNo             >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(source_code_location_function_template);
}

// SourceCodeLocation() => SourceCodeLocation
// SourceCodeLocation(sourceCodeFilename: string, lineno: Number, [colno: Number]) => SourceCodeLocation
// SourceCodeLocation(loc: string) => SourceCodeLocation
template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	unisim::util::debug::SourceCodeLocation source_code_location;
	if(args.Length() > 1)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(sourceCodeFilename: string, lineno: Number, [colno: Number])"; } };
		v8::Local<v8::Value> arg0 = args[0]; // sourceCodeFilename
		std::string source_code_filename;
		if(!arg0->IsString() || !ToString(args.GetIsolate(), arg0, source_code_filename))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a string for 'sourceCodeFilename'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1]; // lineno
		int64_t lineno = 0;
		if((!arg1->IsNumber() && !arg1->IsBigInt()) || !ToInt(args.GetIsolate(), arg1, lineno))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for 'lineno'"));
			return;
		}
		if((lineno < 1) || (lineno > std::numeric_limits<unsigned>::max()))
		{
			nodejs.Throw(nodejs.RangeError(Synopsis().str() + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for 'lineno'"));
			return;
		}
		int64_t colno = 0;
		if(args.Length() >= 3)
		{
			v8::Local<v8::Value> arg2 = args[2]; // colno
			if((!arg2->IsNumber() && !arg2->IsBigInt()) || !ToInt(args.GetIsolate(), arg2, colno))
			{
				nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.colno'"));
				return;
			}
			if((colno < 1) || (colno > std::numeric_limits<unsigned>::max()))
			{
				nodejs.Throw(nodejs.RangeError(Synopsis().str() + " expects a number in range [1," + ToString(std::numeric_limits<unsigned>::max()) + "] for property 'loc.colno'"));
				return;
			}
		}
		
		source_code_location = unisim::util::debug::SourceCodeLocation(source_code_filename, lineno, colno);
	}
	else if(args.Length() > 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(loc: string)"; } };
		v8::Local<v8::Value> arg0 = args[0]; // loc
		std::string loc;
		if(!arg0->IsString() || !ToString(args.GetIsolate(), arg0, loc) || !source_code_location.Parse(loc))
		{
			nodejs.Throw(nodejs.RangeError(Synopsis().str() + " expects a source code location as a string for 'loc'"));
			return;
		}
	}
	
	SourceCodeLocationWrapper<CONFIG> *source_code_location_wrapper = new SourceCodeLocationWrapper<CONFIG>(nodejs, source_code_location);
	source_code_location_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
SourceCodeLocationWrapper<CONFIG>::SourceCodeLocationWrapper(NodeJS<CONFIG>& _nodejs, unisim::util::debug::SourceCodeLocation const& _source_code_location, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, source_code_location(_source_code_location)
{
}

template <typename CONFIG>
SourceCodeLocationWrapper<CONFIG>::~SourceCodeLocationWrapper()
{
}

template <typename CONFIG>
unisim::util::debug::SourceCodeLocation const& SourceCodeLocationWrapper<CONFIG>::GetSourceCodeLocation() const
{
	return source_code_location;
}

template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::GetSourceCodeFilename(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), source_code_location.GetSourceCodeFilename().c_str()).ToLocalChecked());
}

template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::GetLineNo(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), source_code_location.GetLineNo()));
}

template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::GetColNo(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), source_code_location.GetColNo()));
}

template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/source_code_location.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_SOURCE_CODE_LOCATION_TCC__
