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

#ifndef __IEEE1666_KERNEL_EVENT_H__
#define __IEEE1666_KERNEL_EVENT_H__

#include <ieee1666/kernel/fwd.h>
#include <vector>
#include <string>
#include <deque>

namespace sc_core {

class sc_event
{
public:
	sc_event();
	explicit sc_event( const char* );
	~sc_event();
	const char* name() const;
	const char* basename() const;
	bool in_hierarchy() const;
	sc_object* get_parent_object() const;
	void notify();
	void notify( const sc_time& );
	void notify( double , sc_time_unit );
	void cancel();
	sc_event_and_expr operator& ( const sc_event& ) const;
	sc_event_and_expr operator& ( const sc_event_and_list& ) const;
	sc_event_or_expr operator| ( const sc_event& ) const;
	sc_event_or_expr operator| ( const sc_event_or_list& ) const;
private:
	// Disabled
	sc_event( const sc_event& );
	sc_event& operator= ( const sc_event& );
	// Other members
	// Implementation-defined
	friend class sc_thread_process;
	friend class sc_kernel;
	
	enum state_t
	{
		NOT_NOTIFIED, DELTA_NOTIFIED, TIMED_NOTIFIED
	};
	
	std::string event_name;
	sc_object *parent_object;
	state_t state;
	sc_kernel_event *kernel_event;              // only used when state == DELTA_NOTIFIED, otherwise undefined
	sc_timed_kernel_event *timed_kernel_event;  // only used when state == TIMED_NOTIFIED, otherwise undefined

	mutable std::deque<sc_thread_process *> dynamically_sensitive_thread_processes;

	void add_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const;
	void trigger();
};

const std::vector<sc_event*>& sc_get_top_level_events();
sc_event* sc_find_event( const char* );

class sc_event_and_list
{
	public:
	sc_event_and_list();
	sc_event_and_list( const sc_event_and_list& );
	sc_event_and_list( const sc_event& );
	sc_event_and_list& operator= ( const sc_event_and_list& );
	~sc_event_and_list();
	int size() const;
	void swap( sc_event_and_list& );
	sc_event_and_list& operator&= ( const sc_event& );
	sc_event_and_list& operator&= ( const sc_event_and_list& );
	sc_event_and_expr operator& ( const sc_event& ) const;
	sc_event_and_expr operator& ( const sc_event_and_list& ) const;
};

class sc_event_or_list
{
public:
	sc_event_or_list();
	sc_event_or_list( const sc_event_or_list& );
	sc_event_or_list( const sc_event& );
	sc_event_or_list& operator= ( const sc_event_or_list& );
	~sc_event_or_list();
	int size() const;
	void swap( sc_event_or_list& );
	sc_event_or_list& operator|= ( const sc_event& );
	sc_event_or_list& operator|= ( const sc_event_or_list& );
	sc_event_or_expr operator| ( const sc_event& ) const;
	sc_event_or_expr operator| ( const sc_event_or_list& ) const;
};

class sc_event_and_expr
{
public:
	operator const sc_event_and_list &() const;
	// Other members
};
sc_event_and_expr operator& ( sc_event_and_expr , sc_event const& );
sc_event_and_expr operator& ( sc_event_and_expr , sc_event_and_list const& );

class sc_event_or_expr
{
public:
	operator const sc_event_or_list &() const;
	// Other members
};

sc_event_or_expr operator| ( sc_event_or_expr , sc_event const& );
sc_event_or_expr operator| ( sc_event_or_expr , sc_event_or_list const& );

} // end of namespace sc_core

#endif
