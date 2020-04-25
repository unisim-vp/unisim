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
#include <sstream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <set>
#include <climits>
#include <cassert>
#include <stdint.h>

namespace unisim {
namespace util {
namespace vcd {

//                      Outputs                   OutputProcessor                    Writers                VCD Bridges
//                                                                   
//                                          +-------------------------+        +-----------------+      +-----------------+
//                       . . .              |                         |        |                 | VCD  |                 |
//                                          |                         |        | StreamWriter    |=====>|  std::ostream   |  
//                       . . .              |                         |        |                 |      |                 |
//                                          |                         |        +-----------------+      +-----------------+
//                       . . .              |                         | Write  |                 |
//                                          |                         |=======>| Writer          |
//                       . . .              |                         |        | * Trace(...)    |
//                                          |                         |        | * Initialize()  |
//               +-------------------+      |                         |        | * Write(...)    |
//          Push |                   | Pull |                         |        | * EndOfCommit() |
// (t,val) =====>| Output<T>         |=====>| OutputProcessor         |        +-----------------+
//               | * Push(...)       |      | * CommitBefore(t)       |   
//               +-------------------+      | * CommitUntil(t)        |        +-----------------+      +-----------------+
//               |                   |      | * RequestPull()         |        |                 | VCD  |                 |
//               | OutputBase        |      | * RequestCommitUntil(t) |        | StreamWriter    |=====>|  GTKWave Stream |=====> GTKWave
//               | * CommitBefore(t) |      |                         |        |                 |      |                 |
//               | * CommitUntil(t)  |      |                         |        +-----------------+      +-----------------+
//               +-------------------+      |                         | Write  |                 |
//                       . . .              |                         |=======>| Writer          |
//                                          |                         |        | * Trace(...)    |
//                       . . .              |                         |        | * Initialize()  |
//                                          |                         |        | * Write(...)    |
//                       . . .              |                         |        | * EndOfCommit() |
//                                          +-------------------------+        +-----------------+

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
class StreamWriter;
class Module;
class OutputProcessor;

///////////////////////////////////////////////////////////////////////////////
//                                Declarations                               //
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////// Option ////////////////////////////////////

enum Option
{
	OPT_SORT_VARIABLES,
	OPT_DEBUG
};

//////////////////////////////////// Time /////////////////////////////////////

typedef uint64_t Time;

////////////////////////////// Global Functions ///////////////////////////////

void SetOption(Option opt, bool flag);
void GetOption(Option opt, bool& flag);
void SetTimeScale(const std::string& timescale);
inline const std::string& TimeScale();
void SetLog(std::ostream& log);
void SetWarningLog(std::ostream& warn_log);
void SetErrorLog(std::ostream& err_log);
inline std::ostream& Log();
inline std::ostream& WarningLog();
inline std::ostream& ErrorLog();
inline void CommitBefore(Time before);
inline void CommitUntil(Time until);
inline Time TimeStamp();

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
	inline void SetTimeStamp(Time time_stamp);
	inline Time GetTimeStamp() const;
	virtual void Free() = 0;
	virtual const OutputBase& GetOutput() const = 0;
	virtual void Print(std::ostream& stream, const std::string& identifier) const = 0;
	inline void Catch();
	inline void Release();
private:
	Time time_stamp;
	unsigned int refcount;
};

//////////////////////////////// OutputProcessor //////////////////////////////

class OutputProcessor
{
private:
	OutputProcessor();
	~OutputProcessor();
	friend class OutputBase;
	friend class Writer;
	template <typename T> friend class Output;
	friend void SetOption(Option opt, bool flag);
	friend void GetOption(Option opt, bool& flag);
	friend void SetTimeScale(const std::string& timescale);
	friend const std::string& TimeScale();
	friend void SetLog(std::ostream& log);
	friend void SetWarningLog(std::ostream& warn_log);
	friend void SetErrorLog(std::ostream& err_log);
	friend std::ostream& Log();
	friend std::ostream& WarningLog();
	friend std::ostream& ErrorLog();
	friend void CommitBefore(Time before);
	friend void CommitUntil(Time until);
	friend Time TimeStamp();

	unsigned int toggle;
	typedef std::vector<Writer *> Writers;
	Writers writers;
	Writers uninitialized_writers;
	typedef std::vector<OutputBase *> Outputs;
	Outputs outputs;
	Outputs pullable_outputs[2];
	typedef std::vector<bool> Agreements;
	struct Round
	{
		unsigned int agreement_count;
		Agreements agreements;
	};
	typedef std::map<Time, Round *> Rounds;
	Rounds rounds;
	typedef std::vector<Round *> FreeList;
	FreeList free_list;
	typedef std::multimap<Time, SampleBase *> Samples;
	Samples samples;
	Time curr_time_stamp;
	bool sort_variables;
	bool debug;
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
	inline void RequestPull(OutputBase& output);
	inline void RequestCommitUntil(OutputBase& output, Time until);
	inline void Add(SampleBase& sample);
	
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanWriters(SCANNER& scanner, ARG_TYPE& arg) const;
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanOutputs(SCANNER& scanner, ARG_TYPE& arg) const;
	
	void SetOption(Option opt, bool flag);
	void GetOption(Option opt, bool& flag);
	void SetTimeScale(const std::string& timescale);
	inline const std::string& TimeScale() const;
	inline Time TimeStamp() const;
	void SetLog(std::ostream& log);
	void SetWarningLog(std::ostream& warn_log);
	void SetErrorLog(std::ostream& err_log);
	std::ostream& Log() const;
	std::ostream& WarningLog() const;
	std::ostream& ErrorLog() const;
	inline void CommitBefore(Time before);
	inline void CommitUntil(Time until);
	void DumpRounds();
	
	struct SampleDispatcher
	{
		inline bool VisitVariable(Variable& variable, SampleBase& sample);
	};
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
	inline void CommitBefore(Time before);
	inline void CommitUntil(Time until);
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanVariables(SCANNER& scanner, ARG_TYPE& arg) const;
	virtual void Dump(std::ostream& stream, const std::string& identifier) = 0;
protected:
	Time push_time_stamp;

	virtual void PullUntil(Time until) = 0;
	void Register();
	void Unregister();
	void AttachVariables();
	void DetachVariables();
	inline void RequestPull();
	inline bool HasAttachedVariables() const;
	inline unsigned int GetId() const;
	void SetId(unsigned int id);
private:
	friend class OutputProcessor;
	
	std::string name;
	std::string type;
	unsigned int size;
	unsigned int id;
	bool registered;
	bool pull_requested;
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

/////////////////////////////////// Output<> //////////////////////////////////

template <typename T>
class Output : public OutputBase
{
public:
	Output(const std::string& name, const std::string& type = std::string());
	Output(const std::string& name, const T& init_value, const std::string& type = std::string());
	virtual ~Output();
	void Push(Time time_stamp, const T& value);
	virtual void Dump(std::ostream& stream, const std::string& identifier);
protected:
	virtual void PullUntil(Time until);
private:
	friend class Sample<T>;
	
	typedef std::deque<Sample<T> *> Samples;
	Samples samples;
	typedef std::vector<Sample<T> *> FreeList;
	FreeList free_list;
	Sample<T> *init_sample;
	Sample<T> *last_sample;
	Sample<T> *last_pulled_sample;
	
	Sample<T> *Allocate();
	void Free(Sample<T> *sample);
};

///////////////////////////////////// Variable ////////////////////////////////

class Variable
{
public:
	Variable(Writer& writer, Module& owner, const std::string& name, const std::string& identifier, unsigned int id);
	virtual ~Variable();
	inline const std::string& GetName() const;
	inline const std::string& GetIdentifier() const;
	inline unsigned int GetId() const;
	inline Writer& GetWriter() const;
	inline Module& GetOwner() const;
	inline OutputBase *GetOutput() const;
	inline void Write(SampleBase& sample);
	void Attach(OutputBase& output);
	void Detach();
	void Dump(std::ostream& stream);
private:
	Writer& writer;
	Module& owner;
	OutputBase *output;
	std::string name;
	std::string identifier;
	unsigned int id;
};

/////////////////////////////// Sample<> //////////////////////////////////////

template <typename T>
class Sample : public SampleBase
{
public:
	Sample(Output<T>& output);
	void Initialize(Time time_stamp, const T& value);
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
	Writer();
	virtual ~Writer();
	void Trace(const std::string& name);
protected:
	virtual void Initialize() = 0;
	virtual void Write(Variable& variable, SampleBase& sample) = 0;
	virtual void EndOfCommit() = 0;
private:
	friend class OutputBase;
	friend class Variable;
	friend class OutputProcessor;
	
	std::string next_variable_identifier;
	unsigned int next_id;
protected:
	Module top;
	typedef std::vector<Variable *> Variables;
	Variables variables;
private:	
	const std::string& NextVariableIdentifier();
	Module& GetModule(Module& module, const std::string& leaf_hierarchical_name);
	Module& GetVariableOwner(const std::string& variable_name);
	
	template <typename SCANNER, typename ARG_TYPE, typename RET_TYPE> RET_TYPE ScanVariables(SCANNER& scanner, ARG_TYPE& arg) const;
	
	struct OutputScanner
	{
		OutputBase *VisitOutput(OutputBase& output, const std::string& name);
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

/////////////////////////////// StreamWriter //////////////////////////////////

class StreamWriter : public Writer
{
public:
	StreamWriter(std::ostream& stream, bool interactive = false);
	StreamWriter(std::ofstream& fstream, std::streamsize io_buffer_size = 8192);
	virtual ~StreamWriter();
protected:
	virtual void Initialize();
	virtual void Write(Variable& variable, SampleBase& sample);
	virtual void EndOfCommit();
private:
	std::ostream *stream;
	char *io_buffer;
	bool interactive;
	bool end_of_commit;
	bool sort_variables;
	Time curr_time_stamp;
	typedef std::vector<std::ostringstream *> StreamPool;
	StreamPool stream_pool;
	typedef std::set<unsigned int> EmitOrder;
	EmitOrder emit_order;
	
	inline void Emit(Variable& variable, SampleBase& sample);
	inline void SortedEmit();
	
	struct Printer
	{
		bool VisitChild(Module& child, std::ostream& stream);
		bool VisitVariable(Variable& variable, std::ostream& stream);
	};
};

///////////////////////////////////////////////////////////////////////////////
//                                 Definitions                               //
///////////////////////////////////////////////////////////////////////////////

inline const std::string& TimeScale()
{
	return OutputProcessor::Instance().TimeScale();
}

inline std::ostream& Log()
{
	return OutputProcessor::Instance().Log();
}

inline std::ostream& WarningLog()
{
	return OutputProcessor::Instance().WarningLog();
}

inline std::ostream& ErrorLog()
{
	return OutputProcessor::Instance().ErrorLog();
}

inline void CommitBefore(Time before)
{
	OutputProcessor::Instance().CommitBefore(before);
}

inline void CommitUntil(Time until)
{
	OutputProcessor::Instance().CommitUntil(until);
}

inline Time TimeStamp()
{
	return OutputProcessor::Instance().TimeStamp();
}

/////////////////////////////////// SampleBase ////////////////////////////////

inline SampleBase::SampleBase()
	: time_stamp(0)
	, refcount(0)
{
}

inline void SampleBase::SetTimeStamp(Time _time_stamp)
{
	time_stamp = _time_stamp;
}

inline Time SampleBase::GetTimeStamp() const
{
	return time_stamp;
}

inline void SampleBase::Catch()
{
	++refcount;
}

inline void SampleBase::Release()
{
	if(refcount && (--refcount == 0)) Free();
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

inline unsigned int OutputBase::GetId() const
{
	return id;
}

inline void OutputBase::CommitBefore(Time before)
{
	CommitUntil(before - 1);
}

inline void OutputBase::CommitUntil(Time until)
{
	OutputProcessor::Instance().RequestCommitUntil(*this, until);
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

inline void OutputBase::RequestPull()
{
	if(!pull_requested)
	{
		OutputProcessor::Instance().RequestPull(*this);
		pull_requested = true;
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
	, samples()
	, free_list()
	, init_sample(0)
	, last_sample(0)
	, last_pulled_sample(0)
{
	this->Register();
	init_sample = Allocate();
	init_sample->Catch();
	init_sample->Initialize(TimeStamp(), T());
	last_sample = init_sample;
	last_sample->Catch();
}

template <typename T>
Output<T>::Output(const std::string& _name, const T& _init_value, const std::string& _type)
	: OutputBase(_name, _type.length() ? _type : std::string(TypeTrait<T>::GetDefaultType()), TypeTrait<T>::GetSize())
	, samples()
	, free_list()
	, init_sample(0)
	, last_sample(0)
	, last_pulled_sample(0)
{
	this->Register();
	init_sample = Allocate();
	init_sample->Catch();
	init_sample->Initialize(TimeStamp(), _init_value);
	last_sample = init_sample;
	last_sample->Catch();
}

template <typename T>
Output<T>::~Output()
{
	this->Unregister();
	init_sample->Release();
	if(last_sample) last_sample->Release();
	if(last_pulled_sample) last_pulled_sample->Release();
	for(typename Samples::iterator it = samples.begin(); it != samples.end(); ++it)
	{
		Sample<T> *sample = *it;
		sample->Release();
	}
	for(typename FreeList::iterator it = free_list.begin(); it != free_list.end(); ++it)
	{
		Sample<T> *sample = *it;
		delete sample;
	}
}

template <typename T>
void Output<T>::Push(Time time_stamp, const T& value)
{
	// check that push time stamp is not in the past
	if(unlikely(time_stamp < push_time_stamp))
	{
		ErrorLog() << "When pushing time stamped values through Output \"" << this->GetName() << "\", the time stamp shall increase monotonically" << std::endl;
		return;
	}

	// if there are not attached variables, just keep push time stamp and return
	if(likely(!HasAttachedVariables()))
	{
		push_time_stamp = time_stamp;
		return;
	}
	
	// if value is same as previous, just keep push time stamp and return
	if(unlikely(value == last_sample->GetValue()))
	{
		push_time_stamp = time_stamp;
		return;
	}
	
	// is it first push?
	if(likely(last_sample != init_sample))
	{
		// no: not first push
		
		// was last sample pulled?
		if(unlikely(samples.size()))
		{
			// no: not pulled
			
			// is push time stamp same as previous push?
			if(unlikely(time_stamp == push_time_stamp))
			{
				// yes: same time stamp
				assert(last_sample->GetTimeStamp() == push_time_stamp);
				
				// just update value of last unpushed sample
				last_sample->SetValue(value);
				return;
			}
			else
			{
				// no: different time stamp
				
				// drop last sample 
				last_sample->Release();
				last_sample = 0;
			}
		}
	}
	else
	{
		// yes: first push
		
		// drop last sample that was initial sample
		last_sample->Release();
		last_sample = 0;
	}
	
	// keep push time stamp
	push_time_stamp = time_stamp;
	
	Sample<T> *sample;
	// reuse last unpushed sample if it exists
	if(likely(last_sample))
	{
		sample = last_sample;
	}
	else // else allocate a new sample
	{
		last_sample = sample = Allocate();
		last_sample->Catch();
	}

	// initialize sample and enqueue it
	sample->Initialize(push_time_stamp, value);
	samples.push_back(sample);
	sample->Catch();
	
	// request pull of samples as soon as possible
	RequestPull();
}

template <typename T>
Sample<T> *Output<T>::Allocate()
{
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
	return sample;
}

template <typename T>
void Output<T>::Free(Sample<T> *sample)
{
	free_list.push_back(sample);
}

template <typename T>
void Output<T>::Dump(std::ostream& stream, const std::string& identifier)
{
	// dump last pulled sample if present otherwise dump initial sample
	TypeTrait<T>::Print(stream, last_pulled_sample ? last_pulled_sample->GetValue() : init_sample->GetValue(), identifier);
}

template <typename T>
void Output<T>::PullUntil(Time until)
{
	// if there are some samples to pull
	if(likely(samples.size()))
	{
		// pull sample until time stamp
		Sample<T> *sample = samples.front();
		if(sample->GetTimeStamp() <= until)
		{
			do
			{
				// send sample to further dispatching toward writers
				OutputProcessor::Instance().Add(*sample);
				samples.pop_front();
				
				// keep last pulled sample
				if(last_pulled_sample) last_pulled_sample->Release();
				last_pulled_sample = sample;
			}
			while(samples.size() && ((sample = samples.front())->GetTimeStamp() <= until));
		}
		
		// if there are still samples to pull, request pull
		if(unlikely(samples.size()))
		{
			RequestPull();
		}
	}
	
	// Do not accept push before pull
	if(likely(until >= push_time_stamp)) push_time_stamp = until + 1;
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

inline unsigned int Variable::GetId() const
{
	return id;
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

inline void Variable::Write(SampleBase& sample)
{
	writer.Write(*this, sample);
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
void Sample<T>::Initialize(Time _time_stamp, const T& _value)
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

/////////////////////////////// StreamWriter //////////////////////////////////

inline void StreamWriter::Emit(Variable& variable, SampleBase& sample)
{
	if(unlikely(sort_variables))
	{
		unsigned int variable_id = variable.GetId();
		std::ostringstream* output_stream = stream_pool[variable_id];
		output_stream->str(std::string());
		sample.Print(*output_stream, variable.GetIdentifier());
		(*output_stream) << '\n';
		emit_order.insert(variable_id);
	}
	else
	{
		sample.Print(*stream, variable.GetIdentifier());
		(*stream) << '\n';
	}
}

inline void StreamWriter::SortedEmit()
{
	if(unlikely(sort_variables))
	{
		if(unlikely(emit_order.size()))
		{
			EmitOrder::iterator it = emit_order.begin();
			do
			{
				unsigned int variable_id = *it;
				std::ostringstream *osstr = stream_pool[variable_id];
				(*stream) << osstr->str();
			}
			while(++it != emit_order.end());
			emit_order.clear();
		}
	}
}

//////////////////////////////// OutputProcessor //////////////////////////////

inline OutputProcessor& OutputProcessor::Instance()
{
	static OutputProcessor instance;
	return instance;
}

inline void OutputProcessor::RequestPull(OutputBase& output)
{
	pullable_outputs[toggle].push_back(&output);
}

inline void OutputProcessor::RequestCommitUntil(OutputBase& output, Time until)
{
	if(until < curr_time_stamp) return; // already committed
	
	if(unlikely(debug))
	{
		Log() << "RequestCommitUntil(output=#" << output.GetId() << ",until=" << until << "):" << std::endl;
		DumpRounds(); Log() << std::endl;
	}
	unsigned output_id = output.GetId();
	bool agree = false;
	bool existing_round = false;
	Time agreed_time_stamp;
	Round *next_round = 0;
	
	// for each round
	if(rounds.size())
	{
		Rounds::iterator it = rounds.begin();
		do
		{
			Time round_time_stamp = (*it).first;
			Round *round = (*it).second;
			unsigned int& agreement_count = round->agreement_count;
			unsigned int num_voters = outputs.size();
			Agreements& agreements = round->agreements;
			// if it's a last minute vote, and all other voters have agreed beyond round date, then consider we all agree
			if(round_time_stamp > until)
			{
				next_round = round;
				if((agreement_count + 1) >= num_voters)
				{
					agree = true;
					agreed_time_stamp = until;
				}
				break;
			}
			else if(round_time_stamp <= until)
			{
				// vote to past rounds (acknowledge) we've not participated
				if(round_time_stamp == until)
				{
					existing_round = true;
				}
				assert(output_id < agreements.size());
				if(!agreements[output_id])
				{
					agreements[output_id] = true;
					assert(output_id < num_voters);
					if(++agreement_count >= num_voters)
					{
						agree = true;
						agreed_time_stamp = round_time_stamp;
					}
				}
			}
		}
		while(++it != rounds.end());
	}
	if(agree)
	{
		// if we agree, then commit until agreed time stamp
		if(unlikely(debug))
		{
			Log() << "Agreed:" << std::endl;
			DumpRounds(); Log() << std::endl;
		}
		CommitUntil(agreed_time_stamp);
	}
	else if(!existing_round)
	{
		// if we're not agreed and round has not started, then start a new round and vote
		Round *round;
		if(likely(free_list.size()))
		{
			round = free_list.back();
			free_list.pop_back();
		}
		else
		{
			round = new Round();
		}
		if(next_round)
		{
			// take care of next round agreements, as agreements of further rounds applies to previous rounds
			*round = *next_round;
			++round->agreement_count;
			round->agreements[output_id] = true;
		}
		else
		{
			// very last round, we're the first to vote
			round->agreement_count = 1;
			round->agreements.clear();
			round->agreements.resize(outputs.size());
			round->agreements[output_id] = true;
		}
		rounds.insert(Rounds::value_type(until, round));
		if(unlikely(debug))
		{
			Log() << "Not existing:" << std::endl;
			DumpRounds(); Log() << std::endl;
		}
	}
}

inline void OutputProcessor::Add(SampleBase& sample)
{
	sample.Catch();
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
	variable.Write(sample);
	return false;
}

inline const std::string& OutputProcessor::TimeScale() const
{
	return timescale;
}

inline Time OutputProcessor::TimeStamp() const
{
	return curr_time_stamp;
}

inline void OutputProcessor::CommitBefore(Time before)
{
	CommitUntil(before - 1);
}

inline void OutputProcessor::CommitUntil(Time until)
{
	if(unlikely(debug))
	{
		Log() << "CommitUntil(until=" << until << ")" << std::endl;
	}
	
	// check that global commit is not in the past
	if(until < curr_time_stamp)
	{
		ErrorLog() << "When committing output timed stamp values, the time stamp shall increase monotonically" << std::endl;
		return;
	}
	
	// at this point, time scale can no longer change
	timescale_fixed = true;
	
	// initialize recently registered writers 
	if(unlikely(uninitialized_writers.size()))
	{
		Writers::iterator writer_it = uninitialized_writers.begin();
		do
		{
			Writer *writer = *writer_it;
			writer->Initialize();
		}
		while(++writer_it != uninitialized_writers.end());
		uninitialized_writers.clear();
	}
	
	// clean up rounds until time stamp
	if(unlikely(rounds.size()))
	{
		Rounds::iterator it = rounds.begin();
		do
		{
			Rounds::iterator next_it = it;
			++next_it;
			Round *round = (*it).second;
			free_list.push_back(round);
			rounds.erase(it);
			it = next_it;
		}
		while((it != rounds.end()) && ((*it).first <= until));
	}

	// pull samples until time stamp
	Outputs& curr_pullable_outputs = pullable_outputs[toggle];
	
	if(unlikely(curr_pullable_outputs.size()))
	{
		// swap the lists of pullable outputs so that we can read the list
		// while outputs readd themselves in the list
		// when there are still more sample to pull in the future
		toggle ^= 1;
		
		Outputs::iterator output_it = curr_pullable_outputs.begin();
		do
		{
			OutputBase *output = *output_it;
			output->pull_requested = false;
			output->PullUntil(until);
		}
		while(++output_it != curr_pullable_outputs.end());
		curr_pullable_outputs.clear();
	}
	
	// process each pulled samples from outputs
	if(unlikely(samples.size()))
	{
		Samples::iterator sample_it = samples.begin();
			
		do
		{
			curr_time_stamp = (*sample_it).first;
			assert(curr_time_stamp <= until);
			SampleBase& sample = *(*sample_it).second;
			const OutputBase& output = sample.GetOutput();
			
			// dispatch pulled samples to writers
			SampleDispatcher sample_dispatcher;
			output.ScanVariables<SampleDispatcher, SampleBase, bool>(sample_dispatcher, sample);
			sample.Release();
			samples.erase(sample_it);
			sample_it = samples.begin();
		}
		while(sample_it != samples.end());
		
		// notify the end of commit to writers
		assert(writers.size());
		Writers::iterator writer_it = writers.begin();
		do
		{
			Writer *writer = *writer_it;
			writer->EndOfCommit();
		}
		while(++writer_it != writers.end());
	}
	
	// Do not accept commit at the time stamp or before
	++curr_time_stamp;
}

} // end of namespace vcd
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_VCD_VCD_HH__
