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
 
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/util/locate/locate.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access()
#else
#include <unistd.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

InlineDebuggerBase::InlineDebuggerBase(const char *_name, unisim::kernel::Object *_parent)
	: unisim::kernel::Object(_name, _parent)
	, search_path()
	, param_search_path("search-path", this, search_path, "Search path for source (separated by ';')")
	, is_started(false)
{
}

InlineDebuggerBase::~InlineDebuggerBase()
{
}

bool InlineDebuggerBase::SigInt()
{
	if(!is_started) return false;
	
	Interrupt();
	
	return true; // handled
}

bool InlineDebuggerBase::LocateFile(const std::string& filename, std::string& match_file_path)
{
	unisim::util::locate::LocateFileOptions options;
	options.search_path = search_path;
	options.shared_directory = unisim::kernel::Object::GetSimulator()->GetSharedDataDirectory();
	return unisim::util::locate::LocateFile(filename, match_file_path, options);
}

bool InlineDebuggerBase::LocateSourceFile(const std::string& filename, std::string& match_file_path)
{
	unisim::util::locate::LocateFileOptions options;
	options.search_path = search_path;
	options.shared_directory = unisim::kernel::Object::GetSimulator()->GetSharedDataDirectory();
	options.lazy_match = true;
	return unisim::util::locate::LocateFile(filename, match_file_path, options);
}

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
