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

#include <inttypes.h>
#include <ostream>

namespace unisim {
namespace util {
namespace debug {

template <typename ADDRESS>
class Event
{
public:
	typedef unsigned int Type;
	
	Event(Type _type) : type(_type), ref(0), event_id(next_event_id++), prc_num(-1), front_end_num(-1), ref_count(0) {}
	Event(Type _type, Event<ADDRESS> *_ref) : type(_type), ref(_ref), event_id(next_event_id++), prc_num(-1), front_end_num(-1), ref_count(0) {}
	virtual ~Event() {}
	Type GetType() const { return type; }
	Event<ADDRESS> *GetReference() const { return ref; }
	unsigned int GetEventId() const { return event_id; }
	int GetProcessorNumber() const { return prc_num; }
	int GetFrontEndNumber() const { return front_end_num; }
	void SetProcessorNumber(int _prc_num) { if((prc_num >= 0) || (_prc_num < 0)) return; prc_num = _prc_num; }
	void SetFrontEndNumber(int _front_end_num) { if((front_end_num >= 0) || (_front_end_num < 0)) return; front_end_num = _front_end_num; }
	void Catch() const { ref_count++; }
	void Release() const { if(ref_count && (--ref_count == 0)) delete this; }
	
private:
	Type type;
	Event<ADDRESS> *ref;
	unsigned int event_id;
	int prc_num;
	int front_end_num;
	mutable unsigned int ref_count;
	
	static unsigned int next_event_id;
	
protected:
	static Type AllocateCustomEventType() { static Type next_event_type = 0; return next_event_type++; }
};

template <typename ADDRESS>
unsigned int Event<ADDRESS>::next_event_id;

template <typename ADDRESS, typename T>
class CustomEvent : public Event<ADDRESS>
{
public:
	static const typename Event<ADDRESS>::Type TYPE;
	
	CustomEvent() : Event<ADDRESS>(TYPE) {}
	CustomEvent(Event<ADDRESS> *ref) : Event<ADDRESS>(TYPE, ref) {}
};

template <typename ADDRESS, typename T>
const typename Event<ADDRESS>::Type CustomEvent<ADDRESS, T>::TYPE = Event<ADDRESS>::AllocateCustomEventType();

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
