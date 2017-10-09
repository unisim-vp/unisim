/*
 *  Copyright (c) 2017,
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
 
#ifndef __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_HH__
#define __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
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

#include <unisim/util/debug/profile.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/ieee754/ieee754.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>

#include <systemc>

#include <inttypes.h>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <stack>
#include <set>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windef.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

////////////////////////// Forward Declarations ///////////////////////////////

template <typename ADDRESS> class AddressProfileBase;
template <typename T> class Sample;
template <typename ADDRESS, typename T> class AddressProfile;
class FunctionInstructionProfileBase;
template <typename ADDRESS, typename T> class FunctionInstructionProfile;
template <typename ADDRESS> class FunctionNameLocationConversionBase;
template <typename ADDRESS> class FunctionNameLocationConversion;
template <typename ADDRESS> class SourceCodeProfileBase;
template <typename ADDRESS, typename T> class SourceCodeProfile;
class AnnotatedSourceCodeFileBase;
template <typename ADDRESS, typename T> class AnnotatedSourceCodeFile;
class AnnotatedSourceCodeFileSetBase;
template <typename ADDRESS, typename T> class AnnotatedSourceCodeFileSet;
template <typename ADDRESS> class Profiler;

//////////////////////////////// Format ///////////////////////////////////////

enum OutputFormat
{
	O_FMT_TEXT,
	O_FMT_HTML,
	O_FMT_CSV
};

std::ostream& operator << (std::ostream& os, OutputFormat o_fmt);

std::string OutputFormatSuffix(OutputFormat o_fmt);

/////////////////////////// c_string_to_HTML //////////////////////////////////

std::string c_string_to_HTML(const char *s);

///////////////////////////// c_string_to_URL /////////////////////////////////

std::string c_string_to_URL(const char *s);

///////////////////////////// c_string_to_CSV /////////////////////////////////

std::string c_string_to_CSV(const char *s);

//////////////////////////////// MakeDir //////////////////////////////////////

void MakeDir(const char *dirname);

///////////////////////////// OStreamContext //////////////////////////////////

class OStreamContext
{
public:
	OStreamContext(std::ostream& _os) : os(_os), fill(os.fill()), flags(os.flags()) {}
	~OStreamContext() { os.fill(fill); os.flags(flags); }
private:
	std::ostream& os;
	std::ostream::char_type fill;
	std::ios_base::fmtflags flags;
};

//////////////////////////////// Sample ///////////////////////////////////////

template <typename T>
class Sample
{
public:
	Sample(unisim::kernel::service::Variable<T> *_var)
		: var(_var)
		, prev_value()
		, curr_value()
		, diff_value()
	{
	}

	const T& Get() const { return diff_value; }
	void Capture();
	void Print(std::ostream& os) const { os << diff_value; }
	const char *GetSampledVariableName() const { return var->GetName(); }
	unisim::kernel::service::Variable<T> *GetSampledVariable() const { return var; }
	
private:
	unisim::kernel::service::Variable<T> *var;
	T prev_value;
	T curr_value;
	T diff_value;
};

////////////////////////// InstructionProfileBase /////////////////////////////

class InstructionProfileBase
{
public:
	virtual ~InstructionProfileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual void Print(std::ostream& os, OutputFormat o_fmt = O_FMT_TEXT, const char *csv_delimiter = ",") const = 0;
};

//////////////////////////// InstructionProfile<> /////////////////////////////

template <typename ADDRESS, typename T>
class InstructionProfile : public InstructionProfileBase
{
public:
	InstructionProfile(const AddressProfile<ADDRESS, T> *addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if);
	
	virtual const char *GetSampledVariableName() const { return addr_profile->GetSampledVariableName(); }
	virtual void Print(std::ostream& os, OutputFormat o_fmt, const char *csv_delimiter) const;
private:
	const AddressProfile<ADDRESS, T> *addr_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if;
};

//////////////////////////////// SLoc //////////////////////////////////////

class SLoc
{
public:
	SLoc() : filename(), lineno(1) {}
	SLoc(const char *_filename, unsigned int _lineno) : filename(_filename), lineno(_lineno) {}

	const char *GetFilename() const { return filename.c_str(); }
	unsigned int GetLineNo() const { return lineno; }
	
	int operator < (const SLoc& sloc) const { return (filename < sloc.filename) || ((filename == sloc.filename) && (lineno < sloc.lineno)); }
	int operator == (const SLoc& sloc) const { return (filename == sloc.filename) && (lineno == sloc.lineno); }
	
	void Print(std::ostream& os) const { os << "<" << filename << ":" << lineno << ">"; }
	
private:
	std::string filename;
	unsigned int lineno;
};

inline std::ostream& operator << (std::ostream& os, const SLoc& sloc)
{
	sloc.Print(os);
	return os;
}

////////////////////// FunctionNameLocationConversionBase /////////////////////////

template <typename ADDRESS>
class FunctionNameLocationConversionBase
{
public:
	virtual ~FunctionNameLocationConversionBase() {}
	virtual bool FunctionNameToAddress(const char *func_name, ADDRESS& addr) const = 0;
	virtual const char *AddressToFunctionName(ADDRESS addr, bool& is_entry_point) const = 0;
	virtual const SLoc *FunctionNameToSLoc(const char *func_name) const = 0;
	virtual const char *SLocToFunctionName(const SLoc& sloc, bool& is_entry_point) const = 0;
};

//////////////////////// FunctionNameLocationConversion<> /////////////////////////

template <typename ADDRESS>
class FunctionNameLocationConversion : public FunctionNameLocationConversionBase<ADDRESS>
{
public:
	FunctionNameLocationConversion(unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if);
	
	virtual bool FunctionNameToAddress(const char *func_name, ADDRESS& addr) const;
	virtual const char *AddressToFunctionName(ADDRESS addr, bool& is_entry_point) const;
	virtual const SLoc *FunctionNameToSLoc(const char *func_name) const;
	virtual const char *SLocToFunctionName(const SLoc& sloc, bool& is_entry_point) const;
	
private:
	std::map<std::string, ADDRESS> func_to_addr;
	std::map<ADDRESS, std::string> addr_to_func;
	std::map<std::string, SLoc> func_to_sloc;
	std::map<SLoc, std::string> sloc_to_func;
};

////////////////////////// AddressProfileBase<> ///////////////////////////////

template <typename ADDRESS>
class AddressProfileBase
{
public:
	virtual ~AddressProfileBase() {}
	virtual void Capture(ADDRESS addr, unsigned int length) = 0;
	virtual const char *GetSampledVariableName() const = 0;
	virtual unisim::kernel::service::VariableBase *GetSampledVariable() const = 0;
	virtual void Print(std::ostream& os, OutputFormat o_fmt = O_FMT_TEXT, const char *csv_delimiter = ",") const = 0;
	virtual InstructionProfileBase *CreateInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv) const = 0;
	virtual FunctionInstructionProfileBase *CreateFunctionInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv) const = 0;
	virtual SourceCodeProfileBase<ADDRESS> *CreateSourceCodeProfile() const = 0;
};

///////////////////////////// AddressProfile<> ////////////////////////////////

template <typename ADDRESS, typename T>
class AddressProfile : public AddressProfileBase<ADDRESS>, public unisim::util::debug::Profile<ADDRESS, T>
{
public:
	typedef unisim::util::debug::Profile<ADDRESS, T> Super;
	
	AddressProfile(unisim::kernel::service::Variable<T> *stat, unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if, unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if);
	
	virtual void Capture(ADDRESS addr, unsigned int length);
	virtual const char *GetSampledVariableName() const { return sample.GetSampledVariableName(); }
	virtual unisim::kernel::service::VariableBase *GetSampledVariable() const { return sample.GetSampledVariable(); }
	virtual void Print(std::ostream& os, OutputFormat o_fmt, const char *csv_delimiter) const;
	virtual InstructionProfileBase *CreateInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv) const;
	virtual FunctionInstructionProfileBase *CreateFunctionInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv) const;
	virtual SourceCodeProfileBase<ADDRESS> *CreateSourceCodeProfile() const;
	
	bool GetValue(ADDRESS addr, T& value) const;
	std::pair<T, T> GetValueRange() const;
	
private:
	unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if;
	unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if;
	unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if;
	Sample<T> sample;
};

////////////////////////// SourceCodeProfileBase<> ////////////////////////////

template <typename ADDRESS>
class SourceCodeProfileBase
{
public:
	virtual ~SourceCodeProfileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual void Print(std::ostream& os) const = 0;
	virtual void Update() = 0;
	virtual AnnotatedSourceCodeFileSetBase *CreateAnnotatedSourceCodeFileSet(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, const char *search_path) const = 0;
};

///////////////////////// FunctionInstructionProfileBase /////////////////////////////////

class FunctionInstructionProfileBase
{
public:
	virtual ~FunctionInstructionProfileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual void Print(std::ostream& os, OutputFormat o_fmt = O_FMT_TEXT, const char *csv_delimiter = ",") const = 0;
};

template <typename T>
class Quantizer
{
public:
	Quantizer(unsigned int _factor, const std::pair<T, T>& _value_range) : factor(_factor), value_range(_value_range) {}
	unsigned int Quantize(const T& value)
	{
		return ((value_range.first != value_range.second) && (value >= value_range.first) && (value <= value_range.second))
			? ceil(factor * ((double)(value - value_range.first) / (double)(value_range.second - value_range.first)))
			: 0;
	}
private:
	unsigned int factor;
	std::pair<T, T> value_range;
};

template <>
class Quantizer<sc_core::sc_time>
{
public:
	Quantizer(unsigned int _factor, const std::pair<sc_core::sc_time, sc_core::sc_time>& _value_range) : factor(_factor), value_range(_value_range) {}
	unsigned int Quantize(const sc_core::sc_time& value)
	{
		return ((value_range.first != value_range.second) && (value >= value_range.first) && (value <= value_range.second))
			? ceil(factor * ((value - value_range.first) / (value_range.second - value_range.first)))
			: 0;
	}
private:
	unsigned int factor;
	std::pair<sc_core::sc_time, sc_core::sc_time> value_range;
};

////////////////////// FunctionInstructionProfile<> ///////////////////////////

template <typename ADDRESS, typename T>
class FunctionInstructionProfile : public FunctionInstructionProfileBase
{
public:
	FunctionInstructionProfile(const AddressProfile<ADDRESS, T> *addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if);
	
	virtual const char *GetSampledVariableName() const { return addr_profile->GetSampledVariableName(); }
	virtual void Print(std::ostream& os, OutputFormat o_fmt, const char *csv_delimiter) const;
private:
	const AddressProfile<ADDRESS, T> *addr_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if;
	std::map<std::string, T> func_insn_profile;
	std::pair<T, T> value_range;
	
	void Init();
};

//////////////////////////// SourceCodeProfile<> //////////////////////////////

template <typename ADDRESS, typename T>
class SourceCodeProfile : public SourceCodeProfileBase<ADDRESS>
{
public:
	SourceCodeProfile(const AddressProfile<ADDRESS, T> *addr_profile, unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if);
	
	virtual const char *GetSampledVariableName() const { return addr_profile->GetSampledVariableName(); }
	virtual void Print(std::ostream& os) const;
	virtual void Update();
	virtual AnnotatedSourceCodeFileSetBase *CreateAnnotatedSourceCodeFileSet(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, const char *search_path) const;
	
	const std::set<std::string>& GetSourceFilenameSet() const { return source_filename_set; }
	const T& GetValue(const char *filename, unsigned int lineno) const;
	const std::pair<T, T>& GetValueRange() const { return value_range; }
private:

	bool FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *>& stmts, ADDRESS addr);
	
	const AddressProfile<ADDRESS, T> *addr_profile;
	unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if;
	unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if;
	std::map<ADDRESS, T> other_profile;
	std::map<SLoc, T> sloc_profile;
	std::set<std::string> source_filename_set;
	std::pair<T, T> value_range;
};

//////////////////////// AnnotatedSourceCodeFileBase //////////////////////////

class AnnotatedSourceCodeFileBase
{
public:
	virtual ~AnnotatedSourceCodeFileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual const char *GetFilename() const = 0;
	virtual const char *GetRealFilename() const = 0;
	virtual void Print(std::ostream& os, OutputFormat o_fmt = O_FMT_TEXT, const char *csv_delimiter = ",") const = 0;
};

///////////////////////// AnnotatedSourceCodeFile<> ///////////////////////////

template <typename ADDRESS, typename T>
class AnnotatedSourceCodeFile : public AnnotatedSourceCodeFileBase
{
public:
	AnnotatedSourceCodeFile(const char *_filename, const SourceCodeProfile<ADDRESS, T> *_source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, const char *search_path);
	
	virtual const char *GetSampledVariableName() const { return source_code_profile->GetSampledVariableName(); }
	virtual const char *GetFilename() const { return filename.c_str(); }
	virtual const char *GetRealFilename() const { return real_filename.c_str(); }
	virtual void Print(std::ostream& os, OutputFormat o_fmt = O_FMT_TEXT, const char *csv_delimiter = ",") const;
private:
	std::string filename;
	std::string real_filename;
	std::string search_path;
	const SourceCodeProfile<ADDRESS, T> *source_code_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	std::vector<std::string> content;
	std::pair<T, T> value_range;

	bool LocateFile(const char *filename, std::string& match_file_path);
	void Init();
};

/////////////////////// AnnotatedSourceCodeFileSetBase ////////////////////////

class AnnotatedSourceCodeFileSetBase
{
public:
	virtual ~AnnotatedSourceCodeFileSetBase() {}
	virtual const char *GetSampledVariableName() = 0;
	virtual void Print(std::ostream& os) const = 0;
	virtual void Output(const char *output_directory, OutputFormat o_fmt = O_FMT_TEXT, const char *csv_delimiter = ",") = 0;
};

/////////////////////// AnnotatedSourceCodeFileSet<> //////////////////////////

template <typename ADDRESS, typename T>
class AnnotatedSourceCodeFileSet : public AnnotatedSourceCodeFileSetBase
{
public:
	AnnotatedSourceCodeFileSet(const SourceCodeProfile<ADDRESS, T> *source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, const char *search_path);
	virtual ~AnnotatedSourceCodeFileSet();
	
	virtual const char *GetSampledVariableName() { return source_code_profile->GetSampledVariableName(); }
	virtual void Print(std::ostream& os) const;
	virtual void Output(const char *output_directory, OutputFormat o_fmt, const char *csv_delimiter = ",");
private:
	const SourceCodeProfile<ADDRESS, T> *source_code_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	std::string search_path;
	std::map<std::string, const AnnotatedSourceCodeFile<ADDRESS, T> *> annotated_source_code_files;
	
	void Clear();
	void Init();
};

//////////////////////////////// Profiler<> ///////////////////////////////////

template <typename ADDRESS>
class Profiler
	: public unisim::kernel::service::Service<unisim::service::interfaces::DebugYielding>
	, public unisim::kernel::service::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::DebugYieldingRequest>
	, public unisim::kernel::service::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::Disassembly<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::Memory<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::Registers>
	, public unisim::kernel::service::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::BackTrace<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::Profiling<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::DebugInfoLoading>
	, public unisim::kernel::service::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
	, public unisim::kernel::service::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >
{
public:
	// Exports to debugger
	unisim::kernel::service::ServiceExport<unisim::service::interfaces::DebugYielding>                debug_yielding_export;
	unisim::kernel::service::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_export;
	
	// Imports from debugger
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>         debug_yielding_request_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >  debug_event_trigger_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >        disasm_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >             memory_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::Registers>                    registers_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >  symbol_table_lookup_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::StatementLookup<ADDRESS> >    stmt_lookup_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::BackTrace<ADDRESS> >          backtrace_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::Profiling<ADDRESS> >          profiling_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugInfoLoading>             debug_info_loading_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >   data_object_lookup_import;
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >   subprogram_lookup_import;
	
	Profiler(const char *name, Object *parent = 0);
	virtual ~Profiler();

	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();
	
	// unisim::service::interfaces::DebugEventListener<ADDRESS>
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);

	virtual bool EndSetup();
	virtual void OnDisconnect();
	
	void Output();

private:

	template <typename T> bool TryProfile(unisim::kernel::service::VariableBase *var);
	
	unisim::kernel::logger::Logger logger;
	std::string search_path;
	std::string filename;
	std::string sampled_variables;
	std::string output_directory;
	OutputFormat output_format;
	std::string csv_delimiter;
	unisim::kernel::service::Parameter<std::string> param_search_path;
	unisim::kernel::service::Parameter<std::string> param_filename;
	unisim::kernel::service::Parameter<std::string> param_sampled_variables;
	unisim::kernel::service::Parameter<std::string> param_output_directory;
	unisim::kernel::service::Parameter<OutputFormat> param_output_format;
	unisim::kernel::service::Parameter<std::string> param_csv_delimiter;

	bool listening_commit;
	bool trap;
	unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event;
	unsigned int num_sampled_variables;
	FunctionNameLocationConversion<ADDRESS> *func_name_loc_conv;
	std::vector<AddressProfileBase<ADDRESS> *> addr_profiles; // address profile for each variable
	std::vector<FunctionInstructionProfileBase *> func_insn_profiles; // function profile for each variable
	ADDRESS pc;
	unsigned int length;
	std::vector<InstructionProfileBase *> instruction_profiles; // instruction profile for each variable
	std::vector<SourceCodeProfileBase<ADDRESS> *> source_code_profiles; // source code profile for each variable
	std::vector<AnnotatedSourceCodeFileSetBase *> annotated_source_code_file_sets; // annotated source code file set for each variable
	
	bool ListenCommit();
	bool UnlistenCommit();
	void LoadDebugInfo();
	void Update();
	void Print(std::ostream& os);
};

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
