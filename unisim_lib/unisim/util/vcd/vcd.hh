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

#ifndef __UNISIM_UTIL_VCD_VCD_HH__
#define __UNISIM_UTIL_VCD_VCD_HH__

#include <unisim/util/likely/likely.hh>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <climits>
#include <cassert>
#include <stdint.h>

namespace unisim {
namespace util {
namespace vcd {

///////////////////////////////////////////////////////////////////////////////
//                           Forward declarations                            //
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct TypeTrait;
class SampleBase;
class OutputBase;
template <typename T> class Output;
class Variable;
template <typename T> class Sample;
class Writer;
class Module;
class OutputProcessor;

///////////////////////////////////////////////////////////////////////////////
//                                Declarations                               //
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////// TypeTrait<> ////////////////////////////////

template <typename T>
struct TypeTrait
{
	static const char *GetDefaultType() { return "wire"; }
	static unsigned int GetSize() { return sizeof(T) * CHAR_BIT; }
	static void Print(std::ostream& stream, const T& value, const std::string& identifier)
	{
		if(GetSize() > 1)
		{
			stream << 'b';
			for(int i = GetSize() - 1; i >= 0; --i) stream << ((value >> i) & 1);
		}
		else
		{
			stream << (value ? '1' :'0');
		}
		stream << ' ' << identifier;
	}
};

template <>
struct TypeTrait<bool>
{
	static const char *GetDefaultType() { return "wire"; }
	static unsigned int GetSize() { return 1; }
	static void Print(std::ostream& stream, const bool& value, const std::string& identifier)
	{
		stream << (value ? '1' : '0') << identifier;
	}
};

template <>
struct TypeTrait<float>
{
	static const char *GetDefaultType() { return "real"; }
	static unsigned int GetSize() { return 1; }
	static void Print(std::ostream& stream, const float& value, const std::string& identifier)
	{
		stream << 'r' << std::setprecision(8) << value << ' ' << identifier;
	}
};

template <>
struct TypeTrait<double>
{
	static const char *GetDefaultType() { return "real"; }
	static unsigned int GetSize() { return 1; }
	static void Print(std::ostream& stream, const double& value, const std::string& identifier)
	{
		stream << 'r' << std::setprecision(16) << value << ' ' << identifier;
	}
};

/////////////////////////////////// SampleBase ////////////////////////////////

class SampleBase
{
public:
	inline SampleBase();
	virtual ~SampleBase();
	inline void SetTimeStamp(uint64_t time_stamp);
	inline uint64_t GetTimeStamp() const;
	virtual void Free() = 0;
	virtual const OutputBase& GetOutput() const = 0;
	virtual void Print(std::ostream& stream, const std::string& identifier) const = 0;
private:
	uint64_t time_stamp;
};

///////////////////////////////// OutputBase //////////////////////////////////

class OutputBase
{
public:
	OutputBase(const std::string& name, const std::string& type, unsigned int size);
	virtual ~OutputBase();
	inline const std::string& GetName() const;
	inline const std::string& GetType() const;
	inline unsigned int GetSize() const;
	void Add(Variable& variable);
	void Remove(Variable& variable);
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanVariables(SCANNER& scanner, ARG_TYPE& arg) const;
protected:
	virtual void UpdateBefore(uint64_t before) = 0;
	virtual void UpdateUntil(uint64_t until) = 0;
protected:
	void Register();
	void Unregister();
	void AttachVariables();
	inline void RequestUpdate();
	inline bool HasAttachedVariables() const;
private:
	friend class OutputProcessor;
	
	std::string name;
	std::string type;
	unsigned int size;
	bool registered;
	bool update_requested;
	typedef std::vector<Variable *> Variables;
	Variables variables;
	
	struct VarAttachScanner
	{
		bool VisitVariable(Variable& variable, OutputBase& output);
	};
	
	struct WritersScanner
	{
		bool VisitWriter(Writer& writer, OutputBase& output);
	};

};

//////////////////////////////// OutputProcessor //////////////////////////////

class OutputProcessor
{
public:
	OutputProcessor();
	~OutputProcessor();
private:
	friend class OutputBase;
	friend class Writer;
	template <typename T> friend class Output;
	
	enum CommitPolicy
	{
		COMMIT_BEFORE = 0,
		COMMIT_UNTIL  = 1
	};
	
	unsigned int toggle;
	typedef std::vector<Writer *> Writers;
	Writers writers;
	typedef std::vector<OutputBase *> Outputs;
	Outputs outputs;
	Outputs updatable_outputs[2];
	typedef std::multimap<uint64_t, SampleBase *> Samples;
	Samples samples;
	uint64_t curr_time_stamp;
	uint64_t next_time_stamp;
	std::string timescale;
	bool timescale_fixed;
	std::ostream *log;
	std::ostream *warn_log;
	std::ostream *err_log;

	static inline OutputProcessor& Instance();

	void Register(Writer& writer);
	void Unregister(Writer& writer);
	bool Register(OutputBase& output);
	void Unregister(OutputBase& output);
	inline void RequestUpdate(OutputBase& output);
	inline void Add(SampleBase& sample);
	
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanWriters(SCANNER& scanner, ARG_TYPE& arg) const;
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanOutputs(SCANNER& scanner, ARG_TYPE& arg) const;
	
	void SetTimeScale(const std::string& timescale);
	void SetLog(std::ostream& log);
	void SetWarningLog(std::ostream& warn_log);
	void SetErrorLog(std::ostream& err_log);
	std::ostream& Log() const;
	std::ostream& WarnLog() const;
	std::ostream& ErrLog() const;
	template <CommitPolicy COMMIT_POLICY> void Commit(uint64_t target);
	
	struct SampleDispatcher
	{
		inline bool VisitVariable(Variable& variable, SampleBase& sample);
	};
};

/////////////////////////////////// Output<> //////////////////////////////////

template <typename T>
class Output : public OutputBase
{
public:
	Output(const std::string& name, const std::string& type = std::string());
	virtual ~Output();
	void Push(uint64_t time_stamp, const T& value);
	void Free(Sample<T> *sample);
protected:
	virtual void UpdateBefore(uint64_t before);
	virtual void UpdateUntil(uint64_t until);
private:
	uint64_t curr_time_stamp;
	typedef std::deque<Sample<T> *> Samples;
	Samples samples;
	typedef std::vector<Sample<T> *> FreeList;
	FreeList free_list;
	Sample<T> *last_sample;

	template <OutputProcessor::CommitPolicy COMMIT_POLICY> void Update(uint64_t target);
};

///////////////////////////////////// Variable ////////////////////////////////

class Variable
{
public:
	Variable(Writer& writer, Module& owner, const std::string& name, const std::string& identifier);
	virtual ~Variable();
	inline const std::string& GetName() const;
	inline const std::string& GetIdentifier() const;
	inline Writer& GetWriter() const;
	inline Module& GetOwner() const;
	inline OutputBase *GetOutput() const;
	inline void Add(SampleBase& sample);
	void Attach(OutputBase& output);
private:
	Writer& writer;
	Module& owner;
	OutputBase *output;
	std::string name;
	std::string identifier;
};

/////////////////////////////// Sample<> //////////////////////////////////////

template <typename T>
class Sample : public SampleBase
{
public:
	Sample(Output<T>& output);
	void Initialize(uint64_t time_stamp, const T& value);
	void SetValue(const T& value);
	virtual void Free();
	virtual const OutputBase& GetOutput() const;
	virtual void Print(std::ostream& stream, const std::string& identifier) const;
	const T& GetValue() const;
private:
	Output<T>& output;
	T value;
};

////////////////////////////////// Module /////////////////////////////////////

class Module
{
public:
	Module();
	Module(const std::string& name, Module& parent);
	~Module();
	inline const std::string& GetName() const;
	Module *FindChild(const std::string& name) const;
	void Add(Module& child);
	void Add(Variable& variable);
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanChildren(SCANNER& scanner, ARG_TYPE& arg) const;
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanVariables(SCANNER& scanner, ARG_TYPE& arg) const;
private:
	std::string name;
	typedef std::vector<Module *> Children;
	Children children;
	typedef std::vector<Variable *> Variables;
	Variables variables;
};

/////////////////////////////////// Writer ////////////////////////////////////

class Writer
{
public:
	static void SetTimeScale(const std::string& timescale);
	static inline void CommitBefore(uint64_t before);
	static inline void CommitUntil(uint64_t until);
	static void SetLog(std::ostream& log);
	static void SetWarningLog(std::ostream& warn_log);
	static void SetErrorLog(std::ostream& err_log);
	
	Writer(std::ostream& stream, bool interactive = false);
	Writer(std::ofstream& fstream, std::streamsize io_buffer_size = 8192);
	~Writer();
	void Trace(const std::string& name);
private:
	friend class OutputBase;
	friend class Variable;
	friend class OutputProcessor;
	
	std::ostream *stream;
	char *io_buffer;
	bool interactive;
	bool initialized;
	std::string next_variable_identifier;
	Module top;
	uint64_t curr_time_stamp;
	typedef std::vector<Variable *> Variables;
	Variables variables;
	
	void Initialize();
	const std::string& NextVariableIdentifier();
	Module& GetModule(Module& module, const std::string& leaf_hierarchical_name);
	Module& GetVariableOwner(const std::string& variable_name);
	inline void Add(Variable& variable, SampleBase& sample);
	inline void EndOfCommit();
	
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanVariables(SCANNER& scanner, ARG_TYPE& arg) const;
	
	struct OutputScanner
	{
		OutputBase *VisitOutput(OutputBase& output, const std::string& name);
	};
	
	struct Printer
	{
		bool VisitChild(Module& child, std::ostream& stream);
		bool VisitVariable(Variable& variable, std::ostream& stream);
	};

	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
	struct ModuleScanner
	{
		ARG_TYPE& arg;
		ModuleScanner(ARG_TYPE& arg);
		RET_TYPE VisitChild(Module& child, SCANNER& var_scanner);
		RET_TYPE VisitVariable(Variable& variable, SCANNER& var_scanner);
	};
};

///////////////////////////////////////////////////////////////////////////////
//                                 Definitions                               //
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////// SampleBase ////////////////////////////////

inline SampleBase::SampleBase()
	: time_stamp(0)
{
}

inline void SampleBase::SetTimeStamp(uint64_t _time_stamp)
{
	time_stamp = _time_stamp;
}

inline uint64_t SampleBase::GetTimeStamp() const
{
	return time_stamp;
}

///////////////////////////////// OutputBase //////////////////////////////////

inline const std::string& OutputBase::GetName() const
{
	return name;
}

inline const std::string& OutputBase::GetType() const
{
	return type;
}

inline unsigned int OutputBase::GetSize() const
{
	return size;
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE OutputBase::ScanVariables(SCANNER& scanner, ARG_TYPE& arg) const
{
	for(Variables::const_iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable *variable = *it;
		RET_TYPE r = scanner.VisitVariable(*variable, arg);
		if(r) return r;
	}
	return RET_TYPE();
}

inline void OutputBase::RequestUpdate()
{
	if(!update_requested)
	{
		OutputProcessor::Instance().RequestUpdate(*this);
		update_requested = true;
	}
}

inline bool OutputBase::HasAttachedVariables() const
{
	return variables.size() != 0;
}

/////////////////////////////////// Output<> //////////////////////////////////

template <typename T>
Output<T>::Output(const std::string& _name, const std::string& _type)
	: OutputBase(_name, _type.length() ? _type : std::string(TypeTrait<T>::GetDefaultType()), TypeTrait<T>::GetSize())
	, curr_time_stamp(0)
	, samples()
	, free_list()
	, last_sample(0)
{
	this->Register();
}

template <typename T>
Output<T>::~Output()
{
	this->Unregister();
	for(typename Samples::iterator it = samples.begin(); it != samples.end(); ++it)
	{
		Sample<T> *sample = *it;
		delete sample;
	}
	for(typename FreeList::iterator it = free_list.begin(); it != free_list.end(); ++it)
	{
		Sample<T> *sample = *it;
		delete sample;
	}
	if(last_sample) delete last_sample;
}

template <typename T>
void Output<T>::Push(uint64_t time_stamp, const T& value)
{
	if(unlikely(time_stamp < curr_time_stamp))
	{
		OutputProcessor::Instance().ErrLog() << "When pushing time stamped values through Output \"" << this->GetName() << "\", the time stamp shall increase monotonically" << std::endl;
		return;
	}

	if(likely(!HasAttachedVariables()))
	{
		curr_time_stamp = time_stamp;
		return;
	}
	
	if(likely(last_sample))
	{
		if(unlikely(value == last_sample->GetValue()))
		{
			curr_time_stamp = time_stamp;
			return;
		}
		if(likely(samples.size()))
		{
			if(unlikely(time_stamp == curr_time_stamp))
			{
				assert(last_sample->GetTimeStamp() == curr_time_stamp);
				last_sample->SetValue(value);
				return;
			}
		}
		else
		{
			free_list.push_back(last_sample);
		}
	}
	
	curr_time_stamp = time_stamp;
	
	Sample<T> *sample;
	if(likely(free_list.size()))
	{
		sample = free_list.back();
		free_list.pop_back();
	}
	else
	{
		sample = new Sample<T>(*this);
	}

	sample->Initialize(curr_time_stamp, value);
	samples.push_back(sample);
	last_sample = sample;
	RequestUpdate();
}

template <typename T>
void Output<T>::Free(Sample<T> *sample)
{
	if(sample != last_sample)
	{
		free_list.push_back(sample);
	}
}

template <typename T>
void Output<T>::UpdateBefore(uint64_t before)
{
	Update<OutputProcessor::COMMIT_BEFORE>(before);
}

template <typename T>
void Output<T>::UpdateUntil(uint64_t until)
{
	Update<OutputProcessor::COMMIT_UNTIL>(until);
}

template <typename T>
template <OutputProcessor::CommitPolicy COMMIT_POLICY>
void Output<T>::Update(uint64_t target)
{
	if(likely(samples.size()))
	{
		Sample<T> *sample = samples.front();
		if(((COMMIT_POLICY == OutputProcessor::COMMIT_BEFORE) && ((sample->GetTimeStamp() <  target) || !target)) ||
		   ((COMMIT_POLICY == OutputProcessor::COMMIT_UNTIL ) && ((sample->GetTimeStamp() <= target))))
			
		{
			do
			{
				OutputProcessor::Instance().Add(*sample);
				samples.pop_front();
			}
			while(samples.size() &&
			      (((COMMIT_POLICY == OutputProcessor::COMMIT_BEFORE) && (((sample = samples.front())->GetTimeStamp() <  target) || !target)) ||
			       ((COMMIT_POLICY == OutputProcessor::COMMIT_UNTIL ) && (((sample = samples.front())->GetTimeStamp() <= target)))));
		}
		
		if(unlikely(samples.size()))
		{
			RequestUpdate();
		}
	}
	switch(COMMIT_POLICY)
	{
		case OutputProcessor::COMMIT_BEFORE:
			assert(target != 0);
			if(target > curr_time_stamp) curr_time_stamp = target;
			break;
			
		case OutputProcessor::COMMIT_UNTIL:
			if(target >= curr_time_stamp) curr_time_stamp = target + 1;
			break;
	}
}

///////////////////////////////////// Variable ////////////////////////////////

inline const std::string& Variable::GetName() const
{
	return name;
}

inline const std::string& Variable::GetIdentifier() const
{
	return identifier;
}

inline Writer& Variable::GetWriter() const
{
	return writer;
}

inline Module& Variable::GetOwner() const
{
	return owner;
}

inline OutputBase *Variable::GetOutput() const
{
	return output;
}

inline void Variable::Add(SampleBase& sample)
{
	writer.Add(*this, sample);
}

/////////////////////////////// Sample<> //////////////////////////////////////

template <typename T>
Sample<T>::Sample(Output<T>& _output)
	: SampleBase()
	, output(_output)
	, value()
{
}

template <typename T>
void Sample<T>::Initialize(uint64_t _time_stamp, const T& _value)
{
	SampleBase::SetTimeStamp(_time_stamp);
	SetValue(_value);
}

template <typename T>
void Sample<T>::SetValue(const T& _value)
{
	value = _value;
}

template <typename T>
void Sample<T>::Free()
{
	output.Free(this);
}

template <typename T>
const OutputBase& Sample<T>::GetOutput() const
{
	return output;
}

template <typename T>
void Sample<T>::Print(std::ostream& stream, const std::string& identifier) const
{
	TypeTrait<T>::Print(stream, value, identifier);
}

template <typename T>
const T& Sample<T>::GetValue() const
{
	return value;
}

////////////////////////////////// Module /////////////////////////////////////

inline const std::string& Module::GetName() const
{
	return name;
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE Module::ScanChildren(SCANNER& scanner, ARG_TYPE& arg) const
{
	for(Children::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		Module *child = *it;
		RET_TYPE r = scanner.VisitChild(*child, arg);
		if(r) return r;
	}
	return RET_TYPE();
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE Module::ScanVariables(SCANNER& scanner, ARG_TYPE& arg) const
{
	for(Variables::const_iterator it = variables.begin(); it != variables.end(); ++it)
	{
		Variable *variable = *it;
		RET_TYPE r = scanner.VisitVariable(*variable, arg);
		if(r) return r;
	}
	return RET_TYPE();
}

/////////////////////////////////// Writer ////////////////////////////////////

inline void Writer::CommitBefore(uint64_t before)
{
	OutputProcessor::Instance().Commit<OutputProcessor::COMMIT_BEFORE>(before);
}

inline void Writer::CommitUntil(uint64_t until)
{
	OutputProcessor::Instance().Commit<OutputProcessor::COMMIT_UNTIL>(until);
}

inline void Writer::Add(Variable& variable, SampleBase& sample)
{
	uint64_t time_stamp = sample.GetTimeStamp();
	bool new_time_stamp = !initialized || (time_stamp != curr_time_stamp);
	
	if(!initialized)
	{
		Initialize();
	}
	
	if(new_time_stamp)
	{
		curr_time_stamp = time_stamp;
		(*stream) << "#" << curr_time_stamp << '\n';
	}
	
	sample.Print(*stream, variable.GetIdentifier());
	(*stream) << '\n';
}

inline void Writer::EndOfCommit()
{
	if(interactive)
	{
		(*stream).flush();
	}
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
Writer::ModuleScanner<SCANNER, ARG_TYPE, RET_TYPE>::ModuleScanner(ARG_TYPE& _arg)
	: arg(_arg)
{
}
	
template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE Writer::ModuleScanner<SCANNER, ARG_TYPE, RET_TYPE>::VisitChild(Module& child, SCANNER& var_scanner)
{
	RET_TYPE r = child.ScanVariables<ModuleScanner, SCANNER, RET_TYPE>(*this, var_scanner);
	if(r) return r;
	return child.ScanChildren<ModuleScanner, SCANNER, RET_TYPE>(*this, var_scanner);
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE Writer::ModuleScanner<SCANNER, ARG_TYPE, RET_TYPE>::VisitVariable(Variable& variable, SCANNER& var_scanner)
{
	return var_scanner.VisitVariable(variable, arg);
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE Writer::ScanVariables(SCANNER& var_scanner, ARG_TYPE& arg) const
{
	ModuleScanner<SCANNER, ARG_TYPE, RET_TYPE> module_scanner(arg);
	RET_TYPE r = top.template ScanVariables<ModuleScanner<SCANNER, ARG_TYPE, RET_TYPE>, SCANNER, RET_TYPE>(module_scanner, var_scanner);
	if(r) return r;
	r = top.template ScanChildren<ModuleScanner<SCANNER, ARG_TYPE, RET_TYPE>, SCANNER, RET_TYPE>(module_scanner, var_scanner);
	return r;
}

//////////////////////////////// OutputProcessor //////////////////////////////

inline OutputProcessor& OutputProcessor::Instance()
{
	static OutputProcessor instance;
	return instance;
}

inline void OutputProcessor::RequestUpdate(OutputBase& output)
{
	updatable_outputs[toggle].push_back(&output);
}

inline void OutputProcessor::Add(SampleBase& sample)
{
	samples.insert(Samples::value_type(sample.GetTimeStamp(), &sample));
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE OutputProcessor::ScanWriters(SCANNER& scanner, ARG_TYPE& arg) const
{
	for(Writers::const_iterator it = writers.begin(); it != writers.end(); ++it)
	{
		Writer *writer = *it;
		RET_TYPE r = scanner.VisitWriter(*writer, arg);
		if(r) return r;
	}
	return RET_TYPE();
}

template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE>
RET_TYPE OutputProcessor::ScanOutputs(SCANNER& scanner, ARG_TYPE& arg) const
{
	for(Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
	{
		OutputBase *output = *it;
		RET_TYPE r = scanner.VisitOutput(*output, arg);
		if(r) return r;
	}
	return RET_TYPE();
}

inline bool OutputProcessor::SampleDispatcher::VisitVariable(Variable& variable, SampleBase& sample)
{
	variable.Add(sample);
	return false;
}

template <OutputProcessor::CommitPolicy COMMIT_POLICY>
void OutputProcessor::Commit(uint64_t target)
{
	if(unlikely(((COMMIT_POLICY == COMMIT_BEFORE) && (target < next_time_stamp)) ||
	            ((COMMIT_POLICY == COMMIT_UNTIL ) && (target < curr_time_stamp))))
	{
		ErrLog() << "When committing output timed stamp values, the time stamp shall increase monotonically" << std::endl;
		return;
	}
	
	timescale_fixed = true;
	
	Outputs& curr_updatable_outputs = updatable_outputs[toggle];
	
	if(unlikely(curr_updatable_outputs.size()))
	{
		toggle ^= 1;
		
		for(Outputs::iterator output_it = curr_updatable_outputs.begin(); output_it != curr_updatable_outputs.end(); ++output_it)
		{
			OutputBase *output = *output_it;
			output->update_requested = false;
			switch(COMMIT_POLICY)
			{
				case COMMIT_BEFORE:	if(!target) { output->UpdateBefore(target); } break;
				case COMMIT_UNTIL : output->UpdateUntil(target);  break;
			}
		}
		curr_updatable_outputs.clear();
	}
	
	if(unlikely(samples.size()))
	{
		Samples::iterator sample_it = samples.begin();
			
		uint64_t sample_time_stamp = (*sample_it).first;
		
		if(((COMMIT_POLICY == COMMIT_BEFORE) && ((sample_time_stamp < target) || !target)) ||
		((COMMIT_POLICY == COMMIT_UNTIL ) &&  (sample_time_stamp <= target)))
		{
			do
			{
				curr_time_stamp = sample_time_stamp;
				SampleBase& sample = *(*sample_it).second;
				const OutputBase& output = sample.GetOutput();
				
				SampleDispatcher sample_dispatcher;
				output.ScanVariables<SampleDispatcher, SampleBase, bool>(sample_dispatcher, sample);
				sample.Free();
				samples.erase(sample_it);
				sample_it = samples.begin();
			}
			while((sample_it != samples.end()) &&
			      (((COMMIT_POLICY == COMMIT_BEFORE) && (((sample_time_stamp = (*sample_it).first) < target) || !target)) ||
			      ((COMMIT_POLICY == COMMIT_UNTIL) && ((sample_time_stamp = (*sample_it).first) <= target))));
		}
		for(Writers::iterator writer_it = writers.begin(); writer_it != writers.end(); ++writer_it)
		{
			Writer *writer = *writer_it;
			writer->EndOfCommit();
		}
	}
	switch(COMMIT_POLICY)
	{
		case COMMIT_BEFORE: next_time_stamp = target;     break;
		case COMMIT_UNTIL : next_time_stamp = target + 1; break;
	}
}

} // end of namespace vcd
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_VCD_VCD_HH__
