/*
 * convert.hh
 *
 *  Created on: 25 juin 2010
 *      Author: rnouacer
 */

#ifndef CONVERT_HH_
#define CONVERT_HH_

#include <stdint.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>

class BadConversion : public std::runtime_error {
public:
	BadConversion(std::string const& s)	: std::runtime_error(s) { }
};

template<typename T>
inline std::string stringify(T const& x) {

	std::ostringstream o;
	if (!(o << x)) {
		throw BadConversion(std::string("stringify(") + typeid(x).name() + ")");
	}

	return (o.str());
}

template<typename T>
inline void convert(std::string const& s, T& x, bool failIfLeftoverChars = true) {

	std::istringstream i(s);
	char c;

	if (!(i >> x) || (failIfLeftoverChars && i.get(c))) {
		throw BadConversion(s);
	}

}

template<typename T>
inline T convertTo(std::string const& s, bool failIfLeftoverChars = true) {

	T x;

	convert(s, x, failIfLeftoverChars);

	return (x);
}

inline bool isHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return (true);
	if(ch >= '0' && ch <= '9') return (true);
	if(ch >= 'A' && ch <= 'F') return (true);
	return (false);
}

inline char nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return (v < 10 ? '0' + v : 'a' + v - 10);
}

inline uint8_t hexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return (ch - 'a' + 10);
	if(ch >= '0' && ch <= '9') return (ch - '0');
	if(ch >= 'A' && ch <= 'F') return (ch - 'A' + 10);
	return (0);
}

inline void number2HexString(uint8_t* value, unsigned int size, std::string& hex, std::string endian) {

	int i;
	char c[2];
	c[1] = 0;

	hex.erase();

	if(endian == "big")
	{
#if BYTE_ORDER == BIG_ENDIAN
		for(i = 0; i < size; i++)
#else
		for(i = size - 1; i >= 0; i--)
#endif
		{
			c[0] = nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = nibble2HexChar(value[i] & 0xf);
			hex += c;
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for(i = 0; i < size; i++)
#else
		for(i = size - 1; i >= 0; i--)
#endif
		{
			c[0] = nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = nibble2HexChar(value[i] & 0xf);
			hex += c;
		}
	}

}

inline bool hexString2Number(const std::string& hex, void* value, unsigned int size, std::string endian) {

	int i;
	unsigned int len = hex.length();
	unsigned int pos = 0;

	if(endian == "big")
	{
#if BYTE_ORDER == BIG_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!isHexChar(hex[pos]) || !isHexChar(hex[pos + 1])) return (false);

			((uint8_t*) value)[i] = (hexChar2Nibble(hex[pos]) << 4) | hexChar2Nibble(hex[pos + 1]);
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!isHexChar(hex[pos]) || !isHexChar(hex[pos + 1])) return (false);

			((uint8_t*) value)[i] = (hexChar2Nibble(hex[pos]) << 4) | hexChar2Nibble(hex[pos + 1]);
		}
	}

	return (true);

}

inline void textToHex(const char *s, int count, std::string& packet)
{
	int i;
	std::stringstream strm;

	for(i = 0; (i<count); i++)
	{
		strm << nibble2HexChar((uint8_t) s[i] >> 4);
		strm << nibble2HexChar((uint8_t) s[i] & 0xf);
	}

	packet = strm.str();
	strm.str(std::string());

}

inline void hexToText(const char *s, int count, std::string& packet)
{
	int i;
	std::stringstream strm;
	uint8_t c;

	for (i = 0; i < count; i=i+2) {
		c = (hexChar2Nibble((uint8_t) s[i]) << 4) | hexChar2Nibble((uint8_t) s[i+1]);
		strm << c;
	}

	packet = strm.str();
	strm.str(std::string());
}

inline void stringSplit(std::string str, const std::string delim, std::vector<std::string>& results)
{
	int cutAt;

	do {

		cutAt = str.find_first_of(delim);

		if (cutAt > 0) {
			results.push_back(str.substr(0,cutAt));
		}

		str = str.substr(cutAt+1);

	} while (cutAt != str.npos);

	if(str.length() > 0)
	{
		results.push_back(str);
	}

}

/* Function to get parity of number n. It returns 1
   if n has odd parity, and returns 0 if n has even
   parity */
template <typename T>
inline bool getParity(T n)
{
    bool parity = false;
    while (n)
    {
        parity = !parity;
        n      = n & (n - 1);
    }
    return parity;
}


#endif /* CONVERT_HH_ */
