/*
 *  Copyright (c) 2015-2019,
 *  Commissariat a l'Energie Atomique et aux Energies Alternatives (CEA)
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

#include <unisim/service/instrumenter/instrumenter.hh>

namespace unisim {
namespace service {
namespace instrumenter {

Instrumenter::Instrumenter(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, http_server_export("http-server-export", this)
	, logger(*this)
	, verbose(false)
	, debug(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, verbose, "Enable/Disable debugging (intended for developper)")
	, vcd_trace_filename()
	, param_vcd_trace_filename("vcd-trace-file", this, vcd_trace_filename, "VCD output filename for signals trace")
	, trace_signals()
	, param_trace_signals("trace-signals", this, trace_signals, "Signals to trace (wildcards '*' and '?' are allowed in signal names)")
	, enable_trace_signals(false)
	, param_enable_trace_signals("enable-trace-signals", this, enable_trace_signals, "Enable/Disable signals trace")
	, trace_file(0)
	, gtkwave_init_script()
	, param_gtkwave_init_script("gtkwave-init-script", this, gtkwave_init_script, "GTKWave initialization script that simulators must automatically generate at startup")
	, enable_user_interface(false)
	, param_enable_user_interface("enable-user-interface", this, enable_user_interface, "Enable/Disable User interface over HTTP")
	, user_interface(0)
	, enable_csv_reader(false)
	, param_enable_csv_reader("enable-csv-reader", this, enable_csv_reader, "Enable/Disable CSV reader")
	, csv_reader(0)
	, enable_csv_writer(false)
	, param_enable_csv_writer("enable-csv-writer", this, enable_csv_writer, "Enable/Disable CSV writer")
	, csv_writer(0)
	, front_ends_priority_order()
	, param_front_ends_priority_order("front-ends-priority-order", this, front_ends_priority_order, "Front-ends priority order (from left/higher to right/lower priority)")
	, signal_pool()
	, auto_signal_pool()
	, port_pool()
	, netlist()
	, typers()
	, signal_tees()
	, instruments()
	, instrumenter_front_ends()
	, pull_down_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_down", false, OUTPUT_INSTRUMENTATION))
	, pull_down_0_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_down_0", false, OUTPUT_INSTRUMENTATION))
	, pull_up_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_up", true, OUTPUT_INSTRUMENTATION))
	, pull_up_0_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_up_0", true, OUTPUT_INSTRUMENTATION))
	, unused_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("unused", false, OUTPUT_INSTRUMENTATION))
	, unused_0_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("unused_0", false, OUTPUT_INSTRUMENTATION))
{
	if(front_ends_priority_order.empty())
	{
		std::stringstream front_ends_priority_order_sstr;
		front_ends_priority_order_sstr << this->GetName() << ".user-interface " << this->GetName() << ".csv-reader " << this->GetName() << ".csv-writer";
		front_ends_priority_order = front_ends_priority_order_sstr.str();
	}
	
	if(enable_user_interface)
	{
		user_interface = new UserInterface("user-interface", this);
		http_server_export >> user_interface->http_server_export;
	}
	
	if(enable_csv_reader)
	{
		csv_reader = new CSV_Reader("csv-reader", this);
	}
	
	if(enable_csv_writer)
	{
		csv_writer = new CSV_Writer("csv-writer", this);
	}
}

Instrumenter::~Instrumenter()
{
	if(trace_file)
	{
		sc_close_vcd_trace_file(trace_file);
	}

	std::vector<InstrumenterFrontEnd *>::iterator instrument_front_end_it;
	for(instrument_front_end_it = instrumenter_front_ends.begin(); instrument_front_end_it != instrumenter_front_ends.end(); instrument_front_end_it++)
	{
		InstrumenterFrontEnd *instrumenter_front_end = *instrument_front_end_it;
		delete instrumenter_front_end;
	}
	
	std::map<InstrumentKey, InstrumentBase *>::iterator instrument_it;
	for(instrument_it = instruments.begin(); instrument_it != instruments.end(); instrument_it++)
	{
		InstrumentBase *instrument = (*instrument_it).second;
		delete instrument;
	}
	
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		delete signal_if; 
	}

	std::map<std::string, sc_core::sc_interface *>::iterator auto_signal_pool_it;
	
	for(auto_signal_pool_it = auto_signal_pool.begin(); auto_signal_pool_it != auto_signal_pool.end(); auto_signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*auto_signal_pool_it).second;
		delete signal_if; 
	}

	std::map<std::string, TyperBase *>::iterator typer_it;
	
	for(typer_it = typers.begin(); typer_it != typers.end(); typer_it++)
	{
		TyperBase *typer = (*typer_it).second;
		delete typer;
	}
	
	std::map<std::string, SignalTeeBase *>::iterator signal_tee_it;
	for(signal_tee_it = signal_tees.begin(); signal_tee_it != signal_tees.end(); signal_tee_it++)
	{
		SignalTeeBase *signal_tee = (*signal_tee_it).second;
		delete signal_tee;
	}
}

bool Instrumenter::BeginSetup()
{
	if(!SetupInstrumentation()) return false;
	
	StartBinding();
	
	return true;
}

bool Instrumenter::EndSetup()
{
	return true;
}

unisim::kernel::tlm2::Clock& Instrumenter::CreateClock(const std::string& clock_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(clock_name);
	if(signal_pool_it != signal_pool.end())
	{
		throw std::runtime_error("Internal error! clock already exists");
	}

	unisim::kernel::tlm2::Clock *clock = new unisim::kernel::tlm2::Clock(clock_name.c_str(), GetParent());
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Creating " << (clock->IsClockLazy() ? "lazy (fast) " : "toggling (painfully slow) ") << "Clock \"" << clock->sc_core::sc_object::name() << "\" with a period of " << clock->GetClockPeriod() << ", a duty cycle of " << clock->GetClockDutyCycle() << ", starting with " << (clock->GetClockPosEdgeFirst() ? "rising" : "falling") << " edge at " << clock->GetClockStartTime() << EndDebugInfo;
	}
	
	signal_pool[clock->sc_core::sc_object::name()] = clock;
	
	typers[clock->sc_core::sc_object::name()] = new Typer<bool>(this);

	return *clock;
}

void Instrumenter::GenerateGTKWaveInitScript(const std::string& filename, int zoom_factor) const
{
	std::ofstream of(filename.c_str(), std::ofstream::out);
	
	if(of.fail())
	{
		logger << DebugWarning << "Can't open output \"" << filename << "\"" << EndDebugWarning;
		return;
	}
	
	of << "# Added some signals to view" << std::endl;
	of << "set my_sig_list [list]" << std::endl;
	std::map<std::string, sc_core::sc_interface *>::const_iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		sc_core::sc_prim_channel *prim_channel = dynamic_cast<sc_core::sc_prim_channel *>(signal_if);
		
		if(prim_channel)
		{
			of << "lappend my_sig_list \"" << prim_channel->name() << "\"" << std::endl;
		}
	}
	
	of << "set num_added [ gtkwave::addSignalsFromList $my_sig_list ]" << std::endl;
	of << "# Adjust zoom" << std::endl;
	of << "gtkwave::setZoomFactor " << zoom_factor << std::endl;
}

void Instrumenter::RegisterPort(sc_core::sc_port_base& port)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Registering Port \"" << port.name() << "\"" << EndDebugInfo;
	}
	std::map<std::string, sc_core::sc_port_base *>::iterator port_pool_it;
	for(port_pool_it = port_pool.begin(); port_pool_it != port_pool.end(); port_pool_it++)
	{
		sc_core::sc_port_base *registered_port_base = (*port_pool_it).second;
		
		if(registered_port_base && (&port == registered_port_base))
		{
			throw std::runtime_error("Internal error! port already registered");
		}
	}
	
	std::string port_name = port.name();
	
	port_pool[port_name] = &port;
}

void Instrumenter::PrepareInstrumentation(const std::string& signal_name_pattern, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id)
{
	instrumentations.insert(InstrumentKey(signal_name_pattern, instrumentation_type, front_end_id));
}

bool Instrumenter::Match(const char *p, const char *s)
{
	while(*p)
	{
		if(*p == '?')
		{
			if(!*s) return false;

			++s;
			++p;
		}
		else if(*p == '*')
		{
			return Match(p + 1, s) || (*s && Match(p, s + 1));
		}
		else
		{
			if(*s++ != *p++) return false;
		}
	}
	
	return !*s && !*p;
}

bool Instrumenter::Match(const std::string& p, const std::string& s)
{
	return Match(p.c_str(), s.c_str());
}

void Instrumenter::MatchSignalPattern(const std::string& signal_name_pattern, std::vector<std::string>& vec_signal_names) const
{
	std::map<std::string, sc_core::sc_interface *>::const_iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		std::string s((*signal_pool_it).first);
		const std::string& p = signal_name_pattern;
			
		if(Match(p, s))
		{
			vec_signal_names.push_back(s);
		}
	}
}

void Instrumenter::TraceSignalPattern(const std::string& signal_name_pattern)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it;
	
	bool traced = false;
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		std::string s((*signal_pool_it).first);
		const std::string& p = signal_name_pattern;
			
		if(Match(p, s))
		{
			std::map<std::string, TyperBase *>::iterator typer_it = typers.find(s);
			
			if(typer_it != typers.end())
			{
				TyperBase *typer = (*typer_it).second;
				
				if(typer->TryTraceSignal(s))
				{
					traced = true;
				}
			}
		}
	}
	
	if(!traced)
	{
		logger << DebugWarning << "Can't trace Signal \"" << signal_name_pattern << "\"" << EndDebugWarning;
	}
}

void Instrumenter::Bind(const std::string& port_name, const std::string& signal_name)
{
	if(netlist.find(port_name) != netlist.end())
	{
		logger << DebugWarning << "port \"" << port_name << "\" is already bound" << EndDebugWarning;
		return;
	}
	
	netlist[port_name] = signal_name;
}

void Instrumenter::BindArray(unsigned int dim, const std::string& port_array_name, unsigned int port_array_begin_idx, unsigned int port_array_stride, const std::string& signal_array_name, unsigned int signal_array_begin_idx, unsigned int signal_array_stride)
{
	unsigned int idx;
	
	for(idx = 0; idx < dim; idx++)
	{
		unsigned int port_array_idx = port_array_begin_idx + (idx * port_array_stride);
		unsigned int signal_array_idx = signal_array_begin_idx + (idx * signal_array_stride);
		std::stringstream port_name_sstr;
		port_name_sstr << port_array_name << "_" << port_array_idx;
		std::string port_name(port_name_sstr.str());

		std::stringstream signal_name_sstr;
		signal_name_sstr << signal_array_name << "_" << signal_array_idx;
		std::string signal_name(signal_name_sstr.str());
		
		Bind(port_name, signal_name);
	}
}

void Instrumenter::BindArray(unsigned int dim, const std::string& port_array_name, unsigned int port_array_begin_idx, const std::string& signal_array_name, unsigned int signal_array_begin_idx)
{
	BindArray(dim, port_array_name, port_array_begin_idx, 1, signal_array_name, signal_array_begin_idx, 1);
}

void Instrumenter::BindArray(unsigned int dim, const std::string& port_array_name, const std::string& signal_array_name)
{
	BindArray(dim, port_array_name, 0, signal_array_name, 0);
}

void Instrumenter::StartBinding()
{
	std::map<std::string, std::string>::iterator netlist_it;
	
	for(netlist_it = netlist.begin(); netlist_it != netlist.end(); netlist_it++)
	{
		const std::string& port_name = (*netlist_it).first;
		const std::string& signal_name = (*netlist_it).second;

		std::map<std::string, TyperBase *>::iterator typer_it = typers.find(signal_name);
		
		if(typer_it != typers.end())
		{
			TyperBase *typer = (*typer_it).second;
			
			if(typer->TryBind(port_name, signal_name))
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Connecting Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << EndDebugInfo;
				}
				continue;
			}
		}
		else
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Signal \"" << signal_name << "\" does not exist" << EndDebugInfo;
			}
		}
		
		logger << DebugWarning << "Can't connect Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << EndDebugWarning;
	}
}

bool Instrumenter::SetupInstrumentation()
{
	// enable generation of GTKWave initialization script if requested
	if(!gtkwave_init_script.empty())
	{
		GenerateGTKWaveInitScript(gtkwave_init_script, -34);
	}

	// enable signal tracing if requested
	if(enable_trace_signals)
	{
		if(!((std::string) vcd_trace_filename).empty())
		{
			std::string vcd_trace_filename_w_ext((std::string) vcd_trace_filename);
			
			std::size_t dot_pos = vcd_trace_filename_w_ext.find_last_of('.');
			std::string vcd_trace_filename_wo_ext = (dot_pos != std::string::npos) ? vcd_trace_filename_w_ext.substr(0, dot_pos) : vcd_trace_filename_w_ext;
			
			trace_file = sc_core::sc_create_vcd_trace_file(vcd_trace_filename_wo_ext.c_str());

			std::string signal_name;
			std::stringstream sstr(trace_signals);

			while(sstr >> signal_name)
			{
				TraceSignalPattern(signal_name);
			}
		}
	}

	std::vector<InstrumenterFrontEnd *>::iterator instrument_front_end_it;
	for(instrument_front_end_it = instrumenter_front_ends.begin(); instrument_front_end_it != instrumenter_front_ends.end(); instrument_front_end_it++)
	{
		InstrumenterFrontEnd *instrumenter_front_end = *instrument_front_end_it;
		if(instrumenter_front_end)
		{
			if(!instrumenter_front_end->SetupInstrumentation()) return false;
			instrumenter_front_end->Initialize();
		}
	}
	
	std::map<std::string, SignalTeeBase *>::iterator signal_tee_it;
	for(signal_tee_it = signal_tees.begin(); signal_tee_it != signal_tees.end(); signal_tee_it++)
	{
		SignalTeeBase *signal_tee = (*signal_tee_it).second;
		signal_tee->Initialize();
	}

	
	return true;
}

InstrumentBase *Instrumenter::FindInstrument(const std::string& name, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id)
{
	std::map<InstrumentKey, InstrumentBase *>::iterator instrument_it = instruments.find(InstrumentKey(name, instrumentation_type, front_end_id));
	
	if(instrument_it != instruments.end())
	{
		InstrumentBase *instrument = (*instrument_it).second;
		return instrument;
	}
	
	return 0;
}

InputInstrumentBase *Instrumenter::FindInputInstrument(const std::string& name, unsigned int front_end_id)
{
	return dynamic_cast<InputInstrumentBase *>(FindInstrument(name, INPUT_INSTRUMENTATION, front_end_id));
}

OutputInstrumentBase *Instrumenter::FindOutputInstrument(const std::string& name, unsigned int front_end_id)
{
	return dynamic_cast<OutputInstrumentBase *>(FindInstrument(name, OUTPUT_INSTRUMENTATION, front_end_id));
}

unsigned int Instrumenter::RegisterFrontEnd(InstrumenterFrontEnd *instrumenter_front_end)
{
	unsigned int front_end_id = 0;
	std::istringstream sstr(front_ends_priority_order);
	std::string front_end_name;
	
	while(sstr >> front_end_name)
	{
		if(front_end_name == instrumenter_front_end->GetName())
		{
			unsigned int num_front_ends = instrumenter_front_ends.size();
			if(front_end_id >= num_front_ends) instrumenter_front_ends.resize(front_end_id + 1);
			instrumenter_front_ends[front_end_id] = instrumenter_front_end;
			return front_end_id;
		}
		
		front_end_id++;
	}
	
// 	unsigned int front_end_id = instrumenter_front_ends.size();
// 	instrumenter_front_ends.push_back(instrumenter_front_end);
// 	return front_end_id;
	logger << DebugError << "No priority specified for \"" << instrumenter_front_end->GetName() << "\" in Parameter \"" << param_front_ends_priority_order.GetName() << "\"" << EndDebugError;
	this->Stop(-1);
	return 0;
}

SignalTeeBase::SignalTeeBase(const std::string& _name)
	: enable_injection_event(sc_core::sc_gen_unique_name("enable_injection_event"))
	, process_spawn_options()
	, enable_mask(0)
	, name(_name)
{
	process_spawn_options.spawn_method();
	process_spawn_options.set_sensitivity(&enable_injection_event);
}

SignalTeeBase::~SignalTeeBase()
{
}

void SignalTeeBase::EnableInjection(unsigned int front_end_id)
{
	uint64_t new_enable_mask = enable_mask | (uint64_t(1) << front_end_id);
	if(new_enable_mask != enable_mask)
	{
		enable_mask = new_enable_mask;
		enable_injection_event.notify(sc_core::SC_ZERO_TIME);
	}
}

void SignalTeeBase::DisableInjection(unsigned int front_end_id)
{
	uint64_t new_enable_mask = enable_mask & ~(uint64_t(1) << front_end_id);
	if(new_enable_mask != enable_mask)
	{
		enable_mask = new_enable_mask;
		enable_injection_event.notify(sc_core::SC_ZERO_TIME);
	}
}

InputInstrumentBase::InputInstrumentBase(const std::string& name, unsigned int front_end_id, SignalTeeBase *_signal_tee)
	: InstrumentBase(name, INPUT_INSTRUMENTATION, front_end_id)
	, signal_tee(_signal_tee)
{
}

void InputInstrumentBase::EnableInjection()
{
	signal_tee->EnableInjection(GetFrontEndId());
}

void InputInstrumentBase::DisableInjection()
{
	signal_tee->DisableInjection(GetFrontEndId());
}

OutputInstrumentBase::OutputInstrumentBase(const std::string& name, unsigned int front_end_id)
	: InstrumentBase(name, OUTPUT_INSTRUMENTATION, front_end_id)
{
}

InstrumenterFrontEnd::InstrumenterFrontEnd(const char *name, Instrumenter *_instrumenter)
	: unisim::kernel::service::Object(name, _instrumenter)
	, instrumenter(_instrumenter)
	, front_end_id(instrumenter->RegisterFrontEnd(this))
	, input_instruments()
	, output_instruments()
	, input_instruments_map()
	, output_instruments_map()
	, output_instrumentation_process_spawn_options()
	, next_input_instrumentation_event(sc_core::sc_gen_unique_name("next_input_instrumentation_event"))
{
}

InstrumenterFrontEnd::~InstrumenterFrontEnd()
{
}

void InstrumenterFrontEnd::MatchSignalPattern(const std::string& signal_name_pattern, std::vector<std::string>& vec_signal_names) const
{
	instrumenter->MatchSignalPattern(signal_name_pattern, vec_signal_names);
}

void InstrumenterFrontEnd::PrepareInstrumentation(const std::string& signal_name_pattern, INSTRUMENTATION_TYPE instrumentation_type)
{
	instrumenter->PrepareInstrumentation(signal_name_pattern, instrumentation_type, front_end_id);
}

InputInstrumentBase *InstrumenterFrontEnd::InstrumentInputSignal(const std::string& signal_name)
{
	InputInstrumentBase *input_instrument = instrumenter->FindInputInstrument(signal_name, front_end_id);
	if(input_instrument)
	{
		input_instruments.push_back(input_instrument);
		input_instruments_map[signal_name] = input_instrument;
	}
	return input_instrument;
}

OutputInstrumentBase *InstrumenterFrontEnd::InstrumentOutputSignal(const std::string& signal_name)
{
	OutputInstrumentBase *output_instrument = instrumenter->FindOutputInstrument(signal_name, front_end_id);
	if(output_instrument)
	{
		output_instrumentation_process_spawn_options.set_sensitivity(output_instrument->GetInterface());
		output_instruments.push_back(output_instrument);
		output_instruments_map[signal_name] = output_instrument;
	}
	return output_instrument;
}

InputInstrumentBase *InstrumenterFrontEnd::FindInputInstrument(const std::string& signal_name) const
{
	std::map<std::string, InputInstrumentBase *>::const_iterator it = input_instruments_map.find(signal_name);
	return (it != input_instruments_map.end()) ? (*it).second : 0;
}

const std::vector<InputInstrumentBase *>& InstrumenterFrontEnd::GetInputInstruments() const
{
	return input_instruments;
}

OutputInstrumentBase *InstrumenterFrontEnd::FindOutputInstrument(const std::string& signal_name) const
{
	std::map<std::string, OutputInstrumentBase *>::const_iterator it = output_instruments_map.find(signal_name);
	return (it != output_instruments_map.end()) ? (*it).second : 0;
}

const std::vector<OutputInstrumentBase *>& InstrumenterFrontEnd::GetOutputInstruments() const
{
	return output_instruments;
}

void InstrumenterFrontEnd::Initialize()
{
	sc_core::sc_spawn_options input_instrumentation_process_spawn_options;
	
	input_instrumentation_process_spawn_options.spawn_method();
	input_instrumentation_process_spawn_options.set_sensitivity(&next_input_instrumentation_event);
	sc_core::sc_spawn(sc_bind(&InstrumenterFrontEnd::InputInstrumentationProcess, this), sc_core::sc_gen_unique_name("InputInstrumentationProcess"), &input_instrumentation_process_spawn_options);
	
	output_instrumentation_process_spawn_options.spawn_method();
	sc_core::sc_spawn(sc_bind(&InstrumenterFrontEnd::OutputInstrumentationProcess, this), sc_core::sc_gen_unique_name("OutputInstrumentationProcess"), &output_instrumentation_process_spawn_options);
}

void InstrumenterFrontEnd::OutputInstrumentationProcess()
{
	ProcessOutputInstruments();
}

void InstrumenterFrontEnd::InputInstrumentationProcess()
{
	ProcessInputInstruments();
}

UserInstrument::UserInstrument(const std::string& _name, InputInstrumentBase *_input_instrument, OutputInstrumentBase *_output_instrument)
	: name(_name)
  , sstr()
	, set_value()
	, get_value()
	, input_instrument(_input_instrument)
	, output_instrument(_output_instrument)
	, enable(false)
	, new_enable(enable)
	, value_changed_breakpoint(false)
	, has_breakpoint_cond(false)
	, get_value_valid(false)
{
	if(input_instrument)
	{
		std::ostringstream set_value_sstr;
		input_instrument->Print(set_value_sstr);
		set_value = set_value_sstr.str();
	}
	
	if(output_instrument)
	{
		std::ostringstream get_value_sstr;
		output_instrument->Output(get_value_sstr);
		get_value = get_value_sstr.str();
		get_value_valid = true;
	}
	
	if(input_instrument)
	{
		if(enable)
		{
			input_instrument->EnableInjection();
		}
		else
		{
			input_instrument->DisableInjection();
		}
	}
}

const std::string& UserInstrument::GetName() const
{
	return name;
}

void UserInstrument::Set(const std::string& value)
{
	set_value = value;
}

void UserInstrument::Get(std::string& value) const
{
	Update();
	value = get_value;
}

void UserInstrument::Set(bool value)
{
	set_value = value ? "1" : "0";
}

void UserInstrument::Get(bool& value) const
{
	Update();
	value = (get_value != "0");
}

void UserInstrument::Toggle()
{
	Update();
	if(get_value == "1") set_value = "0";
	else if(get_value == "0") set_value = "1";
}

void UserInstrument::InitialFetch()
{
	if(likely(output_instrument))
	{
		sstr.clear();
		sstr.seekp(0);
		output_instrument->Output(sstr);
		get_value = sstr.str();
	}
}

void UserInstrument::Update() const
{
	if(!get_value_valid)
	{
		sstr.clear();
		sstr.seekp(0);
		output_instrument->Output(sstr);
		get_value = sstr.str();
		get_value_valid = true;
	}
}

void UserInstrument::Sample()
{
	if(likely(output_instrument))
	{
		output_instrument->Sample();
// 		sstr.clear();
// 		sstr.seekp(0);
// 		output_instrument->Output(sstr);
		has_breakpoint_cond = value_changed_breakpoint && output_instrument->ValueChanged();
// 		get_value = sstr.str();
		get_value_valid = false;
	}
}

void UserInstrument::Latch()
{
	if(likely(output_instrument))
	{
		output_instrument->Latch();
	}
}

void UserInstrument::Fetch()
{
	Sample();
	Latch();
}

void UserInstrument::EnableValueChangedBreakpoint()
{
	value_changed_breakpoint = true;
}

void UserInstrument::DisableValueChangedBreakpoint()
{
	value_changed_breakpoint = false;
}

bool UserInstrument::IsValueChangedBreakpointEnabled() const
{
	return value_changed_breakpoint;
}

bool UserInstrument::HasBreakpointCondition() const
{
	return has_breakpoint_cond;
}

void UserInstrument::Commit()
{
	if(likely(input_instrument))
	{
		if(new_enable != enable)
		{
			enable = new_enable;
			if(enable)
			{
				input_instrument->EnableInjection();
			}
			else
			{
				input_instrument->DisableInjection();
			}
		}
		//std::cerr << "UserInstrument:Commit(): \"" << name << "\" <- " << set_value << std::endl;
		std::istringstream sstr(set_value);
		input_instrument->Input(sstr);
		input_instrument->Inject();
	}
}

void UserInstrument::EnableInjection()
{
	if(input_instrument) new_enable = true;
}

void UserInstrument::DisableInjection()
{
	new_enable = false;
}

bool UserInstrument::IsInjectionEnabled() const
{
	return new_enable;
}

bool UserInstrument::IsReadOnly() const
{
	return !input_instrument;
}

bool UserInstrument::IsBoolean() const
{
	return (input_instrument && (input_instrument->GetTypeInfo() == typeid(bool))) || (output_instrument && (output_instrument->GetTypeInfo() == typeid(bool)));
}

static std::string String_to_HTML(const std::string& s)
{
	std::stringstream sstr;
	std::size_t pos = 0;
	std::size_t len = s.length();
	
	for(pos = 0; pos < len; pos++)
	{
		char c = s[pos];

		switch(c)
		{
			case '\n':
				sstr << "<br>";
				break;
			case '<':
				sstr << "&lt;";
				break;
			case '>':
				sstr << "&gt;";
				break;
			case '&':
				sstr << "&amp;";
				break;
			case '"':
				sstr << "&quot;";
				break;
			case '\'':
				sstr << "&apos;";
				break;
			case ' ':
				sstr << "&nbsp;";
				break;
			case '\t':
				sstr << "&nbsp;&nbsp;&nbsp;&nbsp;";
			default:
				sstr << c;
		}
	}
	
	return sstr.str();
}

UserInterface::UserInterface(const char *name, Instrumenter *instrumenter)
	: unisim::kernel::service::Object(name, instrumenter)
	, InstrumenterFrontEnd(name, instrumenter)
	, unisim::kernel::service::Service<unisim::service::interfaces::HttpServer>(name, instrumenter)
	, http_server_export("http-server-export", this)
	, logger(*this)
	, program_name(unisim::kernel::service::Simulator::Instance()->FindVariable("program-name")->operator std::string())
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, instrumentation()
	, param_instrumentation("instrumentation", this, instrumentation, "Instrumented signals (wildcards '*' and '?' are allowed in signal names) that are controlled by user over HTTP")
	, intr_poll_period(1.0, sc_core::SC_US)
	, param_intr_poll_period("intr-poll-period", this, intr_poll_period, "Polling period for user interrupt request while continue")
	, instrumented_signal_names()
	, user_instruments()
	, user_step_time(sc_core::SC_ZERO_TIME)
	, curr_time_stamp(sc_core::SC_ZERO_TIME)
	, bad_user_step_time(false)
	, timed_step(false)
	, delta_steps(0)
	, cont(false)
	, intr(true)
	, halt(false)
	, has_breakpoint_cond(false)
	, enable_all_input_instruments(false)
	, disable_all_input_instruments(false)
	, enable_all_value_changed_breakpoints(false)
	, disable_all_value_changed_breakpoints(false)
	, enable_input_instruments()
	, enable_value_changed_breakpoints()
	, mutex_instruments()
	, mutex_post()
	, run(false)
	, cond_run()
	, mutex_run()
	, user(false)
	, cond_user()
	, mutex_user()
{
	pthread_mutex_init(&mutex_instruments, NULL);
	pthread_mutex_init(&mutex_post, NULL);
	pthread_cond_init(&cond_run, NULL);
	pthread_mutex_init(&mutex_run, NULL);
	pthread_cond_init(&cond_user, NULL);
	pthread_mutex_init(&mutex_user, NULL);
	
	std::stringstream sstr(instrumentation);
	
	std::string signal_name_pattern;
	while(sstr >> signal_name_pattern)
	{
		PrepareInstrumentation(signal_name_pattern, INPUT_OUTPUT_INSTRUMENTATION);
	}
}

UserInterface::~UserInterface()
{
	UnblockUser();
	
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		delete user_instrument;
	}

	pthread_mutex_destroy(&mutex_instruments);
	pthread_mutex_destroy(&mutex_post);
	pthread_cond_destroy(&cond_run);
	pthread_mutex_destroy(&mutex_run);
	pthread_cond_destroy(&cond_user);
	pthread_mutex_destroy(&mutex_user);
}

bool UserInterface::EndSetup()
{
	return true;
}

void UserInterface::SigInt()
{
	Run();
}

bool UserInterface::SetupInstrumentation()
{
	std::stringstream sstr(instrumentation);
	std::string signal_name_pattern;

	instrumented_signal_names.clear();
	
	while(sstr >> signal_name_pattern)
	{
		MatchSignalPattern(signal_name_pattern, instrumented_signal_names);
	}
	
	std::vector<std::string>::iterator instrumented_signal_name_it;
	for(instrumented_signal_name_it = instrumented_signal_names.begin(); instrumented_signal_name_it != instrumented_signal_names.end(); instrumented_signal_name_it++)
	{
		const std::string& instrumented_signal_name = *instrumented_signal_name_it;
	
		InputInstrumentBase *input_instrument = InstrumentInputSignal(instrumented_signal_name);
		OutputInstrumentBase *output_instrument = InstrumentOutputSignal(instrumented_signal_name);
		
		if(input_instrument || output_instrument)
		{
			UserInstrument *user_instrument = new UserInstrument(instrumented_signal_name, input_instrument, output_instrument);
			user_instruments[instrumented_signal_name] = user_instrument;
			enable_input_instruments[user_instrument] = false;
			enable_value_changed_breakpoints[user_instrument] = false;
			set_input_instruments[user_instrument] = std::string();
			toggle_input_instruments[user_instrument] = false;
			if(!input_instrument)
			{
				logger << DebugWarning << "User instrument \"" << user_instrument->GetName() << "\" is read-only" << EndDebugWarning;
			}
			if(!output_instrument)
			{
				logger << DebugWarning << "User instrument \"" << user_instrument->GetName() << "\" is write-only" << EndDebugWarning;
			}
		}
		else
		{
			logger << DebugWarning << "Can't get input/output instruments for signal \"" << instrumented_signal_name << "\"" << EndDebugWarning;
		}
	}
	
	InitialFetch();
	DisableInjection();
	DisableValueChangedBreakpoint();
	
	return true;
}

void UserInterface::WaitForUser()
{
	//std::cerr << sc_core::sc_time_stamp() << ":WaitForUser: start" << std::endl;
	pthread_mutex_lock(&mutex_user);
	user = true;
	pthread_cond_signal(&cond_user);
	pthread_mutex_lock(&mutex_run);
	run = false;
	pthread_mutex_unlock(&mutex_user);
	do
	{
		pthread_cond_wait(&cond_run, &mutex_run);
	}
	while(!run);
	pthread_mutex_unlock(&mutex_run);
	//std::cerr << sc_core::sc_time_stamp() << ":WaitForUser: end" << std::endl;
}

void UserInterface::WaitForSimulation()
{
	//std::cerr << "WaitForSimulation: start" << std::endl;
	pthread_mutex_lock(&mutex_run);
	run = true;
	pthread_cond_signal(&cond_run);
	pthread_mutex_lock(&mutex_user);
	user = false;
	pthread_mutex_unlock(&mutex_run);
	do
	{
		pthread_cond_wait(&cond_user, &mutex_user);
	}
	while(!user);
	pthread_mutex_unlock(&mutex_user);
	//std::cerr << "WaitForSimulation: end" << std::endl;
}

void UserInterface::Run()
{
	//std::cerr << "Run" << std::endl;
	pthread_mutex_lock(&mutex_run);
	run = true;
	pthread_cond_signal(&cond_run);
	pthread_mutex_unlock(&mutex_run);
}

void UserInterface::UnblockUser()
{
	//std::cerr << "UnblockUser" << std::endl;
	pthread_mutex_lock(&mutex_user);
	user = true;
	pthread_cond_signal(&cond_user);
	pthread_mutex_unlock(&mutex_user);
}

void UserInterface::LockInstruments()
{
	pthread_mutex_lock(&mutex_instruments);
}

void UserInterface::UnlockInstruments()
{
	pthread_mutex_unlock(&mutex_instruments);
}

void UserInterface::LockPost()
{
	pthread_mutex_lock(&mutex_post);
}

void UserInterface::UnlockPost()
{
	pthread_mutex_unlock(&mutex_post);
}

void UserInterface::Commit()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->Commit();
	}
}

void UserInterface::ProcessInputInstruments()
{
	if(unlikely(halt))
	{
		UnblockUser();
		return;
	}
	curr_time_stamp = sc_core::sc_time_stamp();
	if(unlikely(timed_step || (delta_steps == 1) || intr))
	{
		WaitForUser();
		LockInstruments();
		Commit();
		UnlockInstruments();
	}
	if(delta_steps > 1) delta_steps--;

	sc_core::sc_time t = timed_step ? user_step_time : ((delta_steps || intr || halt) ? sc_core::SC_ZERO_TIME : intr_poll_period);
	
	//std::cerr << sc_core::sc_time_stamp() << ": next input instrumentation after " << t << std::endl;
	NextInputTrigger(t);
}

void UserInterface::InitialFetch()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->InitialFetch();
	}
}

void UserInterface::Sample()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->Sample();
	}
}

void UserInterface::Fetch()
{
	has_breakpoint_cond = false;
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->Fetch();
		
		if(user_instrument->HasBreakpointCondition())
		{
			//std::cerr << "\"" << user_instrument->GetName() << "\" has breakpoint condition" << std::endl;
			intr = true;
			timed_step = false;
			delta_steps = 0;
			cont = false;
			has_breakpoint_cond = true;
			NextInputTrigger(sc_core::SC_ZERO_TIME);
		}
	}
}

void UserInterface::ProcessOutputInstruments()
{
	LockInstruments();
	Fetch();
	UnlockInstruments();
}

bool UserInterface::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
	{
		LockPost();
		LockInstruments();
		//std::cerr << "content length=" << req.GetContentLength() << std::endl;
		
		struct PropertySetter : public unisim::util::hypapp::Form_URL_Encoded_Decoder
		{
			PropertySetter(UserInterface& _user_interface)
				: user_interface(_user_interface)
			{
			}
			
			virtual bool FormAssign(const std::string& name, const std::string& value)
			{
				if(name.compare("step-time") == 0)
				{
					std::stringstream value_sstr(value);
					double time_value = 0.0;
					std::string time_unit;
					
					if((value_sstr >> time_value) && (value_sstr >> time_unit))
					{
						if(time_unit.compare("s") == 0) user_interface.user_step_time = sc_core::sc_time(time_value, sc_core::SC_SEC);
						else if(time_unit.compare("ms") == 0) user_interface.user_step_time = sc_core::sc_time(time_value, sc_core::SC_MS);
						else if(time_unit.compare("us") == 0) user_interface.user_step_time = sc_core::sc_time(time_value, sc_core::SC_US);
						else if(time_unit.compare("ns") == 0) user_interface.user_step_time = sc_core::sc_time(time_value, sc_core::SC_NS);
						else if(time_unit.compare("ps") == 0) user_interface.user_step_time = sc_core::sc_time(time_value, sc_core::SC_PS);
						else if(time_unit.compare("fs") == 0) user_interface.user_step_time = sc_core::sc_time(time_value, sc_core::SC_FS);
						else
						{
							user_interface.logger << DebugWarning << "malformed time unit ('" << time_unit << "'): expecting 's', 'ms', 'us', 'ns', 'ps', or 'fs'." << EndDebugWarning;
							user_interface.bad_user_step_time = true;
							return false;
						}
					}
					else
					{
						user_interface.logger << DebugWarning << "expecting a time in time-step" << EndDebugWarning;
						user_interface.bad_user_step_time = true;
						return false;
					}
					
					if(!value_sstr.eof())
					{
						user_interface.logger << DebugWarning << "ignoring extra characters after time step" << EndDebugWarning;
					}
				}
				else if((name.compare("timed-step") == 0) && (value.compare("on") == 0))
				{
					if(user_interface.verbose)
					{
						user_interface.logger << DebugInfo << "Timed Step" << EndDebugInfo;
					}
					user_interface.timed_step = true;
				}
				else if((name.compare("delta-step") == 0) && (value.compare("on") == 0))
				{
					if(user_interface.verbose)
					{
						user_interface.logger << DebugInfo << "Delta Step" << EndDebugInfo;
					}
					user_interface.delta_steps = 3;
				}
				else if((name.compare("cont") == 0) && (value.compare("on") == 0))
				{
					if(user_interface.verbose)
					{
						user_interface.logger << DebugInfo << "Continue" << EndDebugInfo;
					}
					user_interface.cont = true;
				}
				else if((name.compare("intr") == 0) && (value.compare("on") == 0))
				{
					if(user_interface.verbose)
					{
						user_interface.logger << DebugInfo << "Interrupt" << EndDebugInfo;
					}
					user_interface.intr = true;
				}
				else if((name.compare("halt") == 0) && (value.compare("on") == 0))
				{
					if(user_interface.verbose)
					{
						user_interface.logger << DebugInfo << "Halt" << EndDebugInfo;
					}
					user_interface.halt = true;
				}
				else
				{
					std::size_t delim_pos = name.find_first_of('*');
					if(delim_pos != std::string::npos)
					{
						std::string user_instrument_action = name.substr(0, delim_pos);
						std::string user_instrument_name = name.substr(delim_pos + 1);
						
						if(user_instrument_name == "all")
						{
							if(user_instrument_action == "enable")
							{
								user_interface.enable_all_input_instruments = true;
							}
							else if(user_instrument_action == "disable")
							{
								user_interface.disable_all_input_instruments = true;
							}
							else if(user_instrument_action == "enable-brkpt")
							{
								user_interface.enable_all_value_changed_breakpoints = true;
							}
							else if(user_instrument_action == "disable-brkpt")
							{
								user_interface.disable_all_value_changed_breakpoints = true;
							}
						}
						else
						{
							UserInstrument *user_instrument = user_interface.FindUserInstrument(user_instrument_name);
							
							if(user_instrument)
							{
								if(user_instrument_action == "enable")
								{
									user_interface.enable_input_instruments[user_instrument] = true;
								}
								else if(user_instrument_action == "enable-brkpt")
								{
									user_interface.enable_value_changed_breakpoints[user_instrument] = true;
								}
								else if(user_instrument_action == "set")
								{
									user_interface.set_input_instruments[user_instrument] = value;
								}
								else if(user_instrument_action == "toggle")
								{
									user_interface.toggle_input_instruments[user_instrument] = true;
								}
								else
								{
									user_interface.logger << DebugWarning << "unknown Action \"" << user_instrument_action << "\" on User instrument \"" << user_instrument_name << "\"" << EndDebugWarning;
									return false;
								}
							}
							else
							{
								user_interface.logger << DebugWarning << "Can't find User instrument \"" << user_instrument_name << "\"" << EndDebugWarning;
								return false;
							}
						}
					}
				}
				
				return true;
			}
			
			UserInterface& user_interface;
		};
		
		DisableInjection();
		DisableValueChangedBreakpoint();
		bad_user_step_time = false;
		timed_step = false;
		delta_steps = 0;
		cont = false;
		intr = false;
		halt = false;
		enable_all_input_instruments = false;
		disable_all_input_instruments = false;
		enable_all_value_changed_breakpoints = false;
		disable_all_value_changed_breakpoints = false;
		
		std::map<UserInstrument *, bool>::iterator enable_input_instrument_it;
		for(enable_input_instrument_it = enable_input_instruments.begin(); enable_input_instrument_it != enable_input_instruments.end(); enable_input_instrument_it++)
		{
			(*enable_input_instrument_it).second = false;
		}
		
		std::map<UserInstrument *, bool>::iterator enable_value_changed_breakpoint_it;
		for(enable_value_changed_breakpoint_it = enable_value_changed_breakpoints.begin(); enable_value_changed_breakpoint_it != enable_value_changed_breakpoints.end(); enable_value_changed_breakpoint_it++)
		{
			(*enable_value_changed_breakpoint_it).second = false;
		}

		std::map<UserInstrument *, std::string>::iterator set_input_instruments_it;
		for(set_input_instruments_it = set_input_instruments.begin(); set_input_instruments_it != set_input_instruments.end(); set_input_instruments_it++)
		{
			(*set_input_instruments_it).second = std::string();
		}

		std::map<UserInstrument *, bool>::iterator toggle_input_instrument_it;
		for(toggle_input_instrument_it = toggle_input_instruments.begin(); toggle_input_instrument_it != toggle_input_instruments.end(); toggle_input_instrument_it++)
		{
			(*toggle_input_instrument_it).second = false;
		}

		PropertySetter property_setter(*this);
		if(property_setter.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
		{
			if(enable_all_input_instruments || disable_all_input_instruments)
			{
				std::map<std::string, UserInstrument *>::iterator user_instrument_it;
				for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
				{
					UserInstrument *user_instrument = (*user_instrument_it).second;
					if(enable_all_input_instruments)
					{
						user_instrument->EnableInjection();
					}
					else if(disable_all_input_instruments)
					{
						user_instrument->DisableInjection();
					}
				}
			}
			else
			{
				std::map<UserInstrument *, bool>::iterator enable_input_instrument_it;
				for(enable_input_instrument_it = enable_input_instruments.begin(); enable_input_instrument_it != enable_input_instruments.end(); enable_input_instrument_it++)
				{
					UserInstrument *user_instrument = (*enable_input_instrument_it).first;
					bool enable_injection = (*enable_input_instrument_it).second;

					if(enable_injection)
					{
						user_instrument->EnableInjection();
					}
					else
					{
						user_instrument->DisableInjection();
					}
				}
			}
			
			if(enable_all_value_changed_breakpoints || disable_all_value_changed_breakpoints)
			{
				std::map<std::string, UserInstrument *>::iterator user_instrument_it;
				for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
				{
					UserInstrument *user_instrument = (*user_instrument_it).second;
					if(enable_all_value_changed_breakpoints)
					{
						user_instrument->EnableValueChangedBreakpoint();
					}
					else if(disable_all_value_changed_breakpoints)
					{
						user_instrument->DisableValueChangedBreakpoint();
					}
				}
			}
			else
			{
				std::map<UserInstrument *, bool>::iterator enable_value_changed_breakpoint_it;
				for(enable_value_changed_breakpoint_it = enable_value_changed_breakpoints.begin(); enable_value_changed_breakpoint_it != enable_value_changed_breakpoints.end(); enable_value_changed_breakpoint_it++)
				{
					UserInstrument *user_instrument = (*enable_value_changed_breakpoint_it).first;
					bool enable_injection = (*enable_value_changed_breakpoint_it).second;

					if(enable_injection)
					{
						user_instrument->EnableValueChangedBreakpoint();
					}
					else
					{
						user_instrument->DisableValueChangedBreakpoint();
					}
				}
			}
			
			std::map<UserInstrument *, std::string>::iterator set_input_instrument_it;
			for(set_input_instrument_it = set_input_instruments.begin(); set_input_instrument_it != set_input_instruments.end(); set_input_instrument_it++)
			{
				UserInstrument *user_instrument = (*set_input_instrument_it).first;
				
				std::map<UserInstrument *, bool>::iterator toggle_input_instrument_it = toggle_input_instruments.find(user_instrument);
				
				bool toggle = (toggle_input_instrument_it == toggle_input_instruments.end()) ? (*toggle_input_instrument_it).second : false;
				if(!toggle)
				{
					const std::string set_value = (*set_input_instrument_it).second;
					if(!set_value.empty())
					{
						if(verbose)
						{
							logger << DebugInfo << "set \"" << user_instrument->GetName() << "\" <- \"" << set_value << "\"" << EndDebugInfo;
						}
						user_instrument->Set(set_value);
						if(!cont && !timed_step) delta_steps = 3;
					}
				}
			}

			std::map<UserInstrument *, bool>::iterator toggle_input_instrument_it;
			for(toggle_input_instrument_it = toggle_input_instruments.begin(); toggle_input_instrument_it != toggle_input_instruments.end(); toggle_input_instrument_it++)
			{
				UserInstrument *user_instrument = (*toggle_input_instrument_it).first;
				bool toggle = (*toggle_input_instrument_it).second;

				if(toggle)
				{
					if(verbose)
					{
						logger << DebugInfo << "toggling \"" << user_instrument->GetName() << "\"" << EndDebugInfo;
					}
					user_instrument->Toggle();
					if(!cont && !timed_step) delta_steps = 3;
				}
			}
		}
		else
		{
			logger << DebugWarning << "parse error in POST data" << EndDebugWarning;
			timed_step = false;
			delta_steps = 0;
			cont = false;
			intr = false;
			halt = false;
		}
		
		UnlockInstruments();
		
		if(cont)
		{
			Run();
		}
		else if(timed_step || delta_steps || intr || halt)
		{
			WaitForSimulation();
		}
		
		UnlockPost();
	}
	
	std::ostringstream doc_sstr;
	
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	
	if(halt)
	{
		Stop(-1, /* asynchronous */ true);
		
		doc_sstr << "\t<head>" << std::endl;
		doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
		doc_sstr << "\t\t<script type=\"text/javascript\">document.domain=\"" << req.GetDomain() << "\";</script>" << std::endl;
		doc_sstr << "\t</head>" << std::endl;
		doc_sstr << "\t<body>" << std::endl;
		doc_sstr << "\t<p>Disconnected</p>" << std::endl;
		doc_sstr << "\t<script type=\"application/javascript\">Reload = function() { window.location.href=window.location.href; }</script>" << std::endl;
		doc_sstr << "\t<button onclick=\"Reload()\">Reconnect</button>" << std::endl;
		doc_sstr << "\t</body>" << std::endl;
	}
	else
	{
		doc_sstr << "\t<head>" << std::endl;
		doc_sstr << "\t\t<title>" << String_to_HTML(program_name) << " - " << String_to_HTML(GetName()) << "</title>" << std::endl;
		doc_sstr << "\t\t<meta name=\"description\" content=\"remote control interface over HTTP of virtual platform hardware signal instrumenter\">" << std::endl;
		doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
		doc_sstr << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
		doc_sstr << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/instrumenter/style.css\" type=\"text/css\" />" << std::endl;
		doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
		doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/instrumenter/script.js\"></script>" << std::endl;
		doc_sstr << "\t</head>" << std::endl;
		
		if(cont)
		{
			doc_sstr << "\t<body onload=\"setTimeout(reload_instrumenter_page, 1000)\">" << std::endl; // while in continue mode, reload page every seconds
		}
		else
		{
			doc_sstr << "\t<body>" << std::endl;
		}
		
		//doc_sstr << "\t\t<h1>" << String_to_HTML(program_name) << " - " << String_to_HTML(GetName()) << "</h1>" << std::endl;
		
		doc_sstr << "\t\t<table class=\"status-table\">" << std::endl;
		doc_sstr << "\t\t\t<thead>" << std::endl;
		doc_sstr << "\t\t\t\t<tr>" << std::endl;
		doc_sstr << "\t\t\t\t\t<th class=\"status\">Status</th>" << std::endl;
		doc_sstr << "\t\t\t\t\t<th class=\"time\">Time</th>" << std::endl;
		doc_sstr << "\t\t\t\t\t<th class=\"time\">(exactly)</th>" << std::endl;
		doc_sstr << "\t\t\t\t</tr>" << std::endl;
		doc_sstr << "\t\t\t</thead>" << std::endl;
		doc_sstr << "\t\t\t<tbody>" << std::endl;
		doc_sstr << "\t\t\t\t<tr>" << std::endl;
		doc_sstr << "\t\t\t\t\t<td class=\"status\">" << (bad_user_step_time ? "bad step time" : (cont ? "running" : "ready")) << "</td>" << std::endl;
		
		std::ios_base::fmtflags ff = doc_sstr.flags();
		doc_sstr.setf(std::ios::fixed);
		doc_sstr.precision(3);
		doc_sstr << "\t\t\t\t\t<td class=\"time\">" << curr_time_stamp.to_seconds() << " seconds</td>" << std::endl;
		doc_sstr.flags(ff);
		
		doc_sstr << "\t\t\t\t\t<td class=\"time\">(" << String_to_HTML(curr_time_stamp.to_string()) << ")</td>" << std::endl;
		doc_sstr << "\t\t\t\t</tr>" << std::endl;
		doc_sstr << "\t\t\t</tbody>" << std::endl;
		doc_sstr << "\t\t</table>" << std::endl;
		
		doc_sstr << "\t\t<form action=\"" << req.GetRequestURI() << "\" method=\"post\" enctype=\"application/x-www-form-urlencoded\">" << std::endl;
		doc_sstr << "\t\t\t<h2>Commands</h2>" << std::endl;
		doc_sstr << "\t\t\t<table class=\"command-table\">" << std::endl;
		doc_sstr << "\t\t\t\t<tbody>" << std::endl;
		doc_sstr << "\t\t\t\t\t<tr>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<td><button class=\"delta-step\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"delta-step\" value=\"on\"" << ((cont || halt) ? " disabled" : "") << ">&delta;</button></td>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<td><button class=\"timed-step\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"timed-step\" value=\"on\"" << ((cont || halt) ? " disabled" : "") << ">Step</button>&nbsp;by&nbsp;<input class=\"step-time\" type=\"text\" name=\"step-time\" value=\"" << user_step_time << "\"" << ((cont || halt) ? " disabled" : "") << "></td>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<td><button class=\"" << (cont ? "intr" : "cont") << "\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"" << (cont ? "intr" : "cont") << "\" value=\"on\"" << (halt ? " disabled" : "") << ">" << (cont ? "Interrupt" : "Continue") << "</button></td>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<td><button class=\"halt\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"halt\" value=\"on\"" << (halt ? " disabled" : "")  << ">Halt</button></td>" << std::endl;
		doc_sstr << "\t\t\t\t\t</tr>" << std::endl;
		doc_sstr << "\t\t\t\t</tbody>" << std::endl;
		doc_sstr << "\t\t\t</table>" << std::endl;
		doc_sstr << "\t\t\t<h2>Instruments</h2>" << std::endl;
		doc_sstr << "\t\t\t<table class=\"instruments-table1\">" << std::endl;
		doc_sstr << "\t\t\t\t<thead>" << std::endl;
		doc_sstr << "\t\t\t\t\t<tr>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<th class=\"signal-enable\">Enable<br><button class=\"signal-disable-all\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"disable*all\">C</button><button class=\"signal-enable-all\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"enable*all\">A</button></th>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<th class=\"signal-brkpt-enable\">Brkpt<br><button class=\"signal-brkpt-disable-all\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"disable-brkpt*all\">C</button><button class=\"signal-brkpt-enable-all\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"enable-brkpt*all\">A</button></th>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<th class=\"signal-name\">Hardware signal</th>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<th class=\"signal-toggle\">Toggle</th>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<th class=\"signal-value\">Value</th>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<th class=\"scrollbar\"></th>" << std::endl;
		doc_sstr << "\t\t\t\t\t</tr>" << std::endl;
		doc_sstr << "\t\t\t\t</thead>" << std::endl;
		doc_sstr << "\t\t\t\t<tbody>" << std::endl;
		doc_sstr << "\t\t\t\t\t<tr>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t<td colspan=\"6\">" << std::endl;
		doc_sstr << "\t\t\t\t\t\t\t<div class=\"scroller\">" << std::endl;
		doc_sstr << "\t\t\t\t\t\t\t\t<table class=\"instruments-table2\">" << std::endl;

		LockInstruments();
		if(likely(!has_breakpoint_cond))
		{
			Sample();
		}
		has_breakpoint_cond = false;

		std::stringstream sstr(instrumentation);
		
		std::vector<std::string>::iterator instrumented_signal_name_it;
		for(instrumented_signal_name_it = instrumented_signal_names.begin(); instrumented_signal_name_it != instrumented_signal_names.end(); instrumented_signal_name_it++)
		{
			const std::string& instrumented_signal_name = *instrumented_signal_name_it;

			UserInstrument *user_instrument = FindUserInstrument(instrumented_signal_name);
			
			if(user_instrument)
			{
				std::string value;
				user_instrument->Get(value);
				bool is_boolean = user_instrument->IsBoolean();
				bool bool_value = false;
				if(is_boolean) user_instrument->Get(bool_value);
				doc_sstr << "\t\t\t\t\t\t\t\t\t\t<tr class=\"signal" << (user_instrument->HasBreakpointCondition() ? " brkpt-cond" : "") << "\">" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t\t\t\t\t<td class=\"signal-enable\"><input class=\"signal-enable-checkbox\" type=\"checkbox\" name=\"enable*" << String_to_HTML(user_instrument->GetName()) << "\"" << (user_instrument->IsInjectionEnabled() ? " checked" : "") << (user_instrument->IsReadOnly() ? " disabled" : "") << "></td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t\t\t\t\t<td class=\"signal-brkpt-enable\"><input class=\"signal-brkpt-enable-checkbox\" type=\"checkbox\" name=\"enable-brkpt*" << String_to_HTML(user_instrument->GetName()) << "\"" << (user_instrument->IsValueChangedBreakpointEnabled() ? " checked" : "") << "></td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t\t\t\t\t<td class=\"signal-name\">" << String_to_HTML(user_instrument->GetName()) << "</td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t\t\t\t\t<td class=\"signal-toggle\">";
				if(is_boolean)
				{
					doc_sstr << "<button class=\"signal-toggle-button signal-" << (bool_value ? "on" : "off") << "\" type=\"submit\" onclick=\"save_instrumenter_scroll_top()\" name=\"toggle*" << String_to_HTML(user_instrument->GetName()) << "\"" << ((cont || halt) ? " disabled" : "") << (user_instrument->IsReadOnly() ? " readonly" : "") << ">" << (bool_value ? "on" : "off")  << "</button>";
				}
				doc_sstr << "</td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t\t\t\t\t<td class=\"signal-value\"><input class=\"signal-value-text" << (user_instrument->IsReadOnly() ? " disabled" : "") << "\" type=\"text\" name=\"set*" << String_to_HTML(user_instrument->GetName()) << "\" value=\"" << String_to_HTML(value) << "\"" << ((cont || halt) ? " disabled" : "") << (user_instrument->IsReadOnly() ? " readonly" : "") << "></td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t\t\t\t</tr>" << std::endl;
			}
		}

		UnlockInstruments();
		
		doc_sstr << "\t\t\t\t\t\t\t\t</table>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t\t</div>" << std::endl;
		doc_sstr << "\t\t\t\t\t\t</td>" << std::endl;
		doc_sstr << "\t\t\t\t\t</tr>" << std::endl;
		doc_sstr << "\t\t\t\t</tbody>" << std::endl;
		doc_sstr << "\t\t\t</table>" << std::endl;
		doc_sstr << "\t\t</form>" << std::endl;
		
		doc_sstr << "\t</body>" << std::endl;
	}
		
	doc_sstr << "</html>" << std::endl;
	doc_sstr << std::endl;
	
	std::string doc(doc_sstr.str());
		
	std::ostringstream http_header_sstr;
	if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
	{
		http_header_sstr << "HTTP/1.1 201 Created\r\n";
	}
	else
	{
		http_header_sstr << "HTTP/1.1 200 OK\r\n";
	}
		
	http_header_sstr << "Server: UNISIM-VP\r\n"
					<< "Cache-control: no-cache\r\n"
					<< "Connection: keep-alive\r\n"
					<< "Content-length: " << doc.length() << "\r\n"
					<< "Content-Type: text/html; charset=utf-8\r\n"
					<< "\r\n";
	
	std::string http_header(http_header_sstr.str());

	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: " << std::endl << http_header << EndDebugInfo;
	}
	if(!conn.Send(http_header.c_str(), http_header.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP header" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: done" << EndDebugInfo;
	}
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::HEAD) return true;
			
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: " << std::endl << doc << EndDebugInfo;
	}
	
	if(!conn.Send(doc.c_str(), doc.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response body" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: done" << EndDebugInfo;
	}
	
	return true;
}

UserInstrument *UserInterface::FindUserInstrument(const std::string& name)
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it = user_instruments.find(name);
	
	if(user_instrument_it != user_instruments.end())
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		return user_instrument;
	}
	
	return 0;
}

void UserInterface::EnableInjection()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->EnableInjection();
	}
}

void UserInterface::DisableInjection()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->DisableInjection();
	}
}

void UserInterface::EnableValueChangedBreakpoint()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->EnableValueChangedBreakpoint();
	}
}

void UserInterface::DisableValueChangedBreakpoint()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->DisableValueChangedBreakpoint();
	}
}

CSV_Reader::CSV_Reader(const char *name, Instrumenter *instrumenter)
	: unisim::kernel::service::Object(name, instrumenter)
	, InstrumenterFrontEnd(name, instrumenter)
	, logger(*this)
	, filename()
	, param_filename("filename", this, filename, "name of CSV input file")
	, instrumentation()
	, param_instrumentation("instrumentation", this, instrumentation, "Instrumented signals (wildcards '*' and '?' are allowed in signal names) that take there input from the CSV file")
	, instrumentation_start_time(sc_core::SC_ZERO_TIME)
	, param_instrumentation_start_time("instrumentation-start-time", this, instrumentation_start_time, "Start time of instrumentation")
	, instrumentation_end_time(sc_core::sc_max_time())
	, param_instrumentation_end_time("instrumentation-end-time", this, instrumentation_end_time, "End time of instrumentation")
	, csv_delimiter(",")
	, param_csv_delimiter("csv-delimiter", this, csv_delimiter, "delimiter in CSV file")
	, file(0)
	, time_resolution(sc_core::SC_ZERO_TIME)
	, csv_instrument_map()
	, csv_time_stamp(sc_core::SC_ZERO_TIME)
{
	file = new std::ifstream(filename.c_str());
		
	if(file->fail())
	{
		file = 0;
		logger << DebugWarning << "Can't open File \"" << filename << "\"" << EndDebugWarning;
		return;
	}

	ParseCSVHeaderAndInstrumentInput(0);
}

CSV_Reader::~CSV_Reader()
{
	if(file) delete file;
}

bool CSV_Reader::SetupInstrumentation()
{
	if(!file) return false;
		
	if(ParseCSVHeaderAndInstrumentInput(1))
	{
		do
		{
			if(!ParseCSV(csv_time_stamp)) return false;
		}
		while(csv_time_stamp < instrumentation_start_time);
		
		instrumentation_start_time = csv_time_stamp; // crop interval
	}
	
	return true;
}

void CSV_Reader::ProcessInputInstruments()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if((time_stamp >= instrumentation_start_time) && (time_stamp <= instrumentation_end_time))
	{
		if(csv_time_stamp == time_stamp)
		{
			unsigned int n = csv_instrument_map.size();
			unsigned int i;
			
			for(i = 0; i < n; i++)
			{
				InputInstrumentBase *input_instrument = csv_instrument_map[i];
				
				//std::cerr << "#" << i << ": Instrument " << (input_instrument ? input_instrument->GetName() : "unknown") << std::endl;
				if(input_instrument)
				{
					input_instrument->Inject();
				}
			}
		}
	
		if(ParseCSV(csv_time_stamp) && (csv_time_stamp >= time_stamp))
		{
			sc_core::sc_time time_to_next_input_instrumentation(csv_time_stamp);
			time_to_next_input_instrumentation -= time_stamp;
			NextInputTrigger(time_to_next_input_instrumentation);
		}
		else
		{
			instrumentation_end_time = time_stamp; // stop instrumentation as there're no more data
		}
	}
}

void CSV_Reader::ProcessOutputInstruments()
{
}

bool CSV_Reader::ParseCSVHeaderAndInstrumentInput(int pass)
{
	if(file)
	{
		switch(pass)
		{
			case 0:
				break;
				
			case 1:
				csv_instrument_map.clear();
				break;
		}
		
		std::string line;
		
		if(std::getline(*file, line))
		{
			unsigned int csv_column = 0;
			std::size_t pos = 0;
			std::size_t delim_pos = 0;
			
			do
			{
				delim_pos = line.find(csv_delimiter, pos);
				
				std::string value;
				if(delim_pos != std::string::npos)
				{
					value = line.substr(pos, delim_pos - pos);
				}
				else
				{
					value = line.substr(pos);
				}
				
				//std::cerr << "value is \"" << value << "\"" << std::endl;
				if(csv_column)
				{
					switch(pass)
					{
						case 0:
							PrepareInstrumentation(value, INPUT_INSTRUMENTATION);
							break;
						case 1:
						{
							InputInstrumentBase *input_instrument = InstrumentInputSignal(value);
							//std::cerr << "CSV column " << csv_column << " is for instrument " << (input_instrument ? input_instrument->GetName() : "unknown") << std::endl;
							
							csv_instrument_map.resize(csv_column);
							csv_instrument_map[csv_column - 1] = input_instrument;
							break;
						}
					}
				}
				else
				{
					switch(pass)
					{
						case 0:
							break;
							
						case 1:
						{
							std::stringstream value_sstr(value);
							double time_value;
							std::string time_unit;
							
							if((value_sstr >> time_value) && (value_sstr >> time_unit))
							{
								if(time_unit.compare("s") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_SEC);
								else if(time_unit.compare("ms") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_MS);
								else if(time_unit.compare("us") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_US);
								else if(time_unit.compare("ns") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_NS);
								else if(time_unit.compare("ps") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_PS);
								else if(time_unit.compare("fs") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_FS);
								else
								{
									logger << DebugWarning << "In input CSV file, first line, first column, malformed time_unit ('" << time_unit << "'): expecting 's', 'ms', 'us', 'ns', 'ps', or 'fs'." << EndDebugWarning;
									return false;
								}
							}
							else
							{
								logger << DebugWarning << "expecting a time resolution in first line, first column (e.g. 1 ps) of input CSV file" << EndDebugWarning;
								return false;
							}
							
							if(!value_sstr.eof())
							{
								logger << DebugWarning << "ignoring extra characters after time resolution in first line, first column of input CSV file" << EndDebugWarning;
							}
							
							break;
						}
					}
				}
				
				pos = delim_pos + csv_delimiter.length(); // skip value and delimiter
				csv_column++;
			}
			while(delim_pos != std::string::npos);
			
			return true;
		}
	}
	
	return false;
}

bool CSV_Reader::ParseCSV(sc_core::sc_time& deadline)
{
	if(file)
	{
		std::string line;
		
		if(std::getline(*file, line))
		{
			unsigned int csv_column = 0;
			std::size_t pos = 0;
			std::size_t delim_pos = 0;
			
			do
			{
				delim_pos = line.find(csv_delimiter, pos);
				
				std::string value;
				if(delim_pos != std::string::npos)
				{
					value = line.substr(pos, delim_pos - pos);
				}
				else
				{
					value = line.substr(pos);
				}
				
				//std::cerr << "value is \"" << value << "\"" << std::endl;
				if(csv_column)
				{
					InputInstrumentBase *input_instrument = csv_instrument_map[csv_column - 1];
					
					if(input_instrument)
					{
						//std::cerr << "value is for " << input_instrument->GetName() << std::endl;
						std::stringstream value_sstr(value);
						input_instrument->Input(value_sstr);
					}
				}
				else
				{
					std::stringstream value_sstr(value);
					sc_dt::uint64 time_value;
					
					if(value_sstr >> time_value)
					{
						deadline = time_resolution;
						deadline *= time_value;
					}
					else
					{
						deadline = sc_core::sc_max_time();
						return false;
					}
				}
				
				pos = delim_pos + csv_delimiter.length(); // skip value and delimiter
				csv_column++;
			}
			while(delim_pos != std::string::npos);
			
			return true;
		}
	}
	return false;
}

CSV_Writer::CSV_Writer(const char *name, Instrumenter *instrumenter)
	: unisim::kernel::service::Object(name, instrumenter)
	, InstrumenterFrontEnd(name, instrumenter)
	, logger(*this)
	, filename()
	, param_filename("filename", this, filename, "name of CSV output file")
	, instrumentation()
	, param_instrumentation("instrumentation", this, instrumentation, "Instrumented signals (wildcards '*' and '?' are allowed in signal names) to record in the CSV file")
	, instrumentation_start_time(sc_core::SC_ZERO_TIME)
	, param_instrumentation_start_time("instrumentation-start-time", this, instrumentation_start_time, "Start time of instrumentation")
	, instrumentation_end_time(sc_core::sc_max_time())
	, param_instrumentation_end_time("instrumentation-end-time", this, instrumentation_end_time, "End time of instrumentation")
	, csv_delimiter(",")
	, param_csv_delimiter("csv-delimiter", this, csv_delimiter, "delimiter in CSV file")
	, file(0)
	, last_instrument_sampling_time_stamp(sc_core::SC_ZERO_TIME)
	, force_output(true)
{
	std::stringstream sstr(instrumentation);
	
	std::string signal_name_pattern;
	while(sstr >> signal_name_pattern)
	{
		PrepareInstrumentation(signal_name_pattern, OUTPUT_INSTRUMENTATION);
	}
}

CSV_Writer::~CSV_Writer()
{
	if(file)
	{
		const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
		if((time_stamp >= instrumentation_start_time) && (time_stamp <= instrumentation_end_time))
		{
			if(time_stamp > last_instrument_sampling_time_stamp)
			{
				OutputInstrumentsAsCSV(*file, last_instrument_sampling_time_stamp);
			}
		}
		
		delete file;
	}
}

bool CSV_Writer::SetupInstrumentation()
{
	file = new std::ofstream(filename.c_str());
	
	if(file->fail())
	{
		file = 0;
		logger << DebugWarning << "Can't open File \"" << filename << "\"" << EndDebugWarning;
		return false;
	}
	
	std::stringstream sstr(instrumentation);
	std::string signal_name_pattern;

	while(sstr >> signal_name_pattern)
	{
		std::vector<std::string> instrumented_signal_names;
		MatchSignalPattern(signal_name_pattern, instrumented_signal_names);
		
		std::vector<std::string>::iterator instrumented_signal_name_it;
		for(instrumented_signal_name_it = instrumented_signal_names.begin(); instrumented_signal_name_it != instrumented_signal_names.end(); instrumented_signal_name_it++)
		{
			const std::string& instrumented_signal_name = *instrumented_signal_name_it;
		
			InstrumentOutputSignal(instrumented_signal_name);
		}
	}
	
	last_instrument_sampling_time_stamp = sc_core::SC_ZERO_TIME;
	
	OutputInstrumentsNamesAsCSV(*file);
	
	return true;
}

void CSV_Writer::ProcessInputInstruments()
{
}

void CSV_Writer::ProcessOutputInstruments()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if((time_stamp >= instrumentation_start_time) && (time_stamp <= instrumentation_end_time))
	{
		if(time_stamp > last_instrument_sampling_time_stamp)
		{
			OutputInstrumentsAsCSV(*file, last_instrument_sampling_time_stamp);
			LatchInstruments();
		}
		
		SampleInstruments();
		last_instrument_sampling_time_stamp = time_stamp;
	}
}

void CSV_Writer::SampleInstruments()
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		output_instrument->Sample();
	}
}

void CSV_Writer::LatchInstruments()
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		output_instrument->Latch();
	}
}

void CSV_Writer::OutputInstrumentsNamesAsCSV(std::ostream& os)
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;

	os << sc_core::sc_get_time_resolution();
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		os << csv_delimiter << output_instrument->GetName();
	}
	os << std::endl;
}

void CSV_Writer::OutputInstrumentsAsCSV(std::ostream& os, const sc_core::sc_time& time_stamp)
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	os << last_instrument_sampling_time_stamp.value();
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		os << csv_delimiter;
		if(force_output || output_instrument->ValueChanged())
		{
			output_instrument->Output(os);
			force_output = false;
		}
	}
	os << std::endl;
}

} // end of namespace instrumenter
} // end of namespace service
} // end of namespace unisim
