/*
 *  Copyright (c) 2014,
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

#include <ieee1666/kernel/event.h>
#include <ieee1666/kernel/kernel.h>
#include <ieee1666/kernel/object.h>
#include <ieee1666/kernel/time.h>
#include <ieee1666/kernel/kernel_event.h>

namespace sc_core {

//////////////////////////////////// sc_event /////////////////////////////////////////////

sc_event::sc_event()
	: event_name()
{
	sc_kernel *kernel = sc_kernel::get_kernel();
	parent_object = kernel->get_current_object();
	if(parent_object) parent_object->add_child_event(this);
}

sc_event::sc_event( const char* _name)
	: event_name(_name)
{
}

sc_event::~sc_event()
{
	cancel();
}

const char* sc_event::name() const
{
	return event_name.c_str();
}

const char* sc_event::basename() const
{
	size_t pos = event_name.find_last_of(".", 0);
	return (pos != std::string::npos) ? event_name.c_str() + pos + 1 : event_name.c_str();
}

bool sc_event::in_hierarchy() const
{
}

sc_object* sc_event::get_parent_object() const
{
	return parent_object;
}

void sc_event::notify()
{
	state = DELTA_NOTIFIED;
	kernel_event = sc_kernel::get_kernel()->notify(this);
}

void sc_event::notify(const sc_time& t)
{
	state = TIMED_NOTIFIED;
	timed_kernel_event = sc_kernel::get_kernel()->notify(this, t);
}

void sc_event::notify(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	state = TIMED_NOTIFIED;
	timed_kernel_event = sc_kernel::get_kernel()->notify(this, t);
}

void sc_event::cancel()
{
	switch(state)
	{
		case NOT_NOTIFIED:
			break;
		case DELTA_NOTIFIED:
			kernel_event->cancel();
			break;
		case TIMED_NOTIFIED:
			timed_kernel_event->cancel();
			break;
	}
	
	state = NOT_NOTIFIED;
}

void sc_event::trigger()
{
	if(dynamically_sensitive_thread_processes.size())
	{
		sc_kernel *kernel = sc_kernel::get_kernel();
		
		do
		{
			sc_thread_process *thread_process = dynamically_sensitive_thread_processes.front();
			dynamically_sensitive_thread_processes.pop_front();
			
			kernel->trigger(thread_process);
		}
		while(dynamically_sensitive_thread_processes.size());
	}
}

sc_event_and_expr sc_event::operator& ( const sc_event& ) const
{
}

sc_event_and_expr sc_event::operator& ( const sc_event_and_list& ) const
{
}

sc_event_or_expr sc_event::operator| ( const sc_event& ) const
{
}

sc_event_or_expr sc_event::operator| ( const sc_event_or_list& ) const
{
}

sc_event::sc_event( const sc_event& )
{
}

sc_event& sc_event::operator= ( const sc_event& )
{
}

const std::vector<sc_event*>& sc_get_top_level_events()
{
}

sc_event* sc_find_event( const char* )
{
}

void sc_event::add_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const
{
	dynamically_sensitive_thread_processes.push_back(thread_process);
}

//////////////////////////////// sc_event_and_list /////////////////////////////////////////

sc_event_and_list::sc_event_and_list()
{
}

sc_event_and_list::sc_event_and_list( const sc_event_and_list& )
{
}

sc_event_and_list::sc_event_and_list( const sc_event& )
{
}

sc_event_and_list& sc_event_and_list::operator= ( const sc_event_and_list& )
{
}

sc_event_and_list::~sc_event_and_list()
{
}

int sc_event_and_list::size() const
{
}

void sc_event_and_list::swap( sc_event_and_list& )
{
}

sc_event_and_list& sc_event_and_list::operator&= ( const sc_event& )
{
}

sc_event_and_list& sc_event_and_list::operator&= ( const sc_event_and_list& )
{
}

sc_event_and_expr sc_event_and_list::operator& ( const sc_event& ) const
{
}

sc_event_and_expr sc_event_and_list::operator& ( const sc_event_and_list& ) const
{
}

//////////////////////////////// sc_event_or_list /////////////////////////////////////////

sc_event_or_list::sc_event_or_list()
{
}

sc_event_or_list::sc_event_or_list( const sc_event_or_list& )
{
}

sc_event_or_list::sc_event_or_list( const sc_event& )
{
}

sc_event_or_list& sc_event_or_list::operator= ( const sc_event_or_list& )
{
}

sc_event_or_list::~sc_event_or_list()
{
}

int sc_event_or_list::size() const
{
}

void sc_event_or_list::swap( sc_event_or_list& )
{
}

sc_event_or_list& sc_event_or_list::operator|= ( const sc_event& )
{
}

sc_event_or_list& sc_event_or_list::operator|= ( const sc_event_or_list& )
{
}

sc_event_or_expr sc_event_or_list::operator| ( const sc_event& ) const
{
}

sc_event_or_expr sc_event_or_list::operator| ( const sc_event_or_list& ) const
{
}

//////////////////////////////// sc_event_and_expr /////////////////////////////////////////

sc_event_and_expr::operator const sc_event_and_list &() const
{
}

sc_event_and_expr operator& ( sc_event_and_expr , sc_event const& )
{
}

sc_event_and_expr operator& ( sc_event_and_expr , sc_event_and_list const& )
{
}

//////////////////////////////// sc_event_or_expr /////////////////////////////////////////

sc_event_or_expr::operator const sc_event_or_list &() const
{
}

sc_event_or_expr operator| ( sc_event_or_expr , sc_event const& )
{
}

sc_event_or_expr operator| ( sc_event_or_expr , sc_event_or_list const& )
{
}

} // end of namespace sc_core
