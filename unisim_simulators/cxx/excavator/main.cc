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

#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/json/json.tcc>
#include <unisim/util/loader/elf_loader/elf_loader.tcc>
#include <iostream>
#include <set>
#include <stack>
#include <fstream>
#include <cassert>
#include <cstdint>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

///////////////////////////////////////////////////////////////////////////////
//                              Declarations                                 //
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Verbose ////////////////////////////////////

enum Verbose
{
	VERBOSE_ERROR   = 0, // errors only
	VERBOSE_WARNING = 1, // errors and warnings only
	VERBOSE_INFO    = 2, // errors, warnings, and general informations only
	VERBOSE_DEBUG   = 3  // everything
};

///////////////////////////// DWARF_Excavator<> ///////////////////////////////

template <typename MEMORY_ADDR, typename CONFIG_READER>
class DWARF_Excavator
{
public:
	DWARF_Excavator(const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler, const CONFIG_READER& config_reader);
	virtual ~DWARF_Excavator();
	bool Dig();
protected:
private:
	typedef typename CONFIG_READER::Sources Sources;
	typedef typename CONFIG_READER::SuppressTypes SuppressTypes;
	typedef typename CONFIG_READER::SuppressFunctions SuppressFunctions;
	
	const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler;
	const std::string& binary;
	Verbose verbose;
	
	const Sources& sources;
	const SuppressTypes& suppress_types;
	const SuppressFunctions& suppress_functions;
	const std::string& output_dir;
};

///////////////////////////// JSON_ConfigReader ///////////////////////////////

struct JSON_ConfigReader : unisim::util::json::JSON_Lexer<JSON_ConfigReader>
{
	typedef unisim::util::json::JSON_Lexer<JSON_ConfigReader> Lexer;
	typedef std::set<std::string> Sources;
	typedef std::vector<std::string> SuppressTypes;
	typedef std::vector<std::string> SuppressFunctions;
	
	JSON_ConfigReader(std::ostream& err = std::cerr);
	
	bool Parse(std::istream& stream) { return ParseRoot(stream); }

	Verbose GetVerbose() const { return verbose; }
	const std::string& GetBinary() const { return binary; }
	const std::string& GetOutputDir() const { return output_dir; }
	const Sources& GetSources() const { return sources; }
	const SuppressTypes& GetSuppressTypes() const { return suppress_types; }
	const SuppressFunctions& GetSuppressFunctions() const { return suppress_functions; }
	const std::string& GetDWARF_HTML_OutputDir() const { return dwarf_html_output_dir; }
	
	void Error(const std::string& msg) { err << msg; }
	
private:
	typedef enum { UNKNOWN, VERBOSE, BINARY, SOURCES, SUPPRESS_TYPES, SUPPRESS_FUNCTIONS, OUTPUT_DIR, DWARF_HTML_OUTPUT_DIR } MemberType;
	std::ostream& err;
	Verbose verbose;
	std::string binary;
	std::string output_dir;
	Sources sources;
	SuppressTypes suppress_types;
	SuppressFunctions suppress_functions;
	std::string dwarf_html_output_dir;
	
	bool ParseRoot(std::istream& stream);
	bool ParseRootMembers(std::istream& stream);
	bool ParseRootMember(std::istream& stream);
	bool ParseBool(std::istream& stream, bool& value);
	bool ParseUnsignedInteger(std::istream& stream, uint64_t& value);
	bool ParseString(std::istream& stream, std::string& value);
	template <typename CONTAINER> bool ParseStringArray(std::istream& stream, CONTAINER& strings);
	template <typename CONTAINER> bool ParseStringArrayElements(std::istream& stream, CONTAINER& strings);
};

///////////////////////////////// Utilities ///////////////////////////////////

namespace {
	// reorder_qualifiers: rewriting "const volatile" into "volatile const"
	inline std::string reorder_qualifiers(const std::string& s)
	{
		std::string r(s);
		std::size_t pos = 0;
		do
		{
			std::size_t fpos = s.find("const volatile", pos);
			if(fpos == std::string::npos) break;
			r.replace(fpos, 14, "volatile const");
			pos = fpos + 14;
		}
		while(pos < s.length());
		return r;
	}
	
	inline std::string kill_array_subscript(const std::string& s)
	{
		std::string r;
		std::size_t pos = 0;
		std::size_t len = s.length();
		while(pos < len)
		{
			char c = s[pos++];
			if(c == '[')
			{
				r += "[]";
				std::size_t right_bracket_pos = s.find_first_of(']', pos);
				assert(right_bracket_pos != std::string::npos);
				pos = right_bracket_pos + 1;
			}
			else
			{
				r += c;
			}
		}
		
		return r;
	}
	
	// Type comparator
	struct TypeComparator
	{
		bool operator () (unisim::util::debug::Type const * const& t1, unisim::util::debug::Type const * const& t2) const
		{
			return reorder_qualifiers(t1->BuildCDecl()) < reorder_qualifiers(t2->BuildCDecl());
		}
	};
	
	bool Match(const char *p, const char *s)
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
}

///////////////////////////////////////////////////////////////////////////////
//                              Definitions                                  //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// DWARF_Excavator<> ///////////////////////////////

template <typename MEMORY_ADDR, typename CONFIG_READER>
DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>::DWARF_Excavator(const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *_dw_handler, const CONFIG_READER& config_reader)
	: dw_handler(_dw_handler)
	, binary(config_reader.GetBinary())
	, verbose(config_reader.GetVerbose())
	, sources(config_reader.GetSources())
	, suppress_types(config_reader.GetSuppressTypes())
	, suppress_functions(config_reader.GetSuppressFunctions())
	, output_dir(config_reader.GetOutputDir())
{
}

template <typename MEMORY_ADDR, typename CONFIG_READER>
DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>::~DWARF_Excavator()
{
}

template <typename MEMORY_ADDR, typename CONFIG_READER>
bool DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>::Dig()
{
	struct DebugInfoVisitor
	{
		typedef typename CONFIG_READER::Sources Sources;
		typedef typename CONFIG_READER::SuppressTypes SuppressTypes;
		typedef typename CONFIG_READER::SuppressFunctions SuppressFunctions;
		
		const DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>& dw_excavator;
		const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler;
		const std::string& binary;
		const std::string& output_dir;
		Verbose verbose;
		const Sources& sources;
		const SuppressTypes& suppress_types;
		const SuppressFunctions& suppress_functions;
		
		struct CompilationUnit;
		
		struct SubProgram
		{
			unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram;
			unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol;
			CompilationUnit *comp_unit;
			unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die;
			std::string note;
			bool weak;
			
			SubProgram(unisim::util::debug::SubProgram<MEMORY_ADDR> const *_subprogram, unisim::util::debug::Symbol<MEMORY_ADDR> const *_symbol, CompilationUnit *_comp_unit, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *_dw_die)
				: subprogram(_subprogram)
				, symbol(_symbol)
				, comp_unit(_comp_unit)
				, dw_die(_dw_die)
				, note()
				, weak(subprogram->IsExternal() && !subprogram->IsInline() && (!dw_die->HasRanges() || !symbol || !dw_die->HasOverlap(symbol->GetAddress(), 1)))
			{
				unisim::util::debug::DeclLocation const *decl_loc = subprogram->GetDeclLocation();
				if(decl_loc)
				{
					std::stringstream note_sstr;
					
					note_sstr << (subprogram->IsDeclaration() ? "Declaration" : "Definition") << " in File " << decl_loc->GetDeclFilename() << ", Line #" << decl_loc->GetDeclLine() << ", Column #" << decl_loc->GetDeclColumn();
					note = note_sstr.str();
				}
				
				subprogram->Catch();
			}
			
			SubProgram(SubProgram const& o)
				: subprogram(o.subprogram)
				, symbol(o.symbol)
				, comp_unit(o.comp_unit)
				, dw_die(o.dw_die)
				, note(o.note)
				, weak(o.weak)
			{
				subprogram->Catch();
			}
			
			SubProgram& operator = (SubProgram const& o)
			{
				if(subprogram != o.subprogram)
				{
					subprogram->Release();
					subprogram = o.subprogram;
					subprogram->Catch();
				}
				symbol = o.symbol;
				comp_unit = o.comp_unit;
				dw_die = o.dw_die;
				note = o.note;
				weak = o.weak;
				return *this;
			}
			
			~SubProgram()
			{
				subprogram->Release();
			}
			
			void AppendNote(const std::string& _note)
			{
				if(note.empty())
				{
					note = _note;
				}
				else
				{
					note += ", ";
					note += _note;
				}
			}
			
			const char *GetName() const { return subprogram->GetName(); }
			bool IsExternal() const { return subprogram->IsExternal(); }
			bool IsDeclaration() const { return subprogram->IsDeclaration(); }
			bool IsInline() const { return subprogram->IsInline(); }
			bool IsInlined() const { return subprogram->IsInlined(); }
			const unisim::util::debug::Type *GetReturnType() const { return subprogram->GetReturnType(); }
			unsigned int GetArity() const { return subprogram->GetArity(); }
			const unisim::util::debug::FormalParameter *GetFormalParameter(unsigned int idx) const { return subprogram->GetFormalParameter(idx); }
			std::string BuildCDecl() const { return subprogram->BuildCDecl(); }
			bool HasSymbol() const { return symbol != 0; }
			MEMORY_ADDR GetAddress() const { return symbol ? symbol->GetAddress() : 0xdeadbeef; }
			CompilationUnit *GetCompilationUnit() const { return comp_unit; }
			const std::string& GetNote() const { return note; }
			bool IsWeak() const { return weak; }
		};
		
		struct Variable
		{
			unisim::util::debug::Variable const *variable;
			unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol;
			CompilationUnit *comp_unit;
			unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die;
			std::string note;
			
			Variable(unisim::util::debug::Variable const *_variable, unisim::util::debug::Symbol<MEMORY_ADDR> const *_symbol, CompilationUnit *_comp_unit, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *_dw_die)
				: variable(_variable)
				, symbol(_symbol)
				, comp_unit(_comp_unit)
				, dw_die(_dw_die)
				, note()
			{
				unisim::util::debug::DeclLocation const *decl_loc = variable->GetDeclLocation();
				if(decl_loc)
				{
					std::stringstream note_sstr;
					
					note_sstr << (variable->IsDeclaration() ? "Declaration" : "Definition") << " in File " << decl_loc->GetDeclFilename() << ", Line #" << decl_loc->GetDeclLine() << ", Column #" << decl_loc->GetDeclColumn();
					note = note_sstr.str();
				}
				variable->Catch();
			}
			
			Variable(Variable const& o)
				: variable(o.variable)
				, symbol(o.symbol)
				, comp_unit(o.comp_unit)
				, dw_die(o.dw_die)
				, note(o.note)
			{
				variable->Catch();
			}
			
			Variable& operator = (Variable const& o)
			{
				if(variable != o.variable)
				{
					variable->Release();
					variable = o.variable;
					variable->Catch();
				}
				symbol = o.symbol;
				comp_unit = o.comp_unit;
				dw_die = o.dw_die;
				note = o.note;
				return *this;
			}
			
			~Variable()
			{
				variable->Release();
			}
			
			void AppendNote(const std::string& _note)
			{
				if(note.empty())
				{
					note = _note;
				}
				else
				{
					note += ", ";
					note += _note;
				}
			}
			
			const char *GetName() const { return variable->GetName(); }
			bool IsExternal() const { return variable->IsExternal(); }
			bool IsDeclaration() const { return variable->IsDeclaration(); }
			const unisim::util::debug::Type *GetType() const { return variable->GetType(); }
			std::string BuildCDecl() const { return variable->BuildCDecl(); }
			bool HasSymbol() const { return symbol != 0; }
			MEMORY_ADDR GetAddress() const { return symbol ? symbol->GetAddress() : 0xdeadbeef; }
			CompilationUnit *GetCompilationUnit() const { return comp_unit; }
			const std::string& GetNote() const { return note; }
		};
		
		// Subprogram comparator
		struct SubProgramComparator
		{
			bool operator () (SubProgram const& sp1, SubProgram const& sp2) const
			{
				return reorder_qualifiers(kill_array_subscript(sp1.BuildCDecl())) < reorder_qualifiers(kill_array_subscript(sp2.BuildCDecl()));
			}
		};

		// Variable comparator
		struct VariableComparator
		{
			bool operator () (Variable const& v1, Variable const& v2) const
			{
				return reorder_qualifiers(kill_array_subscript(v1.BuildCDecl())) < reorder_qualifiers(kill_array_subscript(v2.BuildCDecl()));
			}
		};

		typedef std::set<unisim::util::debug::Type const *, TypeComparator> Types;
		typedef std::vector<unisim::util::debug::Type const *> Stack;
		typedef std::set<SubProgram, SubProgramComparator> SubPrograms;
		typedef std::set<Variable, VariableComparator> Variables;
		typedef std::multimap<MEMORY_ADDR, SubProgram const *> SubProgramDefinitionsByAddr;
		typedef std::multimap<MEMORY_ADDR, Variable const *> VariableDefinitionsByAddr;
		
		struct CompilationUnit
		{
			DebugInfoVisitor& debug_info_visitor;
			const DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>& dw_excavator;
			const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler;
			unisim::util::debug::dwarf::DWARF_CompilationUnit<MEMORY_ADDR> const *dw_cu;
			const std::string& binary;
			Verbose verbose;
			
			Types types;
			
			std::set<unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *> visited;
			std::set<unisim::util::debug::Type const *, TypeComparator> pushed;
			
			Stack stack;
			
			SubPrograms subprogram_declarations;
			SubPrograms subprogram_definitions;
			
			Variables variable_declarations;
			Variables variable_definitions;
			
			CompilationUnit(DebugInfoVisitor& _debug_info_visitor, unisim::util::debug::dwarf::DWARF_CompilationUnit<MEMORY_ADDR> const *_dw_cu)
				: debug_info_visitor(_debug_info_visitor)
				, dw_excavator(debug_info_visitor.dw_excavator)
				, dw_handler(debug_info_visitor.dw_handler)
				, dw_cu(_dw_cu)
				, binary(dw_excavator.binary)
				, verbose(dw_excavator.verbose)
				, types()
				, visited()
				, pushed()
				, stack()
				, subprogram_declarations()
				, subprogram_definitions()
				, variable_declarations()
				, variable_definitions()
			{
			}
			
			virtual ~CompilationUnit()
			{
				for(typename Types::const_iterator it = types.begin(); it != types.end(); ++it)
				{
					unisim::util::debug::Type const *type = *it;
					type->Release();
				}
				
				for(typename Stack::const_iterator it = stack.begin(); it != stack.end(); ++it)
				{
					unisim::util::debug::Type const *type = *it;
					if(type)
					{
						type->Release();
					}
				}
			}
			
			unsigned int GetId() const
			{
				return dw_cu ? dw_cu->GetId() : 0xdeadbeef;
			}
			
			const char *GetName() const
			{
				return dw_cu ? dw_cu->GetName() : "unnamed";
			}
			
			void Insert(unisim::util::debug::Type const *type, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
			{
				if(types.find(type) == types.end())
				{
					if(verbose >= VERBOSE_DEBUG)
					{
						std::cout << "Discovering Type " << type->BuildCDecl() << " in DIE #" << dw_die->GetId() << " (" << dw_die->GetHREF() << ")" << std::endl;
					}
					type->Catch();
					types.insert(type);
				}
			}
			
			void InsertDeclaration(unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram, unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
			{
				subprogram_declarations.insert(SubProgram(subprogram, symbol, this, dw_die));
			}
			
			void InsertDefinition(unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram, unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
			{
				std::pair<typename SubPrograms::iterator, bool> r = subprogram_definitions.insert(SubProgram(subprogram, symbol, this, dw_die));
				if(r.second) // newly inserted?
				{
					SubProgram const& inserted_subprogram = *r.first;
					debug_info_visitor.InsertDefinition(&inserted_subprogram);
				}
			}
			
			void InsertDeclaration(unisim::util::debug::Variable const *variable, unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
			{
				variable_declarations.insert(Variable(variable, symbol, this, dw_die));
			}
			
			void InsertDefinition(unisim::util::debug::Variable const *variable, unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
			{
				std::pair<typename Variables::iterator, bool> r = variable_definitions.insert(Variable(variable, symbol, this, dw_die));
				if(r.second) // newly inserted?
				{
					Variable const& inserted_variable = *r.first;
					debug_info_visitor.InsertDefinition(&inserted_variable);
				}
			}
			
			bool AnnotateDeclaration(SubProgram const *subprogram, const std::string& note)
			{
				typename SubPrograms::const_iterator it = subprogram_declarations.find(*subprogram);
				
				if(it != subprogram_declarations.end())
				{
					SubProgram patched_subprogram = SubProgram(*subprogram);
					patched_subprogram.AppendNote(note);
					subprogram_declarations.erase(it);
					subprogram_declarations.insert(patched_subprogram);
					return true;
				}

				std::cerr << "INTERNAL ERROR! can't find subprogram in compilation unit #" << dw_cu->GetId() << " (" << GetName() << ")" << std::endl;
				return false;
			}

			bool AnnotateDefinition(SubProgram const *subprogram, const std::string& note)
			{
				typename SubPrograms::const_iterator it = subprogram_definitions.find(*subprogram);
				if(it != subprogram_definitions.end())
				{
					SubProgram patched_subprogram = SubProgram(*subprogram);
					patched_subprogram.AppendNote(note);
					subprogram_definitions.erase(it);
					subprogram_definitions.insert(patched_subprogram);
					return true;
				}
				
				std::cerr << "INTERNAL ERROR! can't find subprogram in compilation unit #" << dw_cu->GetId() << " (" << GetName() << ")" << std::endl;
				return false;
			}

			bool AnnotateDeclaration(Variable const *variable, const std::string& note)
			{
				typename Variables::const_iterator it = variable_declarations.find(*variable);
				if(it != variable_declarations.end())
				{
					Variable patched_variable = Variable(*variable);
					patched_variable.AppendNote(note);
					variable_declarations.erase(it);
					variable_declarations.insert(patched_variable);
					return true;
				}
				
				std::cerr << "INTERNAL ERROR! can't find variable in compilation unit #" << dw_cu->GetId() << " (" << GetName() << ")" << std::endl;
				return false;
			}
			
			bool AnnotateDefinition(Variable const *variable, const std::string& note)
			{
				typename Variables::const_iterator it = variable_definitions.find(*variable);
				if(it != variable_definitions.end())
				{
					Variable patched_variable = Variable(*variable);
					patched_variable.AppendNote(note);
					variable_definitions.erase(it);
					variable_definitions.insert(patched_variable);
					return true;
				}
				
				std::cerr << "INTERNAL ERROR! can't find variable in compilation unit #" << dw_cu->GetId() << " (" << GetName() << ")" << std::endl;
				return false;
			}

			void Push(unisim::util::debug::Type const *type)
			{
				if(pushed.find(type) == pushed.end())
				{
					if(verbose >= VERBOSE_DEBUG)
					{
						std::cout << "Pushing Type " << type->BuildCDecl() << std::endl;
					}
					pushed.insert(type);
					type->Catch();
					stack.push_back(type);
				}
			}
			
			bool Visited(unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
			{
				if(visited.find(dw_die) != visited.end()) return true;
				visited.insert(dw_die);
				return false;
			}
			
			void SortTypes()
			{
				if(verbose >= VERBOSE_DEBUG)
				{
					std::cout << "Sorting types" << std::endl;
				}
				for(typename Types::const_iterator it = types.begin(); it != types.end(); ++it)
				{
					unisim::util::debug::Type const *type = *it;
					type->Visit(debug_info_visitor);
				}
			}
			
			bool Generate(const std::string& output_filename, bool gen_main)
			{
				if(verbose >= VERBOSE_DEBUG)
				{
					std::cout << "Opening output File \"" << output_filename << "\"" << std::endl;
				}
				std::ofstream file(output_filename.c_str());
				
				if(file.fail())
				{
					std::cerr << "ERROR! Can't open output File \"" << output_filename << "\"" << std::endl;
					return false;
				}
				
				if(dw_cu)
				{
					const char *comp_unit_name = dw_cu->GetName();
					file << "//// " << (comp_unit_name ? comp_unit_name : "unnamed") << " ////" << std::endl;
					file << std::endl;
				}
				
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << "//                                  Types                                    //" << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << std::endl;
				for(typename Stack::const_iterator it = stack.begin(); it != stack.end(); ++it)
				{
					unisim::util::debug::Type const *type = *it;
					if(type)
					{
						file << type->BuildCDecl() << ";";
						unisim::util::debug::DeclLocation const *decl_loc = type->GetDeclLocation();
						if(decl_loc)
						{
							file << " // Declaration in File " << decl_loc->GetDeclFilename() << ", Line #" << decl_loc->GetDeclLine() << ", Column #" << decl_loc->GetDeclColumn();
						}
						file << std::endl;
					}
				}
				
				file << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << "//                            External Functions                             //" << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << std::endl;
				for(typename SubPrograms::const_iterator it = subprogram_declarations.begin(); it != subprogram_declarations.end(); ++it)
				{
					SubProgram const& subprogram = *it;
					if(!subprogram.IsExternal()) continue;
					file << "extern ";
					file << subprogram.BuildCDecl() << ";";
					const std::string& note = subprogram.GetNote();
					if(!note.empty())
					{
						file << " // " << note;
					}
					file << std::endl;
				}
				
				file << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << "//                         External Public Variables                         //" << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << std::endl;
				for(typename Variables::const_iterator it = variable_declarations.begin(); it != variable_declarations.end(); ++it)
				{
					Variable const& variable = *it;
					if(!variable.IsExternal()) continue;
					file << "extern ";
					file << variable.BuildCDecl() << ";";
					const std::string& note = variable.GetNote();
					if(!note.empty())
					{
						file << " // " << note;
					}
					file << std::endl;
				}

				file << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << "//                              Public Variables                             //" << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << std::endl;
				for(typename Variables::const_iterator it = variable_definitions.begin(); it != variable_definitions.end(); ++it)
				{
					Variable const& variable = *it;
					if(!variable.IsExternal()) continue;
					file << variable.BuildCDecl() << ";";
					const std::string& note = variable.GetNote();
					if(!note.empty())
					{
						file << " // " << note;
					}
					file << std::endl;
				}

				file << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << "//                              Public Functions                             //" << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << std::endl;
				for(typename SubPrograms::const_iterator it = subprogram_definitions.begin(); it != subprogram_definitions.end(); ++it)
				{
					SubProgram const& subprogram = *it;
					if(!subprogram.IsExternal()) continue;
					if(subprogram.IsWeak())
					{
						file << "__attribute__((weak)) ";
					}
					if(subprogram.IsInline())
					{
						file << "inline ";
					}
					file << subprogram.BuildCDecl();
					const std::string& note = subprogram.GetNote();
					if(!note.empty())
					{
						file << " // " << note;
					}
					file << std::endl;
					file << "{" << std::endl;
					file << "}" << std::endl;
					file << std::endl;
				}
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << "//                             Private Variables                             //" << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << std::endl;
				for(typename Variables::const_iterator it = variable_definitions.begin(); it != variable_definitions.end(); ++it)
				{
					Variable const& variable = *it;
					if(variable.IsExternal()) continue;
					file << "static ";
					file << variable.BuildCDecl() << ";";
					const std::string& note = variable.GetNote();
					if(!note.empty())
					{
						file << " // " << note;
					}
					file << std::endl;
				}
				file << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << "//                              Private Functions                            //" << std::endl;
				file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
				file << std::endl;
				for(typename SubPrograms::const_iterator it = subprogram_definitions.begin(); it != subprogram_definitions.end(); ++it)
				{
					SubProgram const& subprogram = *it;
					if(subprogram.IsExternal()) continue;
					file << "static ";
					if(subprogram.IsInline())
					{
						file << "inline ";
					}
					file << subprogram.BuildCDecl();
					const std::string& note = subprogram.GetNote();
					if(!note.empty())
					{
						file << " // " << note;
					}
					file << std::endl;
					file << "{" << std::endl;
					file << "}" << std::endl;
					file << std::endl;
				}

				if(gen_main)
				{
					file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
					file << "//                                   Main                                    //" << std::endl;
					file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
					file << std::endl;
					file << "int main(int argc, char *argv[])" << std::endl;
					file << "{" << std::endl;
					file << "\treturn 0;" << std::endl;
					file << "}" << std::endl;
				}
				
				return true;
			}
		};
		
		typedef std::vector<CompilationUnit *> CompilationUnits;
		CompilationUnits compilation_units;
		CompilationUnit *testbench;
		CompilationUnit *curr_comp_unit;
		
		unisim::util::debug::SymbolTable<MEMORY_ADDR> const *symbol_table;
		SubProgramDefinitionsByAddr subprogram_definitions_by_addr;
		VariableDefinitionsByAddr variable_definitions_by_addr;
		
		DebugInfoVisitor(const DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>& _dw_excavator, const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *_dw_handler)
			: dw_excavator(_dw_excavator)
			, dw_handler(_dw_handler)
			, binary(dw_excavator.binary)
			, output_dir(dw_excavator.output_dir)
			, verbose(dw_excavator.verbose)
			, sources(dw_excavator.sources)
			, suppress_types(dw_excavator.suppress_types)
			, suppress_functions(dw_excavator.suppress_functions)
			, compilation_units()
			, testbench(0) 
			, curr_comp_unit(0)
			, symbol_table(dw_handler->GetSymbolTable())
			, subprogram_definitions_by_addr()
			, variable_definitions_by_addr()
		{
		}
		
		virtual ~DebugInfoVisitor()
		{
			for(typename CompilationUnits::iterator it = compilation_units.begin(); it != compilation_units.end(); ++it)
			{
				CompilationUnit *comp_unit = *it;
				delete comp_unit;
			}
			
			if(testbench)
			{
				delete testbench;
			}
		}
			
		bool IsSuppressedType(const std::string& s) const
		{
			for(typename SuppressTypes::const_iterator it = suppress_types.begin(); it != suppress_types.end(); ++it)
			{
				const std::string& pattern = *it;
				if(Match(pattern.c_str(), s.c_str())) return true;
			}
			return false;
		}

		bool IsSuppressedFunction(const std::string& s) const
		{
			for(typename SuppressFunctions::const_iterator it = suppress_functions.begin(); it != suppress_functions.end(); ++it)
			{
				const std::string& pattern = *it;
				if(Match(pattern.c_str(), s.c_str())) return true;
			}
			return false;
		}

		void Insert(unisim::util::debug::Type const *type, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
		{
			curr_comp_unit->Insert(type, dw_die);
		}
		
		void Push(unisim::util::debug::Type const *type)
		{
			curr_comp_unit->Push(type);
		}
		
		void InsertDefinition(SubProgram const *subprogram)
		{
			if(subprogram->HasSymbol())
			{
				if(verbose >= VERBOSE_DEBUG)
				{
					std::cout << "Subprogram \"" << subprogram->GetName() << "\" is at 0x" << std::hex << subprogram->GetAddress() << std::dec << std::endl;
				}
				subprogram_definitions_by_addr.insert(typename SubProgramDefinitionsByAddr::value_type(subprogram->GetAddress(), subprogram));
			}
		}

		void InsertDefinition(Variable const *variable)
		{
			if(variable->HasSymbol())
			{
				if(verbose >= VERBOSE_DEBUG)
				{
					std::cout << "Variable \"" << variable->GetName() << "\" is at 0x" << std::hex << variable->GetAddress() << std::dec << std::endl;
				}
				variable_definitions_by_addr.insert(typename VariableDefinitionsByAddr::value_type(variable->GetAddress(), variable));
			}
		}
		
		void InsertDeclaration(unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
		{
			curr_comp_unit->InsertDeclaration(subprogram, symbol_table->FindSymbolByName(subprogram->GetName()), dw_die);
		}
		
		void InsertDefinition(unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
		{
			curr_comp_unit->InsertDefinition(subprogram, symbol_table->FindSymbolByName(subprogram->GetName()), dw_die);
		}
		
		void InsertDeclaration(unisim::util::debug::Variable const *variable, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
		{
			curr_comp_unit->InsertDeclaration(variable, symbol_table->FindSymbolByName(variable->GetName()), dw_die);
		}
		
		void InsertDefinition(unisim::util::debug::Variable const *variable, unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
		{
			curr_comp_unit->InsertDefinition(variable, symbol_table->FindSymbolByName(variable->GetName()), dw_die);
		}
		
		bool IsAutoVariable(unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die) const
		{
			if(dw_die->GetTag() == unisim::util::debug::dwarf::DW_TAG_variable)
			{
				while((dw_die = dw_die->GetParentDIE()))
				{
					uint16_t tag = dw_die->GetTag();
					switch(tag)
					{
						case unisim::util::debug::dwarf::DW_TAG_subprogram:
						case unisim::util::debug::dwarf::DW_TAG_inlined_subroutine:
						case unisim::util::debug::dwarf::DW_TAG_entry_point:
							return true;
					}
				}
			}
			return false;
		}
			
		bool Visit(unisim::util::debug::dwarf::DWARF_CompilationUnit<MEMORY_ADDR> const *dw_cu)
		{
			char const *dw_cu_name = dw_cu->GetName();
			if(sources.empty() || (dw_cu_name && (sources.find(dw_cu_name) != sources.end())))
			{
				if(verbose >= VERBOSE_INFO)
				{
					std::cout << "Processing Compilation Unit #" << dw_cu->GetId() << " (" << (dw_cu_name ? dw_cu_name : "unnamed") << ")" << std::endl;
				}
				curr_comp_unit = new CompilationUnit(*this, dw_cu);
				compilation_units.push_back(curr_comp_unit);
				dw_cu->Scan(*this);
				curr_comp_unit->SortTypes();
			}
			else
			{
				if(verbose >= VERBOSE_INFO)
				{
					std::cout << "Skipping Compilation Unit #" << dw_cu->GetId() << " (" << dw_cu_name << ")" << std::endl;
				}
			}
			return true;
		}
		
		bool Visit(unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
		{
			uint16_t dw_tag = dw_die->GetTag();
			switch(dw_tag)
			{
				case unisim::util::debug::dwarf::DW_TAG_subprogram:
				{
					char const *subprogram_name = dw_die->GetName();
					if(!subprogram_name && (verbose >= VERBOSE_WARNING))
					{
						std::cerr << "WARNING! In File \"" << binary << "\", Subprogram in DIE #" << dw_die->GetId() << " (" << dw_die->GetHREF() << ") has no name!" << std::endl;
					}
					if(subprogram_name && !IsSuppressedFunction(subprogram_name))
					{
						if(verbose >= VERBOSE_DEBUG)
						{
							std::cout << "Discovering Subprogram " << subprogram_name << std::endl;
						}
						unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram = dw_die->GetSubProgram(0);
						
						if(subprogram->IsDeclaration())
						{
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "Subprogram " << subprogram_name << " in DIE #" << dw_die->GetId() << " (" << dw_die->GetHREF() << ") is a declaration: looking for definition" << std::endl;
							}
							const unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram_def = dw_handler->FindSubProgramDIE(subprogram_name, 0);
							if(dw_die_subprogram_def)
							{
								unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram_def = dw_die_subprogram_def->GetSubProgram(0);
								if(verbose >= VERBOSE_DEBUG)
								{
									std::cout << "Found definition of " << subprogram_def->BuildCDecl() << " in DIE #" << dw_die_subprogram_def->GetId() << " (" << dw_die->GetHREF() << "): looking for dependencies" << std::endl;
								}
								InsertDeclaration(subprogram_def, dw_die_subprogram_def);
								dw_die_subprogram_def->ScanType(*this);
							}
							else
							{
								if(verbose >= VERBOSE_WARNING)
								{
									std::cerr << "WARNING! In File \"" << binary << "\", can't find definition of Subprogram " << subprogram_name << " needed by Compilation unit #" << curr_comp_unit->GetId() << " (" << curr_comp_unit->GetName() << ")" << std::endl;
								}
								InsertDeclaration(subprogram, dw_die); // Note: subprogram is probably in assembly code. We emit declaration even if it may be incomplete (void fn())
							}
						}
						else
						{
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "Subprogram " << subprogram->BuildCDecl() << " in DIE #" << dw_die->GetId() << " (" << dw_die->GetHREF() << ") is a definition: looking for dependencies" << std::endl;
							}
							InsertDefinition(subprogram, dw_die);
							dw_die->ScanType(*this);
						}
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_variable:
				{
					char const *variable_name = dw_die->GetName();
					if(!variable_name && (verbose >= VERBOSE_WARNING))
					{
						std::cerr << "WARNING! In File \"" << binary << "\", Variable in DIE #" << dw_die->GetId() << " (" << dw_die->GetHREF() << ") has no name!" << std::endl;
					}
					if(variable_name)
					{
						if(!IsAutoVariable(dw_die))
						{
							unisim::util::debug::Variable const *variable = dw_die->GetVariable(0);
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "Discovering Variable " << variable->BuildCDecl() << std::endl;
							}
							if(variable->IsDeclaration())
							{
								if(verbose >= VERBOSE_DEBUG)
								{
									std::cout << "Variable " << variable_name << " in DIE #" << dw_die->GetId() << " (" << dw_die->GetHREF() << ") is a declaration: looking for dependencies" << std::endl;
								}
								InsertDeclaration(variable, dw_die);
							}
							else
							{
								if(verbose >= VERBOSE_DEBUG)
								{
									std::cout << "Variable " << variable_name << " in DIE #" << dw_die->GetId() << " (" << dw_die->GetHREF() << ") is a definition: looking for dependencies" << std::endl;
								}
								InsertDefinition(variable, dw_die);
							}
						
							dw_die->ScanType(*this);
						}
					}
					break;
				}
			}
			
			dw_die->Scan(*this);
			return true;
		}
		
		bool VisitType(unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *dw_die)
		{
			if(curr_comp_unit->Visited(dw_die)) return true;
			
			uint16_t dw_tag = dw_die->GetTag();
			switch(dw_tag)
			{
				case unisim::util::debug::dwarf::DW_TAG_enumeration_type:
				{
					const char *enum_name = dw_die->GetName();
					if(enum_name && !IsSuppressedType(enum_name))
					{
						const unisim::util::debug::EnumType *enum_type = (const unisim::util::debug::EnumType *) dw_die->GetType(0);
						Insert(enum_type, dw_die);
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_structure_type:
				{
					const char *struct_name = dw_die->GetName();
					if(struct_name && !IsSuppressedType(struct_name))
					{
						const unisim::util::debug::StructureType *struct_type = (const unisim::util::debug::StructureType *) dw_die->GetType(0);
						Insert(struct_type, dw_die);
						dw_die->ScanType(*this);
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_union_type:
				{
					const char *union_name = dw_die->GetName();
					if(union_name && !IsSuppressedType(union_name))
					{
						const unisim::util::debug::UnionType *union_type = (const unisim::util::debug::UnionType *) dw_die->GetType(0);
						Insert(union_type, dw_die);
						dw_die->ScanType(*this);
					}
					break;
				}

				case unisim::util::debug::dwarf::DW_TAG_typedef:
				{
					const char *typedef_name = dw_die->GetName();
					if(typedef_name && !IsSuppressedType(typedef_name))
					{
						const unisim::util::debug::Typedef *_typedef = (const unisim::util::debug::Typedef *) dw_die->GetType(0);
						unisim::util::debug::Type const *type = _typedef->GetType();
						if(type->IsComposite())
						{
							unisim::util::debug::CompositeType const *comp_type = dynamic_cast<unisim::util::debug::CompositeType const *>(type);
							if(comp_type->IsIncomplete())
							{
								break;
							}
						}
						Insert(_typedef, dw_die);
						dw_die->ScanType(*this);
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_formal_parameter:
				case unisim::util::debug::dwarf::DW_TAG_member:
				case unisim::util::debug::dwarf::DW_TAG_pointer_type:
				case unisim::util::debug::dwarf::DW_TAG_const_type:
				case unisim::util::debug::dwarf::DW_TAG_volatile_type:
				case unisim::util::debug::dwarf::DW_TAG_array_type:
				case unisim::util::debug::dwarf::DW_TAG_subroutine_type:
				{
					dw_die->ScanType(*this);
					break;
				}
			}
			
			return true;
		}
		
		bool Visit(unisim::util::debug::Type const *type)
		{
			type->Scan(*this);
			return true;
		}

		bool Visit(unisim::util::debug::CompositeType const *comp_type)
		{
			comp_type->Scan(*this);
			if(comp_type->HasName() && !IsSuppressedType(comp_type->GetName()))
			{
				Push(comp_type);
			}
			return true;
		}
		
		bool Visit(unisim::util::debug::EnumType const *enum_type)
		{
			enum_type->Scan(*this);
			if(enum_type->HasName() && !IsSuppressedType(enum_type->GetName()))
			{
				Push(enum_type);
			}
			return true;
		}
		
		bool Visit(unisim::util::debug::Typedef const *_typedef)
		{
			_typedef->Scan(*this);
			if(_typedef->HasName() && !IsSuppressedType(_typedef->GetName()))
			{
				Push(_typedef);
			}
			return true;
		}
		
		bool Visit(unisim::util::debug::Member const *member)
		{
			member->Scan(*this);
			return true;
		}
		
		bool Visit(unisim::util::debug::FormalParameter const *formal_param)
		{
			formal_param->Scan(*this);
			return true;
		}
		
		bool Visit(unisim::util::debug::Enumerator const *enumerator)
		{
			return true;
		}
		
		bool Run()
		{
			if(verbose >= VERBOSE_INFO)
			{
				std::cout << "Scanning debug information" << std::endl;
			}
			dw_handler->Scan(*this);
			
			if(verbose >= VERBOSE_INFO)
			{
				std::cout << "Checking for multiple definitions of subprograms" << std::endl;
			}
			std::map<std::string, std::set<SubProgram const *> > subprogram_multiple_definitions;
			
			for(typename CompilationUnits::const_iterator comp_unit1_it = compilation_units.begin(); comp_unit1_it != compilation_units.end(); ++comp_unit1_it)
			{
				CompilationUnit const *comp_unit1 = *comp_unit1_it;
				for(typename SubPrograms::const_iterator comp_unit1_subprogram_def_it = comp_unit1->subprogram_definitions.begin(); comp_unit1_subprogram_def_it != comp_unit1->subprogram_definitions.end(); ++comp_unit1_subprogram_def_it)
				{
					SubProgram const& subprogram1 = *comp_unit1_subprogram_def_it;
					if(subprogram1.IsExternal() && !subprogram1.IsWeak())
					{
						for(typename CompilationUnits::const_iterator comp_unit2_it = compilation_units.begin(); comp_unit2_it != compilation_units.end(); ++comp_unit2_it)
						{
							if(comp_unit1_it != comp_unit2_it)
							{
								CompilationUnit const *comp_unit2 = *comp_unit2_it;
								
								typename SubPrograms::const_iterator comp_unit2_subprogram_def_it = comp_unit2->subprogram_definitions.find(subprogram1);
								if(comp_unit2_subprogram_def_it != comp_unit2->subprogram_definitions.end())
								{
									SubProgram const& subprogram2 = *comp_unit2_subprogram_def_it;
									if(!subprogram2.IsWeak())
									{
										subprogram_multiple_definitions[subprogram1.GetName()].insert(&subprogram1);
									}
								}
							}
						}
					}
				}
			}
			
			for(typename std::map<std::string, std::set<SubProgram const *> >::const_iterator it = subprogram_multiple_definitions.begin(); it != subprogram_multiple_definitions.end(); ++it)
			{
				const std::string& subprogram_name = (*it).first;
				if(verbose >= VERBOSE_WARNING)
				{
					std::cerr << "WARNING! multiple definition of Subprogram " << subprogram_name << std::endl;
				}
				
				const std::set<SubProgram const *>& subprogram_multiple_definitions2 = (*it).second;
				for(typename std::set<SubProgram const *>::const_iterator it2 = subprogram_multiple_definitions2.begin(); it2 != subprogram_multiple_definitions2.end(); ++it2)
				{
					SubProgram const *subprogram = *it2;
					if(verbose >= VERBOSE_WARNING)
					{
						std::cerr << "WARNING! " << subprogram_name << " is defined in Compilation unit #" << subprogram->GetCompilationUnit()->GetId() << " (" << subprogram->GetCompilationUnit()->GetName() << ")" << std::endl;
					}
					CompilationUnit *comp_unit = subprogram->GetCompilationUnit();
					if(!comp_unit->AnnotateDefinition(subprogram, "maybe defined more than once")) return false;
				}
			}

			if(verbose >= VERBOSE_INFO)
			{
				std::cout << "Checking for multiple definitions of variables" << std::endl;
			}
			std::map<std::string, std::set<Variable const *> > variable_multiple_definitions;
			
			for(typename CompilationUnits::const_iterator comp_unit1_it = compilation_units.begin(); comp_unit1_it != compilation_units.end(); ++comp_unit1_it)
			{
				CompilationUnit const *comp_unit1 = *comp_unit1_it;
				for(typename Variables::const_iterator comp_unit1_variable_def_it = comp_unit1->variable_definitions.begin(); comp_unit1_variable_def_it != comp_unit1->variable_definitions.end(); ++comp_unit1_variable_def_it)
				{
					Variable const& variable1 = *comp_unit1_variable_def_it;
					if(variable1.IsExternal())
					{
						for(typename CompilationUnits::const_iterator comp_unit2_it = compilation_units.begin(); comp_unit2_it != compilation_units.end(); ++comp_unit2_it)
						{
							if(comp_unit1_it != comp_unit2_it)
							{
								CompilationUnit const *comp_unit2 = *comp_unit2_it;
								
								typename Variables::const_iterator comp_unit2_variable_def_it = comp_unit2->variable_definitions.find(variable1);
								if(comp_unit2_variable_def_it != comp_unit2->variable_definitions.end())
								{
									variable_multiple_definitions[variable1.GetName()].insert(&variable1);
								}
							}
						}
					}
				}
			}

			for(typename std::map<std::string, std::set<Variable const *> >::const_iterator it = variable_multiple_definitions.begin(); it != variable_multiple_definitions.end(); ++it)
			{
				const std::string& variable_name = (*it).first;
				if(verbose >= VERBOSE_WARNING)
				{
					std::cerr << "WARNING! multiple definition of Variable " << variable_name << std::endl;
				}
				
				const std::set<Variable const *>& variable_multiple_definitions2 = (*it).second;
				for(typename std::set<Variable const *>::const_iterator it2 = variable_multiple_definitions2.begin(); it2 != variable_multiple_definitions2.end(); ++it2)
				{
					Variable const *variable = *it2;
					if(verbose >= VERBOSE_WARNING)
					{
						std::cerr << "WARNING! " << variable_name << " is defined in Compilation unit #" << variable->GetCompilationUnit()->GetId() << " (" << variable->GetCompilationUnit()->GetName() << ")" << std::endl;
					}
					CompilationUnit *comp_unit = variable->GetCompilationUnit();
					if(!comp_unit->AnnotateDefinition(variable, "maybe defined more than once")) return false;
				}
			}

			if(verbose >= VERBOSE_INFO)
			{
				std::cout << "Building a testbench skeleton" << std::endl;
			}
			curr_comp_unit = testbench = new CompilationUnit(*this, 0);
			
			std::set<SubProgram, SubProgramComparator> unresolved_subprogram_declarations;
			std::vector<SubProgram const *> comp_unit_unresolved_subprogram_declarations;
			
			for(typename CompilationUnits::const_iterator comp_unit1_it = compilation_units.begin(); comp_unit1_it != compilation_units.end(); ++comp_unit1_it)
			{
				CompilationUnit const *comp_unit1 = *comp_unit1_it;
				if(verbose >= VERBOSE_DEBUG)
				{
					std::cout << "Looking for Subprogram dependencies of Compilation unit #" << comp_unit1->GetId() << " (" << comp_unit1->GetName() << ")" << std::endl;
				}
				for(typename SubPrograms::const_iterator comp_unit_subprogram_decl_it = comp_unit1->subprogram_declarations.begin(); comp_unit_subprogram_decl_it != comp_unit1->subprogram_declarations.end(); ++comp_unit_subprogram_decl_it)
				{
					bool found = false;
					SubProgram const& subprogram = *comp_unit_subprogram_decl_it;
					for(typename CompilationUnits::const_iterator comp_unit2_it = compilation_units.begin(); comp_unit2_it != compilation_units.end(); ++comp_unit2_it)
					{
						CompilationUnit const *comp_unit2 = *comp_unit2_it;
						if(comp_unit2->subprogram_definitions.find(subprogram) != comp_unit2->subprogram_definitions.end())
						{
							found = true;
							break;
						}
					}
					
					if(!found)
					{
						if(unresolved_subprogram_declarations.find(subprogram) == unresolved_subprogram_declarations.end())
						{
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "Subprogram " << subprogram.BuildCDecl() << " may need stubbing" << std::endl;
							}
							const char *subprogram_name = subprogram.GetName();
							const unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = dw_handler->FindSubProgramDIE(subprogram_name, 0);
							if(dw_die_subprogram)
							{
								unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram_def = dw_die_subprogram->GetSubProgram(0);
								
								if(verbose >= VERBOSE_DEBUG)
								{
									std::cout << "Found definition of " << subprogram_def->BuildCDecl() << " in DIE #" << dw_die_subprogram->GetId() << " (" << dw_die_subprogram->GetHREF() << "): looking for dependencies" << std::endl;
								}
								testbench->InsertDefinition(subprogram_def, symbol_table->FindSymbolByName(subprogram_def->GetName()), dw_die_subprogram);
								dw_die_subprogram->ScanType(*this);
							}
							else
							{
								if(verbose >= VERBOSE_WARNING)
								{
									std::cerr << "WARNING! In File \"" << binary << "\", can't find definition of Subprogram " << subprogram_name << " needed by Compilation unit #" << subprogram.GetCompilationUnit()->GetId() << " (" << subprogram.GetCompilationUnit()->GetName() << ")" << std::endl;
								}
								comp_unit_unresolved_subprogram_declarations.push_back(&subprogram);
								unresolved_subprogram_declarations.insert(subprogram);
							}
						}
						else
						{
							comp_unit_unresolved_subprogram_declarations.push_back(&subprogram);
						}
					}
				}
			}
			
			for(typename CompilationUnits::const_iterator comp_unit_it = compilation_units.begin(); comp_unit_it != compilation_units.end(); ++comp_unit_it)
			{
				CompilationUnit const *comp_unit = *comp_unit_it;
				if(verbose >= VERBOSE_DEBUG)
				{
					std::cout << "Looking for external subprogram dependencies of Testbench regarding Compilation unit \"" << comp_unit->GetName() << "\"" << std::endl;
				}
				const char *comp_unit_name = comp_unit->GetName();
				for(typename SubPrograms::const_iterator comp_unit_subprogram_def_it = comp_unit->subprogram_definitions.begin(); comp_unit_subprogram_def_it != comp_unit->subprogram_definitions.end(); ++comp_unit_subprogram_def_it)
				{
					SubProgram const& subprogram = *comp_unit_subprogram_def_it;
					
					if(subprogram.IsExternal())
					{
						if(verbose >= VERBOSE_DEBUG)
						{
							std::cout << "Testbench may use " << subprogram.BuildCDecl();
							if(comp_unit_name)
							{
								std::cout << " provided by " << comp_unit_name;
							}
							std::cout << std::endl;
						}
						const char *subprogram_name = subprogram.GetName();
						const unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = dw_handler->FindSubProgramDIE(subprogram_name, 0);
						if(dw_die_subprogram)
						{
							unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram_def = dw_die_subprogram->GetSubProgram(0);
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "Found definition of " << subprogram_def->BuildCDecl() << " in DIE #" << dw_die_subprogram->GetId() << " (" << dw_die_subprogram->GetHREF() << "): looking for dependencies" << std::endl;
							}
							testbench->InsertDeclaration(subprogram_def, symbol_table->FindSymbolByName(subprogram_def->GetName()), dw_die_subprogram);
							dw_die_subprogram->ScanType(*this);
						}
						else if(verbose >= VERBOSE_WARNING)
						{
							std::cerr << "WARNING! In File \"" << binary << "\", can't find definition of Subprogram " << subprogram_name << " needed by Compilation unit #" << subprogram.GetCompilationUnit()->GetId() << " (" << subprogram.GetCompilationUnit()->GetName() << ")" << std::endl;
						}
					}
				}
			}
			
			std::set<Variable, VariableComparator> unresolved_variable_declarations;
			std::vector<Variable const *> comp_unit_unresolved_variable_declarations;
			
			for(typename CompilationUnits::const_iterator comp_unit1_it = compilation_units.begin(); comp_unit1_it != compilation_units.end(); ++comp_unit1_it)
			{
				CompilationUnit const *comp_unit1 = *comp_unit1_it;
				if(verbose >= VERBOSE_DEBUG)
				{
					std::cout << "Looking for external variable dependencies of Compilation unit #" << comp_unit1->GetId() << " (" << comp_unit1->GetName() << ")" << std::endl;
				}
				for(typename Variables::const_iterator comp_unit_variable_decl_it = comp_unit1->variable_declarations.begin(); comp_unit_variable_decl_it != comp_unit1->variable_declarations.end(); ++comp_unit_variable_decl_it)
				{
					bool found = false;
					Variable const& variable = *comp_unit_variable_decl_it;
					for(typename CompilationUnits::const_iterator comp_unit2_it = compilation_units.begin(); comp_unit2_it != compilation_units.end(); ++comp_unit2_it)
					{
						CompilationUnit const *comp_unit2 = *comp_unit2_it;
						if(comp_unit2->variable_definitions.find(variable) != comp_unit2->variable_definitions.end())
						{
							found = true;
							break;
						}
					}
					
					if(!found)
					{
						if(unresolved_variable_declarations.find(variable) == unresolved_variable_declarations.end())
						{
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "Variable " << variable.BuildCDecl() << " may need stubbing" << std::endl;
							}
							const char *variable_name = variable.GetName();
							const unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> *dw_die_variable_def = dw_handler->FindVariableDIE(variable_name, 0);
							
							if(dw_die_variable_def)
							{
								unisim::util::debug::Variable const *variable_def = dw_die_variable_def->GetVariable(0);
								
								if(verbose >= VERBOSE_DEBUG)
								{
									std::cout << "Found definition of " << variable_def->BuildCDecl() << " in DIE #" << dw_die_variable_def->GetId() << " (" << dw_die_variable_def->GetHREF() << "): looking for dependencies" << std::endl;
								}
								testbench->InsertDefinition(variable_def, symbol_table->FindSymbolByName(variable_def->GetName()), dw_die_variable_def);
								dw_die_variable_def->ScanType(*this);
							}
							else
							{
								if(verbose >= VERBOSE_WARNING)
								{
									std::cerr << "WARNING! In File \"" << binary << "\", can't find definition of Variable " << variable_name << " needed by Compilation unit #" << variable.GetCompilationUnit()->GetId() << " (" << variable.GetCompilationUnit()->GetName() << ")" << std::endl;
								}
								comp_unit_unresolved_variable_declarations.push_back(&variable);
								unresolved_variable_declarations.insert(variable);
							}
						}
						else
						{
							comp_unit_unresolved_variable_declarations.push_back(&variable);
						}
					}
				}
			}
			
			for(typename std::vector<SubProgram const *>::iterator it = comp_unit_unresolved_subprogram_declarations.begin(); it != comp_unit_unresolved_subprogram_declarations.end(); ++it)
			{
				SubProgram const *unresolved_subprogram = *it;
				CompilationUnit *comp_unit = unresolved_subprogram->GetCompilationUnit();
				if(unresolved_subprogram->HasSymbol())
				{
					std::pair<typename SubProgramDefinitionsByAddr::const_iterator, typename SubProgramDefinitionsByAddr::const_iterator> range = subprogram_definitions_by_addr.equal_range(unresolved_subprogram->GetAddress());
					if(range.first != range.second)
					{
						for(typename SubProgramDefinitionsByAddr::const_iterator it = range.first; it != range.second; ++it)
						{
							SubProgram const *subprogram_def = (*it).second;
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "SubProgram " << unresolved_subprogram->BuildCDecl() << " may be an alias of " << subprogram_def->BuildCDecl() << std::endl;
							}
							std::stringstream note_sstr;
							note_sstr << "at &" << subprogram_def->GetName() << "+0x" << std::hex << (unresolved_subprogram->GetAddress() - subprogram_def->GetAddress()) << " but subprogram definition is missing in source code: see linker script";
							if(!comp_unit->AnnotateDeclaration(unresolved_subprogram, note_sstr.str())) return false;
						}
					}
					else
					{
						if(verbose >= VERBOSE_DEBUG)
						{
							std::cout << "Subprogram definition of " << unresolved_subprogram->BuildCDecl() << " is missing" << std::endl;
						}
						
						unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol = symbol_table->FindSymbolByName(unresolved_subprogram->GetName());
						if(symbol)
						{
							std::stringstream note_sstr;
							note_sstr << "at 0x" << std::hex << symbol->GetAddress() << " but definition is missing in binary: see linker script or assembly";
							if(!comp_unit->AnnotateDeclaration(unresolved_subprogram, note_sstr.str())) return false;
						}
						else
						{
							std::stringstream note_sstr;
							note_sstr << "definition is missing in binary: see linker script";
							if(!comp_unit->AnnotateDeclaration(unresolved_subprogram, note_sstr.str())) return false;
						}
					}
				}
				else
				{
					std::stringstream note_sstr;
					note_sstr << "definition is missing in source code and symbol is undefined: see linker script";
					if(!comp_unit->AnnotateDeclaration(unresolved_subprogram, note_sstr.str())) return false;
				}
			}

			for(typename std::vector<Variable const *>::iterator it = comp_unit_unresolved_variable_declarations.begin(); it != comp_unit_unresolved_variable_declarations.end(); ++it)
			{
				Variable const *unresolved_variable = *it;
				CompilationUnit *comp_unit = unresolved_variable->GetCompilationUnit();
				if(unresolved_variable->HasSymbol())
				{
					std::pair<typename VariableDefinitionsByAddr::const_iterator, typename VariableDefinitionsByAddr::const_iterator> range = variable_definitions_by_addr.equal_range(unresolved_variable->GetAddress());
					if(range.first != range.second)
					{
						for(typename VariableDefinitionsByAddr::const_iterator it = range.first; it != range.second; ++it)
						{
							Variable const *variable_def = (*it).second;
							if(verbose >= VERBOSE_DEBUG)
							{
								std::cout << "Variable " << unresolved_variable->BuildCDecl() << " may be an alias of " << variable_def->BuildCDecl() << std::endl;
							}
							std::stringstream note_sstr;
							note_sstr << "at &" << variable_def->GetName() << "+0x" << std::hex << (unresolved_variable->GetAddress() - variable_def->GetAddress()) << " but variable definition is missing in source code: see linker script";
							if(!comp_unit->AnnotateDeclaration(unresolved_variable, note_sstr.str())) return false;
						}
					}
					else
					{
						if(verbose >= VERBOSE_DEBUG)
						{
							std::cout << "Variable definition of " << unresolved_variable->BuildCDecl() << " is missing" << std::endl;
						}
						
						unisim::util::debug::Symbol<MEMORY_ADDR> const *symbol = symbol_table->FindSymbolByName(unresolved_variable->GetName());
						if(symbol)
						{
							std::stringstream note_sstr;
							note_sstr << "at 0x" << std::hex << symbol->GetAddress() << " but definition is missing in source code: see linker script or assembly";
							if(!comp_unit->AnnotateDeclaration(unresolved_variable, note_sstr.str())) return false;
						}
						else
						{
							std::stringstream note_sstr;
							note_sstr << "definition is missing in source code: see linker script";
							if(!comp_unit->AnnotateDeclaration(unresolved_variable, note_sstr.str())) return false;
						}
					}
				}
				else
				{
					std::stringstream note_sstr;
					note_sstr << "definition is missing in source code and symbol is undefined: see linker script";
					if(!comp_unit->AnnotateDeclaration(unresolved_variable, note_sstr.str())) return false;
				}
			}
			
			testbench->SortTypes();
			
			return true;
		}
		
		bool Generate()
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			mkdir(output_dir.c_str());
#else
			mkdir(output_dir.c_str(), S_IRWXU);
#endif
			
			for(typename CompilationUnits::iterator it = compilation_units.begin(); it != compilation_units.end(); ++it)
			{
				CompilationUnit *comp_unit = *it;
				
				std::stringstream output_filename_sstr;
				output_filename_sstr << output_dir << '/' << comp_unit->dw_cu->GetId() << ".c";
				std::string output_filename(output_filename_sstr.str());
				if(!comp_unit->Generate(output_filename, false)) return false;
			}
			if(testbench)
			{
				if(!testbench->Generate(output_dir + "/testbench.c", true)) return false;
			}
			return true;
		}
	};

	DebugInfoVisitor debug_info_visitor(*this, dw_handler);
	if(!debug_info_visitor.Run()) return false;
	if(!debug_info_visitor.Generate()) return false;
	
	return true;
}

///////////////////////////// JSON_ConfigReader ///////////////////////////////

JSON_ConfigReader::JSON_ConfigReader(std::ostream& _err)
	: err(_err)
	, verbose(VERBOSE_ERROR)
	, binary()
	, output_dir()
	, sources()
	, suppress_types()
	, suppress_functions()
	, dwarf_html_output_dir()
{
	SetOption(unisim::util::json::OPT_ENABLE_C_COMMENTS, true);
	SetOption(unisim::util::json::OPT_ENABLE_CPP_COMMENTS, true);
}

bool JSON_ConfigReader::ParseRoot(std::istream& stream)
{
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0) return false;
	
	if(token != unisim::util::json::TOK_LEFT_BRACE)
	{
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_LEFT_BRACE));
		return false;
	}

	token = Lexer::Next(stream, *this);
	if(token < 0) return false;

	if(token == unisim::util::json::TOK_RIGHT_BRACE)
	{
		return true;
	}

	Lexer::Back();

	if(!ParseRootMembers(stream))
	{
		Lexer::ParseError(stream, *this, std::string("expecting root object members"));
		return false;
	}
	
	token = Lexer::Next(stream, *this);
	
	if(token != unisim::util::json::TOK_RIGHT_BRACE)
	{
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_COMMA) + " or " + ToString(unisim::util::json::TOK_RIGHT_BRACE));
		return false;
	}
	
	return true;
}

bool JSON_ConfigReader::ParseRootMembers(std::istream& stream)
{
	if(!ParseRootMember(stream))
	{
		Lexer::ParseError(stream, *this, std::string("expecting a root object member"));
		return false;
	}
	
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0)
	{
		return false;
	}
	
	if(token == unisim::util::json::TOK_COMMA)
	{
		return ParseRootMembers(stream);
	}

	Lexer::Back();
	
	return true;
}

bool JSON_ConfigReader::ParseRootMember(std::istream& stream)
{
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0) return false;
	
	if(token != unisim::util::json::TOK_STRING)
	{
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_STRING));
		return false;
	}
	
	const std::string& member_name = Lexer::GetStringValue();
	MemberType member_type = ( member_name == "verbose"              ) ? VERBOSE            :
	                         ((member_name == "binary"               ) ? BINARY             :
	                         ((member_name == "sources"              ) ? SOURCES            :
	                         ((member_name == "suppress-types"       ) ? SUPPRESS_TYPES     :
	                         ((member_name == "suppress-functions"   ) ? SUPPRESS_FUNCTIONS :
	                         ((member_name == "output-dir"           ) ? OUTPUT_DIR         :
	                         ((member_name == "dwarf-html-output-dir") ? DWARF_HTML_OUTPUT_DIR : UNKNOWN))))));
	
	if(member_type == UNKNOWN)
	{
		Lexer::ParseError(stream, *this, std::string("expecting a root object member name"));
		return false;
	}
	
	token = Lexer::Next(stream, *this);
	if(token < 0)
	{
		return false;
	}
	
	if(token != unisim::util::json::TOK_COLON)
	{
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_COLON));
		return false;
	}
	
	switch(member_type)
	{
		case UNKNOWN:
			std::cerr << "ERROR! JSON Parser internal error" << std::endl;
			return false;
		case VERBOSE:
		{
			uint64_t uint_value = 0;
			if(!ParseUnsignedInteger(stream, uint_value))
			{
				Lexer::ParseError(stream, *this, std::string("expecting an unsigned integer value for Member \"verbose\""));
				return false;
			}
			switch(uint_value)
			{
				case VERBOSE_ERROR:
				case VERBOSE_WARNING:
				case VERBOSE_INFO:
				case VERBOSE_DEBUG:
					verbose = Verbose(uint_value);
					break;
				default:
				{
					std::stringstream msg_sstr;
					msg_sstr << "expecting " << +VERBOSE_ERROR << ", " << +VERBOSE_WARNING << ", " << +VERBOSE_INFO << ", or " << +VERBOSE_DEBUG << " for Member \"verbose\"";
					Lexer::ParseError(stream, *this, msg_sstr.str());
					return false;
				}
			}
			break;
		}
		case BINARY:
			if(!ParseString(stream, binary))
			{
				Lexer::ParseError(stream, *this, std::string("expecting a string value for Member \"binary\""));
				return false;
			}
			break;
		case OUTPUT_DIR:
			if(!ParseString(stream, output_dir))
			{
				Lexer::ParseError(stream, *this, std::string("expecting a string value for Member \"output-dir\""));
				return false;
			}
			break;
		case SOURCES:
			if(!ParseStringArray(stream, sources))
			{
				Lexer::ParseError(stream, *this, std::string("expecting an array of strings for Member \"sources\""));
				return false;
			}
			break;
		case SUPPRESS_TYPES:
			if(!ParseStringArray(stream, suppress_types))
			{
				Lexer::ParseError(stream, *this, std::string("expecting an array of strings for Member \"suppress-types\""));
				return false;
			}
			break;
		case SUPPRESS_FUNCTIONS:
			if(!ParseStringArray(stream, suppress_functions))
			{
				Lexer::ParseError(stream, *this, std::string("expecting an array of strings for Member \"suppress-functions\""));
				return false;
			}
			break;
		case DWARF_HTML_OUTPUT_DIR:
			if(!ParseString(stream, dwarf_html_output_dir))
			{
				Lexer::ParseError(stream, *this, std::string("expecting a string for Member \"dwarf-html-output-dir\""));
				return false;
			}
	}
	
	return true;
}

bool JSON_ConfigReader::ParseBool(std::istream& stream, bool& value)
{
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0) return false;
	
	if(token == unisim::util::json::TOK_TRUE)
	{
		value = true;
		return true;
	}
	
	if(token == unisim::util::json::TOK_FALSE)
	{
		value = false;
		return true;
	}
	
	Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_TRUE) + " or " +  ToString(unisim::util::json::TOK_FALSE));
	return false;
}

bool JSON_ConfigReader::ParseUnsignedInteger(std::istream& stream, uint64_t& value)
{
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0) return false;
	
	if(token == unisim::util::json::TOK_UINT)
	{
		value = Lexer::GetUIntValue();
		return true;
	}
	
	Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_UINT));
	return false;
}

bool JSON_ConfigReader::ParseString(std::istream& stream, std::string& value)
{
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0) return false;
	
	if(token == unisim::util::json::TOK_STRING)
	{
		value = Lexer::GetStringValue();
		return true;
	}
	
	Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_STRING));
	return false;
}

template <typename CONTAINER>
bool JSON_ConfigReader::ParseStringArray(std::istream& stream, CONTAINER& strings)
{
	strings.clear();
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0) return false;
	
	if(token != unisim::util::json::TOK_LEFT_BRACKET)
	{
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_LEFT_BRACKET));
		return false;
	}
	
	token = Lexer::Next(stream, *this);
	if(token < 0) return false;
	
	if(token == unisim::util::json::TOK_RIGHT_BRACKET)
	{
		return true;
	}
	
	Lexer::Back();
	
	if(!ParseStringArrayElements(stream, strings))
	{
		return false;
	}
	
	token = Lexer::Next(stream, *this);
	
	if(token != unisim::util::json::TOK_RIGHT_BRACKET)
	{
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_COMMA) + " or " + ToString(unisim::util::json::TOK_RIGHT_BRACKET));
		return false;
	}
	
	return true;
}

template <typename CONTAINER>
bool JSON_ConfigReader::ParseStringArrayElements(std::istream& stream, CONTAINER& strings)
{
	std::string value;
	if(!ParseString(stream, value))
	{
		return false;
	}
	
	strings.insert(strings.end(), value);
	
	unisim::util::json::Token token = Lexer::Next(stream, *this);
	if(token < 0)
	{
		return false;
	}
	
	if(token == unisim::util::json::TOK_COMMA)
	{
		return ParseStringArrayElements(stream, strings);
	}
	
	Lexer::Back();
	
	return true;
}

//////////////////////////////////// Help /////////////////////////////////////

void Help(std::ostream& stream)
{
	stream << PACKAGE_STRING << ": A tool to dig DWARF debug information in an ELF executable binary, processing each compilation unit, and generating one reversed engineered C sources code for each processed compilation unit (intended for understanding the source code dependencies), and the C source of a testbench skeleton (intended for later testing the real compilation unit source code)." << std::endl;
	stream << std::endl;
	stream << "Usage: " << PACKAGE << '-' << PACKAGE_VERSION << " <JSON configuration file>" << std::endl;
	stream << std::endl;
	stream << "Options:" << std::endl;
	stream << "\t--help     display this help and exit" << std::endl;
	stream << "\t--version  display version information and exit" << std::endl;
	stream << std::endl;
	stream << "Examples:" << std::endl;
    stream << "Below is a JSON Configuration file sample" << std::endl;
	stream << std::endl;
	stream << "{" << std::endl;
	stream << "\t\"binary\" : \"vmlinux\"," << std::endl;
	stream << std::endl;
	stream << "\t\"sources\" : [ \"drivers/watchdog/at91sam9_wdt.c\" ]," << std::endl;
	stream << std::endl;
	stream << "\t\"suppress-types\" : [ \"__builtin_*\" ]," << std::endl;
	stream << std::endl;
	stream << "\t\"suppress-functions\" : [ \"__compiletime_assert_*\", \"__builtin_*\", \"snprintf\", \"sprintf\", \"sscanf\", \"strcspn\", \"strncasecmp\", \"strncat\", \"vsnprintf\"," << std::endl;
	stream << "\t                         \"strspn\", \"bcmp\", \"fabs\", \"strlen\", \"strncmp\", \"strncpy\", \"memcmp\", \"memmove\", \"strchr\", \"memset\", \"strrchr\", \"memchr\", \"abort\" ]," << std::endl;
	stream << std::endl;
	stream << "\t\"output-dir\" : \"out\"," << std::endl;
	stream << std::endl;
	stream << "\t// uncomment lines below for more details" << std::endl;
	stream << std::endl;
	stream << "\t/*" << std::endl;
	stream << "\t\"verbose\" : " << +VERBOSE_INFO << "," << std::endl;
	stream << std::endl;
	stream << "\t\"dwarf-html-output-dir\" : \"dwarf\"" << std::endl;
	stream << "\t*/" << std::endl;
	stream << "}" << std::endl;
	stream << std::endl;
	stream << "\"binary\" is the input ELF executable binary (with strict DWARF4 debug information, e.g. compiled with gcc -gdwarf-4 -gstrict-dwarf) to analyze (*mandatory option*)." << std::endl;
	stream << "\"sources\" is the list of compilation units to process; if not specified or empty, it means all compilation units." << std::endl;
	stream << "\"suppress-types\" is the list of types to suppress while analysis, e.g. types to be considered as compiler builtins." << std::endl;
	stream << "\"suppress-functions\" is the list of functions to suppress while analysis, e.g. functions to be considered as compiler builtins." << std::endl;
	stream << "\"output-dir\" is the output directory of the analysis (*mandatory option*); the result of the analysis is a reversed engineered C source codes." << std::endl;
	stream << "\"verbose\" is the level of verbosity during analysis (" << +VERBOSE_ERROR << ":errors, " << +VERBOSE_WARNING << ":warnings, " << +VERBOSE_INFO << ":info, " << +VERBOSE_DEBUG<< ":debug)." << std::endl;
	stream << "\"dwarf-html-output-dir\" is the directory where to dump DWARF debugging information as HTML; if not specified or empty, it means no dump." << std::endl;
}

void Version(std::ostream& stream)
{
	stream << PACKAGE_STRING << std::endl << std::endl;
	stream << "Copyright (C) 2020, Commissariat a l'Energie Atomique (CEA)" << std::endl << std::endl;
	stream << "License: BSD (see file COPYING)" << std::endl << std::endl;
	stream << "Written by Gilles Mouchard <gilles.mouchard@cea.fr>, Franck Vedrine <franck.vedrine@cea.fr> and Yves Lhuillier <yves.lhuillier@cea.fr>" << std::endl << std::endl;
	stream << "Report bugs to " << PACKAGE_BUGREPORT << std::endl;
	stream << std::endl;
}

//////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		Help(std::cerr);
		return -1;
	}
	
	if(strcmp(argv[1], "--help") == 0)
	{
		Help(std::cout);
		return 0;
	}

	if(strcmp(argv[1], "--version") == 0)
	{
		Version(std::cout);
		return 0;
	}
	
	char const *json_config_filename = argv[1];
	
	// Opening JSON configuration file
	std::ifstream json_config_file(json_config_filename, std::ifstream::in | std::ifstream::binary);
	
	if(json_config_file.fail())
	{
		std::cerr << "ERROR! Can't open JSON configuration File \"" << json_config_filename << "\"" << std::endl;
		return -1;
	}

	// Parsing JSON configuration file
	JSON_ConfigReader json_config_reader;
	
	if(!json_config_reader.Parse(json_config_file))
	{
		std::cerr << "ERROR! Parse error in JSON configuration File \"" << json_config_filename << "\"" << std::endl;
		std::cerr << std::endl;
		Help(std::cerr);
		return -1;
	}
	
	// Checking that a binary filename was specified in JSON configuration file
	const std::string& binary_filename = json_config_reader.GetBinary();
	
	if(binary_filename.empty())
	{
		std::cerr << "ERROR! No binary specified in JSON configuration File \"" << json_config_filename << "\"" << std::endl;
		Help(std::cerr);
		return -1;
	}
	
	// Checking that an output directory was specified in JSON configuration file
	const std::string& output_dir = json_config_reader.GetOutputDir();
	
	if(output_dir.empty())
	{
		std::cerr << "ERROR! No output directory specified in JSON configuration File \"" << json_config_filename << "\"" << std::endl;
		Help(std::cerr);
		return -1;
	}
	
	// Reading 5 first bytes (magic number) to determine if file is an ELF32 or an ELF64 file
	std::ifstream file(binary_filename.c_str(), std::ifstream::in | std::ifstream::binary);
	
	if(file.fail())
	{
		std::cerr << "ERROR! Can't open binary File \"" << binary_filename << "\"" << std::endl;
		return -1;
	}

	uint8_t magic[5];
	unsigned int size;
	for(size = 0; size < sizeof(magic); size++)
	{
		file.read((char *) &magic[size], 1);
		if(!file.good()) break;
	}
	
	int status = 0;
	
	if(size >= 5)
	{
		if((magic[0] == 0x7f) && (magic[1] == 'E') && (magic[2] == 'L') && (magic[3] == 'F'))
		{
			if(magic[4] == 1)
			{
				// Binary file is an ELF32 file
				unisim::util::loader::elf_loader::Elf32Loader<uint32_t> elf32_loader;
				elf32_loader.SetFileName(binary_filename.c_str());
				elf32_loader.SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, json_config_reader.GetVerbose() >= VERBOSE_INFO);
				elf32_loader.SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, true);
				elf32_loader.SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY, json_config_reader.GetDWARF_HTML_OutputDir().c_str());
				
				// Load binary file
				if(elf32_loader.Load())
				{
					// Dig in debug information
					const unisim::util::debug::dwarf::DWARF_Handler<uint32_t> *dw_handler32 = elf32_loader.GetDWARFHandler();
					if(!dw_handler32)
					{
						std::cerr << "INTERNAL ERROR! can't get DWARF Handler" << std::endl;
						return -1;
					}
					if(!dw_handler32->HasDebugInfo() && (json_config_reader.GetVerbose() >= VERBOSE_WARNING))
					{
						std::cerr << "WARNING! ELF32 Binary file \"" << binary_filename << "\" does not contain any debugging information" << std::endl;
					}
					DWARF_Excavator<uint32_t, JSON_ConfigReader> dw_excavator32(dw_handler32, json_config_reader);
					status = !dw_excavator32.Dig();
				}
				else
				{
					std::cerr << "Loading ELF32 File \"" << binary_filename << "\" failed" << std::endl;
					return -1;
				}
			}
			else if(magic[4] == 2)
			{
				// Binary file is an ELF64 file
				unisim::util::loader::elf_loader::Elf64Loader<uint64_t> elf64_loader;
				elf64_loader.SetFileName(binary_filename.c_str());
				elf64_loader.SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, json_config_reader.GetVerbose() >= VERBOSE_INFO);
				elf64_loader.SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, true);
				elf64_loader.SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY, json_config_reader.GetDWARF_HTML_OutputDir().c_str());
				
				// Load binary file
				if(elf64_loader.Load())
				{
					// Dig in debug information
					const unisim::util::debug::dwarf::DWARF_Handler<uint64_t> *dw_handler64 = elf64_loader.GetDWARFHandler();
					if(!dw_handler64)
					{
						std::cerr << "INTERNAL ERROR! can't get DWARF Handler" << std::endl;
						return -1;
					}
					if(!dw_handler64->HasDebugInfo() && (json_config_reader.GetVerbose() >= VERBOSE_WARNING))
					{
						std::cerr << "WARNING! ELF64 Binary file \"" << binary_filename << "\" does not contain any debugging information" << std::endl;
					}
					DWARF_Excavator<uint64_t, JSON_ConfigReader> dw_excavator64(dw_handler64, json_config_reader);
					status = !dw_excavator64.Dig();
				}
				else
				{
					std::cerr << "Loading ELF64 File \"" << binary_filename << "\" failed" << std::endl;
					return -1;
				}
			}
			else
			{
				// Binary file is not an ELF file
				std::cerr << "ERROR! ELF File \"" << binary_filename << "\" is neither ELF32 or ELF64" << std::endl;
				return -1;
			}
		}
	}
	else
	{
		// Binary file is not an ELF file (less than 5 bytes long)
		std::cerr << "ERROR! File \"" << binary_filename << "\" is not an ELF file" << std::endl;
		return -1;
	}
	
	if(json_config_reader.GetVerbose() >= VERBOSE_INFO)
	{
		std::cout << "Analysis has " << (status ? "failed" : "finished successfully") << std::endl;
	}
	return status;
}
