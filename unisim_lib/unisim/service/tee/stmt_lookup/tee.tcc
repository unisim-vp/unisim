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
 * 
 */

#ifndef __UNISIM_SERVICE_TEE_STMT_LOOKUP_TEE_TCC__
#define __UNISIM_SERVICE_TEE_STMT_LOOKUP_TEE_TCC__

#include <string>
#include <sstream>

namespace unisim {
namespace service {
namespace tee {
namespace stmt_lookup {

using std::stringstream;
using std::string;

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::Tee(const char *name, Object *parent)
  	: Object(name, parent, "This service/client implements a tee ('T'). It unifies the statement lookup capability of several services that individually provides their own statement lookup capability" )
  	, Service<StatementLookup<ADDRESS> >(name, parent)
  	, Client<StatementLookup<ADDRESS> >(name, parent)
  	, stmt_lookup_export("stmt-lookup-export", this)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		stringstream sstr;
		sstr << "stmt-lookup-import[" << i << "]";
		string import_name = sstr.str();
		stmt_lookup_import[i] = new ServiceImport<StatementLookup<ADDRESS> >(import_name.c_str(), this);
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::~Tee()
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(stmt_lookup_import[i]) delete stmt_lookup_import[i];
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(stmt_lookup_import[i])
		{
			if(*stmt_lookup_import[i])
			{
				(*stmt_lookup_import[i])->ScanStatements(scanner, filename);
			}
		}
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const unisim::util::debug::Statement<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindStatement(ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret_stmt = 0;
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(stmt_lookup_import[i])
		{
			if(*stmt_lookup_import[i])
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = (*stmt_lookup_import[i])->FindStatement(addr, filename, scope);
				if(stmt)
				{
					switch(scope)
					{
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT:
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION:
							if(stmt->GetAddress() <= addr)
							{
								if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
							}
							break;
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEXT_STMT:
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEXT_STMT_WITHIN_FUNCTION:
							if(stmt->GetAddress() > addr)
							{
								if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
							}
							break;
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_EXACT_STMT:
							return stmt;
							break;
					}
				}
			}
		}
	}

	return ret_stmt;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const unisim::util::debug::Statement<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret_stmt = 0;
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(stmt_lookup_import[i])
		{
			if(*stmt_lookup_import[i])
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = (*stmt_lookup_import[i])->FindStatements(scanner, addr, filename, scope);
				if(stmt)
				{
					switch(scope)
					{
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT:
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION:
							if(stmt->GetAddress() <= addr)
							{
								if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
							}
							break;
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEXT_STMT:
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEXT_STMT_WITHIN_FUNCTION:
							if(stmt->GetAddress() > addr)
							{
								if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
							}
							break;
						case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_EXACT_STMT:
							return stmt;
							break;
					}
				}
			}
		}
	}

	return ret_stmt;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const unisim::util::debug::Statement<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(stmt_lookup_import[i])
		{
			if(*stmt_lookup_import[i])
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = (*stmt_lookup_import[i])->FindStatement(source_code_location, filename);
				if(stmt) return stmt;
			}
		}
	}

	return 0;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const unisim::util::debug::Statement<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret = 0;
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(stmt_lookup_import[i])
		{
			if(*stmt_lookup_import[i])
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = (*stmt_lookup_import[i])->FindStatements(scanner, source_code_location, filename);
				if(!ret) ret = stmt;
			}
		}
	}

	return ret;
}

} // end of namespace memory_access_reporting
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_STMT_LOOKUP_TEE_TCC__
