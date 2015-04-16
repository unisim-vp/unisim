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
#include <unordered_set>

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
	friend class sc_method_process;
	friend class sc_kernel;
	friend class sc_sensitive;
	friend class sc_event_list;
	
	enum state_t
	{
		NOT_NOTIFIED, DELTA_NOTIFIED, TIMED_NOTIFIED
	};
	
	std::string event_name;
	sc_object *parent_object;
	state_t state;
	sc_kernel_event *kernel_event;              // only used when state == DELTA_NOTIFIED, otherwise undefined
	sc_timed_kernel_event *timed_kernel_event;  // only used when state == TIMED_NOTIFIED, otherwise undefined

	mutable std::unordered_set<sc_thread_process *> dynamically_sensitive_thread_processes;
	mutable std::unordered_set<sc_method_process *> dynamically_sensitive_method_processes;
	mutable std::vector<sc_thread_process *> statically_sensitive_thread_processes;
	mutable std::vector<sc_method_process *> statically_sensitive_method_processes;

	std::string create_hierarchical_name(const char *_name) const;
	void add_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const;
	void add_dynamically_sensitive_method_process(sc_method_process *method_process) const;
	void remove_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const;
	void remove_dynamically_sensitive_method_process(sc_method_process *method_process) const;
	void add_statically_sensitive_thread_process(sc_thread_process *thread_process) const;
	void add_statically_sensitive_method_process(sc_method_process *method_process) const;
	void trigger();
	void clear_dynamically_sensitive_processes();
};

const std::vector<sc_event*>& sc_get_top_level_events();
sc_event* sc_find_event( const char* );

enum sc_event_list_type_t
{
	EVENT_AND_LIST,
	EVENT_OR_LIST
};

class sc_event_list
{
public:
	sc_event_list(sc_event_list_type_t type, bool auto_mm);
	sc_event_list( const sc_event_list& );
	sc_event_list(sc_event_list_type_t type, bool auto_mm, const sc_event& );
	sc_event_list& operator= ( const sc_event_list& );
	~sc_event_list();
	int size() const;
	void swap( sc_event_list& );

	void add_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const;
	void add_dynamically_sensitive_method_process(sc_method_process *method_process) const;
	void remove_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const;
	void remove_dynamically_sensitive_method_process(sc_method_process *method_process) const;
protected:
	friend class sc_event_and_expr;
	friend class sc_event_or_expr;
	
	void insert(const sc_event &);
	void insert(const sc_event_list& );
private:
	friend class sc_thread_process;
	friend class sc_method_process;
	
	sc_event_list_type_t type;
	mutable bool auto_mm;
	std::unordered_set<const sc_event *> events;
	
	void acquire() const;
	void release() const;
	
	mutable unsigned int ref_count;
};

class sc_event_and_list : public sc_event_list
{
public:
	sc_event_and_list();
	sc_event_and_list( const sc_event_and_list& );
	sc_event_and_list( const sc_event& );
	//sc_event_and_list& operator= ( const sc_event_and_list& ); /* provided by sc_event_list */ 
	~sc_event_and_list();
	sc_event_and_list& operator &= (const sc_event& e);
	sc_event_and_list& operator &= (const sc_event_and_list& el);
	sc_event_and_expr operator & (const sc_event& e) const;
	sc_event_and_expr operator & (const sc_event_and_list& el) const;
private:
	friend class sc_event_and_expr;
	
	sc_event_and_list(bool auto_mm);
};

class sc_event_or_list : public sc_event_list
{
public:
	sc_event_or_list();
	sc_event_or_list( const sc_event_or_list& );
	sc_event_or_list( const sc_event& );
	//sc_event_or_list& operator= ( const sc_event_or_list& ); /* provided by sc_event_list */ 
	~sc_event_or_list();
	sc_event_or_list& operator |= (const sc_event& e);
	sc_event_or_list& operator |= (const sc_event_or_list& el);
	sc_event_or_expr operator | (const sc_event& e) const;
	sc_event_or_expr operator | (const sc_event_or_list& el) const;
private:
	friend class sc_event_or_expr;
	
	sc_event_or_list(bool auto_mm);
};

class sc_event_and_expr
{
public:
	sc_event_and_expr();
	~sc_event_and_expr();
	
	operator const sc_event_and_list& () const;
	sc_event_and_expr operator & (const sc_event& e);
	sc_event_and_expr operator & (const sc_event_and_list& el);
	
	sc_event_and_expr& operator &= (const sc_event& e);
	sc_event_and_expr& operator &= (const sc_event_and_list& el);
private:
	// Other members
	sc_event_and_list *event_and_list;
};

sc_event_and_expr operator & (sc_event_and_expr event_and_expr, const sc_event& e);
sc_event_and_expr operator & (sc_event_and_expr event_and_expr, const sc_event_and_list& el);

class sc_event_or_expr
{
public:
	sc_event_or_expr();
	~sc_event_or_expr();

	operator const sc_event_or_list& () const;

	sc_event_or_expr& operator |= (const sc_event& e);
	sc_event_or_expr& operator |= (const sc_event_or_list& el);
private:
	// Other members
	sc_event_or_list *event_or_list;
};

sc_event_or_expr operator | (sc_event_or_expr event_or_expr, const sc_event& e);
sc_event_or_expr operator | (sc_event_or_expr event_or_expr, const sc_event_or_list& el);

} // end of namespace sc_core

#endif
