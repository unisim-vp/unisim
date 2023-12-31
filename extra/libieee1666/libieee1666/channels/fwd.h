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

#ifndef __LIBIEEE1666_CHANNELS_FWD_H__
#define __LIBIEEE1666_CHANNELS_FWD_H__

namespace sc_core {

enum sc_writer_policy
{
	SC_ONE_WRITER,
	SC_MANY_WRITERS,
	SC_UNCHECKED_WRITERS // not in IEEE1666-2011 but in SystemC >= 2.3.0
};

template <class T, sc_writer_policy WRITER_POLICY> class sc_buffer;
template <class T> class sc_fifo;
template <class T> class sc_fifo_nonblocking_in_if;
template <class T> class sc_fifo_blocking_in_if;
template <class T> class sc_fifo_in_if;
template <class T> class sc_fifo_nonblocking_out_if;
template <class T> class sc_fifo_blocking_out_if;
template <class T> class sc_fifo_out_if;
template <class T> class sc_fifo_in;
template <class T> class sc_fifo_out;
template <class T> class sc_in;
class sc_in_resolved;
template <class T> class sc_inout;
class sc_inout_resolved;
class sc_mutex;
class sc_mutex_if;
template <class T> class sc_out;
class sc_out_resolved;
class sc_semaphore;
class sc_semaphore_if;
template <class T, sc_writer_policy WRITER_POLICY> class sc_signal;
class sc_signal_resolved;
template <class T> class sc_signal_in_if;

} // end of namespace sc_core

#endif
