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

#ifndef __IEEE1666_KERNEL_PROCESS_HANDLE_H__
#define __IEEE1666_KERNEL_PROCESS_HANDLE_H__

#include <ieee1666/kernel/fwd.h>
#include <ieee1666/kernel/object.h>
#include <ieee1666/kernel/event.h>
#include <stdexcept>

namespace sc_core {

class sc_unwind_exception: public std::exception
{
public:
	virtual const char* what() const throw();
	virtual bool is_reset() const;
protected:
	sc_unwind_exception();
	sc_unwind_exception( const sc_unwind_exception& );
	virtual ~sc_unwind_exception() throw();
};

class sc_process_handle
{
public:
	sc_process_handle();
	sc_process_handle( const sc_process_handle& );
	explicit sc_process_handle( sc_object* );
	~sc_process_handle();
	
	bool valid() const;
	
	sc_process_handle& operator= ( const sc_process_handle& );
	bool operator== ( const sc_process_handle& ) const;
	bool operator!= ( const sc_process_handle& ) const;
	bool operator< ( const sc_process_handle& ) const;
	void swap( sc_process_handle& );
	
	const char* name() const;
	sc_curr_proc_kind proc_kind() const;
	const std::vector<sc_object*>& get_child_objects() const;
	const std::vector<sc_event*>& get_child_events() const;
	sc_object* get_parent_object() const;
	sc_object* get_process_object() const;
	bool dynamic() const;
	bool terminated() const;
	const sc_event& terminated_event() const;

	void suspend ( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
	void resume ( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
	void disable ( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
	void enable ( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
	void kill( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
	void reset( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
	bool is_unwinding() const;
	const sc_event& reset_event() const;
	
	void sync_reset_on( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
	void sync_reset_off( sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );

	template <typename T>
	void throw_it( const T& user_defined_exception, sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS );
private:
	sc_process *process;
	sc_event null_event;
	
	sc_process_handle(sc_process *process);
};

sc_process_handle sc_get_current_process_handle();
bool sc_is_unwinding();

///////////////////////////////// sc_process_handle ///////////////////////////////////////////

template <typename T>
void sc_process_handle::throw_it( const T& user_defined_exception, sc_descendant_inclusion_info include_descendants)
{
}

} // end of namespace sc_core

#endif
