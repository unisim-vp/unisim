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

#include <unisim/util/hla/hla.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#else
#include <time.h>
#endif

namespace unisim {
namespace util {
namespace hla {

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

/////////////////////////// TimeImplementations<> /////////////////////////////

const wchar_t *TimeImplementations<uint64_t>::HLA_TIME_IMPL_NAME = L"HLAinteger64Time";
const wchar_t *TimeImplementations<double>::HLA_TIME_IMPL_NAME = L"HLAfloat64Time";

//////////////////////////////// ObjectClass //////////////////////////////////

ObjectClass::ObjectClass(FederateBase& _federate, const std::string& _name)
	: handle()
	, federate(_federate)
	, name(unisim::util::unicode::utf8_string_to_unicode_wstring(_name))
{
	federate.object_classes[name] = this;
}

ObjectClass::ObjectClass(FederateBase& _federate, const std::wstring& _name)
	: handle()
	, federate(_federate)
	, name(_name)
{
	federate.object_classes[name] = this;
}

ObjectClass::~ObjectClass()
{
}

const std::wstring& ObjectClass::GetName() const
{
	return name;
}

const AttributeBase& ObjectClass::GetAttribute(const std::string& attr_name) const
{
	return GetAttribute(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

const AttributeBase& ObjectClass::GetAttribute(const std::wstring& attr_name) const
{
	AttributesPerName::const_iterator it = attrs_per_name.find(attr_name);
	if(it == attrs_per_name.end()) throw UndefinedAttributeError(name);
	const AttributeBase *attr_base = (*it).second;
	return *attr_base;
}

AttributeBase& ObjectClass::GetAttribute(const std::string& attr_name)
{
	return GetAttribute(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

AttributeBase& ObjectClass::GetAttribute(const std::wstring& attr_name)
{
	AttributesPerName::iterator it = attrs_per_name.find(attr_name);
	if(it == attrs_per_name.end()) throw UndefinedAttributeError(attr_name);
	AttributeBase *attr_base = (*it).second;
	return *attr_base;
}

std::string ObjectClass::GetAttributeTypeName(const std::string& attr_name) const
{
	return unisim::util::unicode::unicode_wstring_to_utf8_string(GetAttributeTypeName(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name)));
}

const std::wstring& ObjectClass::GetAttributeTypeName(const std::wstring& attr_name) const
{
	static std::wstring void_type_name;
	AttributesPerName::const_iterator it = attrs_per_name.find(attr_name);
	if(it != attrs_per_name.end())
	{
		const AttributeBase& attr_base = *(*it).second;
		return attr_base.GetTypeName();
	}
	return void_type_name;
}

bool ObjectClass::HasPublishedAttributes() const
{
	for(AttributesPerName::const_iterator it = attrs_per_name.begin(); it != attrs_per_name.end(); ++it)
	{
		const AttributeBase& attr_base = *(*it).second;
		if(attr_base.publish) return true;
	}
	return false;
}

void ObjectClass::DestroyAttributes()
{
	for(AttributesPerName::iterator it = attrs_per_name.begin(); it != attrs_per_name.end(); ++it)
	{
		AttributeBase *attr_base = (*it).second;
		delete attr_base;
	}
}

////////////////////////////// ObjectInstance /////////////////////////////////

ObjectInstance::ObjectInstance(ObjectClass& _object_class, const std::string& _name)
	: handle()
	, object_class(_object_class)
	, name(unisim::util::unicode::utf8_string_to_unicode_wstring(_name))
	, attr_values()
	, attr_values_per_name()
	, user_supplied_tag()
{
	if(object_class.object_instances.find(name) != object_class.object_instances.end()) throw ObjectInstanceAlreadyExists(name);
	object_class.object_instances[name] = this;
	if(object_class.federate.object_instances_per_name.find(name) != object_class.federate.object_instances_per_name.end()) throw ObjectInstanceAlreadyExists(name);
	object_class.federate.object_instances_per_name[name] = this;
	for(ObjectClass::AttributesPerName::const_iterator it = object_class.attrs_per_name.begin(); it != object_class.attrs_per_name.end(); ++it)
	{
		attr_values_per_name[(*it).first] = (*it).second->CreateAttributeValue(*this);
	}
}

ObjectInstance::ObjectInstance(ObjectClass& _object_class, const std::wstring& _name)
	: handle()
	, object_class(_object_class)
	, name(_name)
	, attr_values()
	, attr_values_per_name()
	, user_supplied_tag()
{
	if(object_class.object_instances.find(name) != object_class.object_instances.end()) throw ObjectInstanceAlreadyExists(name);
	object_class.object_instances[name] = this;
	if(object_class.federate.object_instances_per_name.find(name) != object_class.federate.object_instances_per_name.end()) throw ObjectInstanceAlreadyExists(name);
	object_class.federate.object_instances_per_name[name] = this;
	for(ObjectClass::AttributesPerName::const_iterator it = object_class.attrs_per_name.begin(); it != object_class.attrs_per_name.end(); ++it)
	{
		attr_values_per_name[(*it).first] = (*it).second->CreateAttributeValue(*this);
	}
}

ObjectInstance::~ObjectInstance()
{
	for(AttributeValuesPerName::iterator it = attr_values_per_name.begin(); it != attr_values_per_name.end(); ++it)
	{
		delete (*it).second;
	}
}

const std::wstring& ObjectInstance::GetName() const
{
	return name;
}

const AttributeValueBase& ObjectInstance::GetAttributeValue(const std::string& attr_name) const
{
	return GetAttributeValue(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

const AttributeValueBase& ObjectInstance::GetAttributeValue(const std::wstring& attr_name) const
{
	AttributeValuesPerName::const_iterator it = attr_values_per_name.find(attr_name);
	if(it == attr_values_per_name.end()) throw UndefinedAttributeError(attr_name);
	const AttributeValueBase *attr_value_base = (*it).second;
	return *attr_value_base;
}

AttributeValueBase& ObjectInstance::GetAttributeValue(const std::string& attr_name)
{
	return GetAttributeValue(unisim::util::unicode::utf8_string_to_unicode_wstring(attr_name));
}

AttributeValueBase& ObjectInstance::GetAttributeValue(const std::wstring& attr_name)
{
	AttributeValuesPerName::iterator it = attr_values_per_name.find(attr_name);
	if(it == attr_values_per_name.end()) throw UndefinedAttributeError(attr_name);
	AttributeValueBase *attr_value_base = (*it).second;
	return *attr_value_base;
}

/////////////////////////////// AttributeBase /////////////////////////////////

AttributeBase::AttributeBase(ObjectClass& _object_class, const std::string& _name, const std::string& _type_name)
	: handle()
	, object_class(_object_class)
	, name(unisim::util::unicode::utf8_string_to_unicode_wstring(_name))
	, type_name(unisim::util::unicode::utf8_string_to_unicode_wstring(_type_name))
	, publish(false)
	, subscribe(false)
{
	if(object_class.attrs_per_name.find(name) != object_class.attrs_per_name.end()) throw AttributeAlreadyExists(_name);
	object_class.attrs_per_name[name] = this;
}

AttributeBase::AttributeBase(ObjectClass& _object_class, const std::wstring& _name, const std::wstring& _type_name)
	: handle()
	, object_class(_object_class)
	, name(_name)
	, type_name(_type_name)
	, publish(false)
	, subscribe(false)
{
	if(object_class.attrs_per_name.find(name) != object_class.attrs_per_name.end()) throw AttributeAlreadyExists(_name);
	object_class.attrs_per_name[name] = this;
}

AttributeBase::~AttributeBase()
{
}

AttributeBase& AttributeBase::Publish()
{
	publish = true;
	return *this;
}

AttributeBase& AttributeBase::Subscribe()
{
	subscribe = true;
	return *this;
}

const std::wstring& AttributeBase::GetName() const
{
	return name;
}

const std::wstring& AttributeBase::GetTypeName() const
{
	return type_name;
}

bool AttributeBase::IsPublished() const
{
	return publish;
}

bool AttributeBase::IsSubscribed() const
{
	return subscribe;
}

/////////////////////////// AttributeValueBase ////////////////////////////////

unsigned int AttributeValueBase::next_id = 0;

AttributeValueBase::AttributeValueBase(const ObjectInstance& _object_instance, const AttributeBase& _attr_base)
	: id(next_id++)
	, object_instance(_object_instance)
	, attr_base(_attr_base)
	, in_scope(false)
{
}

AttributeValueBase::~AttributeValueBase()
{
}

const ObjectInstance& AttributeValueBase::GetObjectInstance() const
{
	return object_instance;
}

const AttributeBase& AttributeValueBase::GetAttribute() const
{
	return attr_base;
}

std::wostream& operator << (std::wostream& stream, const AttributeValueBase& attr_value_base)
{
	return attr_value_base.Print(stream);
}

//////////////////////////////// FederateBase /////////////////////////////////

FederateBase::FederateBase()
	: verbose(false)
	, debug(false)
	, federation_execution_name()
	, fom_module()
	, federate_name()
	, federate_type()
	, rti_ambassador()
	, object_classes()
	, object_instances_per_name()
	, joined()
	, time_advance_granted(false)
	, time_regulation_enabled(false)
	, time_constained_enabled(false)
	, killed(false)
	, debug_info_stream(&std::wcout)
	, debug_warning_stream(&std::wcerr)
	, debug_error_stream(&std::wcerr)
{
}

FederateBase::FederateBase(
	const std::wstring& _url,
	const std::wstring& _federation_execution_name,
	const std::wstring& _fom_module,
	const std::wstring& _federate_name,
	const std::wstring& _federate_type)
	: verbose(false)
	, debug(false)
	, federation_execution_name(_federation_execution_name)
	, fom_module(_fom_module)
	, federate_name(_federate_name)
	, federate_type(_federate_type)
	, rti_ambassador()
	, object_classes()
	, object_instances_per_name()
	, joined()
	, time_advance_granted(false)
	, time_regulation_enabled(false)
	, time_constained_enabled(false)
	, killed(false)
	, debug_info_stream(&std::wcout)
	, debug_warning_stream(&std::wcerr)
	, debug_error_stream(&std::wcerr)
{
}

FederateBase::~FederateBase()
{
}

void FederateBase::SetVerbose(bool _verbose)
{
	verbose = _verbose;
}

void FederateBase::SetDebug(bool _debug)
{
	debug = _debug;
}

void FederateBase::SetURL(const std::wstring& _url)
{
	url = _url;
}

void FederateBase::SetFederationExecutionName(const std::wstring& _federation_execution_name)
{
	federation_execution_name = _federation_execution_name;
}

void FederateBase::SetFOMModule(const std::wstring& _fom_module)
{
	fom_module = _fom_module;
}

void FederateBase::SetFederateName(const std::wstring& _federate_name)
{
	federate_name = _federate_name;
}

void FederateBase::SetFederateType(const std::wstring& _federate_type)
{
	federate_type = _federate_type;
}

void FederateBase::SetDebugInfoStream(std::wostream& stream)
{
	debug_info_stream = &stream;
}

void FederateBase::SetDebugWarningStream(std::wostream& stream)
{
	debug_warning_stream = &stream;
}

void FederateBase::SetDebugErrorStream(std::wostream& stream)
{
	debug_error_stream = &stream;
}

std::wostream& FederateBase::DebugInfoStream() const
{
	return *debug_info_stream;
}

std::wostream& FederateBase::DebugWarningStream() const
{
	return *debug_warning_stream;
}

std::wostream& FederateBase::DebugErrorStream() const
{
	return *debug_error_stream;
}

bool FederateBase::Killed() const
{
	return killed;
}

void FederateBase::Kill()
{
	killed = true;
}

void FederateBase::WaitHostTime(unsigned int msec)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	Sleep(msec);
#else
	struct timespec tim_req, tim_rem;
	tim_req.tv_sec = msec / 1000;
	tim_req.tv_nsec = 1000000 * (msec % 1000);
	
	while(1)
	{
		int status = nanosleep(&tim_req, &tim_rem);
		
		if(status == 0) break;
		
		if(status != -1) break;
		
		if(errno != EINTR) break;

		tim_req.tv_nsec = tim_rem.tv_nsec;
	}
#endif
}

} // end of namespace hla
} // end of namespace util
} // end of namespace unisim
