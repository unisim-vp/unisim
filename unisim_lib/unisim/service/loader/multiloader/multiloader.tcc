/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_SERVICE_LOADER_MULTILOADER_MULTILOADER_TCC__
#define __UNISIM_SERVICE_LOADER_MULTILOADER_MULTILOADER_TCC__

#include <unisim/kernel/debug/debug.hh>
#include <string.h>
#include <limits>
#include <algorithm>
#include <string>

namespace unisim {
namespace service {
namespace loader {
namespace multiloader {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;

///////////////////////////////////////////////////////////////////////////////////////////
//                                  LoadStatementOption                                  //
///////////////////////////////////////////////////////////////////////////////////////////

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatementOption::LoadStatementOption(LoadStatementOptionType _type, unsigned int _pos, const std::string& _value)
	: type(_type)
	, pos(_pos)
	, value(_value)
{
}

///////////////////////////////////////////////////////////////////////////////////////////
//                                       LoadStatement                                   //
///////////////////////////////////////////////////////////////////////////////////////////

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatement::LoadStatement(unsigned int _pos)
	: pos(_pos)
{
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatement::~LoadStatement()
{
	typename std::vector<LoadStatementOption *>::iterator iter;
	for(iter = opts.begin(); iter != opts.end(); iter++)
	{
		delete *iter;
	}
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
void MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatement::AddOption(LoadStatementOption *opt)
{
	opts.push_back(opt);
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatementOption *MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatement::FindOption(LoadStatementOptionType opt_type)
{
	typename std::vector<LoadStatementOption *>::iterator opt_iter;
	
	for(opt_iter = opts.begin(); opt_iter != opts.end(); opt_iter++)
	{
		LoadStatementOption *opt = *opt_iter;
		
		if(opt->type == opt_type) return opt;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
//                                        MultiLoader                                    //
///////////////////////////////////////////////////////////////////////////////////////////


template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::MultiLoader(const char *name, Object *parent)
	: Object(name, parent)
	, loader_export("loader-export", this)
	, blob_export("blob-export", this)
	, symbol_table_lookup_export("symbol-table-lookup-export", this)
	, stmt_lookup_export("stmt-lookup-export", this)
	, logger(*this)
	, verbose(false)
	, tee_loader(0)
	, tee_blob(0)
	, tee_symbol_table_lookup(0)
	, memory_router(0)
	, filename()
	, positional_option_types()
	, raw_loaders()
	, elf32_loaders()
	, elf64_loaders()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_filename("filename", this, filename, "List of files to load. Syntax: [[(filename=]<filename1>[:[format=]<format1>]][,[(filename=]<filename2>[:[format=]<format2>]]... (e.g. boot.bin:raw,app.elf)")
{
	positional_option_types.push_back(OPT_FILENAME);
	positional_option_types.push_back(OPT_FORMAT);
	
	unsigned int i;
	
	for(i = 0; i < MAX_MEMORIES; i++)
	{
		std::stringstream sstr_import_name;
		sstr_import_name << "memory-import[" << i << "]";
		
		memory_import[i] = new ServiceImport<Memory<MEMORY_ADDR> >(sstr_import_name.str().c_str(), this);
	}
	
	tee_loader = new unisim::service::tee::loader::Tee<>("tee-loader", this);
	tee_blob = new unisim::service::tee::blob::Tee<MEMORY_ADDR>("tee-blob", this);
	tee_symbol_table_lookup = new unisim::service::tee::symbol_table_lookup::Tee<MEMORY_ADDR>("tee-symbol-table-lookup", this);
	tee_stmt_lookup = new unisim::service::tee::stmt_lookup::Tee<MEMORY_ADDR>("tee-stmt-lookup", this);
	
	memory_router = new MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>("memory-router", this);
	
	if(!filename.empty())
	{
		// Parse the filenames to load, and decompose them into load statements
		
		std::vector<LoadStatement *> ld_stmts;
		unsigned int pos = 0;

		do
		{
			Token tok;
			std::string tok_value;

			ReadToken(filename, pos, tok, tok_value);
			
			if(tok == TOK_EOF) break;
			
			LoadStatement *ld_stmt = ParseLoadStatement(filename, pos);
			
			if(!ld_stmt)
			{
				break;
			}
			
			ld_stmts.push_back(ld_stmt);
			
			unsigned int n;

			n = ReadToken(filename, pos, tok, tok_value);
			
			if(tok == TOK_EOF) break;

			if(tok != TOK_COMMA)
			{
				logger << DebugWarning << "In Parameter " << param_filename.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a ',')" <<EndDebugWarning;
				PrettyPrintSyntaxErrorLocation(filename.c_str(), pos);
				break;
			}
			
			pos += n;
		}
		while(1);
		
		// For each load statement, instantiate a loader
		unsigned int stmt_idx;
		
		for(stmt_idx = 0; stmt_idx < ld_stmts.size(); stmt_idx++)
		{
			LoadStatement *ld_stmt = ld_stmts[stmt_idx];
			
			LoadStatementOption *ld_stmt_opt_filename = ld_stmt->FindOption(OPT_FILENAME);
			if(!ld_stmt_opt_filename)
			{
				logger << DebugWarning << "No file specified for loading in " << param_filename.GetName() << " at column #" << ld_stmt->pos << EndDebugWarning;
				break;
			}
			
			const std::string& filename_to_load = ld_stmt_opt_filename->value;

			FileFormat file_fmt = FFMT_RAW;

			LoadStatementOption *ld_stmt_opt_format = ld_stmt->FindOption(OPT_FORMAT);
			
			if(ld_stmt_opt_format)
			{
				file_fmt = GetFileFormat(ld_stmt_opt_format->value.c_str());
			}
			else
			{
				// Try to magically guess file format
				if(verbose)
				{
					logger << DebugInfo << "Guessing file format of \"" << filename_to_load << "\"" << EndDebugInfo;
				}
				file_fmt = GuessFileFormat(filename_to_load);
				if(verbose)
				{
					logger << DebugInfo << "File \"" << filename_to_load << "\" looks like " << GetFileFormatName(file_fmt) << EndDebugInfo;
				}
			}
			
			if(verbose)
			{
				logger << DebugInfo << "File \"" << filename_to_load << " is " << GetFileFormatName(file_fmt) << EndDebugInfo;
			}
			
			std::stringstream sstr_loader_name;
			sstr_loader_name << GetFileFormatName(file_fmt) << "-loader[" << stmt_idx << "]";
			std::string loader_name(sstr_loader_name.str());
			std::transform(loader_name.begin(), loader_name.end(), loader_name.begin(), ::tolower); // make sure service name is lower case
			
			std::stringstream sstr_loader_param_filename_name;
			sstr_loader_param_filename_name << Object::GetName() << "." << loader_name << ".filename";
			std::string loader_param_filename_name(sstr_loader_param_filename_name.str());
			
			unisim::kernel::service::Simulator::simulator->SetVariable(loader_param_filename_name.c_str(), filename_to_load.c_str());
			
			// the real stuff of instantiating a loader is there
			switch(file_fmt)
			{
				case FFMT_RAW:
					{
						RawLoader<MEMORY_ADDR> *raw_loader = new RawLoader<MEMORY_ADDR>(loader_name.c_str(), this);
						raw_loaders.push_back(raw_loader);
						
						*tee_loader->loader_import[stmt_idx] >> raw_loader->loader_export;
						raw_loader->memory_import >> memory_router->memory_export;
					}
					break;
				case FFMT_ELF32:
					{
						Elf32Loader<MEMORY_ADDR> *elf32_loader = new Elf32Loader<MEMORY_ADDR>(loader_name.c_str(), this);
						elf32_loaders.push_back(elf32_loader);
						
						*tee_loader->loader_import[stmt_idx] >> elf32_loader->loader_export;
						*tee_blob->blob_import[stmt_idx] >> elf32_loader->blob_export;
						*tee_symbol_table_lookup->symbol_table_lookup_import[stmt_idx] >> elf32_loader->symbol_table_lookup_export;
						*tee_stmt_lookup->stmt_lookup_import[stmt_idx] >> elf32_loader->stmt_lookup_export;
						elf32_loader->memory_import >> memory_router->memory_export;
					}
					break;
				case FFMT_ELF64:
					{
						Elf64Loader<MEMORY_ADDR> *elf64_loader = new Elf64Loader<MEMORY_ADDR>(loader_name.c_str(), this);
						elf64_loaders.push_back(elf64_loader);
						
						*tee_loader->loader_import[stmt_idx] >> elf64_loader->loader_export;
						*tee_blob->blob_import[stmt_idx] >> elf64_loader->blob_export;
						*tee_symbol_table_lookup->symbol_table_lookup_import[stmt_idx] >> elf64_loader->symbol_table_lookup_export;
						*tee_stmt_lookup->stmt_lookup_import[stmt_idx] >> elf64_loader->stmt_lookup_export;
						elf64_loader->memory_import >> memory_router->memory_export;
					}
					break;
				case FFMT_S19:
					{
						S19_Loader<MEMORY_ADDR> *s19_loader = new S19_Loader<MEMORY_ADDR>(loader_name.c_str(), this);
						s19_loaders.push_back(s19_loader);
						
						*tee_loader->loader_import[stmt_idx] >> s19_loader->loader_export;
						s19_loader->memory_import >> memory_router->memory_export;
					}
					break;
				case FFMT_COFF:
					{
						CoffLoader<MEMORY_ADDR> *coff_loader = new CoffLoader<MEMORY_ADDR>(loader_name.c_str(), this);
						coff_loaders.push_back(coff_loader);
						
						*tee_loader->loader_import[stmt_idx] >> coff_loader->loader_export;
						coff_loader->memory_import >> memory_router->memory_export;
					}
					break;
			}
		}
	}
	
	loader_export >> tee_loader->loader_export;
	blob_export >> tee_blob->blob_export;
	symbol_table_lookup_export >> tee_symbol_table_lookup->symbol_table_lookup_export;
	stmt_lookup_export >> tee_stmt_lookup->stmt_lookup_export;
	
	for(i = 0; i < MAX_MEMORIES; i++)
	{
		*memory_router->memory_import[i] >> *memory_import[i];
	}
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::~MultiLoader()
{
	unsigned int i;
	
	for(i = 0; i < MAX_MEMORIES; i++)
	{
		delete memory_import[i];
	}
	
	for(i = 0; i < raw_loaders.size(); i++)
	{
		delete raw_loaders[i];
	}
	
	for(i = 0; i < elf32_loaders.size(); i++)
	{
		delete elf32_loaders[i];
	}

	for(i = 0; i < elf64_loaders.size(); i++)
	{
		delete elf64_loaders[i];
	}

	for(i = 0; i < s19_loaders.size(); i++)
	{
		delete s19_loaders[i];
	}

	for(i = 0; i < coff_loaders.size(); i++)
	{
		delete coff_loaders[i];
	}

	if(tee_loader) delete tee_loader;
	if(tee_blob) delete tee_blob;
	if(tee_symbol_table_lookup) delete tee_symbol_table_lookup;
	if(tee_stmt_lookup) delete tee_stmt_lookup;
	if(memory_router) delete memory_router;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
bool MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::IsVerbose() const
{
	return verbose;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatement *MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::ParseLoadStatement(const std::string& s, unsigned int& pos)
{
	unsigned opt_idx = 0;
	LoadStatement *ld_stmt = new LoadStatement(pos);
	
	do
	{
		LoadStatementOption *ld_stmt_opt = ParseLoadStatementOption(s, pos, opt_idx);
		
		if(!ld_stmt_opt)
		{
			delete ld_stmt;
			return 0;
		}
		
		ld_stmt->AddOption(ld_stmt_opt);
		opt_idx++;
		
		Token tok;
		std::string tok_value;
		unsigned int n;

		n = ReadToken(s, pos, tok, tok_value);
		
		if((tok == TOK_COMMA) || (tok == TOK_EOF))
		{
			return ld_stmt;
		}
		
		if(tok != TOK_COLON)
		{
			logger << DebugWarning << "In Parameter " << param_filename.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a ':')" << EndDebugWarning;
			PrettyPrintSyntaxErrorLocation(filename.c_str(), pos);
			delete ld_stmt;
			return 0;
		}
		
		pos += n;
	}
	while(1);
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatementOption *MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::ParseLoadStatementOption(const std::string& s, unsigned int& pos, unsigned int opt_idx)
{
	LoadStatementOption *ld_stmt_opt = 0;
	Token tok;
	Token look_ahead_tok;
	std::string dummy;
	std::string tok_value;
	unsigned int n;
	LoadStatementOptionType opt_type = OPT_UNKNOWN;
	int state = 0;
	
	do
	{
		n = ReadToken(s, pos, tok, tok_value);
		
		switch(state)
		{
			case 0:
				if(tok != TOK_STRING)
				{
					logger << DebugWarning << "In Parameter " << param_filename.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a string)" << EndDebugWarning;
					PrettyPrintSyntaxErrorLocation(filename.c_str(), pos);
					return 0;
				}
				ReadToken(s, pos + n, look_ahead_tok, dummy);
				
				opt_type = GetOptionType(tok_value);
				if(opt_type == OPT_UNKNOWN)
				{
					if(look_ahead_tok == TOK_EQUAL)
					{
						logger << DebugWarning << "In Parameter " << param_filename.GetName() << ", unknown option name in " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << EndDebugWarning;
						PrettyPrintSyntaxErrorLocation(filename.c_str(), pos);
						return 0;
					}
					else
					{
						if(opt_idx < positional_option_types.size())
						{
							ld_stmt_opt = new LoadStatementOption(positional_option_types[opt_idx], pos, tok_value);
						}
						else
						{
							logger << DebugWarning << "In Parameter " << param_filename.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << EndDebugWarning;
							PrettyPrintSyntaxErrorLocation(filename.c_str(), pos);
							return 0;
						}
					}
				}
				state = 1;
				break;
			case 1:
				if(tok != TOK_EQUAL)
				{
					logger << DebugWarning << "In Parameter " << param_filename.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting an '=')" << EndDebugWarning;
					PrettyPrintSyntaxErrorLocation(filename.c_str(), pos);
					return 0;
				}
				state = 2;
				break;
			case 2:
				if(tok != TOK_STRING)
				{
					logger << DebugWarning << "In Parameter " << param_filename.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a string)" << EndDebugWarning;
					PrettyPrintSyntaxErrorLocation(filename.c_str(), pos);
					return 0;
				}
				
				ld_stmt_opt = new LoadStatementOption(opt_type, pos, tok_value);
		}
		
		pos += n;
	}
	while(!ld_stmt_opt);
	
	return ld_stmt_opt;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
unsigned int MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::ReadToken(const std::string& s, unsigned int pos, Token& tok, std::string& tok_value)
{
	unsigned int n;
	tok_value.clear();
	char c;
	
	if(pos >= s.length())
	{
		tok = TOK_EOF;
		return 0;
	}
	
	c = s[pos];
	
	n = 1;

	if(c == '=')
	{
		tok_value += c;
		tok = TOK_EQUAL;
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
		return n;
	}
	
	if(c == ',')
	{
		tok_value = c;
		tok = TOK_COMMA;
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
		return n;
	}
	
	if(c == ':')
	{
		tok_value = c;
		tok = TOK_COLON;
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
		return n;
	}
	
	do
	{
		tok_value += c;
		if((pos + n) >= s.length()) break;
		c = s[pos + n];
		if((c == '=') || (c == ',') || (c == ':')) break;
		n++;
	}
	while(1);
	
	tok = TOK_STRING;
	
	if(IsVerbose())
	{
		logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
	}
	return n;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
std::string MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::GetTokenName(Token tok, const std::string& tok_value)
{
	std::stringstream sstr;
	switch(tok)
	{
		case TOK_COMMA:
			sstr << "token ','";
			break;
		case TOK_COLON:
			sstr << "token ':'";
			break;
		case TOK_EQUAL:
			sstr << "token '='";
			break;
		case TOK_STRING:
			sstr << "string '" << tok_value << "'";
			break;
		case TOK_EOF:
			sstr << "end";
			break;
	}
	return sstr.str();
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::LoadStatementOptionType MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::GetOptionType(const std::string& opt_name)
{
	if(opt_name.compare("filename") == 0) return OPT_FILENAME;
	if(opt_name.compare("format") == 0) return OPT_FORMAT;
	return OPT_UNKNOWN;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::FileFormat MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::GuessFileFormat(const std::string& filename_to_guess_file_format)
{
	uint8_t magic[8];
	
	std::string location = Object::GetSimulator()->SearchSharedDataFile(filename_to_guess_file_format.c_str());
			
	std::ifstream f(location.c_str(), std::ifstream::in | std::ifstream::binary);
	
	if(f.fail())
	{
		logger << DebugError << "Can't open input \"" << location << "\"" << EndDebugError;
		return FFMT_RAW;
	}
	
	std::streampos size = f.readsome((char *) magic, sizeof(magic));
	
	if(f.fail())
	{
		logger << DebugError << "Input/Ouput error while reading file \"" << location << "\"" << EndDebugError;
		return FFMT_RAW;
	}
	
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		std::cerr << "magic[" << i << "]=0x" << std::hex << (unsigned int) magic[i] << std::dec << std::endl;
	}
	
	if(size >= 5)
	{
		if((magic[0] == 0x7f) && (magic[1] == 'E') && (magic[2] == 'L') && (magic[3] == 'F'))
		{
			if(magic[4] == 1) return FFMT_ELF32;
			if(magic[4] == 2) return FFMT_ELF64;
		}
	}
	
	if(size >= 2)
	{
		if((magic[0] == 'S') && (magic[1] == '0')) return FFMT_S19;
		
		if((magic[0] == 0xc1) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0xc2) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0x92) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0x93) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0x95) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0x98) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0x99) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0x9d) && (magic[1] == 0x00)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0xc1)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0xc2)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0x92)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0x93)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0x95)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0x98)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0x99)) return FFMT_COFF;
		if((magic[0] == 0x00) && (magic[1] == 0x9d)) return FFMT_COFF;
	}
	
	return FFMT_RAW;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
const char *MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::GetFileFormatName(FileFormat file_fmt)
{
	switch(file_fmt)
	{
		case FFMT_ELF32: return "ELF32";
		case FFMT_ELF64: return "ELF64";
		case FFMT_RAW: return "raw";
		case FFMT_S19: return "S19";
		case FFMT_COFF: return "COFF";
	}
	return "?";
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::FileFormat MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::GetFileFormat(const char *name)
{
	if((strcmp(name, "elf32") == 0) || (strcmp(name, "ELF32") == 0)) return FFMT_ELF32;
	if((strcmp(name, "elf64") == 0) || (strcmp(name, "ELF64") == 0)) return FFMT_ELF64;
	if((strcmp(name, "s19") == 0) || (strcmp(name, "S19") == 0)) return FFMT_S19;
	if((strcmp(name, "coff") == 0) || (strcmp(name, "COFF") == 0)) return FFMT_COFF;
	return FFMT_RAW;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
void MultiLoader<MEMORY_ADDR, MAX_MEMORIES>::PrettyPrintSyntaxErrorLocation(const char *s, unsigned int pos)
{
	logger << DebugWarning << s << EndDebugWarning;
	logger << DebugWarning;
	unsigned int i;
	for(i = 0; i < pos; i++) logger << " ";
	logger << "^" << EndDebugWarning;
}

///////////////////////////////////////////////////////////////////////////////////////////
//                                     AddressRange                                      //
///////////////////////////////////////////////////////////////////////////////////////////

template <class MEMORY_ADDR>
AddressRange<MEMORY_ADDR>::AddressRange()
	: low(std::numeric_limits<MEMORY_ADDR>::max())
	, high(std::numeric_limits<MEMORY_ADDR>::min())
{
}

///////////////////////////////////////////////////////////////////////////////////////////
//                               MappingStatementOption                                  //
///////////////////////////////////////////////////////////////////////////////////////////

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatementOption::MappingStatementOption(MappingStatementOptionType _type, unsigned int _pos, const std::string& _value)
	: type(_type)
	, pos(_pos)
	, value(_value)
	, range()
{
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatementOption::MappingStatementOption(MappingStatementOptionType _type, unsigned int _pos, const AddressRange<MEMORY_ADDR>& _range)
	: type(_type)
	, pos(_pos)
	, value()
	, range(_range)
{
}

///////////////////////////////////////////////////////////////////////////////////////////
//                                   MappingStatement                                    //
///////////////////////////////////////////////////////////////////////////////////////////

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatement::MappingStatement(unsigned int _pos)
	: pos(_pos)
{
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatement::~MappingStatement()
{
	typename std::vector<MappingStatementOption *>::iterator iter;
	for(iter = opts.begin(); iter != opts.end(); iter++)
	{
		delete *iter;
	}
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
void MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatement::AddOption(MappingStatementOption *opt)
{
	opts.push_back(opt);
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatementOption *MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatement::FindOption(MappingStatementOptionType opt_type)
{
	typename std::vector<MappingStatementOption *>::iterator opt_iter;
	
	for(opt_iter = opts.begin(); opt_iter != opts.end(); opt_iter++)
	{
		MappingStatementOption *opt = *opt_iter;
		
		if(opt->type == opt_type) return opt;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryRouter                                     //
///////////////////////////////////////////////////////////////////////////////////////////

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MemoryRouter(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<MEMORY_ADDR> >(name, parent)
	, Client<Memory<MEMORY_ADDR> >(name, parent)
	, memory_export("memory-export", this)
	, logger(*this)
	, verbose(false)
	, mapping()
	, mapping_table()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_mapping("mapping", this, mapping, "Memory mapping. Syntax: [[(memory=]<memory1>[:[range=]<low1-high1>]][,[(memory=]<memory2>[:[range=]<low2-high2>]]... (e.g. ram:0x0-0x00ffff,rom:0xff0000-0xffffff)")
{
	positional_option_types.push_back(OPT_MEMORY);
	positional_option_types.push_back(OPT_RANGE);

	unsigned int i;
	
	for(i = 0; i < MAX_MEMORIES; i++)
	{
		std::stringstream sstr_import_name;
		sstr_import_name << "memory-import[" << i << "]";
		memory_import[i] = new ServiceImport<Memory<MEMORY_ADDR> >(sstr_import_name.str().c_str(), this);
	}
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::~MemoryRouter()
{
	unsigned int i;
	
	for(i = 0; i < MAX_MEMORIES; i++)
	{
		delete memory_import[i];
	}
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
bool MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::BeginSetup()
{
	if(!mapping.empty())
	{
		std::vector<MappingStatement *> mapping_stmts;
		unsigned int pos = 0;

		do
		{
			Token tok;
			std::string tok_value;
			MEMORY_ADDR dummy;

			ReadToken(mapping, pos, tok, tok_value, dummy);
			
			if(tok == TOK_EOF) break;
			
			MappingStatement *mapping_stmt = ParseMappingStatement(mapping, pos);
			
			if(!mapping_stmt)
			{
				break;
			}
			
			mapping_stmts.push_back(mapping_stmt);
			
			unsigned int n;

			n = ReadToken(mapping, pos, tok, tok_value, dummy);
			
			if(tok == TOK_EOF) break;

			if(tok != TOK_COMMA)
			{
				logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a ',')" <<EndDebugWarning;
				PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
				break;
			}
			
			pos += n;
		}
		while(1);
		
		unsigned int num_mapping_stmts = mapping_stmts.size();
		unsigned int i, j;
		
		for(i = 0; i < num_mapping_stmts; i++)
		{
			int memory_num = -1;
			const AddressRange<MEMORY_ADDR> *addr_range = 0;
			
			const MappingStatement *mapping_stmt = mapping_stmts[i];
			const std::vector<MappingStatementOption *>& mapping_stmt_opts = mapping_stmt->opts;
			unsigned int num_mapping_stmt_opts = mapping_stmt_opts.size();
			
			for(j = 0; j < num_mapping_stmt_opts; j++)
			{
				const MappingStatementOption *mapping_stmt_opt = mapping_stmt_opts[j];
				
				switch(mapping_stmt_opt->type)
				{
					case OPT_MEMORY:
						{
							const std::string memory_name = mapping_stmt_opt->value;
							if(memory_num < 0)
							{
								memory_num = FindMemory(memory_name.c_str());
								if(memory_num < 0)
								{
									logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", memory '" << memory_name << "' is unknown or disconnected" << EndDebugWarning;
									PrettyPrintSyntaxErrorLocation(mapping.c_str(), mapping_stmt_opt->pos);
								}
							}
							else
							{
								logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected memory '" << memory_name << "' at specified character #" << (mapping_stmt_opt->pos + 1) << " because a memory was already specified" << EndDebugWarning;
								PrettyPrintSyntaxErrorLocation(mapping.c_str(), mapping_stmt_opt->pos);
							}
						}
						break;
					case OPT_RANGE:
						if(!addr_range)
						{
							addr_range = &mapping_stmt_opt->range;
						}
						else
						{
							logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << mapping_stmt_opt->value << " address range at character #" << (mapping_stmt_opt->pos + 1) << " because an address range was already specified" << EndDebugWarning;
							PrettyPrintSyntaxErrorLocation(mapping.c_str(), mapping_stmt_opt->pos);
						}
						break;
					default:
						logger << DebugError << "Internal error" << EndDebugError;
						Object::Stop(-1);
						break;
				}
			}
			
			if((memory_num >= 0) && addr_range)
			{
				mapping_table.push_back(new Mapping<MEMORY_ADDR>(memory_num, *addr_range));
			}
			else
			{
				if(memory_num < 0)
				{
					logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", expected a memory in statement at character #" << (mapping_stmt->pos + 1) << EndDebugWarning;
					PrettyPrintSyntaxErrorLocation(mapping.c_str(), mapping_stmt->pos);
				}
				if(!addr_range)
				{
					logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", expected an address range in statement at character #" << (mapping_stmt->pos + 1) << EndDebugWarning;
					PrettyPrintSyntaxErrorLocation(mapping.c_str(), mapping_stmt->pos);
				}
			}
		}
	}
	
	return true;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
int MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::FindMemory(const char *name)
{
	unsigned int i;
	
	for(i = 0; i < MAX_MEMORIES; i++)
	{
		if(memory_import[i])
		{
			if(*memory_import[i])
			{
				Object *obj = memory_import[i]->GetService();
				if(strcmp(obj->GetName(), name) == 0) return i;
			}
		}
	}
	return -1;
}


template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
bool MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::IsVerbose() const
{
	return verbose;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
unsigned int MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::ReadToken(const std::string& s, unsigned int pos, Token& tok, std::string& tok_value, MEMORY_ADDR& tok_addr_value)
{
	unsigned int n;
	tok_value.clear();
	char c;
	
	if(pos >= s.length())
	{
		tok = TOK_EOF;
		return 0;
	}
	
	c = s[pos];
	
	n = 1;

	if(c == '=')
	{
		tok_value += c;
		tok = TOK_EQUAL;
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
		return n;
	}
	
	if(c == ',')
	{
		tok_value = c;
		tok = TOK_COMMA;
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
		return n;
	}
	
	if(c == ':')
	{
		tok_value = c;
		tok = TOK_COLON;
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
		return n;
	}
	
	if(c == '-')
	{
		tok_value = c;
		tok = TOK_MINUS;
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
		return n;
	}

	if((c >= '0') && (c <= '9'))
	{
		tok_value += c;
		tok_addr_value = c - '0';
		
		int state = 0;
		do
		{
			if((pos + n) >= s.length()) break;
			c = s[pos + n];
			
			if((c == '=') || (c == ',') || (c == ':')) break;

			switch(state)
			{
				case 0:
					if((c == 'x') || (c == 'X'))
					{
						state = 2;
						break;
					}
					if((c >= '0') && (c <= '9'))
					{
						state = 1;
						break;
					}
					state = -1;
					break;
				case 1:
					if((c >= '0') && (c <= '9'))
					{
						tok_addr_value = (tok_addr_value * 10) + (MEMORY_ADDR)(c - '0');
						break;
					}
					state = -1;
					break;
				case 2:
					if((c >= '0') && (c <= '9'))
					{
						tok_addr_value = (tok_addr_value << 4) + (MEMORY_ADDR)(c - '0');
						break;
					}
					else if((c >= 'a') && (c <= 'f'))
					{
						tok_addr_value = (tok_addr_value << 4) + (MEMORY_ADDR)(c - 'a') + 10;
						break;
					}
					else if((c >= 'A') && (c <= 'F'))
					{
						tok_addr_value = (tok_addr_value << 4) + (MEMORY_ADDR)(c - 'A') + 10;
						break;
					}
					state = -1;
					break;
			}
			
			if(state != -1)
			{
				tok_value += c;
				n++;
			}
		}
		while(state != -1);
		
		tok = TOK_ADDRESS;

		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
	}
	else
	{
		do
		{
			tok_value += c;
			if((pos + n) >= s.length()) break;
			c = s[pos + n];
			if((c == '=') || (c == ',') || (c == ':')) break;
			n++;
		}
		while(1);
	
		tok = TOK_STRING;
		
		if(IsVerbose())
		{
			logger << DebugInfo << "At character #" << (pos + 1) << ", got " << GetTokenName(tok, tok_value) << " (" << n << " characters length )" << EndDebugInfo;
		}
	}
	return n;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
std::string MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::GetTokenName(Token tok, const std::string& tok_value)
{
	std::stringstream sstr;
	switch(tok)
	{
		case TOK_COMMA:
			sstr << "token ','";
			break;
		case TOK_COLON:
			sstr << "token ':'";
			break;
		case TOK_EQUAL:
			sstr << "token '='";
			break;
		case TOK_STRING:
			sstr << "string '" << tok_value << "'";
			break;
		case TOK_ADDRESS:
			sstr << "address " << tok_value;
			break;
		case TOK_MINUS:
			sstr << "token '-'";
			break;
		case TOK_EOF:
			sstr << "end";
			break;
	}
	return sstr.str();
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
bool MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::ParseAddressRange(const std::string& s, unsigned int& pos, AddressRange<MEMORY_ADDR>& addr_range)
{
	Token tok;
	std::string tok_value;
	MEMORY_ADDR tok_addr_value;
	MEMORY_ADDR dummy;
	unsigned int n;
	
	n = ReadToken(s, pos, tok, tok_value, tok_addr_value);
	
	if(tok != TOK_ADDRESS)
	{
		logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting an address)" << EndDebugWarning;
		PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
		return false;
	}
	
	pos += n;
	MEMORY_ADDR low = tok_addr_value;
	
	n = ReadToken(s, pos, tok, tok_value, dummy);
	
	if(tok != TOK_MINUS)
	{
		logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a '-')" << EndDebugWarning;
		PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
		return false;
	}

	pos += n;
	n = ReadToken(s, pos, tok, tok_value, tok_addr_value);
	
	if(tok != TOK_ADDRESS)
	{
		logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting an address)" << EndDebugWarning;
		PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
		return false;
	}
	
	MEMORY_ADDR high = tok_addr_value;
	
	addr_range.low = low;
	addr_range.high = high;
	return true;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatementOption *MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::ParseMappingStatementOption(const std::string& s, unsigned int& pos, unsigned int opt_idx)
{
	MappingStatementOption *mapping_stmt_opt = 0;
	Token tok;
	MEMORY_ADDR dummy_addr;
	std::string tok_value;
	unsigned int n;
	MappingStatementOptionType opt_type = OPT_UNKNOWN;
	int state = 0;
	
	do
	{
		n = ReadToken(s, pos, tok, tok_value, dummy_addr);
		
		switch(state)
		{
			case 0:
				opt_type = GetOptionType(tok_value);
				if(opt_type == OPT_UNKNOWN)
				{
					if(opt_idx < positional_option_types.size())
					{
						opt_type = positional_option_types[opt_idx];
						
						switch(opt_type)
						{
							case OPT_MEMORY:
								mapping_stmt_opt = new MappingStatementOption(OPT_MEMORY, pos, tok_value);
								break;
							case OPT_RANGE:
								{
									AddressRange<MEMORY_ADDR> addr_range;
									if(!ParseAddressRange(s, pos, addr_range))
									{
										logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", expecting an address range at character #" << (pos + 1) << EndDebugWarning;
										PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
										return 0;
									}
									mapping_stmt_opt = new MappingStatementOption(OPT_RANGE, pos, addr_range);
								}
								break;
							default:
								logger << DebugError << "Internal error" << EndDebugError;
								Object::Stop(-1);
						}
					}
					else
					{
						logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << EndDebugWarning;
						PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
						return 0;
					}
				}
				state = 1;
				break;
				
			case 1:
				if(tok != TOK_EQUAL)
				{
					logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting an '=')" << EndDebugWarning;
					PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
					return 0;
				}
				state = 2;
				break;
			case 2:
				if(tok != TOK_STRING)
				{
					logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a string)" << EndDebugWarning;
					PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
					return 0;
				}
				
				mapping_stmt_opt = new MappingStatementOption(opt_type, pos, tok_value);
		}
		
		pos += n;
	}
	while(!mapping_stmt_opt);
	
	return mapping_stmt_opt;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatement *MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::ParseMappingStatement(const std::string& s, unsigned int& pos)
{
	unsigned opt_idx = 0;
	MappingStatement *mapping_stmt = new MappingStatement(pos);
	
	do
	{
		MappingStatementOption *mapping_stmt_opt = ParseMappingStatementOption(s, pos, opt_idx);
		
		if(!mapping_stmt_opt)
		{
			delete mapping_stmt;
			return 0;
		}
		
		mapping_stmt->AddOption(mapping_stmt_opt);
		opt_idx++;
		
		Token tok;
		std::string tok_value;
		MEMORY_ADDR dummy;
		unsigned int n;

		n = ReadToken(s, pos, tok, tok_value, dummy);
		
		if((tok == TOK_COMMA) || (tok == TOK_EOF))
		{
			return mapping_stmt;
		}
		
		if(tok != TOK_COLON)
		{
			logger << DebugWarning << "In Parameter " << param_mapping.GetName() << ", unexpected " << GetTokenName(tok, tok_value) << " at character #" << (pos + 1) << " (expecting a ':')" << EndDebugWarning;
			PrettyPrintSyntaxErrorLocation(mapping.c_str(), pos);
			delete mapping_stmt;
			return 0;
		}
		
		pos += n;
	}
	while(1);
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
typename MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::MappingStatementOptionType MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::GetOptionType(const std::string& opt_name)
{
	if(opt_name.compare("memory") == 0) return OPT_MEMORY;
	if(opt_name.compare("range") == 0) return OPT_RANGE;
	return OPT_UNKNOWN;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
void MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::PrettyPrintSyntaxErrorLocation(const char *s, unsigned int pos)
{
	logger << DebugWarning << s << EndDebugWarning;
	logger << DebugWarning;
	unsigned int i;
	for(i = 0; i < pos; i++) logger << " ";
	logger << "^" << EndDebugWarning;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
void MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::Reset()
{
	unsigned int i;
	
	for(i = 0; i < MAX_MEMORIES; i++)
	{
		if(*memory_import[i])
		{
			(*memory_import[i])->Reset();
		}
	}
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
bool MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::ReadMemory(MEMORY_ADDR addr, void *buffer, uint32_t size)
{
	MEMORY_ADDR low = addr;
	MEMORY_ADDR high = addr + size - 1;
	
	if(low > high) // out of address space ?
	{
		logger << DebugWarning << "Access out of address space" << EndDebugWarning;
		memset((uint8_t *) buffer, 0, size);
		high = std::numeric_limits<MEMORY_ADDR>::max();
	}
	
	do
	{
		bool mapped = false;
		unsigned int i;
		unsigned int num_mappings = mapping_table.size();
		for(i = 0; i < num_mappings; i++)
		{
			const Mapping<MEMORY_ADDR> *mapping = mapping_table[i];
			if(mapping->addr_range.high >= mapping->addr_range.low) // filter invalid address ranges
			{
				if((low >= mapping->addr_range.low) && (low <= mapping->addr_range.high))
				{
					uint32_t sz = (high > mapping->addr_range.high) ? mapping->addr_range.high - low + 1 : high - low + 1;
					if(!(*memory_import[mapping->memory_num])->ReadMemory(low, (uint8_t *) buffer + (low - addr), sz)) return false;
					MEMORY_ADDR next_addr = low + sz;
					if(next_addr < low) return true; // return if end of address space has been reached
					low = next_addr;
					mapped = true;
					break;
				}
			}
		}
		
		if(!mapped)
		{
			logger << DebugWarning << "Address 0x" << std::hex << low << std::dec << " is unmapped" << EndDebugWarning;
			return false;
		}
	}
	while(low <= high);
	
	return true;
}

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
bool MemoryRouter<MEMORY_ADDR, MAX_MEMORIES>::WriteMemory(MEMORY_ADDR addr, const void *buffer, uint32_t size)
{
	MEMORY_ADDR low = addr;
	MEMORY_ADDR high = addr + size - 1;
	
	if(low > high) // out of address space ?
	{
		logger << DebugWarning << "Access out of address space" << EndDebugWarning;
		high = std::numeric_limits<MEMORY_ADDR>::max();
	}
	
	do
	{
		bool mapped = false;
		unsigned int i;
		unsigned int num_mappings = mapping_table.size();
		for(i = 0; i < num_mappings; i++)
		{
			const Mapping<MEMORY_ADDR> *mapping = mapping_table[i];
			if(mapping->addr_range.high >= mapping->addr_range.low) // filter invalid address ranges
			{
				if((low >= mapping->addr_range.low) && (low <= mapping->addr_range.high))
				{
					uint32_t sz = (high > mapping->addr_range.high) ? mapping->addr_range.high - low + 1 : high - low + 1;
					if(!(*memory_import[mapping->memory_num])->WriteMemory(low, (const uint8_t *) buffer + (low - addr), sz)) return false;
					MEMORY_ADDR next_addr = low + sz;
					if(next_addr < low) return true; // return if end of address space has been reached
					low = next_addr;
					mapped = true;
					break;
				}
			}
		}
		
		if(!mapped)
		{
			logger << DebugWarning << "Address 0x" << std::hex << low << std::dec << " is unmapped" << EndDebugWarning;
			return false;
		}
	}
	while(low <= high);
	
	return true;
}

} // end of namespace multiloader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_LOADER_MULTILOADER_TCC__

