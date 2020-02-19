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

///////////////////////////////////////////////////////////////////////////////
//                              Declarations                                 //
///////////////////////////////////////////////////////////////////////////////

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
	const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler;
	bool verbose;
	const typename CONFIG_READER::Sources& sources;
	const typename CONFIG_READER::BuiltinFunctions& builtin_functions;
	const std::string& output;
};

///////////////////////////// JSON_ConfigReader ///////////////////////////////

struct JSON_ConfigReader : unisim::util::json::JSON_Lexer<JSON_ConfigReader>
{
	typedef unisim::util::json::JSON_Lexer<JSON_ConfigReader> Lexer;
	typedef std::set<std::string> Sources;
	typedef std::set<std::string> BuiltinFunctions;
	
	JSON_ConfigReader(std::ostream& err = std::cerr);
	
	bool Parse(std::istream& stream) { return ParseRoot(stream); }

	bool GetVerbose() const { return verbose; }
	const std::string& GetBinary() const { return binary; }
	const std::string& GetOutput() const { return output; }
	const Sources& GetSources() const { return sources; }
	const BuiltinFunctions& GetBuiltinFunctions() const { return builtin_functions; }
	const std::string& GetDWARF_HTML_OutputDir() const { return dwarf_html_output_dir; }
	
	void Error(const std::string& msg) { err << msg; }
	
private:
	typedef enum { UNKNOWN, VERBOSE, BINARY, SOURCES, BUILTIN_FUNCTIONS, OUTPUT, DWARF_HTML_OUTPUT_DIR } MemberType;
	std::ostream& err;
	bool verbose;
	std::string binary;
	std::string output;
	Sources sources;
	BuiltinFunctions builtin_functions;
	std::string dwarf_html_output_dir;
	
	bool ParseRoot(std::istream& stream);
	bool ParseRootMembers(std::istream& stream);
	bool ParseRootMember(std::istream& stream);
	bool ParseBool(std::istream& stream, bool& value);
	bool ParseString(std::istream& stream, std::string& value);
	bool ParseStringArray(std::istream& stream, std::set<std::string>& strings);
	bool ParseStringArrayElements(std::istream& stream, std::set<std::string>& strings);
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
	
	// Type comparator
	struct TypeComparator
	{
		bool operator () (unisim::util::debug::Type const * const& t1, unisim::util::debug::Type const * const& t2) const
		{
			return reorder_qualifiers(t1->BuildCDecl()) < reorder_qualifiers(t2->BuildCDecl());
		}
	};

	// Subprogram comparator
	template <typename MEMORY_ADDR>
	struct SubProgramComparator
	{
		bool operator () (unisim::util::debug::SubProgram<MEMORY_ADDR> const * const& sp1, unisim::util::debug::SubProgram<MEMORY_ADDR> const * const& sp2) const
		{
			return reorder_qualifiers(sp1->BuildCDecl()) < reorder_qualifiers(sp2->BuildCDecl());
		}
	};

	// Variable comparator
	struct VariableComparator
	{
		bool operator () (unisim::util::debug::Variable const * const& v1, unisim::util::debug::Variable const * const& v2) const
		{
			return reorder_qualifiers(v1->BuildCDecl()) < reorder_qualifiers(v2->BuildCDecl());
		}
	};
}

///////////////////////////////////////////////////////////////////////////////
//                              Definitions                                  //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// DWARF_Excavator<> ///////////////////////////////

template <typename MEMORY_ADDR, typename CONFIG_READER>
DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>::DWARF_Excavator(const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *_dw_handler, const CONFIG_READER& config_reader)
	: dw_handler(_dw_handler)
	, verbose(config_reader.GetVerbose())
	, sources(config_reader.GetSources())
	, builtin_functions(config_reader.GetBuiltinFunctions())
	, output(config_reader.GetOutput())
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
		const DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>& dw_excavator;
		const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler;
		bool verbose;
		const typename CONFIG_READER::Sources& sources;
		const typename CONFIG_READER::BuiltinFunctions& builtin_functions;

		typedef std::set<unisim::util::debug::Type const *, TypeComparator> Types;
		Types types;
		
		std::set<unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> const *> visited;
		std::set<unisim::util::debug::Type const *, TypeComparator> pushed;
		
		typedef std::vector<unisim::util::debug::Type const *> Stack;
		Stack stack;
		
		typedef std::set<unisim::util::debug::SubProgram<MEMORY_ADDR> const *, SubProgramComparator<MEMORY_ADDR> > SubPrograms;
		SubPrograms subprogram_declarations;
		SubPrograms subprogram_definitions;
		
		typedef std::set<unisim::util::debug::Variable const *, VariableComparator> Variables;
		Variables variable_declarations;
		Variables variable_definitions;
		
		DebugInfoVisitor(const DWARF_Excavator<MEMORY_ADDR, CONFIG_READER>& _dw_excavator, const unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *_dw_handler)
			: dw_excavator(_dw_excavator)
			, dw_handler(_dw_handler)
			, verbose(dw_excavator.verbose)
			, sources(dw_excavator.sources)
			, builtin_functions(dw_excavator.builtin_functions)
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
		
		~DebugInfoVisitor()
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
			
			for(typename SubPrograms::const_iterator it = subprogram_declarations.begin(); it != subprogram_declarations.end(); ++it)
			{
				unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram = *it;
				subprogram->Release();
			}
			
			for(typename SubPrograms::const_iterator it = subprogram_definitions.begin(); it != subprogram_definitions.end(); ++it)
			{
				unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram = *it;
				subprogram->Release();
			}
			
			for(typename Variables::const_iterator it = variable_declarations.begin(); it != variable_declarations.end(); ++it)
			{
				unisim::util::debug::Variable const *variable = *it;
				variable->Release();
			}

			for(typename Variables::const_iterator it = variable_definitions.begin(); it != variable_definitions.end(); ++it)
			{
				unisim::util::debug::Variable const *variable = *it;
				variable->Release();
			}
		}
		
		bool IsBuiltin(const std::string& s) const
		{
			static std::string builtin_prefix("__builtin_");
			if(s.compare(0, builtin_prefix.length(), builtin_prefix) == 0)
			{
				return true;
			}
			return false;
		}
		
		bool IsBuiltinType(const std::string& s) const
		{
			return IsBuiltin(s);
		}
		
		bool IsBuiltinFunction(const std::string& s) const
		{
			if(IsBuiltin(s)) return true;
			return builtin_functions.find(s) != builtin_functions.end();
		}

		void Insert(unisim::util::debug::Type const *type)
		{
			if(types.find(type) == types.end())
			{
				if(verbose)
				{
					std::cerr << "Discovering Type " << type->BuildCDecl() << std::endl;
				}
				type->Catch();
				types.insert(type);
			}
		}
		
		void Push(unisim::util::debug::Type const *type)
		{
			if(pushed.find(type) == pushed.end())
			{
				if(verbose)
				{
					std::cerr << "Pushing Type " << type->BuildCDecl() << std::endl;
				}
				pushed.insert(type);
				type->Catch();
				stack.push_back(type);
			}
		}

		bool Visit(unisim::util::debug::dwarf::DWARF_CompilationUnit<MEMORY_ADDR> const *dw_cu)
		{
			char const *dw_cu_name = dw_cu->GetName();
			if(sources.empty() || (dw_cu_name && (sources.find(dw_cu_name) != sources.end())))
			{
				if(verbose)
				{
					std::cerr << "Processing Compilation Unit #" << dw_cu->GetId() << " (" << dw_cu_name << ")" << std::endl;
				}
				dw_cu->Scan(*this);
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
					if(!subprogram_name)
					{
						std::cerr << "WARNING! DIE #" << dw_die->GetId() << " has no name!" << std::endl;
					}
					if(subprogram_name && !IsBuiltinFunction(subprogram_name))
					{
						if(verbose)
						{
							std::cerr << "Discovering Subprogram " << subprogram_name << std::endl;
						}
						unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram = dw_die->GetSubProgram(0);
						
						if(subprogram->IsDeclaration())
						{
							if(verbose)
							{
								std::cerr << "Subprogram " << subprogram_name << " in DIE #" << dw_die->GetId() << " is a declaration: looking for definition" << std::endl;
							}
							if(subprogram_declarations.find(subprogram) == subprogram_declarations.end())
							{
								subprogram->Catch();
								subprogram_declarations.insert(subprogram);
							}
							
							const unisim::util::debug::dwarf::DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = dw_handler->FindSubProgramDIE(subprogram_name, 0);
							if(dw_die_subprogram)
							{
								if(verbose)
								{
									std::cerr << "Found definition of " << dw_die_subprogram->GetSubProgram(0)->BuildCDecl() << " in DIE #" << dw_die_subprogram->GetId() << ": looking for dependencies" << std::endl;
								}
								dw_die_subprogram->ScanType(*this);
							}
							else
							{
								std::cerr << "WARNING! Can't find definition of " << subprogram_name << std::endl;
							}
						}
						else
						{
							if(verbose)
							{
								std::cerr << "Subprogram " << subprogram->BuildCDecl() << " in DIE #" << dw_die->GetId() << " is a definition: looking for dependencies" << std::endl;
							}
							if(subprogram_definitions.find(subprogram) == subprogram_definitions.end())
							{
								subprogram->Catch();
								subprogram_definitions.insert(subprogram);
							}
							dw_die->ScanType(*this);
						}
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_variable:
				{
					char const *variable_name = dw_die->GetName();
					if(!variable_name)
					{
						std::cerr << "WARNING! DIE #" << dw_die->GetId() << " has no name!" << std::endl;
					}
					if(variable_name)
					{
						unisim::util::debug::Variable const *variable = dw_die->GetVariable(0);
						if(variable->IsExternal())
						{
							if(verbose)
							{
								std::cerr << "Discovering External Variable " << variable->BuildCDecl() << " in DIE #" << dw_die->GetId() << std::endl;
							}
							if(variable->IsDeclaration())
							{
								if(verbose)
								{
									std::cerr << "Variable " << variable_name << " in DIE #" << dw_die->GetId() << " is a declaration: looking for dependencies" << std::endl;
								}
								if(variable_declarations.find(variable) == variable_declarations.end())
								{
									variable->Catch();
									variable_declarations.insert(variable);
								}
							}
							else
							{
								if(verbose)
								{
									std::cerr << "Variable " << variable_name << " in DIE #" << dw_die->GetId() << " is a definition: looking for dependencies" << std::endl;
								}
								if(variable_definitions.find(variable) == variable_definitions.end())
								{
									variable->Catch();
									variable_definitions.insert(variable);
								}
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
			if(visited.find(dw_die) != visited.end()) return true;
			visited.insert(dw_die);
			
			uint16_t dw_tag = dw_die->GetTag();
			switch(dw_tag)
			{
				case unisim::util::debug::dwarf::DW_TAG_enumeration_type:
				{
					const char *enum_name = dw_die->GetName();
					if(enum_name && !IsBuiltinType(enum_name))
					{
						const unisim::util::debug::EnumType *enum_type = (const unisim::util::debug::EnumType *) dw_die->GetType(0);
						Insert(enum_type);
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_structure_type:
				{
					const char *struct_name = dw_die->GetName();
					if(struct_name && !IsBuiltinType(struct_name))
					{
						const unisim::util::debug::StructureType *struct_type = (const unisim::util::debug::StructureType *) dw_die->GetType(0);
						Insert(struct_type);
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_union_type:
				{
					const char *union_name = dw_die->GetName();
					if(union_name && !IsBuiltinType(union_name))
					{
						const unisim::util::debug::UnionType *union_type = (const unisim::util::debug::UnionType *) dw_die->GetType(0);
						Insert(union_type);
					}
					break;
				}

				case unisim::util::debug::dwarf::DW_TAG_typedef:
				{
					const char *typedef_name = dw_die->GetName();
					if(typedef_name && !IsBuiltinType(typedef_name))
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
						Insert(_typedef);
					}
					break;
				}
				
				case unisim::util::debug::dwarf::DW_TAG_formal_parameter:
				case unisim::util::debug::dwarf::DW_TAG_member:
				case unisim::util::debug::dwarf::DW_TAG_pointer_type:
				case unisim::util::debug::dwarf::DW_TAG_const_type:
				case unisim::util::debug::dwarf::DW_TAG_volatile_type:
				case unisim::util::debug::dwarf::DW_TAG_array_type:
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
			if(comp_type->HasName() && !IsBuiltinType(comp_type->GetName()))
			{
				Push(comp_type);
			}
			return true;
		}
		
		bool Visit(unisim::util::debug::EnumType const *enum_type)
		{
			enum_type->Scan(*this);
			if(enum_type->HasName() && !IsBuiltinType(enum_type->GetName()))
			{
				Push(enum_type);
			}
			return true;
		}
		
		bool Visit(unisim::util::debug::Typedef const *_typedef)
		{
			_typedef->Scan(*this);
			if(_typedef->HasName() && !IsBuiltinType(_typedef->GetName()))
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
		
		void SortTypes()
		{
			if(verbose)
			{
				std::cerr << "Sorting types" << std::endl;
			}
			for(typename Types::const_iterator it = types.begin(); it != types.end(); ++it)
			{
				unisim::util::debug::Type const *type = *it;
				type->Visit(*this);
			}
		}
		
		void Run()
		{
			if(verbose)
			{
				std::cerr << "Scanning debug information" << std::endl;
			}
			dw_handler->Scan(*this);
			SortTypes();
		}
	};

	DebugInfoVisitor debug_info_visitor(*this, dw_handler);
	debug_info_visitor.Run();
	
	if(verbose)
	{
		std::cerr << "Opening output File \"" << output << "\"" << std::endl;
	}
	std::ofstream file(output.c_str());
	
	if(file.fail())
	{
		std::cerr << "ERROR! Can't open output File \"" << output << "\"" << std::endl;
		return false;
	}

	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << "//                                  Types                                    //" << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << std::endl;
	for(typename DebugInfoVisitor::Stack::const_iterator it = debug_info_visitor.stack.begin(); it != debug_info_visitor.stack.end(); ++it)
	{
		unisim::util::debug::Type const *type = *it;
		if(type)
		{
			file << type->BuildCDecl() << ";" << std::endl;
		}
	}
	
	file << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << "//                            External Functions                             //" << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << std::endl;
	for(typename DebugInfoVisitor::SubPrograms::const_iterator it = debug_info_visitor.subprogram_declarations.begin(); it != debug_info_visitor.subprogram_declarations.end(); ++it)
	{
		unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram = *it;
		unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram_def = dw_handler->FindSubProgram(0, subprogram->GetName());
		if(!subprogram_def)
		{
			std::cerr << "WARNING! Can't find definition of " << subprogram->GetName() << std::endl;
			continue;
		}
		file << "extern ";
		file << subprogram_def->BuildCDecl() << ";" << std::endl;
	}
	
	file << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << "//                         External Public Variables                         //" << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << std::endl;
	for(typename DebugInfoVisitor::Variables::const_iterator it = debug_info_visitor.variable_declarations.begin(); it != debug_info_visitor.variable_declarations.end(); ++it)
	{
		unisim::util::debug::Variable const *variable = *it;
		file << "extern " << variable->BuildCDecl() << ";" << std::endl;
	}

	file << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << "//                              Public Variables                             //" << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << std::endl;
	for(typename DebugInfoVisitor::Variables::const_iterator it = debug_info_visitor.variable_definitions.begin(); it != debug_info_visitor.variable_definitions.end(); ++it)
	{
		unisim::util::debug::Variable const *variable = *it;
		file << variable->BuildCDecl() << ";" << std::endl;
	}

	file << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << "//                              Private Functions                            //" << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << std::endl;
	for(typename DebugInfoVisitor::SubPrograms::const_iterator it = debug_info_visitor.subprogram_definitions.begin(); it != debug_info_visitor.subprogram_definitions.end(); ++it)
	{
		unisim::util::debug::SubProgram<MEMORY_ADDR> const *subprogram = *it;
		if(!subprogram->IsExternal())
		{
			file << "static ";
		}
		if(subprogram->IsInline())
		{
			file << "inline ";
		}
		file << subprogram->BuildCDecl() << " { }" << std::endl;
	}

	file << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << "//                                   Main                                    //" << std::endl;
	file << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
	file << std::endl;
	file << "int main(int argc, char *argv[]) { return 0; }" << std::endl;
	
	return true;
}

///////////////////////////// JSON_ConfigReader ///////////////////////////////

JSON_ConfigReader::JSON_ConfigReader(std::ostream& _err)
	: err(_err)
	, verbose(false)
	, binary()
	, output()
	, sources()
	, builtin_functions()
{
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
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_RIGHT_BRACE));
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
	MemberType member_type = ( member_name == "verbose"              ) ? VERBOSE           :
	                         ((member_name == "binary"               ) ? BINARY            :
	                         ((member_name == "sources"              ) ? SOURCES           :
	                         ((member_name == "builtin-functions"    ) ? BUILTIN_FUNCTIONS :
	                         ((member_name == "output"               ) ? OUTPUT            :
	                         ((member_name == "dwarf-html-output-dir") ? DWARF_HTML_OUTPUT_DIR : UNKNOWN)))));
	
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
			if(!ParseBool(stream, verbose))
			{
				Lexer::ParseError(stream, *this, std::string("expecting a boolean value for Member \"verbose\""));
				return false;
			}
			break;
		case BINARY:
			if(!ParseString(stream, binary))
			{
				Lexer::ParseError(stream, *this, std::string("expecting a string value for Member \"binary\""));
				return false;
			}
			break;
		case OUTPUT:
			if(!ParseString(stream, output))
			{
				Lexer::ParseError(stream, *this, std::string("expecting a string value for Member \"output\""));
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
		case BUILTIN_FUNCTIONS:
			if(!ParseStringArray(stream, builtin_functions))
			{
				Lexer::ParseError(stream, *this, std::string("expecting an array of strings for Member \"builtin-functions\""));
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

bool JSON_ConfigReader::ParseStringArray(std::istream& stream, std::set<std::string>& strings)
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
		Lexer::ParseError(stream, *this, std::string("unexpected ") + PrettyString(token, Lexer::GetText()) + ", expecting " + ToString(unisim::util::json::TOK_RIGHT_BRACKET));
		return false;
	}
	
	return true;
}

bool JSON_ConfigReader::ParseStringArrayElements(std::istream& stream, std::set<std::string>& strings)
{
	std::string value;
	if(!ParseString(stream, value))
	{
		return false;
	}
	
	strings.insert(value);
	
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

void Help()
{
	std::cerr << "UNISIM Excavator: a tool to dig DWARF debug information in ELF executable binaries" << std::endl;
	std::cerr << "Copyright (C) 2020, Commissariat a l'Energie Atomique (CEA)" << std::endl;
	std::cerr << "License: BSD (see file COPYING)" << std::endl;
	std::cerr << "Authors: Gilles Mouchard <gilles.mouchard@cea.fr>, Franck Vedrine <franck.vedrine@cea.fr>, Yves Lhuillier <yves.lhuillier@cea.fr>" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Usage: excavator <JSON configuration file>" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Sample JSON configuration file:" << std::endl;
	std::cerr << "{" << std::endl;
	std::cerr << "\t\"binary\" : \"vmlinux\"," << std::endl;
	std::cerr << "\t\"sources\" :" << std::endl;
	std::cerr << "\t[" << std::endl;
	std::cerr << "\t\t\"drivers/watchdog/at91sam9_wdt.c\"" << std::endl;
	std::cerr << "\t]," << std::endl;
	std::cerr << "\t\"builtin-functions\" :" << std::endl;
	std::cerr << "\t[" << std::endl;
	std::cerr << "\t\t\"snprintf\"," << std::endl;
	std::cerr << "\t\t\"sprintf\"," << std::endl;
	std::cerr << "\t\t\"sscanf\"," << std::endl;
	std::cerr << "\t\t\"strcspn\"," << std::endl;
	std::cerr << "\t\t\"strncasecmp\"," << std::endl;
	std::cerr << "\t\t\"strncat\"," << std::endl;
	std::cerr << "\t\t\"vsnprintf\"," << std::endl;
	std::cerr << "\t\t\"strspn\"," << std::endl;
	std::cerr << "\t\t\"bcmp\"," << std::endl;
	std::cerr << "\t\t\"fabs\"" << std::endl;
	std::cerr << "\t]," << std::endl;
	std::cerr << "\t\"output\" : \"out.c\"," << std::endl;
	std::cerr << "\t\"verbose\" : true," << std::endl;
	std::cerr << "\t\"dwarf-html-output-dir\" : \"dwarf\"" << std::endl;
	std::cerr << "}" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Options:" << std::endl;
	std::cerr << "  - binary: input ELF executable binary to analyze (mandatory option)." << std::endl;
	std::cerr << "  - sources: names of compilation units to process; if not specified or empty, it means all compilation units." << std::endl;
	std::cerr << "  - builtin-functions: functions to be considered as compiler builtins." << std::endl;
	std::cerr << "  - output: output file of the analysis (mandatory option); the result of the analysis is a reversed engineered C source code." << std::endl;
	std::cerr << "  - verbose: a flag to enable/disable verbosity during analysis." << std::endl;
	std::cerr << "  - dwarf-html-output-dir: directory where to dump DWARF debugging information as HTML; if not specified or empty, it means no dump." << std::endl;
	std::cerr << std::endl;
}

//////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		Help();
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
		Help();
		return -1;
	}
	
	// Checking that a binary filename was specified in JSON configuration file
	const std::string& binary_filename = json_config_reader.GetBinary();
	
	if(binary_filename.empty())
	{
		std::cerr << "ERROR! No binary specified in JSON configuration File \"" << json_config_filename << "\"" << std::endl;
		return -1;
	}
	
	// Checking that an output filename was specified in JSON configuration file
	const std::string& output_filename = json_config_reader.GetOutput();
	
	if(output_filename.empty())
	{
		std::cerr << "ERROR! No output specified in JSON configuration File \"" << json_config_filename << "\"" << std::endl;
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
	
	if(size >= 5)
	{
		if((magic[0] == 0x7f) && (magic[1] == 'E') && (magic[2] == 'L') && (magic[3] == 'F'))
		{
			if(magic[4] == 1)
			{
				// Binary file is an ELF32 file
				unisim::util::loader::elf_loader::Elf32Loader<uint32_t> elf32_loader;
				elf32_loader.SetFileName(binary_filename.c_str());
				elf32_loader.SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, json_config_reader.GetVerbose());
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
					if(!dw_handler32->HasDebugInfo())
					{
						std::cerr << "WARNING! ELF32 Binary file \"" << binary_filename << "\" does not contain any debugging information" << std::endl;
					}
					DWARF_Excavator<uint32_t, JSON_ConfigReader> dw_excavator32(dw_handler32, json_config_reader);
					if(!dw_excavator32.Dig()) return -1;
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
				elf64_loader.SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, json_config_reader.GetVerbose());
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
					if(!dw_handler64->HasDebugInfo())
					{
						std::cerr << "WARNING! ELF64 Binary file \"" << binary_filename << "\" does not contain any debugging information" << std::endl;
					}
					DWARF_Excavator<uint64_t, JSON_ConfigReader> dw_excavator64(dw_handler64, json_config_reader);
					if(!dw_excavator64.Dig()) return -1;
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
	
	return 0;
}
