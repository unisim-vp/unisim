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
#include <string>
#include <vector>
#include <iosfwd>

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

typedef enum
{
	T_UNKNOWN = 0,
	T_CHAR = 1,
	T_INTEGER = 2,
	T_FLOAT = 3,
	T_BOOL = 4,
	T_STRUCT = 5,
	T_UNION = 6,
	T_CLASS = 7,
	T_INTERFACE = 8,
	T_ARRAY = 9,
	T_POINTER = 10
} TYPE_CLASS;

typedef enum
{
	TINIT_TOK_LITERAL = 0,
	TINIT_TOK_BEGIN_OF_STRUCT = 1,
	TINIT_TOK_END_OF_STRUCT = 2,
	TINIT_TOK_STRUCT_MEMBER_SEPARATOR = 3,
	TINIT_TOK_BEGIN_OF_ARRAY = 4,
	TINIT_TOK_END_OF_ARRAY = 5,
	TINIT_TOK_ARRAY_ELEMENT_SEPARATOR = 6
} TypeInitializerToken;

class Type;
class BaseType;
class IntegerType;
class CharType;
class FloatingPointType;
class BooleanType;
class Member;
class StructureType;
class ArraySubRange;
class ArrayType;
class PointerType;
class TypeVisitor;
template <class ADDRESS> class DataObject;
template <class ADDRESS> class DataObjectInitializer;

std::ostream& operator << (std::ostream& os, const Type& type);
std::ostream& operator << (std::ostream& os, const IntegerType& integer_type);
std::ostream& operator << (std::ostream& os, const CharType& char_type);
std::ostream& operator << (std::ostream& os, const FloatingPointType& floating_point_type);
std::ostream& operator << (std::ostream& os, const BooleanType& boolean_type);
std::ostream& operator << (std::ostream& os, const Member& member);
std::ostream& operator << (std::ostream& os, const StructureType& structure_type);
std::ostream& operator << (std::ostream& os, const ArraySubRange& array_sub_range);
std::ostream& operator << (std::ostream& os, const ArrayType& array_type);
std::ostream& operator << (std::ostream& os, const PointerType& pointer_type);

class Type
{
public:
	Type();
	Type(TYPE_CLASS type_class);
	virtual ~Type();
	
	TYPE_CLASS GetClass() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
private:
	TYPE_CLASS type_class;
	
	friend std::ostream& operator << (std::ostream& os, const Type& type);
};

class BaseType : public Type
{
public:
	BaseType(TYPE_CLASS type_class, unsigned int bit_size);
	virtual ~BaseType();
	unsigned int GetBitSize() const;
private:
	unsigned int bit_size;
};

class IntegerType : public BaseType
{
public:
	IntegerType(unsigned int bit_size, bool is_signed);
	virtual ~IntegerType();
	bool IsSigned() const;
private:
	bool is_signed;
	
	friend std::ostream& operator << (std::ostream& os, const IntegerType& integer_type);
};

class CharType : public BaseType
{
public:
	CharType(unsigned int bit_size, bool is_signed);
	virtual ~CharType();
	bool IsSigned() const;
private:
	bool is_signed;
	
	friend std::ostream& operator << (std::ostream& os, const CharType& char_type);
};

class FloatingPointType : public BaseType
{
public:
	FloatingPointType(unsigned int bit_size);
	virtual ~FloatingPointType();
private:
	friend std::ostream& operator << (std::ostream& os, const FloatingPointType& floating_point_type);
};

class BooleanType : public BaseType
{
public:
	BooleanType(unsigned int bit_size);
	virtual ~BooleanType();
private:
	friend std::ostream& operator << (std::ostream& os, const BooleanType& boolean_type);
};

class Member
{
public:
	Member(const char *name, const Type *type, uint64_t bit_size);
	virtual ~Member();
	const char *GetName() const;
	const Type *GetType() const;
	uint64_t GetBitSize() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
private:
	std::string name;
	const Type *type;
	uint64_t bit_size;
	
	friend std::ostream& operator << (std::ostream& os, const Member& member);
};

class StructureType : public Type
{
public:
	StructureType(TYPE_CLASS type_class);
	virtual ~StructureType();
	
	void Add(const Member *member);
	unsigned int GetMemberCount() const;
	const Member *GetMember(unsigned int idx) const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
private:
	std::vector<const Member *> members;
	
	friend std::ostream& operator << (std::ostream& os, const StructureType& structure_type);
};

class ArrayType : public Type
{
public:
	ArrayType(const Type *type_of_element, int64_t lower_bound, int64_t upper_bound);
	virtual ~ArrayType();
	const Type *GetTypeOfElement() const;
	int64_t GetLowerBound() const;
	int64_t GetUpperBound() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
private:
	const Type *type_of_element;
	int64_t lower_bound;
	int64_t upper_bound;
	
	friend std::ostream& operator << (std::ostream& os, const ArrayType& array_type);
};

class PointerType : public Type
{
public:
	PointerType(const Type *type_of_dereferenced_object);
	virtual ~PointerType();
	const Type *GetTypeOfDereferencedObject() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
private:
	const Type *type_of_dereferenced_object;
	
	friend std::ostream& operator << (std::ostream& os, const PointerType& pointer_type);
};

class TypeVisitor
{
public:
	virtual ~TypeVisitor() {}
	virtual void Visit(const char *path, const Type *type, TypeInitializerToken tok = TINIT_TOK_LITERAL) const = 0;
private:
};

template <class ADDRESS>
class DataObject
{
public:
	virtual ~DataObject() {}
	virtual const char *GetName() const = 0;
	virtual ADDRESS GetBitSize() const = 0;
	virtual const Type *GetType() const = 0;
	virtual unisim::util::endian::endian_type GetEndian() const = 0;
	virtual bool IsOptimizedOut() const = 0;
	virtual bool Fetch() = 0;
	virtual bool Commit() = 0;
	virtual bool Read(ADDRESS obj_bit_offset, uint64_t& value, ADDRESS bit_size) const = 0;
	virtual bool Write(ADDRESS obj_bit_offset, uint64_t value, ADDRESS bit_size) = 0;
	virtual bool Read(ADDRESS obj_bit_offset, void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) const = 0;
	virtual bool Write(ADDRESS obj_bit_offset, const void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) = 0;
};

template <class ADDRESS>
std::ostream& operator << (std::ostream& os, const DataObjectInitializer<ADDRESS>& data_object_initializer);

template <class ADDRESS>
class DataObjectInitializer : public TypeVisitor
{
public:
	DataObjectInitializer(const DataObject<ADDRESS> *data_object, ADDRESS pc, const unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if);
	virtual ~DataObjectInitializer();
	
	virtual void Visit(const char *data_object_name, const Type *type, TypeInitializerToken tok) const;
private:
	const DataObject<ADDRESS> *data_object;
	ADDRESS pc;
	const unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if;
	std::stringstream *os;
	friend std::ostream& operator << <ADDRESS>(std::ostream& os, const DataObjectInitializer<ADDRESS>& data_object_initializer);
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__
