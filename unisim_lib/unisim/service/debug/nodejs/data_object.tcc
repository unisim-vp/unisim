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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_DATA_OBJECT_TCC__
#define __UNISIM_SERVICE_DEBUG_NODEJS_DATA_OBJECT_TCC__

#include <unisim/service/debug/nodejs/data_object.hh>
#include <unisim/util/debug/data_object.tcc>
#include <unisim/util/debug/data_object_serializer.tcc>

#include <unordered_set>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::util::nodejs::ToString;
using unisim::util::ostream::ToString;


///////////////////////////////////////////////////////////////////////////////
//                                Definitions                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// DataObjectWrapper<> //////////////////////////////

template <typename CONFIG>
const char *DataObjectWrapper<CONFIG>::CLASS_NAME = "DataObject";

template <typename CONFIG>
const uint32_t DataObjectWrapper<CONFIG>::CLASS_ID = unisim::util::nodejs::ObjectWrapper::AllocateClassId();

template <typename CONFIG>
v8::Local<v8::FunctionTemplate> DataObjectWrapper<CONFIG>::CreateFunctionTemplate(NodeJS<CONFIG>& nodejs)
{
	v8::Isolate *isolate = nodejs.GetIsolate();
	v8::EscapableHandleScope handle_scope(isolate);
	
	// Create function template for the constructor function
	v8::Local<v8::FunctionTemplate> data_object_function_template = unisim::util::nodejs::CreateCtorFunctionTemplate<NodeJS<CONFIG>, &This::Ctor>(isolate, nodejs);
	
	// Get the object template
	v8::Local<v8::ObjectTemplate> object_template = data_object_function_template->InstanceTemplate();
	
	// Set accessors
	struct { const char *property_name; v8::AccessorNameGetterCallback accessor_getter_callback; v8::AccessorNameSetterCallback accessor_setter_callback; } accessors_config[] =
	{
		{ "expression"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetExpression                                                                >, 0 },
		{ "name"             , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetName                                                                      >, 0 },
		{ "processor"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetProcessor                                                                 >, 0 },
		{ "bitSize"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetBitSize                                                                   >, 0 },
		{ "type"             , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetType                                                                      >, 0 },
		{ "cvUnqualifiedType", unisim::util::nodejs::AccessorGetterCallback<This, &This::GetCVUnqualifiedType                                                         >, 0 },
		{ "endian"           , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetEndian                                                                    >, 0 },
		{ "address"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetAddress                                                                   >, 0 },
		{ "exists"           , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::Exists        > >, 0 },
		{ "isOptimizedOut"   , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsOptimizedOut> >, 0 },
		{ "isObject"         , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsObject      > >, 0 },
		{ "isStructure"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsStructure   > >, 0 },
		{ "isUnion"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsUnion       > >, 0 },
		{ "isClass"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsClass       > >, 0 },
		{ "isInterface"      , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsInterface   > >, 0 },
		{ "isBasic"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsBasic       > >, 0 },
		{ "isChar"           , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsChar        > >, 0 },
		{ "isInteger"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsInteger     > >, 0 },
		{ "isBoolean"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsBoolean     > >, 0 },
		{ "isFloat"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsFloat       > >, 0 },
		{ "isPointer"        , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsPointer     > >, 0 },
		{ "isFunction"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsFunction    > >, 0 },
		{ "isArray"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsArray       > >, 0 },
		{ "isConst"          , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsConst       > >, 0 },
		{ "isVoid"           , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsVoid        > >, 0 },
		{ "isVolatile"       , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsVolatile    > >, 0 },
		{ "isCharPointer"    , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsCharPointer > >, 0 },
		{ "isSigned"         , unisim::util::nodejs::AccessorGetterCallback<This, &This::GetIsSomething<&unisim::util::debug::DataObjectRef<ADDRESS>::IsSigned      > >, 0 },
	};
	for(auto accessor_config : accessors_config)
	{
		object_template->SetAccessor(
			v8::String::NewFromUtf8(isolate, accessor_config.property_name, v8::NewStringType::kInternalized).ToLocalChecked(),
			accessor_config.accessor_getter_callback,
			accessor_config.accessor_setter_callback
		);
	}
	
	// Get the prototype template
	v8::Local<v8::Template> prototype_template = data_object_function_template->PrototypeTemplate();
	
	// Add methods
	struct { const char *method_name; v8::FunctionCallback callback; } methods_config[] =
	{
		{ "get", &unisim::util::nodejs::FunctionCallback<This, &This::GetCb> },
		{ "set", &unisim::util::nodejs::FunctionCallback<This, &This::SetCb> }
	};
	for(auto method_config : methods_config)
	{
		prototype_template->Set(isolate, method_config.method_name, v8::FunctionTemplate::New(isolate, method_config.callback));
	}
	
	return handle_scope.Escape(data_object_function_template);
}

// DataObject() => DataObject
// DataObject(processor: Processor, expression : string) => DataObject
template <typename CONFIG>
void DataObjectWrapper<CONFIG>::Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	if(!args.IsConstructCall())
	{
		nodejs.Throw(nodejs.TypeError(std::string("Constructor ") + CLASS_NAME + " requires 'new'"));
		return;
	}
	
	ProcessorWrapper<CONFIG> *processor_wrapper = 0;
	std::string expression;
	if(args.Length() != 0)
	{
		struct Synopsis { std::string str() const { return std::string(CLASS_NAME) + "(processor: Processor, expression : string)"; } };
		v8::Local<v8::Value> arg0 = args[0];
		if(!arg0->IsObject() || !(processor_wrapper = ProcessorWrapper<CONFIG>::GetInstance(arg0)))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a Processor for 'processor'"));
			return;
		}
		v8::Local<v8::Value> arg1 = args[1];
		if(arg1->IsNullOrUndefined() || !ToString(args.GetIsolate(), arg1, expression))
		{
			nodejs.Throw(nodejs.TypeError(Synopsis().str() + " expects a string for 'expression'"));
			return;
		}
	}
	
	DataObjectWrapper<CONFIG> *data_object_wrapper = new DataObjectWrapper<CONFIG>(nodejs, processor_wrapper, expression);
	data_object_wrapper->template BindObject<This>(args.This());
	args.GetReturnValue().Set(args.This());
}

template <typename CONFIG>
DataObjectWrapper<CONFIG>::DataObjectWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, const std::string& _expression, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, expression(_expression)
	, data_object()
{
}

template <typename CONFIG>
DataObjectWrapper<CONFIG>::DataObjectWrapper(NodeJS<CONFIG>& _nodejs, ProcessorWrapper<CONFIG> *_processor_wrapper, unisim::util::debug::DataObjectRef<ADDRESS> _data_object, std::size_t size)
	: Super(_nodejs, /* ptr */ 0, size ? size : sizeof(*this))
	, processor_wrapper(_processor_wrapper)
	, expression(_data_object.GetName())
	, data_object(_data_object)
{
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::Update()
{
	if(data_object.IsUndefined())
	{
		data_object = (*this->GetProcessor())[expression];
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::Set(v8::Local<v8::Value> value, const SetOptions& options)
{
	Update();

	try
	{
		if(this->IsVerbose())
		{
			std::string str;
			this->DebugInfoStream() << data_object.GetName() << " <- " << (ToString(this->GetIsolate(), value, str) ? str : "?") << std::endl;
		}
		
		struct Setter
		{
			NodeJS<CONFIG>& nodejs;
			DataObjectWrapper<CONFIG>& data_object_wrapper;
			unisim::util::debug::DataObjectRef<ADDRESS>& data_object;
			
			struct Hash
			{
				size_t operator () (const v8::Local<v8::Value>& value) const
				{
					return (size_t) (const v8::Value *)(*value);
				}
			};
			typedef std::unordered_set<v8::Local<v8::Value>, Hash> Visited;
			Visited visited;
			
			Setter(NodeJS<CONFIG>& _nodejs, DataObjectWrapper<CONFIG>& _data_object_wrapper, unisim::util::debug::DataObjectRef<ADDRESS>& _data_object)
				: nodejs(_nodejs)
				, data_object_wrapper(_data_object_wrapper)
				, data_object(_data_object)
				, visited()
			{
			}

			v8::Isolate *GetIsolate() const { return nodejs.isolate; }
			
			struct Writer
			{
				Setter& setter;
				unisim::util::debug::DataObjectRef<ADDRESS>& data_object;
				
				Writer(Setter& _setter, unisim::util::debug::DataObjectRef<ADDRESS>& _data_object)
					: setter(_setter)
					, data_object(_data_object)
				{
				}
				
				v8::Isolate *GetIsolate() const { return setter.GetIsolate(); }
				
				void Write(v8::Local<v8::Value> value)
				{
					if(setter.visited.count(value)) return;
					
					DataObjectWrapper<CONFIG> *data_object_wrapper = DataObjectWrapper<CONFIG>::GetInstance(value);
					
					if(data_object.IsPointer())
					{
						PointerWrapper<CONFIG> *pointer_wrapper = PointerWrapper<CONFIG>::GetInstance(value);
						if(pointer_wrapper)
						{
							data_object.Write(pointer_wrapper->GetDataObject());
						}
						else if(data_object_wrapper)
						{
							data_object.Write(data_object_wrapper->GetDataObject());
						}
						else
						{
							unisim::util::debug::DataObjectRef<ADDRESS> target_data_object = *data_object;
							Writer writer = Writer(setter, target_data_object);
							writer.Write(value);
						}
						return;
					}
					
					setter.visited.insert(value);
					
					if(data_object.IsArray())
					{
						if(data_object_wrapper)
						{
							data_object.Write(data_object_wrapper->GetDataObject());
						}
						else if(value->IsArray())
						{
							v8::Local<v8::Array> array = value.As<v8::Array>();
							uint32_t array_length = array->Length();
							int64_t lower_bound = data_object.GetLowerBound();
							int64_t upper_bound = data_object.GetUpperBound();
							for(uint32_t i = 0; i < array_length; ++i)
							{
								int64_t subscript = lower_bound + i;
								if((subscript >= lower_bound) && (subscript <= upper_bound))
								{
									unisim::util::debug::DataObjectRef<ADDRESS> item_data_object = data_object[subscript];
									v8::Local<v8::Value> array_item = array->Get(GetIsolate()->GetCurrentContext(), i).ToLocalChecked();
									Writer item_writer = Writer(setter, item_data_object);
									item_writer.Write(array_item);
								}
							}
						}
					}
					else if(data_object.IsObject())
					{
						if(data_object_wrapper)
						{
							data_object.Write(data_object_wrapper->GetDataObject());
						}
						else if(value->IsObject())
						{
							v8::Local<v8::Object> object = value.As<v8::Object>();
							v8::Local<v8::Array> property_names = object->GetPropertyNames(GetIsolate()->GetCurrentContext()).ToLocalChecked();
							for(uint32_t i = 0; i < property_names->Length(); ++i)
							{
								v8::Local<v8::Value> object_property_name = property_names->Get(GetIsolate()->GetCurrentContext(), i).ToLocalChecked();
								std::string property_name;
								if(ToString(GetIsolate(), object_property_name, property_name) && data_object.HasProperty(property_name))
								{
									unisim::util::debug::DataObjectRef<ADDRESS> property_data_object = data_object[property_name];
									v8::Local<v8::Value> object_property_value = object->Get(GetIsolate()->GetCurrentContext(), object_property_name).ToLocalChecked();
									Writer property_writer = Writer(setter, property_data_object);
									property_writer.Write(object_property_value);
								}
							}
						}
					}
					else if(data_object_wrapper)
					{
						data_object.Write(data_object_wrapper->GetDataObject());
					}
					else if(value->IsNumber())
					{
						v8::Local<v8::Number> number = value.As<v8::Number>();
						data_object = number->Value();
					}
					else if(value->IsBoolean())
					{
						v8::Local<v8::Boolean> boolean = value.As<v8::Boolean>();
						data_object = boolean->Value();
					}
					else if(value->IsInt32())
					{
						v8::Local<v8::Int32> int32 = value.As<v8::Int32>();
						data_object = int32->Value();
					}
					else if(value->IsUint32())
					{
						v8::Local<v8::Uint32> uint32 = value.As<v8::Uint32>();
						data_object = uint32->Value();
					}
					else if(value->IsString())
					{
						v8::Local<v8::String> string = value.As<v8::String>();
						std::string str;
						ToString(GetIsolate(), string, str);
						data_object = str;
					}
					else if(value->IsNull())
					{
						data_object = 0;
					}
					else if(value->IsBigInt())
					{
						v8::Local<v8::BigInt> big_int = value.As<v8::BigInt>();
						int word_count = big_int->WordCount();
						int sign_bit;
						uint64_t words[word_count];
						big_int->ToWordsArray(&sign_bit, &word_count, &words[0]);
						if(sign_bit)
						{
							data_object = big_int->Int64Value();
						}
						else
						{
							data_object = big_int->Uint64Value();
						}
					}
				}
			};
			
			void Set(v8::Local<v8::Value> value)
			{
				Writer writer = Writer(*this, data_object);
				writer.Write(value);
			}
		};
		
		Setter setter = Setter(this->nodejs, *this, data_object);
		setter.Set(value);
	}
	catch(typename unisim::util::debug::DataObject<ADDRESS>::Error& data_object_error)
	{
		this->Throw(this->Error(data_object_error.what()));
	}
}

template <typename CONFIG>
v8::Local<v8::Value> DataObjectWrapper<CONFIG>::Get(const GetOptions& options)
{
	Update();
	
	try
	{
		if(this->IsVerbose())
		{
			if(!data_object.IsOptimizedOut())
			{
				unisim::util::debug::DataObjectSerializer<ADDRESS> data_object_serializer = unisim::util::debug::DataObjectSerializer<ADDRESS>(data_object, this->GetProcessor(), unisim::util::debug::DataObjectSerializer<ADDRESS>::C_LANG, unisim::util::debug::DataObjectSerializer<ADDRESS>::LAZY);
				this->DebugInfoStream() << data_object.GetName() << " = " << data_object_serializer << std::endl;
			}
			else
			{
				this->DebugInfoStream() << data_object.GetName() << " = <optimized out>" << std::endl;
			}
		}
		
		struct Getter
		{
			NodeJS<CONFIG>& nodejs;
			DataObjectWrapper<CONFIG>& data_object_wrapper;
			unisim::util::debug::DataObjectRef<ADDRESS>& data_object;
			const GetOptions& options;
			typedef std::map<ADDRESS, v8::Local<v8::Value> > Map;
			Map map;
			
			Getter(NodeJS<CONFIG>& _nodejs, DataObjectWrapper<CONFIG>& _data_object_wrapper, unisim::util::debug::DataObjectRef<ADDRESS>& _data_object, const GetOptions& _options)
				: nodejs(_nodejs)
				, data_object_wrapper(_data_object_wrapper)
				, data_object(_data_object)
				, options(_options)
				, map()
			{
			}

			v8::Isolate *GetIsolate() const { return nodejs.isolate; }
			
			struct Builder
			{
				Getter& getter;
				
				Builder(Getter& _getter)
					: getter(_getter)
				{
				}
				
				v8::Isolate *GetIsolate() const { return getter.GetIsolate(); }
				
				struct ObjectVisitor
				{
					Getter& getter;
					unisim::util::debug::DataObjectRef<ADDRESS>& data_object;
					v8::Local<v8::Object>& object;
					
					ObjectVisitor(Getter& _getter, unisim::util::debug::DataObjectRef<ADDRESS>& _data_object, v8::Local<v8::Object>& _object)
						: getter(_getter)
						, data_object(_data_object)
						, object(_object)
					{
					}
					
					v8::Isolate *GetIsolate() const { return getter.GetIsolate(); }
					
					bool Visit(const char *property_name, unisim::util::debug::DataObjectRef<ADDRESS> property_data_object)
					{
						v8::Local<v8::Value> object_property_value;
						if(getter.options.flatten)
						{
							Builder property_builder = Builder(getter);
							object_property_value = property_builder.Build(property_data_object);
						}
						else
						{
							DataObjectWrapper<CONFIG> *property_data_object_wrapper = new DataObjectWrapper<CONFIG>(getter.nodejs, getter.data_object_wrapper.GetProcessorWrapper(), property_data_object);
							object_property_value = property_data_object_wrapper->MakeObject();
						}
						object->CreateDataProperty(
							GetIsolate()->GetCurrentContext(),
							v8::String::NewFromUtf8(GetIsolate(), property_name).ToLocalChecked(),
							object_property_value
						).ToChecked();
						return false;
					}
				};
				
				struct ArrayVisitor
				{
					Getter& getter;
					unisim::util::debug::DataObjectRef<ADDRESS>& data_object;
					int64_t lower_bound;
					v8::Local<v8::Array>& array;
					
					ArrayVisitor(Getter& _getter, unisim::util::debug::DataObjectRef<ADDRESS>& _data_object, v8::Local<v8::Array>& _array)
						: getter(_getter)
						, data_object(_data_object)
						, lower_bound(data_object.GetLowerBound())
						, array(_array)
					{
					}
					
					v8::Isolate *GetIsolate() const { return getter.GetIsolate(); }
					
					bool Visit(int64_t subscript, unisim::util::debug::DataObjectRef<ADDRESS> item_data_object)
					{
						v8::Local<v8::Value> array_item_value;
						if(getter.options.flatten)
						{
							Builder item_builder = Builder(getter);
							array_item_value = item_builder.Build(item_data_object);
						}
						else
						{
							DataObjectWrapper<CONFIG> *item_data_object_wrapper = new DataObjectWrapper<CONFIG>(getter.nodejs, getter.data_object_wrapper.GetProcessorWrapper(), item_data_object);
							array_item_value = item_data_object_wrapper->MakeObject();
						}
						array->CreateDataProperty(GetIsolate()->GetCurrentContext(), subscript - lower_bound, array_item_value).ToChecked();
						return false;
					}
				};
				
				v8::Local<v8::Value> Build(unisim::util::debug::DataObjectRef<ADDRESS>& data_object)
				{
					v8::EscapableHandleScope handle_scope(GetIsolate());
					
					if(data_object.IsObject())
					{
						v8::Local<v8::Object> object = v8::Object::New(GetIsolate());
						ObjectVisitor visitor(getter, data_object, object);
						data_object.ScanProperties(visitor);
						ADDRESS data_object_addr = 0;
						if(getter.options.flatten && data_object.GetAddress(data_object_addr))
						{
							typename Map::iterator it = getter.map.find(data_object_addr);
							if(it == getter.map.end())
							{
								getter.map[data_object_addr] = object;
							}
						}
						return handle_scope.Escape(object);
					}
					else if(data_object.IsArray())
					{
						v8::Local<v8::Array> array = v8::Array::New(GetIsolate());
						ArrayVisitor visitor(getter, data_object, array);
						data_object.ScanItems(visitor);
						ADDRESS data_object_addr = 0;
						if(getter.options.flatten && data_object.GetAddress(data_object_addr))
						{
							typename Map::iterator it = getter.map.find(data_object_addr);
							if(it == getter.map.end())
							{
								getter.map[data_object_addr] = array;
							}
						}
						return handle_scope.Escape(array);
					}
					else if(data_object.IsPointer())
					{
						if(getter.options.flatten)
						{
							unisim::util::debug::DataObjectRef<ADDRESS> target_data_object = *data_object;
							if(target_data_object.IsObject() || target_data_object.IsArray())
							{
								ADDRESS target_data_object_addr = 0;
								if(target_data_object.GetAddress(target_data_object_addr))
								{
									typename Map::iterator it = getter.map.find(target_data_object_addr);
									if(it != getter.map.end())
									{
										return (*it).second;
									}
								}
							}
							
							Builder builder = Builder(getter);
							v8::Local<v8::Value> value = builder.Build(target_data_object);
							return handle_scope.Escape(value);
						}
						else
						{
							PointerWrapper<CONFIG> *pointer_wrapper = new PointerWrapper<CONFIG>(getter.nodejs, getter.data_object_wrapper.GetProcessorWrapper(), data_object);
							
							v8::Local<v8::Object> value = pointer_wrapper->MakeObject();
							return handle_scope.Escape(value);
						}
					}
					else if(data_object.IsInteger())
					{
						if(data_object.IsSigned())
						{
							int64_t int64_value = data_object;
							if(data_object.GetBitSize() > 32)
							{
								v8::Local<v8::BigInt> value = v8::BigInt::New(GetIsolate(), int64_value);
								return handle_scope.Escape(value);
							}
							else
							{
								v8::Local<v8::Integer> value = v8::Integer::New(GetIsolate(), int32_t(int64_value));
								return handle_scope.Escape(value);
							}
						}
						else
						{
							uint64_t uint64_value = data_object;
							if(data_object.GetBitSize() > 32)
							{
								v8::Local<v8::BigInt> value = v8::BigInt::NewFromUnsigned(GetIsolate(), uint64_value);
								return handle_scope.Escape(value);
							}
							else
							{
								v8::Local<v8::Integer> value = v8::Integer::NewFromUnsigned(GetIsolate(), uint32_t(uint64_value));
								return handle_scope.Escape(value);
							}
						}
					}
					else if(data_object.IsFloat())
					{
						double float_value = data_object;
						v8::Local<v8::Number> value = v8::Number::New(GetIsolate(), float_value);
						return handle_scope.Escape(value);
					}
					else if(data_object.IsBoolean())
					{
						bool boolean_value = data_object;
						v8::Local<v8::Boolean> value = v8::Boolean::New(GetIsolate(), boolean_value);
						return handle_scope.Escape(value);
					}
					else if(data_object.IsCharPointer())
					{
						std::string string_value = data_object;
						v8::Local<v8::String> value;
						if(v8::String::NewFromUtf8(GetIsolate(), string_value.c_str()).ToLocal(&value))
						{
						}
						return handle_scope.Escape(value);
					}
					
					return handle_scope.Escape(v8::Local<v8::Value>());
				}
			};
			
			v8::Local<v8::Value> Get()
			{
				v8::EscapableHandleScope handle_scope(GetIsolate());
				Builder builder = Builder(*this);
				v8::Local<v8::Value> value = builder.Build(data_object);
				return handle_scope.Escape(value);
			}
		};
		
		v8::EscapableHandleScope handle_scope(this->GetIsolate());
		Getter getter = Getter(this->nodejs, *this, data_object, options);
		v8::Local<v8::Value> value = getter.Get();
		return handle_scope.Escape(value);
	}
	catch(typename unisim::util::debug::DataObject<ADDRESS>::Error& data_object_error)
	{
		this->Throw(this->Error(data_object_error.what()));
	}
	
	return v8::Local<v8::Value>();
}

// DataObject.set(value : *, [options : Object])
template <typename CONFIG>
void DataObjectWrapper<CONFIG>::SetCb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	SetOptions options;
	v8::Local<v8::Value> arg0 = args[0];
	if(arg0->IsUndefined())
	{
		this->Throw(this->Error(std::string(CLASS_NAME) + ".set(value : *, [options : Object]) expects a value as 'value'"));
		return;
	}
	v8::Local<v8::Value> arg1 = args[1];
	if(!arg1->IsUndefined())
	{
		if(!arg1->IsObject())
		{
			this->Throw(this->TypeError(std::string(CLASS_NAME) + ".set(value : *, [options : Object]) expects an object as 'options'"));
			return;
		}
		v8::Local<v8::Object> get_options = arg1.As<v8::Object>();
		options.flatten = get_options->Get(
			args.GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(args.GetIsolate(), "flatten")
		).ToLocalChecked()->ToBoolean(args.GetIsolate())->Value();
	}
	
	Set(arg0, options);
}

// DataObjectWrapper.get([options : Object]) => *
template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetCb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	GetOptions options;
	v8::Local<v8::Value> arg0 = args[0];
	if(!arg0->IsUndefined())
	{
		if(!arg0->IsObject())
		{
			this->Throw(this->TypeError(std::string(CLASS_NAME) + ".get([options : Object]) expects an object as 'options'"));
			return;
		}
		v8::Local<v8::Object> get_options = arg0.As<v8::Object>();
		options.flatten = get_options->Get(
			args.GetIsolate()->GetCurrentContext(),
			v8::String::NewFromUtf8Literal(args.GetIsolate(), "flatten")
		).ToLocalChecked()->ToBoolean(args.GetIsolate())->Value();
	}
	
	args.GetReturnValue().Set(Get(options));
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetExpression(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), expression.c_str()).ToLocalChecked());
}

template <typename CONFIG>
template <bool (unisim::util::debug::DataObjectRef<typename CONFIG::ADDRESS>::*METHOD)() const>
void DataObjectWrapper<CONFIG>::GetIsSomething(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	Update();
	
	if(data_object.IsDefined())
	{
		info.GetReturnValue().Set(v8::Boolean::New(this->GetIsolate(), ((&data_object)->*METHOD)()));
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	Update();
	
	if(data_object.IsDefined())
	{
		info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), data_object.GetName()).ToLocalChecked());
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetProcessor(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if(processor_wrapper)
	{
		info.GetReturnValue().Set(processor_wrapper->MakeObject());
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetBitSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	Update();
	
	if(data_object.IsDefined())
	{
		info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), data_object.GetBitSize()));
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	Update();
	
	if(data_object.IsDefined())
	{
		const unisim::util::debug::Type *type = data_object.GetType();
		if(type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type));
		}
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetCVUnqualifiedType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	Update();
	
	if(data_object.IsDefined())
	{
		const unisim::util::debug::Type *type = data_object.GetCVUnqualifiedType();
		if(type)
		{
			info.GetReturnValue().Set(TypeWrapper<CONFIG>::MakeObjectFromType(this->nodejs, type));
		}
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetEndian(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	Update();
	
	if(data_object.IsDefined())
	{
		info.GetReturnValue().Set(v8::String::NewFromUtf8(this->GetIsolate(), ToString(data_object.GetEndian()).c_str()).ToLocalChecked());
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::GetAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	Update();
	
	if(data_object.IsDefined())
	{
		ADDRESS addr = 0;
		if(data_object.GetAddress(addr))
		{
			info.GetReturnValue().Set(MakeInteger(this->GetIsolate(), addr));
		}
	}
}

template <typename CONFIG>
void DataObjectWrapper<CONFIG>::Help(std::ostream& stream)
{
	stream <<
#include <unisim/service/debug/nodejs/doc/data_object.h>
	;
}

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_DATA_OBJECT_TCC__
