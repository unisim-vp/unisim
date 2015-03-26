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

#include <ieee1666/kernel/process_handle.h>

namespace sc_core {

//////////////////////////////// sc_unwind_exception //////////////////////////////////////////

const char* sc_unwind_exception::what() const throw()
{
}

bool sc_unwind_exception::is_reset() const
{
}

sc_unwind_exception::sc_unwind_exception()
{
}

sc_unwind_exception::sc_unwind_exception( const sc_unwind_exception& )
{
}

sc_unwind_exception::~sc_unwind_exception() throw()
{
}

///////////////////////////////// sc_process_handle ///////////////////////////////////////////

sc_process_handle::sc_process_handle()
{
}

sc_process_handle::sc_process_handle( const sc_process_handle& )
{
}

sc_process_handle::sc_process_handle( sc_object* )
{
}

sc_process_handle::~sc_process_handle()
{
}

bool sc_process_handle::valid() const
{
}

sc_process_handle& sc_process_handle::operator= ( const sc_process_handle& )
{
}

bool sc_process_handle::operator== ( const sc_process_handle& ) const
{
}

bool sc_process_handle::operator!= ( const sc_process_handle& ) const
{
}

bool sc_process_handle::operator< ( const sc_process_handle& ) const
{
}

void sc_process_handle::swap( sc_process_handle& )
{
}

const char* sc_process_handle::name() const
{
}

sc_curr_proc_kind sc_process_handle::proc_kind() const
{
}

const std::vector<sc_object*>& sc_process_handle::get_child_objects() const
{
}

const std::vector<sc_event*>& sc_process_handle::get_child_events() const
{
}

sc_object* sc_process_handle::get_parent_object() const
{
}

sc_object* sc_process_handle::get_process_object() const
{
}

bool sc_process_handle::dynamic() const
{
}

bool sc_process_handle::terminated() const
{
}

const sc_event& sc_process_handle::terminated_event() const
{
}

void sc_process_handle::suspend ( sc_descendant_inclusion_info include_descendants)
{
}

void sc_process_handle::resume ( sc_descendant_inclusion_info include_descendants)
{
}

void sc_process_handle::disable ( sc_descendant_inclusion_info include_descendants)
{
}

void sc_process_handle::enable ( sc_descendant_inclusion_info include_descendants)
{
}

void sc_process_handle::kill( sc_descendant_inclusion_info include_descendants)
{
}

void sc_process_handle::reset( sc_descendant_inclusion_info include_descendants)
{
}

bool sc_process_handle::is_unwinding() const
{
}

const sc_event& sc_process_handle::reset_event() const
{
}

void sc_process_handle::sync_reset_on( sc_descendant_inclusion_info include_descendants)
{
}

void sc_process_handle::sync_reset_off( sc_descendant_inclusion_info include_descendants)
{
}

///////////////////////////////// global functions  ///////////////////////////////////////////

sc_process_handle sc_get_current_process_handle()
{
}

bool sc_is_unwinding()
{
}

} // end of namespace sc_core
