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

#ifndef __UNISIM_UTIL_DEBUG_DATA_OBJECT_TCC__
#define __UNISIM_UTIL_DEBUG_DATA_OBJECT_TCC__

#include <unisim/util/debug/data_object.hh>
#include <unisim/util/json/json.tcc>
#include <unisim/service/interfaces/data_object_lookup.hh>

namespace unisim {
namespace util {
namespace debug {

template <class ADDRESS>
template <typename VISITOR, typename T> T DataObject<ADDRESS>::ScanProperties(VISITOR& visitor) const
{
	if(IsComposite())
	{
		struct CompositeVisitor
		{
			CompositeVisitor(const DataObject<ADDRESS>& _data_object, VISITOR& _visitor)
				: data_object(_data_object)
				, visitor(_visitor)
			{
			}
			
			T Visit(Member const *member)
			{
				return member->HasName() ? visitor.Visit(member->GetName(), data_object[member->GetName()]) : T();
			}
		private:
			const DataObject<ADDRESS>& data_object;
			VISITOR& visitor;
		};
		
		CompositeVisitor composite_visitor(*this, visitor);
		
		return GetCVUnqualifiedType()->AsComposite().template Scan<CompositeVisitor, T>(composite_visitor);
	}
	
	return T();
}

template <class ADDRESS>
template <typename VISITOR, typename T> T DataObject<ADDRESS>::ScanProperties(VISITOR& visitor)
{
	if(IsComposite())
	{
		struct CompositeVisitor
		{
			CompositeVisitor(DataObject<ADDRESS>& _data_object, VISITOR& _visitor)
				: data_object(_data_object)
				, visitor(_visitor)
			{
			}
			
			T Visit(Member const *member)
			{
				return member->HasName() ? visitor.Visit(member->GetName(), data_object[member->GetName()]) : T();
			}
		private:
			DataObject<ADDRESS>& data_object;
			VISITOR& visitor;
		};
		
		CompositeVisitor composite_visitor(*this, visitor);
		
		return GetCVUnqualifiedType()->AsComposite().template Scan<CompositeVisitor, T>(composite_visitor);
	}
	
	return T();
}

template <class ADDRESS>
void DataObjectRef<ADDRESS>::Write(const DataObjectRef<ADDRESS>& src_data_object)
{
	Check();
	src_data_object.Check();
	if(*GetCVUnqualifiedType() != *src_data_object.GetCVUnqualifiedType()) throw typename DataObject<ADDRESS>::TypeError(GetName(), std::string("Can't convert '" + src_data_object.GetCVUnqualifiedType()->GetCDecl(true) + "' to '" + GetCVUnqualifiedType()->GetCDecl(true) + "'"));
	ADDRESS dst_bit_size = GetBitSize();
	ADDRESS src_bit_size = src_data_object.GetBitSize();
	ADDRESS bit_size = (dst_bit_size < src_bit_size) ? dst_bit_size : src_bit_size;
	std::vector<uint8_t> buffer((bit_size + 7) / 8);
	if(!src_data_object.Read(0, &buffer[0], 0, bit_size)) throw typename DataObject<ADDRESS>::ReadError(std::string("Can't read ") + src_data_object.GetName());
	if(!Write(0, &buffer[0], 0, bit_size)) throw typename DataObject<ADDRESS>::ReadError(std::string("Can't write ") + GetName());
}


template <class ADDRESS>
template <typename VISITOR, typename T> T DataObject<ADDRESS>::ScanItems(VISITOR& visitor) const
{
	if(IsArray())
	{
		const ArrayType& array_type = GetCVUnqualifiedType()->AsArray();
		
		int64_t lower_bound = array_type.GetLowerBound();
		int64_t upper_bound = array_type.GetUpperBound();
		for(int64_t subscript = lower_bound; subscript <= upper_bound; ++subscript)
		{
			T ret = visitor.Visit(subscript, (*this)[subscript]);
			if(ret) return ret;
		}
	}
	
	return T();
}

template <class ADDRESS>
template <typename VISITOR, typename T> T DataObject<ADDRESS>::ScanItems(VISITOR& visitor)
{
	if(IsArray())
	{
		const ArrayType& array_type = GetCVUnqualifiedType()->AsArray();
		
		int64_t lower_bound = array_type.GetLowerBound();
		int64_t upper_bound = array_type.GetUpperBound();
		for(int64_t subscript = lower_bound; subscript <= upper_bound; ++subscript)
		{
			T ret = visitor.Visit(subscript, (*this)[subscript]);
			if(ret) return ret;
		}
	}
	
	return T();
}

template <class ADDRESS>
DataObject2JSON<ADDRESS>::DataObject2JSON(const unisim::service::interfaces::DataObjectLookup<ADDRESS> *_data_object_lookup_if, Mode _mode)
	: data_object_lookup_if(_data_object_lookup_if)
	, mode(_mode)
{
}

template <class ADDRESS>
typename DataObject2JSON<ADDRESS>::Status DataObject2JSON<ADDRESS>::Do(const DataObjectRef<ADDRESS>& data_object, unisim::util::json::JSON_Value *& json_value) const
{
	const Type *data_object_type = data_object.GetType();
	
	std::string data_object_name = std::string(data_object.GetName());
	ContextStack ctx_stack(*this, data_object);
	ctx_stack.PushContext(data_object_name);
	data_object_type->Visit(ctx_stack);
	json_value = ctx_stack.json_value;
	return ctx_stack.status;
}

template <class ADDRESS>
DataObject2JSON<ADDRESS>::ContextStack::ContextStack(const DataObject2JSON<ADDRESS>& _data_object_serializer, const DataObjectRef<ADDRESS>& _data_object)
	: data_object_to_json(_data_object_serializer)
	, data_object(_data_object)
	, stack()
	, status(OK)
	, json_value(0)
{
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(Type const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	type->Scan(*this);
	return false;
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(CharType const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = data_object_to_json.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		char char_value = data_object;
		unisim::util::json::JSON_String *json_string;
		if((char_value >= 32) && (char_value < 128))
		{
			json_string = new unisim::util::json::JSON_String(std::string(1, char_value));
		}
		else
		{
			std::ostringstream sstr;
			sstr.width(4);
			sstr.fill('0');
			sstr << "\\u" << std::hex << +(unsigned char) char_value;
			json_string = new unisim::util::json::JSON_String(sstr.str());
		}
		SetValue(json_string);
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::DoesNotExistError&) { status |= DOES_NOT_EXIST; }
	catch(typename DataObject<ADDRESS>::OptimizedOutError&) { status |= OPTIMIZED_OUT; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(IntegerType const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = data_object_to_json.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		unisim::util::json::JSON_Integer *json_integer = type->IsSigned() ? new unisim::util::json::JSON_Integer(int64_t(data_object)) : new unisim::util::json::JSON_Integer(uint64_t(data_object));
		SetValue(json_integer);
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::DoesNotExistError&) { status |= DOES_NOT_EXIST; }
	catch(typename DataObject<ADDRESS>::OptimizedOutError&) { status |= OPTIMIZED_OUT; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(FloatingPointType const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = data_object_to_json.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		uint64_t bit_size = data_object.GetBitSize();
		unisim::util::json::JSON_Float *json_float = (bit_size == 32) ? new unisim::util::json::JSON_Float(float(data_object))
		                                                              : new unisim::util::json::JSON_Float(double(data_object));
		SetValue(json_float);
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::DoesNotExistError&) { status |= DOES_NOT_EXIST; }
	catch(typename DataObject<ADDRESS>::OptimizedOutError&) { status |= OPTIMIZED_OUT; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	catch(typename DataObject<ADDRESS>::TypeError&) { status |= UNSUPPORTED_FLOAT_FORMAT; }
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(BooleanType const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = data_object_to_json.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		unisim::util::json::JSON_Boolean *json_boolean = new unisim::util::json::JSON_Boolean(bool(data_object));
		SetValue(json_boolean);
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::DoesNotExistError&) { status |= DOES_NOT_EXIST; }
	catch(typename DataObject<ADDRESS>::OptimizedOutError&) { status |= OPTIMIZED_OUT; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(CompositeType const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	unisim::util::json::JSON_Object *json_object = new unisim::util::json::JSON_Object();
	SetValue(json_object);
	type->Scan(*this);
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(ArrayType const *array_type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	
	unisim::util::json::JSON_Array *json_array = new unisim::util::json::JSON_Array();
	SetValue(json_array);
	
	int64_t lower_bound = array_type->GetLowerBound();
	int64_t upper_bound = array_type->GetUpperBound();
	for(int64_t subscript = lower_bound; subscript <= upper_bound; ++subscript)
	{
		std::stringstream element_path_sstr;
		element_path_sstr << '[' << subscript << ']';
		PushContext(element_path_sstr.str());
		const Type *type_of_element = array_type->GetTypeOfElement();
		type_of_element->Visit(*this);
		unisim::util::json::JSON_Value *json_element = GetValue();
		if(json_element) json_array->Push(json_element);
		PopContext();
	}
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(PointerType const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = data_object_to_json.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		uint64_t pointer_value = uint64_t(data_object);
		if(pointer_value)
		{
			if(TypeIsCharPointer::Test(type))
			{
				try
				{
					std::string string_value = std::string(data_object);
					unisim::util::json::JSON_String *json_string = new unisim::util::json::JSON_String(string_value);
					SetValue(json_string);
				}
				catch(typename DataObject<ADDRESS>::ReadError&)
				{
					unisim::util::json::JSON_Integer *json_integer = new unisim::util::json::JSON_Integer(pointer_value);
					SetValue(json_integer);
				}
			}
		}
		else
		{
			unisim::util::json::JSON_Integer *json_integer = new unisim::util::json::JSON_Integer(pointer_value);
			SetValue(json_integer);
		}
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::DoesNotExistError&) { status |= DOES_NOT_EXIST; }
	catch(typename DataObject<ADDRESS>::OptimizedOutError&) { status |= OPTIMIZED_OUT; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(FunctionType const *type)
{
	return false;
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(EnumType const *type)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = data_object_to_json.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		unisim::util::json::JSON_Integer *json_integer = new unisim::util::json::JSON_Integer(int64_t(data_object));
		SetValue(json_integer);
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(UnspecifiedType const *type)
{
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(Member const *member)
{
	if((data_object_to_json.mode == LAZY) && (status != OK)) return true;
	if(member->HasName())
	{
		std::string member_name = member->GetName();
		PushContext(std::string(".") + member_name);
		member->Scan(*this);
		unisim::util::json::JSON_Value *json_value = GetValue();
		unisim::util::json::JSON_Member *json_member = json_value ? new unisim::util::json::JSON_Member(member_name, json_value) : 0;
		PopContext();
		unisim::util::json::JSON_Value *json_object = GetValue();
		if(json_object && json_member) json_object->AsObject().Add(json_member);
	}
	else
	{
		member->Scan(*this);
	}
	
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool DataObject2JSON<ADDRESS>::ContextStack::Visit(FormalParameter const *formal_param)
{
	return (data_object_to_json.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
void DataObject2JSON<ADDRESS>::ContextStack::PushContext(const std::string& name)
{
	stack.push_back(Context(name));
}

template <class ADDRESS>
void DataObject2JSON<ADDRESS>::ContextStack::PopContext()
{
	stack.pop_back();
}

template <class ADDRESS>
std::string DataObject2JSON<ADDRESS>::ContextStack::DataObjectName() const
{
	std::string data_object_name;
	unsigned int n = stack.size();
	for(unsigned int i = 0; i < n; ++i)
	{
		data_object_name += stack[i].name;
	}
	return data_object_name;
}

template <class ADDRESS>
unisim::util::json::JSON_Value *DataObject2JSON<ADDRESS>::ContextStack::GetValue() const
{
	return stack.back().json_value;
}

template <class ADDRESS>
void DataObject2JSON<ADDRESS>::ContextStack::SetValue(unisim::util::json::JSON_Value *json_value)
{
	stack.back().json_value = json_value;
}

template <class ADDRESS>
DataObject2JSON<ADDRESS>::ContextStack::Context::Context()
	: name()
	, json_value(0)
{
}

template <class ADDRESS>
DataObject2JSON<ADDRESS>::ContextStack::Context::Context(const std::string& _name)
	: name(_name)
	, json_value(0)
{
}

template <class ADDRESS>
JSON2DataObject<ADDRESS>::JSON2DataObject(const unisim::service::interfaces::DataObjectLookup<ADDRESS> *_data_object_lookup_if, Mode _mode)
	: data_object_lookup_if(_data_object_lookup_if)
	, mode(_mode)
{
}

template <class ADDRESS>
typename JSON2DataObject<ADDRESS>::Status JSON2DataObject<ADDRESS>::Do(const unisim::util::json::JSON_Value *json_value, DataObjectRef<ADDRESS>& data_object) const
{
	ContextStack ctx_stack(*this, data_object);
	json_value->Visit(ctx_stack);
	
	return ctx_stack.status;
}

template <class ADDRESS>
JSON2DataObject<ADDRESS>::ContextStack::ContextStack(const JSON2DataObject<ADDRESS>& _json_to_data_object, DataObjectRef<ADDRESS>& _data_object)
	: json_to_data_object(_json_to_data_object)
	, stack()
	, status(OK)
{
}

template <class ADDRESS>
bool JSON2DataObject<ADDRESS>::ContextStack::Visit(const unisim::util::json::JSON_Value& json_value)
{
	if((json_to_data_object.mode == LAZY) && (status != OK)) return true;
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = json_to_data_object.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		const unisim::util::debug::Type *data_object_type = data_object.GetType();
		unisim::util::json::JSON_ValueType json_value_type = json_value.GetType();
		
		if(unisim::util::debug::TypeIsFloat::Test(data_object_type))
		{
			double value = 0.0;
			switch(json_value_type)
			{
				case unisim::util::json::JSON_INT: value = json_value.AsInteger(); break;
				case unisim::util::json::JSON_FLOAT: value = json_value.AsFloat(); break;
				case unisim::util::json::JSON_BOOLEAN: value = json_value.AsBoolean(); break;
				default: break;
			}
			
			data_object = value;
		}
		else
		{
			uint64_t value = 0;
			switch(json_value_type)
			{
				case unisim::util::json::JSON_INT: value = json_value.AsInteger(); break;
				case unisim::util::json::JSON_FLOAT: value = (int64_t) (double) json_value.AsFloat(); break;
				case unisim::util::json::JSON_BOOLEAN: value = json_value.AsBoolean(); break;
				default: break;
			}
			data_object = value;
		}
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::DoesNotExistError&) { status |= DOES_NOT_EXIST; }
	catch(typename DataObject<ADDRESS>::OptimizedOutError&) { status |= OPTIMIZED_OUT; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	catch(typename DataObject<ADDRESS>::WriteError&) { status |= WRITE_ERROR; }
	
	if(InArray()) NextIndex();
	return (json_to_data_object.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool JSON2DataObject<ADDRESS>::ContextStack::Visit(const unisim::util::json::JSON_String& value)
{
	if((json_to_data_object.mode == LAZY) && (status != OK)) return true;
	std::string data_object_name = DataObjectName();
	DataObjectRef<ADDRESS> data_object = json_to_data_object.data_object_lookup_if->FindDataObject(data_object_name.c_str());
	try
	{
		const unisim::util::debug::Type *data_object_type = data_object.GetType();
		
		if(unisim::util::debug::TypeIsCharPointer::Test(data_object_type))
		{
			data_object = (const std::string&) value.AsString();
		}
		else
		{
			status |= TYPE_ERROR;
		}
	}
	catch(typename DataObject<ADDRESS>::UndefinedReferenceError&) { status |= NOT_FOUND; }
	catch(typename DataObject<ADDRESS>::DoesNotExistError&) { status |= DOES_NOT_EXIST; }
	catch(typename DataObject<ADDRESS>::OptimizedOutError&) { status |= OPTIMIZED_OUT; }
	catch(typename DataObject<ADDRESS>::TypeError&) { status |= TYPE_ERROR; }
	catch(typename DataObject<ADDRESS>::ReadError&) { status |= READ_ERROR; }
	catch(typename DataObject<ADDRESS>::WriteError&) { status |= WRITE_ERROR; }
	
	if(InArray()) NextIndex();
	return (json_to_data_object.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool JSON2DataObject<ADDRESS>::ContextStack::Visit(const unisim::util::json::JSON_Object& object)
{
	if((json_to_data_object.mode == LAZY) && (status != OK)) return true;
	object.Scan(*this);
	if(InArray()) NextIndex();
	return (json_to_data_object.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool JSON2DataObject<ADDRESS>::ContextStack::Visit(const unisim::util::json::JSON_Member& member)
{
	if((json_to_data_object.mode == LAZY) && (status != OK)) return true;
	PushMemberContext(member.GetName());
	member.Scan(*this);
	PopContext();
	return (json_to_data_object.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
bool JSON2DataObject<ADDRESS>::ContextStack::Visit(const unisim::util::json::JSON_Array& array)
{
	if((json_to_data_object.mode == LAZY) && (status != OK)) return true;
	PushArrayContext();
	array.Scan(*this);
	PopContext();
	if(InArray()) NextIndex();
	return (json_to_data_object.mode != LAZY) && (status != OK);
}

template <class ADDRESS>
void JSON2DataObject<ADDRESS>::ContextStack::PushMemberContext(const std::string& name)
{
	stack.push_back(Context(Context::CTX_MEMBER, name, 0));
}

template <class ADDRESS>
void JSON2DataObject<ADDRESS>::ContextStack::PushArrayContext()
{
	stack.push_back(Context(Context::CTX_ARRAY, std::string(), 0));
}

template <class ADDRESS>
void JSON2DataObject<ADDRESS>::ContextStack::PopContext()
{
	stack.pop_back();
}

template <class ADDRESS>
bool JSON2DataObject<ADDRESS>::ContextStack::InArray() const
{
	return (stack.size() != 0) && (stack.back().type == Context::CTX_ARRAY);
}

template <class ADDRESS>
void JSON2DataObject<ADDRESS>::ContextStack::NextIndex()
{
	if(stack.size() != 0)
	{
		++stack.back().index;
	}
}

template <class ADDRESS>
std::string JSON2DataObject<ADDRESS>::ContextStack::DataObjectName() const
{
	std::string data_object_name;
	unsigned int n = stack.size();
	for(unsigned int i = 0; i < n; ++i)
	{
		const Context& ctx = stack[i];
		
		switch(ctx.type)
		{
			case Context::CTX_MEMBER:
			{
				const std::string& name = ctx.name;
				if(name.length() != 0)
				{
					if(data_object_name.length() != 0)
					{
						data_object_name += '.';
					}
					data_object_name += name;
				}
				break;
			}
			
			case Context::CTX_ARRAY:
			{
				std::ostringstream sstr;
				sstr << '[' << ctx.index << ']';
				data_object_name += sstr.str();
				break;
			}
		}
	}
	return data_object_name;
}

template <class ADDRESS>
JSON2DataObject<ADDRESS>::ContextStack::Context::Context()
	: type()
	, name()
	, index(0)
{
}

template <class ADDRESS>
JSON2DataObject<ADDRESS>::ContextStack::Context::Context(Type _type, const std::string& _name, unsigned int _index)
	: type(_type)
	, name(_name)
	, index(_index)
{
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DATA_OBJECT_TCC__