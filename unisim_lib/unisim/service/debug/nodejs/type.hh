/*
 *  Copyright (c) 2023,
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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_TYPE_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_TYPE_HH__

#include <unisim/service/debug/nodejs/fwd.hh>
#include <unisim/util/nodejs/nodejs.hh>
#include <unisim/util/debug/type.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// TypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct TypeWrapper : ObjectWrapper<CONFIG>
{
	typedef ObjectWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef TypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id)
	{
		return (class_id == CLASS_ID) ||
		       NamedTypeWrapper<CONFIG>::IsA(class_id) ||
		       ArrayTypeWrapper<CONFIG>::IsA(class_id) ||
		       PointerTypeWrapper<CONFIG>::IsA(class_id) ||
		       FunctionTypeWrapper<CONFIG>::IsA(class_id) ||
		       ConstTypeWrapper<CONFIG>::IsA(class_id) ||
		       UnspecifiedTypeWrapper<CONFIG>::IsA(class_id) ||
		       VolatileTypeWrapper<CONFIG>::IsA(class_id);
	}
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static TypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Type *type);
	static v8::Local<v8::Object> MakeObjectFromType(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Type *type);
	TypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Type *type, std::size_t size = 0);
	virtual ~TypeWrapper();
	const unisim::util::debug::Type *GetType() const;
	template <bool (unisim::util::debug::Type::*METHOD)() const> void GetIsSomething(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetDeclLoc(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetCDecl(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::Type *type;
};

//////////////////////////////// NamedTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct NamedTypeWrapper : TypeWrapper<CONFIG>
{
	typedef TypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef NamedTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id)
	{
		return (class_id == CLASS_ID) ||
		       BaseTypeWrapper<CONFIG>::IsA(class_id) ||
		       CompositeTypeWrapper<CONFIG>::IsA(class_id) ||
		       TypedefWrapper<CONFIG>::IsA(class_id) ||
		       EnumTypeWrapper<CONFIG>::IsA(class_id);
	}
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	NamedTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::NamedType *named_type, std::size_t size = 0);
	virtual ~NamedTypeWrapper();
	const unisim::util::debug::NamedType *GetNamedType() const;
	void GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetHasName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::NamedType *named_type;
};

//////////////////////////////// BaseTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct BaseTypeWrapper : NamedTypeWrapper<CONFIG>
{
	typedef NamedTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef BaseTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id)
	{
		return (class_id == CLASS_ID) ||
		       IntegerTypeWrapper<CONFIG>::IsA(class_id) ||
		       CharTypeWrapper<CONFIG>::IsA(class_id) ||
		       FloatingPointTypeWrapper<CONFIG>::IsA(class_id) ||
		       BooleanTypeWrapper<CONFIG>::IsA(class_id);
	}
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	BaseTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::BaseType *base_type, std::size_t size = 0);
	virtual ~BaseTypeWrapper();
	const unisim::util::debug::BaseType *GetBaseType() const;
	void GetBitSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::BaseType *base_type;
};

//////////////////////////////// IntegerTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct IntegerTypeWrapper : BaseTypeWrapper<CONFIG>
{
	typedef BaseTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef IntegerTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static IntegerTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::IntegerType *integer_type);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	IntegerTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::IntegerType *integer_type, std::size_t size = 0);
	virtual ~IntegerTypeWrapper();
	const unisim::util::debug::IntegerType *GetIntegerType() const;
	void GetIsSigned(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::IntegerType *integer_type;
};

//////////////////////////////// CharTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct CharTypeWrapper : BaseTypeWrapper<CONFIG>
{
	typedef BaseTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef CharTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static CharTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::CharType *char_type);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	CharTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::CharType *char_type, std::size_t size = 0);
	virtual ~CharTypeWrapper();
	const unisim::util::debug::CharType *GetCharType() const;
	void GetIsSigned(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::CharType *char_type;
};

//////////////////////////////// FloatingPointTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct FloatingPointTypeWrapper : BaseTypeWrapper<CONFIG>
{
	typedef BaseTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef FloatingPointTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static FloatingPointTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FloatingPointType *floating_point_type);
	FloatingPointTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FloatingPointType *floating_point_type, std::size_t size = 0);
	virtual ~FloatingPointTypeWrapper();
	const unisim::util::debug::FloatingPointType *GetFloatingPointType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::FloatingPointType *floating_point_type;
};

//////////////////////////////// BooleanTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct BooleanTypeWrapper : BaseTypeWrapper<CONFIG>
{
	typedef BaseTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef BooleanTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static BooleanTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::BooleanType *boolean_type);
	BooleanTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::BooleanType *boolean_type, std::size_t size = 0);
	virtual ~BooleanTypeWrapper();
	const unisim::util::debug::BooleanType *GetBooleanType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::BooleanType *boolean_type;
};

//////////////////////////////// MemberWrapper<> /////////////////////////////////

template <typename CONFIG>
struct MemberWrapper : ObjectWrapper<CONFIG>
{
	typedef ObjectWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef MemberWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static MemberWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Member *member);
	MemberWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Member *member, std::size_t size = 0);
	virtual ~MemberWrapper();
	const unisim::util::debug::Member *GetMember() const;
	void GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetHasName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetBitSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetCDecl(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::Member *member;
};

//////////////////////////////// CompositeTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct CompositeTypeWrapper : NamedTypeWrapper<CONFIG>
{
	typedef NamedTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef CompositeTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id)
	{
		return (class_id == CLASS_ID) ||
		       StructureTypeWrapper<CONFIG>::IsA(class_id) ||
		       UnionTypeWrapper<CONFIG>::IsA(class_id) ||
		       ClassTypeWrapper<CONFIG>::IsA(class_id) ||
		       InterfaceTypeWrapper<CONFIG>::IsA(class_id);
	}
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	CompositeTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::CompositeType *composite_type, std::size_t size = 0);
	virtual ~CompositeTypeWrapper();
	const unisim::util::debug::CompositeType *GetCompositeType() const;
	void GetIsIncomplete(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject();
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::CompositeType *composite_type;
};

//////////////////////////////// StructureTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct StructureTypeWrapper : CompositeTypeWrapper<CONFIG>
{
	typedef CompositeTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef StructureTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static StructureTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::StructureType *structure_type);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	StructureTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::StructureType *structure_type, std::size_t size = 0);
	virtual ~StructureTypeWrapper();
	const unisim::util::debug::StructureType *GetStructureType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::StructureType *structure_type;
};

//////////////////////////////// UnionTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct UnionTypeWrapper : CompositeTypeWrapper<CONFIG>
{
	typedef CompositeTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef UnionTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static UnionTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::UnionType *union_type);
	UnionTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::UnionType *union_type, std::size_t size = 0);
	virtual ~UnionTypeWrapper();
	const unisim::util::debug::UnionType *GetUnionType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::UnionType *union_type;
};

//////////////////////////////// ClassTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct ClassTypeWrapper : CompositeTypeWrapper<CONFIG>
{
	typedef CompositeTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef ClassTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static ClassTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ClassType *class_type);
	ClassTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ClassType *class_type, std::size_t size = 0);
	virtual ~ClassTypeWrapper();
	const unisim::util::debug::ClassType *GetClassType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::ClassType *class_type;
};

//////////////////////////////// InterfaceTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct InterfaceTypeWrapper : CompositeTypeWrapper<CONFIG>
{
	typedef CompositeTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef InterfaceTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static InterfaceTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::InterfaceType *interface_type);
	InterfaceTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::InterfaceType *interface_type, std::size_t size = 0);
	virtual ~InterfaceTypeWrapper();
	const unisim::util::debug::InterfaceType *GetInterfaceType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::InterfaceType *interface_type;
};

//////////////////////////////// ArrayTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct ArrayTypeWrapper : TypeWrapper<CONFIG>
{
	typedef TypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef ArrayTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static ArrayTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ArrayType *array_type);
	ArrayTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ArrayType *array_type, std::size_t size = 0);
	virtual ~ArrayTypeWrapper();
	const unisim::util::debug::ArrayType *GetArrayType() const;
	void GetTypeOfElement(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetOrder(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetLowerBound(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetUpperBound(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::ArrayType *array_type;
};

//////////////////////////////// PointerTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct PointerTypeWrapper : TypeWrapper<CONFIG>
{
	typedef TypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef PointerTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static PointerTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::PointerType *pointer_type);
	PointerTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::PointerType *pointer_type, std::size_t size = 0);
	virtual ~PointerTypeWrapper();
	const unisim::util::debug::PointerType *GetPointerType() const;
	void GetTypeOfDereferencedObject(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::PointerType *pointer_type;
};

//////////////////////////////// TypedefWrapper<> /////////////////////////////////

template <typename CONFIG>
struct TypedefWrapper : NamedTypeWrapper<CONFIG>
{
	typedef NamedTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef TypedefWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static TypedefWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Typedef *typedef_type);
	TypedefWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Typedef *typedef_type, std::size_t size = 0);
	virtual ~TypedefWrapper();
	const unisim::util::debug::Typedef *GetTypedef() const;
	void GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::Typedef *typedef_type;
};

//////////////////////////////// FormalParameterWrapper<> /////////////////////////////////

template <typename CONFIG>
struct FormalParameterWrapper : ObjectWrapper<CONFIG>
{
	typedef ObjectWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef FormalParameterWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static FormalParameterWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FormalParameter *formal_parameter);
	FormalParameterWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FormalParameter *formal_parameter, std::size_t size = 0);
	virtual ~FormalParameterWrapper();
	const unisim::util::debug::FormalParameter *GetFormalParameter() const;
	void GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::FormalParameter *formal_parameter;
};

//////////////////////////////// FunctionTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct FunctionTypeWrapper : TypeWrapper<CONFIG>
{
	typedef TypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef FunctionTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static FunctionTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FunctionType *function_type);
	FunctionTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::FunctionType *function_type, std::size_t size = 0);
	virtual ~FunctionTypeWrapper();
	const unisim::util::debug::FunctionType *GetFunctionType() const;
	void GetReturnType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject();
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::FunctionType *function_type;
};

//////////////////////////////// ConstTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct ConstTypeWrapper : TypeWrapper<CONFIG>
{
	typedef TypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef ConstTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static ConstTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ConstType *const_type);
	ConstTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::ConstType *const_type, std::size_t size = 0);
	virtual ~ConstTypeWrapper();
	const unisim::util::debug::ConstType *GetConstType() const;
	void GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::ConstType *const_type;
};

//////////////////////////////// EnumeratorWrapper<> /////////////////////////////////

template <typename CONFIG>
struct EnumeratorWrapper : ObjectWrapper<CONFIG>
{
	typedef ObjectWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef EnumeratorWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static EnumeratorWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Enumerator *enumerator);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	EnumeratorWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::Enumerator *enumerator, std::size_t size = 0);
	virtual ~EnumeratorWrapper();
	const unisim::util::debug::Enumerator *GetEnumerator() const;
	void GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetValue(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::Enumerator *enumerator;
};

//////////////////////////////// EnumTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct EnumTypeWrapper : NamedTypeWrapper<CONFIG>
{
	typedef NamedTypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef EnumTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static EnumTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::EnumType *enum_type);
	EnumTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::EnumType *enum_type, std::size_t size = 0);
	virtual ~EnumTypeWrapper();
	const unisim::util::debug::EnumType *GetEnumType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject();
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::EnumType *enum_type;
};

//////////////////////////////// UnspecifiedTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct UnspecifiedTypeWrapper : TypeWrapper<CONFIG>
{
	typedef TypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef UnspecifiedTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static UnspecifiedTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::UnspecifiedType *unspecified_type);
	UnspecifiedTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::UnspecifiedType *unspecified_type, std::size_t size = 0);
	virtual ~UnspecifiedTypeWrapper();
	const unisim::util::debug::UnspecifiedType *GetUnspecifiedType() const;
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::UnspecifiedType *unspecified_type;
};

//////////////////////////////// VolatileTypeWrapper<> /////////////////////////////////

template <typename CONFIG>
struct VolatileTypeWrapper : TypeWrapper<CONFIG>
{
	typedef TypeWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef VolatileTypeWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static VolatileTypeWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, const unisim::util::debug::VolatileType *volatile_type);
	VolatileTypeWrapper(NodeJS<CONFIG>& nodejs, const unisim::util::debug::VolatileType *volatile_type, std::size_t size = 0);
	virtual ~VolatileTypeWrapper();
	const unisim::util::debug::VolatileType *GetVolatileType() const;
	void GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Base::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	const unisim::util::debug::VolatileType *volatile_type;
};

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_TYPE_HH__
