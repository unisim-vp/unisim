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

#ifndef __UNISIM_UTIL_HLA_HLA_HH__
#define __UNISIM_UTIL_HLA_HLA_HH__

#include <unisim/util/unicode/unicode.hh>

#include <memory> // Note: missing include in RTI headers for std::auto_ptr!
#include <RTI/RTIambassador.h>
#include <RTI/FederateAmbassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTIambassadorFactory.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <RTI/time/HLAinteger64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>
#include <RTI/time/HLAfloat64Time.h>

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace unisim {
namespace util {
namespace hla {

///////////////////////////////////////////////////////////////////////////////
//                          Forward declarations                             //
///////////////////////////////////////////////////////////////////////////////

struct Exception;
struct ObjectClass;
struct ObjectInstance;
struct AttributeBase;
template <typename T> struct Attribute;
struct AttributeValueBase;
template <typename T> struct AttributeValue;
struct FederateBase;
template <typename TIME_TRAIT> struct Federate;

///////////////////////////////////////////////////////////////////////////////
//                               Exceptions                                  //
///////////////////////////////////////////////////////////////////////////////

struct Exception : std::runtime_error
{
	Exception(const std::string& msg) : std::runtime_error(msg.c_str()) {}
	Exception(const std::wstring& msg) : std::runtime_error(unisim::util::unicode::unicode_wstring_to_utf8_string(msg).c_str()) {}
};

struct UndefinedAttributeError : Exception
{
	UndefinedAttributeError(const std::wstring& attr_name) : Exception(std::wstring(L"Undefined attribute ") + attr_name) {}
	UndefinedAttributeError(const rti1516e::AttributeHandle& attr_handle) : Exception(std::wstring(L"Undefined attribute with handle ") + attr_handle.toString()) {}
};

struct AttributeTypeError : Exception
{
	AttributeTypeError(const std::wstring& attr_name) : Exception(std::wstring(L"Type error for attribute ") + attr_name) {}
	AttributeTypeError(const rti1516e::AttributeHandle& attr_handle) : Exception(std::wstring(L"Type error for attribute with handle ") + attr_handle.toString()) {}
};

struct InvalidWaitDelay : Exception
{
	InvalidWaitDelay() : Exception(L"Invalid wait delay (delay shall be > lookahead") {}
};

struct ObjectClassAlreadyExists : Exception
{
	ObjectClassAlreadyExists(const std::string& object_class_name) : Exception(std::string("Object class ") + object_class_name + " already exists") {}
	ObjectClassAlreadyExists(const std::wstring& object_class_name) : Exception(std::wstring(L"Object class ") + object_class_name + L" already exists") {}
};

struct AttributeAlreadyExists : Exception
{
	AttributeAlreadyExists(const std::string& attr_name) : Exception(std::string("Attribute ") + attr_name + " already exists") {}
	AttributeAlreadyExists(const std::wstring& attr_name) : Exception(std::wstring(L"Attribute ") + attr_name + L" already exists") {}
};

struct ObjectInstanceAlreadyExists : Exception
{
	ObjectInstanceAlreadyExists(const std::string& object_instance_name) : Exception(std::string("Object instance ") + object_instance_name + " already exists") {}
	ObjectInstanceAlreadyExists(const std::wstring& object_instance_name) : Exception(std::wstring(L"Object instance ") + object_instance_name + L" already exists") {}
};

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// ObjectClass //////////////////////////////////

struct ObjectClass
{
	ObjectClass(FederateBase& federate, const std::string& name);
	ObjectClass(FederateBase& federate, const std::wstring& name);
	virtual ~ObjectClass();
	const std::wstring& GetName() const;
	const AttributeBase& GetAttribute(const std::string& attr_name) const;
	const AttributeBase& GetAttribute(const std::wstring& attr_name) const;
	AttributeBase& GetAttribute(const std::string& attr_name);
	AttributeBase& GetAttribute(const std::wstring& attr_name);
	std::string GetAttributeTypeName(const std::string& attr_name) const;
	const std::wstring& GetAttributeTypeName(const std::wstring& attr_name) const;
	template <typename T> const Attribute<T>& GetAttribute(const std::string& attr_name) const;
	template <typename T> const Attribute<T>& GetAttribute(const std::wstring& attr_name) const;
	template <typename T> Attribute<T>& GetAttribute(const std::string& attr_name);
	template <typename T> Attribute<T>& GetAttribute(const std::wstring& attr_name);
	template <typename T> const Attribute<T>& GetAttribute(const rti1516e::AttributeHandle& attr_handle) const;
	template <typename T> Attribute<T>& GetAttribute(const rti1516e::AttributeHandle& attr_handle);
	bool HasPublishedAttributes() const;
protected:
	template <typename TIME_TRAIT> friend struct Federate;
	friend struct AttributeBase;
	friend struct ObjectInstance;
	rti1516e::ObjectClassHandle handle;
	FederateBase& federate;
	std::wstring name;
	typedef std::map<rti1516e::AttributeHandle, AttributeBase *> Attributes;
	Attributes attrs;
	typedef std::map<std::wstring, AttributeBase *> AttributesPerName;
	AttributesPerName attrs_per_name;
	typedef std::map<std::wstring, ObjectInstance *> ObjectInstances;
	ObjectInstances object_instances;
	
	void DestroyAttributes();
};

////////////////////////////// ObjectInstance /////////////////////////////////

struct ObjectInstance
{
	ObjectInstance(ObjectClass& object_class, const std::string& name);
	ObjectInstance(ObjectClass& object_class, const std::wstring& name);
	virtual ~ObjectInstance();
	const std::wstring& GetName() const;
	const AttributeValueBase& GetAttributeValue(const std::string& attr_name) const;
	const AttributeValueBase& GetAttributeValue(const std::wstring& attr_name) const;
	AttributeValueBase& GetAttributeValue(const std::string& attr_name);
	AttributeValueBase& GetAttributeValue(const std::wstring& attr_name);
	template <typename T> const AttributeValue<T>& GetAttributeValue(const std::string& attr_name) const;
	template <typename T> const AttributeValue<T>& GetAttributeValue(const std::wstring& attr_name) const;
	template <typename T> AttributeValue<T>& GetAttributeValue(const std::string& attr_name);
	template <typename T> AttributeValue<T>& GetAttributeValue(const std::wstring& attr_name);
	template <typename T> const AttributeValue<T>& GetAttributeValue(const rti1516e::AttributeHandle& attr_handle) const;
	template <typename T> AttributeValue<T>& GetAttributeValue(const rti1516e::AttributeHandle& attr_handle);
protected:
	template <typename TIME_TRAIT> friend struct Federate;
	
	rti1516e::ObjectInstanceHandle handle;
	ObjectClass& object_class;
	std::wstring name;
	typedef std::map<rti1516e::AttributeHandle, AttributeValueBase *> AttributeValues;
	AttributeValues attr_values;
	typedef std::map<std::wstring, AttributeValueBase *> AttributeValuesPerName;
	AttributeValuesPerName attr_values_per_name;
	rti1516e::VariableLengthData user_supplied_tag;
};

/////////////////////////////// AttributeBase /////////////////////////////////

struct AttributeBase
{
	AttributeBase(ObjectClass& object_class, const std::string& name, const std::string& type_name = std::string());
	AttributeBase(ObjectClass& object_class, const std::wstring& name, const std::wstring& type_name = std::wstring());
	virtual ~AttributeBase();
	AttributeBase& Publish();
	AttributeBase& Subscribe();
	const std::wstring& GetName() const;
	const std::wstring& GetTypeName() const;
	bool IsPublished() const;
	bool IsSubscribed() const;
protected:
	template <typename TIME_TRAIT> friend struct Federate;
	friend struct ObjectClass;
	friend struct ObjectInstance;
	
	virtual AttributeValueBase *CreateAttributeValue(const ObjectInstance& object_instance) = 0;
	
	rti1516e::AttributeHandle handle;
	ObjectClass& object_class;
	std::wstring name;
	std::wstring type_name;
	bool publish;
	bool subscribe;
};

//////////////////////////////// Attribute<> //////////////////////////////////

template <typename T>
struct Attribute : AttributeBase
{
	Attribute(ObjectClass& object_class, const std::string& name, const std::string& type_name = std::string());
	Attribute(ObjectClass& object_class, const std::wstring& name, const std::wstring& type_name = std::wstring());
	const AttributeValue<T>& Value(const ObjectInstance& object_instance) const;
	AttributeValue<T>& Value(ObjectInstance& object_instance);
protected:
	template <typename TIME_TRAIT> friend struct Federate;
	
	virtual AttributeValueBase *CreateAttributeValue(const ObjectInstance& object_instance);
};

/////////////////////////// AttributeValueBase ////////////////////////////////

struct AttributeValueBase
{
	static unsigned int next_id;
	
	AttributeValueBase(const ObjectInstance& object_instance, const AttributeBase& attr_base);
	virtual ~AttributeValueBase();
	
	virtual void Get(rti1516e::VariableLengthData& value) = 0;
	virtual void Set(const rti1516e::VariableLengthData& value) = 0;
	virtual std::wostream& Print(std::wostream& stream) const = 0;
	
	const ObjectInstance& GetObjectInstance() const;
	const AttributeBase& GetAttribute() const;
	
	friend std::wostream& operator << (std::wostream& stream, const AttributeValueBase& attr_value_base);
protected:
	template <typename TIME_TRAIT> friend struct Federate;
	
	unsigned int id;
	const ObjectInstance& object_instance;
	const AttributeBase& attr_base;
	bool in_scope;
};

//////////////////////////// AttributeValue<> /////////////////////////////////

template <typename T>
struct AttributeValue : AttributeValueBase
{
	AttributeValue(const ObjectInstance& object_instance, const Attribute<T>& attr, const T& _value = T());
	
	AttributeValue<T>& operator = (const T& value);
	operator const T& () const;
	
	virtual void Get(rti1516e::VariableLengthData& value);
	virtual void Set(const rti1516e::VariableLengthData& value);
	virtual std::wostream& Print(std::wostream& stream) const;
protected:
	template <typename TIME_TRAIT> friend struct Federate;
	const Attribute<T>& attr;
	T value;
};

////////////////////////////// TimeAdapters<> /////////////////////////////////

template <typename TIME_TYPE, typename SCALAR_TYPE>
struct TimeAdapters
{
	static SCALAR_TYPE ToScalar(const TIME_TYPE& time) { return SCALAR_TYPE(time); }
	static bool IsNull(const TIME_TYPE& time) { return SCALAR_TYPE(time) == 0; }
	static TIME_TYPE Epsilon() { return SCALAR_TYPE(1); }
	static std::string ToString(const TIME_TYPE& time) { std::ostringstream sstr; sstr << time; return sstr.str(); }
	static TIME_TYPE FromString(const std::string& str) { std::istringstream sstr(str); TIME_TYPE t; sstr >> t; return t; }
};

/////////////////////////// TimeImplementations<> /////////////////////////////

template <typename SCALAR_TYPE>
struct TimeImplementations
{
};

template <>
struct TimeImplementations<uint64_t>
{
	typedef rti1516e::HLAinteger64TimeFactory HLA_TIME_FACTORY;
	typedef rti1516e::HLAinteger64Time HLA_TIME;
	typedef rti1516e::HLAinteger64Interval HLA_TIME_INTERVAL;
	static const wchar_t *HLA_TIME_IMPL_NAME;
};

template <>
struct TimeImplementations<double>
{
	typedef rti1516e::HLAfloat64TimeFactory HLA_TIME_FACTORY;
	typedef rti1516e::HLAfloat64Time HLA_TIME;
	typedef rti1516e::HLAfloat64Interval HLA_TIME_INTERVAL;
	static const wchar_t *HLA_TIME_IMPL_NAME;
};

//////////////////////////////// FederateBase /////////////////////////////////

struct FederateBase : rti1516e::NullFederateAmbassador
{
	FederateBase();
	FederateBase(
		const std::wstring& url,
		const std::wstring& federation_execution_name,
		const std::wstring& fom_module,
		const std::wstring& federate_name,
		const std::wstring& federate_type);
	virtual ~FederateBase();
	
	void SetVerbose(bool verbose = true);
	void SetDebug(bool debug = true);
	void SetURL(const std::wstring& url);
	void SetFederationExecutionName(const std::wstring& federation_execution_name);
	void SetFOMModule(const std::wstring& fom_module);
	void SetFederateName(const std::wstring& federate_name);
	void SetFederateType(const std::wstring& federate_type);
	void SetDebugInfoStream(std::wostream& stream);
	void SetDebugWarningStream(std::wostream& stream);
	void SetDebugErrorStream(std::wostream& stream);
	std::wostream& DebugInfoStream() const;
	std::wostream& DebugWarningStream() const;
	std::wostream& DebugErrorStream() const;
	bool Killed() const;
	void Kill();
protected:
	friend struct ObjectClass;
	friend struct ObjectInstance;
	
	bool verbose;
	bool debug;
	std::wstring url;
	std::wstring federation_execution_name;
	std::wstring fom_module;
	std::wstring federate_name;
	std::wstring federate_type;
	RTI_UNIQUE_PTR<rti1516e::RTIambassador> rti_ambassador;
	typedef std::map<std::wstring, ObjectClass *> ObjectClasses;
	ObjectClasses object_classes;
	typedef std::map<rti1516e::ObjectInstanceHandle, ObjectInstance *> ObjectInstances;
	ObjectInstances object_instances;
	typedef std::map<std::wstring, ObjectInstance *> ObjectInstancesPerName;
	ObjectInstancesPerName object_instances_per_name;
	bool joined;
	bool time_advance_granted;
	bool time_regulation_enabled;
	bool time_constained_enabled;
	bool killed;
	std::wostream *debug_info_stream;
	std::wostream *debug_warning_stream;
	std::wostream *debug_error_stream;
	
	void WaitHostTime(unsigned int msec);
};

///////////////////////////////// Federate<> //////////////////////////////////

template <typename TIME_TRAIT>
struct Federate : FederateBase
{
	typedef typename TIME_TRAIT::TIME_TYPE TIME_TYPE;
	typedef typename TIME_TRAIT::SCALAR_TYPE SCALAR_TYPE;
	typedef typename TimeImplementations<SCALAR_TYPE>::HLA_TIME_FACTORY HLA_TIME_FACTORY;
	typedef typename TimeImplementations<SCALAR_TYPE>::HLA_TIME HLA_TIME;
	typedef typename TimeImplementations<SCALAR_TYPE>::HLA_TIME_INTERVAL HLA_TIME_INTERVAL;
	typedef TimeAdapters<TIME_TYPE, SCALAR_TYPE> TIME_ADAPTER;
	
	Federate();
	Federate(
		const std::wstring& url,
		const std::wstring& federation_execution_name,
		const std::wstring& fom_module,
		const std::wstring& federate_name,
		const std::wstring& federate_type,
		const TIME_TYPE& lookahead);
	virtual ~Federate();
	void SetLookahead(const TIME_TYPE& lookahead);
	bool Initialize();
	void WaitUntil(const TIME_TYPE& until);
	void Wait(const TIME_TYPE& delay);
	const TIME_TYPE& TimeStamp() const;
private:
	HLA_TIME_FACTORY hla_time_factory;
	TIME_TYPE lookahead;
	TIME_TYPE time_stamp;
	RTI_UNIQUE_PTR<HLA_TIME> hla_time;
	
	virtual void objectInstanceNameReservationSucceeded(
		std::wstring const & theObjectInstanceName)
		RTI_THROW ((
			rti1516e::FederateInternalError));

	virtual void objectInstanceNameReservationFailed (
		std::wstring const & theObjectInstanceName)
		RTI_THROW ((
			rti1516e::FederateInternalError));
		
	virtual void discoverObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::ObjectClassHandle theObjectClass,
		std::wstring const & theObjectInstanceName)
		RTI_THROW ((
			rti1516e::FederateInternalError));
		
	virtual void timeRegulationEnabled (
		rti1516e::LogicalTime const & theFederateTime)
		RTI_THROW ((
			rti1516e::FederateInternalError));
	
	virtual void timeConstrainedEnabled (
		rti1516e::LogicalTime const & theFederateTime)
		RTI_THROW ((
			rti1516e::FederateInternalError));
	
	virtual void attributesInScope (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError));
	
	virtual void attributesOutOfScope (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError));
	
	virtual void provideAttributeValueUpdate (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes,
		rti1516e::VariableLengthData const & theUserSuppliedTag)
		RTI_THROW ((
			rti1516e::FederateInternalError));
	
	virtual void reflectAttributeValues (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleValueMap const & theAttributeValues,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::TransportationType theType,
		rti1516e::LogicalTime const & theTime,
		rti1516e::OrderType receivedOrder,
		rti1516e::SupplementalReflectInfo theReflectInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError));
	
	virtual void timeAdvanceGrant (
		rti1516e::LogicalTime const & theTime)
		RTI_THROW ((
			rti1516e::FederateInternalError));
};

} // end of namespace hla
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_HLA_HLA_HH__
