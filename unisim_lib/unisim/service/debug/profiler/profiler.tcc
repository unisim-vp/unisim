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
 
#ifndef __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_TCC_
#define __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_TCC_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <set>
#include <stdexcept>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/debug/data_object_initializer.hh>
#include <unisim/util/debug/data_object_initializer.tcc>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/debug/profile.tcc>

#include <unisim/util/endian/endian.hh>
#include <fstream>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access() and mkdir()
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

//////////////////////////////// Sample ///////////////////////////////////////

template <typename T>
void Sample<T>::Capture()
{
	prev_value = curr_value;
	curr_value = var->Get();
	diff_value = curr_value - prev_value;
}

////////////////////// FunctionNameLocationConversion<> ///////////////////////

template <typename ADDRESS>
FunctionNameLocationConversion<ADDRESS>::FunctionNameLocationConversion(unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if)
	: func_to_addr()
	, addr_to_func()
	, func_to_sloc()
	, sloc_to_func()
{
	std::list<const unisim::util::debug::Symbol<ADDRESS> *> func_symbols;
	
	symbol_table_lookup_if->GetSymbols(func_symbols, unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC);
	
	typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>::const_iterator func_symbol_it;
	
	for(func_symbol_it = func_symbols.begin(); func_symbol_it != func_symbols.end(); func_symbol_it++)
	{
		const unisim::util::debug::Symbol<ADDRESS> *func_symbol = *func_symbol_it;
		
		const char *func_name = func_symbol->GetName();
		
		ADDRESS func_addr = func_symbol->GetAddress();
		ADDRESS func_size = func_symbol->GetSize();
		ADDRESS func_start_addr = func_addr;
		ADDRESS func_end_addr = func_addr + func_size - 1;

		typename std::map<std::string, ADDRESS>::iterator func_to_addr_it = func_to_addr.find(func_name);
		
		if(func_to_addr_it == func_to_addr.end())
		{
			func_to_addr[func_name] = func_start_addr;
		}
		else
		{
			std::cerr << "Multiple definition of function \"" << func_name << "\"" << std::endl;
		}
		
		ADDRESS addr;
		for(addr = func_start_addr; addr <= func_end_addr; addr++)
		{
			addr_to_func[addr] = func_name;
			
			//const unisim::util::debug::Statement<ADDRESS> *stmt = stmt_lookup_if->FindStatement(addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT);
			
			std::vector<const unisim::util::debug::Statement<ADDRESS> *> stmts;
			
			if(stmt_lookup_if->FindStatements(stmts, addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT))
			{
				typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::const_iterator stmt_it;
				
				for(stmt_it = stmts.begin(); stmt_it != stmts.end(); stmt_it++)
				{
					const unisim::util::debug::Statement<ADDRESS> *stmt = *stmt_it;
				
					const char *source_filename = stmt->GetSourceFilename();
					
					if(source_filename)
					{
						unsigned int lineno = stmt->GetLineNo();
					
						std::string source_path;
						const char *source_dirname = stmt->GetSourceDirname();
						if(source_dirname)
						{
							source_path += source_dirname;
							source_path += '/';
						}
						source_path += source_filename;
						
						SLoc sloc = SLoc(source_path.c_str(), lineno);
				
						sloc_to_func[sloc] = func_name;
						
						if(func_to_sloc.find(func_name) == func_to_sloc.end())
						{
							// temporary heuristic: associate first sloc (in address order) to function
							func_to_sloc[func_name] = sloc;
						}
					}
				}
			}
		}
	}
}

template <typename ADDRESS>
bool FunctionNameLocationConversion<ADDRESS>::FunctionNameToAddress(const char *func_name, ADDRESS& addr) const
{
	typename std::map<std::string, ADDRESS>::const_iterator func_to_addr_it = func_to_addr.find(func_name);
	
	if(func_to_addr_it != func_to_addr.end())
	{
		addr = (*func_to_addr_it).second;
		return true;
	}
	
	return false;
}

template <typename ADDRESS>
const char *FunctionNameLocationConversion<ADDRESS>::AddressToFunctionName(ADDRESS addr, bool& is_entry_point) const
{
	typename std::map<ADDRESS, std::string>::const_iterator addr_to_func_it = addr_to_func.find(addr);
	
	if(addr_to_func_it != addr_to_func.end())
	{
		const std::string& func_name = (*addr_to_func_it).second;
		
		typename std::map<std::string, ADDRESS>::const_iterator func_to_addr_it = func_to_addr.find(func_name);
		
		ADDRESS func_addr = (*func_to_addr_it).second;
		
		is_entry_point = (addr == func_addr);
		
		return func_name.c_str();
	}
	
	return 0;
}

template <typename ADDRESS>
const SLoc *FunctionNameLocationConversion<ADDRESS>::FunctionNameToSLoc(const char *func_name) const
{
	typename std::map<std::string, SLoc>::const_iterator func_to_sloc_it = func_to_sloc.find(func_name);
	
	if(func_to_sloc_it != func_to_sloc.end())
	{
		const SLoc *sloc = &(*func_to_sloc_it).second; // returning a pointer is dangerous (any insertion/deletion may invalidate pointer)
	
		return sloc;
	}
	
	return 0;
}

template <typename ADDRESS>
const char *FunctionNameLocationConversion<ADDRESS>::SLocToFunctionName(const SLoc& sloc, bool& is_entry_point) const
{
	typename std::map<SLoc, std::string>::const_iterator sloc_to_func_it = sloc_to_func.find(sloc);
	
	if(sloc_to_func_it != sloc_to_func.end())
	{
		const char *func_name = (*sloc_to_func_it).second.c_str(); // returning a pointer is dangerous (any insertion/deletion may invalidate pointer)
		
		typename std::map<std::string, SLoc>::const_iterator func_to_sloc_it = func_to_sloc.find(func_name);
		
		if(func_to_sloc_it != func_to_sloc.end())
		{
			const SLoc& entry_point_sloc = (*func_to_sloc_it).second;
		
			is_entry_point = (sloc == entry_point_sloc);
			return func_name; // returning a pointer is dangerous (any insertion/deletion may invalidate pointer)
		}
	}
	
	return 0;
}

///////////////////////////// AddressProfile<> ////////////////////////////////

template <typename ADDRESS, typename T>
AddressProfile<ADDRESS, T>::AddressProfile(unisim::kernel::service::Variable<T> *stat, unisim::service::interfaces::Disassembly<ADDRESS> *_disasm_if, unisim::service::interfaces::StatementLookup<ADDRESS> *_stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *_symbol_table_lookup_if)
	: Super()
	, disasm_if(_disasm_if)
	, stmt_lookup_if(_stmt_lookup_if)
	, symbol_table_lookup_if(_symbol_table_lookup_if)
	, sample(stat)
{
}

template <typename ADDRESS, typename T>
void AddressProfile<ADDRESS, T>::Capture(ADDRESS addr, unsigned int length)
{
	static T zero = T();
	
	sample.Capture();
	
	const T& value = sample.Get();
	
	if(value != zero)
	{
		Super::Cover(addr, length);
		Super::Accumulate(addr, sample.Get());
	}
}

template <typename ADDRESS, typename T>
void AddressProfile<ADDRESS, T>::Print(std::ostream& os, OutputFormat o_fmt, const char *csv_delimiter) const
{
	OStreamContext osc(os);
	
	switch(o_fmt)
	{
		case O_FMT_TEXT:
			os << (*(Super *) this) << std::endl;
			break;
			
		case O_FMT_CSV:
		{
			os << "Address" << csv_delimiter << "Value" << std::endl;
			
			std::map<ADDRESS, T> value_per_addr_map = *(Super *) this;
			typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
			for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
			{
				ADDRESS addr = (*value_per_addr_map_it).first;
				const T& value = (*value_per_addr_map_it).second;
				
				os << "0x" << std::hex << addr << csv_delimiter << std::dec << value << std::endl;
			}
			break;
		}
			
		case O_FMT_HTML:
		{
			std::map<ADDRESS, T> value_per_addr_map = *(Super *) this;
			
			bool is_first_value = true;
			
			std::pair<T, T> value_range;
			
			typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
			for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
			{
				const T& value = (*value_per_addr_map_it).second;
				
				if(is_first_value)
				{
					value_range.first = value;
					value_range.second = value;
					is_first_value = false;
				}
				else
				{
					if(value < value_range.first) value_range.first = value;
					if(value > value_range.second) value_range.second = value;
				}
			}
			
			Quantizer<T> quantizer = Quantizer<T>(16, value_range);
			
			os << "<!DOCTYPE HTML>" << std::endl;
			os << "<html lang=\"en\">" << std::endl;
			os << "<head>" << std::endl;
			os << "<meta name=\"description\" content=\"Instruction profile report - By address [" << c_string_to_HTML(GetSampledVariableName()) << "]\">" << std::endl;
			os << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << "<title>Instruction profile report - By address [" << c_string_to_HTML(GetSampledVariableName()) << "]</title>" << std::endl;
			os << "<style type=\"text/css\">" << std::endl;
			os << "<!--" << std::endl;
			os << "body{ font-family:Arial, Helvetica, sans-serif;font-style:normal;font-size:14px;text-align:left;font-weight:400;color:#000;margin-left:auto;margin-right:auto;background:#FFF; }" << std::endl;
			os << "table{ width:512px;margin-left:auto; margin-right:auto; }" << std::endl;
			os << "table th { text-align:center; }" << std::endl;
			os << "table th { font-weight:bold; }" << std::endl;
			os << "table td { text-align:left; }" << std::endl;
			os << "table { border-style:solid; }" << std::endl;
			os << "table { border-width:1px; }" << std::endl;
			os << "table { border-collapse: collapse; }" << std::endl;
			os << "th, td { margin:0; }" << std::endl;
			os << "th, td { border-style:solid; }" << std::endl;
			os << "th, td { border-width:1px; }" << std::endl;
			os << "th, td { border-width:1px; }" << std::endl;
			os << ".scale0 { background-color:#ffffff; }" << std::endl;
			os << ".scale1 { background-color:#fff0f0; }" << std::endl;
			os << ".scale2 { background-color:#ffe0e0; }" << std::endl;
			os << ".scale3 { background-color:#ffd0d0; }" << std::endl;
			os << ".scale4 { background-color:#ffc0c0; }" << std::endl;
			os << ".scale5 { background-color:#ffb0b0; }" << std::endl;
			os << ".scale6 { background-color:#ffa0a0; }" << std::endl;
			os << ".scale7 { background-color:#ff9090; }" << std::endl;
			os << ".scale8 { background-color:#ff8080; }" << std::endl;
			os << ".scale9 { background-color:#ff7070; }" << std::endl;
			os << ".scale10 { background-color:#ff6060; }" << std::endl;
			os << ".scale11 { background-color:#ff5050; }" << std::endl;
			os << ".scale12 { background-color:#ff4040; }" << std::endl;
			os << ".scale13 { background-color:#ff3030; }" << std::endl;
			os << ".scale14 { background-color:#ff2020; }" << std::endl;
			os << ".scale15 { background-color:#ff1010; }" << std::endl;
			os << ".scale16 { background-color:#ff0000; }" << std::endl;
			os << "a:link { text-decoration:none }" << std::endl;
			os << "#title { text-align:center; } " << std::endl;
			os << "#content { width:512px; margin-left:auto; margin-right:auto; }" << std::endl;
			os << "-->" << std::endl;
			os << "</style>" << std::endl;
			os << "</head>" << std::endl;
			os << "<body>" << std::endl;
			os << "<div id=\"content\">" << std::endl;
			os << "<h1 id=\"title\"><a href=\"../index.html\">&#8624;</a>&nbsp;Instruction profile report - By address<br>[" << c_string_to_HTML(GetSampledVariableName()) << "]</h1>" << std::endl;
			os << "<table>" << std::endl;
			os << "<tr>" << std::endl;
			os << "<th>Program Address</th>" << std::endl;
			os << "<th>" << c_string_to_HTML(GetSampledVariableName()) << "</th>" << std::endl;
			os << "</tr>" << std::endl;
			
			for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
			{
				ADDRESS addr = (*value_per_addr_map_it).first;
				const T& value = (*value_per_addr_map_it).second;
				
				if(value >= value_range.first)
				{
					unsigned int scale = quantizer.Quantize(value);
					//std::cerr << value << " -> " << scale << std::endl;
					
					os << "<tr class=\"scale" << scale << "\">";
				}
				else
				{
					os << "<tr>";
				}
				
				os << "<td>";
				os << "0x";

				os.fill('0');
				os.width(2 * sizeof(ADDRESS));
				os << std::hex << addr;
				os << "</td>";
				
				std::stringstream sstr;
				sstr << value;
				
				os << "<td>" << c_string_to_HTML(sstr.str().c_str()) << "</td></tr>" << std::endl;
				
			}
			
			os << "</table>" << std::endl;
			os << "</div>" << std::endl;
			os << "</body>" << std::endl;

			break;
		}
	}
}

template <typename ADDRESS, typename T>
InstructionProfileBase *AddressProfile<ADDRESS, T>::CreateInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv) const
{
	return new InstructionProfile<ADDRESS, T>(this, func_name_loc_conv, disasm_if);
}

template <typename ADDRESS, typename T>
FunctionInstructionProfileBase *AddressProfile<ADDRESS, T>::CreateFunctionInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv) const
{
	return new FunctionInstructionProfile<ADDRESS, T>(this, func_name_loc_conv, symbol_table_lookup_if);
}

template <typename ADDRESS, typename T>
SourceCodeProfileBase<ADDRESS> *AddressProfile<ADDRESS, T>::CreateSourceCodeProfile() const
{
	return new SourceCodeProfile<ADDRESS, T>(this, stmt_lookup_if, symbol_table_lookup_if);
}

template <typename ADDRESS, typename T>
bool AddressProfile<ADDRESS, T>::GetValue(ADDRESS addr, T& value) const
{
	return this->GetWeight(addr, value);
}

template <typename ADDRESS, typename T>
std::pair<T, T> AddressProfile<ADDRESS, T>::GetValueRange() const
{
	std::pair<T, T> value_range;
	std::map<ADDRESS, T> value_per_addr_map = *(Super *) this;

	typename std::map<ADDRESS, T>::const_iterator value_per_addr_map_it = value_per_addr_map.begin();
	bool is_first_value = true;

	for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
	{
		const T& value = (*value_per_addr_map_it).second;
		
		if(is_first_value)
		{
			value_range.first = value;
			value_range.second = value;
			is_first_value = false;
		}
		else
		{
			if(value < value_range.first) value_range.first = value;
			if(value > value_range.second) value_range.second = value;
		}
	}
	
	return value_range;
}

//////////////////////////// InstructionProfile<> /////////////////////////////

template <typename ADDRESS, typename T>
InstructionProfile<ADDRESS, T>::InstructionProfile(const AddressProfile<ADDRESS, T> *_addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, unisim::service::interfaces::Disassembly<ADDRESS> *_disasm_if)
	: addr_profile(_addr_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, disasm_if(_disasm_if)
{
}

template <typename ADDRESS, typename T>
void InstructionProfile<ADDRESS, T>::Print(std::ostream& os, OutputFormat o_fmt, const char *csv_delimiter) const
{
	OStreamContext osc(os);

	std::vector<std::pair<ADDRESS, ADDRESS> > addr_ranges;
	std::pair<T, T> value_range = addr_profile->GetValueRange();
	addr_profile->GetAddressRanges(addr_ranges);
	
	ADDRESS addr;
	ADDRESS next_addr;
	
	switch(o_fmt)
	{
		case O_FMT_TEXT:
		{
			typename std::vector<std::pair<ADDRESS, ADDRESS> >::const_iterator addr_range_it;
			
			for(addr_range_it = addr_ranges.begin(); addr_range_it < addr_ranges.end(); addr_range_it++)
			{
				os << "..." << std::endl;
				
				const std::pair<ADDRESS, ADDRESS>& addr_range = *addr_range_it;
				
				for(addr = addr_range.first; addr <= addr_range.second; addr = next_addr)
				{
					T value = T();
					
					if(!addr_profile->GetValue(addr, value)) throw std::runtime_error("Internal Error! can't get value");
					
					os.fill(' ');
					os.flags(std::ios_base::right);
					
					os << "0x";
					os.width(2 * sizeof(ADDRESS));
					os << std::hex << addr << "  ";
					
					os.width(21);
					
					if(value != T())
					{
						os << std::dec << value;
					}
					else
					{
						os << ' ';
					}
					
					os << "  " << disasm_if->Disasm(addr, next_addr) << std::endl;
				}
				os << "..." << std::endl;
			}
			break;
		}
		
		case O_FMT_CSV:
		{
			typename std::vector<std::pair<ADDRESS, ADDRESS> >::const_iterator addr_range_it;
			
			for(addr_range_it = addr_ranges.begin(); addr_range_it < addr_ranges.end(); addr_range_it++)
			{
				const std::pair<ADDRESS, ADDRESS>& addr_range = *addr_range_it;
				
				for(addr = addr_range.first; addr <= addr_range.second; addr = next_addr)
				{
					T value = T();
					
					if(!addr_profile->GetValue(addr, value)) throw std::runtime_error("Internal Error! can't get value");
					
					os << "0x";
					os.width(2 * sizeof(ADDRESS));
					os << std::hex << addr << std::dec << "  " << csv_delimiter;

					if(value != T())
					{
						os << value;
					}
					
					os << csv_delimiter << c_string_to_CSV(disasm_if->Disasm(addr, next_addr).c_str()) << std::endl;
				}
			}
			break;
		}
		
		case O_FMT_HTML:
		{
			Quantizer<T> quantizer = Quantizer<T>(16, value_range);
			//std::cerr << "\"" << filename << "\": value_range=[" << value_range.first << "," << value_range.second << "]" << std::endl;
			
			os << "<!DOCTYPE HTML>" << std::endl;
			os << "<html lang=\"en\">" << std::endl;
			os << "<head>" << std::endl;
			os << "<meta name=\"description\" content=\"Instruction profile report - Annotated disassembly - [" << c_string_to_HTML(GetSampledVariableName()) << "]\">" << std::endl;
			os << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << "<title>Instruction profile report - Annotated disassembly - [" << c_string_to_HTML(GetSampledVariableName()) << "]</title>" << std::endl;
			os << "<style type=\"text/css\">" << std::endl;
			os << "<!--" << std::endl;
			os << "body{ font-family:Arial, Helvetica, sans-serif;font-style:normal;font-size:14px;text-align:left;font-weight:400;color:#000;margin-left:auto;margin-right:auto;background:#FFF; }" << std::endl;
			os << "table{ width:512px;margin-left:auto; margin-right:auto; }" << std::endl;
			os << "table th { text-align:center; }" << std::endl;
			os << "table th { font-weight:bold; }" << std::endl;
			os << "table td { text-align:left; }" << std::endl;
			os << "table { border-style:solid; }" << std::endl;
			os << "table { border-width:0px; }" << std::endl;
			os << "table { border-collapse: collapse; }" << std::endl;
			os << "th, td { padding-left: 1em; }" << std::endl;
			os << "th, td { margin:0; }" << std::endl;
			os << "th, td { border-style:solid; }" << std::endl;
			os << "th, td { border-width:0px; }" << std::endl;
			os << "th, td { border-width:0px; }" << std::endl;
			os << ".scale0 { background-color:#ffffff; }" << std::endl;
			os << ".scale1 { background-color:#fff0f0; }" << std::endl;
			os << ".scale2 { background-color:#ffe0e0; }" << std::endl;
			os << ".scale3 { background-color:#ffd0d0; }" << std::endl;
			os << ".scale4 { background-color:#ffc0c0; }" << std::endl;
			os << ".scale5 { background-color:#ffb0b0; }" << std::endl;
			os << ".scale6 { background-color:#ffa0a0; }" << std::endl;
			os << ".scale7 { background-color:#ff9090; }" << std::endl;
			os << ".scale8 { background-color:#ff8080; }" << std::endl;
			os << ".scale9 { background-color:#ff7070; }" << std::endl;
			os << ".scale10 { background-color:#ff6060; }" << std::endl;
			os << ".scale11 { background-color:#ff5050; }" << std::endl;
			os << ".scale12 { background-color:#ff4040; }" << std::endl;
			os << ".scale13 { background-color:#ff3030; }" << std::endl;
			os << ".scale14 { background-color:#ff2020; }" << std::endl;
			os << ".scale15 { background-color:#ff1010; }" << std::endl;
			os << ".scale16 { background-color:#ff0000; }" << std::endl;
			os << "a:link { text-decoration:none }" << std::endl;
			os << "#title { text-align:center; } " << std::endl;
			os << "#content { width:512px; margin-left:auto; margin-right:auto; }" << std::endl;
			os << ".source-code { font-family:\"Courier New\",Courier,monospace; }" << std::endl;
			os << "-->" << std::endl;
			os << "</style>" << std::endl;
			os << "</head>" << std::endl;
			os << "<body>" << std::endl;
			os << "<div id=\"content\">" << std::endl;
			os << "<h1 id=\"title\"><a href=\"by_function.html\">&#8624;</a>&nbsp;Instruction profile report - Annotated disassembly<br>[" << c_string_to_HTML(GetSampledVariableName()) << "]</h1>" << std::endl;
			
			os << "<table>" << std::endl;
			
			typename std::vector<std::pair<ADDRESS, ADDRESS> >::const_iterator addr_range_it;
			
			for(addr_range_it = addr_ranges.begin(); addr_range_it < addr_ranges.end(); addr_range_it++)
			{
				os << "<tr><td colspan=\"3\">...</td></tr>" << std::endl;
				
				const std::pair<ADDRESS, ADDRESS>& addr_range = *addr_range_it;
				
				bool first_addr_in_addr_range = true;
				
				for(addr = addr_range.first; addr <= addr_range.second; addr = next_addr, first_addr_in_addr_range = false)
				{
					T value = T();
					
					if(!addr_profile->GetValue(addr, value)) throw std::runtime_error("Internal Error! can't get value");
					
					std::stringstream sstr;
					sstr << value;
					
					bool is_entry_point = false;
					const char *func_name = func_name_loc_conv->AddressToFunctionName(addr, is_entry_point);
					
					if(func_name && is_entry_point)
					{
						if(!first_addr_in_addr_range)
						{
							os << "<tr><td colspan=\"3\">&nbsp;</td></tr>" << std::endl;
						}
						os << "<tr><td>0x" << std::hex << addr << std::dec << "</td><td colspan=\"2\">&lt;" << c_string_to_HTML(func_name) << "&gt;:</td></tr>" << std::endl;
					}

					if(value >= value_range.first)
					{
						unsigned int scale = quantizer.Quantize(value);
						
						os << "<tr class=\"scale" << scale << "\"";
					}
					else
					{
						os << "<tr";
					}
					
					if(func_name && is_entry_point)
					{
						os << " id=\"" << c_string_to_URL(func_name) <<"\"";
					}
					
					os << ">";

					if(func_name)
					{
						os << "<!--in " << c_string_to_HTML(func_name) << " -->";
					}
					
					os << "<td>0x" << std::hex << addr << std::dec << "</td><td>" << ((value != T()) ? c_string_to_HTML(sstr.str().c_str()) : "") << "</td><td class=\"source-code\">" << c_string_to_HTML(disasm_if->Disasm(addr, next_addr).c_str()) << "</td></tr>" << std::endl;
				}
				
				os << "<tr><td colspan=\"3\">...</td></tr>" << std::endl;
			}
			os << "</table>" << std::endl;
			
			os << "</div>" << std::endl;
			os << "</body>" << std::endl;
			break;
		}
	}
}

/////////////////////// FunctionInstructionProfile<> //////////////////////////

template <typename ADDRESS, typename T>
FunctionInstructionProfile<ADDRESS, T>::FunctionInstructionProfile(const AddressProfile<ADDRESS, T> *_addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *_symbol_table_lookup_if)
	: addr_profile(_addr_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, symbol_table_lookup_if(_symbol_table_lookup_if)
	, func_insn_profile()
	, value_range()
{
	Init();
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::Print(std::ostream& os, OutputFormat o_fmt, const char *csv_delimiter) const
{
	switch(o_fmt)
	{
		case O_FMT_TEXT:
		{
			typename std::map<std::string, T>::const_iterator func_prof_it;
			
			for(func_prof_it = func_insn_profile.begin(); func_prof_it != func_insn_profile.end(); func_prof_it++)
			{
				const std::string func_name = (*func_prof_it).first;
				const T& value = (*func_prof_it).second;
				
				os << func_name << ":" << std::dec << value << std::endl;
			}
			break;
		}
		
		case O_FMT_CSV:
		{
			typename std::map<std::string, T>::const_iterator func_prof_it;
			
			for(func_prof_it = func_insn_profile.begin(); func_prof_it != func_insn_profile.end(); func_prof_it++)
			{
				const std::string func_name = (*func_prof_it).first;
				const T& value = (*func_prof_it).second;
				
				os << c_string_to_CSV(func_name.c_str()) << csv_delimiter << std::dec << value << std::endl;
			}
			break;
		}
		
		case O_FMT_HTML:
		{
			std::stringstream html;
			std::stringstream svg;
			
			//std::cerr << "FunctionInstructionProfile: value_range=[" << value_range.first << "," << value_range.second << "]" << std::endl;
			Quantizer<T> quant16 = Quantizer<T>(16, value_range);
			
			html << "<table>" << std::endl;
			html << "<tr>" << std::endl;
			html << "<th>Function</th>" << std::endl;
			html << "<th>" << c_string_to_HTML(GetSampledVariableName()) << "</th>" << std::endl;
			html << "<th>Annotated<br>disassembly</th>" << std::endl;
			html << "<th>Annotated<br>source code</th>" << std::endl;
			html << "</tr>" << std::endl;
			
			unsigned int page_width = 1280;
			
			unsigned int svg_margin = 12;
			
			unsigned int svg_bar_width = 16;
			unsigned int svg_width = (2 * svg_margin) + (((svg_bar_width * 125) / 100) * func_insn_profile.size());
			unsigned int svg_height = 600;
			
			unsigned int svg_histogram_height = (svg_height - (2 * svg_margin)) / 2;
			unsigned int svg_text_height = (svg_height - (2 * svg_margin)) / 2;
			unsigned int axis_y = svg_histogram_height;
			
			Quantizer<T> histogram_quant = Quantizer<T>(svg_histogram_height, value_range);
			
			svg << "<svg width=\"" << svg_width << "\" height=\"" << svg_height << "\">" << std::endl;
			svg << "<line x1=\"0\" y1=\"" << axis_y << "\" x2=\"" << svg_width << "\" y2=\"" << axis_y << "\" style=\"stroke:rgb(0,0,0);stroke-width:1\"/>" << std::endl;
			
			unsigned int histogram_column = 0;
			typename std::map<std::string, T>::const_iterator func_prof_it;
			
			for(func_prof_it = func_insn_profile.begin(); func_prof_it != func_insn_profile.end(); func_prof_it++)
			{
				const std::string func_name = (*func_prof_it).first;
				const T& value = (*func_prof_it).second;
				
				const SLoc *sloc = func_name_loc_conv->FunctionNameToSLoc(func_name.c_str());

				if(value >= value_range.first)
				{
					unsigned int scale16 = quant16.Quantize(value);
					//std::cerr << value << " -> " << scale << std::endl;
					
					html << "<tr class=\"scale" << scale16 << "\">";
					
					if(sloc)
					{
						svg << "<a href=\"" << c_string_to_URL(sloc->GetFilename()) << ".html#" << c_string_to_URL(func_name.c_str()) << "\">" << std::endl;
					}
					
					unsigned int svg_bar_height = histogram_quant.Quantize(value);
					unsigned int svg_bar_x = svg_margin + (((svg_bar_width * 125) / 100) * histogram_column);
					unsigned int svg_bar_y = (svg_histogram_height - svg_bar_height);
//					svg << "<rect x=\"" << svg_bar_x << "\" y=\"" << svg_bar_y << "\" width=\"" << svg_bar_width << "\" height=\"" << svg_bar_height << "\" style=\"fill:#ff0000;fill-opacity:1;stroke:#e6e6ff;stroke-width:1.36399996;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\"/>" << std::endl;
					svg << "<rect x=\"" << svg_bar_x << "\" y=\"" << svg_bar_y << "\" width=\"" << svg_bar_width << "\" height=\"" << svg_bar_height << "\" style=\"fill:#ff0000\"/>" << std::endl;
					unsigned int svg_text_x = svg_margin + (((svg_bar_width * 125) / 100) * histogram_column) - 3;
					unsigned int svg_text_y = axis_y; //svg_height - svg_margin - svg_text_height;
					
//					svg << "<text text-anchor=\"end\" alignment-baseline=\"end\" transform=\"rotate(-90," << (svg_text_x + svg_bar_width) << "," << svg_text_y << ")\" x=\"" << svg_text_x << "\" y=\"" << svg_text_y << "\" with=\"" << svg_bar_width << "\" height=\"" << svg_text_height << "\" style=\"font-style:normal;font-weight:normal;font-size:16px;line-height:125%;font-family:monospace;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\">" << c_string_to_HTML(func_name.c_str()) << "</text>" << std::endl;
					svg << "<text text-anchor=\"end\" alignment-baseline=\"end\" transform=\"rotate(-90," << (svg_text_x + svg_bar_width) << "," << svg_text_y << ")\" x=\"" << svg_text_x << "\" y=\"" << svg_text_y << "\" with=\"" << svg_bar_width << "\" height=\"" << svg_text_height << "\" style=\"fill:#000000\">" << c_string_to_HTML(func_name.c_str()) << "</text>" << std::endl;
					
					if(sloc)
					{
						svg << "</a>" << std::endl;
					}
					
					histogram_column++;
				}
				else
				{
					html << "<tr>";
				}
				
				// Function
				html << "<td>" << c_string_to_HTML(func_name.c_str()) << "</td>";
			
				// Value
				std::stringstream value_sstr;
				value_sstr << value;

				html << "<td>" << c_string_to_HTML(value_sstr.str().c_str()) << "</td>";

				// Annotated disassembly
				std::stringstream func_addr_sstr;
				ADDRESS func_addr = 0;
				if(func_name_loc_conv->FunctionNameToAddress(func_name.c_str(), func_addr))
				{
					func_addr_sstr << "@0x" << std::hex << func_addr;
				}
				else
				{
					func_addr_sstr << "?";
				}
				
				html << "<td><a href=\"disassembly.html#" << c_string_to_URL(func_name.c_str()) << "\">" << c_string_to_HTML(func_addr_sstr.str().c_str()) << "</td>";
				
				// Annotated souce code
				html << "<td>";
				
				if(sloc)
				{
					html << "<a href=\"" << c_string_to_URL(sloc->GetFilename()) << ".html#" << c_string_to_URL(func_name.c_str()) << "\">" << c_string_to_HTML(sloc->GetFilename()) << "</a>";
				}
				else
				{
					html << c_string_to_HTML(func_name.c_str());
				}
				html << "</td>";

				html << "</tr>" << std::endl;
			}
			
			html << "</table>" << std::endl;
			svg << "</svg>" << std::endl;
			
			os << "<!DOCTYPE HTML>" << std::endl;
			os << "<html lang=\"en\">" << std::endl;
			os << "<head>" << std::endl;
			os << "<meta name=\"description\" content=\"Instruction profile report - By function [" << c_string_to_HTML(GetSampledVariableName()) << "]\">" << std::endl;
			os << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << "<title>Instruction profile report - By function [" << c_string_to_HTML(GetSampledVariableName()) << "]</title>" << std::endl;
			os << "<style type=\"text/css\">" << std::endl;
			os << "<!--" << std::endl;
			os << "body{ font-family:Arial, Helvetica, sans-serif;font-style:normal;font-size:14px;text-align:left;font-weight:400;color:#000;margin-left:auto;margin-right:auto;background:#FFF; }" << std::endl;
			os << "table{ width:" << page_width << "px; margin-left:auto; margin-right:auto; }" << std::endl;
			os << "table th { text-align:center; }" << std::endl;
			os << "table th { font-weight:bold; }" << std::endl;
			os << "table td { text-align:left; }" << std::endl;
			os << "table { border-style:solid; }" << std::endl;
			os << "table { border-width:1px; }" << std::endl;
			os << "table { border-collapse: collapse; }" << std::endl;
			os << "th, td { margin:0; }" << std::endl;
			os << "th, td { border-style:solid; }" << std::endl;
			os << "th, td { border-width:1px; }" << std::endl;
			os << "th, td { border-width:1px; }" << std::endl;
			os << ".scale0 { background-color:#ffffff; }" << std::endl;
			os << ".scale1 { background-color:#fff0f0; }" << std::endl;
			os << ".scale2 { background-color:#ffe0e0; }" << std::endl;
			os << ".scale3 { background-color:#ffd0d0; }" << std::endl;
			os << ".scale4 { background-color:#ffc0c0; }" << std::endl;
			os << ".scale5 { background-color:#ffb0b0; }" << std::endl;
			os << ".scale6 { background-color:#ffa0a0; }" << std::endl;
			os << ".scale7 { background-color:#ff9090; }" << std::endl;
			os << ".scale8 { background-color:#ff8080; }" << std::endl;
			os << ".scale9 { background-color:#ff7070; }" << std::endl;
			os << ".scale10 { background-color:#ff6060; }" << std::endl;
			os << ".scale11 { background-color:#ff5050; }" << std::endl;
			os << ".scale12 { background-color:#ff4040; }" << std::endl;
			os << ".scale13 { background-color:#ff3030; }" << std::endl;
			os << ".scale14 { background-color:#ff2020; }" << std::endl;
			os << ".scale15 { background-color:#ff1010; }" << std::endl;
			os << ".scale16 { background-color:#ff0000; }" << std::endl;
			os << "a:link { text-decoration:none }" << std::endl;
			os << "#title { text-align:center; } " << std::endl;
			os << "#content { width:" << page_width << "px; margin-left:auto; margin-right:auto; }" << std::endl;
			os << "#histogram { font-size:14px; width:" << page_width << "px; height:" << (svg_height + 32) << "px; overflow-x:auto; margin-bottom:1em; }" << std::endl;
			os << "-->" << std::endl;
			os << "</style>" << std::endl;
			os << "</head>" << std::endl;
			os << "<body>" << std::endl;
			os << "<div id=\"content\">" << std::endl;
			os << "<h1 id=\"title\"><a href=\"../index.html\">&#8624;</a>&nbsp;Instruction profile report - By function<br>[" << c_string_to_HTML(GetSampledVariableName()) << "]</h1>" << std::endl;
			os << "<div id=\"histogram\">" << std::endl;
			os << svg.str();
			os << "</div>" << std::endl;
			os << html.str();
			os << "</div>" << std::endl;
			os << "</body>" << std::endl;
			break;
		}
	}
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::Init()
{
	std::map<ADDRESS, T> value_per_addr_map = *addr_profile;
	
	typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
	
	for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
	{
		ADDRESS addr = (*value_per_addr_map_it).first;
		const T& value = (*value_per_addr_map_it).second;
		
		const unisim::util::debug::Symbol<ADDRESS> *func_symbol = symbol_table_lookup_if->FindSymbolByAddr(addr, unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC);
		
		if(func_symbol)
		{
			func_insn_profile[func_symbol->GetName()] += value;
		}
	}
	
	bool is_first_value = true;
	
	typename std::map<std::string, T>::const_iterator func_prof_it;
	
	for(func_prof_it = func_insn_profile.begin(); func_prof_it != func_insn_profile.end(); func_prof_it++)
	{
		const T& value = (*func_prof_it).second;
		
		if(is_first_value)
		{
			value_range.first = value;
			value_range.second = value;
			is_first_value = false;
		}
		else
		{
			if(value < value_range.first) value_range.first = value;
			if(value > value_range.second) value_range.second = value;
		}
	}
}

//////////////////////////// SourceCodeProfile<> //////////////////////////////

template <typename ADDRESS, typename T>
SourceCodeProfile<ADDRESS, T>::SourceCodeProfile(const AddressProfile<ADDRESS, T> *_addr_profile, unisim::service::interfaces::StatementLookup<ADDRESS> *_stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *_symbol_table_lookup_if)
	: addr_profile(_addr_profile)
	, stmt_lookup_if(_stmt_lookup_if)
	, symbol_table_lookup_if(_symbol_table_lookup_if)
	, other_profile()
	, sloc_profile()
	, source_filename_set()
	, value_range()
{
}

template <typename ADDRESS, typename T>
void SourceCodeProfile<ADDRESS, T>::Print(std::ostream& os) const
{
	OStreamContext osc(os);

	os << "-------- slocs ---------" << std::endl;
	
	typename std::map<SLoc, T>::const_iterator sloc_prof_it;
	
	for(sloc_prof_it = sloc_profile.begin(); sloc_prof_it != sloc_profile.end(); sloc_prof_it++)
	{
		const SLoc& sloc = (*sloc_prof_it).first;
		const T& value = (*sloc_prof_it).second;
		
		sloc.Print(os);
		os << ":" << value << std::endl;
	}
	os << "-------- other ---------" << std::endl;
	
	typename std::map<ADDRESS, T>::const_iterator other_prof_it;
	
	for(other_prof_it = other_profile.begin(); other_prof_it != other_profile.end(); other_prof_it++)
	{
		const ADDRESS& addr = (*other_prof_it).first;
		const T& value = (*other_prof_it).second;
		
		os << "0x" << std::hex << addr << std::dec << ":" << value << std::endl;
	}
	os << "-----------------------------" << std::endl;
}

template <typename ADDRESS, typename T>
bool SourceCodeProfile<ADDRESS, T>::FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *>& stmts, ADDRESS addr)
{
	const unisim::util::debug::Statement<ADDRESS> *stmt = stmt_lookup_if->FindStatements(stmts, addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT);
	
	if(stmt) return stmt;
	
	const unisim::util::debug::Symbol<ADDRESS> *func_symbol = symbol_table_lookup_if->FindSymbolByAddr(addr, unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC);
	
	ADDRESS func_start_addr = addr;
	ADDRESS func_end_addr = addr;
	if(func_symbol)
	{
		ADDRESS func_addr = func_symbol->GetAddress();
		ADDRESS func_size = func_symbol->GetSize();
		if(func_size != 0)
		{
			func_start_addr = func_addr;
			func_end_addr = func_addr + func_size - 1;
		}
	}

	// could not find exact statemement, try to find nearest statements
	stmts.clear();
	stmt = stmt_lookup_if->FindStatements(stmts, addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT);
	
	// filter statements outside from the function scope
	if(stmt && (stmt->GetAddress() >= func_start_addr) && (stmt->GetAddress() <= func_end_addr))
	{
		typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::iterator stmt_it = stmts.begin();
		
		do
		{
			stmt = *stmt_it;
			if((stmt->GetAddress() < func_start_addr) || (stmt->GetAddress() > func_end_addr))
			{
				stmt_it = stmts.erase(stmt_it);
			}
			else
			{
				stmt_it++;
			}
		}
		while(stmt_it != stmts.end());
		
		return !stmts.empty();
	}
	
	return false;
}

template <typename ADDRESS, typename T>
void SourceCodeProfile<ADDRESS, T>::Update()
{
	sloc_profile.clear();
	other_profile.clear();
	
	std::map<ADDRESS, T> value_per_addr_map = *addr_profile;
	
	typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
	
	for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
	{
		ADDRESS addr = (*value_per_addr_map_it).first;
		const T& value = (*value_per_addr_map_it).second;
		
		std::vector<const unisim::util::debug::Statement<ADDRESS> *> stmts;
		if(FindStatements(stmts, addr))
		{
			typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::iterator stmt_it;
			for(stmt_it = stmts.begin(); stmt_it != stmts.end(); stmt_it++)
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = *stmt_it;
		
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					unsigned int lineno = stmt->GetLineNo();
					//unsigned int colno = stmt->GetColNo();
					std::string source_path;
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
					{
						source_path += source_dirname;
						source_path += '/';
					}
					source_path += source_filename;
					
					SLoc sloc = SLoc(source_path.c_str(), lineno);
					
					sloc_profile[sloc] += value;
					
					source_filename_set.insert(source_path);
				}
			}
		}
		else
		{
			other_profile[addr] += value;
		}
	}
	
	bool is_first_value = true;

	typename std::map<SLoc, T>::const_iterator sloc_prof_it;
	
	for(sloc_prof_it = sloc_profile.begin(); sloc_prof_it != sloc_profile.end(); sloc_prof_it++)
	{
		const T& value = (*sloc_prof_it).second;
		
		if(is_first_value)
		{
			value_range.first = value;
			value_range.second = value;
			is_first_value = false;
		}
		else
		{
			if(value < value_range.first) value_range.first = value;
			if(value > value_range.second) value_range.second = value;
		}
	}
}

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFileSetBase *SourceCodeProfile<ADDRESS, T>::CreateAnnotatedSourceCodeFileSet(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, const char *search_path) const
{
	return new AnnotatedSourceCodeFileSet<ADDRESS, T>(this, func_name_loc_conv, search_path);
}

template <typename ADDRESS, typename T>
const T& SourceCodeProfile<ADDRESS, T>::GetValue(const char *filename, unsigned int lineno) const
{
	SLoc sloc = SLoc(filename, lineno);
	
	typename std::map<SLoc, T>::const_iterator sloc_prof_it = sloc_profile.find(sloc);
	
	if(sloc_prof_it != sloc_profile.end())
	{
		const T& value = (*sloc_prof_it).second;
		return value;
	}
	
	static T zero = T();
	
	return zero;
}


///////////////////////// AnnotatedSourceCodeFile<> ///////////////////////////

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFile<ADDRESS, T>::AnnotatedSourceCodeFile(const char *_filename, const SourceCodeProfile<ADDRESS, T> *_source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, const char *_search_path)
	: filename(_filename)
	, real_filename()
	, search_path(_search_path)
	, source_code_profile(_source_code_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, content()
	, value_range()
{
	Init();
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFile<ADDRESS, T>::Print(std::ostream& os, OutputFormat o_fmt, const char *csv_delimiter) const
{
	OStreamContext osc(os);

	switch(o_fmt)
	{
		case O_FMT_TEXT:
		{
			unsigned int num_lines = content.size();
			unsigned int i;
			for(i = 0; i < num_lines; i++)
			{
				unsigned int lineno = i + 1;
				
				const T& value = source_code_profile->GetValue(filename.c_str(), lineno);
				
				os.fill(' ');
				os.flags(std::ios_base::right);
				
				os.width(6);
				os << std::dec << lineno << "  ";
				
				os.width(21);
				
				if(value != T())
				{
					os << std::dec << value;
				}
				else
				{
					os << ' ';
				}
				
				os << "  " << content[i] << std::endl;
			}
			break;
		}
		
		case O_FMT_CSV:
		{
			os << "Line number" << csv_delimiter << "Value" << csv_delimiter << "Annotated source code" << std::endl;
			unsigned int num_lines = content.size();
			unsigned int i;
			for(i = 0; i < num_lines; i++)
			{
				unsigned int lineno = i + 1;
				
				const T& value = source_code_profile->GetValue(filename.c_str(), lineno);
				
				os << std::dec << lineno << csv_delimiter;
				
				if(value != T())
				{
					os << std::dec << value;
				}
				
				os << csv_delimiter << c_string_to_CSV(content[i].c_str()) << std::endl;
			}
			break;
		}
		
		case O_FMT_HTML:
		{
			Quantizer<T> quantizer = Quantizer<T>(16, value_range);
			//std::cerr << "\"" << filename << "\": value_range=[" << value_range.first << "," << value_range.second << "]" << std::endl;
			
			os << "<!DOCTYPE HTML>" << std::endl;
			os << "<html lang=\"en\">" << std::endl;
			os << "<head>" << std::endl;
			os << "<meta name=\"description\" content=\"Instruction profile report - Annotated Source Code - " << c_string_to_HTML(filename.c_str()) << " [" << c_string_to_HTML(GetSampledVariableName()) << "]\">" << std::endl;
			os << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << "<title>Instruction profile report - Annotated Source Code - " << c_string_to_HTML(filename.c_str()) << " [" << c_string_to_HTML(GetSampledVariableName()) << "]</title>" << std::endl;
			os << "<style type=\"text/css\">" << std::endl;
			os << "<!--" << std::endl;
			os << "body{ font-family:Arial, Helvetica, sans-serif;font-style:normal;font-size:14px;text-align:left;font-weight:400;color:#000;margin-left:auto;margin-right:auto;background:#FFF; }" << std::endl;
			os << "table{ width:1280px; margin-left:auto; margin-right:auto; }" << std::endl;
			os << "table th { text-align:center; }" << std::endl;
			os << "table th { font-weight:bold; }" << std::endl;
			os << "table td { text-align:left; }" << std::endl;
			os << "table { border-style:solid; }" << std::endl;
			os << "table { border-width:0px; }" << std::endl;
			os << "table { border-collapse: collapse; }" << std::endl;
			os << "th, td { padding-left: 1em; }" << std::endl;
			os << "th, td { margin:0; }" << std::endl;
			os << "th, td { border-style:solid; }" << std::endl;
			os << "th, td { border-width:0px; }" << std::endl;
			os << "th, td { border-width:0px; }" << std::endl;
			os << ".scale0 { background-color:#ffffff; }" << std::endl;
			os << ".scale1 { background-color:#fff0f0; }" << std::endl;
			os << ".scale2 { background-color:#ffe0e0; }" << std::endl;
			os << ".scale3 { background-color:#ffd0d0; }" << std::endl;
			os << ".scale4 { background-color:#ffc0c0; }" << std::endl;
			os << ".scale5 { background-color:#ffb0b0; }" << std::endl;
			os << ".scale6 { background-color:#ffa0a0; }" << std::endl;
			os << ".scale7 { background-color:#ff9090; }" << std::endl;
			os << ".scale8 { background-color:#ff8080; }" << std::endl;
			os << ".scale9 { background-color:#ff7070; }" << std::endl;
			os << ".scale10 { background-color:#ff6060; }" << std::endl;
			os << ".scale11 { background-color:#ff5050; }" << std::endl;
			os << ".scale12 { background-color:#ff4040; }" << std::endl;
			os << ".scale13 { background-color:#ff3030; }" << std::endl;
			os << ".scale14 { background-color:#ff2020; }" << std::endl;
			os << ".scale15 { background-color:#ff1010; }" << std::endl;
			os << ".scale16 { background-color:#ff0000; }" << std::endl;
			os << "a:link { text-decoration:none }" << std::endl;
			os << "#title { text-align:center; } " << std::endl;
			os << "#content { width:1280px; margin-left:auto; margin-right:auto; }" << std::endl;
			os << ".source-code { width:768px; font-family:\"Courier New\",Courier,monospace; }" << std::endl;
			os << "-->" << std::endl;
			os << "</style>" << std::endl;
			os << "</head>" << std::endl;
			os << "<body>" << std::endl;
			os << "<div id=\"content\">" << std::endl;
			os << "<h1 id=\"title\"><a href=\"by_function.html\">&#8624;</a>&nbsp;Instruction profile report - Annotated Source Code - " << c_string_to_HTML(filename.c_str()) << "<br>[" << c_string_to_HTML(GetSampledVariableName()) << "]</h1>" << std::endl;
			
			os << "<table>" << std::endl;
			os << "<tr>" << std::endl;
			os << "<th>Line #</th>" << std::endl;
			os << "<th>" << c_string_to_HTML(GetSampledVariableName()) << "</th>" << std::endl;
			os << "<th></th>" << std::endl;
			os << "</tr>" << std::endl;
			unsigned int num_lines = content.size();
			unsigned int i;
			for(i = 0; i < num_lines; i++)
			{
				unsigned int lineno = i + 1;
				
				const T& value = source_code_profile->GetValue(filename.c_str(), lineno); // may return a zero value
				
				std::stringstream sstr;
				sstr << value;
				
				SLoc sloc = SLoc(filename.c_str(), lineno);
				
				bool is_entry_point = false;
				const char *func_name = func_name_loc_conv->SLocToFunctionName(sloc, is_entry_point);
				
				if(value >= value_range.first)
				{
					unsigned int scale = quantizer.Quantize(value);
					//std::cerr << "#" << lineno << ": " << value << " -> " << scale << std::endl;
					
					os << "<tr class=\"scale" << scale << "\"";
				}
				else
				{
					os << "<tr";
				}
				
				if(func_name)
				{
					if(is_entry_point)
					{
						os << " id=\"" << c_string_to_URL(func_name) <<"\"";
					}
				}
				
				os << ">";

				if(func_name)
				{
					os << "<!--in " << c_string_to_HTML(func_name) << " -->";
				}
				
				os << "<td>" << lineno << "</td><td>" << ((value != T()) ? c_string_to_HTML(sstr.str().c_str()) : "") << "<td class=\"source-code\">" << c_string_to_HTML(content[i].c_str()) << "</td></tr>" << std::endl;
			}
			os << "</table>" << std::endl;
			
			os << "</div>" << std::endl;
			os << "</body>" << std::endl;
			break;
		}
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFile<ADDRESS, T>::Init()
{
	if(LocateFile(filename.c_str(), real_filename))
	{
		std::ifstream f(real_filename.c_str(), std::ifstream::in);
		
		if(!f.fail())
		{
			std::string line;
			
			while(std::getline(f, line))
			{
				content.push_back(line);
			}
			
			if(!f.eof())
			{
				std::cerr << "I/O error while reading \"" << real_filename << "\"" << std::endl;
			}
		}
	}
	
	value_range = source_code_profile->GetValueRange();
}

template <typename ADDRESS, typename T>
bool AnnotatedSourceCodeFile<ADDRESS, T>::LocateFile(const char *filename, std::string& match_file_path)
{
	if(access(filename, R_OK) == 0)
	{
		match_file_path = filename;
		return true;
	}
	
	std::string s;
	const char *p;

	std::vector<std::string> search_paths;

	s.clear();
	p = search_path.c_str();
	do
	{
		if(*p == 0 || *p == ';')
		{
			search_paths.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	std::vector<std::string> hierarchical_path;
	
	s.clear();
	p = filename;
	do
	{
		if(*p == 0 || *p == '/' || *p == '\\')
		{
			hierarchical_path.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	bool match = false;
	
	int hierarchical_path_depth = hierarchical_path.size();
	
	if(hierarchical_path_depth > 0)
	{
		int num_search_paths = search_paths.size();
		int k;
		
		for(k = 0; k < num_search_paths; k++)
		{
			const std::string& search_path = search_paths[k];
			int i;
			for(i = 0; (!match) && (i < hierarchical_path_depth); i++)
			{
				std::string try_file_path = search_path;
				int j;
				for(j = i; j < hierarchical_path_depth; j++)
				{
					if(!try_file_path.empty()) try_file_path += '/';
					try_file_path += hierarchical_path[j];
				}
				//std::cerr << "try_file_path=\"" << try_file_path << "\":";
				if(access(try_file_path.c_str(), R_OK) == 0)
				{
					//std::cerr << "found" << std::endl;
					match = true;
					match_file_path = try_file_path;
				}
				else
				{
					//std::cerr << "not found" << std::endl;
				}
			}
		}
	}
	
	return match;
}

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFileSet<ADDRESS, T>::AnnotatedSourceCodeFileSet(const SourceCodeProfile<ADDRESS, T> *_source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, const char *_search_path)
	: source_code_profile(_source_code_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, search_path(_search_path)
	, annotated_source_code_files()
{
	Init();
}

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFileSet<ADDRESS, T>::~AnnotatedSourceCodeFileSet()
{
	Clear();
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFileSet<ADDRESS, T>::Print(std::ostream& os) const
{
	typename std::map<std::string, const AnnotatedSourceCodeFile<ADDRESS, T> *>::const_iterator annotated_source_code_file_it;
	
	for(annotated_source_code_file_it = annotated_source_code_files.begin(); annotated_source_code_file_it != annotated_source_code_files.end(); annotated_source_code_file_it++)
	{
		const AnnotatedSourceCodeFile<ADDRESS, T> *annotated_source_code_file = (*annotated_source_code_file_it).second;
		
		annotated_source_code_file->Print(os);
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFileSet<ADDRESS, T>::Output(const char *output_directory, OutputFormat o_fmt, const char *csv_delimiter)
{
	MakeDir(output_directory);
	
	typename std::map<std::string, const AnnotatedSourceCodeFile<ADDRESS, T> *>::const_iterator annotated_source_code_file_it;
	
	for(annotated_source_code_file_it = annotated_source_code_files.begin(); annotated_source_code_file_it != annotated_source_code_files.end(); annotated_source_code_file_it++)
	{
		const AnnotatedSourceCodeFile<ADDRESS, T> *annotated_source_code_file = (*annotated_source_code_file_it).second;
		
		std::string output_filename(std::string(output_directory) + "/" + c_string_to_URL(annotated_source_code_file->GetFilename()) + OutputFormatSuffix(o_fmt));
		
		std::ofstream output(output_filename.c_str(), std::ios::out);
		
		annotated_source_code_file->Print(output, o_fmt, csv_delimiter);
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFileSet<ADDRESS, T>::Init()
{
	Clear();
	
	const std::set<std::string>& source_filename_set = source_code_profile->GetSourceFilenameSet();
	
	typename std::set<std::string>::const_iterator source_filename_set_it;
	for(source_filename_set_it = source_filename_set.begin(); source_filename_set_it != source_filename_set.end(); source_filename_set_it++)
	{
		const std::string& source_filename = *source_filename_set_it;

		annotated_source_code_files[source_filename] = new AnnotatedSourceCodeFile<ADDRESS, T>(source_filename.c_str(), source_code_profile, func_name_loc_conv, search_path.c_str());
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFileSet<ADDRESS, T>::Clear()
{
	typename std::map<std::string, const AnnotatedSourceCodeFile<ADDRESS, T> *>::iterator annotated_source_code_file_it;
	
	for(annotated_source_code_file_it = annotated_source_code_files.begin(); annotated_source_code_file_it != annotated_source_code_files.end(); annotated_source_code_file_it++)
	{
		const AnnotatedSourceCodeFile<ADDRESS, T> *annotated_source_code_file = (*annotated_source_code_file_it).second;
		delete annotated_source_code_file;
	}
	annotated_source_code_files.clear();
}

//////////////////////////////// Profiler<> ///////////////////////////////////

template <typename ADDRESS>
Profiler<ADDRESS>::Profiler(const char *_name, Object *_parent)
	: Object(_name, _parent, "this service implements an instruction profiler")
	, unisim::kernel::service::Service<unisim::service::interfaces::DebugYielding>(_name, _parent)
	, unisim::kernel::service::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::DebugYieldingRequest>(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::Disassembly<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::Memory<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::Registers>(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::BackTrace<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::Profiling<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::DebugInfoLoading>(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(_name, _parent)
	, debug_yielding_export("debug-yielding-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_yielding_request_import("debug-yielding-request-import", this)
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
	, logger(*this)
	, search_path()
	, filename()
	, sampled_variables()
	, output_directory()
	, output_format(O_FMT_TEXT)
	, csv_delimiter(",")
	, param_search_path("search-path", this, search_path, "Search path for source (separated by ';')")
	, param_filename("filename", this, filename, "List of (binary) files (preferably ELF) to profile (separated by ',')")
	, param_sampled_variables("sampled-variables", this, sampled_variables, "Variables to sample (separated by spaces)")
	, param_output_directory("output-directory", this, output_directory, "Output directory where to generate profiling report")
	, param_output_format("output-format", this, output_format, "Output format (text, html or csv)")
	, param_csv_delimiter("csv-delimiter", this, csv_delimiter, "CSV delimiter")
	, listening_commit(false)
	, trap(false)
	, commit_insn_event(0)
	, num_sampled_variables(0)
	, func_name_loc_conv(0)
	, addr_profiles()
	, func_insn_profiles()
	, pc(0)
	, instruction_profiles()
	, source_code_profiles()
	, annotated_source_code_file_sets()
{
	commit_insn_event = new unisim::util::debug::CommitInsnEvent<ADDRESS>();
	commit_insn_event->Catch();
}

template <typename ADDRESS>
Profiler<ADDRESS>::~Profiler()
{
	unsigned int i;
	
	if(num_sampled_variables > 0)
	{
		UnlistenCommit();
	}

	if(func_name_loc_conv) delete func_name_loc_conv;
	
	unsigned int num_func_insn_profiles = func_insn_profiles.size();
	for(i = 0; i < num_func_insn_profiles; i++)
	{
		FunctionInstructionProfileBase *func_insn_profile = func_insn_profiles[i];
		delete func_insn_profile;
	}

	unsigned int num_annotated_source_code_file_sets = annotated_source_code_file_sets.size();
	for(i = 0; i < num_annotated_source_code_file_sets; i++)
	{
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		delete annotated_source_code_file_set;
	}

	unsigned int num_instruction_profiles = instruction_profiles.size();
	for(i = 0; i < num_instruction_profiles; i++)
	{
		InstructionProfileBase *instruction_profile = instruction_profiles[i];
		delete instruction_profile;
	}

	unsigned int num_source_code_profiles = source_code_profiles.size();
	for(i = 0; i < num_source_code_profiles; i++)
	{
		SourceCodeProfileBase<ADDRESS> *source_code_profile = source_code_profiles[i];
		delete source_code_profile;
	}

	unsigned int num_addr_profiles = addr_profiles.size();
	for(i = 0; i < num_addr_profiles; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		delete addr_profile;
	}

	if(commit_insn_event) commit_insn_event->Release();
}

template<class ADDRESS>
bool Profiler<ADDRESS>::EndSetup()
{
	num_sampled_variables = 0;
	
	if(!registers_import) return false;
	if(!stmt_lookup_import) return false;
	if(!symbol_table_lookup_import) return false;

	LoadDebugInfo();
	
	func_name_loc_conv = new FunctionNameLocationConversion<ADDRESS>(stmt_lookup_import, symbol_table_lookup_import);
	
	std::stringstream sstr(sampled_variables);
	std::string var_name;

	while(sstr >> var_name)
	{
		unisim::kernel::service::VariableBase *var = unisim::kernel::service::Object::GetSimulator()->FindVariable(var_name.c_str());
		
		if(var->IsVoid())
		{
			logger << DebugWarning << "Variable \"" << var_name << "\" does not exist" << EndDebugWarning;
		}
		else
		{
			if(TryProfile<signed char>(var) ||
			   TryProfile<short>(var) ||
			   TryProfile<int>(var) ||
			   TryProfile<long>(var) ||
			   TryProfile<long long>(var) ||
			   TryProfile<unsigned char>(var) ||
			   TryProfile<bool>(var) ||
			   TryProfile<unsigned short>(var) ||
			   TryProfile<unsigned int>(var) ||
			   TryProfile<unsigned long>(var) ||
			   TryProfile<unsigned long long>(var) ||
			   TryProfile<double>(var)
			)
			{
				logger << DebugInfo << "Profiling for Variable \"" << var_name << "\"" << EndDebugInfo;
			}
			else if((strcmp(var->GetDataTypeName(), "sc_time") == 0) && TryProfile<sc_core::sc_time>(var))
			{
				logger << DebugInfo << "Profiling for time Statistic \"" << var_name << "\"" << EndDebugInfo;
			}
			else
			{
				logger << DebugWarning << "Can't profile for Statistic \"" << var_name << "\"" << EndDebugWarning;
			}
		}
	}
	
	listening_commit = false;
	pc = 0;
	trap = false;

	if(num_sampled_variables > 0)
	{
		if(!ListenCommit()) return false;
	}
	
	return true;
}

template <typename ADDRESS>
void Profiler<ADDRESS>::OnDisconnect()
{
}

// unisim::service::interfaces::DebugYielding
template <typename ADDRESS>
void Profiler<ADDRESS>::DebugYield()
{
	unsigned int num_addr_profiles = addr_profiles.size();
	unsigned int i;
	
	for(i = 0; i < num_addr_profiles; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		
		addr_profile->Capture(pc, length);
	}
}

// unisim::service::interfaces::DebugEventListener<ADDRESS>
template <typename ADDRESS>
void Profiler<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	
	if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN))
	{
		const unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<const unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);
		
		pc = commit_insn_event->GetAddress();
		length = commit_insn_event->GetLength();
	}
}

template <typename ADDRESS>
bool Profiler<ADDRESS>::ListenCommit()
{
	if(listening_commit) return true;
	
	if(!debug_event_trigger_import->Listen(commit_insn_event))
	{
		logger << DebugError << "Can't track committed instructions" << EndDebugError;
		return false;
	}

	listening_commit = true;
	
	return true;
}

template <typename ADDRESS>
bool Profiler<ADDRESS>::UnlistenCommit()
{
	if(!listening_commit) return true;
	
	if(!debug_event_trigger_import->Unlisten(commit_insn_event))
	{
		logger << DebugError << "Can't untrack committed instructions" << EndDebugError;
		return false;
	}
	
	listening_commit = false;
	
	return true;
}

template <typename ADDRESS>
template <typename T> bool Profiler<ADDRESS>::TryProfile(unisim::kernel::service::VariableBase *var)
{
	unisim::kernel::service::Variable<T> *typed_var = dynamic_cast<unisim::kernel::service::Variable<T> *>(var);
	
	if(typed_var)
	{
		AddressProfile<ADDRESS, T> *addr_profile = new AddressProfile<ADDRESS, T>(typed_var, disasm_import, stmt_lookup_import, symbol_table_lookup_import);
		addr_profiles.push_back(addr_profile);
		
		InstructionProfileBase *instruction_profile = addr_profile->CreateInstructionProfile(func_name_loc_conv);
		instruction_profiles.push_back(instruction_profile);
		
		SourceCodeProfileBase<ADDRESS> *source_code_profile = addr_profile->CreateSourceCodeProfile();
		source_code_profiles.push_back(source_code_profile);
		
		num_sampled_variables++;
		return true;
	}
	
	return false;
}

template <typename ADDRESS>
void Profiler<ADDRESS>::LoadDebugInfo()
{
	// Disable all binaries for debugging
	
	std::list<std::string> binaries;
	
	debug_info_loading_import->EnumerateBinaries(binaries);
	
	std::list<std::string>::iterator binary_it;
	for(binary_it = binaries.begin(); binary_it != binaries.end(); binary_it++)
	{
		const std::string& binary = *binary_it;
		
		debug_info_loading_import->EnableBinary(binary.c_str(), false);
	}
	
	// Selectively load/reenable debug information from binaries
	if(filename.empty())
	{
		logger << DebugWarning << "No debugging information loaded" << EndDebugWarning;
	}
	else
	{
		std::size_t pos = 0;
		std::size_t delim_pos = std::string::npos;
		
		do
		{
			delim_pos = filename.find_first_of(",", pos);
			
			std::string _filename = (delim_pos != std::string::npos) ? filename.substr(pos, delim_pos - pos) : filename.substr(pos);
			
			std::string real_filename = unisim::kernel::service::Object::GetSimulator()->SearchSharedDataFile(_filename.c_str());
			
			if(debug_info_loading_import->EnableBinary(real_filename.c_str(), true))
			{
				logger << DebugInfo << "Debugging information from \"" << real_filename << "\" previously loaded" << EndDebugInfo;
			}
			else
			{
				if(debug_info_loading_import->LoadDebugInfo(real_filename.c_str()))
				{
					logger << DebugInfo << "Debugging information from \"" << real_filename << "\" loaded" << EndDebugInfo;
				}
				else
				{
					logger << DebugInfo << "No debugging information loaded from \"" << real_filename << "\"" << EndDebugInfo;
				}
			}
			pos = (delim_pos != std::string::npos) ? delim_pos + 1 : std::string::npos;
		}
		while(pos != std::string::npos);
	}
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Update()
{
	unsigned int i;
	
	unsigned int num_func_insn_profiles = func_insn_profiles.size();
	for(i = 0; i < num_func_insn_profiles; i++)
	{
		FunctionInstructionProfileBase *func_insn_profile = func_insn_profiles[i];
		delete func_insn_profile;
	}
	func_insn_profiles.clear();

	unsigned int num_annotated_source_code_file_sets = annotated_source_code_file_sets.size();
	for(i = 0; i < num_annotated_source_code_file_sets; i++)
	{
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		delete annotated_source_code_file_set;
	}
	annotated_source_code_file_sets.clear();
	
	unsigned int num_addr_profiles = addr_profiles.size();
	
	for(i = 0; i < num_addr_profiles; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		
		FunctionInstructionProfileBase *func_insn_profile = addr_profile->CreateFunctionInstructionProfile(func_name_loc_conv);
		func_insn_profiles.push_back(func_insn_profile);
	}
	
	unsigned int num_source_code_profiles = source_code_profiles.size();
	
	for(i = 0; i < num_source_code_profiles; i++)
	{
		SourceCodeProfileBase<ADDRESS> *source_code_profile = source_code_profiles[i];
		
		source_code_profile->Update();
		
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = source_code_profile->CreateAnnotatedSourceCodeFileSet(func_name_loc_conv, search_path.c_str());
		annotated_source_code_file_sets.push_back(annotated_source_code_file_set);
	}
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Print(std::ostream& os)
{
	OStreamContext osc(os);

	unsigned int i;

	for(i = 0; i < num_sampled_variables; i++)
	{
		os << "################ Address profiles ###################" << std::endl;
	
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
	
		os << "[" << addr_profile->GetSampledVariableName() << "]" << std::endl;
		addr_profile->Print(os);

		os << "################ Function/Source code profiles ###################" << std::endl;
	
		SourceCodeProfileBase<ADDRESS> *source_code_profile = source_code_profiles[i];
		
		os << "[" << source_code_profile->GetSampledVariableName() << "]" << std::endl;
		source_code_profile->Print(os);
		
		continue;
		
		os << "################ Annotated Source codes ###################" << std::endl;

		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		
		os << "[" << annotated_source_code_file_set->GetSampledVariableName() << "]" << std::endl;
		annotated_source_code_file_set->Print(os);
	}
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Output()
{
	if(num_sampled_variables == 0) return;
	
	Update();
		
	if(output_directory.empty())
	{
		logger << DebugWarning << "No output directory was specified in \"" << param_output_directory.GetName() << "\"" << EndDebugWarning;
		return;
	}
	
	unsigned int i;

	MakeDir(output_directory.c_str());

	switch(output_format)
	{
		case O_FMT_TEXT:
			break;
			
		case O_FMT_CSV:
			break;

		case O_FMT_HTML:
		{
			std::string index_filename(output_directory + "/index.html");
			
			std::ofstream index(index_filename.c_str(), std::ios::out);
			
			index << "<!DOCTYPE HTML>" << std::endl;
			index << "<html lang=\"en\">" << std::endl;
			index << "<head>" << std::endl;
			index << "<meta name=\"description\" content=\"Instruction profile report - By variable\">" << std::endl;
			index << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			index << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			index << "<title>Instruction profile report - By variable</title>" << std::endl;
			index << "<style type=\"text/css\">" << std::endl;
			index << "<!--" << std::endl;
			index << "body{ font-family:Arial, Helvetica, sans-serif;font-style:normal;font-size:14px;text-align:left;font-weight:400;color:#000;margin-left:auto;margin-right:auto;background:#FFF; }" << std::endl;
			index << "table{ width:1280px; margin-left:auto; margin-right:auto; }" << std::endl;
			index << "table th { text-align:center; }" << std::endl;
			index << "table th { font-weight:bold; }" << std::endl;
			index << "table td { text-align:left; }" << std::endl;
			index << "table { border-style:solid; }" << std::endl;
			index << "table { border-width:1px; }" << std::endl;
			index << "table { border-collapse: collapse; }" << std::endl;
			index << "th, td { margin:0; }" << std::endl;
			index << "th, td { border-style:solid; }" << std::endl;
			index << "th, td { border-width:1px; }" << std::endl;
			index << "th, td { border-width:1px; }" << std::endl;
			index << "a:link { text-decoration:none }" << std::endl;
			index << "#title { text-align:center; } " << std::endl;
			index << "#content { width:1280px; margin-left:auto; margin-right:auto; }" << std::endl;
			index << "-->" << std::endl;
			index << "</style>" << std::endl;
			index << "</head>" << std::endl;
			index << "<body>" << std::endl;
			index << "<div id=\"content\">" << std::endl;
			index << "<h1 id=\"title\">Instruction profile report - By variable</h1>" << std::endl;
			index << "<table>" << std::endl;
			index << "<tr>" << std::endl;
			index << "<th>Variable</th><th>Description</th><th>Value</th><th>Raw data</th><th>Profile</th>" << std::endl;
			index << "</tr>" << std::endl;
			for(i = 0; i < num_sampled_variables; i++)
			{
				AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
				
				index << "<tr>" << std::endl;
				index << "<td>" << c_string_to_HTML(addr_profile->GetSampledVariableName()) << "</td>" << std::endl;
				index << "<td>" << c_string_to_HTML(addr_profile->GetSampledVariable()->GetDescription()) << "</td>" << std::endl;
				index << "<td>" << c_string_to_HTML(((std::string) *addr_profile->GetSampledVariable()).c_str()) << "</td>" << std::endl;
				index << "<td><a href=\"" << c_string_to_URL(addr_profile->GetSampledVariableName()) << "/by_address.html\">By address</a></td>" << std::endl;
				index << "<td><a href=\"" << c_string_to_URL(addr_profile->GetSampledVariableName()) << "/by_function.html\">By function</a></td>" << std::endl;
				index << "</tr>" << std::endl;
			}
			index << "</table>" << std::endl;
			
			index << "</body>" << std::endl;
			index << "</div>" << std::endl;
			index << "</html>" << std::endl;
			break;
		}
	}

	for(i = 0; i < num_sampled_variables; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		
		std::string variable_output_directory(output_directory + "/" + addr_profile->GetSampledVariableName());
		
		MakeDir(variable_output_directory.c_str());
		
		std::string by_addr_filename(variable_output_directory + "/by_address" + OutputFormatSuffix(output_format));
		
		std::ofstream by_addr(by_addr_filename.c_str(), std::ios::out);
		
		addr_profile->Print(by_addr, output_format, csv_delimiter.c_str());
	}

	for(i = 0; i < num_sampled_variables; i++)
	{
		FunctionInstructionProfileBase *func_insn_profile = func_insn_profiles[i];
		
		std::string variable_output_directory(output_directory + "/" + func_insn_profile->GetSampledVariableName());
		
		MakeDir(variable_output_directory.c_str());
		
		std::string by_function_filename(variable_output_directory + "/by_function" + OutputFormatSuffix(output_format));
		
		std::ofstream by_function(by_function_filename.c_str(), std::ios::out);
		
		func_insn_profile->Print(by_function, output_format, csv_delimiter.c_str());
		
		std::string disassembly_filename(variable_output_directory + "/disassembly" + OutputFormatSuffix(output_format));
		
		std::ofstream disassembly(disassembly_filename.c_str(), std::ios::out);
		
		InstructionProfileBase *instruction_profile = instruction_profiles[i];
		instruction_profile->Print(disassembly, output_format, csv_delimiter.c_str());
		
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		
		annotated_source_code_file_set->Output(variable_output_directory.c_str(), output_format, csv_delimiter.c_str());
	}
}

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
