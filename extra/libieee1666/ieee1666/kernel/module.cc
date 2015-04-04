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

namespace sc_core {

/////////////////////////////////// sc_module /////////////////////////////////////////////

sc_module::~sc_module()
{
}

const char* sc_module::kind() const
{
}

//void operator() ( const sc_bind_proxy& p001,
//const sc_bind_proxy& p002 = SC_BIND_PROXY_NIL,
//const sc_bind_proxy& p003 = SC_BIND_PROXY_NIL,
//...
//const sc_bind_proxy& p063 = SC_BIND_PROXY_NIL,
//const sc_bind_proxy& p064 = SC_BIND_PROXY_NIL )
//{
//}

const std::vector<sc_object*>& sc_module::get_child_objects() const
{
}

const std::vector<sc_event*>& sc_module::get_child_events() const
{
}

sc_module::sc_module( const sc_module_name& module_name )
	: sc_object((const char *) sc_kernel::get_kernel()->get_top_of_module_name_stack())
{
	sc_module_name *non_const_module_name = sc_kernel::get_kernel()->get_top_of_module_name_stack();
	non_const_module_name->set_module(this);
	init();
}

sc_module::sc_module()
	: sc_object((const char *) sc_kernel::get_kernel()->get_top_of_module_name_stack())
{
	sc_module_name *non_const_module_name = sc_kernel::get_kernel()->get_top_of_module_name_stack();
	non_const_module_name->set_module(this);
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
}

void sc_module::set_stack_size( size_t )
{
}

void sc_module::next_trigger()
{
}

void sc_module::next_trigger( const sc_event& )
{
}

void sc_module::next_trigger( const sc_event_or_list & )
{
}

void sc_module::next_trigger( const sc_event_and_list & )
{
}

void sc_module::next_trigger( const sc_time& )
{
}

void sc_module::next_trigger( double , sc_time_unit )
{
}

void sc_module::next_trigger( const sc_time& , const sc_event& )
{
}

void sc_module::next_trigger( double , sc_time_unit , const sc_event& )
{
}

void sc_module::next_trigger( const sc_time& , const sc_event_or_list &)
{
}

void sc_module::next_trigger( double , sc_time_unit , const sc_event_or_list & )
{
}

void sc_module::next_trigger( const sc_time& , const sc_event_and_list & )
{
}

void sc_module::next_trigger( double , sc_time_unit , const sc_event_and_list & )
{
}

void sc_module::wait()
{
	sc_kernel::get_kernel()->get_current_thread_process()->wait();
}

void sc_module::wait( int )
{
}

void sc_module::wait( const sc_event& e)
{
	sc_kernel::get_kernel()->get_current_thread_process()->wait(e);
}

void sc_module::wait( const sc_event_or_list &)
{
}

void sc_module::wait( const sc_event_and_list & )
{
}

void sc_module::wait( const sc_time& )
{
}

void sc_module::wait( double , sc_time_unit )
{
}

void sc_module::wait( const sc_time& , const sc_event& )
{
}

void sc_module::wait( double , sc_time_unit , const sc_event& )
{
}

void sc_module::wait( const sc_time& , const sc_event_or_list & )
{
}

void sc_module::wait( double , sc_time_unit , const sc_event_or_list & )
{
}

void sc_module::wait( const sc_time& , const sc_event_and_list & )
{
}

void sc_module::wait( double , sc_time_unit , const sc_event_and_list & )
{
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

sc_module::sc_module( const sc_module& )
{
}

sc_module& sc_module::operator= ( const sc_module& )
{
}

void sc_module::end_module()
{
	sc_kernel *kernel = sc_kernel::get_kernel();
	kernel->end_object();
}

void sc_module::init()
{
	sc_kernel::get_kernel()->add_module(this);
}

//////////////////////////////////// global functions ///////////////////////////////////////

void next_trigger()
{
}

void next_trigger( const sc_event& )
{
}

void next_trigger( const sc_event_or_list & )
{
}

void next_trigger( const sc_event_and_list & )
{
}

void next_trigger( const sc_time& )
{
}

void next_trigger( double , sc_time_unit )
{
}

void next_trigger( const sc_time& , const sc_event& )
{
}

void next_trigger( double , sc_time_unit , const sc_event& )
{
}

void next_trigger( const sc_time& , const sc_event_or_list & )
{
}

void next_trigger( double , sc_time_unit , const sc_event_or_list & )
{
}

void next_trigger( const sc_time& , const sc_event_and_list & )
{
}

void next_trigger( double , sc_time_unit , const sc_event_and_list & )
{
}

void wait()
{
}

void wait( int )
{
}

void wait( const sc_event& )
{
}

void wait( const sc_event_or_list & )
{
}

void wait( const sc_event_and_list & )
{
}

void wait( const sc_time& )
{
}

void wait( double , sc_time_unit )
{
}

void wait( const sc_time& , const sc_event& )
{
}

void wait( double , sc_time_unit , const sc_event& )
{
}

void wait( const sc_time& , const sc_event_or_list & )
{
}

void wait( double , sc_time_unit , const sc_event_or_list & )
{
}

void wait( const sc_time& , const sc_event_and_list & )
{
}

void wait( double , sc_time_unit , const sc_event_and_list & )
{
}

const char* sc_gen_unique_name( const char* )
{
}

} // end of namespace sc_core
