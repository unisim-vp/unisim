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

#ifndef __IEEE1666_KERNEL_PROCESS_H__
#define __IEEE1666_KERNEL_PROCESS_H__

#include <ieee1666/kernel/fwd.h>
#include <ieee1666/kernel/object.h>

namespace sc_core {

class sc_process_owner 
{
public:
	sc_process_owner(bool automatic = false);
	virtual ~sc_process_owner();
	
	bool is_automatic() const;
private:
	bool automatic;
};

class sc_process : public sc_object
{
public:
	sc_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, sc_curr_proc_kind process_kind);
	virtual ~sc_process();
	
	void call_process_owner_method();
	
	sc_curr_proc_kind proc_kind() const;
	bool dynamic() const;
	
	virtual bool terminated() const = 0;
	virtual const sc_event& terminated_event() const = 0;
	virtual void suspend(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS) = 0;
	virtual void resume(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS) = 0;
	virtual void disable(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS) = 0;
	virtual void enable(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS) = 0;
	virtual void kill(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS) = 0;
	virtual void reset(sc_descendant_inclusion_info include_descendants = SC_NO_DESCENDANTS) = 0;
	void acquire();
	void release();
	
	void add_static_sensitivity(const sc_event& e);
	void clear_static_sensitivity();
private:
	sc_process_owner *process_owner;
	sc_process_owner_method_ptr process_owner_method_ptr;
	sc_curr_proc_kind process_kind;
	bool flag_dynamic;
	bool automatic_process_owner;

	unsigned int ref_count;

	std::vector<const sc_event *> static_sensitivity;
protected:
	bool enabled;
	bool suspended;
	bool runnable_on_resuming;
};

} // end of namespace sc_core

#endif
