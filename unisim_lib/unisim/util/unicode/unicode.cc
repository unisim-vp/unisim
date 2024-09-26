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

#include <unisim/util/unicode/unicode.hh>
#include <climits>
#include <cstdint>

namespace unisim {
namespace util {
namespace unicode {

bool append_utf8_string_to_unicode_wstring(const std::string& input, std::wstring& output, bool strict)
{
	bool status = true;
	std::size_t n = input.size();
	std::size_t i = 0;
	while(i < n)
	{
		uint32_t code_point;
		std::size_t remaining_characters;
		uint8_t c = input[i++];
		if(c <= 0x7f)
		{
			// 1-byte UTF-8 character
			code_point = c;
			remaining_characters = 0;
		}
		else if(c <= 0xbf)
		{
			// invalid UTF-8 character
			if(strict) return false;
			code_point = '?';
			remaining_characters = 0;
			status = false;
		}
		else if(c <= 0xdf)
		{
			// 2-byte UTF-8 character
			code_point = c & 0x1f;
			remaining_characters = 1;
		}
		else if(c <= 0xef)
		{
			// 3-byte UTF-8 character
			code_point = c & 0x0f;
			remaining_characters = 2;
		}
		else if(c <= 0xf7)
		{
			// 4-byte UTF-8 character
			code_point = c & 0x07;
			remaining_characters = 3;
		}
		else
		{
			return false;
		}
		if(remaining_characters)
		{
			do
			{
				if(i == n)
				{
					// short read
					if(strict) return false;
					code_point = '?';
					status = false;
					break;
				}
				c = input[i++];
				if((c & 0xc0) != 0x80) return false; // invalid UTF-8 character
				code_point <<= 6;
				code_point += c & 0x3f;
			}
			while(--remaining_characters);
		}
		if(code_point > 0x10ffff)
		{
			// code point is too large for UTF-8
			if(strict) return false;
			code_point = '?';
			status = false;
		}
		
		if(sizeof(wchar_t) * CHAR_BIT == 16)
		{
			// UTF-16
			if((code_point >= 0xd800) && (code_point <= 0xdfff)) return false; // reserved
			if(code_point <= 0xffff)
			{
				output += (wchar_t) code_point;
			}
			else
			{
					code_point -= 0x10000;
					output += (wchar_t)((code_point >> 10) + 0xd800);
					output += (wchar_t)((code_point & 0x3ff) + 0xdc00);
			}
		}
		else if(sizeof(wchar_t) * CHAR_BIT == 32)
		{
			// UTF-32
			output += (wchar_t) code_point;
		}
	}
	return status;
}

bool append_unicode_wchar_to_utf8_string(const wchar_t c, std::string& output, bool strict)
{
	bool status = true;
	uint32_t code_point = c;
	if(code_point <= 0x7f)
	{
		// 1-byte UTF-8 character
		char c0 = (char)(code_point & 0xff);
		output += c0;
	}
	else if(code_point <= 0x7ff)
	{
		// 2-byte UTF-8 character: vvvvvxxxxxx => 110vvvvv  10xxxxxx
		uint16_t v = 0xc080 | ((code_point << 2) & 0x1f00) | (code_point & 0x3f);
		char c0 = (char)((v >> 8) & 0xff);
		char c1 = (char)(v & 0xff);
		output += c0;
		output += c1;
	}
	else if(code_point <= 0xffff)
	{
		// 3-byte UTF-8 character: vvvvxxxxxxyyyyyy => 1110vvvv 10xxxxxx 10yyyyyy
		uint32_t v = 0xe08080 | ((code_point << 4) & 0x0f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
		char c0 = (char)((v >> 16) & 0xff);
		char c1 = (char)((v >> 8) & 0xff);
		char c2 = (char)(v & 0xff);
		output += c0;
		output += c1;
		output += c2;
	}
	else if(code_point <= 0x10ffff)
	{
		// 4-byte UTF-8 character: vvvxxxxxxyyyyyyzzzzzz => 11110vvv 10xxxxxx 10yyyyyy 10zzzzzz
		uint32_t v = 0xf0808080 | ((code_point << 6) & 0x07000000) | ((code_point << 4) & 0x3f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
		char c0 = (char)((v >> 24) & 0xff);
		char c1 = (char)((v >> 16) & 0xff);
		char c2 = (char)((v >> 8) & 0xff);
		char c3 = (char)(v & 0xff);
		output += c0;
		output += c1;
		output += c2;
		output += c3;
	}
	else
	{
		if(strict) return false;
		output += '?';
		status = false;
	}
	return status;
}

bool append_unicode_wstring_to_utf8_string(const std::wstring input, std::string& output, bool strict)
{
	bool status = true;
	std::size_t n = input.size();
	std::size_t i = 0;
	while(i < n)
	{
		if(!append_unicode_wchar_to_utf8_string(input[i++], output, strict))
		{
			if(strict) return false;
			status = false;
		}
	}
	return status;
}

std::wstring utf8_string_to_unicode_wstring(const std::string& input)
{
	std::wstring output;
	append_utf8_string_to_unicode_wstring(input, output, false);
	return output;
}

std::string unicode_wchar_to_utf8_string(const wchar_t c)
{
	std::string output;
	append_unicode_wchar_to_utf8_string(c, output, false);
	return output;
}

std::string unicode_wstring_to_utf8_string(const std::wstring& input)
{
	std::string output;
	append_unicode_wstring_to_utf8_string(input, output, false);
	return output;
}

std::wstring utf8_string_to_unicode_wstring(const char *input)
{
	return utf8_string_to_unicode_wstring(std::string(input));
}

std::string unicode_wstring_to_utf8_string(const wchar_t *input)
{
	return unicode_wstring_to_utf8_string(std::wstring(input));
}

} // end of namespace unicode
} // end of namespace util
} // end of namespace unisim
