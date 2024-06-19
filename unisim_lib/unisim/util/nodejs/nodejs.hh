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

#ifndef __UNISIM_UTIL_NODEJS_NODEJS_HH__
#define __UNISIM_UTIL_NODEJS_NODEJS_HH__

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_NODEJS

#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <set>

#include <node.h>

namespace unisim {
namespace util {
namespace nodejs {

///////////////////////////////////////////////////////////////////////////////
//                          Forward declarations                             //
///////////////////////////////////////////////////////////////////////////////

struct NodeJS;
struct ObjectWrapper;

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// Helper functions ////////////////////////////////

bool ToString(v8::Isolate *isolate, v8::Local<v8::Value> value, std::string& str);

template <typename T> bool ToInt(v8::Isolate *isolate, v8::Local<v8::Value> value, T& out);

template <typename T> v8::Local<v8::Value> MakeInteger(v8::Isolate *isolate, T value);

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
void GlobalFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
void FunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

inline v8::Local<v8::ObjectTemplate> CreateObjectTemplate(v8::Isolate *isolate);

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(v8::Isolate *isolate);

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
v8::Local<v8::Function> CreateFunction(v8::Local<v8::Context> context);

template <class CLASS>
v8::Local<v8::Object> CreateObject(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template, CLASS *ptr, uint32_t class_id);

template <class CLASS, void (CLASS::*FINALIZE_METHOD)()>
v8::Local<v8::Object> CreateObject(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template, CLASS *ptr, uint32_t class_id);

/////////////////////////////////// NodeJS ////////////////////////////////////

struct NodeJS
{
	NodeJS(const std::string& executable_path);
	virtual ~NodeJS();
	bool Initialize();

	void SetFilename(const std::string& filename);
	void SetArguments(const std::vector<std::string>& arguments);
	void SetArguments(int argc, const char **arguments);
	void SetOptions(const std::vector<std::string>& options);
	void SetOptions(int optc, const char **options);
	void SetVerbose(bool flag = true);
	void SetDebug(bool flag = true);
	void SetDebugInfoStream(std::ostream& stream);
	void SetDebugWarningStream(std::ostream& stream);
	void SetDebugErrorStream(std::ostream& stream);
	bool IsVerbose() const { return verbose; }
	bool IsDebugging() const { return debug; }
	std::ostream& DebugInfoStream() const;
	std::ostream& DebugWarningStream() const;
	std::ostream& DebugErrorStream() const;
	int GetExitCode() const;
	
	bool Killed() const;
	void Kill();
	bool IsRunning() const;
	
	template <class CLASS> v8::Local<v8::External> ThisToExternal(CLASS *ptr);
	
	virtual void AtExit();
	
protected:
	friend struct ObjectWrapper;
	
	static std::vector<std::string> options;
	static std::unique_ptr<node::MultiIsolatePlatform> platform;
	v8::Isolate *isolate;
	std::unique_ptr<node::Environment, decltype(&node::FreeEnvironment)> *env;
	v8::Global<v8::Context> context;
	v8::Global<v8::Function> require;
	std::string executable_path;
	std::ostream *debug_info_stream;
	std::ostream *debug_warning_stream;
	std::ostream *debug_error_stream;
	bool verbose;
	bool debug;
	bool killed;
	bool exited;
	bool running;
	std::string filename;
	std::vector<std::string> arguments;
	int exit_code;
	
	void Cleanup();
	
	void AddObject(ObjectWrapper *object_wrapper);
	void RemoveObject(ObjectWrapper *object_wrapper);
	
	v8::Isolate *GetIsolate() const { return isolate; }
	v8::Local<v8::Context> GetContext() { return context.Get(isolate); }
	
	template <typename T>
	void CreateIsInstanceOf();
	
	template <typename T>
	v8::Local<v8::Function> GetCtorFunction();
	
	virtual v8::Local<v8::ObjectTemplate> CreateGlobalObjectTemplate();
	virtual void BeforeExecution();
	
	template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
	v8::Local<v8::FunctionTemplate> CreateFunctionTemplate();
	
	template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
	v8::Local<v8::Function> CreateFunction();
	
	void YieldToNodeJS();
	bool LoadSource(const std::string& filename, std::string& source_str);
	v8::Local<v8::Value> Execute(const std::string& source_str, const std::string& filename);
	v8::Local<v8::Value> RangeError(const std::string& err_msg) const;
	v8::Local<v8::Value> TypeError(const std::string& err_msg) const;
	v8::Local<v8::Value> Error(const std::string& err_msg) const;
	void Throw(v8::Local<v8::Value> error);
private:
	static unsigned int platform_ref_count;
	std::vector<std::string> node_args;
	std::vector<std::string> node_exec_args;
	std::thread *thread;
	std::mutex init_mutex;
	std::condition_variable init_cond;
	bool init_cond_val;
	bool init_status;
	std::mutex main_thrd_mutex;
	std::condition_variable main_thrd_cond;
	bool main_thrd_cond_val;
	std::mutex nodejs_thrd_mutex;
	std::condition_variable nodejs_thrd_cond;
	bool nodejs_thrd_cond_val;
	typedef std::set<ObjectWrapper *> ObjectWrappers;
	ObjectWrappers object_wrappers;
	
	template <typename T>
	static bool IsInstanceOf(v8::Local<v8::Value> value);

	template <typename T>
	static void IsInstanceOf(const v8::FunctionCallbackInfo<v8::Value>& args);
	
	template <typename T>
	static T *GetInstanceOf(v8::Local<v8::Value> value);
	
	void Thread();
	void YieldToMain();
	void AbortNodeJS();
	void UnblockNodeJS();
	void ReportException(v8::TryCatch& try_catch);
	bool ExecuteInitScript();
};

//////////////////////////////// ObjectWrapper /////////////////////////////////

struct ObjectWrapper
{
	ObjectWrapper(NodeJS& nodejs, std::size_t size);
	virtual ~ObjectWrapper();
	
	virtual void Finalize();
	
	static uint32_t AllocateClassId() { static uint32_t class_id = 0; return ++class_id; }
protected:
	v8::Isolate *GetIsolate() const { return nodejs.GetIsolate(); }
	v8::Local<v8::Context> GetContext() { return nodejs.GetContext(); }
	bool IsDebugging() const { return nodejs.IsDebugging(); }
	bool IsVerbose() const { return nodejs.IsVerbose(); }
	bool Killed() const { return nodejs.Killed(); }
	std::ostream& DebugInfoStream() const { return nodejs.DebugInfoStream(); }
	std::ostream& DebugWarningStream() const { return nodejs.DebugWarningStream(); }
	std::ostream& DebugErrorStream() const { return nodejs.DebugErrorStream(); }
	v8::Local<v8::Value> RangeError(const std::string& err_msg) const { return nodejs.RangeError(err_msg); }
	v8::Local<v8::Value> TypeError(const std::string& err_msg) const { return nodejs.TypeError(err_msg); }
	v8::Local<v8::Value> Error(const std::string& err_msg) const { return nodejs.Error(err_msg); }
	void Throw(v8::Local<v8::Value> error) { return nodejs.Throw(error); }
	template <typename T> v8::Local<v8::Object> MakeObject(v8::Local<v8::ObjectTemplate> object_template);
	template <typename T> static bool IsInstanceOf(v8::Local<v8::Value> value) { return NodeJS::IsInstanceOf<T>(value); }
	template <typename T> static T *GetInstanceOf(v8::Local<v8::Value> value) { return NodeJS::GetInstanceOf<T>(value); }
private:
	friend struct NodeJS;
	
	NodeJS& nodejs;
	std::size_t size;
	v8::Local<v8::Object> MakeObject(v8::Local<v8::ObjectTemplate> object_template, uint32_t class_id);
};

////////////////////////////////////////////////////////////////////////////////
//                                Definitions                                 //
////////////////////////////////////////////////////////////////////////////////

///////////////////////////// Helper functions ////////////////////////////////

template <typename T>
bool ToInt(v8::Isolate *isolate, v8::Local<v8::Value> value, T& out)
{
	if(value->IsBigInt())
	{
		v8::Local<v8::BigInt> big_int = value.As<v8::BigInt>();
		int word_count = big_int->WordCount();
		int sign_bit;
		uint64_t words[word_count];
		big_int->ToWordsArray(&sign_bit, &word_count, &words[0]);
		if(sign_bit)
		{
			out = big_int->Int64Value();
		}
		else
		{
			out = big_int->Uint64Value();
		}
	}
	else
	{
		v8::Local<v8::Integer> integer;
		if(value->IsUndefined() || !value->ToInteger(isolate->GetCurrentContext()).ToLocal(&integer))
		{
			return false;
		}
		out = integer->Value();
	}
	return true;
}

template <typename T>
v8::Local<v8::Value> MakeInteger(v8::Isolate *isolate, T value)
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::Value> int_value;
	if(std::numeric_limits<T>::is_signed)
	{
		if((std::numeric_limits<T>::max() > std::numeric_limits<int32_t>::max()) ||
		   (std::numeric_limits<T>::min() < std::numeric_limits<int32_t>::min()))
		{
			int_value = v8::BigInt::New(isolate, int64_t(value));
		}
		else
		{
			int_value = v8::Integer::New(isolate, int32_t(value));
		}
	}
	else
	{
		if(std::numeric_limits<T>::max() > std::numeric_limits<uint32_t>::max())
		{
			int_value = v8::BigInt::NewFromUnsigned(isolate, uint64_t(value));
		}
		else
		{
			int_value = v8::Integer::NewFromUnsigned(isolate, uint32_t(value));
		}
	}
	return handle_scope.Escape(int_value);
}

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
void GlobalFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	(static_cast<CLASS *>(args.Data().As<v8::External>()->Value())->*MEMBER_METHOD)(args);
}

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
void FunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	(static_cast<CLASS *>(args.This()->GetInternalField(0).As<v8::External>()->Value())->*MEMBER_METHOD)(args);
}

inline v8::Local<v8::ObjectTemplate> CreateObjectTemplate(v8::Isolate *isolate)
{
	v8::EscapableHandleScope scope(isolate);
	v8::Local<v8::ObjectTemplate> object_template = v8::ObjectTemplate::New(isolate);
	object_template->SetInternalFieldCount(2);
	return scope.Escape(object_template);
}

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(v8::Isolate *isolate)
{
	return v8::FunctionTemplate::New(isolate, &FunctionCallback<CLASS, MEMBER_METHOD>);
}

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
v8::Local<v8::Function> CreateFunction(v8::Local<v8::Context> context)
{
	return v8::Function::New(context, &FunctionCallback<CLASS, MEMBER_METHOD>).ToLocalChecked();
}

template <class CLASS>
v8::Local<v8::Object> CreateObject(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template, CLASS *ptr, uint32_t class_id)
{
	v8::EscapableHandleScope scope(isolate);
	v8::Local<v8::Object> local_object = object_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
	local_object->SetInternalField(0, v8::External::New(isolate, reinterpret_cast<void *>(ptr)));
	local_object->SetInternalField(1, v8::Integer::New(isolate, class_id));
	return scope.Escape(local_object);
}

template <class CLASS, void (CLASS::*FINALIZE_METHOD)()>
v8::Local<v8::Object> CreateObject(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> object_template, CLASS *ptr, uint32_t class_id)
{
	v8::EscapableHandleScope scope(isolate);
	v8::Local<v8::Object> local_object = object_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
	local_object->SetInternalField(0, v8::External::New(isolate, reinterpret_cast<void *>(ptr)));
	local_object->SetInternalField(1, v8::Integer::New(isolate, class_id));
	v8::Global<v8::Object> global_object(isolate, local_object);
	global_object.SetWeak(ptr, [](const v8::WeakCallbackInfo<CLASS>& data)
	{
		CLASS *ptr = static_cast<CLASS *>(data.GetParameter());
		(ptr->*FINALIZE_METHOD)();
	}, v8::WeakCallbackType::kParameter);
	return scope.Escape(local_object);
}

/////////////////////////////////// NodeJS ////////////////////////////////////

template <typename T>
void NodeJS::CreateIsInstanceOf()
{
	v8::HandleScope handle_scope(GetIsolate());
	
	v8::Local<v8::Function> ctor_function = GetCtorFunction<T>();
	// add a property to the constructor function named "is..." (e.g. Foo.isFoo) which is a function convenient to test whether constructor function constructed the argument
	ctor_function->Set(
		this->context.Get(isolate),
		v8::String::NewFromUtf8(isolate, (std::string("is") + (const char *) T::CLASS_NAME).c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
		v8::Function::New(this->context.Get(isolate), &NodeJS::IsInstanceOf<T>).ToLocalChecked()
	).Check();
}

template <typename T>
v8::Local<v8::Function> NodeJS::GetCtorFunction()
{
	v8::EscapableHandleScope handle_scope(GetIsolate());
	v8::Local<v8::Object> global_object = GetContext()->Global();
	// retrieve constructor function
	v8::Local<v8::Function> ctor_function = global_object->Get(
		this->context.Get(isolate),
		v8::String::NewFromUtf8(isolate, (const char *) T::CLASS_NAME).ToLocalChecked()
	).ToLocalChecked().As<v8::Function>();
	return handle_scope.Escape(ctor_function);
}

template <typename T>
bool NodeJS::IsInstanceOf(v8::Local<v8::Value> value)
{
	bool is_instance_of;
	if(!value->IsObject())
	{
		is_instance_of = false;
	}
	else
	{
		v8::Local<v8::Object> object = value.As<v8::Object>();
		is_instance_of = (object->InternalFieldCount() >= 2) && (object->GetInternalField(1).As<v8::Integer>()->Value() == T::CLASS_ID);
	}
	return is_instance_of;
}

template <typename T>
void NodeJS::IsInstanceOf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), IsInstanceOf<T>(args[0])));
}

template <typename T>
T *NodeJS::GetInstanceOf(v8::Local<v8::Value> value)
{
	if(!value->IsObject()) return 0;
	v8::Local<v8::Object> object = value.As<v8::Object>();
	return ((object->InternalFieldCount() >= 2) && (object->GetInternalField(1).As<v8::Integer>()->Value() == T::CLASS_ID)) ? static_cast<T *>(object->GetInternalField(0).As<v8::External>()->Value()) : 0;
}

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
v8::Local<v8::FunctionTemplate> NodeJS::CreateFunctionTemplate()
{
	return v8::FunctionTemplate::New(GetIsolate(), &GlobalFunctionCallback<CLASS, MEMBER_METHOD>, v8::External::New(GetIsolate(), reinterpret_cast<void *>(dynamic_cast<CLASS *>(this))));
}

template <class CLASS, void (CLASS::*MEMBER_METHOD)(const v8::FunctionCallbackInfo<v8::Value>& args)>
v8::Local<v8::Function> NodeJS::CreateFunction()
{
	return v8::Function::New(GetContext(), &GlobalFunctionCallback<CLASS, MEMBER_METHOD>, v8::External::New(GetIsolate(), reinterpret_cast<void *>(dynamic_cast<CLASS *>(this)))).ToLocalChecked();
}

//////////////////////////// ObjectWrapper<> /////////////////////////////

template <typename T>
v8::Local<v8::Object> ObjectWrapper::MakeObject(v8::Local<v8::ObjectTemplate> object_template)
{
	v8::EscapableHandleScope handle_scope(GetIsolate());

	// set prototype of object using the prototype of the constructor function, so that 'new Foo() instanceof Foo == true' works.
	v8::Local<v8::Object> object = MakeObject(object_template, T::CLASS_ID);
	object->SetPrototype(GetContext(), nodejs.template GetCtorFunction<T>()->Get(GetContext(), v8::String::NewFromUtf8Literal(GetIsolate(), "prototype")).ToLocalChecked()).ToChecked();
	
	return handle_scope.Escape(object);
}

} // end of namespace nodejs
} // end of namespace util
} // end of namespace unisim

#endif // HAVE_NODEJS

#endif // __UNISIM_UTIL_NODEJS_NODEJS_HH__
