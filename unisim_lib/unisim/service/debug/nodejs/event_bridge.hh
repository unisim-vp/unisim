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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_EVENT_BRIDGE_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_EVENT_BRIDGE_HH__

#include <unisim/util/debug/event.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/debug/nodejs/fwd.hh>

namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename CONFIG>
struct EventBridge : unisim::service::interfaces::DebugEventListener<typename CONFIG::ADDRESS>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	EventBridge(NodeJS<CONFIG>& _nodejs, unisim::util::debug::Event<ADDRESS> *_event, v8::Global<v8::Object> const& _recv)
		: nodejs(_nodejs)
		, event(_event)
		, recv(this->GetIsolate(), _recv)
		, listening_event(false)
		, trap(false)
		, functions()
		, resolvers()
	{
		event->AddEventListener(this);
		event->Catch();
	}
	
	virtual ~EventBridge()
	{
		event->RemoveEventListener(this);
		event->Release();
		
		for(typename Functions::iterator it = functions.begin(); it != functions.end(); ++it)
		{
			v8::Global<v8::Function>& function = *it;
			function.Reset();
		}
		for(typename Resolvers::iterator it = resolvers.begin(); it != resolvers.end(); ++it)
		{
			v8::Global<v8::Promise::Resolver>& resolver = *it;
			resolver.Reset();
		}
	}
	
	void Trap(bool flag = true)
	{
		trap = flag;
		if(!Update(true))
		{
			this->Throw(SetRemoveError());
		}
	}
	
	bool AddListener(v8::Local<v8::Function> function)
	{
		functions.resize(functions.size() + 1);
		functions.back().Reset(this->GetIsolate(), function);
		if(!Update(true))
		{
			this->Throw(SetRemoveError());
			return false;
		}
		return true;
	}
	
	v8::Local<v8::Promise> NewPromise()
	{
		v8::EscapableHandleScope handle_scope(this->GetIsolate());
		v8::Local<v8::Context> context = this->GetContext();
		v8::Local<v8::Promise::Resolver> resolver;
		if(!v8::Promise::Resolver::New(context).ToLocal(&resolver))
		{
			this->DebugErrorStream() << "Can't create Javascript promise resolver" << std::endl;
			this->Kill();
			return v8::Undefined(this->GetIsolate()).template As<v8::Promise>();
		}
		resolvers.resize(resolvers.size() + 1);
		resolvers.back().Reset(this->GetIsolate(), resolver);
		if(Update(true))
		{
			return handle_scope.Escape(resolver->GetPromise());
		}
		else
		{
			Reject(SetRemoveError());
			return v8::Undefined(this->GetIsolate()).template As<v8::Promise>();
		}
	}
	
	void RemoveListener(v8::Local<v8::Function> func)
	{
		Functions::iterator it = functions.begin();
		while(it != functions.end())
		{
			v8::Global<v8::Function>& function = *it;
			if(function == func)
			{
				function.Reset();
				it = functions.erase(it);
			}
			else
			{
				++it;
			}
		}
		
		if(!Update(trap || !functions.empty() || !resolvers.empty()))
		{
			this->Throw(SetRemoveError());
		}
	}
	
	v8::Local<v8::Value> SetRemoveError()
	{
		std::ostringstream err_msg;
		err_msg << "Can't " << (listening_event ? "remove" : "set") << " event";
		return this->Error(err_msg.str());
	}
	
	template <v8::Maybe<bool> (v8::Promise::Resolver::*SETTLE_METHOD)(v8::Local<v8::Context>, v8::Local<v8::Value>)>
	void Settle(v8::Local<v8::Value> value)
	{
		for(Resolvers::iterator it = resolvers.begin(); (it != resolvers.end()) && !this->Killed(); ++it)
		{
			v8::Global<v8::Promise::Resolver>& resolver = *it;
			if(!resolver.IsEmpty())
			{
				v8::Local<v8::Promise::Resolver> local_resolver = resolver.Get(this->GetIsolate());
				((*local_resolver)->*SETTLE_METHOD)(this->GetIsolate()->GetCurrentContext(), value).ToChecked();
			}
		}
		resolvers.clear();
	}
	
	void Resolve(v8::Local<v8::Value> value)
	{
		Settle<&v8::Promise::Resolver::Resolve>(value);
	}

	void Reject(v8::Local<v8::Value> value)
	{
		Settle<&v8::Promise::Resolver::Reject>(value);
	}
	
	bool Update(bool has_listeners)
	{
		return has_listeners ? Listen() : Unlisten();
	}
	
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *_event)
	{
		if(event != _event) return;
		
		if(this->nodejs.debug)
		{
			this->nodejs.logger << DebugInfo << "Got an event" << EndDebugInfo;
		}
		
		v8::Locker locker(this->GetIsolate());
		v8::Isolate::Scope isolateScope(this->GetIsolate());
		v8::HandleScope handle_scope(this->GetIsolate());
		v8::Local<v8::Context> context = this->GetContext();
		v8::Context::Scope context_scope(context);
		
		// beware of reentrancy
		if(trap)
		{
			this->nodejs.ResolveContinue(v8::Undefined(this->GetIsolate()));
		}
		
		typedef std::vector<v8::Local<v8::Function> > LocalFunctions;
		LocalFunctions local_functions;
		
		for(Functions::iterator it = functions.begin(); it != functions.end(); ++it)
		{
			v8::Global<v8::Function>& function = *it;
			local_functions.push_back(function.Get(this->GetIsolate()));
		}
		
		for(LocalFunctions::iterator it = local_functions.begin(); it != local_functions.end() && !this->Killed(); ++it)
		{
			v8::Local<v8::Function>& local_function = *it;
			v8::Local<v8::Value> _recv = recv.Get(this->GetIsolate()); // "this"
			v8::Local<v8::Value> result;
			if(!local_function->Call(context, _recv, 0, (v8::Local<v8::Value> *) 0).ToLocal(&result))
			{
				Kill();
			}
		}

		if(!resolvers.empty())
		{
			if(Update(trap || !functions.empty()))
			{
				Resolve(v8::Undefined(this->GetIsolate()));
			}
			else
			{
				Reject(SetRemoveError());
			}
		}
	}
	
	v8::Isolate *GetIsolate() const { return nodejs.GetIsolate(); }
	v8::Local<v8::Context> GetContext() { return nodejs.GetContext(); }
	bool IsDebugging() const { return nodejs.IsDebugging(); }
	bool IsVerbose() const { return nodejs.IsVerbose(); }
	void Kill() { nodejs.Kill(); }
	bool Killed() const { return nodejs.Killed(); }
	std::ostream& DebugInfoStream() const { return nodejs.DebugInfoStream(); }
	std::ostream& DebugWarningStream() const { return nodejs.DebugWarningStream(); }
	std::ostream& DebugErrorStream() const { return nodejs.DebugErrorStream(); }
	v8::Local<v8::Value> RangeError(const std::string& err_msg) const { return nodejs.RangeError(err_msg); }
	v8::Local<v8::Value> TypeError(const std::string& err_msg) const { return nodejs.TypeError(err_msg); }
	v8::Local<v8::Value> Error(const std::string& err_msg) const { return nodejs.Error(err_msg); }
	void Throw(v8::Local<v8::Value> error) { return nodejs.Throw(error); }
	bool Listen()
	{
		if(listening_event) return true;
		return listening_event = nodejs.debug_event_trigger_import->Listen(event);
	}
	bool Unlisten() 
	{
		if(!listening_event) return true;
		return !(listening_event = !nodejs.debug_event_trigger_import->Unlisten(event));
	}
	
	NodeJS<CONFIG>& nodejs;
	unisim::util::debug::Event<ADDRESS> *event;
	v8::Global<v8::Object> recv;
	bool listening_event;
	bool trap;
	typedef std::list<v8::Global<v8::Function> > Functions;
	Functions functions;
	typedef std::vector<v8::Global<v8::Promise::Resolver> > Resolvers;
	Resolvers resolvers;
};

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_EVENT_BRIDGE_HH__
