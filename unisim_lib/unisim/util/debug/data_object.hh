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

#ifndef __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__
#define __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/type.hh>
#include <unisim/util/json/json.hh>
#include <string>
#include <stdexcept>

namespace unisim {
namespace service {
namespace interfaces {

template <class ADDRESS> class DataObjectLookup;

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace util {
namespace debug {

template <class ADDRESS> class DataObject;
template <class ADDRESS> class DataObjectRef;

namespace {

template <class ADDRESS, typename T>
T Sum(const DataObject<ADDRESS>& data_object, T value)
{
	return T(data_object) + value;
}

template <class ADDRESS, typename T>
T Difference(const DataObject<ADDRESS>& data_object, T value)
{
	return T(data_object) - value;
}

template <class ADDRESS, typename T>
T Difference(T value, const DataObject<ADDRESS>& data_object)
{
	return value - T(data_object);
}

template <class ADDRESS, typename T>
T Multiply(const DataObject<ADDRESS>& data_object, T value)
{
	return T(data_object) * value;
}

template <class ADDRESS, typename T>
T Divide(const DataObject<ADDRESS>& data_object, T value)
{
	return T(data_object) / value;
}

template <class ADDRESS, typename T>
T Divide(T value, const DataObject<ADDRESS>& data_object)
{
	return value / T(data_object);
}

template <class ADDRESS, typename T>
T Modulo(const DataObject<ADDRESS>& data_object, T value)
{
	return T(data_object) % value;
}

template <class ADDRESS, typename T>
T Modulo(T value, const DataObject<ADDRESS>& data_object)
{
	return value % T(data_object);
}

template <class ADDRESS, typename T>
T BitwiseAnd(const DataObject<ADDRESS>& data_object, T value)
{
	return value & T(data_object);
}

template <class ADDRESS, typename T>
T BitwiseOr(const DataObject<ADDRESS>& data_object, T value)
{
	return value | T(data_object);
}

template <class ADDRESS, typename T>
T BitwiseXor(const DataObject<ADDRESS>& data_object, T value)
{
	return value ^ T(data_object);
}

template <class ADDRESS>
long long BitwiseNot(const DataObject<ADDRESS>& data_object)
{
	return ~(long long) data_object;
}

template <class ADDRESS>
long long ShiftRight(const DataObject<ADDRESS>& data_object, unsigned int n)
{
	return (long long) data_object >> n;
}

template <class ADDRESS>
long long ShiftLeft(const DataObject<ADDRESS>& data_object, unsigned int n)
{
	return (long long) data_object << n;
}

template <class ADDRESS>
bool LogicalAnd(const DataObject<ADDRESS>& data_object, bool value)
{
	return value && bool(data_object);
}

template <class ADDRESS>
bool LogicalOr(const DataObject<ADDRESS>& data_object, bool value)
{
	return value || bool(data_object);
}

template <class ADDRESS, typename T>
long long Not(const DataObject<ADDRESS>& data_object)
{
	return !bool(data_object);
}

} // end of anonymous namespace

template <class ADDRESS>
class DataObject
{
public:
	class Error : public std::runtime_error
	{
	public:
		Error(const char *msg);
		Error(const std::string& msg);
	};
	
	class ReadError : public Error
	{
	public:
		ReadError(const char *data_object_name);
		ReadError(const std::string& data_object_name);
	};
	
	class WriteError : public Error
	{
	public:
		WriteError(const char *data_object_name);
		WriteError(const std::string& data_object_name);
	};
	
	class TypeError : public Error
	{
	public:
		TypeError(const char *data_object_name, const char *reason = 0);
		TypeError(const std::string& data_object_name, const std::string& reason = std::string());
	};
	
	class UndefinedReferenceError : public Error
	{
	public:
		UndefinedReferenceError();
		UndefinedReferenceError(const char *data_object_name);
		UndefinedReferenceError(const std::string& data_object_name);
	};
	
	class DoesNotExistError : public Error
	{
	public:
		DoesNotExistError(const char *data_object_name);
		DoesNotExistError(const std::string& data_object_name);
	};
	
	class OptimizedOutError : public Error
	{
	public:
		OptimizedOutError(const char *data_object_name);
		OptimizedOutError(const std::string& data_object_name);
	};
	
	virtual ~DataObject() {}
	virtual const char *GetName() const = 0;
	virtual unsigned int GetProcessorNumber() const = 0;
	virtual ADDRESS GetBitSize() const = 0;
	virtual const Type *GetType() const = 0; // returns (possibly) cv-qualified type
	inline const Type *GetCVUnqualifiedType() const; // returns cv-unqualified type
	virtual unisim::util::endian::endian_type GetEndian() const = 0;
	virtual bool Exists() const = 0;
	virtual bool IsOptimizedOut() const = 0;
	virtual bool GetAddress(ADDRESS& addr) const = 0;
	virtual bool Read(ADDRESS obj_bit_offset, uint64_t& value, ADDRESS bit_size) const = 0;
	virtual bool Write(ADDRESS obj_bit_offset, uint64_t value, ADDRESS bit_size) = 0;
	virtual bool Read(ADDRESS obj_bit_offset, void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) const = 0;
	virtual bool Write(ADDRESS obj_bit_offset, const void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) = 0;
	
	// testing type
	bool IsComposite() const { return GetCVUnqualifiedType()->IsComposite(); } // either structure, union, class, or interface
	bool IsObject() const { return IsComposite(); } // same as composite
	bool IsStructure() const { return GetCVUnqualifiedType()->IsStructure(); }
	bool IsUnion() const { return GetCVUnqualifiedType()->IsUnion(); }
	bool IsClass() const { return GetCVUnqualifiedType()->IsClass(); }
	bool IsInterface() const { return GetCVUnqualifiedType()->IsInterface(); }
	bool IsBasic() const { return GetCVUnqualifiedType()->IsBase(); } // either char, integer, float, or boolean
	bool IsChar() const { return GetCVUnqualifiedType()->IsChar(); }
	bool IsInteger() const { return GetCVUnqualifiedType()->IsInteger(); }
	bool IsBoolean() const { return GetCVUnqualifiedType()->IsBoolean();; }
	bool IsFloat() const { return GetCVUnqualifiedType()->IsFloat(); }
	bool IsPointer() const { return GetCVUnqualifiedType()->IsPointer(); }
	bool IsFunction() const { return GetCVUnqualifiedType()->IsFunction(); }
	bool IsArray() const { return GetCVUnqualifiedType()->IsArray(); }
	bool IsConst() const { return unisim::util::debug::TypeIsConst::Test(GetType()); }
	bool IsEnum() const { return GetCVUnqualifiedType()->IsEnum(); }
	bool IsVoid() const { return GetCVUnqualifiedType()->IsUnspecified(); } // has an unspecified type
	bool IsVolatile() const { return unisim::util::debug::TypeIsVolatile::Test(GetType()); }
	bool IsCharPointer() const { return unisim::util::debug::TypeIsCharPointer::Test(GetType()); }
	bool IsSigned() const { return unisim::util::debug::TypeIsSigned::Test(GetCVUnqualifiedType()); } // for char and integer
	
	// conversion operators
	virtual operator bool() const = 0;
	virtual operator char() const = 0;
	virtual operator signed char() const = 0;
	virtual operator short() const = 0;
	virtual operator int() const = 0;
	virtual operator long() const = 0;
	virtual operator long long() const = 0;
	virtual operator unsigned char() const = 0;
	virtual operator unsigned short() const = 0;
	virtual operator unsigned int() const = 0;
	virtual operator unsigned long() const = 0;
	virtual operator unsigned long long() const = 0;
	virtual operator float() const = 0;
	virtual operator double() const = 0;
	virtual operator std::string() const = 0;

	// assignment operators
	virtual DataObject<ADDRESS>& operator = (bool value) = 0;
	virtual DataObject<ADDRESS>& operator = (signed char value) = 0;
	virtual DataObject<ADDRESS>& operator = (short value) = 0;
	virtual DataObject<ADDRESS>& operator = (int value) = 0;
	virtual DataObject<ADDRESS>& operator = (long value) = 0;
	virtual DataObject<ADDRESS>& operator = (long long value) = 0;
	virtual DataObject<ADDRESS>& operator = (unsigned char value) = 0;
	virtual DataObject<ADDRESS>& operator = (unsigned short value) = 0;
	virtual DataObject<ADDRESS>& operator = (unsigned int value) = 0;
	virtual DataObject<ADDRESS>& operator = (unsigned long value) = 0;
	virtual DataObject<ADDRESS>& operator = (unsigned long long value) = 0;
	virtual DataObject<ADDRESS>& operator = (float value) = 0;
	virtual DataObject<ADDRESS>& operator = (double value) = 0;
	virtual DataObject<ADDRESS>& operator = (const std::string& value) = 0;
	
	// dereferencing operator
	virtual DataObjectRef<ADDRESS> operator * () = 0;
	virtual const DataObjectRef<ADDRESS> operator * () const = 0;
	
	// object property accessors
	virtual DataObjectRef<ADDRESS> operator [] (const char *property_name) = 0;
	virtual const DataObjectRef<ADDRESS> operator [] (const char *property_name) const = 0;
	DataObjectRef<ADDRESS> operator [] (const std::string& property_name) { return this->operator [] (property_name.c_str()); }
	const DataObjectRef<ADDRESS> operator [] (const std::string& property_name) const { return this->operator [] (property_name.c_str()); }
	template <typename VISITOR, typename T = bool> T ScanProperties(VISITOR& visitor) const;
	template <typename VISITOR, typename T = bool> T ScanProperties(VISITOR& visitor);
	bool HasProperty(const char *property_name) const { return IsComposite() && GetCVUnqualifiedType()->AsComposite().HasMember(property_name); }
	bool HasProperty(const std::string& property_name) const { return IsComposite() && GetCVUnqualifiedType()->AsComposite().HasMember(property_name.c_str()); }
	
	// array item accessors
	virtual DataObjectRef<ADDRESS> operator [] (int64_t subscript) = 0;
	virtual const DataObjectRef<ADDRESS> operator [] (int64_t subscript) const = 0;
	template <typename VISITOR, typename T = bool> T ScanItems(VISITOR& visitor) const;
	template <typename VISITOR, typename T = bool> T ScanItems(VISITOR& visitor);
	int64_t GetLowerBound() const { return IsArray() ? GetCVUnqualifiedType()->AsArray().GetLowerBound() : 0; }
	int64_t GetUpperBound() const { return IsArray() ? GetCVUnqualifiedType()->AsArray().GetUpperBound() : 0; }
	int64_t GetLength() const { return IsArray() ? GetCVUnqualifiedType()->AsArray().GetCount() : 0; }
	
	// arithmetic operators
	template <typename T> DataObject<ADDRESS>& operator += (T value) { return this->operator = (Sum<ADDRESS, T>(*this, value)); }
	template <typename T> DataObject<ADDRESS>& operator -= (T value) { return this->operator = (Difference<ADDRESS, T>(*this, value)); }
	template <typename T> DataObject<ADDRESS>& operator *= (T value) { return this->operator = (Multiply<ADDRESS, T>(*this, value)); }
	template <typename T> DataObject<ADDRESS>& operator /= (T value) { return this->operator = (Divide<ADDRESS, T>(*this, value)); }
	template <typename T> DataObject<ADDRESS>& operator %= (T value) { return this->operator = (Divide<ADDRESS, T>(*this, value)); }
	DataObject<ADDRESS>& operator ++ () { return this->operator = (Sum<ADDRESS, long long>(*this, 1LL)); } // Prefix
	DataObject<ADDRESS>& operator -- () { return this->operator = (Difference<ADDRESS, long long>(*this, 1LL)); } // Prefix

	// bitwise operators
	template <typename T> DataObject<ADDRESS>& operator &= (T value) { return this->operator = (BitwiseAnd<ADDRESS, T>(*this, value)); }
	template <typename T> DataObject<ADDRESS>& operator |= (T value) { return this->operator = (BitwiseOr<ADDRESS, T>(*this, value)); }
	template <typename T> DataObject<ADDRESS>& operator ^= (T value) { return this->operator = (BitwiseXor<ADDRESS, T>(*this, value)); }
	template <typename T> DataObject<ADDRESS>& operator >>= (unsigned int n) { return this->operator = (ShiftRight<ADDRESS>(*this, n)); }
	template <typename T> DataObject<ADDRESS>& operator <<= (unsigned int n) { return this->operator = (ShiftLeft<ADDRESS>(*this, n)); }

protected:
	DataObject() : ref_count(0), cv_unqualified_type(0) {}
private:
	friend class DataObjectRef<ADDRESS>;
	
	unsigned int ref_count;
	mutable const Type *cv_unqualified_type;
	
	template <typename T> int CompareBaseType(T value) const { return this->operator T() - value; }
	template <typename T> T Sum(T value) const { return this->operator T() + value; }
	void Acquire() { ++ref_count; }
	void Release() { if(ref_count && (--ref_count == 0)) delete this; }
};

template <class ADDRESS>
inline const Type *DataObject<ADDRESS>::GetCVUnqualifiedType() const
{
	return cv_unqualified_type ? cv_unqualified_type : (cv_unqualified_type = unisim::util::debug::TypeResolver::Resolve(GetType()));
}

template <class ADDRESS, typename T>
bool operator == (const DataObject<ADDRESS>& data_object, T value) { return Difference<ADDRESS, T>(data_object, value) == 0; }

template <class ADDRESS, typename T>
bool operator == (T value, const DataObject<ADDRESS>& data_object) { return Difference<ADDRESS, T>(value, data_object) == 0; }

template <class ADDRESS, typename T>
bool operator != (const DataObject<ADDRESS>& data_object, T value) { return Difference<ADDRESS, T>(data_object, value) != 0; }

template <class ADDRESS, typename T>
bool operator != (T value, const DataObject<ADDRESS>& data_object) { return Difference<ADDRESS, T>(value, data_object) != 0; }

template <class ADDRESS, typename T>
bool operator < (const DataObject<ADDRESS>& data_object, T value) { return Difference<ADDRESS, T>(data_object, value) < 0; }

template <class ADDRESS, typename T>
bool operator < (T value, const DataObject<ADDRESS>& data_object) { return Difference<ADDRESS, T>(value, data_object) < 0; }

template <class ADDRESS, typename T>
bool operator > (const DataObject<ADDRESS>& data_object, T value) { return Difference<ADDRESS, T>(data_object, value) > 0; }

template <class ADDRESS, typename T>
bool operator > (T value, const DataObject<ADDRESS>& data_object) { return Difference<ADDRESS, T>(value, data_object) > 0; }

template <class ADDRESS, typename T>
T operator + (const DataObject<ADDRESS>& data_object, T value) { return Sum<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator + (T value, const DataObject<ADDRESS>& data_object) { return Sum<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator - (const DataObject<ADDRESS>& data_object, T value) { return Difference<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator - (T value, const DataObject<ADDRESS>& data_object) { return Difference<ADDRESS, T>(value, data_object); }

template <class ADDRESS, typename T>
T operator * (const DataObject<ADDRESS>& data_object, T value) { return Difference<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator * (T value, const DataObject<ADDRESS>& data_object) { return Difference<ADDRESS, T>(value, data_object); }

template <class ADDRESS, typename T>
T operator / (const DataObject<ADDRESS>& data_object, T value) { return Divide<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator / (T value, const DataObject<ADDRESS>& data_object) { return Divide<ADDRESS, T>(value, data_object); }

template <class ADDRESS, typename T>
T operator % (const DataObject<ADDRESS>& data_object, T value) { return Modulo<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator % (T value, const DataObject<ADDRESS>& data_object) { return Modulo<ADDRESS, T>(value, data_object); }

template <class ADDRESS, typename T>
T operator & (const DataObject<ADDRESS>& data_object, T value) { return BitwiseAnd<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator & (T value, const DataObject<ADDRESS>& data_object) { return BitwiseAnd<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator | (const DataObject<ADDRESS>& data_object, T value) { return BitwiseOr<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator | (T value, const DataObject<ADDRESS>& data_object) { return BitwiseOr<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator ^ (const DataObject<ADDRESS>& data_object, T value) { return BitwiseXor<ADDRESS, T>(data_object, value); }

template <class ADDRESS, typename T>
T operator ^ (T value, const DataObject<ADDRESS>& data_object) { return BitwiseXor<ADDRESS, T>(data_object, value); }

template <class ADDRESS>
long long operator ~ (const DataObject<ADDRESS>& data_object) { return BitwiseNot<ADDRESS>(data_object); }

template <class ADDRESS>
long long operator >> (const DataObject<ADDRESS>& data_object, unsigned int n) { return ShiftRight<ADDRESS>(data_object, n); }

template <class ADDRESS>
long long operator << (const DataObject<ADDRESS>& data_object, unsigned int n) { return ShiftLeft<ADDRESS>(data_object, n); }

template <class ADDRESS, typename T>
bool operator && (const DataObject<ADDRESS>& data_object, bool value) { return LogicalAnd<ADDRESS>(data_object, value); }

template <class ADDRESS, typename T>
bool operator && (bool value, const DataObject<ADDRESS>& data_object) { return LogicalAnd<ADDRESS>(data_object, value); }

template <class ADDRESS, typename T>
bool operator || (const DataObject<ADDRESS>& data_object, bool value) { return LogicalOr<ADDRESS>(data_object, value); }

template <class ADDRESS, typename T>
bool operator || (bool value, const DataObject<ADDRESS>& data_object) { return LogicalOr<ADDRESS>(data_object, value); }

template <class ADDRESS>
bool operator ! (const DataObject<ADDRESS>& data_object) { return Not<ADDRESS>(data_object); }

template <class ADDRESS>
class DataObjectRef
{
public:
	static DataObjectRef<ADDRESS> Undefined;
	
	DataObjectRef() : data_object(0) {}
	DataObjectRef(DataObject<ADDRESS> *_data_object) : data_object(_data_object) { if(data_object) data_object->Acquire(); }
	DataObjectRef(const DataObjectRef<ADDRESS>& o) : data_object(o.data_object) { if(data_object) data_object->Acquire(); }
	DataObjectRef<ADDRESS>& operator = (const DataObjectRef<ADDRESS>& o) { if(data_object) data_object->Release(); data_object = o.data_object; if(data_object) data_object->Acquire(); return *this; }
	~DataObjectRef() { if(data_object) data_object->Release(); }
	
	bool IsDefined() const { return data_object != 0; }
	bool IsUndefined() const { return data_object == 0; }
	bool operator == (const DataObjectRef<ADDRESS>& o) const { return data_object == o.data_object; }
	bool operator != (const DataObjectRef<ADDRESS>& o) const { return data_object != o.data_object; }
	
	const char *GetName() const { Check(); return data_object->GetName(); }
	unsigned int GetProcessorNumber() const { Check(); return data_object->GetProcessorNumber(); }
	ADDRESS GetBitSize() const { Check(); return data_object->GetBitSize(); }
	const Type *GetType() const { Check(); return data_object->GetType(); }
	const Type *GetCVUnqualifiedType() const { Check(); return data_object->GetCVUnqualifiedType(); } 
	unisim::util::endian::endian_type GetEndian() const { Check(); return data_object->GetEndian(); }
	bool Exists() const { Check(); return data_object->Exists(); }
	bool IsOptimizedOut() const { Check(); return data_object->IsOptimizedOut(); }
	bool GetAddress(ADDRESS& addr) const { Check(); return data_object->GetAddress(addr); }
	bool Read(ADDRESS obj_bit_offset, uint64_t& value, ADDRESS bit_size) const { Check(); return data_object->Read(obj_bit_offset, value, bit_size); }
	bool Write(ADDRESS obj_bit_offset, uint64_t value, ADDRESS bit_size) { Check(); return data_object->Write(obj_bit_offset, value, bit_size); }
	bool Read(ADDRESS obj_bit_offset, void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) const { Check(); return data_object->Read(obj_bit_offset, buffer, buf_bit_offset, bit_size); }
	bool Write(ADDRESS obj_bit_offset, const void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) { Check(); return data_object->Write(obj_bit_offset, buffer, buf_bit_offset, bit_size); }
	void Write(const DataObjectRef<ADDRESS>& src_data_object);

	// testing type
	bool IsComposite() const { Check(); return data_object->IsComposite(); }
	bool IsObject() const { Check(); return data_object->IsObject(); }
	bool IsStructure() const { Check(); return data_object->IsStructure(); }
	bool IsUnion() const { Check(); return data_object->IsUnion(); }
	bool IsClass() const { Check(); return data_object->IsClass(); }
	bool IsInterface() const { Check(); return data_object->IsInterface(); }
	bool IsBasic() const { Check(); return data_object->IsBasic(); }
	bool IsChar() const { Check(); return data_object->IsChar(); }
	bool IsInteger() const { Check(); return data_object->IsInteger(); }
	bool IsBoolean() const { Check(); return data_object->IsBoolean(); }
	bool IsFloat() const { Check(); return data_object->IsFloat(); }
	bool IsPointer() const { Check(); return data_object->IsPointer(); }
	bool IsFunction() const { Check(); return data_object->IsFunction(); }
	bool IsArray() const { Check(); return data_object->IsArray(); }
	bool IsConst() const { Check(); return data_object->IsConst(); }
	bool IsEnum() const { Check(); return data_object->IsEnum(); }
	bool IsVoid() const { Check(); return data_object->IsVoid(); }
	bool IsVolatile() const { Check(); return data_object->IsVolatile(); }
	bool IsCharPointer() const { Check(); return data_object->IsCharPointer(); }
	bool IsSigned() const { Check(); return data_object->IsSigned(); }
	
	// conversion operators
	operator bool() const { Check(); return data_object->operator bool(); }
	operator char() const { Check(); return data_object->operator char(); }
	operator signed char() const { Check(); return data_object->operator signed char(); }
	operator short() const { Check(); return data_object->operator short(); }
	operator int() const { Check(); return data_object->operator int(); }
	operator long() const { Check(); return data_object->operator long(); }
	operator long long() const { Check(); return data_object->operator long long(); }
	operator unsigned char() const { Check(); return data_object->operator unsigned char(); }
	operator unsigned short() const { Check(); return data_object->operator unsigned short(); }
	operator unsigned int() const { Check(); return data_object->operator unsigned int(); }
	operator unsigned long() const { Check(); return data_object->operator unsigned long(); }
	operator unsigned long long() const { Check(); return data_object->operator unsigned long long(); }
	operator float() const { Check(); return data_object->operator float(); }
	operator double() const { Check(); return data_object->operator double(); }
	operator std::string() const { Check(); return data_object->operator std::string(); }
	operator DataObject<ADDRESS>& () { Check(); return *data_object; }
	operator DataObject<ADDRESS>& () const { Check(); return *data_object; }

	// assignment operators
	DataObjectRef<ADDRESS>& operator = (bool value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (signed char value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (short value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (int value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (long value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (long long value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (unsigned char value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (unsigned short value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (unsigned int value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (unsigned long value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (unsigned long long value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (float value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (double value) { Check(); data_object->operator = (value); return *this; }
	DataObjectRef<ADDRESS>& operator = (const std::string& value) { Check(); data_object->operator = (value); return *this; }
	
	// dereferencing operator
	DataObjectRef<ADDRESS> operator * () { Check(); return data_object->operator *(); }
	const DataObjectRef<ADDRESS> operator * () const { Check(); return data_object->operator *(); }
	
	// object property accessors
	DataObjectRef<ADDRESS> operator [] (const char *property_name) { Check(); return data_object->operator [] (property_name); }
	const DataObjectRef<ADDRESS> operator [] (const char *property_name) const { Check(); return data_object->operator [] (property_name); }
	DataObjectRef<ADDRESS> operator [] (const std::string& property_name) { Check(); return data_object->operator [] (property_name); }
	const DataObjectRef<ADDRESS> operator [] (const std::string& property_name) const { Check(); return data_object->operator [] (property_name); }
	template <typename VISITOR, typename T = bool> T ScanProperties(VISITOR& visitor) const { Check(); return data_object->template ScanProperties<VISITOR, T>(visitor); }
	bool HasProperty(const char *property_name) const { Check(); return data_object->HasProperty(property_name); }
	bool HasProperty(const std::string& property_name) const { Check(); return data_object->HasProperty(property_name); }
	
	// array item accessors
	DataObjectRef<ADDRESS> operator [] (int64_t subscript) { Check(); return data_object->operator [](subscript); }
	const DataObjectRef<ADDRESS> operator [] (int64_t subscript) const { Check(); return data_object->operator [](subscript); }
	template <typename VISITOR, typename T = bool> T ScanItems(VISITOR& visitor) const { Check(); return data_object->template ScanItems<VISITOR, T>(visitor); }
	int64_t GetLowerBound() const { Check(); return data_object->GetLowerBound(); }
	int64_t GetUpperBound() const { Check(); return data_object->GetUpperBound(); }
	int64_t GetLength() const { Check(); return data_object->GetLength(); }
	
	// arithmetic operators
	template <typename T> DataObjectRef<ADDRESS>& operator += (T value) { Check(); data_object->operator += (value); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator -= (T value) { Check(); data_object->operator -= (value); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator *= (T value) { Check(); data_object->operator *= (value); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator /= (T value) { Check(); data_object->operator /= (value); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator %= (T value) { Check(); data_object->operator %= (value); return *this; }
	DataObjectRef<ADDRESS>& operator ++ () { Check(); data_object->operator ++ (); return *this; } // Prefix
	DataObject<ADDRESS>& operator -- () { Check(); data_object->operator -- (); return *this; } // Prefix

	// bitwise operators
	template <typename T> DataObjectRef<ADDRESS>& operator &= (T value) { Check(); data_object->operator &= (value); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator |= (T value) { Check(); data_object->operator |= (value); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator ^= (T value) { Check(); data_object->operator ^= (value); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator >>= (unsigned int n) { Check(); data_object->operator >>= (n); return *this; }
	template <typename T> DataObjectRef<ADDRESS>& operator <<= (unsigned int n) { Check(); data_object->operator <<= (n); return *this; }
	
private:
	template <typename T>
	friend bool operator == (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) == value; }

	template <typename T>
	friend bool operator == (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return (*data_object_ref.data_object) == value; }

	template <typename T>
	friend bool operator != (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) != value; }

	template <typename T>
	friend bool operator != (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return (*data_object_ref.data_object) != value; }

	template <typename T>
	friend bool operator < (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) < value; }

	template <typename T>
	friend bool operator < (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value < (*data_object_ref.data_object); }

	template <typename T>
	friend bool operator > (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) > value; }

	template <typename T>
	friend bool operator > (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value > (*data_object_ref.data_object); }

	template <typename T>
	friend T operator + (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) +value; }

	template <typename T>
	friend T operator + (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value + (*data_object_ref.data_object); }

	template <typename T>
	friend T operator - (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) - value; }

	template <typename T>
	friend T operator - (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value - (*data_object_ref.data_object); }

	template <typename T>
	friend T operator * (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) * value; }

	template <typename T>
	friend T operator * (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value * (*data_object_ref.data_object); }

	template <typename T>
	friend T operator / (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) / value; }

	template <typename T>
	friend T operator / (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value / (*data_object_ref.data_object); }

	template <typename T>
	friend T operator % (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) % value;; }

	template <typename T>
	friend T operator % (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value % (*data_object_ref.data_object); }

	template <typename T>
	friend T operator & (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) & value; }

	template <typename T>
	friend T operator & (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value & (*data_object_ref.data_object); }

	template <typename T>
	friend T operator | (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) | value; }

	template <typename T>
	friend T operator | (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value | (*data_object_ref.data_object); }

	template <typename T>
	friend T operator ^ (const DataObjectRef<ADDRESS>& data_object_ref, T value) { data_object_ref.Check(); return (*data_object_ref.data_object) ^ value; }

	template <typename T>
	friend T operator ^ (T value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value ^ (*data_object_ref.data_object); }

	template <typename T>
	friend long long operator ~ (const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return ~(*data_object_ref.data_object); }

	template <typename T>
	friend long long operator >> (const DataObjectRef<ADDRESS>& data_object_ref, unsigned int n) { data_object_ref.Check(); return (*data_object_ref.data_object) >> n; }

	template <typename T>
	friend long long operator << (const DataObjectRef<ADDRESS>& data_object_ref, unsigned int n) { data_object_ref.Check(); return (*data_object_ref.data_object) << n; }

	template <typename T>
	friend bool operator && (const DataObjectRef<ADDRESS>& data_object_ref, bool value) { data_object_ref.Check(); return (*data_object_ref.data_object) && value; }

	template <typename T>
	friend bool operator && (bool value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value && (*data_object_ref.data_object); }

	template <typename T>
	friend bool operator || (const DataObjectRef<ADDRESS>& data_object_ref, bool value) { data_object_ref.Check(); return (*data_object_ref.data_object) || value; }

	template <typename T>
	friend bool operator || (bool value, const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return value || (*data_object_ref.data_object); }

	template <typename T>
	friend bool operator ! (const DataObjectRef<ADDRESS>& data_object_ref) { data_object_ref.Check(); return !(*data_object_ref.data_object); }

	DataObject<ADDRESS> *data_object;
	
	void Check() const { if(!data_object) throw typename DataObject<ADDRESS>::UndefinedReferenceError(); }
};

template <typename ADDRESS>
class DataObject2JSON
{
public:
	enum Mode
	{
		STRICT,
		LAZY
	};
	
	enum StatusEnum
	{
		OK                       = 0,
		DOES_NOT_EXIST           = 1,
		OPTIMIZED_OUT            = 2,
		READ_ERROR               = 4,
		NOT_FOUND                = 8,
		UNSUPPORTED_FLOAT_FORMAT = 16
	};
	
	typedef unsigned int Status;
	
	DataObject2JSON(const unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if, Mode mode = STRICT);
	
	Status Do(const DataObjectRef<ADDRESS>& data_object, unisim::util::json::JSON_Value *& json_value) const;
	
private:
	const unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if;
	Mode mode;
	
	struct ContextStack
	{
		ContextStack(const DataObject2JSON<ADDRESS>& data_object_2_json, const DataObjectRef<ADDRESS>& data_object);
		
		bool Visit(Type const *type);
		bool Visit(CharType const *type);
		bool Visit(IntegerType const *type);
		bool Visit(FloatingPointType const *type);
		bool Visit(BooleanType const *type);
		bool Visit(CompositeType const *type);
		bool Visit(ArrayType const *type);
		bool Visit(PointerType const *type);
		bool Visit(FunctionType const *type);
		bool Visit(EnumType const *type);
		bool Visit(UnspecifiedType const *type);
		bool Visit(Member const *member);
		bool Visit(FormalParameter const *formal_param);
		
		void PushContext(const std::string& s);
		void PopContext();
		std::string DataObjectName() const;
		unisim::util::json::JSON_Value *GetValue() const;
		void SetValue(unisim::util::json::JSON_Value *json_value);
		
		struct Context
		{
			Context();
			Context(const std::string& name);
			
			std::string name;
			unisim::util::json::JSON_Value *json_value;
		};
		
		const DataObject2JSON<ADDRESS>& data_object_to_json;
		const DataObjectRef<ADDRESS> data_object;
		typedef std::vector<Context> Stack;
		Stack stack;
		
		Status status;
		unisim::util::json::JSON_Value *json_value;
	};
};

template <typename ADDRESS>
class JSON2DataObject
{
public:
	enum Mode
	{
		STRICT,
		LAZY
	};
	
	enum StatusEnum
	{
		OK                       = 0,
		DOES_NOT_EXIST           = 1,
		OPTIMIZED_OUT            = 2,
		TYPE_ERROR               = 4,
		READ_ERROR               = 8,
		WRITE_ERROR              = 16,
		NOT_FOUND                = 32,
		UNSUPPORTED_FLOAT_FORMAT = 64
	};
	
	typedef unsigned int Status;
	
	JSON2DataObject(const unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if, Mode mode = STRICT);
	
	Status Do(const unisim::util::json::JSON_Value *json_value, DataObjectRef<ADDRESS>& data_object) const;
private:
	const unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if;
	Mode mode;
	
	struct ContextStack
	{
		ContextStack(const JSON2DataObject<ADDRESS>& json_to_data_object, DataObjectRef<ADDRESS>& data_object);
		
		bool Visit(const unisim::util::json::JSON_Value& value);
		bool Visit(const unisim::util::json::JSON_String& value);
		bool Visit(const unisim::util::json::JSON_Object& object);
		bool Visit(const unisim::util::json::JSON_Member& member);
		bool Visit(const unisim::util::json::JSON_Array& array);
		
		void PushMemberContext(const std::string& name);
		void PushArrayContext();
		void PopContext();
		bool InArray() const;
		void NextIndex();
		std::string DataObjectName() const;
		
		struct Context
		{
			enum Type
			{
				CTX_MEMBER,
				CTX_ARRAY
			};
			
			Context();
			Context(Type type, const std::string& name, unsigned int index);
			
			Type type;
			std::string name;
			unsigned int index;
		};
		
		const JSON2DataObject<ADDRESS>& json_to_data_object;
		typedef std::vector<Context> Stack;
		Stack stack;
		Status status;
	};
};

template <class ADDRESS>
DataObjectRef<ADDRESS> DataObjectRef<ADDRESS>::Undefined;

template <class ADDRESS>
inline DataObject<ADDRESS>::Error::Error(const char *msg)
	: std::runtime_error(msg)
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::Error::Error(const std::string& msg)
	: std::runtime_error(msg)
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::ReadError::ReadError(const char *data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" read error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::ReadError::ReadError(const std::string& data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" read error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::TypeError::TypeError(const char *data_object_name, const char *reason)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" type error" + (reason ? ": " : "") + (reason ? reason : ""))
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::TypeError::TypeError(const std::string& data_object_name, const std::string& reason)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" type error" + ((!reason.empty()) ? ": " : "") + reason)
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::WriteError::WriteError(const char *data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" write error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::WriteError::WriteError(const std::string& data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" write error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::UndefinedReferenceError::UndefinedReferenceError()
	: DataObject<ADDRESS>::Error("undefined reference error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::UndefinedReferenceError::UndefinedReferenceError(const char *data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" undefined reference error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::UndefinedReferenceError::UndefinedReferenceError(const std::string& data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" undefined reference error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::DoesNotExistError::DoesNotExistError(const char *data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" does not exist in this context")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::DoesNotExistError::DoesNotExistError(const std::string& data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" does not exist in this context")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::OptimizedOutError::OptimizedOutError(const char *data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" optimized out error")
{
}

template <class ADDRESS>
inline DataObject<ADDRESS>::OptimizedOutError::OptimizedOutError(const std::string& data_object_name)
	: DataObject<ADDRESS>::Error(std::string("\"") + data_object_name + "\" optimized out error")
{
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__
