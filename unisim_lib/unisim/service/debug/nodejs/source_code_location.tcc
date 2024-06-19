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

using unisim::util::nodejs::MakeInteger;

//////////////////////////////// SourceCodeLocationWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *SourceCodeLocationWrapper<CONFIG>::CLASS_NAME = "SourceCodeLocation";

template <typename CONFIG>
const uint32_t SourceCodeLocationWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Global<v8::ObjectTemplate> SourceCodeLocationWrapper<CONFIG>::cached_object_template;

template <typename CONFIG>
SourceCodeLocationWrapper<CONFIG>::SourceCodeLocationWrapper(NodeJS<CONFIG>& _nodejs, unisim::util::debug::SourceCodeLocation const& _source_code_location, std::size_t size)
	: Super(_nodejs, size ? size : sizeof(*this))
	, source_code_location(_source_code_location)
	, shadow_object()
{
}

template <typename CONFIG>
SourceCodeLocationWrapper<CONFIG>::~SourceCodeLocationWrapper()
{
}

template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::Cleanup()
{
	cached_object_template.Reset();
}

template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::Finalize()
{
	shadow_object.Reset();
	Super::Finalize();
}

template <typename CONFIG>
void SourceCodeLocationWrapper<CONFIG>::FillObjectTemplate(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template)
{
	// add "XXXX" method
	//object_template->Set(isolate, "xxxx", unisim::util::nodejs::CreateFunctionTemplate<SourceCodeLocationWrapper<CONFIG>, &SourceCodeLocationWrapper<CONFIG>::XXXX>(isolate));
}

template <typename CONFIG>
v8::Local<v8::ObjectTemplate> SourceCodeLocationWrapper<CONFIG>::MakeObjectTemplate(v8::Isolate *isolate)
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
v8::Local<v8::Object> SourceCodeLocationWrapper<CONFIG>::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
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
	// add "source_code_filename" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "source_code_filename", v8::NewStringType::kInternalized),
		v8::String::NewFromUtf8(this->GetIsolate(), source_code_location.GetSourceCodeFilename().c_str()).ToLocalChecked(),
		v8::ReadOnly
 	).ToChecked();
	// add "lineno" read-only property
	object->DefineOwnProperty(
		this->GetIsolate()->GetCurrentContext(),
		v8::String::NewFromUtf8Literal(this->GetIsolate(), "lineno", v8::NewStringType::kInternalized),
		MakeInteger(this->GetIsolate(), source_code_location.GetLineNo()),
		v8::ReadOnly
 	).ToChecked();
	// add "colno" read-only property
	if(source_code_location.GetColNo())
	{
		object->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "colno", v8::NewStringType::kInternalized),
			MakeInteger(this->GetIsolate(), source_code_location.GetColNo()),
			v8::ReadOnly
		).ToChecked();
	}
	
	// keep object reference for setting "this" of callback
	shadow_object.Reset(this->GetIsolate(), object);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
v8::Local<v8::Object> SourceCodeLocationWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	// create object template
	v8::Local<v8::ObjectTemplate> object_template = MakeObjectTemplate(this->GetIsolate());
	// create object
	v8::Local<v8::Object> object = this->template MakeObject<This>(object_template);
	
	return handle_scope.Escape(object);
}

template <typename CONFIG>
unisim::util::debug::SourceCodeLocation const& SourceCodeLocationWrapper<CONFIG>::GetSourceCodeLocation() const
{
	return source_code_location;
}

//template <typename CONFIG>
//void SourceCodeLocationWrapper<CONFIG>::XXXX(const v8::FunctionCallbackInfo<v8::Value>& args)
//{
//	v8::Local<v8::Value> recv = shadow_object.Get(this->GetIsolate()); // "this"
//}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_SOURCE_CODE_LOCATION_TCC__
