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

#include <data_types/util.h>
#include <sstream>

namespace sc_dt {

static const std::string int_to_string(bool is_signed, uint_type value, int len, sc_numrep numrep, bool w_prefix)
{
	bool is_negative = is_signed && ((int_type) value < 0);
	
	switch(numrep)
	{
		case SC_BIN_US:
		case SC_OCT_US:
		case SC_HEX_US:
			if(is_negative) return std::string("negative");
			break;
		default:
			break;
	}
	
	std::string s;
	switch(numrep)
	{
		case SC_BIN_SM:
		case SC_OCT_SM:
		case SC_HEX_SM:
			if(is_negative) s += '-';
			break;
		default:
			break;
	}

	if(w_prefix)
	{
		switch(numrep)
		{
			case SC_NOBASE: ; break;
			case SC_BIN   : s += "0b"; break;
			case SC_OCT   : s += "0o"; break;
			case SC_DEC   : s += "0d"; break;
			case SC_HEX   : s += "0x"; break;
			case SC_BIN_US: s += "0bus"; break;
			case SC_BIN_SM: s += "0bsm"; break;
			case SC_OCT_US: s += "0ous"; break;
			case SC_OCT_SM: s += "0osm"; break;
			case SC_HEX_US: s += "0xus"; break;
			case SC_HEX_SM: s += "0xsm"; break;
			case SC_CSD   : s += "0csd"; break;
		}
	}

	uint_type v = value;
	
	switch(numrep)
	{
		case SC_BIN_SM:
		case SC_OCT_SM:
		case SC_HEX_SM:
			if(is_negative) v = -v;
			if(value && !v) s += '1';
			break;
		default:
			break;
	}
	
	int msb_pos = len - 1;
	unsigned int msb = (v >> msb_pos) & 1;
	while(--msb_pos >= 0)
	{
		unsigned int d = (v >> msb_pos) & 1;
		if(d != msb) break;
	}
	
	std::size_t digit_pos = s.length();
	
	switch(numrep)
	{
		case SC_BIN:
		case SC_BIN_SM:
		case SC_BIN_US:
			s.append(msb_pos + 1, '0'); // reserve space for digits
			for(int i = msb_pos; i >= 0; --i)
			{
				unsigned int d = (v >> i) & 1;
				s[digit_pos + i] = '0' + d;
			}
			break;
		case SC_OCT:
		case SC_OCT_SM:
		case SC_OCT_US:
			s.append((msb_pos / 3) + 1, '0'); // reserve space for digits
			for(int i = msb_pos - (msb_pos % 3); i >= 0; i -= 3)
			{
				unsigned int d = (v >> i) & 7;
				s[digit_pos + i] = '0' + d;
			}
			break;
		case SC_HEX:
		case SC_HEX_SM:
		case SC_HEX_US:
			s.append((msb_pos / 4) + 1, '0'); // reserve space for digits
			for(int i = msb_pos - (msb_pos % 4); i >= 0; i -= 4)
			{
				unsigned int d = (value >> i) & 0xf;
				s[digit_pos + i] = (d < 10) ? ('0' + d) : ('a' + d - 10);
			}
			break;
		case SC_NOBASE:
		case SC_DEC   :
		{
			std::stringstream sstr;
			if(is_signed)
			{
				sstr << (int_type) value;
			}
			else
			{
				sstr << value;
			}
			s += sstr.str();
			break;
		}
		case SC_CSD   :
		{
			const char conv_table[2][2] = {
			//  next    y  =>  digit     symbol
			/*   0      0  =>    0    */  '0',
			/*   0      1  =>   -1    */  '-',
			/*   1      0  =>    0    */  '0',
			/*   1      1  =>   +1    */  '1'
			}; 
				
			s.append(msb_pos + 1, '0'); // reserve space for digits
			bool y = false;
			bool prev = false; // v[i-1]: v[-1]=0
			bool curr = v & 1; // v[i]
			bool next;         // v[i+1]
			for(int i = msb_pos; i >= 0; --i)
			{
				if(i >= 0) next = (v >>= 1) & 1; // v[msb_pos+1]=v[msb_pos]
				bool x = curr ^ prev; // x = v[i] ^ v[i-1]
				y = !y & x;
				s[digit_pos + i] = conv_table[next][y]; // symbol=(1 - 2*v[i+1])*y
				prev = curr;
				curr = next;
			}
			break;
		}
		default:
			break;
	}
	
	return s;
}

static bool parse_sign(const char *& s, bool& is_negative)
{
	char c = *s;
	if(c == '-')
	{
		++s;
		is_negative = true;
		return true;
	}
	else if(c == '+')
	{
		++s;
		is_negative = false;
		return true;
	}
	
	return false;
}

static bool parse_prefix(const char *s, sc_numrep& numrep)
{
	int state = 0;
	char c = *s;
	if(c != 0)
	{
		sc_numrep nr = SC_NOBASE;
		do
		{
			switch(state)
			{
				case 0:
					if(c == '0')
					{
						state = 1;
						break;
					}
					return false;
				case 1:
					if((c == 'b') || (c == 'B'))
					{
						nr = SC_BIN;
						state = 2;
						break;
					}
					if((c == 'o') || (c == 'O'))
					{
						nr = SC_OCT;
						state = 2;
						break;
					}
					if((c == 'x') || (c == 'X'))
					{
						nr = SC_HEX;
						state = 2;
						break;
					}
					if((c == 'd') || (c == 'D'))
					{
						numrep = SC_DEC;
						return true;
					}
					if((c == 'c') || (c == 'C'))
					{
						state = 5;
						break;
					}
					--s;
					return false;
				case 2:
					if((c == 'u') || (c == 'U'))
					{
						state = 3;
						break;
					}
					if((c == 's') || (c == 'S'))
					{
						state = 4;
						break;
					}
					numrep = nr;
					return true;
				case 3:
					if((c == 's') || (c == 'S'))
					{
						if(nr == SC_BIN) numrep = SC_BIN_US;
						else if(nr == SC_OCT) numrep = SC_OCT_US;
						else if(nr == SC_HEX) numrep = SC_HEX_US;
						return true;
					}
					s -= 3;
					return false;
				case 4:
					if((c == 'm') || (c == 'M'))
					{
						if(nr == SC_BIN) numrep = SC_BIN_SM;
						else if(nr == SC_OCT) numrep = SC_OCT_SM;
						else if(nr == SC_HEX) numrep = SC_HEX_SM;
						return true;
					}
					s -= 3;
					return false;
				case 5:
					if((c == 's') || (c == 'S'))
					{
						state = 6;
						break;
					}
					s -= 2;
					return false;
				case 6:
					if((c == 'd') || (c == 'D'))
					{
						numrep = SC_CSD;
						return true;
					}
					s -= 3;
					return false;
			}
		}
		while((c = *++s) != 0);
	}
	
	return false;
}

static bool string_to_int(const char *s, bool is_signed, uint_type& value)
{
	bool is_negative = false;
	bool has_sign = parse_sign(s, is_negative);

	sc_numrep numrep;
	if(!parse_prefix(s, numrep)) numrep = SC_DEC;
	
	switch(numrep)
	{
		case SC_BIN:
		case SC_OCT:
		case SC_HEX:
		case SC_BIN_US:
		case SC_OCT_US:
		case SC_HEX_US:
		case SC_CSD   :
			if(has_sign) return false;
		default:
			break;
	}
	
	char c = *s;
	if(c == 0) return false;
	
	uint_type v = 0;
	
	do
	{
		switch(numrep)
		{
			case SC_BIN:
			case SC_BIN_US:
			case SC_BIN_SM:
				if((c == '0') || (c == '1'))
				{
					v <<= 1;
					v |= c - '0';
					break;
				}
				return false;
			case SC_OCT:
			case SC_OCT_US:
			case SC_OCT_SM:
				if((c >= '0') && (c <= '7'))
				{
					v <<= 3;
					v |= c - '0';
					break;
				}
				return false;
			case SC_HEX:
			case SC_HEX_US:
			case SC_HEX_SM:
				if((c >= '0') && (c <= '9')) 
				{
					v <<= 4;
					v |= c - '0';
					break;
				}
				else if((c >= 'a') && (c <= 'f')) 
				{
					v <<= 4;
					v |= c - 'a';
					break;
				}
				else if((c >= 'A') && (c <= 'F')) 
				{
					v <<= 4;
					v |= c - 'A';
					break;
				}
				return false;
			case SC_NOBASE:
			case SC_DEC:
				if((c >= '0') && (c <= '9')) 
				{
					v *= 10;
					v += c - '0';
					break;
				}
				return false;
			case SC_CSD:
				if(c == '1')
				{
					v <<= 1;
					v |= 1;
					break;
				}
				if(c == '-')
				{
					v <<= 1;
					--v;
					break;
				}
				if(c == '0')
				{
					v <<= 1;
					break;
				}
				
				return false;
		}
	}
	while((c = *++s) != 0);
	
	switch(numrep)
	{
		case SC_BIN_SM:
		case SC_OCT_SM:
		case SC_HEX_SM:
			if(is_negative) v = -v;
			break;
		default:
			break;
	}
	
	return true;
}

const std::string int_to_string(int_type value, int len, sc_numrep numrep, bool w_prefix)
{
	return int_to_string(true, value, len, numrep, w_prefix);
}

const std::string int_to_string(uint_type value, int len, sc_numrep numrep, bool w_prefix)
{
	return int_to_string(false, value, len, numrep, w_prefix);
}

bool string_to_int(const char *s, int_type& value)
{
	return string_to_int(s, false, (uint_type&) value);
}

bool string_to_int(const char *s, uint_type& value)
{
	return string_to_int(s, false, value);
}

} // end of namespace sc_dt
