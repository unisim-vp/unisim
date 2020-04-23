/*
 *  Copyright (c) 2020,
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

#include <unisim/util/vcd/vcd.hh>

namespace unisim {
namespace util {
namespace vcd {

///////////////////////////////////////////////////////////////////////////////
//                                 Definitions                               //
///////////////////////////////////////////////////////////////////////////////

void SetOption(Option opt, bool flag)
{
	OutputProcessor::Instance().SetOption(opt, flag);
}

void GetOption(Option opt, bool& flag)
{
	OutputProcessor::Instance().GetOption(opt, flag);
}

void SetTimeScale(const std::string& timescale)
{
	OutputProcessor::Instance().SetTimeScale(timescale);
}

void SetLog(std::ostream& log)
{
	OutputProcessor::Instance().SetLog(log);
}

void SetWarningLog(std::ostream& warn_log)
{
	OutputProcessor::Instance().SetWarningLog(warn_log);
}

void SetErrorLog(std::ostream& err_log)
{
	OutputProcessor::Instance().SetErrorLog(err_log);
}

/////////////////////////////////// SampleBase ////////////////////////////////

SampleBase::~SampleBase()
{
}

///////////////////////////////// OutputBase //////////////////////////////////

OutputBase::OutputBase(const std::string& _name, const std::string& _type, unsigned int _size)
	: push_time_stamp(TimeStamp())
	, name(_name)
	, type(_type)
	, size(_size)
	, id(0)
	, registered(false)
	, pull_requested(false)
	, variables()
{
}

OutputBase::~OutputBase()
{
}

void OutputBase::Register()
{
	if((registered = OutputProcessor::Instance().Register(*this)))
	{
		AttachVariables();
	}
}

void OutputBase::Unregister()
{
	DetachVariables();
	OutputProcessor::Instance().Unregister(*this);
}

bool OutputBase::VarAttachScanner::VisitVariable(Variable& variable, OutputBase& output)
{
	if(variable.GetName() == output.GetName())
	{
		variable.Attach(output);
		return true;
	}
	return false;
}

bool OutputBase::WritersScanner::VisitWriter(Writer& writer, OutputBase& output)
{
	VarAttachScanner var_attach_scanner;
	writer.ScanVariables<VarAttachScanner, OutputBase, bool>(var_attach_scanner, output);
	return false;
}

void OutputBase::AttachVariables()
{
	if(registered)
	{
		WritersScanner writers_scanner;
		OutputProcessor::Instance().ScanWriters<WritersScanner, OutputBase, bool>(writers_scanner, *this);
	}
}

void OutputBase::DetachVariables()
{
	for(Variables::iterator variable_it = variables.begin(); variable_it != variables.end(); ++variable_it)
	{
		Variable *variable = *variable_it;
		variable->Detach();
	}
}

void OutputBase::Add(Variable& _variable)
{
	for(Variables::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable *variable = *it;
		if(variable == &_variable) return;
	}
	
	variables.push_back(&_variable);
}

void OutputBase::Remove(Variable& _variable)
{
	for(Variables::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable *variable = *it;
		if(variable == &_variable)
		{
			variables.erase(it);
			return;
		}
	}
}

void OutputBase::SetId(unsigned int _id)
{
	id = _id;
}

/////////////////////////////////// Output<> //////////////////////////////////


///////////////////////////////////// Variable ////////////////////////////////

Variable::Variable(Writer& _writer, Module& _owner, const std::string& _name, const std::string& _identifier, unsigned int _id)
	: writer(_writer)
	, owner(_owner)
	, output(0)
	, name(_name)
	, identifier(_identifier)
	, id(_id)
{
	owner.Add(*this);
}

void Variable::Attach(OutputBase& _output)
{
	output = &_output;
	output->Add(*this);
}

void Variable::Detach()
{
	output = 0;
}

Variable::~Variable()
{
	if(output) output->Remove(*this);
}

/////////////////////////////// Sample<> //////////////////////////////////////


////////////////////////////////// Module /////////////////////////////////////

Module::Module()
	: name()
	, children()
	, variables()
{
}

Module::Module(const std::string& _name, Module& parent)
	: name(_name)
	, children()
	, variables()
{
	parent.Add(*this);
}

Module::~Module()
{
	for(Variables::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable *variable = *it;
		delete variable;
	}
	for(Children::iterator it = children.begin(); it != children.end(); ++it)
	{
		Module *child = *it;
		delete child;
	}
}

void Module::Add(Module& child)
{
	children.push_back(&child);
}

void Module::Add(Variable& variable)
{
	variables.push_back(&variable);
}

Module *Module::FindChild(const std::string& child_name) const
{
	for(Children::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		Module *child = *it;
		if(child->GetName() == child_name) return child;
	}
	return 0;
}

/////////////////////////////////// Writer ////////////////////////////////////

Writer::Writer(std::ostream& _stream, bool _interactive)
	: stream(&_stream)
	, io_buffer(0)
	, interactive(_interactive)
	, initialized(false)
	, next_variable_identifier()
	, next_id(0)
	, sort_variables(OutputProcessor::Instance().sort_variables)
	, top()
	, curr_time_stamp(0)
	, variables()
	, stream_pool()
	, emit_order()
{
	OutputProcessor::Instance().Register(*this);
}

Writer::Writer(std::ofstream& _stream, std::streamsize io_buffer_size)
	: stream(&_stream)
	, io_buffer(new char[io_buffer_size])
	, interactive(false)
	, initialized(false)
	, next_variable_identifier()
	, next_id(0)
	, sort_variables(OutputProcessor::Instance().sort_variables)
	, top()
	, curr_time_stamp(0)
	, variables()
	, stream_pool()
	, emit_order()
{
	_stream.rdbuf()->pubsetbuf(io_buffer, io_buffer_size);
	OutputProcessor::Instance().Register(*this);
}

Writer::~Writer()
{
	for(StreamPool::iterator it = stream_pool.begin(); it != stream_pool.end(); ++it)
	{
		std::ostringstream *osstr = *it;
		delete osstr;
	}
	OutputProcessor::Instance().Unregister(*this);
	if(io_buffer) delete[] io_buffer;
}

OutputBase *Writer::OutputScanner::VisitOutput(OutputBase& output, const std::string& name)
{
	if(output.GetName() == name)
	{
		return &output;
	}
	return 0;
}

void Writer::Trace(const std::string& name)
{
	for(Variables::const_iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable *variable = *it;
		if(variable->GetName() == name) return;
	}
	
	Module& owner = GetVariableOwner(name);
	Variable *variable = new Variable(*this, owner, name, NextVariableIdentifier(), next_id++);
	
	OutputScanner output_scanner;
	OutputBase *output = OutputProcessor::Instance().ScanOutputs<OutputScanner, const std::string, OutputBase *>(output_scanner, name);
	
	if(output)
	{
		variable->Attach(*output);
	}
	
	variables.push_back(variable);
	stream_pool.push_back(new std::ostringstream());
}

bool Writer::Printer::VisitChild(Module& child, std::ostream& stream)
{
	stream << "$scope module " << child.GetName() << " $end" << '\n';
	child.ScanVariables<Printer, std::ostream, bool>(*this, stream);
	child.ScanChildren<Printer, std::ostream, bool>(*this, stream);
	stream << "$upscope $end" << '\n';
	return false;
}

bool Writer::Printer::VisitVariable(Variable& variable, std::ostream& stream)
{
	OutputBase *output = variable.GetOutput();
	if(output)
	{
		stream << "$var " << output->GetType() << " " << output->GetSize() << " " << variable.GetIdentifier() << " " << variable.GetName() << " $end" << '\n';
	}
	return false;
}

void Writer::Initialize()
{
	(*stream) << "$version Generated by UNISIM-VP $end" << '\n';
	(*stream) << "$timescale " << OutputProcessor::Instance().timescale << " $end" << '\n';
	
	(*stream) << "$scope module TOP $end" << '\n';
	Printer printer;
	top.ScanVariables<Printer, std::ostream, bool>(printer, *stream);
	top.ScanChildren<Printer, std::ostream, bool>(printer, *stream);
	(*stream) << "$upscope $end" << '\n';
	
	(*stream) << "$enddefinitions $end" << std::endl;

	Variables::iterator it = variables.begin();
	while(it != variables.end())
	{
		Variable *variable = *it;
		if(!variable->GetOutput())
		{
			OutputProcessor::Instance().WarnLog() << "Variable \"" << variable->GetName() << "\" is not attached to an output" << std::endl;
			it = variables.erase(it);
		}
		else
		{
			++it;
		}
	}
	
	initialized = true;
}

const std::string& Writer::NextVariableIdentifier()
{
	const char first = '!';
	const char last = '~';
	std::size_t len = next_variable_identifier.length();
	if(len)
	{
		std::size_t pos = len - 1;
		do
		{
			char c = next_variable_identifier[pos];
			bool extend = (c == last);
			next_variable_identifier[pos] = extend ? first : (c + 1);
			if(!extend) return next_variable_identifier;
			if(!pos)
			{
				next_variable_identifier.insert(0, 1, first);
				return next_variable_identifier;
			}
			--pos;
		}
		while(1);
	}

	return next_variable_identifier = first;
}

Module& Writer::GetModule(Module& module, const std::string& leaf_hierarchical_name)
{
	std::size_t hierarchical_delimiter_pos = leaf_hierarchical_name.find_first_of('.');
	
	std::string child_name = leaf_hierarchical_name.substr(0, hierarchical_delimiter_pos);
	
	Module *found_child = module.FindChild(child_name);
	
	if(!found_child)
	{
		// not found
		found_child = new Module(child_name, module);
	}
	
	return (hierarchical_delimiter_pos == std::string::npos) ? (*found_child) : GetModule(*found_child, leaf_hierarchical_name.substr(hierarchical_delimiter_pos + 1));
}

Module& Writer::GetVariableOwner(const std::string& variable_name)
{
	std::size_t hierarchical_delimiter_pos = variable_name.find_last_of('.');
	
	if(hierarchical_delimiter_pos == std::string::npos)
	{
		return top;
	}
	
	return GetModule(top, variable_name.substr(0, hierarchical_delimiter_pos));
}

//////////////////////////////// OutputProcessor //////////////////////////////

OutputProcessor::OutputProcessor()
	: toggle(0)
	, writers()
	, outputs()
	, pullable_outputs()
	, rounds()
	, free_list()
	, samples()
	, curr_time_stamp(0)
	, sort_variables(false)
	, debug(false)
	, timescale("1 ps")
	, timescale_fixed(false)
	, log(&std::cout)
	, warn_log(&std::cerr)
	, err_log(&std::cerr)
{
}

OutputProcessor::~OutputProcessor()
{
	for(Samples::iterator it = samples.begin(); it != samples.end(); ++it)
	{
		SampleBase *sample = (*it).second;
		sample->Free();
	}
	for(typename FreeList::iterator it = free_list.begin(); it != free_list.end(); ++it)
	{
		Round *round = *it;
		delete round;
	}
}

void OutputProcessor::Register(Writer& _writer)
{
	for(Writers::iterator it = writers.begin(); it != writers.end(); ++it)
	{
		Writer *writer = *it;
		if(writer == &_writer) return;
	}
	writers.push_back(&_writer);
}

void OutputProcessor::Unregister(Writer& _writer)
{
	for(Writers::iterator it = writers.begin(); it != writers.end(); ++it)
	{
		Writer *writer = *it;
		if(writer == &_writer)
		{
			writers.erase(it);
			return;
		}
	}
}

bool OutputProcessor::Register(OutputBase& _output)
{
	for(Outputs::iterator it = outputs.begin(); it != outputs.end(); ++it)
	{
		OutputBase *output = *it;
		if(output->GetName() == _output.GetName())
		{
			ErrLog() << "Multiple definition of Output \"" << output->GetName() << "\"" << std::endl;
			return false;
		}
	}
	unsigned int output_id = outputs.size();
	_output.SetId(output_id);
	outputs.push_back(&_output);
	
	for(Rounds::iterator round_it = rounds.begin(); round_it != rounds.end(); ++round_it)
	{
		Round *round = (*round_it).second;
		round->agreements.resize(outputs.size());
	}
	return true;
}

void OutputProcessor::Unregister(OutputBase& _output)
{
	for(Outputs::iterator it = outputs.begin(); it != outputs.end(); ++it)
	{
		OutputBase *output = *it;
		if(output == &_output)
		{
			Outputs& curr_pullable_outputs = pullable_outputs[toggle];
			for(Outputs::iterator output_it = curr_pullable_outputs.begin(); output_it != curr_pullable_outputs.end(); ++output_it)
			{
				OutputBase *committable_output = *output_it;
				if(committable_output == &_output)
				{
					curr_pullable_outputs.erase(output_it);
					break;
				}
			}
			
			for(Rounds::iterator round_it = rounds.begin(); round_it != rounds.end(); ++round_it)
			{
				Round *round = (*round_it).second;
				if(round->agreements[_output.GetId()]) round->agreement_count--;
				round->agreements.erase(round->agreements.begin() + _output.GetId());
			}
			
			outputs.erase(it);
			
			for(Rounds::iterator round_it = rounds.begin(); round_it != rounds.end(); ++round_it)
			{
				Round *round = (*round_it).second;
				if(round->agreement_count >= outputs.size())
				{
					uint64_t agreed_time_stamp = (*round_it).first;
					CommitUntil(agreed_time_stamp);
				}
			}
			
			unsigned int output_id = 0;
			for(Outputs::iterator it = outputs.begin(); it != outputs.end(); ++it, ++output_id)
			{
				OutputBase *output = *it;
				output->SetId(output_id);
			}
			return;
		}
	}
}

void OutputProcessor::DumpRounds()
{
	Log() << "[";
	for(Rounds::const_iterator round_it = rounds.begin(); round_it != rounds.end(); ++round_it)
	{
		if(round_it != rounds.begin()) Log() << ", ";
		uint64_t time_stamp = (*round_it).first;
		Round *round = (*round_it).second;
		Log() << time_stamp << " => (" << round->agreement_count << "/" << outputs.size() << ":";
		for(Agreements::const_iterator agreement_it = round->agreements.begin(); agreement_it != round->agreements.end(); ++agreement_it)
		{
			bool agreement = *agreement_it;
			Log() << (agreement ? 'x' : '.');
		}
		Log() << ")";
	}
	Log() << "]";
}

void OutputProcessor::SetOption(Option opt, bool flag)
{
	switch(opt)
	{
		case OPT_SORT_VARIABLES: sort_variables = flag; break;
		case OPT_DEBUG: debug = flag; break;
		default: break;
	}
}

void OutputProcessor::GetOption(Option opt, bool& flag)
{
	switch(opt)
	{
		case OPT_SORT_VARIABLES: flag = sort_variables; break;
		case OPT_DEBUG: flag = debug; break;
		default: break;
	}
}

void OutputProcessor::SetTimeScale(const std::string& _timescale)
{
	if(timescale_fixed)
	{
		WarnLog() << "Timescale is already fixed" << std::endl;
		return;
	}
	
	timescale = _timescale;
}

void OutputProcessor::SetLog(std::ostream& _log)
{
	log = &_log;
}

void OutputProcessor::SetWarningLog(std::ostream& _warn_log)
{
	warn_log = &_warn_log;
}

void OutputProcessor::SetErrorLog(std::ostream& _err_log)
{
	err_log = &_err_log;
}

std::ostream& OutputProcessor::Log() const
{
	return *log;
}

std::ostream& OutputProcessor::WarnLog() const
{
	return *warn_log;
}

std::ostream& OutputProcessor::ErrLog() const
{
	return *err_log;
}

} // end of namespace vcd
} // end of namespace util
} // end of namespace unisim
