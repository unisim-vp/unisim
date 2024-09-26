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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_EXECUTABLE_BINARY_FILE_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_EXECUTABLE_BINARY_FILE_HH__

#include <unisim/service/debug/nodejs/fwd.hh>
#include <unisim/util/nodejs/nodejs.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

//////////////////////////////// ExecutableBinaryFileWrapper<> /////////////////////////////////

template <typename CONFIG>
struct ExecutableBinaryFileWrapper : ObjectWrapper<CONFIG>
{
	typedef ObjectWrapper<CONFIG> Super;
	typedef unisim::util::nodejs::ObjectWrapper Base;
	typedef ExecutableBinaryFileWrapper<CONFIG> This;
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const char *CLASS_NAME;
	static const uint32_t CLASS_ID;
	static bool IsA(uint32_t class_id) { return class_id == CLASS_ID; }
	static v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(NodeJS<CONFIG>& nodejs);
	static void Ctor(NodeJS<CONFIG>& nodejs, const v8::FunctionCallbackInfo<v8::Value>& args);
	static ExecutableBinaryFileWrapper<CONFIG> *Wrap(NodeJS<CONFIG>& nodejs, unisim::service::interfaces::ExecutableBinaryFile *executable_binary_file);
	ExecutableBinaryFileWrapper(NodeJS<CONFIG>& nodejs, unisim::service::interfaces::ExecutableBinaryFile *executable_binary_file = 0, std::size_t size = 0);
	virtual ~ExecutableBinaryFileWrapper();
	unisim::service::interfaces::ExecutableBinaryFile *GetExecutableBinaryFile() const;
	void GetFile(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetFileFormat(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void GetEnable(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	void SetEnable(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static bool IsInstance(v8::Local<v8::Value> value) { return Super::template IsInstanceOf<This>(value); }
	static This *GetInstance(v8::Local<v8::Value> value) { return Super::template GetInstanceOf<This>(value); }
	v8::Local<v8::Object> MakeObject() { return Super::template MakePersistentObject<This>(); }
	static void Help(std::ostream& stream);
private:
	unisim::service::interfaces::ExecutableBinaryFile *executable_binary_file;
};

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_EXECUTABLE_BINARY_FILE_HH__
