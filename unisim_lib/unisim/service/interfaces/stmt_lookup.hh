/*
 *  Copyright (c) 2010,
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
 
#ifndef __UNISIM_SERVICE_INTERFACES_STMT_LOOKUP_HH__
#define __UNISIM_SERVICE_INTERFACES_STMT_LOOKUP_HH__

#include <unisim/service/interfaces/interface.hh>
#include <unisim/util/debug/stmt.hh>
#include <unisim/util/debug/source_code_location.hh>
#include <map>
#include <vector>

namespace unisim {
namespace service {
namespace interfaces {

template <class MEMORY_ADDR>
struct StatementScanner : public ServiceInterface
{
  virtual void Append(const unisim::util::debug::Statement<MEMORY_ADDR> *) = 0;
};

struct StatementLookupBase : ServiceInterface
{
	typedef enum
	{
		SCOPE_NEAREST_LOWER_OR_EQUAL_STMT,
		SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION,
		SCOPE_EXACT_STMT,
		SCOPE_NEXT_STMT,
		SCOPE_NEXT_STMT_WITHIN_FUNCTION
	}
	Scope;
	
	static constexpr Scope Scopes[] = { SCOPE_NEAREST_LOWER_OR_EQUAL_STMT, SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION, SCOPE_EXACT_STMT, SCOPE_NEXT_STMT, SCOPE_NEXT_STMT_WITHIN_FUNCTION };
};

template <class MEMORY_ADDR>
struct StatementLookup : StatementLookupBase
{
	virtual void ScanStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, const char *filename = 0) const = 0;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr, const char *filename = 0, Scope scope = SCOPE_EXACT_STMT) const = 0;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, MEMORY_ADDR addr, const char *filename = 0, Scope scope = SCOPE_EXACT_STMT) const = 0;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename = 0) const = 0;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename = 0) const = 0;
	
	void ScanStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, const std::string& filename) const { ScanStatements(scanner, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr, const std::string& filename, Scope scope = SCOPE_EXACT_STMT) const { return FindStatement(addr, filename.length() ? filename.c_str() : 0, scope); }
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, MEMORY_ADDR addr, const std::string& filename, Scope scope = SCOPE_EXACT_STMT) const { return FindStatements(scanner, addr, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatement(source_code_location, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatements(scanner, source_code_location, filename.length() ? filename.c_str() : 0); }
};

inline std::ostream& operator << (std::ostream& stream, const typename StatementLookupBase::Scope scope)
{
	switch(scope)
	{
		case StatementLookupBase::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT: stream << "nearest-lower-or-equal"; break;
		case StatementLookupBase::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION: stream << "nearest-lower-or-equal-stmt-within-function"; break;
		case StatementLookupBase::SCOPE_EXACT_STMT: stream << "exact-stmt"; break;
		case StatementLookupBase::SCOPE_NEXT_STMT: stream << "next-stmt"; break;
		case StatementLookupBase::SCOPE_NEXT_STMT_WITHIN_FUNCTION: stream << "next-stmt-within-function"; break;
	}
	return stream;
}

inline std::istream& operator >> (std::istream& stream, typename StatementLookupBase::Scope& scope)
{
	std::string str;
	if(stream >> str)
	{
		if(str == "nearest-lower-or-equal") scope = StatementLookupBase::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT;
		else if(str == "nearest-lower-or-equal-stmt-within-function") scope = StatementLookupBase::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION;
		else if(str == "exact-stmt") scope = StatementLookupBase::SCOPE_EXACT_STMT;
		else if(str == "next-stmt") scope = StatementLookupBase::SCOPE_NEXT_STMT;
		else if(str == "next-stmt-within-function") scope = StatementLookupBase::SCOPE_NEXT_STMT_WITHIN_FUNCTION;
		else stream.setstate(std::ios::failbit);
	}
	return stream;
}

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
