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

/////////////////////////////////// SampleBase ////////////////////////////////

SampleBase::~SampleBase()
{
}

///////////////////////////////// OutputBase //////////////////////////////////

OutputBase::OutputBase(const std::string& _name, const std::string& _type, unsigned int _size)
	: name(_name)
	, type(_type)
	, size(_size)
	, update_requested(false)
	, variables()
{
}

OutputBase::~OutputBase()
{
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
	WritersScanner writers_scanner;
	OutputProcessor::Instance().ScanWriters<WritersScanner, OutputBase, bool>(writers_scanner, *this);
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

/////////////////////////////////// Output<> //////////////////////////////////


///////////////////////////////////// Variable ////////////////////////////////

Variable::Variable(Writer& _writer, Module& _owner, const std::string& _name, const std::string& _identifier)
	: writer(_writer)
	, owner(_owner)
	, output(0)
	, name(_name)
	, identifier(_identifier)
{
	owner.Add(*this);
}

void Variable::Attach(OutputBase& _output)
{
	output = &_output;
	output->Add(*this);
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

void Writer::SetTimeScale(const std::string& timescale)
{
	OutputProcessor::Instance().SetTimeScale(timescale);
}

void Writer::SetLog(std::ostream& log)
{
	OutputProcessor::Instance().SetLog(log);
}

void Writer::SetWarningLog(std::ostream& warn_log)
{
	OutputProcessor::Instance().SetWarningLog(warn_log);
}

void Writer::SetErrorLog(std::ostream& err_log)
{
	OutputProcessor::Instance().SetErrorLog(err_log);
}

Writer::Writer(std::ostream& _stream, bool _interactive)
	: stream(&_stream)
	, io_buffer(0)
	, interactive(_interactive)
	, initialized(false)
	, next_variable_identifier()
	, top()
	, curr_time_stamp(0)
	, variables()
{
	OutputProcessor::Instance().Register(*this);
}

Writer::Writer(std::ofstream& _stream, std::streamsize io_buffer_size)
	: stream(&_stream)
	, io_buffer(new char[io_buffer_size])
	, interactive(false)
	, initialized(false)
	, next_variable_identifier()
	, top()
	, curr_time_stamp(0)
	, variables()
{
	_stream.rdbuf()->pubsetbuf(io_buffer, io_buffer_size);
	OutputProcessor::Instance().Register(*this);
}

Writer::~Writer()
{
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
	Variable *variable = new Variable(*this, owner, name, NextVariableIdentifier());
	
	OutputScanner output_scanner;
	OutputBase *output = OutputProcessor::Instance().ScanOutputs<OutputScanner, const std::string, OutputBase *>(output_scanner, name);
	
	if(output)
	{
		variable->Attach(*output);
	}
	
	variables.push_back(variable);
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
	std::size_t len = next_variable_identifier.length();
	if(len)
	{
		std::size_t back_pos = len - 1;
		char c = next_variable_identifier[back_pos];
		if(c < '~')
		{
			next_variable_identifier[back_pos]++;
		}
		else
		{
			next_variable_identifier.push_back('!');
		}
	}
	else
	{
		next_variable_identifier = '!';
	}
	
	return next_variable_identifier;
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
	, updatable_outputs()
	, samples()
	, curr_time_stamp(0)
	, next_time_stamp(0)
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
	outputs.push_back(&_output);
	return true;
}

void OutputProcessor::Unregister(OutputBase& _output)
{
	for(Outputs::iterator it = outputs.begin(); it != outputs.end(); ++it)
	{
		OutputBase *output = *it;
		if(output == &_output)
		{
			outputs.erase(it);
			return;
		}
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
