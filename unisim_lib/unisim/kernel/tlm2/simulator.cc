/*
 *  Copyright (c) 2015-2016,
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

#include <unisim/kernel/tlm2/simulator.hh>

namespace unisim {
namespace kernel {
namespace tlm2 {

Instrumenter::Instrumenter(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, enable_output_instrumentation(false)
	, param_enable_output_instrumentation("enable-output-instrumentation", this, enable_output_instrumentation, "Enable/Disable output instrumentation of signals")
	, instrumentation_output_filename()
	, param_instrumentation_output_filename("instrumentation-output-filename", this, instrumentation_output_filename, "Output filename for instrumented signals")
	, output_instrumentation()
	, param_output_instrumentation("output-instrumentation", this, output_instrumentation, "Instrumented signals to output in the instrumentation output file")
	, output_instrumentation_start_time(sc_core::SC_ZERO_TIME)
	, param_output_instrumentation_start_time("output-instrumentation-start-time", this, output_instrumentation_start_time, "Start time for output instrumentation of signals")
	, output_instrumentation_end_time(sc_core::sc_max_time())
	, param_output_instrumentation_end_time("output-instrumentation-end-time", this, output_instrumentation_end_time, "End time for output instrumentation of signals")
	, output_csv_delimiter(",")
	, param_output_csv_delimiter("output-csv-delimiter", this, output_csv_delimiter, "CSV delimiter for instrumentation output file")
	, enable_input_instrumentation(false)
	, param_enable_input_instrumentation("enable-input-instrumentation", this, enable_input_instrumentation, "Enable/Disable input instrumentation of signals")
	, instrumentation_input_filename()
	, param_instrumentation_input_filename("instrumentation-input-filename", this, instrumentation_input_filename, "Input filename for instrumented signals")
	, input_instrumentation()
	, param_input_instrumentation("input-instrumentation", this, input_instrumentation, "Instrumented signals (wildcard '*' is allowed in signal names) that take there input from the instrumentation input file")
	, input_instrumentation_start_time(sc_core::SC_ZERO_TIME)
	, param_input_instrumentation_start_time("input-instrumentation-start-time", this, input_instrumentation_start_time, "Start time for input instrumentation of signals")
	, input_instrumentation_end_time(sc_core::sc_max_time())
	, param_input_instrumentation_end_time("input-instrumentation-end-time", this, input_instrumentation_end_time, "End time for input instrumentation of signals")
	, input_csv_delimiter(",")
	, param_input_csv_delimiter("input-csv-delimiter", this, input_csv_delimiter, "CSV delimiter for instrumentation input file")
	, vcd_trace_filename()
	, param_vcd_trace_filename("vcd-trace-file", this, vcd_trace_filename, "VCD output filename for signals trace")
	, trace_signals()
	, param_trace_signals("trace-signals", this, trace_signals, "Signals to trace (wildcard '*' is allowed in signal names)")
	, enable_trace_signals(false)
	, param_enable_trace_signals("enable-trace-signals", this, enable_trace_signals, "Enable/Disable signals trace")
	, trace_file(0)
	, gtkwave_init_script()
	, param_gtkwave_init_script("gtkwave-init-script", this, gtkwave_init_script, "GTKWave initialization script that simulators must automatically generate at startup")
	, signal_pool()
	, port_pool()
	, netlist()
	, input_instrumentation_set()
	, instrumentation_output_file(0)
	, instrumentation_input_file(0)
	, output_instruments()
	, input_instruments()
	, output_instrumentation_process_spawn_options()
	, last_instrument_sampling_time_stamp(sc_core::SC_ZERO_TIME)
	, input_time_resolution(sc_core::sc_get_time_resolution())
	, input_csv_instrument_map()
	, input_time_stamp(sc_core::SC_ZERO_TIME)
	, next_input_instrumentation_event("next_input_instrumentation_event")
{
	if(enable_input_instrumentation && !instrumentation_input_filename.empty())
	{
		std::stringstream sstr(input_instrumentation);
		std::string instrumented_signal_name;

		while(sstr >> instrumented_signal_name)
		{
			input_instrumentation_set.insert(instrumented_signal_name);
		}
	}
}

Instrumenter::~Instrumenter()
{
	if(trace_file)
	{
		sc_close_vcd_trace_file(trace_file);
	}

	if(enable_output_instrumentation && instrumentation_output_file)
	{
		if((last_instrument_sampling_time_stamp >= output_instrumentation_start_time) && (last_instrument_sampling_time_stamp <= output_instrumentation_end_time))
		{
			OutputInstrumentsAsCSV(*instrumentation_output_file, last_instrument_sampling_time_stamp);
		}
		
		delete instrumentation_output_file;
	}

	std::vector<OutputInstrumentBase *>::size_type num_output_instruments = output_instruments.size();
	std::vector<OutputInstrumentBase *>::size_type output_instrument_num;
	
	for(output_instrument_num = 0; output_instrument_num < num_output_instruments; output_instrument_num++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[output_instrument_num];
		
		delete output_instrument;
	}
	
	std::vector<InputInstrumentBase *>::size_type num_input_instruments = input_instruments.size();
	std::vector<InputInstrumentBase *>::size_type input_instrument_num;
	
	for(input_instrument_num = 0; input_instrument_num < num_input_instruments; input_instrument_num++)
	{
		InputInstrumentBase *input_instrument = input_instruments[input_instrument_num];
		
		delete input_instrument;
	}

	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		delete signal_if; 
	}

	std::map<std::string, TyperBase *>::iterator typer_it;
	
	for(typer_it = typers.begin(); typer_it != typers.end(); typer_it++)
	{
		TyperBase *typer = (*typer_it).second;
		delete typer;
	}
}

bool Instrumenter::EndSetup()
{
	StartInstrumentation();
	StartBinding();
	
	return true;
}

sc_core::sc_clock& Instrumenter::CreateClock(const std::string& clock_name, const sc_core::sc_time& clock_period, double clock_duty_cycle, const sc_core::sc_time& clock_start_time, bool clock_posedge_first)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(clock_name);
	if(signal_pool_it != signal_pool.end())
	{
		throw std::runtime_error("Internal error! clock already exists");
	}

	sc_core::sc_clock *clock = new sc_core::sc_clock(clock_name.c_str(), clock_period, clock_duty_cycle, clock_start_time, clock_posedge_first);
	std::cout << "Creating clock \"" << clock->name() << "\" with a period of " << clock_period << ", a duty cycle of " << clock_duty_cycle << ", starting with " << (clock_posedge_first ? "rising" : "falling") << " edge at " << clock_start_time << std::endl;
	
	signal_pool[clock->name()] = clock;
	
	typers[clock->name()] = new Typer<bool>(this);

	return *clock;
}

void Instrumenter::GenerateGTKWaveInitScript(const std::string& filename, int zoom_factor) const
{
	std::ofstream of(filename.c_str(), std::ofstream::out);
	
	if(of.fail())
	{
		std::cerr << "WARNING! Can't open output \"" << filename << "\"" << std::endl;
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
	std::cout << "Registering Port \"" << port.name() << "\"" << std::endl;
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
		std::cerr << "WARNING! Can't trace Signal \"" << signal_name_pattern << "\"" << std::endl;
	}
}

void Instrumenter::Bind(const std::string& port_name, const std::string& signal_name)
{
	if(netlist.find(port_name) != netlist.end())
	{
		std::cerr << "WARNING! port \"" << port_name << "\" is already bound" << std::endl;
		return;
	}
	
	netlist[port_name] = signal_name;
}

void Instrumenter::BindArray(const std::string& port_array_name, const std::string& signal_array_name, unsigned int begin_idx, unsigned int end_idx)
{
	unsigned int array_index;
	
	for(array_index = begin_idx; array_index <= end_idx; array_index++)
	{
		std::stringstream port_name_sstr;
		port_name_sstr << port_array_name << "_" << array_index;
		std::string port_name(port_name_sstr.str());

		std::stringstream signal_name_sstr;
		signal_name_sstr << signal_array_name << "_" << array_index;
		std::string signal_name(signal_name_sstr.str());
		
		Bind(port_name, signal_name);
	}
}

bool Instrumenter::Match(const std::string& p, const std::string& s) const
{
	std::size_t s_pos = 0;
	std::size_t p_pos = 0;
	std::size_t s_l = s.length();
	std::size_t p_l = p.length();
	int state = 0;
	
	while(state >= 0)
	{
		switch(state)
		{
			case 0: // 1-to-1 character matching
				if((p_pos >= p_l) && (s_pos >= s_l))
				{
					// end of scan
					state = -2; // match
				}
				else if(p_pos >= p_l)
				{
					// end of pattern but not end of string
					state = -1; // !match
				}
				else if(p[p_pos] == '*')
				{
					// pattern has a wildcard
					p_pos++; // skip wildcard
					state = 1; // continue with wildcard matching
				}
				else if(s_pos >= s_l)
				{
					// end of string but not end of pattern and pattern has no wildcard at that position
					state = -1; // !match
				}
				else if(s[s_pos] != p[p_pos])
				{
					// characters differs
					state = -1; // !match
				}
				else
				{
					// characters match
					p_pos++;
					s_pos++;
				}
				break;
			case 1: // wildcard matching
				if(p_pos >= p_l)
				{
					// wildcard at the end of pattern, no need to scan all remaining characters in string
					state = -2; // match
				}
				else if(s_pos >= s_l)
				{
					// wildcard in the middle but end of string
					state = -1; // !match
				}
				else if(s[s_pos] == p[p_pos])
				{
					// character match after wildcard
					p_pos++;
					s_pos++;
					state = 0; // continue 1-to-1 character matching
				}
				else
				{
					s_pos++; // eat string character (wildcard)
				}
				break;
		}
	}
	
	return (state == -2);
}

void Instrumenter::StartBinding()
{
	std::map<std::string, std::string>::iterator netlist_it;
	
	for(netlist_it = netlist.begin(); netlist_it != netlist.end(); netlist_it++)
	{
		const std::string& port_name = (*netlist_it).first;
		const std::string& signal_name = (*netlist_it).second;

#if 0
		bool status = TryBind<bool>(port_name, signal_name);
		if(!status) status = TryBind<sc_dt::sc_uint<32> >(port_name, signal_name);
		if(!status) status = TryBind<sc_dt::sc_uint<3> >(port_name, signal_name);
		if(!status) status = TryBind<sc_dt::sc_uint<2> >(port_name, signal_name);
		
		if(status)
		{
			std::cout << "Connecting Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << std::endl;
		}
		else
		{
			std::cerr << "WARNING! Can't connect Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << std::endl;
		}
#endif
		
		std::map<std::string, TyperBase *>::iterator typer_it = typers.find(port_name);
		
		if(typer_it != typers.end())
		{
			TyperBase *typer = (*typer_it).second;
			
			if(typer->TryBind(port_name, signal_name))
			{
				std::cout << "Connecting Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << std::endl;
				continue;
			}
		}
		
		std::cerr << "WARNING! Can't connect Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << std::endl;
	}
}

void Instrumenter::StartInstrumentation()
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

	if(enable_output_instrumentation && !instrumentation_output_filename.empty())
	{
		instrumentation_output_file = new std::ofstream(instrumentation_output_filename.c_str());
		
		std::stringstream sstr(output_instrumentation);
		std::string instrumented_signal_name;

		while(sstr >> instrumented_signal_name)
		{
			InstrumentOutputSignal(instrumented_signal_name);
		}
		
		last_instrument_sampling_time_stamp = sc_core::SC_ZERO_TIME;
		
		OutputInstrumentsNamesAsCSV(*instrumentation_output_file);

		output_instrumentation_process_spawn_options.spawn_method();
		sc_core::sc_spawn(sc_bind(&Instrumenter::OutputInstrumentationProcess, this), "OutputInstrumentationProcess", &output_instrumentation_process_spawn_options);
	}

	if(enable_input_instrumentation && !instrumentation_input_filename.empty())
	{
		instrumentation_input_file = new std::ifstream(instrumentation_input_filename.c_str());
		
		if(ParseCSVHeaderAndInstrumentInput())
		{
			bool status = true;
			
			do
			{
				status = ParseCSV();
			}
			while(status && (input_time_stamp < input_instrumentation_start_time));
			
			input_instrumentation_start_time = input_time_stamp; // crop interval
			
			if(status)
			{
				sc_core::sc_spawn_options input_instrumentation_process_spawn_options;
				input_instrumentation_process_spawn_options.spawn_method();
				input_instrumentation_process_spawn_options.set_sensitivity(&next_input_instrumentation_event);
				
				sc_core::sc_spawn(sc_bind(&Instrumenter::InputInstrumentationProcess, this), "InputInstrumentationProcess", &input_instrumentation_process_spawn_options);
				
				next_input_instrumentation_event.notify(input_time_stamp);
			}
		}
	}
}

bool Instrumenter::IsInInputInstrumentSet(const std::string& name) const
{
	std::set<std::string>::const_iterator it;
	for(it = input_instrumentation_set.begin(); it != input_instrumentation_set.end(); it++)
	{
		const std::string& pattern = *it;
		if(Match(pattern, name))
		{
			return true;
		}
	}
	return false;
}

void Instrumenter::SampleInstruments(const sc_core::sc_time& time_stamp)
{
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *instrument = output_instruments[i];
		
		instrument->Sample();
	}
	
	last_instrument_sampling_time_stamp = time_stamp;
}

void Instrumenter::OutputInstrumentsNamesAsCSV(std::ostream& os)
{
	unsigned int n = output_instruments.size();
	unsigned int i;

	os << sc_core::sc_get_time_resolution();
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *instrument = output_instruments[i];
		
		os << output_csv_delimiter << instrument->GetName();
	}
	os << std::endl;
}

void Instrumenter::OutputInstrumentsAsCSV(std::ostream& os, const sc_core::sc_time& time_stamp)
{
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	bool force_output = (time_stamp == output_instrumentation_start_time);
	
	os << time_stamp.value();
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *instrument = output_instruments[i];
		
		os << output_csv_delimiter;
		instrument->Output(os, force_output);
	}
	os << std::endl;
}

void Instrumenter::OutputInstrumentationProcess()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if((time_stamp >= output_instrumentation_start_time) && (time_stamp <= output_instrumentation_end_time))
	{
		if(time_stamp > last_instrument_sampling_time_stamp)
		{
			OutputInstrumentsAsCSV(*instrumentation_output_file, last_instrument_sampling_time_stamp);
		}
		
		SampleInstruments(time_stamp);
	}
}

bool Instrumenter::ParseCSVHeaderAndInstrumentInput()
{
	input_csv_instrument_map.clear();

	if(instrumentation_input_file)
	{
		std::string line;
		
		if(std::getline(*instrumentation_input_file, line))
		{
			unsigned int csv_column = 0;
			std::size_t pos = 0;
			std::size_t delim_pos = 0;
			
			do
			{
				delim_pos = line.find(input_csv_delimiter, pos);
				
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
					InputInstrumentBase *instrument = 0;
					
					if(InstrumentInputSignal(value))
					{
						instrument = FindInputInstrument(value);
					}
					//std::cerr << "CSV column " << csv_column << " is for instrument " << (instrument ? instrument->GetName() : "unknown") << std::endl;
					
					input_csv_instrument_map.resize(csv_column);
					input_csv_instrument_map[csv_column - 1] = instrument;
				}
				else
				{
					std::stringstream value_sstr(value);
					double time_value;
					std::string time_unit;
					
					if((value_sstr >> time_value) && (value_sstr >> time_unit))
					{
						if(time_unit.compare("s") == 0) input_time_resolution = sc_core::sc_time(time_value, sc_core::SC_SEC);
						else if(time_unit.compare("ms") == 0) input_time_resolution = sc_core::sc_time(time_value, sc_core::SC_MS);
						else if(time_unit.compare("us") == 0) input_time_resolution = sc_core::sc_time(time_value, sc_core::SC_US);
						else if(time_unit.compare("ns") == 0) input_time_resolution = sc_core::sc_time(time_value, sc_core::SC_NS);
						else if(time_unit.compare("ps") == 0) input_time_resolution = sc_core::sc_time(time_value, sc_core::SC_PS);
						else if(time_unit.compare("fs") == 0) input_time_resolution = sc_core::sc_time(time_value, sc_core::SC_FS);
						else
						{
							std::cerr << "WARNING! In input CSV file, first line, first column, malformed time_unit ('" << time_unit << "'): expecting 's', 'ms', 'us', 'ns', 'ps', or 'fs'." << std::endl;
							return false;
						}
					}
					else
					{
						std::cerr << "WARNING! expecting a time resolution in first line, first column (e.g. 1 ps) of input CSV file" << std::endl;
						return false;
					}
					
					if(!value_sstr.eof())
					{
						std::cerr << "WARNING! ignoring extra characters after time resolution in first line, first column of input CSV file" << std::endl;
					}
				}
				
				pos = delim_pos + input_csv_delimiter.length(); // skip value and delimiter
				csv_column++;
			}
			while(delim_pos != std::string::npos);
			
			return true;
		}
	}
	
	return false;
}

bool Instrumenter::ParseCSV()
{
	if(instrumentation_input_file)
	{
		std::string line;
		
		if(std::getline(*instrumentation_input_file, line))
		{
			unsigned int csv_column = 0;
			std::size_t pos = 0;
			std::size_t delim_pos = 0;
			
			do
			{
				delim_pos = line.find(input_csv_delimiter, pos);
				
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
					InputInstrumentBase *instrument = input_csv_instrument_map[csv_column - 1];
					
					if(instrument)
					{
						//std::cerr << "value is for " << instrument->GetName() << std::endl;
						std::stringstream value_sstr(value);
						instrument->Input(value_sstr);
					}
				}
				else
				{
					std::stringstream value_sstr(value);
					sc_dt::uint64 time_value;
					
					if(value_sstr >> time_value)
					{
						input_time_stamp = input_time_resolution;
						input_time_stamp *= time_value;
					}
					else
					{
						input_time_stamp = sc_core::sc_max_time();
						return false;
					}
				}
				
				pos = delim_pos + input_csv_delimiter.length(); // skip value and delimiter
				csv_column++;
			}
			while(delim_pos != std::string::npos);
			
			return true;
		}
	}
	return false;
}

void Instrumenter::InputInstrumentationProcess()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if((time_stamp >= input_instrumentation_start_time) && (time_stamp <= input_instrumentation_end_time))
	{
		if(input_time_stamp == time_stamp)
		{
			unsigned int n = input_csv_instrument_map.size();
			unsigned int i;
			
			for(i = 0; i < n; i++)
			{
				InputInstrumentBase *instrument = input_csv_instrument_map[i];
				
				//std::cerr << "#" << i << ": Instrument " << (instrument ? instrument->GetName() : "unknown") << std::endl;
				if(instrument)
				{
					instrument->Inject();
				}
			}
		}
	
		if(ParseCSV() && (input_time_stamp >= time_stamp))
		{
			sc_core::sc_time time_to_next_input_instrumentation(input_time_stamp);
			time_to_next_input_instrumentation -= time_stamp;
			next_input_instrumentation_event.notify(time_to_next_input_instrumentation);
		}
		else
		{
			input_instrumentation_end_time = time_stamp; // stop instrumentation as there're no more data
		}
	}
}

void Instrumenter::InstrumentOutputSignal(const std::string& signal_name)
{
	std::map<std::string, TyperBase *>::iterator typer_it = typers.find(signal_name);
	
	if(typer_it != typers.end())
	{
		TyperBase *typer = (*typer_it).second;
		
		if(typer->TryInstrumentOutputSignal(signal_name))
		{
			return;
		}
	}
	std::cerr << "WARNING! Can't instrument Signal \"" << signal_name << "\" for output" << std::endl;	
	
#if 0
	bool status_bool = TryInstrumentOutputSignal<bool>(signal_name);
	bool status_sc_uint_32 = TryInstrumentOutputSignal<sc_dt::sc_uint<32> >(signal_name);
	bool status_sc_uint_3 = TryInstrumentOutputSignal<sc_dt::sc_uint<3> >(signal_name);
	bool status_sc_uint_2 = TryInstrumentOutputSignal<sc_dt::sc_uint<2> >(signal_name);
	if(!status_bool && !status_sc_uint_32 && !status_sc_uint_3 && !status_sc_uint_2)
	{
		std::cerr << "WARNING! Can't instrument Signal \"" << signal_name << "\" for output" << std::endl;
	}
#endif
}

bool Instrumenter::InstrumentInputSignal(const std::string& signal_name)
{
	if(!IsInInputInstrumentSet(signal_name)) return false;

	std::map<std::string, TyperBase *>::iterator typer_it = typers.find(signal_name);
	
	if(typer_it != typers.end())
	{
		TyperBase *typer = (*typer_it).second;
		
		if(typer->TryInstrumentOutputSignal(signal_name))
		{
			return true;
		}
	}
	std::cerr << "WARNING! Can't instrument Signal \"" << signal_name << "\" for input" << std::endl;	
	return false;

#if 0
	bool status_bool = TryInstrumentInputSignal<bool>(signal_name);
	bool status_sc_uint_32 = TryInstrumentInputSignal<sc_dt::sc_uint<32> >(signal_name);
	bool status_sc_uint_3 = TryInstrumentInputSignal<sc_dt::sc_uint<3> >(signal_name);
	bool status_sc_uint_2 = TryInstrumentInputSignal<sc_dt::sc_uint<2> >(signal_name);
	if(!status_bool && !status_sc_uint_32 && !status_sc_uint_3 && !status_sc_uint_2)
	{
		std::cerr << "WARNING! Can't instrument Signal \"" << signal_name << "\" for input" << std::endl;
		return false;
	}

	return true;
#endif
}

InputInstrumentBase *Instrumenter::FindInputInstrument(const std::string& name)
{
	unsigned int n = input_instruments.size();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		InputInstrumentBase *instrument = input_instruments[i];
		
		if(instrument->GetName().compare(name) == 0) return instrument;
	}
	
	return 0;
}

Simulator::Simulator(sc_core::sc_module_name const& name, int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::service::Simulator *simulator))
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, unisim::kernel::service::Object(name)
	, instrumenter(0)
{
	instrumenter = new Instrumenter("instrumenter");
}

Simulator::~Simulator()
{
	delete instrumenter;
}

sc_core::sc_clock& Simulator::CreateClock(const std::string& clock_name, const sc_core::sc_time& clock_period, double clock_duty_cycle, const sc_core::sc_time& clock_start_time, bool clock_posedge_first)
{
	return instrumenter->CreateClock(clock_name, clock_period, clock_duty_cycle, clock_start_time, clock_posedge_first);
}

void Simulator::TraceSignalPattern(const std::string& signal_name_pattern)
{
	instrumenter->TraceSignalPattern(signal_name_pattern);
}

void Simulator::Bind(const std::string& port_name, const std::string& signal_name)
{
	instrumenter->Bind(port_name, signal_name);
}

void Simulator::BindArray(const std::string& port_array_name, const std::string& signal_array_name, unsigned int begin_idx, unsigned int end_idx)
{
	instrumenter->BindArray(port_array_name, signal_array_name, begin_idx, end_idx);
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim
