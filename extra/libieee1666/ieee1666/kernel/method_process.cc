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

#include <ieee1666/kernel/method_process.h>
#include <ieee1666/kernel/kernel.h>

namespace sc_core {

sc_method_process::sc_method_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, const sc_spawn_options *spawn_options)
	: sc_process(_name, _process_owner, _process_owner_method_ptr, SC_METHOD_PROC_)
{
	sc_kernel::get_kernel()->add_method_process(this);
}

sc_method_process::~sc_method_process()
{
}

void sc_method_process::next_trigger()
{
}

void sc_method_process::next_trigger(const sc_event& e)
{
	e.add_dynamically_sensitive_method_process(this);
}

void sc_method_process::suspend(sc_descendant_inclusion_info include_descendants)
{
}

void sc_method_process::resume(sc_descendant_inclusion_info include_descendants)
{
}

void sc_method_process::disable(sc_descendant_inclusion_info include_descendants)
{
}

void sc_method_process::enable(sc_descendant_inclusion_info include_descendants)
{
}

void sc_method_process::kill(sc_descendant_inclusion_info include_descendants)
{
}

void sc_method_process::reset(sc_descendant_inclusion_info include_descendants)
{
}

} // end of namespace sc_core
