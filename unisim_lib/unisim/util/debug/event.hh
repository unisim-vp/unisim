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
 
#ifndef __UNISIM_UTIL_DEBUG_EVENT_HH__
#define __UNISIM_UTIL_DEBUG_EVENT_HH__

#include <unisim/service/interfaces/debug_event.hh>
#include <inttypes.h>
#include <ostream>
#include <set>

namespace unisim {
namespace service {
namespace interfaces {

template <class ADDRESS> class DebugEventListener;

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace util {
namespace debug {

template <typename ADDRESS>
class Event
{
public:
	typedef unsigned int Type;
	
	virtual ~Event() {}
	Type GetType() const { return type; }
	unsigned int GetProcessorNumber() const { return prc_num; }
	void AddEventListener(unisim::service::interfaces::DebugEventListener<ADDRESS> *listener) { event_listeners.insert(listener); }
	void RemoveEventListener(unisim::service::interfaces::DebugEventListener<ADDRESS> *listener) { event_listeners.erase(listener); }
	bool HasEventListeners() const { return !event_listeners.empty(); }
	inline void Trigger() const;
	void Catch() const { ref_count++; }
	void Release() const { if(ref_count && (--ref_count == 0)) delete this; }
	
protected:
	Event(Type _type, unsigned int _prc_num) : type(_type), prc_num(_prc_num), event_listeners(), ref_count(0) {}
private:
	Type type;
	int prc_num;
	typedef std::set<unisim::service::interfaces::DebugEventListener<ADDRESS> *> EventListeners;
	EventListeners event_listeners;
	mutable unsigned int ref_count;
	
protected:
	static Type AllocateCustomEventType() { static Type next_event_type = 0; return next_event_type++; }
};

template <typename ADDRESS>
inline void Event<ADDRESS>::Trigger() const
{
	typename EventListeners::size_type i = 0, n = event_listeners.size();
	unisim::service::interfaces::DebugEventListener<ADDRESS> *_event_listeners[n];
	for(typename EventListeners::const_iterator it = event_listeners.begin(); it != event_listeners.end(); ++i, ++it)
	{
		_event_listeners[i] = *it;
	}
	for(i = 0; i < n; ++i)
	{
		unisim::service::interfaces::DebugEventListener<ADDRESS> *event_listener = _event_listeners[i];
		event_listener->OnDebugEvent(this);
	}
}

template <typename ADDRESS, typename T>
class CustomEvent : public Event<ADDRESS>
{
public:
	static const typename Event<ADDRESS>::Type TYPE;
	
	static bool IsInstanceOf(const Event<ADDRESS> *event) { return event->GetType() == TYPE; }

protected:
	CustomEvent(unsigned int _prc_num) : Event<ADDRESS>(TYPE, _prc_num) {}
};

template <typename ADDRESS, typename T>
const typename Event<ADDRESS>::Type CustomEvent<ADDRESS, T>::TYPE = Event<ADDRESS>::AllocateCustomEventType();

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
