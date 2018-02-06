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

#include <unisim/util/likely/likely.hh>
#include <tlm>
#include <iostream>
#include <stdexcept>
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

class tlm_serial_extension_base
{
public:
	virtual tlm_serial_extension_base *clone() const = 0;
	virtual void free() { delete this; }
	virtual void copy_from(tlm_serial_extension_base const &) = 0;
protected:
	virtual ~tlm_serial_extension_base() {}
public:
	static inline unsigned int num_extensions(bool allocate = false);
};

template <typename T>
class tlm_serial_extension : public tlm_serial_extension_base
{
public:
	virtual tlm_serial_extension_base* clone() const = 0;
	virtual void copy_from(tlm_serial_extension_base const &) = 0;
	virtual ~tlm_serial_extension() {}
	const static unsigned int ID;
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
	inline std::vector<bool>& get_data() { return data; }
	inline const std::vector<bool>& get_data() const { return data; }
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
	inline tlm_serial_extension_base *set_extension(unsigned int, tlm_serial_extension_base *);
	template <typename T> T *set_auto_extension(T *);
	inline tlm_serial_extension_base *set_auto_extension(unsigned int, tlm_serial_extension_base *);
	template <typename T> void get_extension(T *&) const;
	template <typename T> T* get_extension() const;
	inline tlm_serial_extension_base *get_extension(unsigned int) const;
	template <typename T> void clear_extension(const T *);
	template <typename T> void clear_extension();
	template <typename T> void release_extension(T *);
	template <typename T> void release_extension();
	inline void resize_extensions();
private:
	std::vector<bool> data;
	sc_core::sc_time period;
	tlm_serial_mm_interface *mm;
	unsigned int ref_count;
	
	struct tlm_serial_extension_slot
	{
		inline tlm_serial_extension_slot();
		
		tlm_serial_extension_base *extension;
		bool sticky;
	};
	
	std::vector<tlm_serial_extension_slot> extension_slots;
	
	inline void free_auto_extensions();
};

enum tlm_bitstream_sync_status
{
	TLM_BITSTREAM_SYNC_OK    = 0,
	TLM_BITSTREAM_NEED_SYNC  = 1,
};

class tlm_bitstream
{
public:
	tlm_bitstream(bool init_value = true)
		: curr_value(init_value)
		, miss_count(0)
		, status(TLM_BITSTREAM_NEED_SYNC)
		, curr_time_stamp(sc_core::SC_ZERO_TIME)
		, curr_period(sc_core::SC_ZERO_TIME)
		, timed_serial_payloads()
		, free_timed_serial_payloads()
		, next_bit_event(sc_core::sc_gen_unique_name("next_bit_event"))
		, mm()
	{
	}
	
	virtual ~tlm_bitstream()
	{
		std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();
		
		while(it != timed_serial_payloads.end())
		{
			std::map<sc_core::sc_time, timed_serial_payload *>::iterator next_it = it;
			next_it++;
			
			timed_serial_payload *tsp = (*it).second;
			
			delete tsp;
			
			timed_serial_payloads.erase(it);
			
			it = next_it;
		}
		
		std::vector<timed_serial_payload *>::size_type num_free_timed_serial_payloads = free_timed_serial_payloads.size();
		std::vector<timed_serial_payload *>::size_type i;
		for(i = 0; i < num_free_timed_serial_payloads; i++)
		{
			timed_serial_payload *tsp = free_timed_serial_payloads[i];
			delete tsp;
		}
	}
	
	sc_core::sc_event& event() { return next_bit_event; }
	
	void fill(tlm_serial_payload& payload)
	{
		if(payload.get_data().empty()) return;
		
		sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
		
		// check time stamp validity (i.e. not in the past or present)
		assert(time_stamp > curr_time_stamp);
		
		// check if there's no conflicts with previously inserted payloads
		assert(!conflicts(time_stamp, payload));
		
		tlm_serial_payload *p = 0;
		
		if(payload.has_mm()) // payload has memory manager ?
		{
			p = &payload;
		}
		else
		{
			// payload has no memory manager
			p = mm.allocate();
			p->deep_copy_from(payload);
			p->set_mm(&mm);
		}
		
		// insert a new timed serial payload
		timed_serial_payload *new_tsp = allocate_timed_serial_payload(time_stamp, p);
		timed_serial_payloads.insert(std::pair<sc_core::sc_time, timed_serial_payload *>(time_stamp, new_tsp));

		// value may change, until trigger
		std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();
		timed_serial_payload *tsp = (*it).second;
		tlm_serial_payload *serial_payload = tsp->serial_payload;
		const std::vector<bool>& serial_data = serial_payload->get_data();
		curr_value = serial_data[tsp->bit_offset];
		curr_time_stamp = tsp->bit_time_stamp;
		curr_period = serial_payload->get_period();
		
		notify(tsp);
	}
	
	bool read() const { return curr_value; }
	
	unsigned int get_miss_count() const { return miss_count; }
	
	const sc_core::sc_time& get_time_stamp() const { return curr_time_stamp; }
	
	const sc_core::sc_time& get_period() const { return curr_period; }
	
	tlm_bitstream_sync_status seek(const sc_core::sc_time& t)
	{
		sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
		time_stamp += t;
		
		// check time stamp validity (i.e. not in the past)
		assert(time_stamp >= curr_time_stamp);
		
		// search a payload matching requested time while droping old payloads
		std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();
		
		while(it != timed_serial_payloads.end())
		{
			timed_serial_payload *tsp = (*it).second;
		
			if(likely(tsp->next_time_stamp > time_stamp))
			{
				// found a payload which ends after requested time stamp
				timed_serial_payload *tsp = (*it).second;
				
				if(time_stamp >= tsp->bit_time_stamp)
				{
					if(time_stamp > tsp->bit_time_stamp)
					{
						// payload match requested time stamp
						tlm_serial_payload *serial_payload = tsp->serial_payload;
						
						const std::vector<bool>& serial_data = serial_payload->get_data();
						std::vector<bool>::size_type serial_data_length = serial_data.size();
						
						unsigned int num_sampled_values = 0;
						do
						{
							tsp->bit_offset++;
							tsp->bit_time_stamp += serial_payload->get_period();
							assert(tsp->bit_offset < serial_data_length);
							curr_value = serial_data[tsp->bit_offset];
							curr_time_stamp = tsp->bit_time_stamp;
							curr_period = serial_payload->get_period();
							num_sampled_values++;
						}
						while(time_stamp > tsp->bit_time_stamp);
						
						miss_count = num_sampled_values - 1;
					}
					else
					{
						assert(time_stamp == curr_time_stamp);
					}
					
					status = TLM_BITSTREAM_SYNC_OK;
				}
				else
				{
					notify(tsp);
					status = TLM_BITSTREAM_NEED_SYNC; // need wait or next_trigger
				}
				
				return status;
			}

			// drop old payload
			std::map<sc_core::sc_time, timed_serial_payload *>::iterator next_it = it;
			next_it++;
			free_timed_serial_payload(tsp);
			timed_serial_payloads.erase(it);
			
			it = next_it;
		}
		
		status = TLM_BITSTREAM_NEED_SYNC; // need wait or next_trigger

		return status;
	}
	
	tlm_bitstream_sync_status next()
	{
		miss_count = 0;

		std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();

		if(it != timed_serial_payloads.end())
		{
			// there's a current payload
			timed_serial_payload *tsp = (*it).second;
			tlm_serial_payload *serial_payload = tsp->serial_payload;
			
			const std::vector<bool>& serial_data = serial_payload->get_data();
			std::vector<bool>::size_type serial_data_length = serial_data.size();
			tsp->bit_offset++;
			tsp->bit_time_stamp += serial_payload->get_period();
			tsp->started = true;
			
			if(tsp->bit_offset >= serial_data_length) // end of payload ?
			{
				// payload is no longer needed
				delete tsp;
				timed_serial_payloads.erase(it);
				
				// search for next payload
				it = timed_serial_payloads.begin();
				if(it == timed_serial_payloads.end())
				{
					// no payload available: a fill is needed
					status = TLM_BITSTREAM_NEED_SYNC; // need wait or next_trigger
				}
				else
				{
					tsp = (*it).second;
					tlm_serial_payload *next_serial_payload = tsp->serial_payload;
					const std::vector<bool>& next_serial_data = next_serial_payload->get_data();
					curr_value = next_serial_data[tsp->bit_offset];
					curr_time_stamp = tsp->bit_time_stamp;
					curr_period = next_serial_payload->get_period();
					status = TLM_BITSTREAM_SYNC_OK;
				}
			}
			else
			{
				curr_value = serial_data[tsp->bit_offset];
				curr_time_stamp = tsp->bit_time_stamp;
				curr_period = serial_payload->get_period();
				status = TLM_BITSTREAM_SYNC_OK;
			}
		}
		else
		{
			// no payload: need a fill
			status = TLM_BITSTREAM_NEED_SYNC; // need wait or next_trigger
		}
		
		return status;
	}
	
	void drop_before(const sc_core::sc_time& t = sc_core::SC_ZERO_TIME)
	{
		sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
		time_stamp += t;
		
		std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();
		
		while(it != timed_serial_payloads.end())
		{
			std::map<sc_core::sc_time, timed_serial_payload *>::iterator next_it = it;
			next_it++;
			
			timed_serial_payload *tsp = (*it).second;
			
			if(likely(tsp->next_time_stamp > time_stamp)) break;

			free_timed_serial_payload(tsp);
			timed_serial_payloads.erase(it);
			
			it = next_it;
		}
	}
	
private:
	struct memory_manager : public tlm_serial_mm_interface
	{
		virtual ~memory_manager()
		{
			std::vector<tlm_serial_payload *>::size_type num_auto_serial_payloads = auto_serial_payloads.size();
			std::vector<tlm_serial_payload *>::size_type i;
			
			for(i = 0; i < num_auto_serial_payloads; i++)
			{
				tlm_serial_payload *serial_payload = auto_serial_payloads[i];
				delete serial_payload;
			}
		}
		
		virtual void free(tlm_serial_payload *serial_payload)
		{
			free_serial_payloads.push_back(serial_payload);
		}
		
		tlm_serial_payload *allocate()
		{
			tlm_serial_payload *serial_payload = 0;
			
			std::vector<tlm_serial_payload *>::size_type num_free_serial_payloads = free_serial_payloads.size();
			
			if(num_free_serial_payloads)
			{
				serial_payload = free_serial_payloads[num_free_serial_payloads - 1];
				free_serial_payloads.resize(num_free_serial_payloads - 1);
			}
			else
			{
				serial_payload = new tlm_serial_payload();
				auto_serial_payloads.push_back(serial_payload);
			}
			
			return serial_payload;
		}
		
		std::vector<tlm_serial_payload *> free_serial_payloads;
		std::vector<tlm_serial_payload *> auto_serial_payloads;
	};
	
	struct timed_serial_payload
	{
		timed_serial_payload(const sc_core::sc_time& _time_stamp, tlm_serial_payload *_serial_payload)
			: started(false)
			, time_stamp()
			, next_time_stamp()
			, serial_payload(0)
			, bit_offset(0)
			, bit_time_stamp()
		{
			initialize(_time_stamp, _serial_payload);
		}
		
		void initialize(const sc_core::sc_time& _time_stamp, tlm_serial_payload *_serial_payload)
		{
			started = false;
			time_stamp = _time_stamp;
			serial_payload = _serial_payload;
			bit_offset = 0;
			bit_time_stamp = time_stamp;
			next_time_stamp = serial_payload->get_period();
			next_time_stamp *= serial_payload->get_data().size();
			next_time_stamp += time_stamp;
			
			serial_payload->acquire();
		}
		
		void clear()
		{
			if(serial_payload) serial_payload->release();

			started = false;
			time_stamp = sc_core::SC_ZERO_TIME;
			serial_payload = 0;
			bit_offset = 0;
			bit_time_stamp = sc_core::SC_ZERO_TIME;
			next_time_stamp = sc_core::SC_ZERO_TIME;
		}
		
		~timed_serial_payload()
		{
			clear();
		}
		
		bool conflicts(const sc_core::sc_time& other_time_stamp, const tlm_serial_payload& other_serial_payload) const
		{
			sc_core::sc_time other_next_time_stamp(other_serial_payload.get_period());
			other_next_time_stamp *= other_serial_payload.get_data().size();
			other_next_time_stamp += other_time_stamp;
			
			const sc_core::sc_time& a_lo = time_stamp;
			const sc_core::sc_time& a_hi = next_time_stamp;
			const sc_core::sc_time& b_lo = other_time_stamp;
			const sc_core::sc_time& b_hi = other_next_time_stamp;
			sc_core::sc_time max_lo = (a_lo > b_lo) ? a_lo : b_lo;
			sc_core::sc_time min_hi = (a_hi < b_hi) ? a_hi : b_hi;
			
			return max_lo < min_hi;
		}
		
		bool started;
		sc_core::sc_time time_stamp;
		sc_core::sc_time next_time_stamp;
		tlm_serial_payload *serial_payload;
		unsigned int bit_offset;
		sc_core::sc_time bit_time_stamp;
	};
	
	timed_serial_payload *allocate_timed_serial_payload(const sc_core::sc_time& time_stamp, tlm_serial_payload *serial_payload)
	{
		timed_serial_payload *tsp = 0;
		
		if(free_timed_serial_payloads.empty())
		{
			tsp = new timed_serial_payload(time_stamp, serial_payload);
		}
		else
		{
			tsp = free_timed_serial_payloads.back();
			free_timed_serial_payloads.resize(free_timed_serial_payloads.size() - 1);
			tsp->initialize(time_stamp, serial_payload);
		}
		
		return tsp;
	}
	
	void free_timed_serial_payload(timed_serial_payload *tsp)
	{
		tsp->clear();
		free_timed_serial_payloads.push_back(tsp);
	}
	
	bool conflicts(const sc_core::sc_time& time_stamp, const tlm_serial_payload& serial_payload) const
	{
		std::map<sc_core::sc_time, timed_serial_payload *>::const_iterator it;
		for(it = timed_serial_payloads.begin(); it != timed_serial_payloads.end(); it++)
		{
			timed_serial_payload *tsp = (*it).second;
			if(tsp->conflicts(time_stamp, serial_payload)) return true;
		}
		
		return false;
	}
	
	void notify(timed_serial_payload *tsp)
	{
		// notify event
		sc_core::sc_time notify_delay(tsp->time_stamp);
		notify_delay -= sc_core::sc_time_stamp();
		next_bit_event.notify(notify_delay);
	}

	bool curr_value;
	unsigned int miss_count;
	tlm_bitstream_sync_status status;
	sc_core::sc_time curr_time_stamp;
	sc_core::sc_time curr_period;
	std::map<sc_core::sc_time, timed_serial_payload *> timed_serial_payloads;
	std::vector<timed_serial_payload *> free_timed_serial_payloads;
	sc_core::sc_event next_bit_event;
	memory_manager mm;
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
#if defined(SC_VERSION_MAJOR) && defined(SC_VERSION_MINOR) && defined(SC_VERSION_PATCH) && \
    ((SC_VERSION_MAJOR > 2) || ((SC_VERSION_MAJOR >= 2) && ((SC_VERSION_MINOR > 3) || ((SC_VERSION_MINOR >= 3) && (SC_VERSION_PATCH >= 2)))))
	virtual sc_core::sc_type_index get_protocol_types() const { return typeid(PROTOCOL); }
#endif
};

template <unsigned int BUSWIDTH = 1, typename PROTOCOL = tlm_default_serial_protocol, int N = 0, sc_core::sc_port_policy POL = sc_core::SC_ZERO_OR_MORE_BOUND>
class tlm_serial_bus_socket : public tlm::tlm_base_target_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>
{
public:
	tlm_serial_bus_socket() : tlm::tlm_base_target_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>(sc_core::sc_gen_unique_name("tlm_serial_bus_socket")) {}
	explicit tlm_serial_bus_socket(const char *name) : tlm::tlm_base_target_socket<BUSWIDTH, tlm_serial_fw_if, tlm_serial_bw_if, N, POL>(sc_core::sc_gen_unique_name(name)) {}
	virtual const char *kind() const { return "tlm_serial_bus_socket"; }
#if defined(SC_VERSION_MAJOR) && defined(SC_VERSION_MINOR) && defined(SC_VERSION_PATCH) && \
    ((SC_VERSION_MAJOR > 2) || ((SC_VERSION_MAJOR >= 2) && ((SC_VERSION_MINOR > 3) || ((SC_VERSION_MINOR >= 3) && (SC_VERSION_PATCH >= 2)))))
	virtual sc_core::sc_type_index get_protocol_types() const { return typeid(PROTOCOL); }
#endif
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
	typename std::vector<tlm_serial_extension_slot>::size_type num_extensions = other.extension_slots.size();
	
	if(num_extensions > extension_slots.size())
	{
		extension_slots.resize(num_extensions);
	}
	
	typename std::vector<tlm_serial_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_serial_extension_slot& orig_extension_slot = other.extension_slots[extension_id];
		
		if(orig_extension_slot.extension)
		{
			if(extension_slots[extension_id].extension)
			{
				extension_slots[extension_id].extension->copy_from(*orig_extension_slot.extension);
			}
			else
			{
				tlm_serial_extension_base *cloned_extension = other.extension_slots[extension_id].extension->clone();
				
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
	typename std::vector<tlm_serial_extension_slot>::size_type num_extensions = other.extension_slots.size();
	typename std::vector<tlm_serial_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_serial_extension_slot& orig_extension_slot = other.extension_slots[extension_id];
		
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
	typename std::vector<tlm_serial_extension_slot>::size_type num_extensions = extension_slots.size();
	typename std::vector<tlm_serial_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_serial_extension_slot& extension_slot = extension_slots[extension_id];
		
		if(extension_slot.extension)
		{
			extension_slot.extension->free();
			
			extension_slots[extension_id].extension = 0;
		}
	}
}

inline void tlm_serial_payload::free_auto_extensions()
{
	typename std::vector<tlm_serial_extension_slot>::size_type num_extensions = extension_slots.size();
	typename std::vector<tlm_serial_extension_slot>::size_type extension_id;

	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		tlm_serial_extension_slot& extension_slot = extension_slots[extension_id];
		
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

inline tlm_serial_extension_base *tlm_serial_payload::set_extension(unsigned int id, tlm_serial_extension_base *extension)
{
	tlm_serial_extension_base *old_extension = (id < extension_slots.size()) ? extension_slots[id].extension : 0;
	extension_slots[id].extension = extension;
	extension_slots[id].sticky = true;
	return old_extension;
}

inline tlm_serial_extension_base* tlm_serial_payload::set_auto_extension(unsigned int id, tlm_serial_extension_base *extension)
{
	if(!mm) throw std::runtime_error("call to tlm_serial_payload::set_auto_extension while payload has no memory management"); 
	tlm_serial_extension_base *old_extension = (id < extension_slots.size()) ? extension_slots[id].extension : 0;
	extension_slots[id].extension = extension;
	extension_slots[id].sticky = false;
	return old_extension;
}

inline tlm_serial_extension_base* tlm_serial_payload::get_extension(unsigned int id) const
{
	return (id < extension_slots.size()) ? extension_slots[id].extension : 0;
}

inline void tlm_serial_payload::resize_extensions()
{
	if(tlm_serial_extension_base::num_extensions() > extension_slots.size())
	{
		extension_slots.resize(tlm_serial_extension_base::num_extensions());
	}
}

inline unsigned int tlm_serial_extension_base::num_extensions(bool allocate)
{
	static unsigned int num_extensions = 0;
	if(allocate) num_extensions++;
	return num_extensions;
}

inline tlm_serial_payload::tlm_serial_extension_slot::tlm_serial_extension_slot()
	: extension(0)
	, sticky(false)
{
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif
