/*
 *  Copyright (c) 2012,
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
 
#ifndef __UNISIM_SERVICE_INTERFACES_DATA_OBJECT_LOOKUP_HH__
#define __UNISIM_SERVICE_INTERFACES_DATA_OBJECT_LOOKUP_HH__

#include <unisim/service/interfaces/interface.hh>
#include <unisim/util/debug/data_object.hh>
#include <set>
#include <string>
#include <stdexcept>
#include <iostream>

namespace unisim {
namespace service {
namespace interfaces {

struct DataObjectNameScanner : ServiceInterface
{
	virtual void Append(const char *data_object_name) = 0;
};

struct DataObjectLookupBase : ServiceInterface
{
	typedef enum
	{
		SCOPE_GLOBAL_ONLY = 1,
		SCOPE_LOCAL_ONLY = 2,
		SCOPE_BOTH_GLOBAL_AND_LOCAL = 3
	} Scope;
	
	static constexpr Scope Scopes[] = { SCOPE_GLOBAL_ONLY, SCOPE_LOCAL_ONLY, SCOPE_BOTH_GLOBAL_AND_LOCAL };
	
	virtual void ScanDataObjectNames(DataObjectNameScanner& scanner, Scope scope = SCOPE_BOTH_GLOBAL_AND_LOCAL) const = 0;
};

template <typename ADDRESS>
struct DataObjectLookup : DataObjectLookupBase
{
	virtual unisim::util::debug::DataObjectRef<ADDRESS> FindDataObject(const char *data_object_name) const = 0;
	virtual unisim::util::debug::DataObjectRef<ADDRESS> GetReturnValue() const = 0;
	virtual unisim::util::debug::DataObjectRef<ADDRESS> GetSubProgramParameter(unsigned int index) const = 0;

	inline unisim::util::debug::DataObjectRef<ADDRESS> operator [] (const char *data_object_name) const;
	inline unisim::util::debug::DataObjectRef<ADDRESS> operator [] (const std::string& data_object_name) const;
};

template <typename ADDRESS>
inline unisim::util::debug::DataObjectRef<ADDRESS> DataObjectLookup<ADDRESS>::operator [] (const char *data_object_name) const
{
	return FindDataObject(data_object_name);
}

template <typename ADDRESS>
inline unisim::util::debug::DataObjectRef<ADDRESS> DataObjectLookup<ADDRESS>::operator [] (const std::string& data_object_name) const
{
	return FindDataObject(data_object_name.c_str());
}

inline std::ostream& operator << (std::ostream& stream, const typename DataObjectLookupBase::Scope scope)
{
	switch(scope)
	{
		case DataObjectLookupBase::SCOPE_GLOBAL_ONLY          : stream << "global-only"; break;
		case DataObjectLookupBase::SCOPE_LOCAL_ONLY           : stream << "local-only"; break;
		case DataObjectLookupBase::SCOPE_BOTH_GLOBAL_AND_LOCAL: stream << "both-global-and-local"; break;
	}
	return stream;
}

inline std::istream& operator >> (std::istream& stream, typename DataObjectLookupBase::Scope& scope)
{
	std::string str;
	if(stream >> str)
	{
		if(str == "global-only") scope = DataObjectLookupBase::SCOPE_GLOBAL_ONLY;
		else if(str == "local-only") scope = DataObjectLookupBase::SCOPE_LOCAL_ONLY;
		else if(str == "both-global-and-local") scope = DataObjectLookupBase::SCOPE_BOTH_GLOBAL_AND_LOCAL;
		else stream.setstate(std::ios::failbit);
	}
	return stream;
}

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INTERFACES_DATA_OBJECT_LOOKUP_HH__
