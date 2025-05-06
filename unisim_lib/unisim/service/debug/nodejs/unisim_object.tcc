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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_UNISIM_OBJECT_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_UNISIM_OBJECT_TCC__

#include <unisim/service/debug/nodejs/unisim_object.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// UnisimObjectWrapper<> /////////////////////////////////

template <typename CONFIG>
const char *UnisimObjectWrapper<CONFIG>::CLASS_NAME = "UnisimObject";

template <typename CONFIG>
const uint32_t UnisimObjectWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> UnisimObjectWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> unisim_object_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = unisim_object_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "name"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName       >, 0 },
		{ "description", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetDescription>, 0 }
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	return handle_scope.Escape(unisim_object_function_template);
}

// UnisimObject() => UnisimObject
template <typename CONFIG>
void UnisimObjectWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
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
UnisimObjectWrapper<CONFIG> *UnisimObjectWrapper<CONFIG>::Wrap(NodeJS<CONFIG>& nodejs, unisim::kernel::Object *unisim_object)
{
	This *unisim_object_wrapper = nodejs.template GetObjectWrapper<This>(unisim_object);
	return unisim_object_wrapper ? unisim_object_wrapper : new This(nodejs, unisim_object);
}

template <typename CONFIG>
UnisimObjectWrapper<CONFIG>::UnisimObjectWrapper(NodeJS<CONFIG>& _nodejs, unisim::kernel::Object *_unisim_object, std::size_t size)
	: Super(_nodejs, _unisim_object, size ? size : sizeof(*this))
	, unisim_object(_unisim_object)
{
}

template <typename CONFIG>
UnisimObjectWrapper<CONFIG>::~UnisimObjectWrapper()
{
}

template <typename CONFIG>
unisim::kernel::Object *UnisimObjectWrapper<CONFIG>::GetUnisimObject() const
{
	return unisim_object;
}

template <typename CONFIG>
void UnisimObjectWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_object) info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), unisim_object->GetName()).ToLocalChecked());
}

template <typename CONFIG>
void UnisimObjectWrapper<CONFIG>::GetDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(unisim_object) info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), unisim_object->GetDescription()).ToLocalChecked());
}

template <typename CONFIG>
v8::Local<v8::Object> UnisimObjectWrapper<CONFIG>::MakeObject()
{
	v8::EscapableHandleScope handle_scope(this->GetIsolate());
	
	v8::Local<v8::Object> unisim_object_object = this->ThisObject();
	
	if(unisim_object_object.IsEmpty())
	{
		unisim_object_object = Super::template MakePersistentObject<This>();
	
		struct ChildVisitor
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& unisim_object_object;
			
			ChildVisitor(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _unisim_object_object) : nodejs(_nodejs), unisim_object_object(_unisim_object_object) {}
			
			bool Visit(unisim::kernel::Object *unisim_child)
			{
				UnisimObjectWrapper<CONFIG> *unisim_child_wrapper = UnisimObjectWrapper<CONFIG>::Wrap(nodejs, unisim_child);
				unisim_object_object->DefineOwnProperty(
					nodejs.GetIsolate()->GetCurrentContext(),
					v8::String::NewFromUtf8(nodejs.GetIsolate(), unisim_child->GetObjectName(), v8::NewStringType::kInternalized).ToLocalChecked(),
					unisim_child_wrapper->MakeObject(),
					v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
				).ToChecked();
				return false;
			}
		};
		
		struct RootObjectVisitor
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& unisim_object_object;
			ChildVisitor& child_visitor;
			
			RootObjectVisitor(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _unisim_object_object, ChildVisitor& _child_visitor)
				: nodejs(_nodejs), unisim_object_object(_unisim_object_object), child_visitor(_child_visitor) {}
			
			bool Visit(unisim::kernel::Object *unisim_object)
			{
				if(!unisim_object->GetParent())
				{
					child_visitor.Visit(unisim_object);
				}
				return false;
			}
		};
		
		ChildVisitor child_visitor(this->nodejs, unisim_object_object);
		
		if(this->unisim_object)
		{
			this->unisim_object->ScanChildren(child_visitor);
		}
		else
		{
			RootObjectVisitor root_object_visitor(this->nodejs, unisim_object_object, child_visitor);
			unisim::kernel::Simulator::Instance()->ScanObjects(root_object_visitor);
		}
		
		struct VariableVisitor
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& unisim_object_object;
			
			VariableVisitor(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _unisim_object_object) : nodejs(_nodejs), unisim_object_object(_unisim_object_object) {}
			
			bool Visit(unisim::kernel::VariableBase *unisim_variable)
			{
				switch(unisim_variable->GetType())
				{
					case unisim::kernel::VariableBase::VAR_PARAMETER:
					case unisim::kernel::VariableBase::VAR_STATISTIC:
					{
						if(!unisim_variable->GetContainer())
						{
							UnisimVariableWrapper<CONFIG> *unisim_variable_wrapper = UnisimVariableWrapper<CONFIG>::Wrap(nodejs, unisim_variable);
							unisim_object_object->DefineOwnProperty(
								nodejs.GetIsolate()->GetCurrentContext(),
								v8::String::NewFromUtf8(nodejs.GetIsolate(), unisim_variable->GetVarName(), v8::NewStringType::kInternalized).ToLocalChecked(),
								unisim_variable_wrapper->MakeObject(),
								v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
							).ToChecked();
						}
						break;
					}
					case unisim::kernel::VariableBase::VAR_ARRAY:
					{
						std::size_t length = unisim_variable->GetLength();
						v8::Local<v8::Array> unisim_variable_array = v8::Array::New(nodejs.GetIsolate(), length);
						for(std::size_t idx = 0; idx < length; ++idx)
						{
							UnisimVariableWrapper<CONFIG> *unisim_variable_wrapper = UnisimVariableWrapper<CONFIG>::Wrap(nodejs, &(*unisim_variable)[idx]);
							unisim_variable_array->Set(nodejs.GetContext(), idx, unisim_variable_wrapper->MakeObject().template As<v8::Value>()).ToChecked();
						}
						
						unisim_object_object->DefineOwnProperty(
							nodejs.GetIsolate()->GetCurrentContext(),
							v8::String::NewFromUtf8(nodejs.GetIsolate(), unisim_variable->GetVarName(), v8::NewStringType::kInternalized).ToLocalChecked(),
							unisim_variable_array,
							v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete)
						).ToChecked();
						break;
					}
					default:
						break;
				}
				return false;
			}
		};
		
		struct RootVariableVisitor
		{
			NodeJS<CONFIG>& nodejs;
			v8::Local<v8::Object>& unisim_object_object;
			VariableVisitor& variable_visitor;
			
			RootVariableVisitor(NodeJS<CONFIG>& _nodejs, v8::Local<v8::Object>& _unisim_object_object, VariableVisitor& _variable_visitor)
				: nodejs(_nodejs), unisim_object_object(_unisim_object_object), variable_visitor(_variable_visitor) {}
			
			bool Visit(unisim::kernel::VariableBase *unisim_variable)
			{
				if(!unisim_variable->GetOwner())
				{
					variable_visitor.Visit(unisim_variable);
				}
				return false;
			}
		};
		
		VariableVisitor variable_visitor(this->nodejs, unisim_object_object);
		
		if(this->unisim_object)
		{
			this->unisim_object->ScanVariables(variable_visitor);
		}
		else
		{
			RootVariableVisitor root_variable_visitor(this->nodejs, unisim_object_object, variable_visitor);
			unisim::kernel::Simulator::Instance()->ScanVariables(root_variable_visitor);
		}
	}
	
	return handle_scope.Escape(unisim_object_object);
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_UNISIM_OBJECT_TCC__
