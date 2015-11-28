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

#ifndef __IEEE1666_KERNEL_MODULE_H__
#define __IEEE1666_KERNEL_MODULE_H__

#include <ieee1666/kernel/fwd.h>
#include <ieee1666/base/fwd.h>
#include <ieee1666/kernel/sensitive.h>
#include <ieee1666/kernel/process.h>
#include <ieee1666/kernel/spawn.h>
#include <vector>

namespace sc_core {

// class sc_bind_proxy
// {
// };
//const sc_bind_proxy SC_BIND_PROXY_NIL;

class sc_module
	: public sc_object
	, public sc_process_owner
{
public:
	virtual ~sc_module();
	virtual const char* kind() const;
// 	void operator() ( const sc_bind_proxy& p001,
// 	const sc_bind_proxy& p002 = SC_BIND_PROXY_NIL,
// 	const sc_bind_proxy& p003 = SC_BIND_PROXY_NIL,
// 	...
// 	const sc_bind_proxy& p063 = SC_BIND_PROXY_NIL,
// 	const sc_bind_proxy& p064 = SC_BIND_PROXY_NIL );
protected:
	sc_module( const sc_module_name& );
	sc_module();
	void reset_signal_is( const sc_in<bool>& , bool );
	void reset_signal_is( const sc_inout<bool>& , bool );
	void reset_signal_is( const sc_out<bool>& , bool );
	void reset_signal_is( const sc_signal_in_if<bool>& , bool );
	void async_reset_signal_is( const sc_in<bool>& , bool );
	void async_reset_signal_is( const sc_inout<bool>& , bool );
	void async_reset_signal_is( const sc_out<bool>& , bool );
	void async_reset_signal_is( const sc_signal_in_if<bool>& , bool );
	sc_sensitive sensitive;
	void dont_initialize();
	void set_stack_size( size_t );
	void next_trigger();
	void next_trigger( const sc_event& );
	void next_trigger( const sc_event_or_list & );
	void next_trigger( const sc_event_and_list & );
	void next_trigger( const sc_time& );
	void next_trigger( double , sc_time_unit );
	void next_trigger( const sc_time& , const sc_event& );
	void next_trigger( double , sc_time_unit , const sc_event& );
	void next_trigger( const sc_time& , const sc_event_or_list &);
	void next_trigger( double , sc_time_unit , const sc_event_or_list & );
	void next_trigger( const sc_time& , const sc_event_and_list & );
	void next_trigger( double , sc_time_unit , const sc_event_and_list & );
	void wait();
	void wait( int );
	void wait( const sc_event& );
	void wait( const sc_event_or_list &);
	void wait( const sc_event_and_list & );
	void wait( const sc_time& );
	void wait( double , sc_time_unit );
	void wait( const sc_time& , const sc_event& );
	void wait( double , sc_time_unit , const sc_event& );
	void wait( const sc_time& , const sc_event_or_list & );
	void wait( double , sc_time_unit , const sc_event_or_list & );
	void wait( const sc_time& , const sc_event_and_list & );
	void wait( double , sc_time_unit , const sc_event_and_list & );
	virtual void before_end_of_elaboration();
	virtual void end_of_elaboration();
	virtual void start_of_simulation();
	virtual void end_of_simulation();
private:
	// Disabled
	sc_module( const sc_module& );
	sc_module& operator= ( const sc_module& );
	
	///////////////////// EVERYTHING BELOW IS NOT PART OF IEEE1666 STANDARD /////////////////////////
protected:
	friend class sc_module_name;
	friend class sc_kernel;

	sc_spawn_options spawn_options;
	
	void end_module();
	
private:
	void init();
};

#define SC_MODULE(name) struct name : public ::sc_core::sc_module
#define SC_CTOR(name) typedef name SC_CURRENT_USER_MODULE; name(const ::sc_core::sc_module_name&)
#define SC_HAS_PROCESS(name) typedef name SC_CURRENT_USER_MODULE // implementation-defined
#define SC_METHOD(name) sensitive.bind(::sc_core::sc_kernel::get_kernel()->create_method_process(#name, this, static_cast<::sc_core::sc_process_owner_method_ptr>(&SC_CURRENT_USER_MODULE::name),&this->spawn_options)) // implementation-defined
#define SC_THREAD(name) sensitive.bind(::sc_core::sc_kernel::get_kernel()->create_thread_process(#name, this, static_cast<::sc_core::sc_process_owner_method_ptr>(&SC_CURRENT_USER_MODULE::name),&this->spawn_options)) // implementation-defined
#define SC_CTHREAD(name,clk_edge_event_finder) ::sc_core::sc_kernel::get_kernel()->create_cthread_process(#name, this, static_cast<::sc_core::sc_process_owner_method_ptr>(&SC_CURRENT_USER_MODULE::name),&this->spawn_options, clk_edge_event_finder) // implementation-defined
typedef sc_module sc_behavior;
typedef sc_module sc_channel;

} // end of namespace sc_core

#endif
