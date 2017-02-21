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
private:
	struct fw_transport_impl_s : public tlm::tlm_fw_transport_if<TYPES>
	{
		fw_transport_impl_s();
		
		virtual tlm::tlm_sync_enum nb_transport_fw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t);
		virtual void b_transport(transaction_type& trans, sc_core::sc_time& t);
		virtual unsigned int transport_dbg(transaction_type& trans);
		virtual bool get_direct_mem_ptr(transaction_type& trans, tlm::tlm_dmi& dmi_data);

		MODULE *mod;
		sync_enum_type (MODULE::*nb_transport_fw_cb)(transaction_type&, phase_type&, sc_core::sc_time&);
		void (MODULE::*b_transport_cb)(transaction_type&, sc_core::sc_time&);
		unsigned int (MODULE::*transport_dbg_cb)(transaction_type&);
		bool (MODULE::*get_direct_mem_ptr_cb)(transaction_type&, tlm::tlm_dmi&);
	};
	
	struct bw_transport_impl_s : public tlm::tlm_bw_transport_if<TYPES>
	{
		bw_transport_impl_s();
		
		virtual tlm::tlm_sync_enum nb_transport_bw(transaction_type&, phase_type&, sc_core::sc_time&);
		virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);

		simple_target_socket<MODULE, BUSWIDTH, TYPES> *socket;
	};

	fw_transport_impl_s fw_transport_impl;
	bw_transport_impl_s bw_transport_impl;
};

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::simple_target_socket()
	: tlm::tlm_target_socket<BUSWIDTH, TYPES>(sc_core::sc_gen_unique_name("simple_target_socket"))
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::simple_target_socket(const char *n)
	: tlm::tlm_target_socket<BUSWIDTH, TYPES>(n)
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_bw_transport_if<TYPES> *simple_target_socket<MODULE, BUSWIDTH, TYPES>::operator -> ()
{
	return tlm::tlm_target_socket<BUSWIDTH, TYPES>::operator -> ();
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_nb_transport_fw(MODULE* mod, sync_enum_type (MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw "tlm_utils::simple_target_socket: a module is already registered";
	fw_transport_impl.mod = mod;
	fw_transport_impl.nb_transport_fw_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_b_transport(MODULE* mod, void (MODULE::*cb)(transaction_type&, sc_core::sc_time&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw "tlm_utils::simple_target_socket: a module is already registered";
	fw_transport_impl.mod = mod;
	fw_transport_impl.b_transport_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_transport_dbg(MODULE* mod, unsigned int (MODULE::*cb)(transaction_type&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw "tlm_utils::simple_target_socket: a module is already registered";
	fw_transport_impl.mod = mod;
	fw_transport_impl.transport_dbg_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_get_direct_mem_ptr(MODULE* mod, bool (MODULE::*cb)(transaction_type&, tlm::tlm_dmi&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw "tlm_utils::simple_target_socket: a module is already registered";
	fw_transport_impl.mod = mod;
	fw_transport_impl.get_direct_mem_ptr_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::fw_transport_impl_s()
	: mod(0)
	, nb_transport_fw_cb(0)
	, b_transport_cb(0)
	, transport_dbg_cb(0)
	, get_direct_mem_ptr_cb(0)
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_transport_fw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t)
{
	return (mod->*nb_transport_fw_cb)(trans, phase, t);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::b_transport(transaction_type& trans, sc_core::sc_time& t)
{
	if(b_transport_cb)
	{
		(mod->*b_transport_cb)(trans, t);
	}
	else
	{
		if(!nb_transport_fw) throw "tlm_utils::simple_target_socket: no nb_transport_fw callback registered";

		/*
		tlm::tlm_phase phase;
		tlm::tlm_sync_enum sync = (mod->*nb_transport_fw)(trans, phase, t);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
				break;
			case tlm::TLM_UPDATED:
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
		
		sync = this->nb_transport_bw(trans, phase, t);

		switch(sync)
		{
			case BEGIN_REQ:
				break;
			case END_REQ:
				break;
			case BEGIN_RESP:
				break;
			case END_RESP:
				break;
		}
		*/
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
unsigned int simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::transport_dbg(transaction_type& trans)
{
	return (mod->*transport_dbg_cb)(trans);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
bool simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::get_direct_mem_ptr(transaction_type& trans, tlm::tlm_dmi& dmi_data)
{
	return (mod->*get_direct_mem_ptr_cb)(trans, dmi_data);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_target_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::nb_transport_bw(transaction_type&, phase_type&, sc_core::sc_time&)
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64)
{
}

} // end of namespace tlm_utils

#endif // __IEEE1666_TLM2_UTIL_SIMPLE_INITIATOR_SOCKET_H__
