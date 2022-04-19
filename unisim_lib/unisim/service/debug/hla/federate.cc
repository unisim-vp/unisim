/*
 *  Copyright (c) 2022,
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

#include <unisim/service/debug/hla/federate.hh>

namespace unisim {
namespace service {
namespace debug {
namespace hla {

Instrument::Instrument()
	: direction()
	, type()
	, expr()
	, hook_stub_name()
{
}

Instrument::~Instrument()
{
}

void Instrument::Initialize(const unisim::util::json::JSON_Object& config)
{
	std::string direction_string = config["direction"].AsString();
	if(direction_string == "write") direction = WRITE;
	else if(direction_string == "read") direction = READ;
	expr = (const std::string&) config["expr"].AsString();
	if(config.HasProperty("hook"))
	{
		type = HOOK;
		hook_stub_name = (const std::string&) config["hook"].AsString();
	}
	else if(config.HasProperty("stub"))
	{
		type = STUB;
		hook_stub_name = (const std::string&) config["stub"].AsString();
	}
}

Attribute::Attribute(const ObjectClass *_object_class)
	: object_class(_object_class)
	, name()
	, handle()
	, publish(false)
	, published(false)
	, subscribe(false)
	, subscribed(false)
{
}

void Attribute::Initialize(const unisim::util::json::JSON_Object& config)
{
	name = config["name"].AsString();
	if(config.HasProperty("publish")) publish = config["publish"].AsBoolean();
	if(config.HasProperty("subscribe")) subscribe = config["subscribe"].AsBoolean();
}

AttributeValue::AttributeValue(const ObjectInstance *_object_instance, const Attribute *_attribute)
	: object_instance(_object_instance)
	, attribute(_attribute)
	, instrument()
	, value()
	, valid(false)
{
}

void AttributeValue::Initialize(const unisim::util::json::JSON_Object& config)
{
	const unisim::util::json::JSON_Object& instrument_config = config["instrument"].AsObject();
	
	try
	{
		instrument.Initialize(instrument_config);
	}
	catch(unisim::util::json::Exception& e)
	{
		throw e;
	}
}

ObjectInstance::ObjectInstance(const ObjectClass *_object_class)
	: object_class(_object_class)
	, name()
	, handle()
{
}

void ObjectInstance::Initialize(const unisim::util::json::JSON_Object& config)
{
	name = config["name"].AsString();

	if(config.HasProperty("attributeValues"))
	{
		const unisim::util::json::JSON_Array& attribute_value_configs = config["attributeValues"].AsArray();
		
		unisim::util::json::JSON_Array::size_type n_attribute_values = attribute_value_configs.Length();
		for(unisim::util::json::JSON_Array::size_type i = 0; i < n_attribute_values; ++i)
		{
			const unisim::util::json::JSON_Object& attribute_value_config = attribute_value_configs[i].AsObject();
			
			std::wstring attribute_name = attribute_value_config["name"].AsString();
			
			Attribute *attribute = object_class->GetAttribute(attribute_name);
			
			AttributeValue *attribute_value = new AttributeValue(this, attribute);
			
			try
			{
				attribute_value->Initialize(attribute_value_config);
			}
			catch(unisim::util::json::Exception& e)
			{
				delete attribute_value;
				throw e;
			}
			
			AddAttributeValue(attribute_value);
		}
	}
}

void ObjectInstance::AddAttributeValue(AttributeValue *attribute_value)
{
	attribute_values[attribute_value->GetAttribute()->GetName()] = attribute_value;
}

AttributeValue *ObjectInstance::GetAttributeValue(const std::wstring& name)
{
	return attribute_values[name];
}

ObjectClass::ObjectClass()
	: name()
	, handle()
{
}

void ObjectClass::Initialize(const unisim::util::json::JSON_Object& config)
{
	name = config["name"].AsString();
	
	const unisim::util::json::JSON_Array& attribute_configs = config["attributes"].AsArray();
	
	unisim::util::json::JSON_Array::size_type n_attributes = attribute_configs.Length();
	for(unisim::util::json::JSON_Array::size_type i = 0; i < n_attributes; ++i)
	{
		const unisim::util::json::JSON_Object& attribute_config = attribute_configs[i].AsObject();
		
		Attribute *attribute = new Attribute(this);
		
		try
		{
			attribute->Initialize(attribute_config);
		}
		catch(unisim::util::json::Exception& e)
		{
			delete attribute;
			throw e;
		}
		
		AddAttribute(attribute);
	}
	
	if(config.HasProperty("objectInstances"))
	{
		const unisim::util::json::JSON_Array& object_instance_configs = config["objectInstances"].AsArray();
		unisim::util::json::JSON_Array::size_type n_object_instances = object_instance_configs.Length();
		for(unisim::util::json::JSON_Array::size_type i = 0; i < n_object_instances; ++i)
		{
			const unisim::util::json::JSON_Object& object_instance_config = object_instance_configs[i].AsObject();
			
			ObjectInstance *object_instance = new ObjectInstance(this);
			
			try
			{
				object_instance->Initialize(object_instance_config);
			}
			catch(unisim::util::json::Exception& e)
			{
				delete object_instance;
				throw e;
			}
			
			AddObjectInstance(object_instance);
		}
	}
}

void ObjectClass::AddAttribute(Attribute *attribute)
{
	attributes[attribute->GetName()] = attribute;
}

Attribute *ObjectClass::GetAttribute(const std::wstring& name) const
{
	Attributes::const_iterator it = attributes.find(name);
	return (it != attributes.end()) ? (*it).second : 0;
}

void ObjectClass::AddObjectInstance(ObjectInstance *object_instance)
{
	object_instances[object_instance->GetName()] = object_instance;
	object_instance->object_class = this;
}

ObjectInstance *ObjectClass::GetObjectInstance(const std::wstring& name)
{
	return object_instances[name];
}

HookStub::HookStub()
	: name()
	, processor(0)
	, loc()
	, json_return_value(0)
{
}

HookStub::~HookStub()
{
	if(json_return_value) delete json_return_value;
}

void HookStub::Initialize(const unisim::util::json::JSON_Object& config)
{
	name = (const std::string&) config["name"].AsString();
	processor = config["processor"].AsInteger();
	loc = (const std::string&) config["loc"].AsString();
	if(config.HasProperty("return"))
	{
		json_return_value = config["return"].Clone();
	}
}

Registry::Registry()
{
}

bool Registry::Initialize(const unisim::util::json::JSON_Object& config, unisim::kernel::logger::Logger& logger)
{
	if(config.HasProperty("objectClasses"))
	{
		const unisim::util::json::JSON_Array& object_classe_configs = config["objectClasses"].AsArray();
		
		unisim::util::json::JSON_Array::size_type n = object_classe_configs.Length();
		
		for(unisim::util::json::JSON_Array::size_type i = 0; i < n; ++i)
		{
			const unisim::util::json::JSON_Object& object_class_config = object_classe_configs[i].AsObject();
			
			ObjectClass *object_class = new ObjectClass();
				
			object_class->Initialize(object_class_config);
				
			if(!AddObjectClass(object_class))
			{
				logger << DebugError << "Duplicate Object class \"" << object_class->GetName() << "\"" << EndDebugError;
				delete object_class;
				return false;
			}
		}
	}
	
	if(config.HasProperty("hooks"))
	{
		const unisim::util::json::JSON_Array& hook_configs = config["hooks"].AsArray();
		
		unisim::util::json::JSON_Array::size_type n = hook_configs.Length();
		
		for(unisim::util::json::JSON_Array::size_type i = 0; i < n; ++i)
		{
			const unisim::util::json::JSON_Object& hook_config = hook_configs[i].AsObject();
			
			Hook *hook = new Hook();
			
			hook->Initialize(hook_config);
			
			if(!AddHook(hook))
			{
				logger << DebugError << "Duplicate Hook \"" << hook->GetName() << "\"" << EndDebugError;
				delete hook;
				return false;
			}
		}
	}
	
	if(config.HasProperty("stubs"))
	{
		const unisim::util::json::JSON_Array& stub_configs = config["stubs"].AsArray();
		
		unisim::util::json::JSON_Array::size_type n = stub_configs.Length();
		
		for(unisim::util::json::JSON_Array::size_type i = 0; i < n; ++i)
		{
			const unisim::util::json::JSON_Object& stub_config = stub_configs[i].AsObject();
			
			Stub *stub = new Stub();
			
			stub->Initialize(stub_config);
			
			if(!AddStub(stub))
			{
				logger << DebugError << "Duplicate Stub \"" << stub->GetName() << "\"" << EndDebugError;
				delete stub;
				return false;
			}
		}
	}
	
	return true;
}

Registry::~Registry()
{
	for(ObjectClasses::iterator it = object_classes.begin(); it != object_classes.end(); ++it)
	{
		delete (*it).second;
	}
}

bool Registry::AddObjectClass(ObjectClass *object_class)
{
	if(GetObjectClass(object_class->GetName())) return false;
	object_classes[object_class->GetName()] = object_class;
	return true;
}

bool Registry::AddHook(Hook *hook)
{
	if(GetHook(hook->GetName())) return false;
	hooks[hook->GetName()] = hook;
	return true;
}

bool Registry::AddStub(Stub *stub)
{
	if(GetStub(stub->GetName())) return false;
	stubs[stub->GetName()] = stub;
	return true;
}

ObjectClass *Registry::GetObjectClass(const std::wstring& name) const
{
	ObjectClasses::const_iterator it = object_classes.find(name);
	return (it != object_classes.end()) ? (*it).second : 0;
}

Hook *Registry::GetHook(const std::string& name) const
{
	Hooks::const_iterator it = hooks.find(name);
	return (it != hooks.end()) ? (*it).second : 0;
}

Stub *Registry::GetStub(const std::string& name) const
{
	Stubs::const_iterator it = stubs.find(name);
	return (it != stubs.end()) ? (*it).second : 0;
}

FederateHookStubBase::FederateHookStubBase(const std::string& _name, const unisim::util::json::JSON_Value *_json_return_value)
	: name(_name)
	, dependencies(0)
	, json_return_value(_json_return_value)
{
}

} // end of namespace hla
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
