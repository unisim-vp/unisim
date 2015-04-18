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

#ifndef __IEEE1666_KERNEL_THREAD_PROCESS_H__
#define __IEEE1666_KERNEL_THREAD_PROCESS_H__

#include <ieee1666/kernel/fwd.h>
#include <ieee1666/kernel/object.h>
#include <ieee1666/kernel/process.h>
#include <ieee1666/kernel/time.h>
//#include <boost/bind.hpp>
#include <boost/coroutine/coroutine.hpp>

namespace sc_core {

typedef boost::coroutines::coroutine<void()> sc_coroutine;
typedef boost::coroutines::attributes sc_coroutine_attributes;

class sc_thread_process_helper
{
public:
	sc_thread_process_helper(sc_thread_process *thread_process, sc_coroutine::caller_type& yield);
	~sc_thread_process_helper();
private:
	friend class sc_thread_process;
	
	sc_thread_process *thread_process;
	sc_coroutine::caller_type& yield;
};

class sc_thread_process : public sc_process
{
public:
	
	sc_thread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *spawn_options = 0);
	virtual ~sc_thread_process();
	
	void set_stack_size(int stack_size);
	
	virtual void start();
	
	void trigger_dynamically(const sc_event *triggered_event);
	void trigger_statically();
	
	void wait();
	void wait(const sc_event& e);
	void wait(const sc_event_and_list& el);
	void wait(const sc_event_or_list& el);
	void wait(const sc_time& t);
	void wait(const sc_time& t, const sc_event& e);
	void wait(const sc_time& t, const sc_event_and_list& el);
	void wait(const sc_time& t, const sc_event_or_list& el);
	
	virtual bool terminated() const;
	virtual const sc_event& terminated_event() const;
	virtual void suspend(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS);
	virtual void resume(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS);
	virtual void disable(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS);
	virtual void enable(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS);
	virtual void kill(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS);
	virtual void reset(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS);
	
private:
	friend class sc_thread_process_helper;
	friend class sc_kernel;
	
	sc_coroutine *coro;
	sc_thread_process_helper *thread_process_helper;
	int stack_size;
	bool thread_process_terminated;
	sc_event thread_process_terminated_event;
	
	enum wait_type_t
	{
		WAIT_DEFAULT,
		WAIT_EVENT,
		WAIT_EVENT_AND_LIST,
		WAIT_EVENT_OR_LIST,
		WAIT_TIME_OUT,
		WAIT_TIME_OUT_OR_EVENT,
		WAIT_TIME_OUT_OR_EVENT_AND_LIST,
		WAIT_TIME_OUT_OR_EVENT_OR_LIST
	};
	
	wait_type_t wait_type;
	const sc_event *wait_event;
	const sc_event_list *wait_event_list;
	unsigned int wait_and_event_list_remaining_count;
	sc_time wait_time_out;
	sc_event wait_time_out_event;

	void coroutine_work(sc_coroutine::caller_type& yield);
	void yield();
	void switch_to();
};

} // end of namespace sc_core

#endif
