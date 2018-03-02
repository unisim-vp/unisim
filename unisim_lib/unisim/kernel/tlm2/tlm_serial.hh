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
#include <limits.h>

namespace unisim {
namespace kernel {
namespace tlm2 {

/////////////////////////////// forward declarations //////////////////////////

class tlm_serial_payload;
class tlm_serial_mm_interface;
class tlm_bitvector;
class tlm_serial_payload;
struct tlm_default_serial_protocol;
class tlm_serial_fw_if;
class tlm_serial_bw_if;
template <unsigned int BUSWIDTH, typename PROTOCOL, int N, sc_core::sc_port_policy POL> class tlm_serial_initiator_socket;
template <unsigned int BUSWIDTH, typename PROTOCOL, int N, sc_core::sc_port_policy POL> class tlm_serial_target_socket;
inline std::ostream& operator << (std::ostream& os, const tlm_bitvector& bv);

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

#if 0
struct tlm_spi_extension : tlm_serial_extension<tlm_spi_extension>
{
	tlm_spi_extension() : slave_select(0) {}
	tlm_spi_extension(sc_dt::uint64 _slave_select) : slave_select(_slave_select) {}

	sc_dt::uint64 get_slave_select() const { return slave_select; }

	virtual tlm_serial_extension_base* clone() const
	{
		tlm_spi_extension *t = new tlm_spi_extension(slave_select);
		return t;
	}

	virtual void copy_from(tlm_serial_extension_base const &ext)
	{
		slave_select = static_cast<tlm_spi_extension const &>(ext).slave_select;
	}

private:
	sc_dt::uint64 slave_select;
};
#endif

#if 0
class tlm_bitvector
{
public:
	typedef uint8_t word_type;
	typedef std::vector<word_type> storage_type;
	typedef storage_type::size_type size_type;

	tlm_bitvector()
		: length(0)
		, vec()
	{
	}
	
	~tlm_bitvector()
	{
	}
	
	void clear()
	{
		vec.clear();
		length = 0;
	}

	void push_back(bool v)
	{
		const unsigned int word_length = CHAR_BIT * sizeof(word_type);
		const unsigned int word_offset = length / word_length;
		const unsigned int bit_offset = length % word_length;
		const bool allocate = (bit_offset == 0);
		word_type e = allocate ? 0 : vec[word_offset];
		
		e |= v << bit_offset;
		
		if(allocate)
		{
			vec.push_back(e);
		}
		else
		{
			vec[word_offset] = e;
		}
		
		length++;
	}
	
	bool operator [] (size_type i) const
	{
		return get(i);
	}
	
	size_type size() const
	{
		return length;
	}
	
	bool empty() const
	{
		return (length == 0);
	}
	
	tlm_bitvector& operator = (const tlm_bitvector& bv)
	{
		length = bv.length;
		vec = bv.vec;
		return *this;
	}
	
private:
	friend std::ostream& operator << (std::ostream& os, const tlm_bitvector& bv);

	size_type length;
	storage_type vec;
	
	bool get(size_type i) const
	{
		const unsigned int word_length = CHAR_BIT * sizeof(word_type);
		const unsigned int word_offset = i / word_length;
		const unsigned int bit_offset = i % word_length;
		return ((vec[word_offset] >> bit_offset) & 1) != 0;
	}

	void print(std::ostream& os) const
	{
		size_type i;
		
		for(i = 0; i < length; i++)
		{
			if(i) os << " ";
			
			bool v = get(i);
			
			os << v;
		}
	}
};

inline std::ostream& operator << (std::ostream& os, const tlm_bitvector& bv)
{
	bv.print(os);
	return os;
}

#else

struct tlm_bitvector : std::vector<bool>
{
	using std::vector<bool>::operator =;
};

std::ostream& operator << (std::ostream& os, const tlm_bitvector& bv)
{
	typename tlm_bitvector::size_type length = bv.size();
	typename tlm_bitvector::size_type i;
	
	for(i = 0; i < length; i++)
	{
		if(i) os << " ";
		
		const bool& v = bv[i];
		
		os << v;
	}
	
	return os;
}
#endif

class tlm_serial_payload
{
public:
	typedef tlm_bitvector data_type;
	
	inline tlm_serial_payload();
	inline explicit tlm_serial_payload(tlm_serial_mm_interface *);
	inline virtual ~tlm_serial_payload();

private:
	// Disable copy constructor and assignment operator
	inline tlm_serial_payload(const tlm_serial_payload&);
	inline tlm_serial_payload& operator = (const tlm_serial_payload&);

public:
	// Access methods
	inline void set_data(const data_type& _data) { data = _data; }
	inline void set_period(const sc_core::sc_time& _period) { period = _period; }
	inline data_type& get_data() { return data; }
	inline const data_type& get_data() const { return data; }
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
	data_type data;
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

struct tlm_serial_memory_manager : public tlm_serial_mm_interface
{
	virtual ~tlm_serial_memory_manager()
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

enum tlm_input_bitstream_sync_status
{
	TLM_INPUT_BITSTREAM_SYNC_OK    = 0,
	TLM_INPUT_BITSTREAM_NEED_SYNC  = 1,
};

enum tlm_bitstream_bit_order
{
	TLM_BITSTREAM_LSB_FIRST = 0,
	TLM_BITSTREAM_MSB_FIRST = 1
};

class tlm_input_bitstream
{
public:
	inline tlm_input_bitstream(bool init_value = true);
	
	virtual inline ~tlm_input_bitstream();
	
	inline sc_core::sc_event& event();
	
	inline void fill(tlm_serial_payload& payload, const sc_core::sc_time& t = sc_core::SC_ZERO_TIME);
	
	inline void latch();
	
	inline bool read() const;
	
	inline bool posedge() const;
	
	inline bool negedge() const;

	inline unsigned int get_miss_count() const;
	
	inline const sc_core::sc_time& get_time_stamp() const;
	
	inline const sc_core::sc_time& get_period() const;
	
	inline tlm_input_bitstream_sync_status seek(const sc_core::sc_time& t = sc_core::SC_ZERO_TIME);
	
	inline tlm_input_bitstream_sync_status next();
	
	inline void drop_before(const sc_core::sc_time& t = sc_core::SC_ZERO_TIME);
	
	template <typename T>
	void inline sample(T& v, bool& parity_bit, sc_core::sc_time& t, const sc_core::sc_time& sampling_period, unsigned int length = CHAR_BIT * sizeof(T));
	
	template <typename T>
	inline void read(T& v, bool& parity_bit, unsigned int length = CHAR_BIT * sizeof(T));
	
private:
	struct timed_serial_payload
	{
		timed_serial_payload(const sc_core::sc_time& _time_stamp, tlm_serial_payload *_serial_payload)
			: time_stamp()
			, next_time_stamp()
			, serial_payload(0)
			, bit_offset(0)
			, bit_time_stamp()
		{
			initialize(_time_stamp, _serial_payload);
		}
		
		void initialize(const sc_core::sc_time& _time_stamp, tlm_serial_payload *_serial_payload)
		{
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
		
		sc_core::sc_time time_stamp;
		sc_core::sc_time next_time_stamp;
		tlm_serial_payload *serial_payload;
		unsigned int bit_offset;
		sc_core::sc_time bit_time_stamp;
	};
	
	timed_serial_payload *allocate_timed_serial_payload(const sc_core::sc_time& time_stamp, tlm_serial_payload *serial_payload);
	void free_timed_serial_payload(timed_serial_payload *tsp);
	bool conflicts(const sc_core::sc_time& time_stamp, const tlm_serial_payload& serial_payload) const;
	void notify(timed_serial_payload *tsp);

	bool prev_value;
	bool curr_value;
	unsigned int num_sampled_values;
	unsigned int miss_count;
	tlm_input_bitstream_sync_status status;
	sc_core::sc_time curr_time_stamp;
	sc_core::sc_time curr_period;
	std::map<sc_core::sc_time, timed_serial_payload *> timed_serial_payloads;
	std::vector<timed_serial_payload *> free_timed_serial_payloads;
	sc_core::sc_event next_bit_event;
	tlm_serial_memory_manager mm;
	tlm_bitstream_bit_order bit_order;
};

inline tlm_input_bitstream::tlm_input_bitstream(bool init_value)
	: prev_value(init_value)
	, curr_value(init_value)
	, num_sampled_values(0)
	, miss_count(0)
	, status(TLM_INPUT_BITSTREAM_NEED_SYNC)
	, curr_time_stamp(sc_core::SC_ZERO_TIME)
	, curr_period(sc_core::SC_ZERO_TIME)
	, timed_serial_payloads()
	, free_timed_serial_payloads()
	, next_bit_event(sc_core::sc_gen_unique_name("next_bit_event"))
	, mm()
{
}

inline tlm_input_bitstream::~tlm_input_bitstream()
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

inline sc_core::sc_event& tlm_input_bitstream::event() { return next_bit_event; }

inline void tlm_input_bitstream::fill(tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	if(payload.get_data().empty()) return;
	
	sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
	time_stamp += t;
	
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

#if 0
	// value may change, until trigger
	std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();
	timed_serial_payload *tsp = (*it).second;
	tlm_serial_payload *serial_payload = tsp->serial_payload;
	const tlm_serial_payload::data_type& serial_data = serial_payload->get_data();
	prev_value = curr_value;
	curr_value = serial_data[tsp->bit_offset];
	curr_time_stamp = tsp->bit_time_stamp;
	curr_period = serial_payload->get_period();
#else
	std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();
	timed_serial_payload *tsp = (*it).second;
#endif
	
	notify(tsp);
}

inline void tlm_input_bitstream::latch()
{
	std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();
	
	if(it != timed_serial_payloads.end())
	{
		timed_serial_payload *tsp = (*it).second;
		tlm_serial_payload *serial_payload = tsp->serial_payload;
		const tlm_serial_payload::data_type& serial_data = serial_payload->get_data();
		prev_value = curr_value;
		curr_value = serial_data[tsp->bit_offset];
		curr_time_stamp = tsp->bit_time_stamp;
		curr_period = serial_payload->get_period();
	}
}

inline bool tlm_input_bitstream::read() const { return curr_value; }

inline bool tlm_input_bitstream::posedge() const { return !prev_value && curr_value; }

inline bool tlm_input_bitstream::negedge() const { return prev_value && !curr_value; }

inline unsigned int tlm_input_bitstream::get_miss_count() const { return miss_count; }

inline const sc_core::sc_time& tlm_input_bitstream::get_time_stamp() const { return curr_time_stamp; }

inline const sc_core::sc_time& tlm_input_bitstream::get_period() const { return curr_period; }

#if 0
inline tlm_input_bitstream_sync_status tlm_input_bitstream::seek(const sc_core::sc_time& t)
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
					
					const tlm_serial_payload::data_type& serial_data = serial_payload->get_data();
					tlm_serial_payload::data_type::size_type serial_data_length = serial_data.size();
					
					unsigned int num_sampled_values = 0;
					do
					{
						tsp->bit_offset++;
						tsp->bit_time_stamp += serial_payload->get_period();
						assert(tsp->bit_offset < serial_data_length);
						prev_value = curr_value;
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
				
				status = TLM_INPUT_BITSTREAM_SYNC_OK;
			}
			else
			{
				notify(tsp);
				status = TLM_INPUT_BITSTREAM_NEED_SYNC; // need wait or next_trigger
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
	
	status = TLM_INPUT_BITSTREAM_NEED_SYNC; // need wait or next_trigger

	return status;
}
#endif

inline tlm_input_bitstream_sync_status tlm_input_bitstream::seek(const sc_core::sc_time& t)
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
	
		tlm_serial_payload *serial_payload = tsp->serial_payload;
		
		const tlm_serial_payload::data_type& serial_data = serial_payload->get_data();
		tlm_serial_payload::data_type::size_type serial_data_length = serial_data.size();
		
		if(likely(tsp->next_time_stamp > time_stamp))
		{
			// found a payload which ends after requested time stamp
			if(time_stamp > tsp->bit_time_stamp)
			{
				// found a payload that match requested time stamp: eat bits until requested time stamp
				do
				{
					tsp->bit_offset++;
					tsp->bit_time_stamp += serial_payload->get_period();
					assert(tsp->bit_offset < serial_data_length);
					prev_value = curr_value;
					curr_value = serial_data[tsp->bit_offset];
					curr_time_stamp = tsp->bit_time_stamp;
					curr_period = serial_payload->get_period();
					num_sampled_values++;
				}
				while(time_stamp > tsp->bit_time_stamp);
				
				miss_count = num_sampled_values - 1;
				num_sampled_values = 0;
				
				status = TLM_INPUT_BITSTREAM_SYNC_OK;
			}
			else
			{
				notify(tsp);
				status = TLM_INPUT_BITSTREAM_NEED_SYNC; // need wait or next_trigger
			}
			
			return status;
		}
		else if(likely(tsp->bit_time_stamp < tsp->next_time_stamp))
		{
			// payload is old: eat all bits
			do
			{
				tsp->bit_offset++;
				tsp->bit_time_stamp += serial_payload->get_period();
				assert(tsp->bit_offset < serial_data_length);
				prev_value = curr_value;
				curr_value = serial_data[tsp->bit_offset];
				curr_time_stamp = tsp->bit_time_stamp;
				curr_period = serial_payload->get_period();
				num_sampled_values++;
			}
			while(tsp->bit_time_stamp < tsp->next_time_stamp);
		}

		// drop old payload
		std::map<sc_core::sc_time, timed_serial_payload *>::iterator next_it = it;
		next_it++;
		free_timed_serial_payload(tsp);
		timed_serial_payloads.erase(it);
		
		it = next_it;
	}
		
	status = TLM_INPUT_BITSTREAM_NEED_SYNC; // need wait or next_trigger

	return status;
}

inline tlm_input_bitstream_sync_status tlm_input_bitstream::next()
{
	miss_count = 0;

	std::map<sc_core::sc_time, timed_serial_payload *>::iterator it = timed_serial_payloads.begin();

	if(it != timed_serial_payloads.end())
	{
		// there's a current payload
		timed_serial_payload *tsp = (*it).second;
		tlm_serial_payload *serial_payload = tsp->serial_payload;
		
		const tlm_serial_payload::data_type& serial_data = serial_payload->get_data();
		tlm_serial_payload::data_type::size_type serial_data_length = serial_data.size();
		tsp->bit_offset++;
		tsp->bit_time_stamp += serial_payload->get_period();
		
		if(tsp->bit_offset >= serial_data_length) // end of payload ?
		{
			// keep a conservative time stamp of hypothetical next contiguous payload
			sc_core::sc_time next_time_stamp = tsp->bit_time_stamp;
			
			// payload is no longer needed
			delete tsp;
			timed_serial_payloads.erase(it);
			
			// search for next payload
			it = timed_serial_payloads.begin();
			if(it == timed_serial_payloads.end())
			{
				// no payload available: a fill is needed
				status = TLM_INPUT_BITSTREAM_NEED_SYNC; // need wait or next_trigger
			}
			else
			{
				tsp = (*it).second;
				tlm_serial_payload *next_serial_payload = tsp->serial_payload;
				const tlm_serial_payload::data_type& next_serial_data = next_serial_payload->get_data();
				if(tsp->bit_time_stamp > next_time_stamp)
				{
					// a payload is available but it's not contiguous with previous payload: a fill may be needed
					notify(tsp);
					status = TLM_INPUT_BITSTREAM_NEED_SYNC; // need wait or next_trigger
				}
				else
				{
					prev_value = curr_value;
					curr_value = next_serial_data[tsp->bit_offset];
					curr_time_stamp = tsp->bit_time_stamp;
					curr_period = next_serial_payload->get_period();
					status = TLM_INPUT_BITSTREAM_SYNC_OK;
				}
			}
		}
		else
		{
			prev_value = curr_value;
			curr_value = serial_data[tsp->bit_offset];
			curr_time_stamp = tsp->bit_time_stamp;
			curr_period = serial_payload->get_period();
			status = TLM_INPUT_BITSTREAM_SYNC_OK;
		}
	}
	else
	{
		// no payload: need a fill
		status = TLM_INPUT_BITSTREAM_NEED_SYNC; // need wait or next_trigger
	}
	
	return status;
}

inline void tlm_input_bitstream::drop_before(const sc_core::sc_time& t)
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

template <typename T>
inline void tlm_input_bitstream::sample(T& v, bool& parity_bit, sc_core::sc_time& t, const sc_core::sc_time& sampling_period, unsigned int length)
{
	T shift_reg = 0;
	
	if(length)
	{
		unsigned int n = length;
		
		do
		{
			if(seek(t) == TLM_INPUT_BITSTREAM_NEED_SYNC)
			{
				sc_core::wait(t);
				t = sc_core::SC_ZERO_TIME;
			}
			
			bool bit_value = read();
			
			shift_reg |= (bit_order == TLM_BITSTREAM_LSB_FIRST) ? bit_value : (1 << (length - 1));
			
			if(bit_order == TLM_BITSTREAM_LSB_FIRST)
			{
				shift_reg <<= 1;
			}
			else
			{
				shift_reg >>= 1;
			}
			
			parity_bit ^= bit_value;
			
			t += sampling_period;
		}
		while(--n);
	}
	
	v = shift_reg;
}

template <typename T>
inline void tlm_input_bitstream::read(T& v, bool& parity_bit, unsigned int length)
{
	T shift_reg = 0;
	
	if(length)
	{
		unsigned int n = length;
		
		do
		{
			if(next() == TLM_INPUT_BITSTREAM_NEED_SYNC)
			{
				sc_core::wait(event());
			}
			
			bool bit_value = read();
			
			shift_reg |= (bit_order == TLM_BITSTREAM_LSB_FIRST) ? bit_value : (1 << (length - 1));
			
			if(bit_order == TLM_BITSTREAM_LSB_FIRST)
			{
				shift_reg <<= 1;
			}
			else
			{
				shift_reg >>= 1;
			}
			
			parity_bit ^= bit_value;
		}
		while(--n);
	}
	
	v = shift_reg;
}

inline tlm_input_bitstream::timed_serial_payload *tlm_input_bitstream::allocate_timed_serial_payload(const sc_core::sc_time& time_stamp, tlm_serial_payload *serial_payload)
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

inline void tlm_input_bitstream::free_timed_serial_payload(timed_serial_payload *tsp)
{
	tsp->clear();
	free_timed_serial_payloads.push_back(tsp);
}

inline bool tlm_input_bitstream::conflicts(const sc_core::sc_time& time_stamp, const tlm_serial_payload& serial_payload) const
{
	std::map<sc_core::sc_time, timed_serial_payload *>::const_iterator it;
	for(it = timed_serial_payloads.begin(); it != timed_serial_payloads.end(); it++)
	{
		timed_serial_payload *tsp = (*it).second;
		if(tsp->conflicts(time_stamp, serial_payload)) return true;
	}
	
	return false;
}

inline void tlm_input_bitstream::notify(timed_serial_payload *tsp)
{
	// notify event
	sc_core::sc_time notify_delay(tsp->bit_time_stamp);
	notify_delay -= sc_core::sc_time_stamp();
	next_bit_event.notify(notify_delay);
}

class tlm_output_bitstream
{
public:
	tlm_output_bitstream(tlm_serial_payload& _serial_payload)
		: serial_payload(_serial_payload)
	{
	}
	
	void write(bool v)
	{
		tlm_serial_payload::data_type& data = serial_payload.get_data();
		data.push_back(v);
	}
	
	template <typename T>
	void write(const T& v, bool& parity_bit, unsigned int length = CHAR_BIT * sizeof(T))
	{
		if(length)
		{
			T shift_reg = v;
			unsigned int n = length;
			tlm_serial_payload::data_type& data = serial_payload.get_data();
			
			do
			{
				bool bit_value = (bit_order == TLM_BITSTREAM_MSB_FIRST) ? ((shift_reg >> (length - 1)) & 1) : (shift_reg & 1);
				
				data.push_back(bit_value);
				
				if(bit_order == TLM_BITSTREAM_MSB_FIRST)
				{
					shift_reg <<= 1;
				}
				else
				{
					shift_reg >>= 1;
				}
				
				parity_bit ^= bit_value;
			}
			while(--n);
		}
	}
	
private:
	tlm_serial_payload& serial_payload;
	tlm_bitstream_bit_order bit_order;
};

struct tlm_default_serial_protocol
{
};

class tlm_serial_fw_if : public virtual sc_core::sc_interface
{
public:
	virtual void nb_send(tlm_serial_payload&, const sc_core::sc_time& t = sc_core::SC_ZERO_TIME) = 0;
};

class tlm_serial_bw_if : public virtual sc_core::sc_interface
{
public:
	virtual void nb_receive(tlm_serial_payload&, const sc_core::sc_time& t = sc_core::SC_ZERO_TIME) = 0;
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
	
	void register_nb_receive(MODULE *mod, void (MODULE::*nb_receive_cb)(int, tlm_serial_payload&, const sc_core::sc_time& t), int id)
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
		
		virtual void nb_receive(tlm_serial_payload& payload, const sc_core::sc_time& t)
		{
			if(!mod || !nb_receive_cb) return;
			
			(mod->*nb_receive_cb)(id, payload, t);
		}
		
		MODULE *mod;
		int id;
		void (MODULE::*nb_receive_cb)(int, tlm_serial_payload&, const sc_core::sc_time&);
	};
	
	serial_bw_if_impl_s serial_bw_if_impl;
};

class tlm_simple_serial_bus
	: public unisim::kernel::service::Object
	, public sc_core::sc_module
	, public tlm_serial_fw_if
{
public:
	tlm_serial_bus_socket<> serial_socket;
	
	tlm_simple_serial_bus(const sc_core::sc_module_name& name, sc_core::sc_signal<bool>& _observable_signal, unisim::kernel::service::Object *parent = 0)
		: unisim::kernel::service::Object(name, parent)
		, serial_socket("serial_socket")
#if 0
		, enable_observer(false)
#endif
		, bitstream()
		, observable_signal(_observable_signal)
#if 0
		, param_enable_observer("enable-observer", this, enable_observer, "enable/disable observer")
#endif
	{
		serial_socket(*this);
		
		SC_HAS_PROCESS(tlm_simple_serial_bus);
		
		SC_METHOD(ObserverProcess);
		sensitive << bitstream.event();
	}
	
	virtual void nb_send(tlm_serial_payload& payload, const sc_core::sc_time& t)
	{
#if 0
		if(unlikely(enable_observer))
		{
#endif
			bitstream.fill(payload);
#if 0
		}
#endif
		
		int n = serial_socket.size();
		int i;
		for(i = 0; i < n; i++)
		{
			serial_socket[i]->nb_receive(payload, t);
		}
	}
	
	void ObserverProcess()
	{
#if 0
		if(likely(enable_observer))
		{
#endif
			bool value = bitstream.read();
			observable_signal.write(value);
				
			if(likely(bitstream.next() != TLM_INPUT_BITSTREAM_NEED_SYNC))
			{
				next_trigger(bitstream.get_period());
			}
#if 0
		}
#endif
	}
	
private:
#if 0
	bool enable_observer;
#endif
	tlm_input_bitstream bitstream;
	sc_core::sc_signal<bool>& observable_signal;
#if 0
	unisim::kernel::service::Parameter<bool> param_enable_observer;
#endif
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
