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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_HLA_RTI1516E

#include <unisim/service/debug/hla/federate.hh>
#include <unisim/util/debug/data_object.tcc>
#include <unisim/util/hla/hla.tcc>
#include <climits>
#include <fstream>

namespace unisim {
namespace service {
namespace debug {
namespace hla {

///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////// Federate<> //////////////////////////////////

template <typename CONFIG>
Federate<CONFIG>::Federate(const char *_name, unisim::kernel::Object *_parent)
	: unisim::kernel::Object(_name, _parent, "this service implements an HLA Federate")
	, unisim::util::hla::Federate<typename CONFIG::TIME_TRAIT>()
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
	, unisim::kernel::Client<unisim::service::interfaces::StackFrame<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Stubbing<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Hooking<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugTiming<sc_core::sc_time> >(_name, _parent)
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
	, stack_frame_import("backtrace-import", this)
	, profiling_import("profiling-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, data_object_lookup_import("data-object-lookup-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, stubbing_import("stubbing-import", this)
	, hooking_import("hooking-import", this)
	, debug_timing_import("debug-timing-import", this)
	, logger(*this)
	, p_config(0)
	, prc_num(-1)
	, config_file()
	, param_config_file("config-file", this, config_file, "Configuration file")
	, param_verbose("verbose", this, this->verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, this->debug, "Enable/Disable debug (intended for developper)")
	, object_classes()
	, object_instances()
	, federate_hooks()
	, federate_stubs()
{
	this->SetDebugInfoStream(logger.DebugInfoWStream());
	this->SetDebugWarningStream(logger.DebugWarningWStream());
	this->SetDebugErrorStream(logger.DebugErrorWStream());
}

template <typename CONFIG>
Federate<CONFIG>::~Federate()
{
	EndOfSimulation();
}

template <typename CONFIG>
void Federate<CONFIG>::EndOfSimulation()
{
	for(typename FederateHooks::const_iterator it = federate_hooks.begin(); it != federate_hooks.end(); ++it)
	{
		FederateHook<CONFIG> *federate_hook = *it;
		if(this->verbose)
		{
			logger << DebugInfo << "Removing hook at " << federate_hook->GetSourceCodeLocation() << EndDebugInfo;
		}
		if(!hooking_import->RemoveHook(federate_hook))
		{
			logger << DebugWarning << "Can't remove hook at " << federate_hook->GetSourceCodeLocation() << EndDebugWarning;
		}
	}
	federate_hooks.clear();
	
	for(typename FederateStubs::const_iterator it = federate_stubs.begin(); it != federate_stubs.end(); ++it)
	{
		FederateStub<CONFIG> *federate_stub = *it;
		if(this->verbose)
		{
			logger << DebugInfo << "Removing stub at " << federate_stub->GetSubProgram()->GetName() << EndDebugInfo;
		}
		if(!stubbing_import->RemoveStub(federate_stub))
		{
			logger << DebugWarning << "Can't remove stub at " << federate_stub->GetSubProgram()->GetName() << EndDebugWarning;
		}
	}
	federate_stubs.clear();
	
	for(typename ObjectInstances::const_iterator it = object_instances.begin(); it != object_instances.end(); ++it)
	{
		unisim::util::hla::ObjectInstance *object_instance = *it;
		delete object_instance;
	}
	object_instances.clear();
	
	for(typename ObjectClasses::const_iterator it = object_classes.begin(); it != object_classes.end(); ++it)
	{
		ObjectClass *object_class = *it;
		delete object_class;
	}
	object_classes.clear();
	
	if(p_config) delete p_config;
	p_config = 0;
}

// unisim::service::interfaces::DebugYielding
template <typename CONFIG>
void Federate<CONFIG>::DebugYield()
{
}

// unisim::service::interfaces::DebugYielding
template <typename CONFIG>
void Federate<CONFIG>::DebugYield()
{
}

// unisim::service::interfaces::DebugEventListener<ADDRESS>
template <typename CONFIG>
void Federate<CONFIG>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
}

// unisim::kernel::Object
template <typename CONFIG>
bool Federate<CONFIG>::BeginSetup()
{
	if(config_file.empty())
	{
		logger << DebugError << param_config_file.GetDescription() << " is unset" << EndDebugError;
		return false;
	}
	
	if(this->verbose)
	{
		logger << DebugInfo << "Opening File \"" << config_file << "\"" << EndDebugInfo;
	}
	
	std::ifstream stream(config_file.c_str());
	
	if(stream.fail())
	{
		logger << DebugError << "Can't open File \"" << config_file << "\"" << EndDebugError;
		return false;
	}
	
	unisim::util::json::JSON_Parser<unisim::util::json::JSON_AST_Builder> json_parser;
	unisim::util::json::JSON_AST_Builder json_ast_builder;
	
	p_config = json_parser.Parse(stream, json_ast_builder);
	
	if(!p_config) return false;
	
	return true;
}

// unisim::kernel::Object
template <typename CONFIG>
bool Federate<CONFIG>::EndSetup()
{
	if(!p_config) return false;
	
	try
	{
		const unisim::util::json::JSON_Object& config = p_config->AsObject();
		
		if(config.HasProperty("processor"))
		{
			prc_num = config["processor"].AsInteger();
			int sel_prc_num = debug_selecting_import->GetSelectedProcessor();
			if(sel_prc_num != prc_num)
			{
				if(this->verbose)
				{
					logger << DebugInfo << "Selecting processor #" << prc_num << " instead of processor #" << sel_prc_num << EndDebugInfo;
				}
				if(!debug_selecting_import->SelectProcessor(prc_num))
				{
					logger << DebugWarning << "Can't select processor #" << prc_num << EndDebugWarning;
					return false;
				}
			}
		}
		else
		{
			prc_num = debug_selecting_import->GetSelectedProcessor();
		}
		
		std::wstring url = config["url"].AsString();
		std::wstring federation_execution_name = config["federateExecutionName"].AsString();
		std::wstring fom_module = config["fomModule"].AsString();
		std::wstring federate_name = config["federateName"].AsString();
		std::wstring federate_type = config["federateType"].AsString();
		
		this->SetURL(url);
		this->SetFederationExecutionName(federation_execution_name);
		this->SetFOMModule(fom_module);
		this->SetFederateName(federate_name);
		this->SetFederateType(federate_type);
		
		if(config.HasProperty("lookahead"))
		{
			sc_core::sc_time lookahead;
			const unisim::util::json::JSON_String& lookahead_config = config["lookahead"].AsString();
			lookahead = Super::TIME_ADAPTER::FromString((const std::string&) lookahead_config);
			if(Super::TIME_ADAPTER::IsNull(lookahead))
			{
				logger << DebugWarning << "In File \"" << config_file << "\", at " << lookahead_config.GetLocation() << ", ignoring zero lookahead" << EndDebugWarning;
			}
			else
			{
				this->SetLookahead(lookahead);
			}
		}
		
		if(config.HasProperty("hooks"))
		{
			const unisim::util::json::JSON_Object& hooks_config = config["hooks"].AsObject();
			unisim::util::json::JSON_Object::KeysType hook_names = hooks_config.Keys();
			for(unisim::util::json::JSON_Object::KeysType::const_iterator hook_name_it = hook_names.begin(); hook_name_it != hook_names.end(); ++hook_name_it)
			{
				const std::string& hook_name = *hook_name_it;
				const unisim::util::json::JSON_Object& hook_config = hooks_config[hook_name].AsObject();
				
				const std::string& loc = hook_config["loc"].AsString();
				const std::string& file = hook_config["file"].AsString();
				const unisim::util::json::JSON_Value *json_return_value = hook_config.HasProperty("return") ? hook_config["return"].Clone() : 0;
				
				unisim::util::debug::SourceCodeLocation source_code_location;
				
				if(source_code_location.Parse(loc.c_str()))
				{
					FederateHook<CONFIG> *federate_hook = new FederateHook<CONFIG>(*this, hook_name, file, source_code_location, json_return_value);
					if(!this->SetHook(federate_hook))
					{
						delete federate_hook;
						return false;
					}
				}
				else
				{
					logger << DebugError << "In File \"" << config_file << "\", at " << hooks_config.GetLocation() << ", \"" << loc << "\" is not a source code location" << EndDebugError;
					return false;
				}
			}
		}
		
		if(config.HasProperty("stubs"))
		{
			const unisim::util::json::JSON_Object& stubs_config = config["stubs"].AsObject();
			unisim::util::json::JSON_Object::KeysType stub_names = stubs_config.Keys();
			for(unisim::util::json::JSON_Object::KeysType::const_iterator stub_name_it = stub_names.begin(); stub_name_it != stub_names.end(); ++stub_name_it)
			{
				const std::string& stub_name = *stub_name_it;
				const unisim::util::json::JSON_Object& stub_config = stubs_config[stub_name].AsObject();
				
				const std::string& loc = stub_config["loc"].AsString();
				const unisim::util::json::JSON_Value *json_return_value = stub_config.HasProperty("return") ? stub_config["return"].Clone() : 0;
				
				const unisim::util::debug::SubProgram<ADDRESS> *subprogram = this->subprogram_lookup_import->FindSubProgram(loc.c_str());
				
				if(subprogram)
				{
					FederateStub<CONFIG> *federate_stub = new FederateStub<CONFIG>(*this, stub_name, subprogram, json_return_value);
					if(!this->SetStub(federate_stub))
					{
						delete federate_stub;
						return false;
					}
				}
				else
				{
					logger << DebugError << "In File \"" << config_file << "\", at " << stub_config.GetLocation() << ", Subprogram \"" << loc << "\" not found" << EndDebugError;
					return false;
				}
			}
		}
		
		if(config.HasProperty("objectClasses"))
		{
			const unisim::util::json::JSON_Object& object_classes_config = config["objectClasses"].AsObject();
			
			unisim::util::json::JSON_Object::KeysType object_class_names = object_classes_config.Keys();
			for(unisim::util::json::JSON_Object::KeysType::const_iterator object_class_name_it = object_class_names.begin(); object_class_name_it != object_class_names.end(); ++object_class_name_it)
			{
				const std::string& object_class_name = *object_class_name_it;
				const unisim::util::json::JSON_Object& object_class_config = object_classes_config[object_class_name].AsObject();
				
				ObjectClass *object_class = new ObjectClass(*this, object_class_name);
				object_classes.push_back(object_class);
				
				if(object_class_config.HasProperty("attributes"))
				{
					const unisim::util::json::JSON_Object& attributes_config = object_class_config["attributes"].AsObject();
					unisim::util::json::JSON_Object::KeysType attribute_names = attributes_config.Keys();
					if(attribute_names.size())
					{
						for(unisim::util::json::JSON_Object::KeysType::const_iterator attribute_name_it = attribute_names.begin(); attribute_name_it != attribute_names.end(); ++attribute_name_it)
						{
							const std::string& attribute_name = *attribute_name_it;
							
							const unisim::util::json::JSON_Object& attribute_config = attributes_config[attribute_name].AsObject();
							
							const unisim::util::json::JSON_String& attribute_type = attribute_config["type"].AsString();
							const std::string& attribute_type_value = attribute_type;
							bool publish = attribute_config.HasProperty("publish") && attribute_config["publish"].AsBoolean();
							bool subscribe = attribute_config.HasProperty("subscribe") && attribute_config["subscribe"].AsBoolean();
							
							unisim::util::hla::AttributeBase *attribute = 0;
							if(attribute_type_value == "int8") attribute = new unisim::util::hla::Attribute<int8_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "int16") attribute = new unisim::util::hla::Attribute<int16_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "int32") attribute = new unisim::util::hla::Attribute<int32_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "int64") attribute = new unisim::util::hla::Attribute<int64_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "uint8") attribute = new unisim::util::hla::Attribute<uint8_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "uint16") attribute = new unisim::util::hla::Attribute<uint16_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "uint32") attribute = new unisim::util::hla::Attribute<uint32_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "uint64") attribute = new unisim::util::hla::Attribute<uint64_t>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "float") attribute = new unisim::util::hla::Attribute<float>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "double") attribute = new unisim::util::hla::Attribute<double>(*object_class, attribute_name, attribute_type_value);
							else if(attribute_type_value == "boolean") attribute = new unisim::util::hla::Attribute<bool>(*object_class, attribute_name, attribute_type_value);
							else
							{
								logger << DebugError << "In File \"" << config_file << "\", at " << attribute_type.GetLocation() << ", unknown Type \"" << attribute_type_value << "\"" << EndDebugError;
								return false;
							}
							
							if(publish) attribute->Publish();
							if(subscribe) attribute->Subscribe();
							if(!publish && !subscribe)
							{
								logger << DebugWarning << "In File \"" << config_file << "\", at " << attribute_config.GetLocation() << ", Attribute \"" << attribute->GetName() << "\" of Object class \"" << object_class->GetName() << "\" is neither published or subscribed" << EndDebugWarning;
							}
						}
					}
					else
					{
						logger << DebugWarning << "In File \"" << config_file << "\", at " << object_class_config.GetLocation() << ", Object class \"" << object_class->GetName() << "\" has no attributes" << EndDebugWarning;
					}
					
					if(object_class_config.HasProperty("objectInstances"))
					{
						const unisim::util::json::JSON_Object& object_instances_config = object_class_config["objectInstances"].AsObject();
						unisim::util::json::JSON_Object::KeysType object_instance_names = object_instances_config.Keys();
						
						if(object_instance_names.size())
						{
							for(unisim::util::json::JSON_Object::KeysType::const_iterator object_instance_name_it = object_instance_names.begin(); object_instance_name_it != object_instance_names.end(); ++object_instance_name_it)
							{
								const std::string& object_instance_name = *object_instance_name_it;
								
								const unisim::util::json::JSON_Object& object_instance_config = object_instances_config[object_instance_name].AsObject();
								
								unisim::util::hla::ObjectInstance *object_instance = new unisim::util::hla::ObjectInstance(*object_class, object_instance_name);
								object_instances.push_back(object_instance);
								
								if(object_instance_config.HasProperty("instruments"))
								{
									const unisim::util::json::JSON_Object& instruments = object_instance_config["instruments"].AsObject();
									
									unisim::util::json::JSON_Object::KeysType instrument_names = instruments.Keys();
									for(unisim::util::json::JSON_Object::KeysType::const_iterator instrument_name_it = instrument_names.begin(); instrument_name_it != instrument_names.end(); ++instrument_name_it)
									{
										const std::string& instrument_name = *instrument_name_it;
										
										unisim::util::hla::AttributeBase& attribute = object_class->GetAttribute(instrument_name);
										
										const unisim::util::json::JSON_Object& instrument_config = instruments[instrument_name].AsObject();
										const std::string& expr = instrument_config["expr"].AsString();
										bool read = instrument_config.HasProperty("read") ? bool(instrument_config["read"].AsBoolean()) : attribute.IsPublished();
										bool write = instrument_config.HasProperty("write") ? bool(instrument_config["write"].AsBoolean()) : attribute.IsSubscribed();
										
										FederateHook<CONFIG> *hook = 0;
										FederateStub<CONFIG> *stub = 0;
										if(instrument_config.HasProperty("hook"))
										{
											hook = this->GetHook(instrument_config["hook"].AsString());
										}
										if(instrument_config.HasProperty("stub"))
										{
											stub = this->GetStub(instrument_config["stub"].AsString());
										}
										
										if(!hook != !stub) // hook XOR stub
										{
											std::string attribute_type_name = object_class->GetAttributeTypeName(instrument_name);
											
											InstrumentBase *instrument = 0;
											
											if(attribute_type_name == "int8") instrument = new Instrument<CONFIG, int8_t>(*this, object_instance->GetAttributeValue<int8_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "int16") instrument = new Instrument<CONFIG, int16_t>(*this, object_instance->GetAttributeValue<int16_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "int32") instrument = new Instrument<CONFIG, int32_t>(*this, object_instance->GetAttributeValue<int32_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "int64") instrument = new Instrument<CONFIG, int64_t>(*this, object_instance->GetAttributeValue<int64_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "uint8") instrument = new Instrument<CONFIG, uint8_t>(*this, object_instance->GetAttributeValue<uint8_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "uint16") instrument = new Instrument<CONFIG, uint16_t>(*this, object_instance->GetAttributeValue<uint16_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "uint32") instrument = new Instrument<CONFIG, uint32_t>(*this, object_instance->GetAttributeValue<uint32_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "uint64") instrument = new Instrument<CONFIG, uint64_t>(*this, object_instance->GetAttributeValue<uint64_t>(instrument_name), expr, read, write);
											else if(attribute_type_name == "float") instrument = new Instrument<CONFIG, float>(*this, object_instance->GetAttributeValue<float>(instrument_name), expr, read, write);
											else if(attribute_type_name == "double") instrument = new Instrument<CONFIG, double>(*this, object_instance->GetAttributeValue<double>(instrument_name), expr, read, write);
											else if(attribute_type_name == "boolean") instrument = new Instrument<CONFIG, bool>(*this, object_instance->GetAttributeValue<bool>(instrument_name), expr, read, write);
											
											if(instrument)
											{
												if(hook)
												{
													if(this->verbose)
													{
														logger << DebugInfo << "Hook \"" << hook->GetName() << "\" will instrument (" << (read ? "read" : "") << ((read && write) ? "/" : "") << (write ? "write": "") << ") \"" << expr << "\" as Attribute value \"" << object_instance->GetName() << "." << instrument_name << "\"" << EndDebugInfo;
													}
													hook->AddInstrument(instrument);
												}
												else if(stub)
												{
													if(this->verbose)
													{
														logger << DebugInfo << "Stub \"" << hook->GetName() << "\" will instrument (" << (read ? "read" : "") << ((read && write) ? "/" : "") << (write ? "write": "") << ") \"" << expr << "\" as Attribute value \"" << object_instance->GetName() << "." << instrument_name << "\"" << EndDebugInfo;
													}
													stub->AddInstrument(instrument);
												}
											}
											else
											{
												logger << DebugError << "Internal error" << EndDebugError;
												return false;
											}
										}
										else
										{
											logger << DebugError << "In File \"" << config_file << "\", at " << instrument_config.GetLocation() << ", Instrument \"" << instrument_name << "\" shall have one of \"hook\" or \"stub\" properties" << EndDebugError;
											return false;
										}
									}
								}
								else
								{
									logger << DebugWarning << "In File \"" << config_file << "\", at " << object_instance_config.GetLocation() << ", no instruments in Object instance \"" << object_instance->GetName() << "\"" << EndDebugWarning;
								}
							}
						}
						else
						{
							logger << DebugWarning << "In File \"" << config_file << "\", at " << object_class_config.GetLocation() << ", no object instances of Object class \"" << object_class->GetName() << "\"" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "In File \"" << config_file << "\", at " << object_class_config.GetLocation() << ", no object instances for Object class \"" << object_class->GetName() << "\"" << EndDebugWarning;
					}
				}
				else
				{
					logger << DebugWarning << "In File \"" << config_file << "\", at " << object_class_config.GetLocation() << ", Object Class \"" << object_class->GetName() << "\" has no attributes" << EndDebugWarning;
				}
			}
		}
		else
		{
			logger << DebugWarning << "In File \"" << config_file << "\", no object classes to simulate" << EndDebugWarning;
		}
	}
	catch(std::exception& e)
	{
		logger << DebugError << e.what() << EndDebugError;
		return false;
	}
	
	return this->Initialize();
}

template <typename CONFIG>
void Federate<CONFIG>::Kill()
{
	unisim::kernel::Object::Kill();
	unisim::util::hla::FederateBase::Kill();
}

template <typename CONFIG>
bool Federate<CONFIG>::Killed() const
{
	return unisim::kernel::Object::Killed() || unisim::util::hla::FederateBase::Killed();
}

template <typename CONFIG>
bool Federate<CONFIG>::HookConflicts(FederateHook<CONFIG> *federate_hook) const
{
	for(typename FederateHooks::const_iterator it = federate_hooks.begin(); it != federate_hooks.end(); ++it)
	{
		if(((*it)->GetName() == federate_hook->GetName()) || (((*it)->GetSourceCodeLocation() == federate_hook->GetSourceCodeLocation()) && ((*it)->GetFilename() == federate_hook->GetFilename())))
		{
			return true;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool Federate<CONFIG>::SetHook(FederateHook<CONFIG> *federate_hook)
{
	if(this->verbose)
	{
		logger << DebugInfo << "Setting Hook \"" << federate_hook->GetName() << "\" at " << federate_hook->GetSourceCodeLocation() << " in file \"" << federate_hook->GetFilename() << "\" for processor #" << this->prc_num << EndDebugInfo;
	}
	
	if(HookConflicts(federate_hook))
	{
		logger << DebugWarning << "Hook \"" << federate_hook->GetName() << "\" at " << federate_hook->GetSourceCodeLocation() << " in file \"" << federate_hook->GetFilename() << "\" for processor #" << this->prc_num << " conflicts" << EndDebugWarning;
		return false;
	}
	
	if(!hooking_import->SetHook(federate_hook))
	{
		logger << DebugWarning << "Can't set Hook \"" << federate_hook->GetName() << "\" at " << federate_hook->GetSourceCodeLocation() << " in file \"" << federate_hook->GetFilename() << "\" for processor #" << this->prc_num << EndDebugWarning;
		return false;
	}

	federate_hooks.push_back(federate_hook);
	
	return true;
}

template <typename CONFIG>
FederateHook<CONFIG> *Federate<CONFIG>::GetHook(const std::string& hook_name) const
{
	for(typename FederateHooks::const_iterator it = federate_hooks.begin(); it != federate_hooks.end(); ++it)
	{
		FederateHook<CONFIG> *federate_hook = *it;
		if(federate_hook->GetName() == hook_name) return federate_hook;
	}
	
	return 0;
}

template <typename CONFIG>
bool Federate<CONFIG>::StubConflicts(FederateStub<CONFIG> *federate_stub) const
{
	for(typename FederateStubs::const_iterator it = federate_stubs.begin(); it != federate_stubs.end(); ++it)
	{
		if(((*it)->GetName() == federate_stub->GetName()) || ((::strcmp((*it)->GetSubProgram()->GetName(), federate_stub->GetSubProgram()->GetName()) == 0) /*&& ((*it)->GetFile() == federate_stub->GetFile())*/))
		{
			return true;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool Federate<CONFIG>::SetStub(FederateStub<CONFIG> *federate_stub)
{
	if(this->verbose)
	{
		logger << DebugInfo << "Setting Stub \"" << federate_stub->GetName() << "\" at " << federate_stub->GetSubProgram()->GetName() << " for processor #" << this->prc_num << EndDebugInfo;
	}

	if(StubConflicts(federate_stub))
	{
		logger << DebugWarning << "Stub \"" << federate_stub->GetName() << "\" at " << federate_stub->GetSubProgram()->GetName() << " for processor #" << this->prc_num << " conflicts" << EndDebugWarning;
		return false;
	}
	
	if(!stubbing_import->SetStub(federate_stub))
	{
		logger << DebugWarning << "Can't set Stub \"" << federate_stub->GetName() << "\" at " << federate_stub->GetSubProgram()->GetName() << " for processor #" << this->prc_num << EndDebugWarning;
		return false;
	}
	
	federate_stubs.push_back(federate_stub);
	
	return true;
}

template <typename CONFIG>
FederateStub<CONFIG> *Federate<CONFIG>::GetStub(const std::string& stub_name) const
{
	for(typename FederateStubs::const_iterator it = federate_stubs.begin(); it != federate_stubs.end(); ++it)
	{
		FederateStub<CONFIG> *federate_stub = *it;
		if(federate_stub->GetName() == stub_name) return federate_stub;
	}
	
	return 0;
}

template <typename CONFIG>
void Federate<CONFIG>::Wait()
{
	const sc_core::sc_time& time_stamp = debug_timing_import->DebugGetTime();
	
	this->WaitUntil(time_stamp);
}

/////////////////////////////// Instrument<> //////////////////////////////////

template <typename CONFIG, typename TYPE>
Instrument<CONFIG, TYPE>::Instrument(Federate<CONFIG>& _federate, unisim::util::hla::AttributeValue<TYPE>& _attr_value, const std::string& _expr, bool _read, bool _write)
	: InstrumentBase(_expr, _read, _write)
	, federate(_federate)
	, attr_value(_attr_value)
	, data_object()
{
}

template <typename CONFIG, typename TYPE>
void Instrument<CONFIG, TYPE>::Do(Direction direction)
{
	if(((direction != InstrumentBase::READ) || !this->read) && ((direction != InstrumentBase::WRITE) || !this->write)) return;
	
	if(data_object.IsUndefined())
	{
		data_object = (*this->federate.data_object_lookup_import)[expr];
	}
	
	if(data_object.Exists())
	{
		TYPE value = TYPE();
		
		switch(direction)
		{
			case InstrumentBase::READ:
				if(this->read)
				{
					if(federate.debug)
					{
						federate.logger << DebugInfo << attr_value.GetObjectInstance().GetName() << '.' << attr_value.GetAttribute().GetName() << " <- " << data_object.GetName() << EndDebugInfo;
					}
					try
					{
						value = TYPE(data_object);
					}
					catch(typename unisim::util::debug::DataObject<ADDRESS>::Error& e)
					{
						federate.logger << DebugWarning << "Can't read data object " << data_object.GetName() << ":" << e.what() << std::endl;
					}
					try
					{
						attr_value = value;
					}
					catch(typename unisim::util::hla::Exception& e)
					{
						federate.logger << DebugWarning << "Can't write attribute " << attr_value.GetObjectInstance().GetName() << '.' << attr_value.GetAttribute().GetName() << ":" << e.what() << std::endl;
					}
				}
				break;
			
			case Instrument::WRITE:
				if(this->write)
				{
					if(federate.debug)
					{
						federate.logger << DebugInfo << data_object.GetName() << " <- " << attr_value.GetObjectInstance().GetName() << '.' << attr_value.GetAttribute().GetName() << EndDebugInfo;
					}
					try
					{
						value = TYPE(attr_value);
					}
					catch(typename unisim::util::hla::Exception& e)
					{
						federate.logger << DebugWarning << "Can't read attribute " << attr_value.GetObjectInstance().GetName() << '.' << attr_value.GetAttribute().GetName() << ":" << e.what() << std::endl;
					}
					try
					{
						data_object = value;
					}
					catch(typename unisim::util::debug::DataObject<ADDRESS>::Error& e)
					{
						federate.logger << DebugWarning << "Can't write data object " << data_object.GetName() << ":" << e.what() << std::endl;
					}
				}
				break;
		}
	}
	else
	{
		federate.logger << DebugWarning << "Data object " << data_object.GetName() << " does not exist" << std::endl;
	}
}

//////////////////////////// FederateHookStub<> ///////////////////////////////

template <typename CONFIG>
FederateHookStub<CONFIG>::FederateHookStub(Federate<CONFIG>& _federate, const std::string& _name, const unisim::util::json::JSON_Value *_json_return_value)
	: federate(_federate)
	, name(_name)
	, json_return_value(_json_return_value)
	, instruments()
	, input_instruments()
	, output_instruments()
{
}

template <typename CONFIG>
FederateHookStub<CONFIG>::~FederateHookStub()
{
	for(Instruments::iterator it = instruments.begin(); it != instruments.end(); ++it)
	{
		InstrumentBase *instrument = *it;
		delete instrument;
	}
}

template <typename CONFIG>
const std::string& FederateHookStub<CONFIG>::GetName() const
{
	return name;
}

template <typename CONFIG>
void FederateHookStub<CONFIG>::AddInstrument(InstrumentBase *instrument)
{
	instruments.push_back(instrument);
	if(instrument->IsRead()) output_instruments.push_back(instrument);
	if(instrument->IsWrite()) input_instruments.push_back(instrument);
}

template <typename CONFIG>
void FederateHookStub<CONFIG>::DoInstrument(InstrumentBase::Direction direction)
{
	Instruments& instruments = direction == (InstrumentBase::READ) ? output_instruments : input_instruments;
	for(Instruments::iterator it = instruments.begin(); it != instruments.end(); ++it)
	{
		InstrumentBase *instrument = *it;
		instrument->Do(direction);
	}
}

////////////////////////////// FederateHook<> /////////////////////////////////

template <typename CONFIG>
FederateHook<CONFIG>::FederateHook(Federate<CONFIG>& _federate, const std::string& _name, const std::string& _file, const unisim::util::debug::SourceCodeLocation& _source_code_location, const unisim::util::json::JSON_Value *_json_return_value)
	: unisim::util::debug::SourceCodeHook<ADDRESS>(_source_code_location, _file)
	, FederateHookStub<CONFIG>(_federate, _name, _json_return_value)
{
}

template <typename CONFIG>
void FederateHook<CONFIG>::Run()
{
	if(this->federate.debug)
	{
		this->federate.logger << DebugInfo << "Reached Hook \"" << this->GetName() << "\"" << EndDebugInfo;
	}
	
	// program variables -> attribute values
	this->DoInstrument(InstrumentBase::READ);
	
	try
	{
		this->federate.Wait();
	}
	catch(unisim::util::hla::Exception& e)
	{
		this->federate.logger << DebugError << e.what() << EndDebugError;
		this->federate.Stop(-1);
		return;
	}
	
	// program variables <- attribute values
	this->DoInstrument(InstrumentBase::WRITE);
	
	if(this->json_return_value)
	{
		if(this->json_return_value->IsNull())
		{
			unisim::util::debug::JSON2DataObject<ADDRESS> json_to_data_object(this->federate.data_object_lookup_import);
			unisim::util::debug::DataObjectRef<ADDRESS> return_value = this->federate.data_object_lookup_import->GetReturnValue();
			json_to_data_object.Do(this->json_return_value, return_value);
		}
		this->federate.stack_frame_import->UnwindStack();
	}
}

////////////////////////////// FederateStub<> /////////////////////////////////

template <typename CONFIG>
FederateStub<CONFIG>::FederateStub(Federate<CONFIG>& _federate, const std::string& _name, const typename unisim::util::debug::SubProgram<ADDRESS> *_subprogram, const unisim::util::json::JSON_Value *_json_return_value)
	: unisim::util::debug::Stub<ADDRESS>(_subprogram)
	, FederateHookStub<CONFIG>(_federate, _name, _json_return_value)
{
}

template <typename CONFIG>
void FederateStub<CONFIG>::Run(typename unisim::util::debug::Stub<ADDRESS>::Parameters& parameters)
{
	if(this->federate.debug)
	{
		this->federate.logger << DebugInfo << "Reached Stub \"" << this->GetName() << "\"" << EndDebugInfo;
	}
	
	// program variables -> attribute values
	this->DoInstrument(InstrumentBase::READ);
	
	this->federate.Wait();
	
	// program variables <- attribute values
	this->DoInstrument(InstrumentBase::WRITE);
	
	if(this->json_return_value)
	{
		if(this->json_return_value->IsNull())
		{
			unisim::util::debug::JSON2DataObject<ADDRESS> json_to_data_object(this->federate.data_object_lookup_import);
			json_to_data_object.Do(this->json_return_value, parameters.GetReturnValue());
		}
	}
}

} // end of namespace hla
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // HAVE_HLA_RTI1516E

#endif // __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_TCC__
