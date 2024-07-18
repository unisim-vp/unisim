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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_SUB_PROGRAM_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_SUB_PROGRAM_TCC__

#include <unisim/service/debug/nodejs/subprogram.hh>
#include <unisim/util/debug/subprogram.tcc>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// SubProgramWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *SubProgramWrapper<CONFIG>::CLASS_NAME = "SubProgram";

template <typename CONFIG>
const uint32_t SubProgramWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> SubProgramWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> subprogram_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = subprogram_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "file"         , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetFile         >, 0 },
		{ "name"         , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName         >, 0 },
		{ "isExternal"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsExternal   >, 0 },
		{ "isDeclaration", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsDeclaration>, 0 },
		{ "isInline"     , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsInline     >, 0 },
		{ "isInlined"    , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsInlined    >, 0 },
		{ "returnType"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetReturnType   >, 0 },
		{ "arity"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetArity        >, 0 },
		{ "declLoc"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDeclLoc      >, 0 },
		{ "address"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetAddress      >, 0 },
		{ "cdecl"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetCDecl        >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(subprogram_function_template);
}

// SubProgram() => SubProgram
template <typename CONFIG>
void SubProgramWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
SubProgramWrapper<CONFIG> *SubProgramWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::SubProgram<ADDRESS> *subprogram)
{
	This *subprogram_wrapper = nodejs.template GetObjectWrapper<This>(subprogram);
	return subprogram_wrapper ? subprogram_wrapper : new This(nodejs, subprogram);
}

template <typename CONFIG>
SubProgramWrapper<CONFIG>::SubProgramWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::SubProgram<ADDRESS> *_subprogram, std::size_t size)
	: Super(_nodejs, _subprogram, size ? size : sizeof(*this))
	, subprogram(_subprogram)
{
	if(subprogram) subprogram->Catch();
}

template <typename CONFIG>
SubProgramWrapper<CONFIG>::~SubProgramWrapper()
{
	if(subprogram) subprogram->Release();
}

template <typename CONFIG>
const unisim::util::debug::SubProgram<typename CONFIG::ADDRESS> *SubProgramWrapper<CONFIG>::GetSubProgram() const
{
	return subprogram;
}

template <typename CONFIG>
v8::Local<v8::Object> SubProgramWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	
	v8::Local<v8::Object> subprogram_object = this->ThisObject();
	
	if(subprogram_object.IsEmpty())
	{
		subprogram_object = Super::template MakePersistentObject<This>();
	
		struct SubProgramScanner
		{
			NodeJS<CONFIG>& nodejs;
			std::vector<v8::Local<v8::Value> > formal_parameter_objects;
			
			SubProgramScanner(NodeJS<CONFIG>& _nodejs) : nodejs(_nodejs), formal_parameter_objects() {}
			
			bool Visit(const unisim::util::debug::Type *type) { return false; }
			
			bool Visit(const unisim::util::debug::FormalParameter *formal_parameter)
			{
				FormalParameterWrapper<CONFIG> *formal_parameter_wrapper = FormalParameterWrapper<CONFIG>::Wrap(nodejs, formal_parameter);
				formal_parameter_objects.push_back(formal_parameter_wrapper->MakeObject().template As<v8::Value>());
				return false; // do not stop: continue with next member
			}
		};
		
		SubProgramScanner subprogram_scanner(this->nodejs);
		this->subprogram->Scan(subprogram_scanner);
		
		// add "formalParameters" read-only property
		subprogram_object->DefineOwnProperty(
			this->GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(this->GetIsolate(), "formalParameters", v8::NewStringType::kInternalized),
			v8::Array::New(this->GetIsolate(), &subprogram_scanner.formal_parameter_objects[0], subprogram_scanner.formal_parameter_objects.size()),
			v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
		).ToChecked();
	}
	
	return handle_scope.Escape(subprogram_object);
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetFile(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), subprogram->GetFilename()).ToLocalChecked());
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), subprogram->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetIsExternal(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), subprogram->IsExternal()));
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetIsDeclaration(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), subprogram->IsDeclaration()));
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetIsInline(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), subprogram->IsInline()));
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetIsInlined(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), subprogram->IsInlined()));
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetReturnType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram)
	{
		const unisim::util::debug::Type *return_type = subprogram->GetReturnType();
		if(return_type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, return_type));
		}
	}
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetArity(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), subprogram->GetArity()));
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetDeclLoc(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram)
	{
		const unisim::util::debug::DeclLocation *decl_location = subprogram->GetDeclLocation();
		if(decl_location)
		{
			SourceCodeLocationWrapper<CONFIG> *source_code_location_wrapper = new SourceCodeLocationWrapper<CONFIG>(this->nodejs, decl_location->GetSourceCodeLocation());
			info.GetReturnValue().Set(source_code_location_wrapper->MakeObject());
		}
	}
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), subprogram->GetAddress()));
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::GetCDecl(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(subprogram) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), subprogram->GetCDecl().c_str()).ToLocalChecked());
}

template <typename CONFIG>
void SubProgramWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/subprogram.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_SUB_PROGRAM_TCC__
