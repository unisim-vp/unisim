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

#ifndef __IEEE1666_TLM2_INTEROPERABILITY_LAYER_SOCKET_TARGET_SOCKET_H__
#define __IEEE1666_TLM2_INTEROPERABILITY_LAYER_SOCKET_TARGET_SOCKET_H__

#include <ieee1666/tlm2/interoperability_layer/fwd.h>

namespace tlm {

// Abstract base class for target sockets
template <unsigned int BUSWIDTH = 32, typename FW_IF = tlm_fw_transport_if<>, typename BW_IF = tlm_bw_transport_if<> >
class tlm_base_target_socket_b
{
public:
	virtual ~tlm_base_target_socket_b();
	virtual sc_core::sc_port_b<BW_IF>& get_base_port() = 0;
	virtual const sc_core::sc_port_b<BW_IF>& get_base_port() const = 0;
	virtual sc_core::sc_export<FW_IF>& get_base_export() = 0;
	virtual const sc_core::sc_export <FW_IF>& get_base_export() const = 0;
	virtual FW_IF& get_base_interface() = 0;
	virtual const FW_IF& get_base_interface() const = 0;
};

// Base class for target sockets, providing binding methods
template <unsigned int BUSWIDTH = 32, typename FW_IF = tlm_fw_transport_if<>, typename BW_IF = tlm_bw_transport_if<>,
          int N = 1, sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class tlm_base_target_socket
	: public tlm_base_target_socket_b<BUSWIDTH, FW_IF, BW_IF>
	, public sc_core::sc_export<FW_IF>
{
public:
	typedef FW_IF fw_interface_type;
	typedef BW_IF bw_interface_type;
	typedef sc_core::sc_port<bw_interface_type, N, POL> port_type;
	typedef sc_core::sc_export<fw_interface_type> export_type;
	typedef tlm_base_initiator_socket_b<BUSWIDTH, fw_interface_type, bw_interface_type> base_initiator_socket_type;
	typedef tlm_base_target_socket_b<BUSWIDTH, fw_interface_type, bw_interface_type> base_type;

	tlm_base_target_socket();
	explicit tlm_base_target_socket(const char* name);
	virtual const char* kind() const;
	unsigned int get_bus_width() const;
	virtual void bind(base_initiator_socket_type& s);
	void operator() (base_initiator_socket_type& s);
	virtual void bind(base_type& s);
	void operator() (base_type& s);
	virtual void bind(fw_interface_type& ifs);
	void operator() (fw_interface_type& s);
	int size() const;
	bw_interface_type* operator-> ();
	bw_interface_type* operator[] (int i);

	// Implementation of pure virtual functions of base class
	virtual sc_core::sc_port_b<BW_IF> &get_base_port() { return m_port; }
	virtual const sc_core::sc_port_b<BW_IF> & get_base_port() const { return m_port; }

	virtual FW_IF& get_base_interface() { return *this; }
	virtual const FW_IF& get_base_interface() const { return *this; }
	virtual sc_core::sc_export<FW_IF>& get_base_export() { return *this; }
	virtual const sc_core::sc_export<FW_IF>& get_base_export() const { return *this; }

protected:
	port_type m_port;
};

// Principal target socket, parameterized with protocol traits class
template <unsigned int BUSWIDTH = 32, typename TYPES = tlm_base_protocol_types,
          int N = 1, sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class tlm_target_socket
	: public tlm_base_target_socket <BUSWIDTH, tlm_fw_transport_if<TYPES>, tlm_bw_transport_if<TYPES>, N, POL>
{
public:
	tlm_target_socket();
	explicit tlm_target_socket(const char* name);
	virtual const char* kind() const;
};

} // end of namespace tlm

#endif // __IEEE1666_TLM2_INTEROPERABILITY_LAYER_SOCKET_TARGET_SOCKET_H__
