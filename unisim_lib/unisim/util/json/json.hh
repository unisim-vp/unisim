/*
 *  Copyright (c) 2019,
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

#ifndef __UNISIM_UTIL_JSON_JSON_HH__
#define __UNISIM_UTIL_JSON_JSON_HH__

#include <unisim/util/dictionary/dictionary.hh>
#include <unisim/util/unicode/unicode.hh>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <cassert>

namespace unisim {
namespace util {
namespace json {

// forward declarations

struct Location;
struct JSON_Printer;
struct JSON_Value;
struct JSON_String;
struct JSON_Number;
struct JSON_Integer;
struct JSON_Float;
struct JSON_Boolean;
struct JSON_Null;
struct JSON_Object;
struct JSON_Member;
struct JSON_Array;
struct JSON_AST_Visitor;
struct JSON_AST_Printer;
struct JSON_AST_Builder;
template <typename VISITOR> struct JSON_Lexer;
template <typename VISITOR> struct JSON_Parser;
struct JSON_Parser_Printer;

// Utility functions

// convert something into a string using the ostream << operator
template <typename T>
std::string ToString(const T& v)
{
	std::ostringstream sstr;
	sstr << v;
	return sstr.str();
}

// bare string to JSON string
std::string Escape(const std::string& s);

// Value type enumeration
enum JSON_ValueType
{
	JSON_UNDEFINED = 0,  // undefined value
	JSON_STRING    = 1,  // string
	JSON_INT       = 2,  // integer
	JSON_FLOAT     = 4,  // floating-point number
	JSON_OBJECT    = 8,  // object
	JSON_ARRAY     = 16, // array
	JSON_BOOLEAN   = 32, // boolean
	JSON_NULL      = 64  // null
};

std::ostream& operator << (std::ostream& os, const JSON_ValueType& type);

// Location in JSON document
struct Location
{
	Location() : lineno(0), colno(0) {}
	Location(unsigned int _lineno) : lineno(_lineno), colno(0) {}
	Location(unsigned int _lineno, unsigned int _colno) : lineno(_lineno), colno(_colno) {}
	
	bool IsDefined() const { return lineno != 0; }
	
	unsigned int lineno;
	unsigned int colno;
};

std::ostream& operator << (std::ostream& stream, const Location& loc);

// JSON Exception
struct Exception : public std::runtime_error
{
public:
	Exception(const char *msg, const Location& loc) : std::runtime_error(loc.IsDefined() ? (std::string("At ") + ToString(loc) + ", " + msg).c_str() : msg) {}
	Exception(const std::string& msg, const Location& loc) : std::runtime_error(loc.IsDefined() ? (std::string("At ") + ToString(loc) + ", " + msg).c_str() : msg.c_str()) {};
};

// JSON type error
struct TypeError : Exception
{
	TypeError(const char *msg, const Location& loc) : Exception(std::string("type error: not ") + msg, loc) {}
	TypeError(const std::string& msg, const Location& loc) : Exception(std::string("type error: not ") + msg, loc) {}
};

// JSON Not a string type error
struct NotAStringTypeError : TypeError
{
	NotAStringTypeError(const Location& loc) : TypeError("a string", loc) {}
};

// JSON Not a number type error
struct NotANumberTypeError : TypeError
{
	NotANumberTypeError(const Location& loc) : TypeError("a number", loc) {}
};

// JSON Not an integer type error
struct NotAnIntegerTypeError : TypeError
{
	NotAnIntegerTypeError(const Location& loc) : TypeError("an integer", loc) {}
};

// JSON Not a floating-point number type error
struct NotAFloatTypeError : TypeError
{
	NotAFloatTypeError(const Location& loc) : TypeError("a floating-point number", loc) {}
};

// JSON Not an object type error
struct NotAnObjectTypeError : TypeError
{
	NotAnObjectTypeError(const Location& loc) : TypeError("an object", loc) {}
};

// JSON Not an array type error
struct NotAnArrayTypeError : TypeError
{
	NotAnArrayTypeError(const Location& loc) : TypeError("an array", loc) {}
};

// JSON Not a boolean type error
struct NotABooleanTypeError : TypeError
{
	NotABooleanTypeError(const Location& loc) : TypeError("a boolean", loc) {}
};

// JSON Not null type error
struct NotNullTypeError : TypeError
{
	NotNullTypeError(const Location& loc) : TypeError("null", loc) {}
};

// JSON undefined value
extern JSON_Value undefined;

// JSON null value
extern JSON_Value null;

// JSON Value
struct JSON_Value
{
	JSON_Value() : type(JSON_UNDEFINED), loc(), ref_count(0) {}
	JSON_Value(const Location& _loc) : type(JSON_UNDEFINED), loc(_loc), ref_count(0) {}
	virtual ~JSON_Value() {}
	
	virtual JSON_Value *Clone() const { return new JSON_Value(type, loc); }
	
	JSON_ValueType GetType() const { return type; }
	
	bool IsDefined() const { return type != JSON_UNDEFINED; }
	bool IsUndefined() const { return type == JSON_UNDEFINED; }
	bool IsString() const { return type == JSON_STRING; }
	bool IsNumber() const { return (type & (JSON_INT | JSON_FLOAT)) != 0; }
	bool IsInteger() const { return type == JSON_INT; }
	bool IsFloat() const { return type == JSON_FLOAT; }
	bool IsObject() const { return type == JSON_OBJECT; }
	bool IsArray() const { return type == JSON_ARRAY; }
	bool IsBoolean() const { return type == JSON_BOOLEAN; }
	bool IsNull() const { return type == JSON_NULL; }
	
	void AssertIsString() const { if(!IsString()) throw NotAStringTypeError(loc); }
	void AssertIsNumber() const { if(!IsNumber()) throw NotANumberTypeError(loc); }
	void AssertIsInteger() const { if(!IsInteger()) throw NotAnIntegerTypeError(loc); }
	void AssertIsFloat() const { if(!IsFloat()) throw NotAFloatTypeError(loc); }
	void AssertIsObject() const { if(!IsObject()) throw NotAnObjectTypeError(loc); }
	void AssertIsArray() const { if(!IsArray()) throw NotAnArrayTypeError(loc); }
	void AssertIsBoolean() const { if(!IsBoolean()) throw NotABooleanTypeError(loc); }
	void AssertIsNull() const { if(!IsNull()) throw NotNullTypeError(loc); }
	
	const JSON_String& AsString() const { AssertIsString(); return *(const JSON_String *) this; }
	const JSON_Number& AsNumber() const { AssertIsNumber(); return *(const JSON_Number *) this; }
	const JSON_Integer& AsInteger() const { AssertIsInteger(); return *(const JSON_Integer *) this; }
	const JSON_Float& AsFloat() const { AssertIsFloat(); return *(const JSON_Float *) this; }
	const JSON_Object& AsObject() const { AssertIsObject(); return *(const JSON_Object *) this; }
	const JSON_Array& AsArray() const { AssertIsArray(); return *(const JSON_Array *) this; }
	const JSON_Boolean& AsBoolean() const { AssertIsBoolean(); return *(const JSON_Boolean *) this; }
	const JSON_Null& AsNull() const { AssertIsNull(); return *(const JSON_Null *) this; }
	
	JSON_String& AsString() { AssertIsString(); return *(JSON_String *) this; }
	JSON_Number& AsNumber() { AssertIsNumber(); return *(JSON_Number *) this; }
	JSON_Integer& AsInteger() { AssertIsInteger(); return *(JSON_Integer *) this; }
	JSON_Float& AsFloat() { AssertIsFloat(); return *(JSON_Float *) this; }
	JSON_Object& AsObject() { AssertIsObject(); return *(JSON_Object *) this; }
	JSON_Array& AsArray() { AssertIsArray(); return *(JSON_Array *) this; }
	JSON_Boolean& AsBoolean() { AssertIsBoolean(); return *(JSON_Boolean *) this; }
	JSON_Null& AsNull() { AssertIsNull(); return *(JSON_Null *) this; }
	
	const Location& GetLocation() const { return loc; }
	
	template <typename VISITOR> void Scan(VISITOR& visitor) const;
	template <typename VISITOR> bool Visit(VISITOR& visitor) const;
protected:
	friend struct JSON_Member;
	friend struct JSON_Object;
	friend struct JSON_Array;
	friend struct JSON_ValueRef;
	friend struct JSON_ValueConstRef;

	JSON_Value(JSON_ValueType _type) : type(_type), loc(), ref_count(0) {}
	JSON_Value(JSON_ValueType _type, const Location& _loc) : type(_type), loc(_loc), ref_count(0) {}
	void Catch() const { if((this != &undefined) && (this != &null)) ++ref_count; }
	void Release() const { if((this != &undefined) && (this != &null) && ref_count && (--ref_count == 0)) delete this; }
	
	JSON_ValueType type;
	Location loc;
	mutable unsigned int ref_count;
};

std::ostream& operator << (std::ostream& os, const JSON_Value& value);

// JSON String
struct JSON_String : JSON_Value
{
	JSON_String(const std::string& v) : JSON_Value(JSON_STRING), value(v) {}
	JSON_String(const char *v) : JSON_Value(JSON_STRING), value(v) {}
	JSON_String(const std::wstring& v) : JSON_Value(JSON_STRING), value(unisim::util::unicode::unicode_wstring_to_utf8_string(v)) {}
	JSON_String(const wchar_t *v) : JSON_Value(JSON_STRING), value(unisim::util::unicode::unicode_wstring_to_utf8_string(v)) {}
	JSON_String(const std::string& v, const Location& _loc) : JSON_Value(JSON_STRING, _loc), value(v) {}
	JSON_String(const char *v, const Location& _loc) : JSON_Value(JSON_STRING, _loc), value(v) {}
	JSON_String(const std::wstring& v, const Location& _loc) : JSON_Value(JSON_STRING, _loc), value(unisim::util::unicode::unicode_wstring_to_utf8_string(v)) {}
	JSON_String(const wchar_t *v, const Location& _loc) : JSON_Value(JSON_STRING, _loc), value(unisim::util::unicode::unicode_wstring_to_utf8_string(v)) {}
	virtual JSON_Value *Clone() const { return new JSON_String(value, GetLocation()); }
	
	operator const std::string& () const { return value; }
	operator std::wstring () const { return unisim::util::unicode::utf8_string_to_unicode_wstring(value); }
	operator const char * () const { return value.c_str(); }
	JSON_String& operator = (const std::string& v) { value = v; return *this; }
	JSON_String& operator = (const char *v) { value = v; return *this; }
	JSON_String& operator = (const std::wstring& v) { value = unisim::util::unicode::unicode_wstring_to_utf8_string(v); return *this; }
	JSON_String& operator = (const wchar_t *v) { value = unisim::util::unicode::unicode_wstring_to_utf8_string(v); return *this; }
protected:
	std::string value;
};

inline std::ostream& operator << (std::ostream& os, const JSON_String& value) { return os << '"' << Escape((const std::string&) value) << '"'; }

// JSON number
struct JSON_Number : JSON_Value
{
	virtual operator char () const = 0;
	virtual operator signed char () const = 0;
	virtual operator short () const = 0;
	virtual operator int () const = 0;
	virtual operator long () const = 0;
	virtual operator long long () const = 0;
	virtual operator unsigned char () const = 0;
	virtual operator unsigned short () const = 0;
	virtual operator unsigned int () const = 0;
	virtual operator unsigned long () const = 0;
	virtual operator unsigned long long () const = 0;
	virtual operator float () const = 0;
	virtual operator double () const = 0;
	virtual JSON_Number& operator = (double v) = 0;
protected:
	JSON_Number(JSON_ValueType type) : JSON_Value(type) {}
	JSON_Number(JSON_ValueType type, const Location& _loc) : JSON_Value(type, _loc) {}
};

// Integer value
struct IntValue
{
	IntValue() : sign(false), abs_value(0) {}
	IntValue(int64_t v) : sign(v < 0), abs_value((v >= 0) ? v : -v) {}
	IntValue(uint64_t v) : sign(false), abs_value(v) {}
	IntValue(bool _sign, uint64_t _abs_value) : sign(_sign), abs_value(_abs_value) {}
	
	operator char () const { return ToScalar<char>(); }
	operator signed char () const { return ToScalar<signed char>(); }
	operator short () const { return ToScalar<short>(); }
	operator int () const { return ToScalar<int>(); }
	operator long () const { return ToScalar<long>(); }
	operator long long () const { return ToScalar<long long>(); }
	operator unsigned char () const { return ToScalar<unsigned char>(); }
	operator unsigned short () const { return ToScalar<unsigned short>(); }
	operator unsigned int () const { return ToScalar<unsigned int>(); }
	operator unsigned long () const { return ToScalar<unsigned long>(); }
	operator unsigned long long () const { return ToScalar<unsigned long long>(); }
	operator float () const { return ToScalar<float>(); }
	operator double () const { return ToScalar<double>(); }
	
	IntValue& operator = (int64_t v) { sign = v < 0; abs_value = (v >= 0) ? v : -v; return *this; }
	IntValue& operator = (uint64_t v) { sign = false; abs_value = v; return *this; }
	IntValue& operator = (double v) { sign = v < 0.0; abs_value = (v >= 0.0) ? v : -v; return *this; }
	
	bool sign;
	uint64_t abs_value;
private:
	template <typename T> T ToScalar() const { return sign ? -T(abs_value) : T(abs_value); }
};

inline std::ostream& operator << (std::ostream& os, const IntValue& int_value) { if(int_value.sign) { os << '-'; } return os << int_value.abs_value; }

// JSON integer number
struct JSON_Integer : JSON_Number
{
	JSON_Integer(int64_t v) : JSON_Number(JSON_INT), int_value(v) {}
	JSON_Integer(uint64_t v) : JSON_Number(JSON_INT), int_value(v) {}
	JSON_Integer(const IntValue& _int_value) : JSON_Number(JSON_INT), int_value(_int_value) {}
	JSON_Integer(const IntValue& _int_value, const Location& _loc) : JSON_Number(JSON_INT, _loc), int_value(_int_value) {}
	virtual JSON_Value *Clone() const { return new JSON_Integer(int_value, GetLocation()); }
	virtual operator char () const { return int_value; }
	virtual operator signed char () const { return int_value; }
	virtual operator short () const { return int_value; }
	virtual operator int () const { return int_value; }
	virtual operator long () const { return int_value; }
	virtual operator long long () const { return int_value; }
	virtual operator unsigned char () const { return int_value; }
	virtual operator unsigned short () const { return int_value; }
	virtual operator unsigned int () const { return int_value; }
	virtual operator unsigned long () const { return int_value; }
	virtual operator unsigned long long () const { return int_value; }
	virtual operator float () const { return int_value; }
	virtual operator double () const { return int_value; }
	virtual JSON_Number& operator = (double v) { int_value = v; return *this; }
	JSON_Integer& operator = (int64_t v) { int_value = v; return *this; }
	JSON_Integer& operator = (uint64_t v) { int_value = v; return *this; }
protected:
	friend std::ostream& operator << (std::ostream& os, const JSON_Integer& value);
	IntValue int_value;
};

inline std::ostream& operator << (std::ostream& os, const JSON_Integer& value) { return os << value.int_value; }

// JSON floating-point number
struct JSON_Float : JSON_Number
{
	JSON_Float(double v) : JSON_Number(JSON_FLOAT), value(v) {}
	JSON_Float(double v, const Location& _loc) : JSON_Number(JSON_FLOAT, _loc), value(v) {}
	virtual JSON_Value *Clone() const { return new JSON_Float(value, GetLocation()); }
	virtual operator char () const { return ToScalar<char>(); }
	virtual operator signed char () const { return ToScalar<signed char>(); }
	virtual operator short () const { return ToScalar<short>(); }
	virtual operator int () const { return ToScalar<int>(); }
	virtual operator long () const { return ToScalar<long>(); }
	virtual operator long long () const { return ToScalar<long long>(); }
	virtual operator unsigned char () const { return ToScalar<unsigned char>(); }
	virtual operator unsigned short () const { return ToScalar<unsigned short>(); }
	virtual operator unsigned int () const { return ToScalar<unsigned int>(); }
	virtual operator unsigned long () const { return ToScalar<unsigned long>(); }
	virtual operator unsigned long long () const { return ToScalar<unsigned long long>(); }
	virtual operator float () const { return ToScalar<float>(); }
	virtual operator double () const { return ToScalar<double>(); }
	virtual JSON_Number& operator = (double v) { value = v; return *this; }
protected:
	double value;
private:
	template <typename T> T ToScalar() const { return value; }
};

inline std::ostream& operator << (std::ostream& os, const JSON_Float& value) { return os << (double) value; }

// JSON boolean value
struct JSON_Boolean : JSON_Value
{
	JSON_Boolean(bool v) : JSON_Value(JSON_BOOLEAN), value(v) {}
	JSON_Boolean(bool v, const Location& _loc) : JSON_Value(JSON_BOOLEAN, _loc), value(v) {}
	virtual JSON_Value *Clone() const { return new JSON_Boolean(value, GetLocation()); }
	operator bool () const { return value; }
	JSON_Boolean& operator = (bool v) { value = v; return *this; }
protected:
	bool value;
};

inline std::ostream& operator << (std::ostream& os, const JSON_Boolean& v) { return os << (v ? "true" : "false"); }

// JSON null value
struct JSON_Null : JSON_Value
{
	JSON_Null() : JSON_Value(JSON_NULL) {}
	JSON_Null(const Location& _loc) : JSON_Value(JSON_NULL, _loc) {}
	virtual JSON_Value *Clone() const { return new JSON_Null(GetLocation()); }
protected:
};

inline std::ostream& operator << (std::ostream& os, const JSON_Null&) { return os << "null"; }

// JSON Value Reference
struct JSON_ValueRef
{
	JSON_ValueRef() : value(&undefined) {}
	JSON_ValueRef(JSON_Value *v) : value(v) { if(value) value->Catch(); }
	JSON_ValueRef(const JSON_ValueRef& o) : value(o.value) { if(value) value->Catch(); }
	~JSON_ValueRef() { if(value) value->Release(); }
	
	JSON_Value *Clone() const { return value->Clone(); }

	operator const JSON_Value& () const { return *value; }
	operator JSON_Value& () { return *value; }
	
	bool IsDefined() const { return value->IsDefined(); }
	bool IsUndefined() const { return value->IsUndefined(); }
	bool IsString() const { return value->IsString(); }
	bool IsNumber() const { return value->IsNumber(); }
	bool IsInteger() const { return value->IsInteger(); }
	bool IsFloat() const { return value->IsFloat(); }
	bool IsObject() const { return value->IsObject(); }
	bool IsArray() const { return value->IsArray(); }
	bool IsBoolean() const { return value->IsBoolean(); }
	bool IsNull() const { return value->IsNull(); }
	
	void AssertIsString() const { value->AssertIsString(); }
	void AssertIsNumber() const { value->AssertIsNumber(); }
	void AssertIsInteger() const { value->AssertIsInteger(); }
	void AssertIsFloat() const { value->AssertIsFloat(); }
	void AssertIsObject() const { value->AssertIsObject(); }
	void AssertIsArray() const { value->AssertIsArray(); }
	void AssertIsBoolean() const { value->AssertIsBoolean(); }
	void AssertIsNull() const { value->AssertIsNull(); }
	
	const JSON_String& AsString() const { return value->AsString(); }
	const JSON_Number& AsNumber() const { return value->AsNumber(); }
	const JSON_Integer& AsInteger() const { return value->AsInteger(); }
	const JSON_Float& AsFloat() const { return value->AsFloat(); }
	const JSON_Object& AsObject() const { return value->AsObject(); }
	const JSON_Array& AsArray() const { return value->AsArray(); }
	const JSON_Boolean& AsBoolean() const { return value->AsBoolean(); }
	const JSON_Null& AsNull() const { return value->AsNull(); }

	JSON_String& AsString() { return value->AsString(); }
	JSON_Number& AsNumber() { return value->AsNumber(); }
	JSON_Integer& AsInteger() { return value->AsInteger(); }
	JSON_Float& AsFloat() { return value->AsFloat(); }
	JSON_Object& AsObject() { return value->AsObject(); }
	JSON_Array& AsArray() { return value->AsArray(); }
	JSON_Boolean& AsBoolean() { return value->AsBoolean(); }
	JSON_Null& AsNull() { return value->AsNull(); }
	
	JSON_ValueRef& operator = (const std::string& v) { if(value) value->Release(); value = new JSON_String(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (const char *v) { if(value) value->Release(); value = new JSON_String(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (const std::wstring& v) { if(value) value->Release(); value = new JSON_String(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (const wchar_t *v) { if(value) value->Release(); value = new JSON_String(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (int64_t v) { if(value) value->Release(); value = new JSON_Integer(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (uint64_t v) { if(value) value->Release(); value = new JSON_Integer(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (double v) { if(value) value->Release(); value = new JSON_Float(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (bool v) { if(value) value->Release(); value = new JSON_Boolean(v); value->Catch(); return *this; }
	JSON_ValueRef& operator = (const JSON_Value& v) { if(value) value->Release(); value = v.IsUndefined() ? &undefined : (v.IsNull() ? &null : v.Clone()); value->Catch(); return *this; }
private:
	friend struct JSON_ValueConstRef;
	
	JSON_Value *value;
};

// JSON Value Const Reference
struct JSON_ValueConstRef
{
	JSON_ValueConstRef() : value(&undefined) {}
	JSON_ValueConstRef(JSON_Value *v) : value(v) { if(value) value->Catch(); }
	JSON_ValueConstRef(const JSON_ValueConstRef& o) : value(o.value) { if(value) value->Catch(); }
	JSON_ValueConstRef(const JSON_ValueRef& o) : value(o.value) { if(value) value->Catch(); }
	~JSON_ValueConstRef() { if(value) value->Release(); }
	
	JSON_Value *Clone() const { return value->Clone(); }
	
	operator const JSON_Value& () const { return *value; }
	
	bool IsDefined() const { return value->IsDefined(); }
	bool IsUndefined() const { return value->IsUndefined(); }
	bool IsString() const { return value->IsString(); }
	bool IsNumber() const { return value->IsNumber(); }
	bool IsInteger() const { return value->IsInteger(); }
	bool IsFloat() const { return value->IsFloat(); }
	bool IsObject() const { return value->IsObject(); }
	bool IsArray() const { return value->IsArray(); }
	bool IsBoolean() const { return value->IsBoolean(); }
	bool IsNull() const { return value->IsNull(); }
	
	void AssertIsString() const { value->AssertIsString(); }
	void AssertIsNumber() const { value->AssertIsNumber(); }
	void AssertIsInteger() const { value->AssertIsInteger(); }
	void AssertIsFloat() const { value->AssertIsFloat(); }
	void AssertIsObject() const { value->AssertIsObject(); }
	void AssertIsArray() const { value->AssertIsArray(); }
	void AssertIsBoolean() const { value->AssertIsBoolean(); }
	void AssertIsNull() const { value->AssertIsNull(); }
	
	const JSON_String& AsString() const { return value->AsString(); }
	const JSON_Number& AsNumber() const { return value->AsNumber(); }
	const JSON_Integer& AsInteger() const { return value->AsInteger(); }
	const JSON_Float& AsFloat() const { return value->AsFloat(); }
	const JSON_Object& AsObject() const { return value->AsObject(); }
	const JSON_Array& AsArray() const { return value->AsArray(); }
	const JSON_Boolean& AsBoolean() const { return value->AsBoolean(); }
	const JSON_Null& AsNull() const { return value->AsNull(); }

	const JSON_String& AsString() { return value->AsString(); }
	const JSON_Number& AsNumber() { return value->AsNumber(); }
	const JSON_Integer& AsInteger() { return value->AsInteger(); }
	const JSON_Float& AsFloat() { return value->AsFloat(); }
	const JSON_Object& AsObject() { return value->AsObject(); }
	const JSON_Array& AsArray() { return value->AsArray(); }
	const JSON_Boolean& AsBoolean() { return value->AsBoolean(); }
	const JSON_Null& AsNull() { return value->AsNull(); }
	
private:
	JSON_Value *value;
};

// JSON member
struct JSON_Member
{
	JSON_Member(const char *_name) : name(_name), value(&undefined), loc(), ref_count(0) {}
	JSON_Member(const char *_name, JSON_Value *v) : name(_name), value(v), loc(), ref_count(0) {}
	JSON_Member(const char *_name, JSON_Value *v, const Location& _loc) : name(_name), value(v), loc(_loc), ref_count(0) {}
	JSON_Member(const std::string& _name, JSON_Value *v) : name(_name), value(v), loc(), ref_count(0) {}
	JSON_Member(const std::string& _name, JSON_Value *v, const Location& _loc) : name(_name), value(v), loc(_loc), ref_count(0) {}
	virtual ~JSON_Member() { Free(); }
	
	const std::string& GetName() const { return name; }
	const JSON_Value& GetValue() const { return value ? (*value) : undefined; }
	JSON_Value& GetValue() { return value ? (*value) : undefined; }
	const Location& GetLocation() const { return loc; }
	
	operator const JSON_Value& () { return GetValue(); }
	operator JSON_Value& () { return GetValue(); }
	
	JSON_Member& operator = (const std::string& v) { Free(); value = new JSON_String(v); return *this; }
	JSON_Member& operator = (const char *v) { Free(); value = new JSON_String(v); return *this; }
	JSON_Member& operator = (const std::wstring& v) { Free(); value = new JSON_String(v); return *this; }
	JSON_Member& operator = (const wchar_t *v) { Free(); value = new JSON_String(v); return *this; }
	JSON_Member& operator = (int64_t v) { Free(); value = new JSON_Integer(v); return *this; }
	JSON_Member& operator = (uint64_t v) { Free(); value = new JSON_Integer(v); return *this; }
	JSON_Member& operator = (double v) { Free(); value = new JSON_Float(v); return *this; }
	JSON_Member& operator = (bool v) { Free(); value = new JSON_Boolean(v); return *this; }
	JSON_Member& operator = (const JSON_Value& v) { Free(); value = v.IsUndefined() ? &undefined : (v.IsNull() ? &null : v.Clone()); return *this; }
	
	template <typename VISITOR> void Scan(VISITOR& visitor) const { value->Visit(visitor); }
	
protected:
	friend struct JSON_MemberRef;
	friend struct JSON_MemberConstRef;
	
	std::string name;
	JSON_Value *value;
	Location loc;
	mutable unsigned int ref_count;
	
	void Catch() const { ++ref_count; }
	void Release() const { if(ref_count && (--ref_count == 0)) delete this; }
	void Free() { if((value != &undefined) && (value != &null)) delete value; }
};

inline std::ostream& operator << (std::ostream& os, const JSON_Member& member) { return os << '"' << member.GetName() << "\":" << member.GetValue(); }

// JSON member reference
struct JSON_MemberRef
{
	JSON_MemberRef() : member(0) {}
	JSON_MemberRef(JSON_Member *_member) : member(_member) { if(member) member->Catch(); }
	JSON_MemberRef(const JSON_MemberRef& o) : member(o.member) { if(member) member->Catch(); }
	~JSON_MemberRef() { if(member) member->Release(); }
	JSON_MemberRef& operator = (const JSON_MemberRef& o) { if(member) member->Release(); member = o.member; if(member) member->Catch(); return *this; }
	
	operator const JSON_Member& () const { return *member; }
	operator JSON_Member& () { return *member; }
	const std::string& GetName() const { return member->GetName(); }
	const JSON_Value& GetValue() const { return member->GetValue(); }
	const Location& GetLocation() const { return member->GetLocation(); }

	JSON_Value *Clone() const { return member->GetValue().Clone(); }

	JSON_ValueType GetType() const { return member->GetValue().GetType(); }
	
	bool IsDefined() const { return member->GetValue().IsDefined(); }
	bool IsUndefined() const { return member->GetValue().IsUndefined(); }
	bool IsString() const { return member->GetValue().IsString(); }
	bool IsNumber() const { return member->GetValue().IsNumber(); }
	bool IsInteger() const { return member->GetValue().IsInteger(); }
	bool IsFloat() const { return member->GetValue().IsFloat(); }
	bool IsObject() const { return member->GetValue().IsObject(); }
	bool IsArray() const { return member->GetValue().IsArray(); }
	bool IsBoolean() const { return member->GetValue().IsBoolean(); }
	bool IsNull() const { return member->GetValue().IsNull(); }
	
	void AssertIsString() const { member->GetValue().AssertIsString(); }
	void AssertIsNumber() const { member->GetValue().AssertIsNumber(); }
	void AssertIsInteger() const { member->GetValue().AssertIsInteger(); }
	void AssertIsFloat() const { member->GetValue().AssertIsFloat(); }
	void AssertIsObject() const { member->GetValue().AssertIsObject(); }
	void AssertIsArray() const { member->GetValue().AssertIsArray(); }
	void AssertIsBoolean() const { member->GetValue().AssertIsBoolean(); }
	void AssertIsNull() const { member->GetValue().AssertIsNull(); }
	
	const JSON_String& AsString() const { return member->GetValue().AsString(); }
	const JSON_Number& AsNumber() const { return member->GetValue().AsNumber(); }
	const JSON_Integer& AsInteger() const { return member->GetValue().AsInteger(); }
	const JSON_Float& AsFloat() const { return member->GetValue().AsFloat(); }
	const JSON_Object& AsObject() const { return member->GetValue().AsObject(); }
	const JSON_Array& AsArray() const { return member->GetValue().AsArray(); }
	const JSON_Boolean& AsBoolean() const { return member->GetValue().AsBoolean(); }
	const JSON_Null& AsNull() const { return member->GetValue().AsNull(); }

	JSON_String& AsString() { return member->GetValue().AsString(); }
	JSON_Number& AsNumber() { return member->GetValue().AsNumber(); }
	JSON_Integer& AsInteger() { return member->GetValue().AsInteger(); }
	JSON_Float& AsFloat() { return member->GetValue().AsFloat(); }
	JSON_Object& AsObject() { return member->GetValue().AsObject(); }
	JSON_Array& AsArray() { return member->GetValue().AsArray(); }
	JSON_Boolean& AsBoolean() { return member->GetValue().AsBoolean(); }
	JSON_Null& AsNull() { return member->GetValue().AsNull(); }

	JSON_MemberRef& operator = (const std::string& v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (const char *v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (const std::wstring& v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (const wchar_t *v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (int64_t v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (uint64_t v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (double v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (bool v) { member->operator = (v); return *this; }
	JSON_MemberRef& operator = (const JSON_Value& v) { member->operator = (v); return *this; }
	
private:
	friend struct JSON_MemberConstRef;
	
	JSON_Member *member;
};

// JSON member const reference
struct JSON_MemberConstRef
{
	JSON_MemberConstRef(const JSON_Member *_member) : member(_member) { if(member) member->Catch(); }
	JSON_MemberConstRef(const JSON_MemberConstRef& o) : member(o.member) { if(member) member->Catch(); }
	JSON_MemberConstRef(const JSON_MemberRef& o) : member(o.member) { if(member) member->Catch(); }
	~JSON_MemberConstRef() { if(member) member->Release(); }
	
	operator const JSON_Member& () const { return *member; }
	const std::string& GetName() const { return member->GetName(); }
	const JSON_Value& GetValue() const { return member->GetValue(); }
	const Location& GetLocation() const { return member->GetLocation(); }

	JSON_Value *Clone() const { return member->GetValue().Clone(); }
	
	JSON_ValueType GetType() const { return member->GetValue().GetType(); }
	
	bool IsDefined() const { return member->GetValue().IsDefined(); }
	bool IsUndefined() const { return member->GetValue().IsUndefined(); }
	bool IsString() const { return member->GetValue().IsString(); }
	bool IsNumber() const { return member->GetValue().IsNumber(); }
	bool IsInteger() const { return member->GetValue().IsInteger(); }
	bool IsFloat() const { return member->GetValue().IsFloat(); }
	bool IsObject() const { return member->GetValue().IsObject(); }
	bool IsArray() const { return member->GetValue().IsArray(); }
	bool IsBoolean() const { return member->GetValue().IsBoolean(); }
	bool IsNull() const { return member->GetValue().IsNull(); }
	
	void AssertIsString() const { member->GetValue().AssertIsString(); }
	void AssertIsNumber() const { member->GetValue().AssertIsNumber(); }
	void AssertIsInteger() const { member->GetValue().AssertIsInteger(); }
	void AssertIsFloat() const { member->GetValue().AssertIsFloat(); }
	void AssertIsObject() const { member->GetValue().AssertIsObject(); }
	void AssertIsArray() const { member->GetValue().AssertIsArray(); }
	void AssertIsBoolean() const { member->GetValue().AssertIsBoolean(); }
	void AssertIsNull() const { member->GetValue().AssertIsNull(); }
	
	const JSON_String& AsString() const { return member->GetValue().AsString(); }
	const JSON_Number& AsNumber() const { return member->GetValue().AsNumber(); }
	const JSON_Integer& AsInteger() const { return member->GetValue().AsInteger(); }
	const JSON_Float& AsFloat() const { return member->GetValue().AsFloat(); }
	const JSON_Object& AsObject() const { return member->GetValue().AsObject(); }
	const JSON_Array& AsArray() const { return member->GetValue().AsArray(); }
	const JSON_Boolean& AsBoolean() const { return member->GetValue().AsBoolean(); }
	const JSON_Null& AsNull() const { return member->GetValue().AsNull(); }
	
private:
	const JSON_Member *member;
};

// JSON object
struct JSON_Object : JSON_Value
{
	typedef std::map<std::string, JSON_MemberRef> Members;
	typedef std::vector<std::string> KeysType;

	JSON_Object() : JSON_Value(JSON_OBJECT), members() {}
	JSON_Object(const Location& _loc) : JSON_Value(JSON_OBJECT, _loc), members() {}
	JSON_Object(const Members& _members) : JSON_Value(JSON_OBJECT), members(_members) {}
	JSON_Object(const Members& _members, const Location& _loc) : JSON_Value(JSON_OBJECT, _loc), members(_members) {}
	virtual ~JSON_Object() {}
	virtual JSON_Value *Clone() const;

	JSON_Object& Add(JSON_Member *member);
	const Members& GetMembers() const { return members; }
	bool HasProperty(const char *member_name) const { return FindMember(member_name) != 0; }
	KeysType Keys() const;
	JSON_MemberRef operator [] (const char *member_name);
	JSON_MemberConstRef operator [] (const char *member_name) const;
	JSON_MemberRef operator [] (const std::string& member_name) { return this->operator [] (member_name.c_str()); }
	JSON_MemberConstRef operator [] (const std::string& member_name) const { return this->operator [] (member_name.c_str()); }
	JSON_MemberRef operator [] (const std::wstring& member_name) { return this->operator [] (unisim::util::unicode::unicode_wstring_to_utf8_string(member_name).c_str()); }
	JSON_MemberConstRef operator [] (const std::wstring& member_name) const { return this->operator [] (unisim::util::unicode::unicode_wstring_to_utf8_string(member_name).c_str()); }
	
	template <typename VISITOR> void Scan(VISITOR& visitor) const;
	
private:
	Members members;
	
	JSON_Member *FindMember(const char *member_name);
	const JSON_Member *FindMember(const char *member_name) const;
};

std::ostream& operator << (std::ostream& os, const JSON_Object& object);

// JSON array
struct JSON_Array : JSON_Value
{
	typedef std::vector<JSON_ValueRef> Elements;
	typedef Elements::size_type size_type;
	
	JSON_Array() : JSON_Value(JSON_ARRAY), elements() {}
	JSON_Array(const Location& _loc) : JSON_Value(JSON_ARRAY, _loc), elements() {}
	JSON_Array(const Elements& _elements) : JSON_Value(JSON_ARRAY), elements(_elements) {}
	JSON_Array(const Elements& _elements, const Location& _loc) : JSON_Value(JSON_ARRAY, _loc), elements(_elements) {}
	virtual ~JSON_Array() {}
	virtual JSON_Value *Clone() const;
	
	JSON_Array& Push(JSON_Value *value) { elements.push_back(value); return *this; }
	const Elements& GetElements() const { return elements; }
	JSON_ValueRef& operator [] (size_type index) { if(index >= elements.size()) elements.resize(index + 1); return elements[index]; }
	JSON_ValueConstRef operator [] (size_type index) const { return (index >= elements.size()) ? JSON_ValueConstRef(&undefined) : JSON_ValueConstRef(elements[index]); }
	size_type Length() const { return elements.size(); }
	
	template <typename VISITOR> void Scan(VISITOR& visitor) const;
private:
	Elements elements;
};

std::ostream& operator << (std::ostream& os, const JSON_Array& array);

// JSON AST printer
struct JSON_AST_Printer
{
	static void Print(std::ostream& stream, const JSON_Value& root);
private:
	struct Visitor
	{
		Visitor(std::ostream& stream, const JSON_Value& value);
		bool Visit(const JSON_Value& value);
		bool Visit(const JSON_Object& object);
		bool Visit(const JSON_Member& member);
		bool Visit(const JSON_Array& array);
		void Push();
		void Pop();
		unsigned int GetIndex() const;
		void NextIndex();
		
		std::ostream& stream;
		const JSON_Value& root;
		typedef std::vector<unsigned int> Stack;
		Stack stack;
	};
};

// JSON Parser visitor for building an AST
struct JSON_AST_Builder
{
	typedef JSON_Value *JSON_VALUE_TYPE;
	typedef JSON_String *JSON_STRING_TYPE;
	typedef JSON_Object *JSON_OBJECT_TYPE;
	typedef JSON_Object::Members *JSON_MEMBERS_TYPE;
	typedef JSON_Member *JSON_MEMBER_TYPE;
	typedef JSON_Array *JSON_ARRAY_TYPE;
	typedef JSON_Array::Elements *JSON_ELEMENTS_TYPE;
	
	JSON_AST_Builder(std::ostream& _err = std::cerr) : err(_err) {}
	
	void BeginObject() {}
	void EndObject() {}
	void BeginArray() {}
	void EndArray() {}
	void Comma() {}
	JSON_String *MemberName(const std::string& name, const Location& loc) { return new JSON_String(name, loc); }
	JSON_String *Value(const std::string& value, const Location& loc) { return new JSON_String(value, loc); }
	JSON_Integer *Value(const IntValue& value, const Location& loc) { return new JSON_Integer(value, loc); }
	JSON_Float *Value(double value, const Location& loc) { return new JSON_Float(value, loc); }
	JSON_Boolean *Value(bool value, const Location& loc) { return new JSON_Boolean(value, loc); }
	JSON_Null *Value(const Location& loc) { return new JSON_Null(loc); }
	JSON_Object *Object(const Location& loc) { return new JSON_Object(loc); }
	JSON_Object *Object(JSON_MEMBERS_TYPE members, const Location& loc) { JSON_Object *object = new JSON_Object(*members, loc); delete members; return object; }
	JSON_MEMBERS_TYPE Members(JSON_MEMBERS_TYPE members, JSON_Member *member) { if(!members) members = new JSON_Object::Members(); JSON_Object::Members::iterator it = members->find(member->GetName()); if(it != members->end()) (*it).second = member; else members->insert(JSON_Object::Members::value_type(member->GetName(), member)); return members; }
	JSON_Member *Member(JSON_String *name, JSON_Value *value, const Location& loc) { JSON_Member *member = new JSON_Member((const std::string&) *name, value, loc); delete name; return member; }
	JSON_Array *Array(const Location& loc) { return new JSON_Array(loc); }
	JSON_Array *Array(JSON_ELEMENTS_TYPE elements, const Location& loc) { JSON_Array *array = new JSON_Array(*elements, loc); delete elements; return array; }
	JSON_ELEMENTS_TYPE Elements(JSON_ELEMENTS_TYPE elements, JSON_Value *value) { if(elements) elements->push_back(value); else elements = new JSON_Array::Elements(1, value); return elements; }
	
	void Error(const std::string& msg) { err << msg; }
	
	void Release(JSON_Value *value) { delete value; }
	void Release(JSON_Member *member) { delete member; }
	void Release(JSON_MEMBERS_TYPE members) { delete members; }
	void Release(JSON_ELEMENTS_TYPE elements) { delete elements; }
private:
	std::ostream& err;
};

// Token
enum Token
{
	TOK_IO_ERROR      = -2,  // I/O error
	TOK_ERROR         = -1,  // Error
	TOK_VOID          = 0,   // no token
	TOK_STRING        = 1,   // double quoted string
	TOK_INT           = 2,   // integer
	TOK_FLOAT         = 3,   // floating-point number
	TOK_TRUE          = 4,   // true
	TOK_FALSE         = 5,   // false
	TOK_NULL          = 6,   // null
	TOK_LEFT_BRACE    = '{', // {
	TOK_RIGHT_BRACE   = '}', // }
	TOK_LEFT_BRACKET  = '[', // [
	TOK_RIGHT_BRACKET = ']', // ]
	TOK_COMMA         = ',', // ,
	TOK_COLON         = ':', // :
	TOK_EOF           = 127  // end-of-file
};

std::ostream& operator << (std::ostream& os, const Token& token);

std::string PrettyString(const Token& token, const std::string& text);

enum JSON_OPTION
{
	OPT_ENABLE_C_COMMENTS,  // enable C-like comments beginning with /* and ending with */
	OPT_ENABLE_CPP_COMMENTS // enable C++-like comments beginning with //
};

// JSON Lexer
template <typename VISITOR>
struct JSON_Lexer
{
	JSON_Lexer();
	~JSON_Lexer();
	
	bool SetOption(JSON_OPTION option, bool value);
	bool GetOption(JSON_OPTION option, bool& value) const;
	bool GetOptionFlag(JSON_OPTION option) const;
	void Reset();
protected:
	Token Next(std::istream& stream, VISITOR& visitor);
	void Back();
	const std::string& GetText() const { return text; }
	const std::string& GetStringValue() const { return str_value; }
	const IntValue& GetIntValue() const { return int_value; }
	double GetFloatValue() const { return float_value; }
	const Location& GetLocation() const { return loc; }
	const Location& GetTokenLocation() const { return token_loc; }
	void ParseError(std::istream& stream, VISITOR& visitor, const std::string& msg);
private:
	void Append(char c);
	void Scan(std::istream& stream, VISITOR& visitor);
	void ScanError(std::istream& stream, VISITOR& visitor, const std::string& msg);
	void Error(std::istream& stream, VISITOR& visitor, const std::string& msg, bool parse_error);
	
	bool enable_c_comments;
	bool enable_cpp_comments;
	bool eof;
	bool error;
	Token token;
	Token look_ahead;
	std::string line;
	std::string text;
	std::string str_value;
	IntValue int_value;
	double float_value;
	Location loc;
	Location token_loc;
	unisim::util::dictionary::Dictionary<Token> token_dictionary;
};

// JSON Parser
template <typename VISITOR>
struct JSON_Parser : JSON_Lexer<VISITOR>
{
	typedef typename VISITOR::JSON_VALUE_TYPE JSON_VALUE_TYPE;
	typedef typename VISITOR::JSON_STRING_TYPE JSON_STRING_TYPE;
	typedef typename VISITOR::JSON_OBJECT_TYPE JSON_OBJECT_TYPE;
	typedef typename VISITOR::JSON_MEMBERS_TYPE JSON_MEMBERS_TYPE;
	typedef typename VISITOR::JSON_MEMBER_TYPE JSON_MEMBER_TYPE;
	typedef typename VISITOR::JSON_ARRAY_TYPE JSON_ARRAY_TYPE;
	typedef typename VISITOR::JSON_ELEMENTS_TYPE JSON_ELEMENTS_TYPE;
	typedef JSON_Lexer<VISITOR> Lexer;
	
	JSON_Parser();
	~JSON_Parser();
	
	JSON_VALUE_TYPE Parse(std::istream& stream, VISITOR& visitor);
private:
	JSON_VALUE_TYPE ParseValue(std::istream& stream, VISITOR& visitor);
	JSON_OBJECT_TYPE ParseObject(std::istream& stream, VISITOR& visitor);
	JSON_MEMBERS_TYPE ParseMembers(std::istream& stream, VISITOR& visitor, JSON_MEMBERS_TYPE members = JSON_MEMBERS_TYPE());
	JSON_MEMBER_TYPE ParseMember(std::istream& stream, VISITOR& visitor);
	JSON_ARRAY_TYPE ParseArray(std::istream& stream, VISITOR& visitor);
	JSON_ELEMENTS_TYPE ParseElements(std::istream& stream, VISITOR& visitor, JSON_ELEMENTS_TYPE elements = JSON_ELEMENTS_TYPE());
};

// JSON Parser visitor for raw printing of input being parsed
struct JSON_Parser_Printer
{
	typedef bool JSON_VALUE_TYPE;
	typedef std::string JSON_STRING_TYPE;
	typedef bool JSON_OBJECT_TYPE;
	typedef bool JSON_MEMBERS_TYPE;
	typedef bool JSON_MEMBER_TYPE;
	typedef bool JSON_ARRAY_TYPE;
	typedef bool JSON_ELEMENTS_TYPE;

	JSON_Parser_Printer(std::ostream& _out = std::cout, std::ostream& _err = std::cerr) : out(_out), err(_err) {}
	
	void BeginObject() { out << '{'; }
	void EndObject() { out << '}'; }
	void BeginArray() { out << '['; }
	void EndArray() { out << ']'; }
	void Comma() { out << ','; }
	
	const std::string& MemberName(const std::string& name, const Location& loc) { out << '"' << Escape(name) << '"'; return name; }
	bool Value(const std::string& value, const Location&) { out << '"' << Escape(value) << '"'; return true; }
	bool Value(const IntValue& value, const Location&) { out << value; return true; }
	bool Value(double value, const Location&) { out << value; return true; }
	bool Value(bool value, const Location&) { out << (value ? "true" : "false"); return true; }
	bool Value(const Location&) { out << "null"; return true; }
	bool Object(const Location&) { return true; }
	bool Object(bool, const Location&) { return true; }
	bool Members(bool, bool) { return true; }
	bool Member(const std::string& name, bool, const Location&) { return true; }
	bool Array(const Location&) { return true; }
	bool Array(bool, const Location&) { return true; }
	bool Elements(bool, bool) { return true; }
	
	void Error(const std::string& msg) { err << std::endl << msg; }
	
	void Release(bool) {}
	void Release(std::string) {}
private:
	std::ostream& out;
	std::ostream& err;
};

} // end of namespace json
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_JSON_JSON_HH__
