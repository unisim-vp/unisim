/*
 *  Copyright (c) 2007,
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

#ifndef __UNISIM_UTIL_LOCATE_LOCATE_HH__
#define __UNISIM_UTIL_LOCATE_LOCATE_HH__

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access()
#else
#include <unistd.h>
#endif

#include <string>
#include <vector>

namespace unisim {
namespace util {
namespace locate {

std::string Dirname(const std::string& path);

std::string Basename(const std::string& path);

// Resolve path to an absolute path
bool ResolvePath(const std::string& unresolved_path, std::string& resolved_path);

struct LocateFileOptions
{
	std::string search_path;      // semi-colon separated list of directory path to look for the file
	std::string shared_directory; // when non-empty the shared directory path is also prepended to each path in the search path to extend the search path
	bool lazy_match;              // to increase the chance to locate a source file when dealing with incomplete pathes in debug information
	
	LocateFileOptions() : search_path(), shared_directory(), lazy_match(false) {}
};

// Try to locate a file and if found output an absolute path and return true, otherwise return false
bool LocateFile(const std::string& filename, std::string& match_file_path, const LocateFileOptions& options = LocateFileOptions());

// Try to locate a file and if found return the absolute path, otherwise return the filename as is.
std::string LocateFile(const std::string& filename, const LocateFileOptions& options = LocateFileOptions());

} // end of namespace locate
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_LOCATE_LOCATE_HH__

