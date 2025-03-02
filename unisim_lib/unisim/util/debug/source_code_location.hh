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

#ifndef __UNISIM_UTIL_DEBUG_SOURCE_CODE_LOCATION_HH__
#define __UNISIM_UTIL_DEBUG_SOURCE_CODE_LOCATION_HH__

#include <ostream>
#include <sstream>
#include <limits>

namespace unisim {
namespace util {
namespace debug {

////////////////////////////// declarations ///////////////////////////////////

//////////////////////////// SourceCodeLocation ///////////////////////////////

class SourceCodeLocation
{
public:
	inline SourceCodeLocation();
	inline SourceCodeLocation(const std::string& source_code_filename, unsigned int lineno, unsigned int colno = 0);
	inline SourceCodeLocation(const char *source_code_filename, unsigned int lineno, unsigned int colno = 0);
	
	inline bool Parse(const char *source_code_location);
	inline bool Parse(const std::string& source_code_location);
	
	const std::string& GetSourceCodeFilename() const { return source_code_filename; }
	unsigned int GetLineNo() const { return lineno; }
	unsigned int GetColNo() const { return colno; }
	
	bool operator == (const SourceCodeLocation& o) const { return (source_code_filename == o.source_code_filename) && (lineno == o.lineno) && (colno == o.colno); }
private:
	std::string source_code_filename;
	unsigned int lineno;
	unsigned int colno;
};

inline std::ostream& operator << (std::ostream& os, const SourceCodeLocation& src_code_loc);

/////////////////////////////// definitions ///////////////////////////////////

//////////////////////////// SourceCodeLocation ///////////////////////////////

inline SourceCodeLocation::SourceCodeLocation()
	: source_code_filename()
	, lineno(0)
	, colno(0)
{
}

inline SourceCodeLocation::SourceCodeLocation(const std::string& _source_code_filename, unsigned int _lineno, unsigned int _colno)
	: source_code_filename(_source_code_filename)
	, lineno(_lineno)
	, colno(_colno)
{
}

inline SourceCodeLocation::SourceCodeLocation(const char *_source_code_filename, unsigned int _lineno, unsigned int _colno)
	: source_code_filename(_source_code_filename)
	, lineno(_lineno)
	, colno(_colno)
{
}

inline bool SourceCodeLocation::Parse(const char *source_code_location)
{
	source_code_filename.clear();
	const char *s = source_code_location;
	while(*s && (*s != ':'))
	{
		source_code_filename += *s;
		s++;
	}
	if(*s++ != ':') return false;
	
	std::string str_lineno;
	while(*s && (*s != ':'))
	{
		str_lineno += *s;
		s++;
	}
		
	std::stringstream sstr_lineno(str_lineno);
	int64_t _lineno;
	if(!(sstr_lineno >> _lineno)) return false;
	if((_lineno < 1) || (_lineno > std::numeric_limits<unsigned>::max())) return false;
	lineno = _lineno;
	
	if(*s++ != ':')
	{
		colno = 0;
		return true;
	}
	
	std::string str_colno;
	while(*s && (*s != ':'))
	{
		str_colno += *s;
		s++;
	}
	
	std::stringstream sstr_colno(str_colno);
	int64_t _colno;
	if(!(sstr_colno >> _colno)) return false;
	if((_colno < 1) || (_colno > std::numeric_limits<unsigned>::max())) return false;
	colno = _colno;
	
	return true;
}

inline bool SourceCodeLocation::Parse(const std::string& source_code_location)
{
	return Parse(source_code_location.c_str());
}

inline std::ostream& operator << (std::ostream& os, const SourceCodeLocation& src_code_loc)
{
	os << src_code_loc.GetSourceCodeFilename() << ":" << src_code_loc.GetLineNo();
	if(src_code_loc.GetColNo())
	{
		os << ":" << src_code_loc.GetColNo();
	}
	
	return os;
}


} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_SOURCE_CODE_LOCATION_HH__
