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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_STATEMENT_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_STATEMENT_TCC__

#include <unisim/service/debug/nodejs/statement.hh>
#include <unisim/util/debug/stmt.tcc>
namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// StatementWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *StatementWrapper<CONFIG>::CLASS_NAME = "Statement";

template <typename CONFIG>
const uint32_t StatementWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> StatementWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> statement_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = statement_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "address"                     , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetAddress                     >, 0 },
		{ "isBeginningOfSourceStatement", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsBeginningOfSourceStatement>, 0 },
		{ "isBeginningOfBasicBlock"     , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsBeginningOfBasicBlock     >, 0 },
		{ "sourceDirname"               , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetSourceDirname               >, 0 },
		{ "sourceFilename"              , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetSourceFilename              >, 0 },
		{ "lineno"                      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetLineNo                      >, 0 },
		{ "colno"                       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetColNo                       >, 0 },
		{ "isa"                         , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetISA                         >, 0 },
		{ "discriminator"               , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDiscriminator               >, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(statement_function_template);
}

// Statement() => Statement
template <typename CONFIG>
void StatementWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
StatementWrapper<CONFIG> *StatementWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Statement<ADDRESS> *stmt)
{
	This *statement_wrapper = nodejs.template GetObjectWrapper<This>(stmt);
	return statement_wrapper ? statement_wrapper : new This(nodejs, stmt);
}

template <typename CONFIG>
StatementWrapper<CONFIG>::StatementWrapper(NodeJS<CONFIG>& _nodejs, const unisim::util::debug::Statement<ADDRESS> *_stmt, std::size_t size)
	: Super(_nodejs, _stmt, size ? size : sizeof(*this))
	, stmt(_stmt)
{
}

template <typename CONFIG>
StatementWrapper<CONFIG>::~StatementWrapper()
{
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *StatementWrapper<CONFIG>::GetStatement() const
{
	return stmt;
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stmt->GetAddress()));
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetIsBeginningOfSourceStatement(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), stmt->IsBeginningOfSourceStatement()));
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetIsBeginningOfBasicBlock(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), stmt->IsBeginningOfBasicBlock()));
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetSourceDirname(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), stmt->GetSourceDirname()).ToLocalChecked());
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetSourceFilename(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), stmt->GetSourceFilename()).ToLocalChecked());
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetLineNo(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stmt->GetLineNo()));
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetColNo(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stmt->GetColNo()));
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetISA(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stmt->GetISA()));
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::GetDiscriminator(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(stmt) info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), stmt->GetDiscriminator()));
}

template <typename CONFIG>
void StatementWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/statement.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_STATEMENT_TCC__
