/*
 * convert.hh
 *
 *  Created on: 25 juin 2010
 *      Author: rnouacer
 */

#ifndef CONVERT_HH_
#define CONVERT_HH_

 #include <iostream>
 #include <sstream>
 #include <string>
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

	return o.str();
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

	return x;
}

inline char Nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return v < 10 ? '0' + v : 'a' + v - 10;
}

inline uint8_t HexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;
}

inline bool IsHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return true;
	if(ch >= '0' && ch <= '9') return true;
	if(ch >= 'A' && ch <= 'F') return true;
	return false;
}

#endif /* CONVERT_HH_ */
