/*
 *  Copyright (c) 2022,
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

#ifndef __UNISIM_UTIL_UNICODE_UNICODE_HH__
#define __UNISIM_UTIL_UNICODE_UNICODE_HH__

#include <string>

namespace unisim {
namespace util {
namespace unicode {

bool append_utf8_string_to_unicode_wstring(const std::string& input, std::wstring& output, bool strict = true);
bool append_unicode_wchar_to_utf8_string(const wchar_t c, std::string& output, bool strict = true);
bool append_unicode_wstring_to_utf8_string(const std::wstring input, std::string& output, bool strict = true);
std::wstring utf8_string_to_unicode_wstring(const std::string& input);
std::string unicode_wchar_to_utf8_string(const wchar_t c);
std::string unicode_wstring_to_utf8_string(const std::wstring& input);
std::wstring utf8_string_to_unicode_wstring(const char *input);
std::string unicode_wstring_to_utf8_string(const wchar_t *input);

} // end of namespace unicode
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_UNICODE_UNICODE_HH__
