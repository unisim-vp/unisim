/*
 *  Copyright (c) 2015,
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

#ifndef __IEEE1666_TLM2_UTIL_SIMPLE_TARGET_SOCKET_H__
#define __IEEE1666_TLM2_UTIL_SIMPLE_TARGET_SOCKET_H__

namespace tlm_utils {

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
class simple_target_socket
	: public tlm::tlm_target_socket<BUSWIDTH, TYPES>
{
public:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type phase_type;
	typedef tlm::tlm_sync_enum sync_enum_type;

	simple_target_socket();
	explicit simple_target_socket(const char *n);
	tlm::tlm_bw_transport_if<TYPES> *operator -> ();
	void register_nb_transport_fw(MODULE* mod, sync_enum_type (MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&));
	void register_b_transport(MODULE* mod, void (MODULE::*cb)(transaction_type&, sc_core::sc_time&));
	void register_transport_dbg(MODULE* mod, unsigned int (MODULE::*cb)(transaction_type&));
	void register_get_direct_mem_ptr(MODULE* mod, bool (MODULE::*cb)(transaction_type&, tlm::tlm_dmi&));
};

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::simple_target_socket()
{
}

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::simple_target_socket(const char *n)
{
}

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
tlm::tlm_bw_transport_if<TYPES> *simple_target_socket<MODULE, BUSWIDTH, TYPES>::operator -> ()
{
}

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_nb_transport_fw(MODULE* mod, sync_enum_type (MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&))
{
}

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_b_transport(MODULE* mod, void (MODULE::*cb)(transaction_type&, sc_core::sc_time&))
{
}

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_transport_dbg(MODULE* mod, unsigned int (MODULE::*cb)(transaction_type&))
{
}

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_get_direct_mem_ptr(MODULE* mod, bool (MODULE::*cb)(transaction_type&, tlm::tlm_dmi&))
{
}

} // end of namespace tlm_utils

#endif // __IEEE1666_TLM2_UTIL_SIMPLE_INITIATOR_SOCKET_H__
