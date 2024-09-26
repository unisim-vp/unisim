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

#ifndef __UNISIM_UTIL_HLA_HLA_TCC__
#define __UNISIM_UTIL_HLA_HLA_TCC__

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_HLA_RTI1516E

#include <unisim/util/hla/hla.hh>
#include <cstring>
#include <limits>

namespace unisim {
namespace util {
namespace hla {

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// ObjectClass //////////////////////////////////

template <typename T> const Attribute<T>& ObjectClass::GetAttribute(const std::string& attr_name) const
{
	return GetAttribute<T>(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

template <typename T> const Attribute<T>& ObjectClass::GetAttribute(const std::wstring& attr_name) const
{
	AttributesPerName::const_iterator it = attrs_per_name.find(attr_name);
	if(it == attrs_per_name.end()) throw UndefinedAttributeError(attr_name);
	const AttributeBase *attr_base = (*it).second;
	const Attribute<T> *attr = dynamic_cast<const Attribute<T> *>(attr_base);
	if(!attr) throw AttributeTypeError(attr_name);
	return *attr;
}

template <typename T> Attribute<T>& ObjectClass::GetAttribute(const std::string& attr_name)
{
	return GetAttribute<T>(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

template <typename T> Attribute<T>& ObjectClass::GetAttribute(const std::wstring& attr_name)
{
	AttributesPerName::iterator it = attrs_per_name.find(attr_name);
	if(it == attrs_per_name.end()) throw UndefinedAttributeError(attr_name);
	AttributeBase *attr_base = (*it).second;
	Attribute<T> *attr = dynamic_cast<Attribute<T> *>(attr_base);
	if(!attr) throw AttributeTypeError(attr_name);
	return *attr;
}

template <typename T> const Attribute<T>& ObjectClass::GetAttribute(const rti1516e::AttributeHandle& attr_handle) const
{
	Attributes::const_iterator it = attrs.find(attr_handle);
	if(it == attrs.end()) throw UndefinedAttributeError(attr_handle);
	const AttributeBase *attr_base = (*it).second;
	const Attribute<T> *attr = dynamic_cast<const Attribute<T> *>(attr_base);
	if(!attr) throw AttributeTypeError(attr_handle);
	return *attr;
}

template <typename T> Attribute<T>& ObjectClass::GetAttribute(const rti1516e::AttributeHandle& attr_handle)
{
	Attributes::iterator it = attrs.find(attr_handle);
	if(it == attrs.end()) throw UndefinedAttributeError(attr_handle);
	AttributeBase *attr_base = (*it).second;
	Attribute<T> *attr = dynamic_cast<Attribute<T> *>(attr_base);
	if(!attr) throw AttributeTypeError(attr_handle);
	return *attr;
}

////////////////////////////// ObjectInstance /////////////////////////////////

template <typename T>
const AttributeValue<T>& ObjectInstance::GetAttributeValue(const std::string& attr_name) const
{
	return GetAttributeValue<T>(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

template <typename T>
const AttributeValue<T>& ObjectInstance::GetAttributeValue(const std::wstring& attr_name) const
{
	AttributeValuesPerName::const_iterator it = attr_values_per_name.find(attr_name);
	if(it == attr_values_per_name.end()) throw UndefinedAttributeError(attr_name);
	const AttributeValueBase *attr_value_base = (*it).second;
	const AttributeValue<T> *attr_value = dynamic_cast<const AttributeValue<T> *>(attr_value_base);
	if(!attr_value) throw AttributeTypeError(attr_name);
	return *attr_value;
}

template <typename T>
AttributeValue<T>& ObjectInstance::GetAttributeValue(const std::string& attr_name)
{
	return GetAttributeValue<T>(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

template <typename T>
AttributeValue<T>& ObjectInstance::GetAttributeValue(const std::wstring& attr_name)
{
	AttributeValuesPerName::iterator it = attr_values_per_name.find(attr_name);
	if(it == attr_values_per_name.end()) throw std::runtime_error(unisim::util::unicode::unicode_wstring_to_utf8_string(std::wstring(L"Undefined attribute ") + attr_name));
	AttributeValueBase *attr_value_base = (*it).second;
	AttributeValue<T> *attr_value = dynamic_cast<AttributeValue<T> *>(attr_value_base);
	if(!attr_value) throw AttributeTypeError(attr_name);
	return *attr_value;
}

template <typename T>
const AttributeValue<T>& ObjectInstance::GetAttributeValue(const rti1516e::AttributeHandle& attr_handle) const
{
	AttributeValues::const_iterator it = attr_values.find(attr_handle);
	if(it == attr_values.end()) throw UndefinedAttributeError(attr_handle);
	const AttributeValueBase *attr_value_base = (*it).second;
	const AttributeValue<T> *attr_value = dynamic_cast<const AttributeValue<T> *>(attr_value_base);
	if(!attr_value) throw AttributeTypeError(attr_handle);
	return *attr_value;
}

template <typename T>
AttributeValue<T>& ObjectInstance::GetAttributeValue(const rti1516e::AttributeHandle& attr_handle)
{
	AttributeValues::iterator it = attr_values.find(attr_handle);
	if(it == attr_values.end()) throw UndefinedAttributeError(attr_handle);
	AttributeValueBase *attr_value_base = (*it).second;
	AttributeValue<T> *attr_value = dynamic_cast<AttributeValue<T> *>(attr_value_base);
	if(!attr_value) throw AttributeTypeError(attr_handle);
	return *attr_value;
}

//////////////////////////////// Attribute<> //////////////////////////////////

template <typename T>
Attribute<T>::Attribute(ObjectClass& _object_class, const std::string& _name, const std::string& _type_name)
	: AttributeBase(_object_class, _name, _type_name)
{
}

template <typename T>
Attribute<T>::Attribute(ObjectClass& _object_class, const std::wstring& _name, const std::wstring& _type_name)
	: AttributeBase(_object_class, _name, _type_name)
{
}

template <typename T>
AttributeValueBase *Attribute<T>::CreateAttributeValue(const ObjectInstance& object_instance)
{
	return new AttributeValue<T>(object_instance, *this);
}

template <typename T>
const AttributeValue<T>& Attribute<T>::Value(const ObjectInstance& object_instance) const
{
	return object_instance.template GetAttributeValue<T>(this->handle);
}

template <typename T>
AttributeValue<T>& Attribute<T>::Value(ObjectInstance& object_instance)
{
	return object_instance.template GetAttributeValue<T>(this->handle);
}

//////////////////////////// AttributeValue<> /////////////////////////////////

template <typename T>
AttributeValue<T>::AttributeValue(const ObjectInstance& _object_instance, const Attribute<T>& _attr, const T& _value)
	: AttributeValueBase(_object_instance, _attr)
	, attr(_attr)
	, value(_value)
{
}

template <typename T>
AttributeValue<T>& AttributeValue<T>::operator = (const T& _value)
{
	value = _value;
	return *this;
}

template <typename T>
AttributeValue<T>::operator const T& () const
{
	return value;
}

template <typename T>
void AttributeValue<T>::Get(rti1516e::VariableLengthData& _value)
{
	_value.setDataPointer(&value, sizeof(value));
}

template <typename T>
void AttributeValue<T>::Set(const rti1516e::VariableLengthData& _value)
{
	memcpy(&value, _value.data(), std::min(_value.size(), sizeof(value)));
}

template <typename T>
std::wostream& AttributeValue<T>::Print(std::wostream& stream) const
{
	return stream << value;
}

///////////////////////////////// Federate<> //////////////////////////////////

template <typename TIME_TRAIT>
Federate<TIME_TRAIT>::Federate()
	: FederateBase()
	, hla_time_factory()
	, lookahead(TIME_ADAPTER::Epsilon())
	, time_stamp()
	, hla_time()
{
	hla_time = hla_time_factory.makeLogicalTime(0);
}

template <typename TIME_TRAIT>
Federate<TIME_TRAIT>::Federate(
	const std::wstring& _url,
	const std::wstring& _federation_execution_name,
	const std::wstring& _fom_module,
	const std::wstring& _federate_name,
	const std::wstring& _federate_type,
	const TIME_TYPE& _lookahead)
	: FederateBase(
		_url,
		_federation_execution_name,
		_fom_module,
		_federate_name,
		_federate_type
	)
	, hla_time_factory()
	, lookahead(TIME_ADAPTER::IsNull(_lookahead) ? TIME_ADAPTER::Epsilon() : _lookahead)
	, time_stamp()
	, hla_time()
{
	hla_time = hla_time_factory.makeLogicalTime(0);
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::SetLookahead(const TIME_TYPE& _lookahead)
{
	if(TIME_ADAPTER::IsNull(lookahead))
	{
		DebugWarningStream() << "Lookahead shall be > 0" << std::endl;
		return;
	}

	lookahead = _lookahead;
}

template <typename TIME_TRAIT>
bool Federate<TIME_TRAIT>::Initialize()
{

	rti1516e::RTIambassadorFactory rti_ambassador_factory;
	
	rti_ambassador = rti_ambassador_factory.createRTIambassador();
	
	try
	{
		if(verbose)
		{
			DebugInfoStream() << "Connecting to RTI at " << url << std::endl;
		}
		
		rti_ambassador->connect(*this, rti1516e::HLA_EVOKED, url);
		
		if(verbose)
		{
			DebugInfoStream() << "Connected" << std::endl;
		}
	}
	catch(rti1516e::Exception& e)
	{
		DebugErrorStream() << e.what() << std::endl;
		return false;
	}
	
	try
	{
		if(verbose)
		{
			DebugInfoStream() << "Creating federation execution " << federation_execution_name << " with FOM module " << fom_module << std::endl;
		}
		
		rti_ambassador->createFederationExecution(
			federation_execution_name,
			fom_module,
			TimeImplementations<SCALAR_TYPE>::HLA_TIME_IMPL_NAME
		);
		if(verbose)
		{
			DebugInfoStream() << "Created" << std::endl;
		}
	}
	catch(rti1516e::FederationExecutionAlreadyExists& e)
	{
		if(verbose)
		{
			DebugInfoStream() << "Federation \"" << federation_execution_name << "\" already exists" << std::endl;
		}
	}
	
	joined = false;
	do
	{
		try
		{
			if(verbose)
			{
				DebugInfoStream() << "Trying to join federation execution " << federation_execution_name << " as Federate " << federate_name << " with type " << federate_type << std::endl;
			}
			rti_ambassador->joinFederationExecution(
				federate_name,
				federate_type,
				federation_execution_name
			);
			joined = true;
			if(verbose)
			{
				DebugInfoStream() << "Joined" << std::endl;
			}
		}
		catch(rti1516e::Exception& e)
		{
			DebugWarningStream() << e.what() << std::endl;
			if(verbose)
			{
				DebugInfoStream() << "Retying later..." << std::endl;
			}
			WaitHostTime(1000);
		}
		if(Killed()) return false;
	}
	while(!joined);
	
	for(ObjectClasses::iterator object_class_it = object_classes.begin(); object_class_it != object_classes.end(); ++object_class_it)
	{
		rti1516e::AttributeHandleSet publish_attribute_handle_set;
		rti1516e::AttributeHandleSet subscribe_attribute_handle_set;
		
		ObjectClass& object_class = *(*object_class_it).second;
		if(debug)
		{
			DebugInfoStream() << "Getting handle of Class " << object_class.name << std::endl;
		}
		
		try
		{
			object_class.handle = rti_ambassador->getObjectClassHandle(object_class.name);
		}
		catch(rti1516e::Exception& e)
		{
			DebugErrorStream() << e.what() << std::endl;
			return false;
		}
		if(debug)
		{
			DebugInfoStream() << "Got " << object_class.handle.toString() << std::endl;
		}

		for(ObjectClass::AttributesPerName::iterator attr_it = object_class.attrs_per_name.begin(); attr_it != object_class.attrs_per_name.end(); ++attr_it)
		{
			AttributeBase& attr_base = *(*attr_it).second;
			if(debug)
			{
				DebugInfoStream() << "Getting handle of Attribute " << attr_base.name << " from Class " << object_class.name << std::endl;
			}
			try
			{
				attr_base.handle = rti_ambassador->getAttributeHandle(object_class.handle, attr_base.name);
			}
			catch(rti1516e::Exception& e)
			{
				DebugErrorStream() << e.what() << std::endl;
				return false;
			}
			object_class.attrs[attr_base.handle] = &attr_base;
			if(debug)
			{
				DebugInfoStream() << "Got " << attr_base.handle.toString() << std::endl;
			}
			
			if(attr_base.publish)
			{
				if(verbose)
				{
					DebugInfoStream() << "Publishing Attribute \"" << attr_base.name << "\" of class \"" << attr_base.object_class.name << "\"" << std::endl;
				}
				publish_attribute_handle_set.insert(attr_base.handle);
			}
			if(attr_base.subscribe)
			{
				if(verbose)
				{
					DebugInfoStream() << "Subscribing to Attribute \"" << attr_base.name << "\" of class \"" << attr_base.object_class.name << "\"" << std::endl;
				}
				subscribe_attribute_handle_set.insert(attr_base.handle);
			}
			
			if(publish_attribute_handle_set.size())
			{
				try
				{
					rti_ambassador->publishObjectClassAttributes(object_class.handle, publish_attribute_handle_set);
				}
				catch(rti1516e::Exception& e)
				{
					DebugErrorStream() << e.what() << std::endl;
					return false;
				}
			}
			
			if(subscribe_attribute_handle_set.size())
			{
				try
				{
					rti_ambassador->subscribeObjectClassAttributes(object_class.handle, subscribe_attribute_handle_set);
				}
				catch(rti1516e::Exception& e)
				{
					DebugErrorStream() << e.what() << std::endl;
					return false;
				}
			}
		}
	}
	
	for(ObjectInstancesPerName::iterator object_instance_it = object_instances_per_name.begin(); object_instance_it != object_instances_per_name.end(); ++object_instance_it)
	{
		ObjectInstance& object_instance = *(*object_instance_it).second;
		if(object_instance.object_class.HasPublishedAttributes())
		{
			if(debug)
			{
				DebugInfoStream() << "Reserving Object instance name \"" << object_instance.name << "\"" << std::endl;
			}
			try
			{
				rti_ambassador->reserveObjectInstanceName(object_instance.name);
			}
			catch(rti1516e::Exception& e)
			{
				DebugErrorStream() << e.what() << std::endl;
				return false;
			}
		}
	}
	
	if(verbose)
	{
		DebugInfoStream() << "Enabling time constrained to receive TSO messages" << std::endl;
	}
	
	try
	{
		rti_ambassador->enableTimeConstrained();
	}
	catch(rti1516e::Exception& e)
	{
		DebugErrorStream() << e.what() << std::endl;
		return false;
	}
	
	try
	{
		rti_ambassador->evokeMultipleCallbacks(0.1, std::numeric_limits<double>::infinity());
	}
	catch(rti1516e::Exception& e)
	{
		DebugErrorStream() << e.what() << std::endl;
		return false;
	}
	
	if(verbose)
	{
		DebugInfoStream() << "Enabling time regulation with a " << unisim::util::unicode::utf8_string_to_unicode_wstring(TIME_ADAPTER::ToString(lookahead)) << " lookahead to send TSO messages" << std::endl;
	}
	RTI_UNIQUE_PTR<HLA_TIME_INTERVAL> hla_time_interval = hla_time_factory.makeLogicalTimeInterval(TIME_ADAPTER::ToScalar(lookahead));
	const rti1516e::LogicalTimeInterval& theLookahead = *(rti1516e::LogicalTimeInterval *) hla_time_interval.get();
	
	try
	{
		rti_ambassador->enableTimeRegulation(theLookahead);
	}
	catch(rti1516e::Exception& e)
	{
		DebugErrorStream() << e.what() << std::endl;
		return false;
	}
	
	if(verbose)
	{
		DebugInfoStream() << "Enabling attribute scope advisory switch" << std::endl;
	}
	try
	{
		rti_ambassador->enableAttributeScopeAdvisorySwitch();
	}
	catch(rti1516e::Exception& e)
	{
		DebugErrorStream() << e.what() << std::endl;
		return false;
	}
	
	if(!time_regulation_enabled || (object_instances.size() != object_instances_per_name.size()))
	{
		if(verbose)
		{
			DebugInfoStream() << "Waiting for object instance discovery and time regulation..." << std::endl;
		}
		
		do
		{
			try
			{
				rti_ambassador->evokeMultipleCallbacks(0.1, std::numeric_limits<double>::infinity());
			}
			catch(rti1516e::Exception& e)
			{
				DebugErrorStream() << e.what() << std::endl;
				return false;
			}
			if(Killed()) return false;
		}
		while(!time_regulation_enabled || (object_instances.size() != object_instances_per_name.size()));
	}
	
	if(verbose)
	{
		DebugInfoStream() << "Ready to simulate" << std::endl;
	}
	
	return true;
}

template <typename TIME_TRAIT>
Federate<TIME_TRAIT>::~Federate()
{
	if(rti_ambassador.get() && joined)
	{
		if(verbose)
		{
			DebugInfoStream() << "Resigning from federation" << std::endl;
		}
		try
		{
			rti_ambassador->resignFederationExecution(
				rti1516e::DELETE_OBJECTS
			);
			
			if(verbose)
			{
				DebugInfoStream() << "Resigned" << std::endl;
			}
		}
		catch(rti1516e::Exception& e)
		{
			DebugWarningStream() << e.what() << std::endl;
		}
	}
}


template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::objectInstanceNameReservationSucceeded(
	std::wstring const & theObjectInstanceName)
	RTI_THROW ((
		rti1516e::FederateInternalError))
{
	try
	{
		ObjectInstancesPerName::iterator object_instance_it = object_instances_per_name.find(theObjectInstanceName);
		if(object_instance_it != object_instances_per_name.end())
		{
			ObjectInstance& object_instance = *(*object_instance_it).second;
			if(verbose)
			{
				DebugInfoStream() << "Registering Object instance \"" << object_instance.name << "\" of Class \"" << object_instance.object_class.name << "\"" << std::endl;
			}
			try
			{
				object_instance.handle = rti_ambassador->registerObjectInstance(object_instance.object_class.handle, object_instance.name);
			}
			catch(rti1516e::Exception& e)
			{
				throw Exception(std::wstring(L"Registration of Object instance \"") + object_instance.name + L"\" failed");
			}
			object_instances[object_instance.handle] = &object_instance;
			if(debug)
			{
				DebugInfoStream() << "Got handle " << object_instance.handle.toString() << std::endl;
			}
			
			for(ObjectInstance::AttributeValuesPerName::iterator attr_value_it = object_instance.attr_values_per_name.begin(); attr_value_it != object_instance.attr_values_per_name.end(); ++attr_value_it)
			{
				AttributeValueBase& attr_value_base = *(*attr_value_it).second;
				object_instance.attr_values[attr_value_base.attr_base.handle] = &attr_value_base;
			}
		}
	}
	catch(rti1516e::Exception& e)
	{
		DebugWarningStream() << e.what() << std::endl;
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::objectInstanceNameReservationFailed (
	std::wstring const & theObjectInstanceName)
	RTI_THROW ((
		rti1516e::FederateInternalError))
{
	throw Exception(std::wstring(L"Reservation of Object instance name \"") + theObjectInstanceName + L"\" failed");
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::discoverObjectInstance (
	rti1516e::ObjectInstanceHandle theObject,
	rti1516e::ObjectClassHandle theObjectClass,
	std::wstring const & theObjectInstanceName)
	RTI_THROW ((
		rti1516e::FederateInternalError))
{
	ObjectInstancesPerName::iterator object_instance_it = object_instances_per_name.find(theObjectInstanceName);
	if(object_instance_it != object_instances_per_name.end())
	{
		ObjectInstance& object_instance = *(*object_instance_it).second;
		if(verbose)
		{
			DebugInfoStream() << "Discovered Object instance \"" << theObjectInstanceName << "\" of Class \"" << object_instance.object_class.name << "\"" << std::endl;
		}
		if(debug)
		{
			DebugInfoStream() << "Got handle " << theObject << std::endl;
		}
		object_instance.handle = theObject;
		object_instances[object_instance.handle] = &object_instance;
		
		for(ObjectInstance::AttributeValuesPerName::iterator attr_value_it = object_instance.attr_values_per_name.begin(); attr_value_it != object_instance.attr_values_per_name.end(); ++attr_value_it)
		{
			AttributeValueBase& attr_value_base = *(*attr_value_it).second;
			object_instance.attr_values[attr_value_base.attr_base.handle] = &attr_value_base;
		}
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::timeRegulationEnabled (
	rti1516e::LogicalTime const & theFederateTime)
	RTI_THROW ((
		rti1516e::FederateInternalError))
{
	time_regulation_enabled = true;
	if(verbose)
	{
		DebugInfoStream() << "Time regulation enabled" << std::endl;
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::timeConstrainedEnabled (
		rti1516e::LogicalTime const & theFederateTime)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	time_constained_enabled = true;
	if(verbose)
	{
		DebugInfoStream() << "Time contrained enabled" << std::endl;
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::attributesInScope (
	rti1516e::ObjectInstanceHandle theObject,
	rti1516e::AttributeHandleSet const & theAttributes)
	RTI_THROW ((
		rti1516e::FederateInternalError))
{
	ObjectInstances::iterator object_instance_it = object_instances.find(theObject);
	if(object_instance_it != object_instances.end())
	{
		ObjectInstance& object_instance = *(*object_instance_it).second;
		for(rti1516e::AttributeHandleSet::iterator it = theAttributes.begin(); it != theAttributes.end(); ++it)
		{
			const rti1516e::AttributeHandle& attr_handle = *it;
			ObjectInstance::AttributeValues::iterator attr_value_it = object_instance.attr_values.find(attr_handle);
			if(attr_value_it != object_instance.attr_values.end())
			{
				AttributeValueBase& attr_value_base = *(*attr_value_it).second;
				if(debug)
				{
					DebugInfoStream() << "Attribute \"" << object_instance.name << "." << attr_value_base.attr_base.name << "\" in scope" << std::endl;
				}
				attr_value_base.in_scope = true;
			}
		}
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::attributesOutOfScope (
	rti1516e::ObjectInstanceHandle theObject,
	rti1516e::AttributeHandleSet const & theAttributes)
	RTI_THROW ((
		rti1516e::FederateInternalError))
{
	ObjectInstances::iterator object_instance_it = object_instances.find(theObject);
	if(object_instance_it != object_instances.end())
	{
		ObjectInstance& object_instance = *(*object_instance_it).second;
		for(rti1516e::AttributeHandleSet::iterator it = theAttributes.begin(); it != theAttributes.end(); ++it)
		{
			const rti1516e::AttributeHandle& attr_handle = *it;
			ObjectInstance::AttributeValues::iterator attr_value_it = object_instance.attr_values.find(attr_handle);
			if(attr_value_it != object_instance.attr_values.end())
			{
				AttributeValueBase& attr_value_base = *(*attr_value_it).second;
				if(debug)
				{
					DebugInfoStream() << "Attribute \"" << object_instance.name << "." << attr_value_base.attr_base.name << "\" out of scope" << std::endl;
				}
				attr_value_base.in_scope = false;
			}
		}
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::provideAttributeValueUpdate (
	rti1516e::ObjectInstanceHandle theObject,
	rti1516e::AttributeHandleSet const & theAttributes,
	rti1516e::VariableLengthData const & theUserSuppliedTag)
	RTI_THROW ((
		rti1516e::FederateInternalError))
{
	ObjectInstances::iterator object_instance_it = object_instances.find(theObject);
	if(object_instance_it != object_instances.end())
	{
		ObjectInstance& object_instance = *(*object_instance_it).second;
		object_instance.user_supplied_tag = theUserSuppliedTag;
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::reflectAttributeValues (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleValueMap const & theAttributeValues,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::TransportationType theType,
		rti1516e::LogicalTime const & theTime,
		rti1516e::OrderType receivedOrder,
		rti1516e::SupplementalReflectInfo theReflectInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	ObjectInstances::iterator object_instance_it = object_instances.find(theObject);
	if(object_instance_it != object_instances.end())
	{
		ObjectInstance& object_instance = *(*object_instance_it).second;
		for(rti1516e::AttributeHandleValueMap::const_iterator it = theAttributeValues.begin(); it != theAttributeValues.end(); ++it)
		{
			const rti1516e::AttributeHandle& attr_handle = (*it).first;
			ObjectInstance::AttributeValues::iterator attr_value_it = object_instance.attr_values.find(attr_handle);
			if(attr_value_it != object_instance.attr_values.end())
			{
				const rti1516e::VariableLengthData& attr_value = (*it).second;
				AttributeValueBase& attr_value_base = *(*attr_value_it).second;
				attr_value_base.Set(attr_value);
				if(debug)
				{
					DebugInfoStream() << object_instance.name << "." << attr_value_base.attr_base.name << " <- " << attr_value_base << std::endl;
				}
			}
		}
	}
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::timeAdvanceGrant (
		rti1516e::LogicalTime const & theTime)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	time_advance_granted = true;
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::WaitUntil(const TIME_TYPE& until)
{
	if(debug)
	{
		DebugInfoStream() << "Waiting until " << unisim::util::unicode::utf8_string_to_unicode_wstring(TIME_ADAPTER::ToString(until)) << std::endl;
	}

	rti_ambassador->evokeMultipleCallbacks(0.000001, std::numeric_limits<double>::infinity());
	
	hla_time->setTime(TIME_ADAPTER::ToScalar(until));
	const rti1516e::LogicalTime& logical_time = *(rti1516e::LogicalTime *) hla_time.get();
	
	for(ObjectInstances::iterator object_instance_it = object_instances.begin(); object_instance_it != object_instances.end(); ++object_instance_it)
	{
		rti1516e::AttributeHandleSet attribute_handle_set;
		rti1516e::AttributeHandleValueMap attribute_handle_value_map;
		
		ObjectInstance& object_instance = *(*object_instance_it).second;
		for(ObjectInstance::AttributeValues::iterator attr_value_it = object_instance.attr_values.begin(); attr_value_it != object_instance.attr_values.end(); ++attr_value_it)
		{
			AttributeValueBase& attr_value_base = *(*attr_value_it).second;
			const AttributeBase& attr_base = attr_value_base.attr_base;
			if(attr_base.subscribe)
			{
				if(debug)
				{
					DebugInfoStream() << "? " << object_instance.name << "." << attr_base.name << " at " << unisim::util::unicode::utf8_string_to_unicode_wstring(TIME_ADAPTER::ToString(until)) << std::endl;
				}
				attribute_handle_set.insert(attr_base.handle);
			}
			if(attr_base.publish && attr_value_base.in_scope)
			{
				if(debug)
				{
					DebugInfoStream() << "! " << object_instance.name << "." << attr_value_base.attr_base.name << " = " << attr_value_base << " at " << unisim::util::unicode::utf8_string_to_unicode_wstring(TIME_ADAPTER::ToString(until)) << std::endl;
				}
				attr_value_base.Get(attribute_handle_value_map[attr_value_base.attr_base.handle]);
				attribute_handle_set.insert(attr_base.handle);
			}
		}
		
		if(attribute_handle_set.size())
		{
			try
			{
				rti_ambassador->requestAttributeValueUpdate(object_instance.handle, attribute_handle_set, rti1516e::VariableLengthData());
			}
			catch(rti1516e::Exception& e)
			{
				DebugErrorStream() << e.what() << std::endl;
				throw Exception(e.what());
			}
		}
		
		if(attribute_handle_value_map.size())
		{
			try
			{
				rti_ambassador->updateAttributeValues(
					object_instance.handle,
					attribute_handle_value_map,
					object_instance.user_supplied_tag,
					logical_time
				);
			}
			catch(rti1516e::Exception& e)
			{
				throw Exception(e.what());
			}
			
			object_instance.user_supplied_tag = rti1516e::VariableLengthData();
		}
	}
	
	if(debug)
	{
		DebugInfoStream() << "Requesting time advance to " << unisim::util::unicode::utf8_string_to_unicode_wstring(TIME_ADAPTER::ToString(until)) << std::endl;
	}
	
	time_advance_granted = false;
	try
	{
		rti_ambassador->timeAdvanceRequest(logical_time);
	}
	catch(rti1516e::Exception& e)
	{
		throw Exception(e.what());
	}
	do
	{
		try
		{
			rti_ambassador->evokeMultipleCallbacks(0.000001, std::numeric_limits<double>::infinity());
		}
		catch(rti1516e::Exception& e)
		{
			throw Exception(e.what());
		}
		if(Killed()) return;
	}
	while(!time_advance_granted);
	if(debug)
	{
		DebugInfoStream() << "Granted time " << unisim::util::unicode::utf8_string_to_unicode_wstring(TIME_ADAPTER::ToString(until)) << std::endl;
	}
	time_stamp = until;
}

template <typename TIME_TRAIT>
void Federate<TIME_TRAIT>::Wait(const TIME_TYPE& delay)
{
	TIME_TYPE until(time_stamp);
	until += delay;
	WaitUntil(until);
}

template <typename TIME_TRAIT>
const typename TIME_TRAIT::TIME_TYPE& Federate<TIME_TRAIT>::TimeStamp() const
{
	return time_stamp;
}

} // end of namespace hla
} // end of namespace util
} // end of namespace unisim

#endif // HAVE_HLA_RTI1516E

#endif // __UNISIM_UTIL_HLA_HLA_TCC__
