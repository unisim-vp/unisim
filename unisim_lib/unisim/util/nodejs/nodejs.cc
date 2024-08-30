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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_NODEJS
// See also:
//  - Node.js C++ embedder API: https://nodejs.org/docs/latest-v14.x/api/embedding.html
//  - Getting started with embedding V8: https://v8.dev/docs/embed
//      - https://github.com/v8/v8/blob/master/samples/shell.cc
//      - https://github.com/v8/v8/blob/master/samples/process.cc
//  - C++ function returning an ES6 promise: https://gist.github.com/jupp0r/5f11c0ee2b046b0ab89660ce85ea480e
//  - uv_loop_close() always return BUSY if a timer is initiated: https://github.com/libuv/libuv/discussions/3342
//  - Possible to make an object instanceof a function, without actually constructing it via that function: https://www.mail-archive.com/v8-users@googlegroups.com/msg04787.html

#include <unisim/util/nodejs/nodejs.hh>
#include <cassert>
#include <fstream>
#include <uv.h>

namespace unisim {
namespace util {
namespace nodejs {

///////////////////////////////////////////////////////////////////////////////
//                               Definitions                                 //
///////////////////////////////////////////////////////////////////////////////
	
///////////////////////////// Helper functions /////////////////////////////////

bool ToString(v8::Isolate *isolate, v8::Local<v8::Value> value, std::string& str)
{
	v8::String::Utf8Value utf8_value(isolate, value);
	const char *cstr = *utf8_value;
	if(!cstr) return false;
	str.clear();
	str.append(cstr, utf8_value.length());
	return true;
}

/////////////////////////////////// NodeJS ////////////////////////////////////

unsigned int NodeJS::platform_ref_count = 0;
std::vector<std::string> NodeJS::options;
std::unique_ptr<node::MultiIsolatePlatform> NodeJS::platform;

NodeJS::NodeJS(const std::string& _executable_path)
	: isolate(0)
	, env(0)
	, context()
	, require()
	, executable_path(_executable_path)
	, debug_info_stream(&std::cout)
	, debug_warning_stream(&std::cerr)
	, debug_error_stream(&std::cerr)
	, verbose(false)
	, debug(false)
	, killed(false)
	, exited(false)
	, running(false)
	, filename()
	, arguments()
	, exit_code(0)
	, node_args()
	, node_exec_args()
	, thread(0)
	, init_mutex()
	, init_cond()
	, init_cond_val(false)
	, init_status(false)
	, main_thrd_mutex()
	, main_thrd_cond()
	, main_thrd_cond_val(false)
	, nodejs_thrd_mutex()
	, nodejs_thrd_cond()
	, nodejs_thrd_cond_val(false)
	, env_mutex()
	, object_wrappers()
	, ctor_function_templates()
{
}

NodeJS::~NodeJS()
{
	if(thread)
	{
		Kill();
		
		thread->join();
		delete thread;
		thread = 0;
	}
	
	if(platform_ref_count && (--platform_ref_count == 0))
	{
		v8::V8::Dispose();
		v8::V8::DisposePlatform();
		node::TearDownOncePerProcess();
	}
}

void NodeJS::Map(ObjectWrapper *object_wrapper)
{
	std::pair<ObjectWrappers::iterator, bool> r = object_wrappers.insert(object_wrapper);
	if(r.second)
	{
		if(object_wrapper->size)
		{
			this->isolate->AdjustAmountOfExternalAllocatedMemory(+object_wrapper->size);
		}
		const void *ptr = object_wrapper->ptr;
		if(ptr)
		{
			object_wrapper_map[ptr] = object_wrapper;
		}
		
		object_wrapper->Catch();
	}
	else
	{
		struct Bad {};
		throw Bad();
	}
}

void NodeJS::Unmap(ObjectWrapper *object_wrapper)
{
	if(object_wrappers.count(object_wrapper))
	{
		const void *ptr = object_wrapper->ptr;
		if(ptr)
		{
			object_wrapper_map.erase(ptr);
		}
		if(object_wrapper->size)
		{
			this->isolate->AdjustAmountOfExternalAllocatedMemory(-object_wrapper->size);
		}
		object_wrappers.erase(object_wrapper);
		object_wrapper->Release();
	}
	else
	{
		struct Bad {};
		throw Bad();
	}
}

void NodeJS::Cleanup()
{
	while(!object_wrappers.empty())
	{
		ObjectWrapper *object_wrapper = *object_wrappers.begin();
		object_wrapper->Finalize();
	}
	for(CtorFunctionTemplates::iterator it = ctor_function_templates.begin(); it != ctor_function_templates.end(); ++it)
	{
		(*it).second.Reset();
	}
	ctor_function_templates.clear();
}

bool NodeJS::Initialize()
{
	if(!platform_ref_count)
	{
		std::vector<std::string> init_node_args;
		init_node_args.push_back(executable_path);
		for(auto option : options) init_node_args.push_back(option);
#if NODE_MAJOR_VERSION >= 22
		std::shared_ptr<node::InitializationResult> init_result = node::InitializeOncePerProcess(
#else
		std::unique_ptr<node::InitializationResult> init_result = node::InitializeOncePerProcess(
#endif
			init_node_args,
			{
				node::ProcessInitializationFlags::kNoInitializeV8,
				node::ProcessInitializationFlags::kNoInitializeNodeV8Platform,
				node::ProcessInitializationFlags::kNoDefaultSignalHandling
			}
		);
		for(const std::string& init_error : init_result->errors())
		{
			DebugErrorStream() << init_error << std::endl;
		}
		if(init_result->early_return() != 0)
		{
			exit_code = init_result->exit_code();
			return false;
		}
		platform = node::MultiIsolatePlatform::Create(4);
		v8::V8::InitializePlatform(platform.get());
		v8::V8::Initialize();
	}
	++platform_ref_count;
	
	node_args.clear();
	node_args.push_back(executable_path);
	node_args.push_back(filename);
	for(auto argument : arguments) node_args.push_back(argument);
	node_exec_args.clear();
	
	if(!thread)
	{
		init_status = true;
		std::unique_lock<std::mutex> lock(init_mutex);
		init_cond_val = false;
		thread = new std::thread([this] { Thread(); });
		
		do
		{
			init_cond.wait(lock);
		}
		while(!init_cond_val);
	}
	
	return init_status;
}

void NodeJS::SetFilename(const std::string& _filename)
{
	filename = _filename;
}

void NodeJS::SetArguments(const std::vector<std::string>& _arguments)
{
	arguments.clear();
	for(auto argument : _arguments) arguments.push_back(argument);
}

void NodeJS::SetArguments(int argc, const char **_arguments)
{
	arguments.clear();
	for(int i = 0; i < argc; ++i) arguments.push_back(_arguments[i]);
}

void NodeJS::SetOptions(const std::vector<std::string>& _options)
{
	options.clear();
	for(auto option : _options) options.push_back(option);
}

void NodeJS::SetOptions(int optc, const char **_options)
{
	options.clear();
	for(int i = 0; i < optc; ++i) options.push_back(_options[i]);
}

void NodeJS::SetVerbose(bool flag)
{
	verbose = flag;
}

void NodeJS::SetDebug(bool flag)
{
	debug = flag;
}

void NodeJS::SetDebugInfoStream(std::ostream& stream)
{
	debug_info_stream = &stream;
}

void NodeJS::SetDebugWarningStream(std::ostream& stream)
{
	debug_warning_stream = &stream;
}

void NodeJS::SetDebugErrorStream(std::ostream& stream)
{
	debug_error_stream = &stream;
}

std::ostream& NodeJS::DebugInfoStream() const
{
	return *debug_info_stream;
}

std::ostream& NodeJS::DebugWarningStream() const
{
	return *debug_warning_stream;
}

std::ostream& NodeJS::DebugErrorStream() const
{
	return *debug_error_stream;
}

int NodeJS::GetExitCode() const
{
	return exit_code;
}

bool NodeJS::Killed() const
{
	return killed;
}

void NodeJS::Kill()
{
	{
		std::unique_lock<std::mutex> lock(env_mutex);
		if(env)
		{
			node::Stop(env->get());
			env = 0;
		}
	}
	killed = true;
	UnblockNodeJS();
}

bool NodeJS::IsRunning() const
{
	return running;
}


void NodeJS::AtExit()
{
}

v8::Local<v8::ObjectTemplate> NodeJS::CreateGlobalObjectTemplate()
{
	// Create a template for the global object
	v8::Local<v8::ObjectTemplate> global_object_template = v8::ObjectTemplate::New(isolate);
	return global_object_template;
}

void NodeJS::BeforeExecution()
{
}

void NodeJS::Thread()
{
	init_mutex.lock();
	
	exit_code = 0;
	// Set up a libuv event loop.
	uv_loop_t loop;
	int ret = uv_loop_init(&loop);
	if(ret == 0)
	{
		uv_timer_t uv_timer;
		uv_timer.data = reinterpret_cast<NodeJS *>(this);
		uv_timer_init(&loop, &uv_timer);
		
		std::shared_ptr<node::ArrayBufferAllocator> allocator = node::ArrayBufferAllocator::Create();
		
		isolate = NewIsolate(allocator, &loop, platform.get());
		if(isolate != nullptr)
		{
			{
				v8::Locker locker(isolate);
				v8::Isolate::Scope isolate_scope(isolate);
				
				// Create a node::IsolateData instance that will later be released using
				// node::FreeIsolateData().
				std::unique_ptr<node::IsolateData, decltype(&node::FreeIsolateData)> isolate_data(
					node::CreateIsolateData(isolate, &loop, platform.get(), allocator.get()),
					node::FreeIsolateData);
				
				v8::HandleScope handle_scope(isolate);
				
				// Create a template for the global object.
				v8::Local<v8::ObjectTemplate> global_object_template = CreateGlobalObjectTemplate();
				
				// Set up a new v8::Context.
				v8::Local<v8::Context> context = node::NewContext(isolate, global_object_template);
				if(!context.IsEmpty())
				{
					// Keep V8 context in a global
					this->context.Reset(isolate, context);
					
					// The v8::Context needs to be entered when node::CreateEnvironment() and
					// node::LoadEnvironment() are being called.
					v8::Context::Scope context_scope(context);
					
					// Give another chance to inheritor to install additional things in the global context
					BeforeExecution();
					
					// Create a node::Environment instance that will later be released using
					// node::FreeEnvironment().
					std::unique_ptr<node::Environment, decltype(&node::FreeEnvironment)> env(
						node::CreateEnvironment(isolate_data.get(), context, node_args, node_exec_args),
						node::FreeEnvironment);
					
					SetProcessExitHandler(env.get(), [this](node::Environment *env, int exit_code)
					{
						this->exit_code = exit_code;
						this->killed = true;
						this->exited = true;
						this->isolate->TerminateExecution();
					});
					
					// Set up the Node.js instance for execution, and run code inside of it.
					// There is also a variant that takes a callback and provides it with
					// the `require` and `process` objects, so that it can manually compile
					// and run scripts as needed.
					// The `require` function inside this script does *not* access the file
					// system, and can only load built-in Node.js modules.
					// `module.createRequire()` is being used to create one that is able to
					// load files from the disk, and uses the standard CommonJS file loader
					// instead of the internal-only `require` function.
					v8::MaybeLocal<v8::Value> loadenv_ret = node::LoadEnvironment(
						env.get(),
#if NODE_MAJOR_VERSION >= 16
						"const publicRequire = require('node:module').createRequire(process.cwd() + '/');"
#else
						"const publicRequire = require('module').createRequire(process.cwd() + '/');"
#endif
						"globalThis.require = publicRequire;"
					);
					
					if((exit_code == 0) && !loadenv_ret.IsEmpty())
					{
						// Get "require" from global...
						v8::Local<v8::Value> require;
						if(context->Global()->Get(context, v8::String::NewFromUtf8Literal(isolate, "require")).ToLocal(&require))
						{
							// ...and keep it for later use
							this->require.Reset(isolate, require.As<v8::Function>());
							
							// Load and execute Javascript file if specified
							if(ExecuteInitScript() || exited)
							{
								// initialization succeeded
								{
									if(debug)
									{
										DebugInfoStream() << "Initialization succeeded" << std::endl;
									}
									{
										std::unique_lock<std::mutex> lock(env_mutex);
										this->env = &env;
									}
									init_status = true;
									init_cond_val = true;
									init_cond.notify_one();
									std::unique_lock<std::mutex> lock(nodejs_thrd_mutex);
									nodejs_thrd_cond_val = false;
									init_mutex.unlock();
									do
									{
										nodejs_thrd_cond.wait(lock);
									}
									while(!nodejs_thrd_cond_val);
								}
								
								if(!exited)
								{
									running = true;
									
									// SealHandleScope protects against handle leaks from callbacks.
									v8::SealHandleScope seal(isolate);
									
									// Install a timer (kind of heart beat) to keep loop alive
									uv_timer_start(&uv_timer, [](uv_timer_t *uv_timer)
									{
										NodeJS *nodejs = static_cast<NodeJS *>(uv_timer->data);
										if(nodejs->killed)
										{
											uv_timer_stop(uv_timer);
										}
									}, 0, 1000);
									
									bool more;
									do
									{
										// Let simulation thread (i.e. main thread) call Javascript callbacks or resolve Javascript promises
										YieldToMain();
										if(killed) break;
										
										uv_run(&loop, UV_RUN_NOWAIT);
										
										// V8 tasks on background threads may end up scheduling new tasks in the
										// foreground, which in turn can keep the event loop going. For example,
										// WebAssembly.compile() may do so.
										platform->DrainTasks(isolate);
										
										// If there are new tasks, continue.
										more = uv_loop_alive(&loop);
										
										if(more || !killed) continue;
										
										// node::EmitProcessBeforeExit() is used to emit the 'beforeExit' event
										// on the `process` object.
										if(node::EmitProcessBeforeExit(env.get()).IsNothing()) break;
										
										// 'beforeExit' can also schedule new work that keeps the event loop
										// running.
										more = uv_loop_alive(&loop);
									}
									while(more || !killed);
								}
								
								// node::EmitProcessExit() returns the current exit code.
								exit_code = node::EmitProcessExit(env.get()).FromMaybe(1);
							}
							else
							{
								DebugErrorStream() << "Execution of '" << filename << "' failed" << std::endl;
								init_status = false;
								exit_code = 1;
							}
							
							if(exit_code != 0)
							{
								DebugWarningStream() << "Javascript process has exited with code " << exit_code << std::endl;
							}
							else if(verbose)
							{
								DebugInfoStream() << "Javascript process has exited" << std::endl;
							}
							
							// Let the simulator know that Javascript process has exited to let it cleanup all things that GC did not clean
							AtExit();
							
							// node::Stop() can be used to explicitly stop the event loop and keep
							// further JavaScript from running. It can be called from any thread,
							// and will act like worker.terminate() if called from another thread.
							{
								std::unique_lock<std::mutex> lock(env_mutex);
								node::Stop(env.get());
								this->env = 0;
							}
							
							this->require.Reset();
							
							running = false;
						}
						else
						{
							DebugErrorStream() << "Can't get 'require' function" << std::endl;
							init_status = false;
						}
					}
					else
					{
						// Exit was called or there has been a JS exception
						if(debug)
						{
							DebugInfoStream() << "Exit was called or there has been a JS exception" << std::endl;
						}
						init_status = false;
					}
					
					// Free our own V8 resources
					Cleanup();
					
					this->context.Reset();
				}
				else
				{
					DebugErrorStream() << "Failed to initialize V8 Context" << std::endl;
				}
			}
			
			// Unregister the Isolate with the platform and add a listener that is called
			// when the Platform is done cleaning up any state it had associated with
			// the Isolate.
			bool platform_finished = false;
			platform->AddIsolateFinishedCallback(isolate, [](void *data)
			{
				*static_cast<bool *>(data) = true;
			}, &platform_finished);
			platform->UnregisterIsolate(isolate);
			isolate->Dispose();
			isolate = 0;
			
			// Wait until the platform has cleaned up all relevant resources.
			while(!platform_finished)
			{
				uv_run(&loop, UV_RUN_ONCE);
			}
			int err = uv_loop_close(&loop);
			if(err == UV_EBUSY)
			{
				uv_walk(&loop, [](uv_handle_t *handle, void *arg)
				{
					if(!uv_is_closing(handle))
					{
						uv_close(handle, NULL);
					}
				}, NULL);
				while(uv_run(&loop, UV_RUN_ONCE)) continue;
				err = uv_loop_close(&loop);
			}
			if(err != 0) std::cerr << uv_err_name(err) << std::endl;
			assert(err == 0);
			
			if(init_status)
			{
				AbortNodeJS();
				return;
			}
		}
		else
		{
			DebugErrorStream() << "Failed to initialize V8 Isolate" << std::endl;
		}
	}
	else
	{
		DebugErrorStream() << "Failed to initialize loop: " << uv_err_name(ret) << std::endl;
	}
	
	// initialization failed
	DebugErrorStream() << "Initialization failed" << std::endl;
	init_status = false;
	init_cond_val = true;
	init_cond.notify_one();
	init_mutex.unlock();
}

bool NodeJS::LoadSource(const std::string& filename, std::string& source_str)
{
	std::ifstream file(filename.c_str(), std::ios_base::binary);

	if(!file.is_open())
	{
		DebugErrorStream() << "Can't open for input \"" << filename << "\"" << std::endl;
		return false;
	}

	file.seekg(0, file.end);

	if(file.bad())
	{
		DebugErrorStream() << "I/O error while seeking to end of \"" << filename << "\"" << std::endl;
		return false;
	}
	
	std::streampos file_length = file.tellg();
	
	file.seekg(0, file.beg);
		
	if(file.bad())
	{
		DebugErrorStream() << "I/O error while seeking to begin of \"" << filename << "\"" << std::endl;
		return false;
	}

	std::vector<char> file_content(file_length);
	
	if(verbose)
	{
		DebugInfoStream() << "Loading " << file_length << " bytes from \"" << filename << "\"" << std::endl;
	}
	
	file.read(&file_content[0], file_length);
	
	if(file.bad())
	{
		DebugErrorStream() << "I/O error while reading from \"" << filename << "\"" << std::endl;
		return false;
	}
	
	source_str.clear();
	source_str.append(&file_content[0], file_length);
	
	return true;
}

void NodeJS::ReportException(v8::TryCatch& try_catch)
{
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::Context> context(isolate->GetCurrentContext());
	v8::Local<v8::Message> message = try_catch.Message();
	if(!message.IsEmpty())
	{
		std::string resource_name_str;
		std::cerr << (ToString(isolate, message->GetScriptOrigin().ResourceName(), resource_name_str) ? resource_name_str : "") << ':' << message->GetLineNumber(context).FromJust() << std::endl;
		std::string source_str;
		if(ToString(isolate, message->GetSourceLine(context).ToLocalChecked(), source_str))
		{
			std::cerr << source_str << std::endl;
		}
		int start_col = message->GetStartColumn(context).FromJust();
		for(int col = 0; col < start_col; col++) std::cerr << " ";
		int end_col = message->GetEndColumn(context).FromJust();
		for(int col = start_col; col < end_col; col++) std::cerr << "^";
		std::cerr << std::endl;
	}
	
	v8::Local<v8::Value> stack_trace;
	if(try_catch.StackTrace(context).ToLocal(&stack_trace) &&
	   stack_trace->IsString() &&
	   stack_trace.As<v8::String>()->Length() > 0)
	{
		std::string stack_trace_str;
		if(ToString(isolate, stack_trace, stack_trace_str))
		{
			std::cerr << std::endl << stack_trace_str << std::endl;
		}
	}
	else
	{
		std::string exception_str;
		if(ToString(isolate, try_catch.Exception(), exception_str))
		{
			std::cerr << exception_str << std::endl;
		}
	}
}

v8::Local<v8::Value> NodeJS::Execute(const std::string& source_str, const std::string& filename)
{
	v8::TryCatch try_catch(isolate);
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::Context> context = this->context.Get(isolate);
	v8::Context::Scope context_scope(context);
	
	v8::Local<v8::String> resource_name;
	v8::Local<v8::Value> result;
	
	if(!v8::String::NewFromUtf8(isolate, filename.c_str()).ToLocal(&resource_name))
	{
		ReportException(try_catch);
		return handle_scope.Escape(result);
	}
	
#if NODE_MAJOR_VERSION >= 18
	v8::ScriptOrigin origin(isolate, resource_name);
#else
	v8::ScriptOrigin origin(resource_name);
#endif
	
	v8::Local<v8::String> source;
	if(!v8::String::NewFromUtf8(isolate, source_str.c_str()).ToLocal(&source))
	{
		ReportException(try_catch);
		return handle_scope.Escape(result);
	}
	
  v8::Local<v8::Script> script;
  if(!v8::Script::Compile(context, source, &origin).ToLocal(&script))
	{
		ReportException(try_catch);
		return handle_scope.Escape(result);
	}
	
	if(!script->Run(context).ToLocal(&result))
	{
		ReportException(try_catch);
		return handle_scope.Escape(result);
	}
	
	return handle_scope.Escape(result);
}

bool NodeJS::ExecuteInitScript()
{
	if(filename.empty()) return true;
	
	v8::Locker locker(GetIsolate());
	v8::Isolate::Scope isolate_scope(GetIsolate());
	
	std::string source_str;
	return LoadSource(filename, source_str) && !Execute(source_str, filename).IsEmpty();
}

v8::Local<v8::Value> NodeJS::RangeError(const std::string& err_msg) const
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::String> message = v8::String::NewFromUtf8(isolate, err_msg.c_str()).ToLocalChecked();
	v8::Local<v8::Value> error = v8::Exception::RangeError(message);
	return handle_scope.Escape(error);
}

v8::Local<v8::Value> NodeJS::TypeError(const std::string& err_msg) const
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::String> message = v8::String::NewFromUtf8(isolate, err_msg.c_str()).ToLocalChecked();
	v8::Local<v8::Value> error = v8::Exception::TypeError(message);
	return handle_scope.Escape(error);
}

v8::Local<v8::Value> NodeJS::Error(const std::string& err_msg) const
{
	v8::EscapableHandleScope handle_scope(isolate);
	v8::Local<v8::String> message = v8::String::NewFromUtf8(isolate, err_msg.c_str()).ToLocalChecked();
	v8::Local<v8::Value> error = v8::Exception::Error(message);
	return handle_scope.Escape(error);
}

void NodeJS::Throw(v8::Local<v8::Value> error)
{
	isolate->ThrowException(error);
}

void NodeJS::YieldToNodeJS()
{
	if(debug)
	{
		DebugInfoStream() << "YieldToNodeJS" << std::endl;
	}
	nodejs_thrd_mutex.lock();
	nodejs_thrd_cond_val = true;
	nodejs_thrd_cond.notify_one();
	std::unique_lock<std::mutex> lock(main_thrd_mutex);
	main_thrd_cond_val = false;
	nodejs_thrd_mutex.unlock();
	do
	{
		main_thrd_cond.wait(lock);
	}
	while(!main_thrd_cond_val);
}

void NodeJS::YieldToMain()
{
	if(debug)
	{
		DebugInfoStream() << "YieldToMain" << std::endl;
	}
	
	{
		isolate->Exit();
		v8::Unlocker unlocker(isolate);
		
		main_thrd_mutex.lock();
		main_thrd_cond_val = true;
		main_thrd_cond.notify_one();
		std::unique_lock<std::mutex> lock(nodejs_thrd_mutex);
		nodejs_thrd_cond_val = false;
		main_thrd_mutex.unlock();
		do
		{
			nodejs_thrd_cond.wait(lock);
		}
		while(!nodejs_thrd_cond_val);
	}
	isolate->Enter();
}

void NodeJS::AbortNodeJS()
{
	main_thrd_mutex.lock();
	main_thrd_cond_val = true;
	main_thrd_cond.notify_one();
	main_thrd_mutex.unlock();
}

void NodeJS::UnblockNodeJS()
{
	nodejs_thrd_mutex.lock();
	nodejs_thrd_cond_val = true;
	nodejs_thrd_cond.notify_one();
	nodejs_thrd_mutex.unlock();
}

//////////////////////////////// ObjectWrapper ////////////////////////////////

ObjectWrapper::ObjectWrapper(NodeJS& _nodejs, const void *_ptr, std::size_t _size)
	: nodejs(_nodejs)
	, ptr(_ptr)
	, size(_size)
	, this_object()
	, ref_count(0)
{
	nodejs.Map(this);
}

ObjectWrapper::~ObjectWrapper()
{
	// std::cerr << "ObjectWrapper::~ObjectWrapper(this=" << this << ")" << std::endl;
	// ptr = 0;
	// size = 0;
	// this_object.Reset();
}

void ObjectWrapper::Finalize()
{
	// std::cerr << "ObjectWrapper::Finalize(this=" << this << ")" << std::endl;
	this_object.Reset();
	nodejs.Unmap(this);
}

void ObjectWrapper::BindObject(NodeJS& nodejs, v8::Local<v8::Object> object)
{
	unisim::util::nodejs::template BindObject<ObjectWrapper, &ObjectWrapper::Finalize>(nodejs.GetIsolate(), object, /* ptr */ 0, /* class_id */ 0);
}

} // end of namespace nodejs
} // end of namespace util
} // end of namespace unisim

#endif // HAVE_NODEJS
