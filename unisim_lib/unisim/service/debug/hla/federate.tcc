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

#ifndef __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_TCC__
#define __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_TCC__

#include <unisim/service/debug/hla/federate.hh>
#include <unisim/util/debug/data_object.tcc>
#include <RTI/RTIambassadorFactory.h>
#include <fstream>

namespace unisim {
namespace service {
namespace debug {
namespace hla {

template <typename ADDRESS>
Federate<ADDRESS>::Federate(const char *_name, unisim::kernel::Object *_parent)
	: unisim::kernel::Object(_name, _parent, "this service implements an HLA Federate")
	, rti1516e::FederateAmbassador()
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugSelecting>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::StackUnwinding>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Stubbing<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Hooking<ADDRESS> >(_name, _parent)
	, debug_yielding_export("debug-yielding-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_yielding_request_import("debug-yielding-request-import", this)
	, debug_selecting_import("debug-selecting-import", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, disasm_import("disasm-import", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, backtrace_import("backtrace-import", this)
	, profiling_import("profiling-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, data_object_lookup_import("data-object-lookup-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, stack_unwinding_import("stack-unwinding-import", this)
	, stubbing_import("stubbing-import", this)
	, hooking_import("hooking-import", this)
	, logger(*this)
	, p_config(0)
	, joined(false)
	, ready(false)
	, time_constained_enabled(false)
	, time_regulation_enabled(false)
	, time_advance_granted(false)
	, lookahead()
	, config_file()
	, verbose(false)
	, debug(false)
	, param_config_file("config-file", this, config_file, "Configuration file")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, debug, "Enable/Disable debug (intended for developper)")
	, registry()
	, federate_hooks()
	, federate_stubs()
{
}

template <typename ADDRESS>
Federate<ADDRESS>::~Federate()
{
	PublisherSubscriber<ADDRESS> unpublisher(*this, PublisherSubscriber<ADDRESS>::PUBLISH, PublisherSubscriber<ADDRESS>::UNSET);
	unpublisher.Do();
	
	PublisherSubscriber<ADDRESS> unsubscriber(*this, PublisherSubscriber<ADDRESS>::SUBSCRIBE, PublisherSubscriber<ADDRESS>::UNSET);
	unsubscriber.Do();
	
	for(typename FederateHooks::const_iterator it = federate_hooks.begin(); it != federate_hooks.end(); ++it)
	{
		FederateHook<ADDRESS> *federate_hook = *it;
		if(verbose)
		{
			logger << DebugInfo << "Removing hook at " << federate_hook->GetSourceCodeLocation() << EndDebugInfo;
		}
		if(!hooking_import->RemoveHook(federate_hook))
		{
			logger << DebugWarning << "Can't remove hook at " << federate_hook->GetSourceCodeLocation() << EndDebugWarning;
		}
	}
	
	if(rti_ambassador.get() && joined)
	{
		try
		{
			if(verbose)
			{
				logger << DebugInfo << "Resigning from federation" << EndDebugInfo;
			}
			rti_ambassador->resignFederationExecution(
				rti1516e::DELETE_OBJECTS
			);
			if(verbose)
			{
				logger << DebugInfo << "Resigned" << EndDebugInfo;
			}
		}
		catch(rti1516e::Exception& e)
		{
			logger << DebugError << e.what() << EndDebugError;
		}
	}
	
	if(p_config) delete p_config;
}

// unisim::service::interfaces::DebugYielding
template <typename ADDRESS>
void Federate<ADDRESS>::DebugYield()
{
}

// unisim::service::interfaces::DebugEventListener<ADDRESS>
template <typename ADDRESS>
void Federate<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
}

// unisim::kernel::Object
template <typename ADDRESS>
bool Federate<ADDRESS>::BeginSetup()
{
	if(config_file.empty())
	{
		logger << DebugError << param_config_file.GetDescription() << " is unset" << EndDebugError;
		return false;
	}
	
	std::ifstream stream(config_file.c_str());
	
	if(stream.fail())
	{
		logger << DebugError << "Can't open \"" << config_file << "\"" << EndDebugError;
		return false;
	}
	
	unisim::util::json::JSON_Parser<unisim::util::json::JSON_AST_Builder> json_parser;
	unisim::util::json::JSON_AST_Builder json_ast_builder;
	
	p_config = json_parser.Parse(stream, json_ast_builder);
	
	if(!p_config) return false;
	
	return true;
}

// unisim::kernel::Object
template <typename ADDRESS>
bool Federate<ADDRESS>::EndSetup()
{
	if(!p_config) return false;
	
	try
	{
		const unisim::util::json::JSON_Object& config = p_config->AsObject();
		
		rti1516e::RTIambassadorFactory rti_ambassador_factory;
		rti_ambassador = rti_ambassador_factory.createRTIambassador();
		
		std::wstring url = config["url"].AsString();
		try
		{
			if(verbose)
			{
				logger << DebugInfo << "Connecting to RTI at " << url << EndDebugInfo;
			}
			rti_ambassador->connect(*this, rti1516e::HLA_EVOKED, url);
			if(verbose)
			{
				logger << DebugInfo << "Connected" << EndDebugInfo;
			}
		}
		catch(rti1516e::Exception& e)
		{
			logger << DebugError << e.what() << EndDebugError;
			return false;
		}
		
		std::wstring federation_execution_name = config["federateExecutionName"].AsString();
		std::wstring fom_module = config["fomModule"].AsString();
		
		try
		{
			if(verbose)
			{
				logger << DebugInfo << "Creating federation execution " << federation_execution_name << " with FOM module " << fom_module << EndDebugInfo;
			}
			rti_ambassador->createFederationExecution(
				federation_execution_name,
				fom_module
			);
			if(verbose)
			{
				logger << DebugInfo << "Created" << EndDebugInfo;
			}
		}
		catch(rti1516e::FederationExecutionAlreadyExists& e)
		{
			logger << DebugWarning << "Federation \"" << federation_execution_name << "\" already exists" << EndDebugWarning;
		}
		catch(rti1516e::Exception& e)
		{
			logger << DebugError << e.what() << EndDebugError;
			return false;
		}
		
		std::wstring federate_name = config["federateName"].AsString();
		std::wstring federate_type = config["federateType"].AsString();
		
		joined = false;
		do
		{
			try
			{
				if(verbose)
				{
					logger << DebugInfo << "Trying to join federation execution " << federation_execution_name << " as Federate " << federate_name << " with type " << federate_type << EndDebugInfo;
				}
				rti_ambassador->joinFederationExecution(
					federate_name,
					federate_type,
					federation_execution_name
				);
				joined = true;
				if(verbose)
				{
					logger << DebugInfo << "Joined" << EndDebugInfo;
				}
			}
			catch(rti1516e::Exception& e)
			{
				logger << DebugWarning << e.what() << EndDebugWarning;
				logger << DebugInfo << "Retying later..." << EndDebugInfo;
				WaitTime(1000);
			}
		}
		while(!joined && !Killed());
		
		if(Killed()) return false;
		
		if(!registry.Initialize(config, logger)) return false;
		
		struct HandleBinder
		{
			Federate<ADDRESS>& federate;
			
			HandleBinder(Federate<ADDRESS>& _federate) : federate(_federate) {}
			
			bool Visit(ObjectClass *object_class)
			{
				if(federate.verbose)
				{
					federate.logger << DebugInfo << "Getting handle of Class " << object_class->GetName() << EndDebugInfo;
				}
				try
				{
					rti1516e::ObjectClassHandle handle = federate.rti_ambassador->getObjectClassHandle(object_class->GetName());
					object_class->Bind(handle);
					
					object_class->ScanAttributes(*this);
				}
				catch (rti1516e::Exception& e)
				{
					federate.logger << DebugError << e.what() << EndDebugError;
				}
				return true;
			}
			
			bool Visit(Attribute *attribute)
			{
				if(federate.verbose)
				{
					federate.logger << DebugInfo << "Getting handle of Attribute " << attribute->GetName() << " from Class " << attribute->GetObjectClass()->GetName() << EndDebugInfo;
				}
				try
				{
					rti1516e::AttributeHandle handle = federate.rti_ambassador->getAttributeHandle(attribute->GetObjectClass()->GetHandle(), attribute->GetName());
					if(federate.verbose)
					{
						federate.logger << DebugInfo << "Got " << handle.toString() << EndDebugInfo;
					}
					attribute->Bind(handle);
				}
				catch (rti1516e::Exception& e)
				{
					federate.logger << DebugError << e.what() << EndDebugError;
				}
				return true;
			}
			
			void Do()
			{
				federate.registry.ScanObjectClasses(*this);
			}
		};
		
		HandleBinder handle_binder(*this);
		handle_binder.Do();
		
		PublisherSubscriber<ADDRESS> publisher(*this, PublisherSubscriber<ADDRESS>::PUBLISH, PublisherSubscriber<ADDRESS>::SET);
		publisher.Do();
		
		PublisherSubscriber<ADDRESS> subscriber(*this, PublisherSubscriber<ADDRESS>::SUBSCRIBE, PublisherSubscriber<ADDRESS>::SET);
		subscriber.Do();
		
		ObjectInstanceReserver<ADDRESS> object_instance_reserver(*this);
		object_instance_reserver.Do();
		
		struct HookStubCreator
		{
			Federate<ADDRESS>& federate;
			
			HookStubCreator(Federate<ADDRESS>& _federate) : federate(_federate) {}
			
			bool Visit(Hook *hook)
			{
				unisim::util::debug::SourceCodeLocation source_code_location;
				
				const std::string& loc = hook->GetLocation();
				int prc_num = hook->GetProcessor();
				const std::string& name = hook->GetName();
				const unisim::util::json::JSON_Value *json_return_value = hook->GetReturn();
				
				if(source_code_location.Parse(loc.c_str()))
				{
					unsigned int sel_prc_num = federate.debug_selecting_import->DebugGetSelected(); // save
					if(federate.debug_selecting_import->DebugSelect(prc_num))
					{
						FederateHook<ADDRESS> *federate_hook = new FederateHook<ADDRESS>(federate, name, source_code_location, json_return_value);
						
						if(federate.verbose)
						{
							federate.logger << DebugInfo << "Setting Hook \"" << name << "\" at " << loc << " for processor #" << prc_num << EndDebugInfo;
						}
						
						if(federate.HookConflicts(federate_hook))
						{
							federate.logger << DebugWarning << "Hook \"" << name << "\" at " << loc << " for processor #" << prc_num << " conflicts" << EndDebugWarning;
							delete federate_hook;
						}
						else if(federate.hooking_import->SetHook(federate_hook))
						{
							 federate.AddHook(federate_hook);
						}
						else
						{
							federate.logger << DebugWarning << "Can't set Hook \"" << name << "\" at " << loc << " for processor #" << prc_num << EndDebugWarning;
							delete federate_hook;
						}
						
						federate.debug_selecting_import->DebugSelect(sel_prc_num); // restore
					}
					else
					{
						federate.logger << DebugWarning << "Can't select processor #" << prc_num << EndDebugWarning;
					}
				}
				else
				{
					federate.logger << DebugWarning << "\"" << loc << "\" is not a source code location" << EndDebugWarning;
				}
				
				return true;
			}
			
			bool Visit(Stub *stub)
			{
				const std::string& loc = stub->GetLocation();
				int prc_num = stub->GetProcessor();
				const std::string& name = stub->GetName();
				const unisim::util::json::JSON_Value *json_return_value = stub->GetReturn();
				
				unsigned int sel_prc_num = federate.debug_selecting_import->DebugGetSelected(); // save
				if(federate.debug_selecting_import->DebugSelect(prc_num))
				{
					if(federate.verbose)
					{
						federate.logger << DebugInfo << "Setting Stub \"" << name << "\" at " << loc << " for processor #" << prc_num << EndDebugInfo;
					}
					
					const unisim::util::debug::SubProgram<ADDRESS> *subprogram = federate.subprogram_lookup_import->FindSubProgram(loc.c_str());
					
					if(subprogram)
					{
						FederateStub<ADDRESS> *federate_stub = new FederateStub<ADDRESS>(federate, name, subprogram, json_return_value);
						
						if(federate.StubConflicts(federate_stub))
						{
							federate.logger << DebugWarning << "Stub \"" << name << "\" at " << loc << " for processor #" << prc_num << " conflicts" << EndDebugWarning;
							delete federate_stub;
						}
						if(federate.stubbing_import->SetStub(federate_stub))
						{
							federate.AddStub(federate_stub);
						}
						else
						{
							federate.logger << DebugWarning << "Can't set Stub \"" << name << "\" at " << loc << " for processor #" << prc_num << EndDebugWarning;
							delete federate_stub;
						}
					}
					else
					{
						federate.logger << DebugWarning << "Subprogram \"" << loc << "\" not found" << EndDebugWarning;
					}
					
					federate.debug_selecting_import->DebugSelect(sel_prc_num); // restore
				}
				else
				{
					federate.logger << DebugWarning << "Can't select processor #" << prc_num << EndDebugWarning;
				}
				
				return true;
			}
			
			void Do()
			{
				federate.registry.ScanHooks(*this);
				federate.registry.ScanStubs(*this);
			}
		};
		
		HookStubCreator hook_stub_creator(*this);
		hook_stub_creator.Do();

		struct HookStubBinder
		{
			Federate<ADDRESS>& federate;
			
			HookStubBinder(Federate<ADDRESS>& _federate) : federate(_federate) {}
			
			bool Visit(ObjectClass *object_class)
			{
				struct ObjectInstanceHookStubBinder
				{
					Federate<ADDRESS>& federate;
			
					ObjectInstanceHookStubBinder(Federate<ADDRESS>& _federate) : federate(_federate) {}
					
					bool Visit(ObjectInstance *object_instance)
					{
						struct AttributeValueHookStubBinder
						{
							Federate<ADDRESS>& federate;
			
							AttributeValueHookStubBinder(Federate<ADDRESS>& _federate) : federate(_federate) {}
							
							bool Visit(AttributeValue *attribute_value)
							{
								unisim::util::debug::SourceCodeLocation source_code_location;
								
								const Instrument& instrument = attribute_value->GetInstrument();
								
								typename Instrument::Type type = instrument.GetType();
								const std::string& hook_stub_name = instrument.GetHookStubName();
								
								switch(type)
								{
									case Instrument::HOOK:
										for(typename FederateHooks::const_iterator it = federate.federate_hooks.begin(); it != federate.federate_hooks.end(); ++it)
										{
											FederateHook<ADDRESS> *federate_hook = *it;
											if(federate_hook->GetName() == hook_stub_name)
											{
												if(federate.verbose)
												{
													federate.logger << DebugInfo << "Binding Attribute value \"" << attribute_value->GetAttribute()->GetName() << "\" of object Instance \"" << attribute_value->GetObjectInstance()->GetName() << "\" to Hook \"" << hook_stub_name << "\"" << EndDebugInfo;
												}
												federate_hook->Bind(attribute_value);
											}
										}
										break;
									case Instrument::STUB:
										for(typename FederateStubs::const_iterator it = federate.federate_stubs.begin(); it != federate.federate_stubs.end(); ++it)
										{
											FederateStub<ADDRESS> *federate_stub = *it;
											if(federate_stub->GetName() == hook_stub_name)
											{
												if(federate.verbose)
												{
													federate.logger << DebugInfo << "Binding Attribute value \"" << attribute_value->GetAttribute()->GetName() << "\" of object Instance \"" << attribute_value->GetObjectInstance()->GetName() << "\" to Stub \"" << hook_stub_name << "\"" << EndDebugInfo;
												}
												federate_stub->Bind(attribute_value);
											}
										}
										break;
								}
								return true;
							}
							
							void Do(ObjectInstance *object_instance)
							{
								object_instance->ScanAttributeValues(*this);
							}
						};
						
						AttributeValueHookStubBinder attribute_value_hook_stub_binder(federate);
						attribute_value_hook_stub_binder.Do(object_instance);
						
						return true;
					}
					
					void Do(ObjectClass *object_class)
					{
						object_class->ScanObjectInstances(*this);
					}
				};
				
				ObjectInstanceHookStubBinder object_instance_hook_stub_binder(federate);
				object_instance_hook_stub_binder.Do(object_class);
				
				return true;
			}
			
			void Do()
			{
				federate.registry.ScanObjectClasses(*this);
			}
		};
		
		HookStubBinder hook_stub_binder(*this);
		hook_stub_binder.Do();
		
		if(verbose)
		{
			logger << DebugInfo << "Enabling time constrained to receive TSO messages" << EndDebugInfo;
		}
		rti_ambassador->enableTimeConstrained();
		
		rti_ambassador->evokeMultipleCallbacks(0.1, std::numeric_limits<double>::infinity());
		
		if(config.HasProperty("lookahead"))
		{
			std::stringstream lookahead_sstr((const std::string&) config["lookahead"].AsString());
			double lookahead_time_value;
			std::string lookahead_time_unit;
			
			if((lookahead_sstr >> lookahead_time_value) && (lookahead_sstr >> lookahead_time_unit))
			{
				if(lookahead_time_unit.compare("s") == 0) lookahead = sc_core::sc_time(lookahead_time_value, sc_core::SC_SEC);
				else if(lookahead_time_unit.compare("ms") == 0) lookahead = sc_core::sc_time(lookahead_time_value, sc_core::SC_MS);
				else if(lookahead_time_unit.compare("us") == 0) lookahead = sc_core::sc_time(lookahead_time_value, sc_core::SC_US);
				else if(lookahead_time_unit.compare("ns") == 0) lookahead = sc_core::sc_time(lookahead_time_value, sc_core::SC_NS);
				else if(lookahead_time_unit.compare("ps") == 0) lookahead = sc_core::sc_time(lookahead_time_value, sc_core::SC_PS);
				else if(lookahead_time_unit.compare("fs") == 0) lookahead = sc_core::sc_time(lookahead_time_value, sc_core::SC_FS);
				else
				{
					logger << DebugWarning << "malformed time unit ('" << lookahead_time_unit << "') in lookahead: expecting 's', 'ms', 'us', 'ns', 'ps', or 'fs'." << EndDebugWarning;
				}
			}
			else
			{
				logger << DebugWarning << "expecting a time in lookahead (e.g. 1 ps)" << EndDebugWarning;
			}
			
			if(!lookahead_sstr.eof())
			{
				logger << DebugWarning << "ignoring extra characters in lookead" << EndDebugWarning;
			}
		}
		
		if(verbose)
		{
			logger << DebugInfo << "Enabling time regulation with a " << lookahead << " lookahead to send TSO messages" << EndDebugInfo;
		}
		RTI_UNIQUE_PTR<rti1516e::HLAfloat64Interval> hla_float64_time_interval = hla_float64_time_factory.makeLogicalTimeInterval(lookahead.to_seconds());
		const rti1516e::LogicalTimeInterval& theLookahead = *(rti1516e::LogicalTimeInterval *) hla_float64_time_interval.get();
		
		rti_ambassador->enableTimeRegulation(theLookahead);
		
		rti_ambassador->evokeMultipleCallbacks(0.1, std::numeric_limits<double>::infinity());
		
		if(verbose)
		{
			logger << DebugInfo << "Waiting for object instance discovery..." << EndDebugInfo;
		}
		while(!ready && !Killed())
		{
			rti_ambassador->evokeMultipleCallbacks(0.1, std::numeric_limits<double>::infinity());
		}
		if(verbose)
		{
			logger << DebugInfo << "Done" << EndDebugInfo;
		}
	}
	catch(std::exception& e)
	{
		logger << DebugError << e.what() << EndDebugError;
		return false;
	}
	
	return true;
}

// rti1516e::FederateAmbassador

// 4.4
template <typename ADDRESS>
void Federate<ADDRESS>::connectionLost (
		std::wstring const & faultDescription)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.8
template <typename ADDRESS>
void Federate<ADDRESS>::reportFederationExecutions (
		rti1516e::FederationExecutionInformationVector const &
		theFederationExecutionInformationList)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.12
template <typename ADDRESS>
void Federate<ADDRESS>::synchronizationPointRegistrationSucceeded (
		std::wstring const & label)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::synchronizationPointRegistrationFailed (
		std::wstring const & label,
		rti1516e::SynchronizationPointFailureReason reason)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.13
template <typename ADDRESS>
void Federate<ADDRESS>::announceSynchronizationPoint (
		std::wstring  const & label,
		rti1516e::VariableLengthData const & theUserSuppliedTag)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.15
template <typename ADDRESS>
void Federate<ADDRESS>::federationSynchronized (
		std::wstring const & label,
		rti1516e::FederateHandleSet const& failedToSyncSet)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.17
template <typename ADDRESS>
void Federate<ADDRESS>::initiateFederateSave (
		std::wstring const & label)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::initiateFederateSave (
		std::wstring const & label,
		rti1516e::LogicalTime const & theTime)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.20
template <typename ADDRESS>
void Federate<ADDRESS>::federationSaved ()
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::federationNotSaved (
		rti1516e::SaveFailureReason theSaveFailureReason)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}


// 4.23
template <typename ADDRESS>
void Federate<ADDRESS>::federationSaveStatusResponse (
		rti1516e::FederateHandleSaveStatusPairVector const &
		theFederateStatusVector)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.25
template <typename ADDRESS>
void Federate<ADDRESS>::requestFederationRestoreSucceeded (
		std::wstring const & label)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::requestFederationRestoreFailed (
		std::wstring const & label)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.26
template <typename ADDRESS>
void Federate<ADDRESS>::federationRestoreBegun ()
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.27
template <typename ADDRESS>
void Federate<ADDRESS>::initiateFederateRestore (
		std::wstring const & label,
		std::wstring const & federateName,
		rti1516e::FederateHandle handle)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.29
template <typename ADDRESS>
void Federate<ADDRESS>::federationRestored ()
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::federationNotRestored (
		rti1516e::RestoreFailureReason theRestoreFailureReason)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 4.32
template <typename ADDRESS>
void Federate<ADDRESS>::federationRestoreStatusResponse (
		rti1516e::FederateRestoreStatusVector const &
		theFederateRestoreStatusVector)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

/////////////////////////////////////
// Declaration Management Services //
/////////////////////////////////////

// 5.10
template <typename ADDRESS>
void Federate<ADDRESS>::startRegistrationForObjectClass (
		rti1516e::ObjectClassHandle theClass)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 5.11
template <typename ADDRESS>
void Federate<ADDRESS>::stopRegistrationForObjectClass (
		rti1516e::ObjectClassHandle theClass)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 5.12
template <typename ADDRESS>
void Federate<ADDRESS>::turnInteractionsOn (
		rti1516e::InteractionClassHandle theHandle)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 5.13
template <typename ADDRESS>
void Federate<ADDRESS>::turnInteractionsOff (
		rti1516e::InteractionClassHandle theHandle)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

////////////////////////////////
// Object Management Services //
////////////////////////////////

// 6.3
template <typename ADDRESS>
void Federate<ADDRESS>::objectInstanceNameReservationSucceeded (
		std::wstring const & theObjectInstanceName)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	ObjectInstanceRegisterer<ADDRESS> object_instance_registerer(*this, theObjectInstanceName);
	object_instance_registerer.Do();
}

template <typename ADDRESS>
void Federate<ADDRESS>::objectInstanceNameReservationFailed (
		std::wstring const & theObjectInstanceName)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	logger << DebugError << "Reservation of Object instance name \"" << theObjectInstanceName << "\" failed" << EndDebugError;
	Stop(-1);
}

// 6.6
template <typename ADDRESS>
void Federate<ADDRESS>::multipleObjectInstanceNameReservationSucceeded (
		std::set<std::wstring> const & theObjectInstanceNames)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	for(std::set<std::wstring>::const_iterator it = theObjectInstanceNames.begin();  it != theObjectInstanceNames.end(); ++it)
	{
		const std::wstring& theObjectInstanceName = *it;
		ObjectInstanceRegisterer<ADDRESS> object_instance_registerer(*this, theObjectInstanceName);
		object_instance_registerer.Do();
	}
}

template <typename ADDRESS>
void Federate<ADDRESS>::multipleObjectInstanceNameReservationFailed (
		std::set<std::wstring> const & theObjectInstanceNames)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	for(typename std::set<std::wstring>::const_iterator it = theObjectInstanceNames.begin(); it != theObjectInstanceNames.end(); ++it)
	{
		const std::wstring& theObjectInstanceName = *it;
		logger << DebugError << "Reservation of Object instance name \"" << theObjectInstanceName << "\" failed" << EndDebugError;
		Stop(-1);
	}
}


// 6.9
template <typename ADDRESS>
void Federate<ADDRESS>::discoverObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::ObjectClassHandle theObjectClass,
		std::wstring const & theObjectInstanceName)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	ObjectInstanceDiscoverer<ADDRESS> object_instance_discoverer(*this, theObject, theObjectClass, theObjectInstanceName);
	object_instance_discoverer.Do();
	
	ObjectInstancesDependencyChecker<ADDRESS> object_instances_dependency_checker(*this);
	ready = object_instances_dependency_checker.Check();
}

template <typename ADDRESS>
void Federate<ADDRESS>::discoverObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::ObjectClassHandle theObjectClass,
		std::wstring const & theObjectInstanceName,
		rti1516e::FederateHandle producingFederate)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	this->discoverObjectInstance(theObject, theObjectClass, theObjectInstanceName);
}

// 6.11
template <typename ADDRESS>
void Federate<ADDRESS>::reflectAttributeValues (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleValueMap const & theAttributeValues,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::TransportationType theType,
		rti1516e::SupplementalReflectInfo theReflectInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	RTI_UNIQUE_PTR<rti1516e::HLAfloat64Time> hla_float64_time = hla_float64_time_factory.makeLogicalTime(sc_core::sc_time_stamp().to_seconds());
	const rti1516e::LogicalTime& theTime = *(rti1516e::LogicalTime *) hla_float64_time.get();
	this->reflectAttributeValues(theObject, theAttributeValues, theUserSuppliedTag, sentOrder, theType, theTime, sentOrder, theReflectInfo);
}

template <typename ADDRESS>
void Federate<ADDRESS>::reflectAttributeValues (
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
// 	reflect_attribute_values_request_queue.Lock();
// 	reflect_attribute_values_request_queue.Push(ReflectAttributeValuesRequest(theObject, theAttributeValues, theUserSuppliedTag, sentOrder, theType, rti1516e::HLAfloat64Time(theTime), receivedOrder));
// 	reflect_attribute_values_request_queue.Unlock();
// 	Run();
	
	struct AttributeValueReflecter
	{
		Federate<ADDRESS>& federate;
		const rti1516e::ObjectInstanceHandle& object_instance_handle;
		const rti1516e::AttributeHandleValueMap& attribute_handle_value_map;
		const rti1516e::VariableLengthData& tag;
		rti1516e::OrderType sent_order;
		rti1516e::TransportationType transportation_type;
		const rti1516e::HLAfloat64Time& logical_time;
		rti1516e::OrderType receivedOrder;
		
		AttributeValueReflecter(
			Federate<ADDRESS>& _federate,
			const rti1516e::ObjectInstanceHandle& _object_instance_handle,
			const rti1516e::AttributeHandleValueMap& _attribute_handle_value_map,
			const rti1516e::VariableLengthData& _tag,
			rti1516e::OrderType _sent_order,
			rti1516e::TransportationType _transportation_type,
			const rti1516e::HLAfloat64Time& _logical_time,
			rti1516e::OrderType _receivedOrder
		)
			: federate(_federate)
			, object_instance_handle(_object_instance_handle)
			, attribute_handle_value_map(_attribute_handle_value_map)
			, tag(_tag)
			, sent_order(_sent_order)
			, transportation_type(_transportation_type)
			, logical_time(_logical_time)
			, receivedOrder(_receivedOrder)
		{
		}
		
		bool Visit(ObjectClass *object_class)
		{
			struct ObjectClassAttributeValueReflecter
			{
				AttributeValueReflecter& attribute_value_reflecter;
				
				ObjectClassAttributeValueReflecter(AttributeValueReflecter& _attribute_value_reflecter) : attribute_value_reflecter(_attribute_value_reflecter) {}
				
				bool Visit(ObjectInstance *object_instance)
				{
					const rti1516e::ObjectInstanceHandle& object_instance_handle = attribute_value_reflecter.object_instance_handle;
					if(object_instance->GetHandle() == object_instance_handle)
					{
						struct ObjectInstanceAttributeValueReflecter
						{
							AttributeValueReflecter& attribute_value_reflecter;
							
							ObjectInstanceAttributeValueReflecter(AttributeValueReflecter& _attribute_value_reflecter) : attribute_value_reflecter(_attribute_value_reflecter) {}
							
							bool Visit(AttributeValue *attribute_value)
							{
								const rti1516e::AttributeHandleValueMap& attribute_handle_value_map = attribute_value_reflecter.attribute_handle_value_map;
								typename rti1516e::AttributeHandleValueMap::const_iterator it = attribute_handle_value_map.find(attribute_value->GetAttribute()->GetHandle());
								if(it != attribute_handle_value_map.end())
								{
									const rti1516e::VariableLengthData& value = (*it).second;
									
									if(!attribute_value->IsValid()) attribute_value->Trigger();
									attribute_value->Set(value, sc_core::sc_get_time_resolution() * attribute_value_reflecter.logical_time.getTime());
								}
									
								return true;
							}
							
							void Do(ObjectInstance *object_instance)
							{
								object_instance->ScanAttributeValues(*this);
							}
						};
					}
					
					return true;
				}
				
				void Do(ObjectClass *object_class)
				{
					object_class->ScanObjectInstances(*this);
				}
			};
			
			return true;
		}
		
		void Do()
		{
			federate.registry.ScanObjectClasses(*this);
		}
	};
	
	AttributeValueReflecter attribute_value_reflecter(*this, theObject, theAttributeValues, theUserSuppliedTag, sentOrder, theType, rti1516e::HLAfloat64Time(theTime), receivedOrder);
	attribute_value_reflecter.Do();
}

template <typename ADDRESS>
void Federate<ADDRESS>::reflectAttributeValues (
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
			rti1516e::FederateInternalError))
{
	this->reflectAttributeValues(theObject, theAttributeValues, theUserSuppliedTag, sentOrder, theType, theTime, receivedOrder, theReflectInfo);
}

// 6.13
template <typename ADDRESS>
void Federate<ADDRESS>::receiveInteraction (
		rti1516e::InteractionClassHandle theInteraction,
		rti1516e::ParameterHandleValueMap const & theParameterValues,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::TransportationType theType,
		rti1516e::SupplementalReceiveInfo theReceiveInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::receiveInteraction (
		rti1516e::InteractionClassHandle theInteraction,
		rti1516e::ParameterHandleValueMap const & theParameterValues,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::TransportationType theType,
		rti1516e::LogicalTime const & theTime,
		rti1516e::OrderType receivedOrder,
		rti1516e::SupplementalReceiveInfo theReceiveInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::receiveInteraction (
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
			rti1516e::FederateInternalError))
{
}

// 6.15
template <typename ADDRESS>
void Federate<ADDRESS>::removeObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::SupplementalRemoveInfo theRemoveInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::removeObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::LogicalTime const & theTime,
		rti1516e::OrderType receivedOrder,
		rti1516e::SupplementalRemoveInfo theRemoveInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::removeObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::VariableLengthData const & theUserSuppliedTag,
		rti1516e::OrderType sentOrder,
		rti1516e::LogicalTime const & theTime,
		rti1516e::OrderType receivedOrder,
		rti1516e::MessageRetractionHandle theHandle,
		rti1516e::SupplementalRemoveInfo theRemoveInfo)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.17
template <typename ADDRESS>
void Federate<ADDRESS>::attributesInScope (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.18
template <typename ADDRESS>
void Federate<ADDRESS>::attributesOutOfScope (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.20
template <typename ADDRESS>
void Federate<ADDRESS>::provideAttributeValueUpdate (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes,
		rti1516e::VariableLengthData const & theUserSuppliedTag)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
// 	provide_attribute_value_update_request_queue.Lock();
// 	provide_attribute_value_update_request_queue.Push(ProvideAttributeValueUpdateRequest(theObject, theAttributes, theUserSuppliedTag));
// 	provide_attribute_value_update_request_queue.Unlock();

	struct AttributeValueUpdater
	{
		Federate<ADDRESS>& federate;
		const rti1516e::ObjectInstanceHandle& object_instance_handle;
		const rti1516e::AttributeHandleSet& attribute_handle_set;
		const rti1516e::VariableLengthData& tag;
		
		AttributeValueUpdater(
			Federate<ADDRESS>& _federate,
			const rti1516e::ObjectInstanceHandle& _object_instance_handle,
			const rti1516e::AttributeHandleSet& _attribute_handle_set,
			const rti1516e::VariableLengthData& _tag
		)
			: federate(_federate)
			, object_instance_handle(_object_instance_handle)
			, attribute_handle_set(_attribute_handle_set)
			, tag(_tag)
		{
		}
		
		bool Visit(ObjectClass *object_class)
		{
			struct ObjectClassAttributeValueUpdater
			{
				AttributeValueUpdater& attribute_value_updater;
				
				ObjectClassAttributeValueUpdater(AttributeValueUpdater& _attribute_value_updater) : attribute_value_updater(_attribute_value_updater) {}
				
				bool Visit(ObjectInstance *object_instance)
				{
					if(object_instance->GetHandle() == attribute_value_updater.object_instance_handle)
					{
						struct ObjectInstanceAttributeValueUpdater
						{
							AttributeValueUpdater& attribute_value_updater;
							
							rti1516e::AttributeHandleValueMap attribute_handle_value_map;
							
							ObjectInstanceAttributeValueUpdater(AttributeValueUpdater& _attribute_value_updater)
								: attribute_value_updater(_attribute_value_updater)
								, attribute_handle_value_map()
							{
							}
							
							bool Visit(AttributeValue *attribute_value)
							{
								if(attribute_value_updater.attribute_handle_set.count(attribute_value->GetAttribute()->GetHandle()))
								{
									attribute_handle_value_map[attribute_value->GetAttribute()->GetHandle()] = attribute_value->Get();
								}
								
								return true;
							}
							
							void Do(ObjectInstance *object_instance)
							{
								Federate& federate = attribute_value_updater.federate;
								
								object_instance->ScanAttributeValues(*this);
								
								RTI_UNIQUE_PTR<rti1516e::HLAfloat64Time> hla_float64_time = attribute_value_updater.federate.hla_float64_time_factory.makeLogicalTime(sc_core::sc_time_stamp().to_seconds());
								const rti1516e::LogicalTime& logical_time = *(rti1516e::LogicalTime *) hla_float64_time.get();
								
								try
								{
									federate.rti_ambassador->updateAttributeValues(
										attribute_value_updater.object_instance_handle,
										attribute_handle_value_map,
										attribute_value_updater.tag,
										logical_time
									);
								}
								catch(rti1516e::Exception& e)
								{
									federate.logger << DebugError << "While updateAttributeValues, " << e.what() << EndDebugError;
									federate.Stop(-1);
								}
							}
						};
					}
					
					return true;
				}
				
				void Do(ObjectClass *object_class)
				{
					object_class->ScanObjectInstances(*this);
				}
			};
			return true;
		}
		
		void Do()
		{
			federate.registry.ScanObjectClasses(*this);
		}
	};
	
	AttributeValueUpdater attribute_value_updater(*this, theObject, theAttributes, theUserSuppliedTag);
	attribute_value_updater.Do();
}

// 6.21
template <typename ADDRESS>
void Federate<ADDRESS>::turnUpdatesOnForObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::turnUpdatesOnForObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes,
		std::wstring const & updateRateDesignator)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.22
template <typename ADDRESS>
void Federate<ADDRESS>::turnUpdatesOffForObjectInstance (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.24
template <typename ADDRESS>
void Federate<ADDRESS>::confirmAttributeTransportationTypeChange (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet theAttributes,
		rti1516e::TransportationType theTransportation)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.26
template <typename ADDRESS>
void Federate<ADDRESS>::reportAttributeTransportationType (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandle theAttribute,
		rti1516e::TransportationType theTransportation)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.28
template <typename ADDRESS>
void Federate<ADDRESS>::confirmInteractionTransportationTypeChange (
		rti1516e::InteractionClassHandle theInteraction,
		rti1516e::TransportationType theTransportation)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 6.30
template <typename ADDRESS>
void Federate<ADDRESS>::reportInteractionTransportationType (
		rti1516e::FederateHandle federateHandle,
		rti1516e::InteractionClassHandle theInteraction,
		rti1516e::TransportationType  theTransportation)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}


///////////////////////////////////
// Ownership Management Services //
///////////////////////////////////

// 7.4
template <typename ADDRESS>
void Federate<ADDRESS>::requestAttributeOwnershipAssumption (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & offeredAttributes,
		rti1516e::VariableLengthData const & theUserSuppliedTag)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 7.5
template <typename ADDRESS>
void Federate<ADDRESS>::requestDivestitureConfirmation (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & releasedAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 7.7
template <typename ADDRESS>
void Federate<ADDRESS>::attributeOwnershipAcquisitionNotification (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & securedAttributes,
		rti1516e::VariableLengthData const & theUserSuppliedTag)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 7.10
template <typename ADDRESS>
void Federate<ADDRESS>::attributeOwnershipUnavailable (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 7.11
template <typename ADDRESS>
void Federate<ADDRESS>::requestAttributeOwnershipRelease (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & candidateAttributes,
		rti1516e::VariableLengthData const & theUserSuppliedTag)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 7.16
template <typename ADDRESS>
void Federate<ADDRESS>::confirmAttributeOwnershipAcquisitionCancellation (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandleSet const & theAttributes)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

// 7.18
template <typename ADDRESS>
void Federate<ADDRESS>::informAttributeOwnership (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandle theAttribute,
		rti1516e::FederateHandle theOwner)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::attributeIsNotOwned (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandle theAttribute)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::attributeIsOwnedByRTI (
		rti1516e::ObjectInstanceHandle theObject,
		rti1516e::AttributeHandle theAttribute)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

//////////////////////////////
// Time Management Services //
//////////////////////////////

// 8.3
template <typename ADDRESS>
void Federate<ADDRESS>::timeRegulationEnabled (
		rti1516e::LogicalTime const & theFederateTime)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	time_regulation_enabled = true;
	if(verbose)
	{
		logger << DebugInfo << "Time regulation enabled" << EndDebugInfo;
	}
}

// 8.6
template <typename ADDRESS>
void Federate<ADDRESS>::timeConstrainedEnabled (
		rti1516e::LogicalTime const & theFederateTime)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	time_constained_enabled = true;
	if(verbose)
	{
		logger << DebugInfo << "Time contrained enabled" << EndDebugInfo;
	}
}

// 8.13
template <typename ADDRESS>
void Federate<ADDRESS>::timeAdvanceGrant (
		rti1516e::LogicalTime const & theTime)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
	double time_stamp = rti1516e::HLAfloat64Time(theTime).getTime();
	if(time_stamp >= sc_core::sc_time_stamp().to_seconds())
	{
		time_advance_granted = true;
	}
}

// 8.22
template <typename ADDRESS>
void Federate<ADDRESS>::requestRetraction (
		rti1516e::MessageRetractionHandle theHandle)
		RTI_THROW ((
			rti1516e::FederateInternalError))
{
}

template <typename ADDRESS>
void Federate<ADDRESS>::WaitTime(unsigned int msec)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(!Killed())
	{
		Sleep(msec);
	}
#else
	struct timespec tim_req, tim_rem;
	tim_req.tv_sec = msec / 1000;
	tim_req.tv_nsec = 1000000 * (msec % 1000);
	
	while(!Killed())
	{
		int status = nanosleep(&tim_req, &tim_rem);
		
		if(status == 0) break;
		
		if(status != -1) break;
		
		if(errno != EINTR) break;

		tim_req.tv_nsec = tim_rem.tv_nsec;
	}
#endif
}

template <typename ADDRESS>
bool Federate<ADDRESS>::HookConflicts(FederateHook<ADDRESS> *federate_hook) const
{
	for(typename FederateHooks::const_iterator it = federate_hooks.begin(); it != federate_hooks.end(); ++it)
	{
		if(((*it)->GetName() == federate_hook->GetName()) || (((*it)->GetSourceCodeLocation() == federate_hook->GetSourceCodeLocation()) && ((*it)->GetProcessorNumber() == federate_hook->GetProcessorNumber())))
		{
			return true;
		}
	}
	
	return false;
}

template <typename ADDRESS>
bool Federate<ADDRESS>::AddHook(FederateHook<ADDRESS> *federate_hook)
{
	if(HookConflicts(federate_hook)) return false;
	
	federate_hooks.push_back(federate_hook);
	
	return true;
}

template <typename ADDRESS>
bool Federate<ADDRESS>::StubConflicts(FederateStub<ADDRESS> *federate_stub) const
{
	for(typename FederateStubs::const_iterator it = federate_stubs.begin(); it != federate_stubs.end(); ++it)
	{
		if(((*it)->GetName() == federate_stub->GetName()) || ((::strcmp((*it)->GetSubProgram()->GetName(), federate_stub->GetSubProgram()->GetName()) == 0) && ((*it)->GetProcessorNumber() == federate_stub->GetProcessorNumber())))
		{
			return true;
		}
	}
	
	return false;
}

template <typename ADDRESS>
bool Federate<ADDRESS>::AddStub(FederateStub<ADDRESS> *federate_stub)
{
	if(StubConflicts(federate_stub)) return false;
	
	federate_stubs.push_back(federate_stub);
	
	return true;
}

template <typename ADDRESS>
void Federate<ADDRESS>::Yield()
{
	rti_ambassador->evokeMultipleCallbacks(0.001, std::numeric_limits<double>::infinity());
}

template <typename ADDRESS>
void Federate<ADDRESS>::Advance()
{
	RTI_UNIQUE_PTR<rti1516e::HLAfloat64Time> hla_float64_time = hla_float64_time_factory.makeLogicalTime(sc_core::sc_time_stamp().to_seconds());
	const rti1516e::LogicalTime& theTime = *(rti1516e::LogicalTime *) hla_float64_time.get();
	time_advance_granted = false;
	try
	{
		rti_ambassador->timeAdvanceRequest(theTime);
		do
		{
			Yield();
		}
		while(!time_advance_granted && !Killed());
	}
	catch(rti1516e::Exception& e)
	{
		logger << DebugError << e.what() << EndDebugError;
		this->Stop(-1);
	}
}

template <typename ADDRESS>
FederateHookStub<ADDRESS>::FederateHookStub(Federate<ADDRESS>& _federate, const std::string& _name, const unisim::util::json::JSON_Value *_json_return_value)
	: FederateHookStubBase(_name, _json_return_value)
	, federate(_federate)
	, attribute_value_bindings()
{
}

template <typename ADDRESS>
void FederateHookStub<ADDRESS>::Bind(AttributeValue *attribute_value)
{
	attribute_value_bindings.push_back(AttributeValueBinding(attribute_value, unisim::util::debug::DataObjectRef<ADDRESS>()));
}

template <typename ADDRESS>
void FederateHookStub<ADDRESS>::WaitDependencies()
{
	// build lists of attribute value per object instance
	typedef std::vector<AttributeValue *> AttributeValues;
	typedef std::map<const ObjectInstance *, AttributeValues > AttributeValuesPerObjectInstance;
	AttributeValuesPerObjectInstance attribute_values_per_object_instance;
	
	for(typename AttributeValueBindings::iterator it = attribute_value_bindings.begin(); it != attribute_value_bindings.end(); ++it)
	{
		AttributeValue *attribute_value = (*it).first;
		const Instrument& instrument = attribute_value->GetInstrument();
		if((instrument.GetDirection() == Instrument::WRITE) && (attribute_value->TimeStamp() < sc_core::sc_time_stamp()))
		{
			attribute_value->Invalidate();
			attribute_value->AddSensitiveHookStub(this);
			const ObjectInstance *object_instance = attribute_value->GetObjectInstance();
			attribute_values_per_object_instance[object_instance].push_back(attribute_value);
		}
	}
	
	// for each object instance
	for(typename AttributeValuesPerObjectInstance::const_iterator it = attribute_values_per_object_instance.begin(); it != attribute_values_per_object_instance.end(); ++it)
	{
		const ObjectInstance *object_instance = (*it).first;
		const AttributeValues& attribute_values = (*it).second;
		
		// build a set of attribute handle
		rti1516e::AttributeHandleSet attribute_handle_set;
		for(typename AttributeValues::const_iterator it = attribute_values.begin(); it != attribute_values.end(); ++it)
		{
			AttributeValue *attribute_value = *it;
			attribute_handle_set.insert(attribute_value->GetAttribute()->GetHandle());
		}
		
		// request the set of attribute value
		try
		{
			this->federate.rti_ambassador->requestAttributeValueUpdate(object_instance->GetHandle(), attribute_handle_set, rti1516e::VariableLengthData());
		}
		catch(rti1516e::Exception& e)
		{
			federate.logger << DebugError << "While requestAttributeValueUpdate, " << e.what() << EndDebugError;
			federate.Stop(-1);
			return;
		}
	}
	
	do
	{
		// yield to RTI ambassador, which will hopefully call reflectAttributeValues callback, and then Ready() may return true.
		federate.Yield();
	}
	while(!this->Ready() && !federate.Killed());
}

template <typename ADDRESS>
void FederateHookStub<ADDRESS>::DoInstrument(Instrument::Direction direction)
{
	for(typename AttributeValueBindings::iterator it = attribute_value_bindings.begin(); it != attribute_value_bindings.end(); ++it)
	{
		AttributeValue *attribute_value = (*it).first;
		const Instrument& instrument = attribute_value->GetInstrument();
		if(instrument.GetDirection() == direction)
		{
			unisim::util::debug::DataObjectRef<ADDRESS> data_object = (*it).second;
			
			if(data_object.IsUndefined())
			{
				data_object = (*federate.data_object_lookup_import)[instrument.GetExpression()];
				(*it).second = data_object;
			}
			
			if(data_object.Exists())
			{
				ADDRESS data_object_bit_size = data_object.GetBitSize();
				ADDRESS data_object_byte_size = (data_object_bit_size + 7 ) / 8;
				switch(direction)
				{
					case Instrument::READ:
					{
						uint8_t data_object_raw_value[data_object_byte_size];
						if(data_object.Read(0, data_object_raw_value, 0, data_object_bit_size))
						{
							attribute_value->SetData(data_object_raw_value, data_object_byte_size, sc_core::sc_time_stamp());
						}
						else
						{
							federate.logger << DebugWarning << "Can't read " << data_object.GetName() << EndDebugWarning;
						}
						break;
					}
					case Instrument::WRITE:
					{
						if(attribute_value->TimeStamp() <= sc_core::sc_time_stamp())
						{
							const rti1516e::VariableLengthData& value = attribute_value->Get();
							size_t sz = value.size();
							if(data_object_byte_size < sz) sz = data_object_byte_size;
							uint8_t data_object_raw_value[data_object_byte_size] = {};
							::memcpy(data_object_raw_value, value.data(), sz);
							if(!data_object.Write(0, data_object_raw_value, 0, data_object_bit_size))
							{
								federate.logger << DebugWarning << "Can't write " << data_object.GetName() << EndDebugWarning;
							}
						}
						break;
					}
				}
			}
		}
	}
}

template <typename ADDRESS>
FederateHook<ADDRESS>::FederateHook(Federate<ADDRESS>& _federate, const std::string& _name, const unisim::util::debug::SourceCodeLocation& _source_code_location, const unisim::util::json::JSON_Value *_json_return_value)
	: unisim::util::debug::SourceCodeHook<ADDRESS>(_source_code_location)
	, FederateHookStub<ADDRESS>(_federate, _name, _json_return_value)
{
}

template <typename ADDRESS>
bool FederateHook<ADDRESS>::Run(typename unisim::util::debug::Hook<ADDRESS>::ReturnValue& return_value)
{
	// program variables -> attribute values
	this->DoInstrument(Instrument::READ);
	
	// wait for attribute value that the hook depends on to write the program variables
	this->WaitDependencies();
	
	// program variables <- attribute values
	this->DoInstrument(Instrument::WRITE);
	
	this->federate.Advance();
	
	const unisim::util::json::JSON_Value *json_return_value = this->GetReturn();
	
	if(json_return_value)
	{
		if(json_return_value->IsNull())
		{
			unisim::util::debug::JSON2DataObject<ADDRESS> json_to_data_object(this->federate.data_object_lookup_import);
			json_to_data_object.Do(json_return_value, return_value);
		}
		return true;
	}
	
	return false;
}

template <typename ADDRESS>
FederateStub<ADDRESS>::FederateStub(Federate<ADDRESS>& _federate, const std::string& _name, const typename unisim::util::debug::SubProgram<ADDRESS> *_subprogram, const unisim::util::json::JSON_Value *_json_return_value)
	: unisim::util::debug::Stub<ADDRESS>(_subprogram)
	, FederateHookStub<ADDRESS>(_federate, _name, _json_return_value)
{
}

template <typename ADDRESS>
bool FederateStub<ADDRESS>::Run(typename unisim::util::debug::Stub<ADDRESS>::Parameters& parameters, typename unisim::util::debug::Stub<ADDRESS>::ReturnValue& return_value)
{
	// program variables -> attribute values
	this->DoInstrument(Instrument::READ);
	
	// wait for attribute value that the hook depends on to write the program variables
	this->WaitDependencies();
	
	// program variables <- attribute values
	this->DoInstrument(Instrument::WRITE);
	
	this->federate.Advance();
	
	const unisim::util::json::JSON_Value *json_return_value = this->GetReturn();
	
	if(json_return_value)
	{
		if(json_return_value->IsNull())
		{
			unisim::util::debug::JSON2DataObject<ADDRESS> json_to_data_object(this->federate.data_object_lookup_import);
			json_to_data_object.Do(json_return_value, return_value);
		}
		return true;
	}
	
	return false;
}

template <typename VISITOR>
void ObjectInstance::ScanAttributeValues(VISITOR& visitor) const
{
	for(typename AttributeValues::const_iterator it = attribute_values.begin(); it != attribute_values.end(); ++it)
	{
		AttributeValue *attribute_value = (*it).second;
		if(!visitor.Visit(attribute_value)) break;
	}
}

template <typename VISITOR>
void ObjectClass::ScanAttributes(VISITOR& visitor) const
{
	for(typename Attributes::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
	{
		Attribute *attribute = (*it).second;
		if(!visitor.Visit(attribute)) break;
	}
}

template <typename VISITOR>
void ObjectClass::ScanObjectInstances(VISITOR& visitor) const
{
	for(typename ObjectInstances::const_iterator it = object_instances.begin(); it != object_instances.end(); ++it)
	{
		ObjectInstance *object_instance = (*it).second;
		if(!visitor.Visit(object_instance)) break;
	}
}

template <typename VISITOR>
void Registry::ScanObjectClasses(VISITOR& visitor) const
{
	for(typename ObjectClasses::const_iterator it = object_classes.begin(); it != object_classes.end(); ++it)
	{
		ObjectClass *object_class = (*it).second;
		if(!visitor.Visit(object_class)) break;
	}
}

template <typename VISITOR>
void Registry::ScanHooks(VISITOR& visitor) const
{
	for(typename Hooks::const_iterator it = hooks.begin(); it != hooks.end(); ++it)
	{
		Hook *hook = (*it).second;
		if(!visitor.Visit(hook)) break;
	}
}

template <typename VISITOR>
void Registry::ScanStubs(VISITOR& visitor) const
{
	for(typename Stubs::const_iterator it = stubs.begin(); it != stubs.end(); ++it)
	{
		Stub *stub = (*it).second;
		if(!visitor.Visit(stub)) break;
	}
}

template <typename ADDRESS>
PublisherSubscriber<ADDRESS>::PublisherSubscriber(Federate<ADDRESS>& _federate, Mode _mode, Action _action)
	: federate(_federate)
	, mode(_mode)
	, action(_action)
{
}
	
template <typename ADDRESS>
void PublisherSubscriber<ADDRESS>::Do()
{
	federate.registry.ScanObjectClasses(*this);
}

template <typename ADDRESS>
bool PublisherSubscriber<ADDRESS>::Visit(ObjectClass *object_class)
{
	struct ObjectClassPublisherSubscriber
	{
		Federate<ADDRESS>& federate;
		Mode mode;
		Action action;
		typedef std::vector<Attribute *> Attributes;
		Attributes attributes;
		
		ObjectClassPublisherSubscriber(Federate<ADDRESS>& _federate, Mode _mode, Action _action) : federate(_federate), mode(_mode), action(_action), attributes() {}
		
		bool Visit(Attribute *attribute)
		{
			bool select = false;
			
			switch(mode)
			{
				case PUBLISH: select = ((action == SET) && attribute->Publishable()) || ((action == UNSET) && attribute->Published()); break;
				case SUBSCRIBE: select = ((action == SET) && attribute->Subscribable()) || ((action == UNSET) && attribute->Subscribed()); break;
			}
			if(select)
			{
				if(federate.verbose)
				{
					federate.logger << DebugInfo;
					switch(mode)
					{
						case PUBLISH:
							switch(action)
							{
								case SET: federate.logger << "Publishing"; break;
								case UNSET: federate.logger << "Unpublishing"; break;
							}
							break;
						case SUBSCRIBE:
							switch(action)
							{
								case SET: federate.logger << "Subscribing to"; break;
								case UNSET: federate.logger << "Unsubscribing to"; break;
							}
							break;
					}
					federate.logger << " Attribute " << attribute->GetName() << " of Class " << attribute->GetObjectClass()->GetName() << EndDebugInfo;
				}
				attributes.push_back(attribute);
			}
			
			return true;
		}
		
		void Do(ObjectClass *object_class)
		{
			object_class->ScanAttributes(*this);
			
			if(attributes.size() != 0)
			{
				rti1516e::AttributeHandleSet attributes_handles;
				for(Attributes::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
				{
					Attribute *attribute = *it;
					attributes_handles.insert(attribute->GetHandle());
				}
				
				try
				{
					switch(mode)
					{
						case PUBLISH:
							switch(action)
							{
								case SET: federate.rti_ambassador->publishObjectClassAttributes(object_class->GetHandle(), attributes_handles); break;
								case UNSET: federate.rti_ambassador->unpublishObjectClassAttributes(object_class->GetHandle(), attributes_handles); break;
							}
							break;
						case SUBSCRIBE:
							switch(action)
							{
								case SET: federate.rti_ambassador->subscribeObjectClassAttributes(object_class->GetHandle(), attributes_handles); break;
								case UNSET: federate.rti_ambassador->unsubscribeObjectClassAttributes(object_class->GetHandle(), attributes_handles); break;
							}
							break;
					}
					
					for(Attributes::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
					{
						Attribute *attribute = *it;
						switch(mode)
						{
							case PUBLISH:
								switch(action)
								{
									case SET: attribute->Publish(); break;
									case UNSET: attribute->Publish(false); break;
								}
								break;
							case SUBSCRIBE:
								switch(action)
								{
									case SET: attribute->Subscribe(); break;
									case UNSET: attribute->Subscribe(false); break;
								}
								break;
						}
					}
				}
				catch(rti1516e::Exception& e)
				{
					federate.logger << DebugError << e.what() << EndDebugError;
				}
			}
		}
	};
	
	ObjectClassPublisherSubscriber object_class_publisher_subscriber(federate, mode, action);
	
	object_class_publisher_subscriber.Do(object_class);
	
	return true;
}

template <typename ADDRESS>
ObjectInstanceReserver<ADDRESS>::ObjectInstanceReserver(Federate<ADDRESS>& _federate)
	: federate(_federate)
{
}

template <typename ADDRESS>
void ObjectInstanceReserver<ADDRESS>::Do()
{
	federate.registry.ScanObjectClasses(*this);
	federate.rti_ambassador->evokeMultipleCallbacks(0.1, std::numeric_limits<double>::infinity());
}

template <typename ADDRESS>
bool ObjectInstanceReserver<ADDRESS>::Visit(ObjectClass *object_class)
{
	struct IsPublishedObjectClass
	{
		ObjectClass *object_class;
		bool result;
		
		IsPublishedObjectClass(ObjectClass *_object_class) : object_class(_object_class), result(false) {}
		
		bool Visit(Attribute *attribute)
		{
			if(attribute->Published())
			{
				result = true;
				return false;
			}
			return true;
		}
		
		bool Test()
		{
			object_class->ScanAttributes(*this);
			return result;
		}
	};
	
	
	struct ObjectClassInstanceReserver
	{
		Federate<ADDRESS>& federate;
		ObjectClass *object_class;
		
		ObjectClassInstanceReserver(Federate<ADDRESS>& _federate, ObjectClass *_object_class) : federate(_federate), object_class(_object_class) {}
		
		bool Visit(ObjectInstance *object_instance)
		{
			try
			{
				if(federate.verbose)
				{
					federate.logger << DebugInfo << "Reserving Object instance name \"" << object_instance->GetName() << "\"" << EndDebugInfo;
				}
				federate.rti_ambassador->reserveObjectInstanceName(object_instance->GetName());
			}
			catch(rti1516e::Exception& e)
			{
				federate.logger << DebugError << e.what() << EndDebugError;
			}
			
			return true;
		}
	};
	
	IsPublishedObjectClass is_published_object_class(object_class);
	if(is_published_object_class.Test())
	{
		ObjectClassInstanceReserver object_class_instance_reserver(federate, object_class);
		object_class->ScanObjectInstances(object_class_instance_reserver);
	}
	
	return true;
}

template <typename ADDRESS>
ObjectInstanceRegisterer<ADDRESS>::ObjectInstanceRegisterer(Federate<ADDRESS>& _federate, const std::wstring& _object_instance_name)
	: federate(_federate)
	, object_instance_name(_object_instance_name)
{
}

template <typename ADDRESS>
void ObjectInstanceRegisterer<ADDRESS>::Do()
{
	federate.registry.ScanObjectClasses(*this);
}

template <typename ADDRESS>
bool ObjectInstanceRegisterer<ADDRESS>::Visit(ObjectClass *object_class)
{
	struct ObjectClassInstanceRegisterer
	{
		ObjectInstanceRegisterer<ADDRESS>& object_instance_registerer;
		ObjectClass *object_class;
		
		ObjectClassInstanceRegisterer(ObjectInstanceRegisterer<ADDRESS>& _object_instance_registerer, ObjectClass *_object_class) : object_instance_registerer(_object_instance_registerer), object_class(_object_class) {}
		
		bool Visit(ObjectInstance *object_instance)
		{
			const std::wstring& object_instance_name = object_instance_registerer.object_instance_name;
			
			if(object_instance->GetName() == object_instance_name)
			{
				Federate<ADDRESS>& federate = object_instance_registerer.federate;
				
				try
				{
					if(federate.verbose)
					{
						federate.logger << DebugInfo << "Registering Object instance \"" << object_instance->GetName() << "\" of Class " << object_instance->GetObjectClass()->GetName() << EndDebugInfo;
					}
					rti1516e::ObjectInstanceHandle object_instance_handle = federate.rti_ambassador->registerObjectInstance(object_class->GetHandle(), object_instance->GetName());
					if(federate.verbose)
					{
						federate.logger << DebugInfo << "Got handle " << object_instance_handle.toString() << EndDebugInfo;
					}
					object_instance->Bind(object_instance_handle);
				}
				catch(rti1516e::Exception& e)
				{
					federate.logger << DebugError << e.what() << EndDebugError;
					federate.Stop(-1);
				}
			}
			
			return true;
		}
	};
	
	ObjectClassInstanceRegisterer object_class_instance_registerer(*this, object_class);
	object_class->ScanObjectInstances(object_class_instance_registerer);
	
	return true;
}

template <typename ADDRESS>
ObjectInstanceDiscoverer<ADDRESS>::ObjectInstanceDiscoverer(Federate<ADDRESS>& _federate, const rti1516e::ObjectInstanceHandle& _object_instance_handle, const rti1516e::ObjectClassHandle& _object_class_handle, const std::wstring& _object_instance_name)
	: federate(_federate)
	, object_instance_handle(_object_instance_handle)
	, object_class_handle(_object_class_handle)
	, object_instance_name(_object_instance_name)
{
}

template <typename ADDRESS>
void ObjectInstanceDiscoverer<ADDRESS>::Do()
{
	federate.registry.ScanObjectClasses(*this);
}

template <typename ADDRESS>
bool ObjectInstanceDiscoverer<ADDRESS>::Visit(ObjectClass *object_class)
{
	struct ObjectClassInstanceDiscoverer
	{
		ObjectInstanceDiscoverer& object_instance_discoverer;
		ObjectClass *object_class;
		
		ObjectClassInstanceDiscoverer(ObjectInstanceDiscoverer& _object_instance_discoverer, ObjectClass *_object_class)
			: object_instance_discoverer(_object_instance_discoverer)
			, object_class(_object_class)
		{
		}
		
		void Do()
		{
			object_class->ScanObjectInstances(*this);
		}
		
		bool Visit(ObjectInstance *object_instance)
		{
			if(object_instance->GetName() == object_instance_discoverer.object_instance_name)
			{
				Federate<ADDRESS>& federate = object_instance_discoverer.federate;
				
				if(federate.verbose)
				{
					federate.logger << DebugInfo << "Discovered Object instance \"" << object_instance->GetName() << "\"" << EndDebugInfo;
					federate.logger << DebugInfo << "Got handle " << object_instance_discoverer.object_instance_handle.toString() << EndDebugInfo;
				}
				object_instance->Bind(object_instance_discoverer.object_instance_handle);
			}
			return true;
		}
	};
	
	if(object_class->GetHandle() == object_class_handle)
	{
		ObjectClassInstanceDiscoverer object_class_instance_discoverer(*this, object_class);
		object_class_instance_discoverer.Do();
	}
	
	return true;
}

template <typename ADDRESS>
ObjectInstancesDependencyChecker<ADDRESS>::ObjectInstancesDependencyChecker(Federate<ADDRESS>& _federate)
	: federate(_federate)
	, status(false)
{
}

template <typename ADDRESS>
bool ObjectInstancesDependencyChecker<ADDRESS>::Check()
{
	federate.registry.ScanObjectClasses(*this);
	return status;
}

template <typename ADDRESS>
bool ObjectInstancesDependencyChecker<ADDRESS>::Visit(ObjectClass *object_class)
{
	struct ObjectClassVisitor
	{
		ObjectInstancesDependencyChecker& object_instances_dependency_checker;
		ObjectClass *object_class;
		
		ObjectClassVisitor(ObjectInstancesDependencyChecker& _object_instances_dependency_checker, ObjectClass *_object_class)
			: object_instances_dependency_checker(_object_instances_dependency_checker)
			, object_class(_object_class)
		{
		}
		
		bool Visit(ObjectInstance *object_instance)
		{
			struct ObjectInstanceVisitor
			{
				ObjectInstancesDependencyChecker& object_instances_dependency_checker;
				ObjectInstance *object_instance;
				
				ObjectInstanceVisitor(ObjectInstancesDependencyChecker& _object_instances_dependency_checker, ObjectInstance *_object_instance)
					: object_instances_dependency_checker(_object_instances_dependency_checker)
					, object_instance(_object_instance)
				{
				}
				
				bool Visit(AttributeValue *attribute_value)
				{
					const Instrument& instrument = attribute_value->GetInstrument();
					
					if((instrument.GetDirection() == Instrument::READ) && !attribute_value->GetAttribute()->GetHandle().isValid())
					{
						object_instances_dependency_checker.status = false;
						return false;
					}
					
					return true;
				}
				
				void Do()
				{
					object_instance->ScanAttributeValues(*this);
				}
			};
			
			ObjectInstanceVisitor object_instance_visitor(object_instances_dependency_checker, object_instance);
			object_instance_visitor.Do();
			return object_instances_dependency_checker.status;
		}
		
		void Do()
		{
			object_class->ScanObjectInstances(*this);
		}
	};
	
	ObjectClassVisitor object_class_visitor(*this, object_class);
	object_class_visitor.Do();
	
	return status;
}

} // end of namespace hla
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_TCC__
