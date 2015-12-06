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

#include <ieee1666/kernel/module.h>
#include <ieee1666/kernel/kernel.h>
#include <ieee1666/kernel/thread_process.h>
#include <ieee1666/kernel/object.h>
#include <ieee1666/kernel/event.h>
#include <ieee1666/kernel/time.h>
#include <ieee1666/kernel/module_name.h>
#include <ieee1666/base/in.h>
#include <ieee1666/base/out.h>
#include <ieee1666/base/inout.h>
#include <ieee1666/base/signal_if.h>

namespace sc_core {

/////////////////////////////////// sc_module /////////////////////////////////////////////

sc_module::~sc_module()
{
	kernel->unregister_module(this);
}

const char* sc_module::kind() const
{
	return "sc_module";
}

//void operator() ( const sc_bind_proxy& p001,
//const sc_bind_proxy& p002 = SC_BIND_PROXY_NIL,
//const sc_bind_proxy& p003 = SC_BIND_PROXY_NIL,
//...
//const sc_bind_proxy& p063 = SC_BIND_PROXY_NIL,
//const sc_bind_proxy& p064 = SC_BIND_PROXY_NIL )
//{
//}

sc_module::sc_module( const sc_module_name& module_name )
	: sc_object((const char *)(*sc_kernel::get_kernel()->get_top_of_module_name_stack()))
	, sensitive()
	, spawn_options()
{
	init();
}

sc_module::sc_module()
	: sc_object((const char *)(*sc_kernel::get_kernel()->get_top_of_module_name_stack()))
	, sensitive()
	, spawn_options()
{
	init();
}

void sc_module::reset_signal_is( const sc_in<bool>& , bool )
{
}

void sc_module::reset_signal_is( const sc_inout<bool>& , bool )
{
}

void sc_module::reset_signal_is( const sc_out<bool>& , bool )
{
}

void sc_module::reset_signal_is( const sc_signal_in_if<bool>& , bool )
{
}

void sc_module::async_reset_signal_is( const sc_in<bool>& , bool )
{
}

void sc_module::async_reset_signal_is( const sc_inout<bool>& , bool )
{
}

void sc_module::async_reset_signal_is( const sc_out<bool>& , bool )
{
}

void sc_module::async_reset_signal_is( const sc_signal_in_if<bool>& , bool )
{
}

void sc_module::dont_initialize()
{
	spawn_options.dont_initialize();
}

void sc_module::set_stack_size(size_t stack_size)
{
	spawn_options.set_stack_size(stack_size);
}

void sc_module::next_trigger()
{
	kernel->next_trigger();
}

void sc_module::next_trigger( const sc_event& e)
{
	kernel->next_trigger(e);
}

void sc_module::next_trigger(const sc_event_or_list& el)
{
	kernel->next_trigger(el);
}

void sc_module::next_trigger(const sc_event_and_list& el)
{
	kernel->next_trigger(el);
}

void sc_module::next_trigger(const sc_time& t)
{
	kernel->next_trigger(t);
}

void sc_module::next_trigger(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t);
}

void sc_module::next_trigger(const sc_time& t, const sc_event& e)
{
	kernel->next_trigger(t, e);
}

void sc_module::next_trigger(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t, e);
}

void sc_module::next_trigger(const sc_time& t, const sc_event_or_list& el)
{
	kernel->next_trigger(t, el);
}

void sc_module::next_trigger(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t, el);
}

void sc_module::next_trigger(const sc_time& t, const sc_event_and_list& el)
{
	kernel->next_trigger(t, el);
}

void sc_module::next_trigger(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t, el);
}

void sc_module::wait()
{
	kernel->wait();
}

void sc_module::wait(int n)
{
	kernel->wait(n);
}

void sc_module::wait(const sc_event& e)
{
	kernel->wait(e);
}

void sc_module::wait(const sc_event_or_list& el)
{
	kernel->wait(el);
}

void sc_module::wait(const sc_event_and_list& el)
{
	kernel->wait(el);
}

void sc_module::wait(const sc_time& t)
{
	kernel->wait(t);
}

void sc_module::wait(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t);
}

void sc_module::wait(const sc_time& t, const sc_event& e)
{
	kernel->wait(t, e);
}

void sc_module::wait(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t, e);
}

void sc_module::wait(const sc_time& t, const sc_event_or_list& el)
{
	kernel->wait(t, el);
}

void sc_module::wait(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t, el);
}

void sc_module::wait(const sc_time& t, const sc_event_and_list& el)
{
	kernel->wait(t, el);
}

void sc_module::wait(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t, el);
}

void sc_module::before_end_of_elaboration()
{
}

void sc_module::end_of_elaboration()
{
}

void sc_module::start_of_simulation()
{
}

void sc_module::end_of_simulation()
{
}

sc_module::sc_module(const sc_module& module)
{
	// DISABLED
}

sc_module& sc_module::operator = ( const sc_module& )
{
	// DISABLED
}

void sc_module::end_module()
{
	sc_kernel *kernel = sc_kernel::get_kernel();
	kernel->end_module();
}

void sc_module::init()
{
	sc_module_name *non_const_module_name = kernel->get_top_of_module_name_stack();
	non_const_module_name->set_module(this);
	kernel->begin_module(this);
	kernel->register_module(this);
}

} // end of namespace sc_core
