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

#ifndef __UNISIM_KERNEL_TLM2_SIMULATOR_HH__
#define __UNISIM_KERNEL_TLM2_SIMULATOR_HH__

#include <unisim/kernel/service/service.hh>
#include <systemc>
#include <stdexcept>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#define DEBUG_INSTRUMENTER 1

namespace unisim {
namespace kernel {
namespace tlm2 {

enum INSTRUMENTATION_TYPE
{
	INPUT_INSTRUMENTATION        = 1,
	OUTPUT_INSTRUMENTATION       = 2
};

class InstrumentBase
{
public:
	inline InstrumentBase(const std::string& name, INSTRUMENTATION_TYPE instrumentation_type);
	inline virtual ~InstrumentBase();
	
	inline virtual const std::string& GetName() const;
	inline INSTRUMENTATION_TYPE GetInstrumentationType() const;
private:
	std::string name;
	INSTRUMENTATION_TYPE instrumentation_type;
};

class InputInstrumentBase : public InstrumentBase
{
public:
	inline InputInstrumentBase(const std::string& name);
	virtual void Inject() = 0;
	virtual void Input(std::istream& is) = 0;
};

class OutputInstrumentBase : public InstrumentBase
{
public:
	inline OutputInstrumentBase(const std::string& name);
	virtual void Sample() = 0;
	virtual void Output(std::ostream& os, bool force_output = false) = 0;
};

template <typename T>
class InputInstrument : public InputInstrumentBase
{
public:
	InputInstrument(const std::string& name, sc_core::sc_signal<T> *signal);
	
	virtual void Inject();
	virtual void Input(std::istream& is);
private:
	sc_core::sc_signal<T> *signal;
	T value;
	bool value_changed;
};

template <typename T>
class OutputInstrument : public OutputInstrumentBase
{
public:
	OutputInstrument(const std::string& name, const sc_core::sc_signal<T> *signal);
	
	virtual void Sample();
	virtual void Output(std::ostream& os, bool force_output);
private:
	const sc_core::sc_signal<T> *signal;
	T latched_value;
	T sample_value;
};

class TyperBase
{
public:
	virtual ~TyperBase() {}
	virtual bool TryTraceSignal(const std::string& signal_name) = 0;
	virtual bool TryInstrumentInputSignal(const std::string& signal_name) = 0;
	virtual bool TryInstrumentOutputSignal(const std::string& signal_name) = 0;
	virtual bool TryBind(const std::string& port_name, const std::string& signal_name) = 0;
};

class Instrumenter;

template <typename T>
class Typer : public TyperBase
{
public:
	Typer(Instrumenter *instrumenter);
	virtual bool TryTraceSignal(const std::string& signal_name);
	virtual bool TryInstrumentInputSignal(const std::string& signal_name);
	virtual bool TryInstrumentOutputSignal(const std::string& signal_name);
	virtual bool TryBind(const std::string& port_name, const std::string& signal_name);
private:
	Instrumenter *instrumenter;
};

class Instrumenter
	: public virtual unisim::kernel::service::Object
{
public:
	Instrumenter(const char *name, unisim::kernel::service::Object *parent = 0);
	virtual ~Instrumenter();
	virtual bool EndSetup();
	
	sc_core::sc_clock& CreateClock(const std::string& clock_name, const sc_core::sc_time& clock_period, double clock_duty_cycle, const sc_core::sc_time& clock_start_time, bool clock_posedge_first);
	template <typename T> sc_core::sc_signal<T>& CreateSignal(const T& init_value);
	template <typename T> sc_core::sc_signal<T>& CreateSignal(const std::string& signal_name, const T& init_value);
	template <typename T> void CreateSignalArray(const std::string& signal_array_name, unsigned int signal_array_dim, const T& init_value);

	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> void RegisterSignal(sc_core::sc_signal<T, WRITER_POLICY> *signal);
	void RegisterPort(sc_core::sc_port_base& port);
	template <typename T> void RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& in_port);
	template <typename T> void RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& out_port);

	void TraceSignalPattern(const std::string& signal_name_pattern);
	void Bind(const std::string& port_name, const std::string& signal_name);
	void BindArray(const std::string& port_array_name, const std::string& signal_array_name, unsigned int begin_idx, unsigned int end_idx);
	
	void StartBinding();
	void StartInstrumentation();
	
	bool enable_output_instrumentation;
	unisim::kernel::service::Parameter<bool> param_enable_output_instrumentation;
	std::string instrumentation_output_filename;
	unisim::kernel::service::Parameter<std::string> param_instrumentation_output_filename;
	std::string output_instrumentation;
	unisim::kernel::service::Parameter<std::string> param_output_instrumentation;
	sc_core::sc_time output_instrumentation_start_time;
	unisim::kernel::service::Parameter<sc_core::sc_time> param_output_instrumentation_start_time;
	sc_core::sc_time output_instrumentation_end_time;
	unisim::kernel::service::Parameter<sc_core::sc_time> param_output_instrumentation_end_time;
	std::string output_csv_delimiter;
	unisim::kernel::service::Parameter<std::string> param_output_csv_delimiter;
	
	bool enable_input_instrumentation;
	unisim::kernel::service::Parameter<bool> param_enable_input_instrumentation;
	std::string instrumentation_input_filename;
	unisim::kernel::service::Parameter<std::string> param_instrumentation_input_filename;
	std::string input_instrumentation;
	unisim::kernel::service::Parameter<std::string> param_input_instrumentation;
	sc_core::sc_time input_instrumentation_start_time;
	unisim::kernel::service::Parameter<sc_core::sc_time> param_input_instrumentation_start_time;
	sc_core::sc_time input_instrumentation_end_time;
	unisim::kernel::service::Parameter<sc_core::sc_time> param_input_instrumentation_end_time;
	std::string input_csv_delimiter;
	unisim::kernel::service::Parameter<std::string> param_input_csv_delimiter;

	std::string vcd_trace_filename;
	unisim::kernel::service::Parameter<std::string> param_vcd_trace_filename;
	std::string trace_signals;
	unisim::kernel::service::Parameter<std::string> param_trace_signals;
	bool enable_trace_signals;
	unisim::kernel::service::Parameter<bool> param_enable_trace_signals;
	sc_core::sc_trace_file *trace_file;

	std::string gtkwave_init_script;
	unisim::kernel::service::Parameter<std::string> param_gtkwave_init_script;

	std::map<std::string, sc_core::sc_interface *> signal_pool;
	std::map<std::string, sc_core::sc_port_base *> port_pool;
	std::map<std::string, std::string> netlist;
	std::set<std::string> input_instrumentation_set;
	std::map<std::string, TyperBase *> typers;

	std::ofstream *instrumentation_output_file;
	std::ifstream *instrumentation_input_file;
	std::vector<OutputInstrumentBase *> output_instruments;
	std::vector<InputInstrumentBase *> input_instruments;
	sc_core::sc_spawn_options output_instrumentation_process_spawn_options;
	sc_core::sc_time last_instrument_sampling_time_stamp;
	sc_core::sc_time input_time_resolution;
	std::vector<InputInstrumentBase *> input_csv_instrument_map;
	sc_core::sc_time input_time_stamp;
	sc_core::sc_event next_input_instrumentation_event;

	template <typename T> sc_core::sc_signal<T>& GetSignal(const std::string& signal_name);
	template <typename T> sc_core::sc_signal<T>& GetSignal(const std::string& signal_array_name, unsigned int idx);
	template <typename T> sc_core::sc_signal<T> *TryGetSignal(const std::string& signal_name);
	template <typename T> sc_core::sc_signal<T> *TryGetSignal(const std::string& signal_array_name, unsigned int idx);
	template <typename T> void MatchSignal(const std::string& signal_name_pattern, std::vector<sc_core::sc_signal<T> *>& vec_signals);
	template <typename T> bool SignalIsA(const std::string& signal_name, const T& sample);
	template <typename T> bool TryTraceSignal(const std::string& signal_name);
	bool IsInInputInstrumentSet(const std::string& name) const;
	void GenerateGTKWaveInitScript(const std::string& filename, int zoom_factor) const;
	bool Match(const std::string& p, const std::string& s) const;
 	template <typename T> bool TryBind(const std::string& port_name, const std::string& signal_name);
	void SampleInstruments(const sc_core::sc_time& time_stamp);
	void OutputInstrumentsNamesAsCSV(std::ostream& os);
	void OutputInstrumentsAsCSV(std::ostream& os, const sc_core::sc_time& time_stamp);
	void OutputInstrumentationProcess();
	void InputInstrumentationProcess();
	template <typename T> void SignalTeeProcess(sc_core::sc_signal<T> *original_signal, sc_core::sc_signal<T> *injected_signal, sc_core::sc_signal<T> *signal);
	template <typename T> bool TryInstrumentInputSignal(const std::string& signal_name);
	template <typename T> bool TryInstrumentOutputSignal(const std::string& signal_name);
	void InstrumentOutputSignal(const std::string& signal_name);
	bool InstrumentInputSignal(const std::string& signal_name);
	
	bool ParseCSVHeaderAndInstrumentInput();
	bool ParseCSV();
	InputInstrumentBase *FindInputInstrument(const std::string& name);
};

class Simulator
	: public unisim::kernel::service::Simulator
	, public unisim::kernel::service::Object
	, public sc_core::sc_module
{
public:
	Simulator(sc_core::sc_module_name const& name, int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::service::Simulator *simulator) = 0);
	virtual ~Simulator();
	
	sc_core::sc_clock& CreateClock(const std::string& clock_name, const sc_core::sc_time& clock_period, double clock_duty_cycle, const sc_core::sc_time& clock_start_time, bool clock_posedge_first);
	template <typename T> sc_core::sc_signal<T>& CreateSignal(const T& init_value);
	template <typename T> sc_core::sc_signal<T>& CreateSignal(const std::string& signal_name, const T& init_value);
	template <typename T> void CreateSignalArray(const std::string& signal_array_name, unsigned int signal_array_dim, const T& init_value);
	
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> void RegisterSignal(sc_core::sc_signal<T, WRITER_POLICY> *signal);
	template <typename T> void RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& in_port);
	template <typename T> void RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& out_port);

	void TraceSignalPattern(const std::string& signal_name_pattern);
	void Bind(const std::string& port_name, const std::string& signal_name);
	void BindArray(const std::string& port_array_name, const std::string& signal_array_name, unsigned int begin_idx, unsigned int end_idx);

private:
	Instrumenter *instrumenter;
};

template <typename T> sc_core::sc_signal<T>& Simulator::CreateSignal(const T& init_value)
{
	return instrumenter->CreateSignal(init_value);
}

template <typename T> sc_core::sc_signal<T>& Simulator::CreateSignal(const std::string& signal_name, const T& init_value)
{
	return instrumenter->CreateSignal(signal_name, init_value);
}

template <typename T> void Simulator::CreateSignalArray(const std::string& signal_array_name, unsigned int signal_array_dim, const T& init_value)
{
	instrumenter->CreateSignalArray(signal_array_name, signal_array_dim, init_value);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY> void Simulator::RegisterSignal(sc_core::sc_signal<T, WRITER_POLICY> *signal)
{
	instrumenter->RegisterSignal(signal);
}

template <typename T> void Simulator::RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& in_port)
{
	instrumenter->RegisterPort(in_port);
}

template <typename T> void Simulator::RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& out_port)
{
	instrumenter->RegisterPort(out_port);
}

inline InstrumentBase::InstrumentBase(const std::string& _name, INSTRUMENTATION_TYPE _instrumentation_type)
	: name(_name)
	, instrumentation_type(_instrumentation_type)
{
}

inline InstrumentBase::~InstrumentBase()
{
}

inline const std::string& InstrumentBase::GetName() const
{
	return name;
}

inline INSTRUMENTATION_TYPE InstrumentBase::GetInstrumentationType() const
{
	return instrumentation_type;
}

inline InputInstrumentBase::InputInstrumentBase(const std::string& name)
	: InstrumentBase(name, INPUT_INSTRUMENTATION)
{
}

inline OutputInstrumentBase::OutputInstrumentBase(const std::string& name)
	: InstrumentBase(name, OUTPUT_INSTRUMENTATION)
{
}

template <typename T>
InputInstrument<T>::InputInstrument(const std::string& name, sc_core::sc_signal<T> *_signal)
	: InputInstrumentBase(name)
	, signal(_signal)
	, value(signal->read())
	, value_changed(true)
{
}

template <typename T>
void InputInstrument<T>::Inject()
{
	if(value_changed)
	{
#if DEBUG_INSTRUMENTER
		std::cout << "InputInstrument<T>::Inject(): At " << sc_core::sc_time_stamp() << ", " << signal->name() << " <- '" << value << "'" << std::endl;
#endif
		signal->write(value);
		value_changed = false;
	}
}

template <typename T>
void InputInstrument<T>::Input(std::istream& is)
{
	T tmp_value;
	
	if(is >> tmp_value)
	{
		if(!is.eof())
		{
			std::cerr << "WARNING! ignoring extra characters after \"" << tmp_value << "\"" << std::endl;
		}
		if(!(value == tmp_value)) value_changed = true;
		value = tmp_value;
	}
}

template <typename T>
OutputInstrument<T>::OutputInstrument(const std::string& name, const sc_core::sc_signal<T> *_signal)
	: OutputInstrumentBase(name)
	, signal(_signal)
	, latched_value(signal->read())
	, sample_value(latched_value)
{
}

template <typename T>
void OutputInstrument<T>::Sample()
{
	sample_value = signal->read();
}

template <typename T>
void OutputInstrument<T>::Output(std::ostream& os, bool force_output)
{
	if(force_output || (sample_value != latched_value))
	{
		latched_value = sample_value;
		os << latched_value;
	}
}

template <typename T>
Typer<T>::Typer(Instrumenter *_instrumenter)
	: instrumenter(_instrumenter)
{
}

template <typename T>
bool Typer<T>::TryTraceSignal(const std::string& signal_name)
{
	return instrumenter->TryTraceSignal<T>(signal_name);
}

template <typename T>
bool Typer<T>::TryInstrumentInputSignal(const std::string& signal_name)
{
	return instrumenter->TryInstrumentInputSignal<T>(signal_name);
}

template <typename T>
bool Typer<T>::TryInstrumentOutputSignal(const std::string& signal_name)
{
	return instrumenter->TryInstrumentOutputSignal<T>(signal_name);
}

template <typename T>
bool Typer<T>::TryBind(const std::string& port_name, const std::string& signal_name)
{
	return instrumenter->TryBind<T>(port_name, signal_name);
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::CreateSignal(const T& init_value)
{
	sc_core::sc_signal<T> *signal = new sc_core::sc_signal<T>();
#ifdef DEBUG_INSTRUMENTER
	std::cout << "Creating Signal \"" << signal->name() << "\" <- '" << init_value << "'" << std::endl;
#endif
	signal_pool[signal->name()] = signal;
	*signal = init_value;
	
	typers[signal->name()] = new Typer<T>(this);
	
	return *signal;
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::CreateSignal(const std::string& signal_name, const T& init_value)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		std::cerr << "ERROR! Signal \"" << signal_name << "\" already exists" << std::endl;
		throw std::runtime_error("Internal error!");
	}
	
	sc_core::sc_signal<T> *signal = new sc_core::sc_signal<T>(signal_name.c_str());
#ifdef DEBUG_INSTRUMENTER
	std::cout << "Creating Signal \"" << signal->name() << "\" <- '" << init_value << "'" << std::endl;
#endif
	signal_pool[signal->name()] = signal;
	*signal = init_value;
	
	typers[signal->name()] = new Typer<T>(this);

	if(enable_input_instrumentation)
	{
		std::string full_signal_name(signal->name());
		
		if(IsInInputInstrumentSet(full_signal_name)/* input_instrumentation_set.find(full_signal_name) != input_instrumentation_set.end()*/)
		{
			// signal is instrumented for input
			std::stringstream original_signal_name_sstr;
			original_signal_name_sstr << "original_" << signal_name;
			sc_core::sc_signal<T> *original_signal = new sc_core::sc_signal<T>((signal_name + "_original").c_str());
			signal_pool[original_signal->name()] = original_signal;
			*original_signal = init_value;
			
			std::stringstream injected_signal_name_sstr;
			injected_signal_name_sstr << "injected_" << signal_name;
			sc_core::sc_signal<T> *injected_signal = new sc_core::sc_signal<T>((signal_name + "_injected").c_str());
			signal_pool[injected_signal->name()] = injected_signal;
			*injected_signal = init_value;
		}
	}
	
	return *signal;
}

template <typename T>
void Instrumenter::CreateSignalArray(const std::string& signal_array_name, unsigned int signal_array_dim, const T& init_value)
{
	unsigned int signal_array_index;
	
	for(signal_array_index = 0; signal_array_index < signal_array_dim; signal_array_index++)
	{
		std::stringstream sstr;
		sstr << signal_array_name << "_" << signal_array_index;
		std::string signal_name(sstr.str());
		CreateSignal(signal_name, init_value);
	}
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::GetSignal(const std::string& signal_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T> *signal = dynamic_cast<sc_core::sc_signal<T> *>(signal_if);
			
			if(signal) return *signal;
			
			std::cerr << "ERROR! Signal \"" << signal_name << "\" has an unexpected type" << std::endl;
			throw std::runtime_error("Internal error!");
		}
	}
	
	std::cerr << "ERROR! Signal \"" << signal_name << "\" not found" << std::endl;
	throw std::runtime_error("Internal error!");
	static sc_core::sc_signal<T> dummy_signal;
	return dummy_signal;
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::GetSignal(const std::string& signal_array_name, unsigned int signal_array_index)
{
	std::stringstream sstr;
	sstr << signal_array_name << "_" << signal_array_index;
	std::string signal_name(sstr.str());

	return GetSignal<T>(signal_name);
}

template <typename T>
sc_core::sc_signal<T> *Instrumenter::TryGetSignal(const std::string& signal_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T> *signal = dynamic_cast<sc_core::sc_signal<T> *>(signal_if);
			
			return signal;
		}
	}
	
	return 0;
}

template <typename T>
sc_core::sc_signal<T> *Instrumenter::TryGetSignal(const std::string& signal_array_name, unsigned int signal_array_index)
{
	std::stringstream sstr;
	sstr << signal_array_name << "_" << signal_array_index;
	std::string signal_name(sstr.str());

	return TryGetSignal<T>(signal_name);
}

template <typename T>
void Instrumenter::MatchSignal(const std::string& signal_name_pattern, std::vector<sc_core::sc_signal<T> *>& vec_signals)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		sc_core::sc_signal<T> *candidate_signal = dynamic_cast<sc_core::sc_signal<T> *>(signal_if);
		
		if(candidate_signal)
		{
			std::string s(candidate_signal->name());
			const std::string& p = signal_name_pattern;
			
			if(Match(p, s))
			{
				// match
				vec_signals.push_back(candidate_signal);
			}
		}
    }
}

template <typename T>
bool Instrumenter::SignalIsA(const std::string& signal_name, const T& sample)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T> *signal = dynamic_cast<sc_core::sc_signal<T> *>(signal_if);
			
			if(signal) return true;
		}
	}
	
	return false;
}

template <typename T>
bool Instrumenter::TryTraceSignal(const std::string& signal_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T> *signal = dynamic_cast<sc_core::sc_signal<T> *>(signal_if);
			
			if(signal)
			{
#ifdef DEBUG_INSTRUMENTER
				std::cout << "Tracing Signal \"" << signal->name() << "\"" << std::endl;
#endif
				sc_trace(trace_file, *signal, signal->name());
				return true;
			}
		}
	}
	
	return false;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void Instrumenter::RegisterSignal(sc_core::sc_signal<T, WRITER_POLICY> *signal)
{
#ifdef DEBUG_INSTRUMENTER
	std::cout << "Registering Signal \"" << signal->name() << "\"" << std::endl;
#endif
	signal_pool[signal->name()] = signal;
	typers[signal->name()] = new Typer<T>(this);
}

template <typename T>
void Instrumenter::RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& in_port)
{
	RegisterPort(*(sc_core::sc_port_base *) &in_port);
	typers[in_port.name()] = new Typer<T>(this);
}

template <typename T>
void Instrumenter::RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& out_port)
{
	RegisterPort(*(sc_core::sc_port_base *) &out_port);
	typers[out_port.name()] = new Typer<T>(this);
}

template <typename T>
bool Instrumenter::TryBind(const std::string& port_name, const std::string& signal_name)
{
	sc_core::sc_port_b<sc_core::sc_signal_in_if<T> > *in_port = 0;
	sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> > *out_port = 0;
	
	std::map<std::string, sc_core::sc_port_base *>::iterator port_pool_it = port_pool.find(port_name);
	
	if(port_pool_it != port_pool.end())
	{
		sc_core::sc_port_base *port_base = (*port_pool_it).second;
		
		in_port = dynamic_cast<sc_core::sc_port_b<sc_core::sc_signal_in_if<T> > *>(port_base);
		
		if(!in_port)
		{
			out_port = dynamic_cast<sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> > *>(port_base);
		}
	}
	
	if(in_port || out_port)
	{
		std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
			
		if(signal_pool_it != signal_pool.end())
		{
			sc_core::sc_interface *signal_if = (*signal_pool_it).second;
			
			if(in_port)
			{
				sc_core::sc_signal_in_if<T> *signal_in_if = dynamic_cast<sc_core::sc_signal_in_if<T> *>(signal_if);
				
				if(signal_in_if)
				{
					(*in_port)(*signal_in_if);
					return true;
				}
			}
			else if(out_port)
			{
				sc_core::sc_signal_inout_if<T> *signal_inout_if = dynamic_cast<sc_core::sc_signal_inout_if<T> *>(signal_if);

				if(signal_inout_if)
				{
					if(param_enable_input_instrumentation && FindInputInstrument(signal_name))
					{
						// signal is instrumented for input
						signal_pool_it = signal_pool.find(signal_name + "_original");
						if(signal_pool_it != signal_pool.end())
						{
							sc_core::sc_interface *original_signal_if = (*signal_pool_it).second;
							sc_core::sc_signal_inout_if<T> *original_signal_inout_if = dynamic_cast<sc_core::sc_signal_inout_if<T> *>(original_signal_if);
							if(!original_signal_inout_if) throw std::runtime_error("Internal error! can't retrieve interface of original signal");
							(*out_port)(*original_signal_inout_if);
							return true;
						}
						throw std::runtime_error("Internal error! can't find original signal");
					}
					(*out_port)(*signal_inout_if);
					return true;
				}
			}
		}
	}
	return false;
}

template <typename T>
bool Instrumenter::TryInstrumentInputSignal(const std::string& signal_name)
{
	sc_core::sc_signal<T> *signal = TryGetSignal<T>(signal_name);
	
	if(signal)
	{
		sc_core::sc_signal<T> *original_signal = TryGetSignal<T>((signal_name + "_original").c_str());
		sc_core::sc_signal<T> *injected_signal = TryGetSignal<T>((signal_name + "_injected").c_str());
		if(!original_signal || !injected_signal) throw std::runtime_error("Internal error! can't get either original or injected signal");
		
#ifdef DEBUG_INSTRUMENTER
		std::cout << "Instrumenting Signal \"" << signal->name() << "\" for input" << std::endl;
#endif
		InputInstrument<T> *instrument = new InputInstrument<T>(signal_name, injected_signal);
		input_instruments.push_back(instrument);
		
		sc_core::sc_spawn_options signal_tee_process_spawn_options;
		signal_tee_process_spawn_options.spawn_method();
		signal_tee_process_spawn_options.set_sensitivity(original_signal);
		signal_tee_process_spawn_options.set_sensitivity(injected_signal);
		
		sc_core::sc_spawn(sc_bind(&Instrumenter::SignalTeeProcess<T>, this, original_signal, injected_signal, signal), sc_core::sc_gen_unique_name("signal_tee_process"), &signal_tee_process_spawn_options);
		return true;
	}
	
	return false;
}

template <typename T>
bool Instrumenter::TryInstrumentOutputSignal(const std::string& signal_name)
{
	bool status = false;
	std::vector<sc_core::sc_signal<T> *> sv;
	MatchSignal<T>(signal_name, sv);
	typename std::vector<sc_core::sc_signal<T> *>::const_iterator sv_it;
	for(sv_it = sv.begin(); sv_it != sv.end(); sv_it++)
	{
		
		sc_core::sc_signal<T> *matched_signal = *sv_it;
#ifdef DEBUG_INSTRUMENTER
		std::cout << "Instrumenting Signal \"" << matched_signal->name() << "\" for output" << std::endl;
#endif
		std::string matched_signal_name(matched_signal->name());
		OutputInstrument<T> *instrument = new OutputInstrument<T>(matched_signal_name, matched_signal);
		output_instruments.push_back(instrument);
		output_instrumentation_process_spawn_options.set_sensitivity(matched_signal);
		status = true;
	}
	
	return status;
}

template <typename T>
void Instrumenter::SignalTeeProcess(sc_core::sc_signal<T> *original_signal, sc_core::sc_signal<T> *injected_signal, sc_core::sc_signal<T> *signal)
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	T value;
	
	if((time_stamp >= input_instrumentation_start_time) && (time_stamp <= input_instrumentation_end_time))
	{
#if DEBUG_INSTRUMENTER
		std::cout << time_stamp << ": Inject: " << signal->name() << " <- '" << (*injected_signal) << "'" << std::endl;
#endif
		value = *injected_signal;
	}
	else
	{
#if DEBUG_INSTRUMENTER
		std::cout << time_stamp << ": Passthrough: " << signal->name() << " <- '" << (*original_signal) << "'" << std::endl;
#endif
		value = *original_signal;
	}
	
	*signal = value;
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_SIMULATOR_HH__