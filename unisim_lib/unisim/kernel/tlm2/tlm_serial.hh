/*
 *  Copyright (c) 2017,
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
 * Author: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_KERNEL_TLM2_TLM_SERIAL_HH__
#define __UNISIM_KERNEL_TLM2_TLM_SERIAL_HH__

#include <tlm>
#include <iostream>
#include <string.h>
#include <assert.h>

namespace unisim {
namespace kernel {
namespace tlm2 {

/////////////////////////////// forward declarations //////////////////////////

class tlm_serial_payload;
class tlm_serial_mm_interface;
class tlm_serial_payload;
struct tlm_default_serial_protocol;
class tlm_serial_fw_if;
class tlm_serial_bw_if;
template <unsigned int BUSWIDTH, typename PROTOCOL, int N, sc_core::sc_port_policy POL> class tlm_serial_initiator_socket;
template <unsigned int BUSWIDTH, typename PROTOCOL, int N, sc_core::sc_port_policy POL> class tlm_serial_target_socket;

/////////////////////////////////// declarations //////////////////////////////

class tlm_serial_mm_interface
{
public:
	virtual void free(tlm_serial_payload *) = 0;
	virtual ~tlm_serial_mm_interface() {}
};

class tlm_serial_payload
{
public:
	inline tlm_serial_payload();
	inline explicit tlm_serial_payload(tlm_serial_mm_interface *);
	inline virtual ~tlm_serial_payload();

private:
	// Disable copy constructor and assignment operator
	inline tlm_serial_payload(const tlm_serial_payload&);
	inline tlm_serial_payload& operator = (const tlm_serial_payload&);

public:
	// Access methods
	inline void set_data(const std::vector<bool>& _data) { data = _data; }
	inline void set_period(const sc_core::sc_time& _period) { period = _period; }
	inline const std::vector<bool> get_data() const { return data; }
	inline const sc_core::sc_time& get_period() const { return period; }
	
	// Memory management
	inline void set_mm(tlm_serial_mm_interface *_mm) { mm = _mm; }
	inline bool has_mm() const { return mm != 0; }
	inline unsigned int get_ref_count() const { return ref_count; }
	inline void acquire() { assert(mm != 0); ref_count++; }
	inline void release() { assert((mm != 0) && (ref_count > 0)); if(--ref_count == 0) mm->free(this); }
	inline void deep_copy_from(const tlm_serial_payload& other);
	inline void update_extensions_from(const tlm_serial_payload & other);
	inline void free_all_extensions();

	// Extension mechanism
	template <typename T> T *set_extension(T *);
	tlm::tlm_extension_base *set_extension(unsigned int, tlm::tlm_extension_base *);
	template <typename T> T *set_auto_extension(T *);
	tlm::tlm_extension_base *set_auto_extension(unsigned int, tlm::tlm_extension_base *);
	template <typename T> void get_extension(T *&) const;
	template <typename T> T* get_extension() const;
	tlm::tlm_extension_base *get_extension(unsigned int) const;
	template <typename T> void clear_extension(const T *);
	template <typename T> void clear_extension();
	template <typename T> void release_extension(T *);
	template <typename T> void release_extension();
	void resize_extensions();
private:
	std::vector<bool> data;
	sc_core::sc_time period;
	tlm_serial_mm_interface *mm;
	unsigned int ref_count;
	
	struct tlm_extension_slot
	{
		tlm_extension_slot();
		
		tlm::tlm_extension_base *extension;
		bool sticky;
	};
	
	std::vector<tlm_extension_slot> extension_slots;
	
	void free_auto_extensions();
};

struct tlm_default_serial_protocol
{
};

class tlm_serial_fw_if : public virtual sc_core::sc_interface
{
public:
	virtual void b_send(tlm_serial_payload&) = 0;
};

class tlm_serial_bw_if : public virtual sc_core::sc_interface
{
public:
	virtual void nb_receive(tlm_serial_payload&) = 0;
};

template <unsigned int BUSWIDTH = 1, typename PROTOCOL = tlm_default_serial_protocol, int N = 1, sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class tlm_serial_peripheral_socket : public tlm::tlm_base_initiator_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>
{
public:
	tlm_serial_peripheral_socket() : tlm::tlm_base_initiator_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>(sc_core::sc_gen_unique_name("tlm_serial_peripheral_socket")) {}
	explicit tlm_serial_peripheral_socket(const char *name) : tlm::tlm_base_initiator_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>(sc_core::sc_gen_unique_name(name)) {}
	virtual const char *kind() const { return "tlm_serial_peripheral_socket"; }
};

template <unsigned int BUSWIDTH = 1, typename PROTOCOL = tlm_default_serial_protocol, int N = 0, sc_core::sc_port_policy POL = sc_core::SC_ZERO_OR_MORE_BOUND>
class tlm_serial_bus_socket : public tlm::tlm_base_target_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>
{
public:
	tlm_serial_bus_socket() : tlm::tlm_base_target_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>(sc_core::sc_gen_unique_name("tlm_serial_bus_socket")) {}
	explicit tlm_serial_bus_socket(const char *name) : tlm::tlm_base_target_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>(sc_core::sc_gen_unique_name(name)) {}
	virtual const char *kind() const { return "tlm_serial_bus_socket"; }
};

template <typename MODULE, unsigned int BUSWIDTH = 1, typename PROTOCOL = tlm_default_serial_protocol, int N = 1, sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class tlm_serial_peripheral_socket_tagged : public tlm_serial_peripheral_socket<BUSWIDTH, PROTOCOL, N, POL>
{
public:
	tlm_serial_peripheral_socket_tagged()
		: tlm_serial_peripheral_socket<BUSWIDTH, PROTOCOL, N, POL>(sc_core::sc_gen_unique_name("tlm_serial_peripheral_socket_tagged"))
	{
		this->bind(serial_bw_if_impl);
	}
	
	explicit tlm_serial_peripheral_socket_tagged(const char *name)
		: tlm_serial_peripheral_socket<BUSWIDTH, PROTOCOL, N, POL>(sc_core::sc_gen_unique_name(name))
	{
		this->bind(serial_bw_if_impl);
	}
	
	void register_nb_receive(MODULE *mod, void (MODULE::*nb_receive_cb)(int, tlm_serial_payload&), int id)
	{
		serial_bw_if_impl.mod = mod;
		serial_bw_if_impl.id = id;
		serial_bw_if_impl.nb_receive_cb = nb_receive_cb;
	}
	
private:
	struct serial_bw_if_impl_s : public tlm_serial_bw_if
	{
		serial_bw_if_impl_s()
			: mod(0), id(0), nb_receive_cb(0)
		{
		}
		
		virtual void nb_receive(tlm_serial_payload& payload)
		{
			if(!mod || !nb_receive_cb) return;
			
			(mod->*nb_receive_cb)(id, payload);
		}
		
		MODULE *mod;
		int id;
		void (MODULE::*nb_receive_cb)(int, tlm_serial_payload&);
	};
	
	serial_bw_if_impl_s serial_bw_if_impl;
};

class tlm_simple_serial_bus
	: public sc_core::sc_module
	, public tlm_serial_fw_if
{
public:
	tlm_serial_bus_socket<> serial_socket;
	
	tlm_simple_serial_bus(const sc_core::sc_module_name& name)
		: serial_socket("serial_socket")
	{
		serial_socket(*this);
	}
	
	virtual void b_send(tlm_serial_payload& payload)
	{
		int n = serial_socket.size();
		int i;
		for(i = 0; i < n; i++)
		{
			serial_socket[i]->nb_receive(payload);
		}
	}
};

/////////////////////////////////// definitions ///////////////////////////////

inline tlm_serial_payload::tlm_serial_payload()
	: data()
	, period(sc_core::SC_ZERO_TIME)
	, mm(0)
	, ref_count(0)
{
}

inline tlm_serial_payload::tlm_serial_payload(tlm_serial_mm_interface *_mm)
	: data()
	, period(sc_core::SC_ZERO_TIME)
	, mm(_mm)
	, ref_count(0)
{
}

inline tlm_serial_payload::~tlm_serial_payload()
{
	free_auto_extensions();
}

inline tlm_serial_payload::tlm_serial_payload(const tlm_serial_payload&)
{
}

inline tlm_serial_payload& tlm_serial_payload::operator = (const tlm_serial_payload&)
{
	return *this;
}

inline void tlm_serial_payload::deep_copy_from(const tlm_serial_payload& other)
{
	data = other.data;
	period = other.period;
	
	extension_slots.reserve(other.extension_slots.capacity());
	typename std::vector<tlm_extension_slot>::size_type num_extensions = other.extension_slots.size();
	
	if(num_extensions > extension_slots.size())
	{
		extension_slots.resize(num_extensions);
	}
	
	typename std::vector<tlm_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_extension_slot& orig_extension_slot = other.extension_slots[extension_id];
		
		if(orig_extension_slot.extension)
		{
			if(extension_slots[extension_id].extension)
			{
				extension_slots[extension_id].extension->copy_from(*orig_extension_slot.extension);
			}
			else
			{
				tlm::tlm_extension_base *cloned_extension = other.extension_slots[extension_id].extension->clone();
				
				if(cloned_extension)
				{
					if(mm)
					{
						set_auto_extension(extension_id, cloned_extension);
					}
					else
					{
						set_extension(extension_id, cloned_extension);
					}
				}
			}
		}
	}
}

inline void tlm_serial_payload::update_extensions_from(const tlm_serial_payload& other)
{
	typename std::vector<tlm_extension_slot>::size_type num_extensions = other.extension_slots.size();
	typename std::vector<tlm_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_extension_slot& orig_extension_slot = other.extension_slots[extension_id];
		
		if(orig_extension_slot.extension)
		{
			if(extension_slots[extension_id].extension)
			{
				extension_slots[extension_id].extension->copy_from(*orig_extension_slot.extension);
			}
		}
	}
}

inline void tlm_serial_payload::free_all_extensions()
{
	typename std::vector<tlm_extension_slot>::size_type num_extensions = extension_slots.size();
	typename std::vector<tlm_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_extension_slot& extension_slot = extension_slots[extension_id];
		
		if(extension_slot.extension)
		{
			extension_slot.extension->free();
			
			extension_slots[extension_id].extension = 0;
		}
	}
}

inline void tlm_serial_payload::free_auto_extensions()
{
	typename std::vector<tlm_extension_slot>::size_type num_extensions = extension_slots.size();
	typename std::vector<tlm_extension_slot>::size_type extension_id;

	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		tlm_extension_slot& extension_slot = extension_slots[extension_id];
		
		if(!extension_slot.sticky && extension_slot.extension)
		{
			extension_slot.extension->free();
			extension_slot.extension = 0;
		}
	}
}

template <typename T> T *tlm_serial_payload::set_extension(T *extension)
{
	return static_cast<T *>(set_extension(T::ID, extension));
}

template <typename T> T *tlm_serial_payload::set_auto_extension(T *extension)
{
	return static_cast<T *>(set_auto_extension(T::ID, extension));
}

template <typename T> void tlm_serial_payload::get_extension(T *& p_extension) const
{
	p_extension = get_extension<T>();
}

template <typename T> T *tlm_serial_payload::get_extension() const
{
	return static_cast<T *>(get_extension(T::ID));
}

template <typename T> void tlm_serial_payload::clear_extension(const T *extension)
{
	extension_slots[T::ID].extension = 0;
}

template <typename T> void tlm_serial_payload::clear_extension()
{
	extension_slots[T::ID].extension = 0;
}

template <typename T> void tlm_serial_payload::release_extension(T *)
{
	release_extension<T>();
}

template <typename T> void tlm_serial_payload::release_extension()
{
	release_extension(T::ID);
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif
