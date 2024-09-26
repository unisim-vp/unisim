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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_EXECUTABLE_BINARY_FILE_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_EXECUTABLE_BINARY_FILE_TCC__

#include <unisim/service/debug/nodejs/executable_binary_file.hh>
#include <unisim/util/ostream/ostream.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::ostream::ToString;

//////////////////////////////// ExecutableBinaryFileWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *ExecutableBinaryFileWrapper<CONFIG>::CLASS_NAME = "ExecutableBinaryFile";

template <typename CONFIG>
const uint32_t ExecutableBinaryFileWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> ExecutableBinaryFileWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> executable_binary_file_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = executable_binary_file_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "file"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetFile      >, 0                                                                    },
		{ "fileFormat", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetFileFormat>, 0                                                                    },
		{ "enable"    , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetEnable    >, unisim::util::nodejs::AccessorSetterCallback<This, &This::SetEnable> }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(executable_binary_file_function_template);
}

// ExecutableBinaryFile() => ExecutableBinaryFile
template <typename CONFIG>
void ExecutableBinaryFileWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	Base::BindObject(nodejs, args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
ExecutableBinaryFileWrapper<CONFIG> *ExecutableBinaryFileWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, unisim::service::interfaces::ExecutableBinaryFile *executable_binary_file)
{
	This *executable_binary_file_wrapper = nodejs.template GetObjectWrapper<This>(executable_binary_file);
	return executable_binary_file_wrapper ? executable_binary_file_wrapper : new This(nodejs, executable_binary_file);
}

template <typename CONFIG>
ExecutableBinaryFileWrapper<CONFIG>::ExecutableBinaryFileWrapper(NodeJS<CONFIG>& _nodejs, unisim::service::interfaces::ExecutableBinaryFile *_executable_binary_file, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, executable_binary_file(_executable_binary_file)
{
}

template <typename CONFIG>
ExecutableBinaryFileWrapper<CONFIG>::~ExecutableBinaryFileWrapper()
{
}

template <typename CONFIG>
unisim::service::interfaces::ExecutableBinaryFile *ExecutableBinaryFileWrapper<CONFIG>::GetExecutableBinaryFile() const
{
	return executable_binary_file;
}

template <typename CONFIG>
void ExecutableBinaryFileWrapper<CONFIG>::GetFile(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(executable_binary_file) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), executable_binary_file->GetFilename()).ToLocalChecked());
}

template <typename CONFIG>
void ExecutableBinaryFileWrapper<CONFIG>::GetFileFormat(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(executable_binary_file) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), ToString(executable_binary_file->GetFileFormat()).c_str()).ToLocalChecked());
}

template <typename CONFIG>
void ExecutableBinaryFileWrapper<CONFIG>::GetEnable(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(executable_binary_file) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), executable_binary_file->IsEnabled()));
}

template <typename CONFIG>
void ExecutableBinaryFileWrapper<CONFIG>::SetEnable(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	if(executable_binary_file) executable_binary_file->Enable(value->ToBoolean(this->GetIsolate())->Value());
}

template <typename CONFIG>
void ExecutableBinaryFileWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/executable_binary_file.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_EXECUTABLE_BINARY_FILE_TCC__
