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

#ifndef __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_HH__
#define __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_HH__

#include <memory>
#include <RTI/RTIambassador.h>
#include <RTI/FederateAmbassador.h>
#include <RTI/time/HLAfloat64TimeFactory.h>
#include <RTI/time/HLAfloat64Time.h>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>
#include <unisim/service/interfaces/stack_unwinding.hh>
#include <unisim/service/interfaces/stubbing.hh>
#include <unisim/service/interfaces/hooking.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/debug/source_code_breakpoint.hh>
#include <unisim/util/ieee754/ieee754.hh>
#include <unisim/util/json/json.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <string>
#include <set>
#include <map>
#include <systemc>

namespace unisim {
namespace service {
namespace debug {
namespace hla {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename ADDRESS> class Federate;
class Instrument;
class Attribute;
class AttributeValue;
class ObjectInstance;
class ObjectClass;
class Hook;
class Stub;
class Registry;
template <typename ADDRESS> class PublisherSubscriber;
template <typename ADDRESS> class ObjectInstanceReserver;
template <typename ADDRESS> class ObjectInstanceRegisterer;
template <typename ADDRESS> class ObjectInstanceDiscoverer;
template <typename ADDRESS> struct ObjectInstancesDependencyChecker;
class FederateHookStubBase;
template <typename ADDRESS> class FederateHookStub;
template <typename ADDRESS> class FederateHook;
template <typename ADDRESS> class FederateStub;

class Registry
{
public:
	Registry();
	
	bool Initialize(const unisim::util::json::JSON_Object& config, unisim::kernel::logger::Logger& logger);
	virtual ~Registry();
	bool AddObjectClass(ObjectClass *object_class);
	bool AddHook(Hook *hook);
	bool AddStub(Stub *stub);
	ObjectClass *GetObjectClass(const std::wstring& name) const;
	Hook *GetHook(const std::string& name) const;
	Stub *GetStub(const std::string& name) const;
	
	template <typename VISITOR> void ScanObjectClasses(VISITOR& visitor) const;
	template <typename VISITOR> void ScanHooks(VISITOR& visitor) const;
	template <typename VISITOR> void ScanStubs(VISITOR& visitor) const;
private:
	typedef std::map<std::wstring, ObjectClass *> ObjectClasses;
	ObjectClasses object_classes;
	typedef std::map<std::string, Hook *> Hooks;
	Hooks hooks;
	typedef std::map<std::string, Stub *> Stubs;
	Stubs stubs;
};

template <typename ADDRESS>
class Federate
	: public rti1516e::FederateAmbassador
	, public unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
	, public unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugSelecting>
	, public unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>
	, public unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Registers>
	, public unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>
	, public unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::StackUnwinding>
	, public unisim::kernel::Client<unisim::service::interfaces::Stubbing<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Hooking<ADDRESS> >
{
public:
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                debug_yielding_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_export;
	
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>         debug_yielding_request_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugSelecting>               debug_selecting_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >  debug_event_trigger_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >        disasm_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >             memory_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>                    registers_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >  symbol_table_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StatementLookup<ADDRESS> >    stmt_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::BackTrace<ADDRESS> >          backtrace_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Profiling<ADDRESS> >          profiling_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugInfoLoading>             debug_info_loading_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >   data_object_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >   subprogram_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StackUnwinding>               stack_unwinding_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Stubbing<ADDRESS> >           stubbing_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Hooking<ADDRESS> >            hooking_import;
	
	Federate(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Federate();

	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();
	
	// unisim::service::interfaces::DebugEventListener<ADDRESS>
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);

	// unisim::kernel::Object
	virtual bool BeginSetup();
	virtual bool EndSetup();
	
	// rti1516e::FederateAmbassador

	// 4.4
	virtual void connectionLost (
			std::wstring const & faultDescription)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.8
	virtual void reportFederationExecutions (
			rti1516e::FederationExecutionInformationVector const &
			theFederationExecutionInformationList)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.12
	virtual void synchronizationPointRegistrationSucceeded (
			std::wstring const & label)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void synchronizationPointRegistrationFailed (
			std::wstring const & label,
			rti1516e::SynchronizationPointFailureReason reason)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.13
	virtual void announceSynchronizationPoint (
			std::wstring  const & label,
			rti1516e::VariableLengthData const & theUserSuppliedTag)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.15
	virtual void federationSynchronized (
			std::wstring const & label,
			rti1516e::FederateHandleSet const& failedToSyncSet)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.17
	virtual void initiateFederateSave (
			std::wstring const & label)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void initiateFederateSave (
			std::wstring const & label,
			rti1516e::LogicalTime const & theTime)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.20
	virtual void federationSaved ()
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void federationNotSaved (
			rti1516e::SaveFailureReason theSaveFailureReason)
			RTI_THROW ((
				rti1516e::FederateInternalError));


	// 4.23
	virtual void federationSaveStatusResponse (
			rti1516e::FederateHandleSaveStatusPairVector const &
			theFederateStatusVector)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.25
	virtual void requestFederationRestoreSucceeded (
			std::wstring const & label)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void requestFederationRestoreFailed (
			std::wstring const & label)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.26
	virtual void federationRestoreBegun ()
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.27
	virtual void initiateFederateRestore (
			std::wstring const & label,
			std::wstring const & federateName,
			rti1516e::FederateHandle handle)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.29
	virtual void federationRestored ()
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void federationNotRestored (
			rti1516e::RestoreFailureReason theRestoreFailureReason)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 4.32
	virtual void federationRestoreStatusResponse (
			rti1516e::FederateRestoreStatusVector const &
			theFederateRestoreStatusVector)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	/////////////////////////////////////
	// Declaration Management Services //
	/////////////////////////////////////

	// 5.10
	virtual void startRegistrationForObjectClass (
			rti1516e::ObjectClassHandle theClass)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 5.11
	virtual void stopRegistrationForObjectClass (
			rti1516e::ObjectClassHandle theClass)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 5.12
	virtual void turnInteractionsOn (
			rti1516e::InteractionClassHandle theHandle)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 5.13
	virtual void turnInteractionsOff (
			rti1516e::InteractionClassHandle theHandle)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	////////////////////////////////
	// Object Management Services //
	////////////////////////////////

	// 6.3
	virtual void objectInstanceNameReservationSucceeded (
			std::wstring const & theObjectInstanceName)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void objectInstanceNameReservationFailed (
			std::wstring const & theObjectInstanceName)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.6
	virtual void multipleObjectInstanceNameReservationSucceeded (
			std::set<std::wstring> const & theObjectInstanceNames)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void multipleObjectInstanceNameReservationFailed (
			std::set<std::wstring> const & theObjectInstanceNames)
			RTI_THROW ((
				rti1516e::FederateInternalError));


	// 6.9
	virtual void discoverObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::ObjectClassHandle theObjectClass,
			std::wstring const & theObjectInstanceName)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void discoverObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::ObjectClassHandle theObjectClass,
			std::wstring const & theObjectInstanceName,
			rti1516e::FederateHandle producingFederate)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.11
	virtual void reflectAttributeValues (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleValueMap const & theAttributeValues,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::TransportationType theType,
			rti1516e::SupplementalReflectInfo theReflectInfo)
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

	virtual void reflectAttributeValues (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleValueMap const & theAttributeValues,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::TransportationType theType,
			rti1516e::LogicalTime const & theTime,
			rti1516e::OrderType receivedOrder,
			rti1516e::MessageRetractionHandle theHandle,
			rti1516e::SupplementalReflectInfo theReflectInfo)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.13
	virtual void receiveInteraction (
			rti1516e::InteractionClassHandle theInteraction,
			rti1516e::ParameterHandleValueMap const & theParameterValues,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::TransportationType theType,
			rti1516e::SupplementalReceiveInfo theReceiveInfo)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void receiveInteraction (
			rti1516e::InteractionClassHandle theInteraction,
			rti1516e::ParameterHandleValueMap const & theParameterValues,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::TransportationType theType,
			rti1516e::LogicalTime const & theTime,
			rti1516e::OrderType receivedOrder,
			rti1516e::SupplementalReceiveInfo theReceiveInfo)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void receiveInteraction (
			rti1516e::InteractionClassHandle theInteraction,
			rti1516e::ParameterHandleValueMap const & theParameterValues,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::TransportationType theType,
			rti1516e::LogicalTime const & theTime,
			rti1516e::OrderType receivedOrder,
			rti1516e::MessageRetractionHandle theHandle,
			rti1516e::SupplementalReceiveInfo theReceiveInfo)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.15
	virtual void removeObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::SupplementalRemoveInfo theRemoveInfo)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void removeObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::LogicalTime const & theTime,
			rti1516e::OrderType receivedOrder,
			rti1516e::SupplementalRemoveInfo theRemoveInfo)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void removeObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::VariableLengthData const & theUserSuppliedTag,
			rti1516e::OrderType sentOrder,
			rti1516e::LogicalTime const & theTime,
			rti1516e::OrderType receivedOrder,
			rti1516e::MessageRetractionHandle theHandle,
			rti1516e::SupplementalRemoveInfo theRemoveInfo)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.17
	virtual void attributesInScope (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.18
	virtual void attributesOutOfScope (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.20
	virtual void provideAttributeValueUpdate (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes,
			rti1516e::VariableLengthData const & theUserSuppliedTag)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.21
	virtual void turnUpdatesOnForObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void turnUpdatesOnForObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes,
			std::wstring const & updateRateDesignator)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.22
	virtual void turnUpdatesOffForObjectInstance (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.24
	virtual void confirmAttributeTransportationTypeChange (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet theAttributes,
			rti1516e::TransportationType theTransportation)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.26
	virtual void reportAttributeTransportationType (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandle theAttribute,
			rti1516e::TransportationType theTransportation)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.28
	virtual void confirmInteractionTransportationTypeChange (
			rti1516e::InteractionClassHandle theInteraction,
			rti1516e::TransportationType theTransportation)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 6.30
	virtual void reportInteractionTransportationType (
			rti1516e::FederateHandle federateHandle,
			rti1516e::InteractionClassHandle theInteraction,
			rti1516e::TransportationType  theTransportation)
			RTI_THROW ((
				rti1516e::FederateInternalError));


	///////////////////////////////////
	// Ownership Management Services //
	///////////////////////////////////

	// 7.4
	virtual void requestAttributeOwnershipAssumption (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & offeredAttributes,
			rti1516e::VariableLengthData const & theUserSuppliedTag)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 7.5
	virtual void requestDivestitureConfirmation (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & releasedAttributes)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 7.7
	virtual void attributeOwnershipAcquisitionNotification (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & securedAttributes,
			rti1516e::VariableLengthData const & theUserSuppliedTag)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 7.10
	virtual void attributeOwnershipUnavailable (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 7.11
	virtual void requestAttributeOwnershipRelease (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & candidateAttributes,
			rti1516e::VariableLengthData const & theUserSuppliedTag)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 7.16
	virtual void confirmAttributeOwnershipAcquisitionCancellation (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandleSet const & theAttributes)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 7.18
	virtual void informAttributeOwnership (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandle theAttribute,
			rti1516e::FederateHandle theOwner)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void attributeIsNotOwned (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandle theAttribute)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	virtual void attributeIsOwnedByRTI (
			rti1516e::ObjectInstanceHandle theObject,
			rti1516e::AttributeHandle theAttribute)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	//////////////////////////////
	// Time Management Services //
	//////////////////////////////

	// 8.3
	virtual void timeRegulationEnabled (
			rti1516e::LogicalTime const & theFederateTime)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 8.6
	virtual void timeConstrainedEnabled (
			rti1516e::LogicalTime const & theFederateTime)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 8.13
	virtual void timeAdvanceGrant (
			rti1516e::LogicalTime const & theTime)
			RTI_THROW ((
				rti1516e::FederateInternalError));

	// 8.22
	virtual void requestRetraction (
			rti1516e::MessageRetractionHandle theHandle)
			RTI_THROW ((
				rti1516e::FederateInternalError));

private:
	friend class PublisherSubscriber<ADDRESS>;
	friend class ObjectInstanceReserver<ADDRESS>;
	friend class ObjectInstanceRegisterer<ADDRESS>;
	friend class ObjectInstanceDiscoverer<ADDRESS>;
	friend class ObjectInstancesDependencyChecker<ADDRESS>;

	friend class FederateHookStub<ADDRESS>;
	friend class FederateHook<ADDRESS>;
	friend class FederateStub<ADDRESS>;
	
	unisim::kernel::logger::Logger logger;
	unisim::util::json::JSON_Value *p_config;
	bool joined;
	bool ready;
	bool time_constained_enabled;
	bool time_regulation_enabled;
	bool time_advance_granted;
	sc_core::sc_time lookahead;
	
	std::string config_file;
	bool verbose;
	bool debug;
	unisim::kernel::variable::Parameter<std::string> param_config_file;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<bool> param_debug;
	
	
	RTI_UNIQUE_PTR<rti1516e::RTIambassador> rti_ambassador;
	rti1516e::HLAfloat64TimeFactory hla_float64_time_factory;
	
	Registry registry;
	typedef std::vector<FederateHook<ADDRESS> *> FederateHooks;
	FederateHooks federate_hooks;
	typedef std::vector<FederateStub<ADDRESS> *> FederateStubs;
	FederateStubs federate_stubs;
	
	void WaitTime(unsigned int msec);
	
	bool HookConflicts(FederateHook<ADDRESS> *federate_hook) const;
	bool AddHook(FederateHook<ADDRESS> *federate_hook);
	bool StubConflicts(FederateStub<ADDRESS> *federate_stub) const;
	bool AddStub(FederateStub<ADDRESS> *federate_stub);
	
	void Yield();
	void Advance();
};

class FederateHookStubBase
{
public:
	FederateHookStubBase(const std::string& _name, const unisim::util::json::JSON_Value *_json_return_value);
	
	const std::string& GetName() const { return name; }
	const unisim::util::json::JSON_Value *GetReturn() const { return json_return_value; }
	
protected:
	friend class AttributeValue;
	void RequestDependency() { dependencies++; }
	void AckDependency() { dependencies--; }
	bool Ready() const { return dependencies == 0; }
private:
	std::string name;
	unsigned int dependencies;
	const unisim::util::json::JSON_Value *json_return_value;
};

class Attribute
{
public:
	
	Attribute(const ObjectClass *_object_class);
	void Initialize(const unisim::util::json::JSON_Object& config);
	const ObjectClass *GetObjectClass() const { return object_class; }
	const std::wstring& GetName() const { return name; }
	const rti1516e::AttributeHandle& GetHandle() const { return handle; }
	void Bind(const rti1516e::AttributeHandle& _handle) { handle = _handle; }
	void Publish(bool flag = true) { published = flag; }
	void Subscribe(bool flag = true) { subscribed = flag; }
	bool Publishable() const { return publish; }
	bool Published() const { return published; }
	bool Subscribable() const { return subscribe; }
	bool Subscribed() const { return subscribed; }
	
private:
	const ObjectClass *object_class;
	std::wstring name;
	rti1516e::AttributeHandle handle;
	bool publish;
	bool published;
	bool subscribe;
	bool subscribed;
};

class Instrument
{
public:
	enum Direction { READ, WRITE };
	enum Type { HOOK, STUB };
	
	Instrument();
	~Instrument();
	
	void Initialize(const unisim::util::json::JSON_Object& config);
	Direction GetDirection() const { return direction; }
	Type GetType() const { return type; }
	const std::string& GetHookStubName() const { return hook_stub_name; }
	const std::string& GetExpression() const { return expr; }

private:
	Direction direction;
	Type type;
	std::string expr;
	std::string hook_stub_name;
};

inline std::ostream& operator << (std::ostream& stream, const Instrument::Direction& dir) { return stream << ((dir == Instrument::READ) ? "read" : "write"); }

class AttributeValue
{
public:
	AttributeValue(const ObjectInstance *object_instance, const Attribute *_attribute);
	void Initialize(const unisim::util::json::JSON_Object& config);
	const ObjectInstance *GetObjectInstance() const { return object_instance; }
	const Attribute *GetAttribute() const { return attribute; }
	const Instrument& GetInstrument() const { return instrument; }
	const void *Data() const { return value.data(); }
	size_t Size() const { return value.size(); }
	void SetData(const void *buffer, size_t size, const sc_core::sc_time& _time_stamp) { value.setData(buffer, size); time_stamp = _time_stamp; valid = true; }
	const rti1516e::VariableLengthData& Get() const { return value; }
	void Set(const rti1516e::VariableLengthData& _value, const sc_core::sc_time& _time_stamp) { value = _value; time_stamp = _time_stamp; valid = true; }
	void Invalidate() { valid = false; }
	bool IsValid() const { return valid; }
	const sc_core::sc_time& TimeStamp() const { return time_stamp; }
	void AddSensitiveHookStub(FederateHookStubBase *federare_hook_stub) { federare_hook_stub->RequestDependency(); sensitive_hook_stubs.push_back(federare_hook_stub); }
	void Trigger() { for(SensitiveHookStubs::const_iterator it = sensitive_hook_stubs.begin(); it != sensitive_hook_stubs.end(); ++it) (*it)->AckDependency(); }
private:
	const ObjectInstance *object_instance;
	const Attribute *attribute;
	Instrument instrument;
	rti1516e::VariableLengthData value;
	sc_core::sc_time time_stamp;
	bool valid;
	typedef std::vector<FederateHookStubBase *> SensitiveHookStubs;
	SensitiveHookStubs sensitive_hook_stubs;
};

class ObjectInstance
{
public:
	ObjectInstance(const ObjectClass *_object_class);
	void Initialize(const unisim::util::json::JSON_Object& config);
	const std::wstring& GetName() const { return name; }
	const ObjectClass *GetObjectClass() const { return object_class; }
	void Bind(const rti1516e::ObjectInstanceHandle& _handle) { handle = _handle; }
	const rti1516e::ObjectInstanceHandle& GetHandle() const { return handle; }
	void AddAttributeValue(AttributeValue *attribute_value);
	AttributeValue *GetAttributeValue(const std::wstring& name);
	
	template <typename VISITOR> void ScanAttributeValues(VISITOR& visitor) const;
private:
	friend class ObjectClass;
	
	const ObjectClass *object_class;
	std::wstring name;
	rti1516e::ObjectInstanceHandle handle;
	typedef std::map<std::wstring, AttributeValue *> AttributeValues;
	AttributeValues attribute_values;
};

class ObjectClass
{
public:
	ObjectClass();
	void Initialize(const unisim::util::json::JSON_Object& config);
	const std::wstring& GetName() const { return name; }
	const rti1516e::ObjectClassHandle& GetHandle() const { return handle; }
	void Bind(rti1516e::ObjectClassHandle _handle) { handle = _handle; }
	void AddAttribute(Attribute *attribute);
	Attribute *GetAttribute(const std::wstring& name) const;
	void AddObjectInstance(ObjectInstance *object_instance);
	ObjectInstance *GetObjectInstance(const std::wstring& name);
	
	template <typename VISITOR> void ScanAttributes(VISITOR& visitor) const;
	template <typename VISITOR> void ScanObjectInstances(VISITOR& visitor) const;
private:
	std::wstring name;
	rti1516e::ObjectClassHandle handle;
	typedef std::map<std::wstring, Attribute *> Attributes;
	Attributes attributes;
	typedef std::map<std::wstring, ObjectInstance *> ObjectInstances;
	ObjectInstances object_instances;
};

class HookStub
{
public:
	HookStub();
	~HookStub();
	void Initialize(const unisim::util::json::JSON_Object& config);
	const std::string& GetName() const { return name; }
	unsigned int GetProcessor() const { return processor; }
	const std::string& GetLocation() const { return loc; }
	const unisim::util::json::JSON_Value *GetReturn() const { return json_return_value; }
private:
	std::string name;
	unsigned int processor;
	std::string loc;
	unisim::util::json::JSON_Value *json_return_value;
};

class Hook : public HookStub {};
class Stub : public HookStub {};

template <typename ADDRESS>
class PublisherSubscriber
{
public:
	enum Mode
	{
		PUBLISH,
		SUBSCRIBE
	};
	
	enum Action
	{
		SET,
		UNSET
	};
	
	PublisherSubscriber(Federate<ADDRESS>& federate, Mode mode, Action action);
	
	void Do();

private:
	bool Visit(ObjectClass *object_class);
	
	friend class Registry;
	
	Federate<ADDRESS>& federate;
	Mode mode;
	Action action;
};

template <typename ADDRESS>
class ObjectInstanceReserver
{
public:
	ObjectInstanceReserver(Federate<ADDRESS>& federate);
	
	void Do();
private:
	friend class Registry;
	
	bool Visit(ObjectClass *object_class);
	
	Federate<ADDRESS>& federate;
};

template <typename ADDRESS>
class ObjectInstanceRegisterer
{
public:
	ObjectInstanceRegisterer(Federate<ADDRESS>& federate, const std::wstring& object_instance_name);
	
	void Do();
private:
	friend class Registry;
	
	bool Visit(ObjectClass *object_class);
	
	Federate<ADDRESS>& federate;
	std::wstring object_instance_name;
};

template <typename ADDRESS>
class ObjectInstanceDiscoverer
{
public:
	ObjectInstanceDiscoverer(Federate<ADDRESS>& _federate, const rti1516e::ObjectInstanceHandle& _object_instance_handle, const rti1516e::ObjectClassHandle& _object_class_handle, const std::wstring& _object_instance_name);
	
	void Do();
private:
	friend class Registry;
	
	Federate<ADDRESS>& federate;
	const rti1516e::ObjectInstanceHandle& object_instance_handle;
	const rti1516e::ObjectClassHandle& object_class_handle;
	const std::wstring& object_instance_name;
	
	bool Visit(ObjectClass *object_class);
};

template <typename ADDRESS>
class ObjectInstancesDependencyChecker
{
public:
	ObjectInstancesDependencyChecker(Federate<ADDRESS>& _federate);

	bool Check();
	
private:
	friend class Registry;
	
	Federate<ADDRESS>& federate;
	bool status;
	
	bool Visit(ObjectClass *object_class);
};

template <typename ADDRESS>
class FederateHookStub : public FederateHookStubBase
{
public:
	FederateHookStub(Federate<ADDRESS>& _federate, const std::string& _name, const unisim::util::json::JSON_Value *_json_return_value);
	
	void Bind(AttributeValue *attribute_value);
protected:
	Federate<ADDRESS>& federate;
	
	void WaitDependencies();
	void DoInstrument(Instrument::Direction dir);
private:
	typedef std::pair<AttributeValue *, unisim::util::debug::DataObjectRef<ADDRESS> > AttributeValueBinding;
	typedef std::vector<AttributeValueBinding> AttributeValueBindings;
	AttributeValueBindings attribute_value_bindings;
};

template <typename ADDRESS>
class FederateHook
	: public unisim::util::debug::SourceCodeHook<ADDRESS>
	, public FederateHookStub<ADDRESS>
{
public:
	FederateHook(Federate<ADDRESS>& _federate, const std::string& _name, const unisim::util::debug::SourceCodeLocation& _source_code_location, const unisim::util::json::JSON_Value *_json_return_value);
	virtual bool Run(typename unisim::util::debug::Hook<ADDRESS>::ReturnValue& return_value);
};

template <typename ADDRESS>
class FederateStub
	: public unisim::util::debug::Stub<ADDRESS>
	, public FederateHookStub<ADDRESS>
{
public:
	FederateStub(Federate<ADDRESS>& _federate, const std::string& _name, const typename unisim::util::debug::SubProgram<ADDRESS> *_subprogram, const unisim::util::json::JSON_Value *_json_return_value);
	virtual bool Run(typename unisim::util::debug::Stub<ADDRESS>::Parameters& parameters, typename unisim::util::debug::Stub<ADDRESS>::ReturnValue& return_value);
};

} // end of namespace hla
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_HH__

