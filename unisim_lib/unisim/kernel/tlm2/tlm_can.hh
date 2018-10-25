/*
 *  Copyright (c) 2018,
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

#ifndef __UNISIM_KERNEL_TLM2_TLM_CAN_HH__
#define __UNISIM_KERNEL_TLM2_TLM_CAN_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm_serial.hh>
#include <unisim/util/random/random.hh>

namespace unisim {
namespace kernel {
namespace tlm2 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;

/////////////////////////////// forward declarations //////////////////////////

class tlm_can_global_quantum;
class tlm_can_quantumkeeper;
class tlm_can_value;
class tlm_can_bus;
class tlm_can_message;
template <typename CAN_NODE> class tlm_can_node;

/////////////////////////////////// declarations //////////////////////////////

// tlm_can_global_quantum

class tlm_can_global_quantum
{
public:
	static tlm_can_global_quantum& instance();
	virtual ~tlm_can_global_quantum();
	void set(const sc_core::sc_time& t);
	const sc_core::sc_time& get() const;
	sc_core::sc_time compute_local_quantum();
protected:
	tlm_can_global_quantum();
private:
	sc_core::sc_time global_quantum;
};

// tlm_can_quantumkeeper

class tlm_can_quantumkeeper
{
public:
	static inline void set_global_quantum(const sc_core::sc_time& t);
	static inline const sc_core::sc_time& get_global_quantum();
	
	inline tlm_can_quantumkeeper();
	inline void inc(const sc_core::sc_time& t);
	inline void set(const sc_core::sc_time& t);
	inline sc_core::sc_time get_current_time() const;
	inline const sc_core::sc_time& get_local_time() const;
	inline bool need_sync() const;
	inline void sync();
	inline void set_and_sync(const sc_core::sc_time& t);
	inline void reset();
	
private:
	sc_core::sc_time local_time_offset;
	sc_core::sc_time sync_deadline;
};

// tlm_can_value_enum

enum tlm_can_value_enum
{
	TLM_CAN_DOMINANT = false,
	TLM_CAN_RECESSIVE = true
};

// tlm_can_value

class tlm_can_value
{
public:
	inline tlm_can_value(tlm_can_value_enum v);
	inline tlm_can_value(bool v);
	inline operator tlm_can_value_enum() const;
	inline operator bool() const;
	friend std::ostream& operator << (std::ostream& os, const tlm_can_value& v);
private:
	tlm_can_value_enum value;
};

// tlm_can_bus

class tlm_can_bus
	: public unisim::kernel::service::Object
	, public sc_core::sc_module
{
public:
	typedef tlm_serial_bus_multi_passthrough_socket<tlm_can_bus> CAN_TX_type;
	typedef tlm_serial_bus_multi_passthrough_socket<tlm_can_bus> CAN_RX_type;
	
	CAN_TX_type CAN_TX;
	CAN_RX_type CAN_RX;
	
	inline tlm_can_bus(const sc_core::sc_module_name& name, sc_core::sc_signal<bool>& _observable_signal, const std::vector<sc_core::sc_signal<bool> *>& _observable_input_signals, unisim::kernel::service::Object *parent = 0);
	virtual inline ~tlm_can_bus();
private:
	virtual inline void end_of_elaboration();
	inline void can_tx_nb_send(int id, tlm_serial_payload& payload, const sc_core::sc_time& t);
	inline void can_rx_nb_send(int id, tlm_serial_payload& payload, const sc_core::sc_time& t);
	inline void broadcast(tlm_serial_payload& payload, const sc_core::sc_time& t);
	inline void process();
	inline void observable_input_signal_process(unsigned int id);
	inline void observable_signal_process();

	struct tlm_can_bus_observer
	{
		bool enable;
		
		inline tlm_can_bus_observer();
		inline void acquire();
		inline void release();
		
		static inline tlm_can_bus_observer *instance();
	private:
		unsigned int ref_count;
		unisim::kernel::service::Parameter<bool> param_enable;
	};

	unisim::kernel::logger::Logger logger;                                // logger
	sc_core::sc_time curr_time;                                           // current time
	unsigned int bitstream_count;                                         // number of bitstreams
	unsigned int bitstream_index;                                         // last scanned bitstream
	sc_core::sc_time min_next_time_stamp;                                 // overall minimal next bit time
	std::vector<tlm_input_bitstream *> bitstreams;                        // bitstreams from CAN_TX
	tlm_serial_payload can_payload;                                       // CAN payload over CAN_RX that is the combination of all incoming CAN payloads over CAN_TX
	sc_core::sc_time can_payload_time;                                    // CAN payload transmission time over CAN_RX
	sc_core::sc_event event;                                              // self notified delta event when one input is not ready
	std::vector<tlm_input_bitstream *> observable_input_signal_bitstream; // observable input signals bitstreams
	tlm_input_bitstream observable_signal_bitstream;                      // observable signal bitstream
	unsigned int delta_count;                                             // number of delta event self notification
	bool verbose;                                                         // enable/disable verbosity
	unisim::kernel::service::Parameter<bool> param_verbose;               // exposes verbose parameter to user
	bool output_value;                                                    // output value that is the combination (logical AND) of all input values
	sc_core::sc_signal<bool>& observable_signal;
	std::vector<sc_core::sc_signal<bool> *> observable_input_signals;
	double bit_inversion_error_probability;                               // probability of bit inversion error
	unisim::kernel::service::Parameter<double> param_bit_inversion_error_probability; // exposes probability of bit inversion error to user
	unisim::util::random::Random rnd;                                     // random number generator
	int32_t rnd_bit_inversion_error_threshold;                            // threshold to choose whether to generate a bit inversion error
};

// tlm_can_model_accuracy

enum tlm_can_model_accuracy
{
	TLM_CAN_CYCLE_ACCURATE                = 0, // cycle accurate
	TLM_CAN_TRANSACTION_ACCURATE          = 1, // transaction accurate: achieve cycle accuracy when no transmission errors occur
	TLM_CAN_TRANSMISSION_START_INACCURATE = 2  // transmission start inaccurate: if there's no pending transmission request just after intermission, transmission may be delayed by several cycles
};

// tlm_can_phase

enum tlm_can_phase
{
	TLM_CAN_IDLE_PHASE = 0,          // node is idle
	TLM_CAN_SOF_PHASE,               // transceiving start of frame
	TLM_CAN_ID_PHASE,                // transceiving identifier
	TLM_CAN_SRR_OR_RTR_PHASE,        // transceiving SRR or RTR
	TLM_CAN_IDE_OR_R1_PHASE,         // transceiving IDE or R1
	TLM_CAN_RTR_PHASE,               // transceiving remote transmission request
	TLM_CAN_R1_PHASE,                // transceiving reserved bit 1
	TLM_CAN_R0_PHASE,                // transceiving reserved bit 0
	TLM_CAN_DLC_PHASE,               // transceiving data length code
	TLM_CAN_DATA_PHASE,              // transceiving data
	TLM_CAN_CRC_PHASE,               // transceiving CRC
	TLM_CAN_CRC_DELIMITER_PHASE,     // transceiving CRC delimiter
	TLM_CAN_ACK_PHASE,               // transceiving ACK
	TLM_CAN_ACK_DELIMITER_PHASE,     // transceiving ACK delimiter
	TLM_CAN_EOF_PHASE,               // transceiving end of frame
	TLM_CAN_ERROR_FLAG_PHASE,        // transceiving error flag
	TLM_CAN_ERROR_DELIMITER_PHASE,   // transceiving error delimiter
	TLM_CAN_INTERMISSION_PHASE,      // transceiving intermission
	TLM_CAN_OVERLOAD_FLAG_PHASE,     // transceiving overload flag
	TLM_CAN_OVERLOAD_DELIMITER_PHASE // transceiving overload delimiter
};

// tlm_çan_format

enum tlm_can_format
{
	TLM_CAN_STD_FMT     = 0,
	TLM_CAN_XTD_FMT     = 1,
	TLM_CAN_FD_BASE_FMT = 2, // for future extension
	TLM_CAN_FD_XTD_FMT  = 3  // for future extension
};

// maximum data length in CAN data frame
static const unsigned int CAN_MAX_DATA_LENGTH = 64; // in bytes

// tlm_can_message

struct tlm_can_message
{
	inline tlm_can_message();
	inline tlm_can_message(tlm_can_format fmt, unsigned int id, bool rtr, bool brs, bool esi, unsigned int dlc, uint8_t *data);
	inline tlm_can_message(const tlm_can_message& msg);
	inline void clear();
	
	inline unsigned int get_data_length() const;
	static inline unsigned int encode_dlc(tlm_can_format fmt, unsigned int data_length);
	static inline bool valid_data_length(tlm_can_format fmt, unsigned int data_length);

	inline void set_format(tlm_can_format _fmt) { fmt = _fmt; }
	inline void set_identifier(unsigned int _id) { id = _id; }
	inline void set_remote_transmission_request(bool _rtr) { rtr = _rtr; }
	inline void set_bit_rate_switch(bool _brs) { brs = _brs; }
	inline void set_error_state_indicator(bool _esi) { esi = _esi; }
	inline void set_data_length_code(unsigned int _dlc) { dlc = _dlc; }
	
	inline tlm_can_format get_format() const { return fmt; }
	inline unsigned int get_identifier() const { return id; }
	inline bool get_remote_transmission_request() const { return rtr; }
	inline bool get_bit_rate_switch() const { return brs; }
	inline bool get_error_state_indicator() const { return esi; }
	inline unsigned int get_data_length_code() const { return dlc; }
	inline void set_data_byte_at(unsigned int index, uint8_t v) { data[index] = v; }
	inline uint8_t get_data_byte_at(unsigned int index) const { return data[index]; }
	
private:
	tlm_can_format fmt;
	unsigned int id;
	bool rtr;
	bool brs; // CAN FD only
	bool esi; // CAN FD only
	unsigned int dlc;
	uint8_t data[CAN_MAX_DATA_LENGTH];
	
	friend std::ostream& operator << (std::ostream& os, const tlm_can_message& msg);
};

// tlm_can_node_activity

enum tlm_can_node_activity
{
	TLM_CAN_NODE_ACTIVITY_SYNCHRONIZING = 0, // node is monitoring bus looking for transmitting/receiving a start of frame
	TLM_CAN_NODE_ACTIVITY_IDLE          = 1, // node is idle
	TLM_CAN_NODE_ACTIVITY_RECEIVER      = 2, // node is a receiver
	TLM_CAN_NODE_ACTIVITY_TRANSMITTER   = 3  // node is a transmitter
};

// tlm_can_error

enum tlm_can_error
{
	TLM_CAN_NO_ERROR     = 0, // no error
	TLM_CAN_STUFF_ERROR  = 1, // stuff error
	TLM_CAN_FORM_ERROR   = 2, // form error
	TLM_CAN_ACK_ERROR    = 3, // acknowledgment error
	TLM_CAN_BIT1_ERROR   = 4, // bit error (recessive bit transmitted, dominant bit received)
	TLM_CAN_BIT0_ERROR   = 5, // bit error (dominant bit transmitted, recessive bit received)
	TLM_CAN_CRC_ERROR    = 6  // CRC error
};

// tlm_can_state

enum tlm_can_state
{
	TLM_CAN_ERROR_ACTIVE_STATE  = 0, // node is 'error active'
	TLM_CAN_ERROR_PASSIVE_STATE = 1, // node is 'error passive'
	TLM_CAN_BUS_OFF_STATE       = 2  // node is 'bus off'
};

// tlm_can_node (CAN core)

template <typename CAN_NODE>
class tlm_can_node
	: public unisim::kernel::service::Object
	, public sc_core::sc_module
{
public:
	typedef tlm_serial_peripheral_socket_tagged<tlm_can_node> CAN_TX_type;
	typedef tlm_serial_peripheral_socket_tagged<tlm_can_node> CAN_RX_type;
	
	CAN_TX_type CAN_TX; // CAN RX interface
	CAN_RX_type CAN_RX; // CAN TX interface
	
	tlm_can_node(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent = 0);
	virtual ~tlm_can_node();
	void enable(bool v = true);
	
protected:
	mutable unisim::kernel::logger::Logger logger; // logger
	bool verbose;                                  // enable/disable verbosity
	
private:
	// interface methods that derived class shall provide
	const sc_core::sc_time& get_sample_point() const;            // returns SYNC_SEG + PROP_SEG + PHASE_SEG1
	const sc_core::sc_time& get_phase_seg2() const;              // returns PHASE_SEG2
	tlm_can_error get_can_error() const;                         // CAN error getter
	void set_can_error(tlm_can_error _can_error);                // CAN error setter
	tlm_can_node_activity get_node_activity() const;             // CAN node activity getter
	void set_node_activity(tlm_can_node_activity);               // CAN node activity setter
	tlm_can_phase get_phase() const;                             // CAN protocol phase getter
	void set_phase(tlm_can_phase _phase);                        // CAN protocol phase setter
	uint8_t get_transmit_error_count() const;                    // TRANSMIT ERROR COUNT getter
	uint8_t get_receive_error_count() const;                     // RECEIVE ERROR COUNT getter
	tlm_can_state get_state() const;                             // CAN node state getter
	void set_transmit_error_count(uint8_t);                      // TRANSMIT ERROR COUNT setter
	void set_receive_error_count(uint8_t);                       // RECEIVE ERROR COUNT setter
	void set_state(tlm_can_state);                               // CAN node state setter
	bool has_pending_transmission_request() const;               // returns whether there's a pending transmission request (i.e. there's a message to transmit or Tx FIFO not empty)
	const tlm_can_message& pending_transmission_request() const; // returns pending transmission request (the message with the highest priority or Tx FIFO front element)
	void transmission_occurred(const tlm_can_message&);          // called whenever a CAN message has been successfully transmitted
	void receive_message(const tlm_can_message&);                // called whenever a CAN message has been successfully received
	void received_bit(bool);                                     // called each time a bit is received when node is not idle
	bool is_loopback_enabled() const;                            // whether loopback is enabled (CAN_RX disconnected from CAN bus, CAN_TX connected to CAN_RX)
	
	// trampoline to the derived class public interface methods
	const sc_core::sc_time& __get_sample_point() const;
	const sc_core::sc_time& __get_phase_seg2() const;
	tlm_can_error __get_can_error() const;
	void __set_can_error(tlm_can_error _can_error);
	tlm_can_node_activity __get_node_activity() const;
	uint8_t __get_transmit_error_count() const;
	uint8_t __get_receive_error_count() const;
	tlm_can_state __get_state() const;
	void __set_node_activity(tlm_can_node_activity _node_activity);
	tlm_can_phase __get_phase() const;
	void __set_phase(tlm_can_phase _phase);
	void __set_transmit_error_count(uint8_t v);
	void __set_receive_error_count(uint8_t v);
	void __set_state(tlm_can_state _state);
	bool __has_pending_transmission_request() const;
	const tlm_can_message& __pending_transmission_request() const;
	void __transmission_occurred(const tlm_can_message& msg);
	void __receive_message(const tlm_can_message& msg);
	void __received_bit(bool value);
	bool __is_loopback_enabled() const;

	static void unimplemented(const char *method_decl);
	virtual void end_of_elaboration();
	void nb_receive(int id, tlm_serial_payload& payload, const sc_core::sc_time& t);
	uint16_t update_crc(uint16_t crc_rg, bool nxtbit);
	const sc_core::sc_time& feed(const sc_core::sc_time& t, bool value);
	void b_seek(int bit_index = -1);
	bool nb_read();
	bool b_read(int bit_index = -1);
	void inc_time(sc_core::sc_time& t, unsigned int cycle);
	void restart_stuffing(bool value);
	bool need_stuff(bool value);
	void stuff(bool value);
	void stuff(tlm_serial_payload::data_type& data, bool value);
	void restart_destuffing(bool value);
	bool need_destuff(bool value);
	void destuff(bool value);
	void inc_transmit_error_count(int delta);
	void inc_receive_error_count(int delta);
	void count_recessive_bits(bool value);
	void transceive_error_frame();
	void transceive_overload_frame();
	void transceive_intermission();
	void transceive_start_of_frame();
	void transceive_data_remote_frame();
	void process();

	// CAN protocol constants (should not be changed)
	static const unsigned int ID_LENGTH                                = 29;
	static const unsigned int STD_FMT_ID_LENGTH                        = 11;
	static const unsigned int DLC_LENGTH                               = 4;
	static const unsigned int INTERMISSION_LENGTH                      = 3;
	static const unsigned int SUSPEND_TRANSMISSION_LENGTH              = 8;
	static const unsigned int EOF_LENGTH                               = 7;
	static const uint16_t     POLYNOMIAL                               = 0x4599;
	static const uint16_t     CRC_INIT                                 = 0x0;
	static const unsigned int CRC_LENGTH                               = 15;
	static const unsigned int MAX_IDENTICAL_CONSECUTIVE_BITS           = 5;
	static const unsigned int ERROR_FLAG_LENGTH                        = 6;
	static const unsigned int ERROR_FLAG_DELIMITER_LENGTH              = 8;
	static const unsigned int OVERLOAD_FLAG_LENGTH                     = 6;
	static const unsigned int OVERLOAD_FLAG_DELIMITER_LENGTH           = 8;
	static const unsigned int BUS_OFF_ROUNDS = 128;
	
	// number of cycles while node can safely inject recessive bits (until last CRC bit) while bus is idle
	static const unsigned int INACCURATE_TRANSMISSION_START_MAX_CYCLES = 1 /* SOF */ + STD_FMT_ID_LENGTH + 1 /* RTR */ + 1 /* r0 */ + 1 /* r1 */ + DLC_LENGTH + CRC_LENGTH;
	static const unsigned int INACCURATE_TRANSMISSION_START_CYCLES = INACCURATE_TRANSMISSION_START_MAX_CYCLES;
	
	enum tlm_can_interface_enum
	{
		CAN_TX_IF, // identifier of CAN_TX interface
		CAN_RX_IF  // identifier of CAN_RX interface
	};

	sc_core::sc_time baud_period;          // baud period
	sc_core::sc_time sample_point;         // sample_point
	sc_core::sc_time phase_seg2;           // phase_seg2
	tlm_input_bitstream rx_bitstream;      // input bitstream over CAN_RX
	tlm_serial_payload payload;            // serial payload sent over CAN_TX
	unsigned int recessive_bit_count;      // number of recessive bits received while looking for a start-of-frame
	sc_core::sc_time last_feed;            // last time bus was feed (to handle temporal decoupling for a node that is not a transmitter)
	unsigned int stuff_count;              // counter for bit stuffing
	bool stuff_old_value;                  // old value transmitted compared against currently transmitted value to decide whether to stuff or not
	unsigned int destuff_count;            // counter for bit destuffing
	bool destuff_old_value;                // old value received compared against currently received value to decide whether to destuff or not
	tlm_can_model_accuracy model_accuracy; // model accuracy
	unsigned int bus_off_round_count;      // number of rounds where 11 consecutive recessive bits were monitored while looking for a start-of-frame
	bool enabled;                          // whether CAN node is enabled
	sc_core::sc_time t_transmit;           // absolute time when transmitting DOMINANT or RECESSIVE bits (or when they were supposed to be transmitted)
	tlm_can_quantumkeeper qk_sample_point; // quantum keeper that keeps local time offset for sampling point
	const tlm_can_message *send_msg;       // CAN message being transmitted
	tlm_can_message recv_msg;              // CAN message being received
	uint16_t send_crc;                     // CRC of CAN message being transmitted
	uint16_t recv_crc;                     // CRC in from CAN message being received
	uint16_t comp_crc;                     // computed CRC of CAN message being received
	bool hot_plugged;                      // whether CAN node was recently enabled
	bool transmitted_last_message;         // whether CAN node transmitted the last message
	unisim::kernel::service::Parameter<bool> param_verbose; // exposes verbose parameter to user
	unisim::kernel::service::Parameter<tlm_can_model_accuracy> param_model_accuracy; // exposes model accuracy parameter to user
};

/////////////////////////////////// definitions ///////////////////////////////

// tlm_can_global_quantum

inline tlm_can_global_quantum& tlm_can_global_quantum::instance()
{
	static tlm_can_global_quantum singleton;
	return singleton;
}

inline tlm_can_global_quantum::~tlm_can_global_quantum()
{
}

inline void tlm_can_global_quantum::set(const sc_core::sc_time& t)
{
	global_quantum = t;
}

inline const sc_core::sc_time& tlm_can_global_quantum::get() const
{
	return global_quantum;
}

inline sc_core::sc_time tlm_can_global_quantum::compute_local_quantum()
{
	if(global_quantum != sc_core::SC_ZERO_TIME)
	{
		sc_core::sc_time offset(sc_core::sc_time_stamp());
		offset %= global_quantum;
		sc_core::sc_time local_quantum(global_quantum);
		local_quantum -= offset;
		return local_quantum;
	}
	
	return sc_core::SC_ZERO_TIME;
}

inline tlm_can_global_quantum::tlm_can_global_quantum()
	: global_quantum(sc_core::SC_ZERO_TIME)
{
}

// tlm_can_quantumkeeper

inline void tlm_can_quantumkeeper::set_global_quantum(const sc_core::sc_time& t)
{
	tlm_can_global_quantum::instance().set(t);
}

inline const sc_core::sc_time& tlm_can_quantumkeeper::get_global_quantum()
{
	return tlm_can_global_quantum::instance().get();
}

inline tlm_can_quantumkeeper::tlm_can_quantumkeeper()
	: local_time_offset(sc_core::SC_ZERO_TIME)
	, sync_deadline(sc_core::SC_ZERO_TIME)
{
	reset();
}

inline void tlm_can_quantumkeeper::inc(const sc_core::sc_time& t)
{
	local_time_offset += t;
}

inline void tlm_can_quantumkeeper::set(const sc_core::sc_time& t)
{
	local_time_offset = t;
}

inline sc_core::sc_time tlm_can_quantumkeeper::get_current_time() const
{
	sc_core::sc_time current_time(sc_core::sc_time_stamp());
	current_time += local_time_offset;
	return current_time;
}

inline const sc_core::sc_time& tlm_can_quantumkeeper::get_local_time() const
{
	return local_time_offset;
}

inline bool tlm_can_quantumkeeper::need_sync() const
{
	sc_core::sc_time current_time(sc_core::sc_time_stamp());
	current_time += local_time_offset;
	
	return unlikely(current_time >= sync_deadline);
}

inline void tlm_can_quantumkeeper::sync()
{
	sc_core::wait(local_time_offset);
	reset();
}

inline void tlm_can_quantumkeeper::set_and_sync(const sc_core::sc_time& t)
{
	set(t);
	if(unlikely(need_sync())) sync();
}

inline void tlm_can_quantumkeeper::reset()
{
	local_time_offset = sc_core::SC_ZERO_TIME;
	sync_deadline = sc_core::sc_time_stamp();

	const sc_core::sc_time& global_quantum = get_global_quantum();
	
	if(likely(global_quantum != sc_core::SC_ZERO_TIME))
	{
		sc_core::sc_time offset(sc_core::sc_time_stamp());
		offset %= global_quantum;
		
		sync_deadline += global_quantum;
		sync_deadline -= offset;
	}
}

// tlm_can_value

inline tlm_can_value::tlm_can_value(tlm_can_value_enum v)
	: value(v)
{
}

inline tlm_can_value::tlm_can_value(bool v)
	: value(v ? TLM_CAN_RECESSIVE : TLM_CAN_DOMINANT)
{
}

inline tlm_can_value::operator tlm_can_value_enum() const
{
	return value;
}

inline tlm_can_value::operator bool() const
{
	return value != TLM_CAN_DOMINANT;
}

inline std::ostream& operator << (std::ostream& os, const tlm_can_value& value)
{
	return os << (value ? "RECESSIVE" : "DOMINANT");
}

// tlm_can_bus

inline tlm_can_bus::tlm_can_bus(const sc_core::sc_module_name& name, sc_core::sc_signal<bool>& _observable_signal, const std::vector<sc_core::sc_signal<bool> *>& _observable_input_signals, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, CAN_TX("CAN_TX")
	, CAN_RX("CAN_RX")
	, logger(*this)
	, curr_time()
	, bitstream_count(0)
	, bitstream_index(0)
	, min_next_time_stamp()
	, bitstreams()
	, can_payload()
	, can_payload_time()
	, event("event")
	, delta_count(0)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, output_value(true)
	, observable_signal(_observable_signal)
	, observable_input_signals(_observable_input_signals)
	, bit_inversion_error_probability(0.0)
	, param_bit_inversion_error_probability("bit-inversion-error-probability", this, bit_inversion_error_probability, "probability of bit inversion error")
	, rnd()
	, rnd_bit_inversion_error_threshold((bit_inversion_error_probability * (rnd.Amplitude / 2)) + 0.5)
{
	tlm_can_bus_observer::instance()->acquire();
	
	CAN_TX.register_nb_send(this, &tlm_can_bus::can_tx_nb_send);
	CAN_RX.register_nb_send(this, &tlm_can_bus::can_rx_nb_send);
}

inline tlm_can_bus::~tlm_can_bus()
{
	unsigned int i;
	
	for(i = 0; i < bitstreams.size(); i++)
	{
		delete bitstreams[i];
		if(tlm_can_bus_observer::instance()->enable) delete observable_input_signal_bitstream[i];
	}
	
	tlm_can_bus_observer::instance()->release();
}

inline void tlm_can_bus::end_of_elaboration()
{
	rnd.Seed(0x1234, 0x5678, 0x9012, 0x3456);
	sc_core::sc_spawn_options process_spawn_options;
	process_spawn_options.spawn_method();
	
	bitstream_count = CAN_TX.size();
	bitstreams.resize(bitstream_count);
	if(tlm_can_bus_observer::instance()->enable)
	{
		assert(observable_input_signals.size() == bitstream_count);
		observable_input_signal_bitstream.resize(bitstream_count);
	}
	bitstream_index = 0;
	min_next_time_stamp = sc_core::sc_max_time();
	output_value = true;
	
	unsigned int i;
	
	for(i = 0; i < bitstreams.size(); i++)
	{
		bitstreams[i] = new tlm_input_bitstream();
		process_spawn_options.set_sensitivity(&bitstreams[i]->event());
		
		if(unlikely(tlm_can_bus_observer::instance()->enable))
		{
			sc_core::sc_signal<bool> *observable_input_signal = (i < observable_input_signals.size()) ? observable_input_signals[i] : 0;
			
			if(observable_input_signal)
			{
				observable_input_signal_bitstream[i] = new tlm_input_bitstream();
				
				std::stringstream observable_input_signal_process_name_sstr;
				observable_input_signal_process_name_sstr << "observable_input_signal_process_" << i;
				sc_core::sc_spawn_options observable_input_signal_process_spawn_options;
				observable_input_signal_process_spawn_options.spawn_method();
				observable_input_signal_process_spawn_options.dont_initialize();
				observable_input_signal_process_spawn_options.set_sensitivity(&observable_input_signal_bitstream[i]->event());
				sc_core::sc_spawn(sc_bind(&tlm_can_bus::observable_input_signal_process, this, i), observable_input_signal_process_name_sstr.str().c_str(), &observable_input_signal_process_spawn_options);
			}
		}
	}
	
	sc_core::sc_spawn(sc_bind(&tlm_can_bus::process, this), "process", &process_spawn_options);
	
	if(tlm_can_bus_observer::instance()->enable)
	{
		sc_core::sc_spawn_options observable_signal_process_spawn_options;
		observable_signal_process_spawn_options.spawn_method();
		observable_signal_process_spawn_options.dont_initialize();
		observable_signal_process_spawn_options.set_sensitivity(&observable_signal_bitstream.event());
		sc_core::sc_spawn(sc_bind(&tlm_can_bus::observable_signal_process, this), "observable_signal_process", &observable_signal_process_spawn_options);
	}
}

inline void tlm_can_bus::can_tx_nb_send(int id, tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	sc_core::sc_time time_stamp = sc_core::sc_time_stamp();
	if(unlikely(verbose))
	{
		logger << DebugInfo << (time_stamp + t) << ":" << time_stamp << "[+" << t << "]:node #" << id << ":receiving [" << payload.get_data() << "] over CAN_TX and a period of " << payload.get_period() << EndDebugInfo;
	}
	bitstreams[id]->fill(payload, t);
	if(unlikely(tlm_can_bus_observer::instance()->enable))
	{
		observable_input_signal_bitstream[id]->fill(payload, t);
	}
	CAN_TX[id]->nb_receive(payload, t); // for loopback
}

inline void tlm_can_bus::can_rx_nb_send(int id, tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	sc_core::sc_time time_stamp = sc_core::sc_time_stamp();
	if(unlikely(verbose))
	{
		logger << DebugInfo << (time_stamp + t) << ":" << time_stamp << "[+" << t << "]:node #" << id << ":using CAN_RX::nb_send is forbidden" << EndDebugInfo;
	}
	sc_core::sc_stop();
}

inline void tlm_can_bus::broadcast(tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ":" << sc_core::sc_time_stamp() << "[+" << t << "]:broadcasting [" << payload.get_data() << "] over CAN_RX with a period of " << payload.get_period() << EndDebugInfo;
	}
	
	for(unsigned int i = 0; i < bitstream_count; i++)
	{
		CAN_RX[i]->nb_receive(payload, t);
	}
	
	if(unlikely(tlm_can_bus_observer::instance()->enable))
	{
		observable_signal_bitstream.fill(payload, t);
	}
}

inline void tlm_can_bus::process()
{
	sc_core::sc_time time_stamp = sc_core::sc_time_stamp();
	sc_core::sc_time time_offset = sc_core::SC_ZERO_TIME;
	assert(curr_time == time_stamp);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << (time_stamp + time_offset) << ":" << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:waking up" << EndDebugInfo;
	}
	
	do
	{
		do
		{
			tlm_input_bitstream *bitstream = bitstreams[bitstream_index];
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:#" << bitstream_index << ":seeking to " << (time_stamp + time_offset) << EndDebugInfo;
			}
			if(bitstream->seek(time_offset) == TLM_INPUT_BITSTREAM_SYNC_OK)
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:#" << bitstream_index << ":input ready" << EndDebugInfo;
				}
				
				const sc_core::sc_time& next_time_stamp = bitstream->get_next_time_stamp();
				if(next_time_stamp < min_next_time_stamp) min_next_time_stamp = next_time_stamp;
				
				bool input_value = bitstream->read();
				if(unlikely(verbose))
				{
					logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:#" << bitstream_index << ":input value = " << input_value << EndDebugInfo;
				}
				
				output_value = output_value && input_value;
			}
			else
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:#" << bitstream_index << ":need sync" << EndDebugInfo;
				}
				
				if(delta_count > 10)
				{
					logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:#" << bitstream_index << ":interlock" << EndDebugInfo;
					sc_core::sc_stop();
				}
				
				if(!can_payload.get_data().empty() && (delta_count < 1))
				{
					sc_core::sc_time can_payload_time_offset(can_payload_time);
					can_payload_time_offset -= time_stamp;
					
					broadcast(can_payload, can_payload_time_offset);
					
					can_payload.get_data().clear();
					can_payload_time = curr_time;
				}
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:synchronizing" << EndDebugInfo;
				}
				event.notify(time_offset);
				next_trigger(event);
				delta_count++;
				return;
			}
		}
		while(++bitstream_index < bitstream_count);

		if(unlikely(verbose))
		{
			logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:output value = " << output_value << EndDebugInfo;
		}

		delta_count = 0;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:next deadline at " << min_next_time_stamp << EndDebugInfo;
		}
		sc_core::sc_time consensus_period(min_next_time_stamp);
		consensus_period -= curr_time;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:consensus period is " << consensus_period << EndDebugInfo;
		}
		
		if(can_payload.get_period() != consensus_period)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:period has changed" << EndDebugInfo;
			}
			if(!can_payload.get_data().empty())
			{
				sc_core::sc_time can_payload_time_offset(can_payload_time);
				can_payload_time_offset -= time_stamp;
				
				broadcast(can_payload, can_payload_time_offset);
				
				can_payload.get_data().clear();
				can_payload_time = curr_time;
			}
			
			can_payload.set_period(consensus_period);
		}

		if(rnd_bit_inversion_error_threshold)
		{
			int32_t r = rnd.Generate();
			if((r >= -rnd_bit_inversion_error_threshold) && (r < rnd_bit_inversion_error_threshold))
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << (time_stamp + time_offset) << ":" << time_stamp << "[+" << time_offset << "]:#" << bitstream_index << ":inverting output" << EndDebugInfo;
				}
				output_value = !output_value;
			}
		}

		tlm_serial_payload::data_type& data = can_payload.get_data();
		data.push_back(output_value);
		
		time_offset += consensus_period;
		curr_time += consensus_period;
		bitstream_index = 0;
		min_next_time_stamp = sc_core::sc_max_time();
		output_value = true;
	}
	while(1);
}

inline void tlm_can_bus::observable_input_signal_process(unsigned int id)
{
	observable_input_signal_bitstream[id]->latch();
	
	bool value = observable_input_signal_bitstream[id]->read();
	observable_input_signals[id]->write(value);
		
	sc_core::sc_time notify_delay(observable_input_signal_bitstream[id]->get_next_time_stamp());
	
	if(likely(observable_input_signal_bitstream[id]->next() != TLM_INPUT_BITSTREAM_NEED_SYNC))
	{
		notify_delay -= sc_core::sc_time_stamp();
		next_trigger(notify_delay);
	}
}

inline void tlm_can_bus::observable_signal_process()
{
	observable_signal_bitstream.latch();

	bool value = observable_signal_bitstream.read();
	observable_signal.write(value);
	
	sc_core::sc_time notify_delay(observable_signal_bitstream.get_next_time_stamp());
	
	if(likely(observable_signal_bitstream.next() != TLM_INPUT_BITSTREAM_NEED_SYNC))
	{
		notify_delay -= sc_core::sc_time_stamp();
		next_trigger(notify_delay);
	}
}

inline tlm_can_bus::tlm_can_bus_observer::tlm_can_bus_observer()
	: enable(false)
	, ref_count(0)
	, param_enable("enable-tlm-can-bus-observer", 0, enable, "Enable/Disable TLM CAN bus observer")
{
}

inline void tlm_can_bus::tlm_can_bus_observer::acquire()
{
	ref_count++;
}

inline void tlm_can_bus::tlm_can_bus_observer::release()
{
	if(ref_count && (--ref_count == 0))
	{
		delete instance();
	}
}

inline tlm_can_bus::tlm_can_bus_observer *tlm_can_bus::tlm_can_bus_observer::instance()
{
	static tlm_can_bus_observer *inst = 0;
	if(unlikely(!inst))
	{
		inst = new tlm_can_bus_observer();
	}
	return inst;
}

// tlm_can_model_accuracy

inline std::ostream& operator << (std::ostream& os, const tlm_can_model_accuracy& model_accuracy)
{
	switch(model_accuracy)
	{
		case TLM_CAN_CYCLE_ACCURATE               : os << "cycle accurate"; break;
		case TLM_CAN_TRANSACTION_ACCURATE         : os << "transaction accurate"; break;
		case TLM_CAN_TRANSMISSION_START_INACCURATE: os << "transmission start inaccurate"; break;
		default                                   : os << "unknown"; break;
	}
	
	return os;
}

// tlm_can_phase

inline std::ostream& operator << (std::ostream& os, const tlm_can_phase& can_phase)
{
	switch(can_phase)
	{
		case TLM_CAN_IDLE_PHASE                : os << "idle"; break;
		case TLM_CAN_SOF_PHASE                 : os << "start of frame"; break;
		case TLM_CAN_ID_PHASE                  : os << "ID field"; break;
		case TLM_CAN_SRR_OR_RTR_PHASE          : os << "SRR or RTR bits"; break;
		case TLM_CAN_IDE_OR_R1_PHASE           : os << "IDE or r1 bits"; break;
		case TLM_CAN_RTR_PHASE                 : os << "RTR bit"; break;
		case TLM_CAN_R1_PHASE                  : os << "r1 bit"; break;
		case TLM_CAN_R0_PHASE                  : os << "r0 bit"; break;
		case TLM_CAN_DLC_PHASE                 : os << "data length code field"; break;
		case TLM_CAN_DATA_PHASE                : os << "data field"; break;
		case TLM_CAN_CRC_PHASE                 : os << "CRC field"; break;
		case TLM_CAN_CRC_DELIMITER_PHASE       : os << "CRC delimiter"; break;
		case TLM_CAN_ACK_PHASE                 : os << "ACK field"; break;
		case TLM_CAN_ACK_DELIMITER_PHASE       : os << "ACK delimiter"; break;
		case TLM_CAN_EOF_PHASE                 : os << "end of frame"; break;
		case TLM_CAN_ERROR_FLAG_PHASE          : os << "error flag"; break;
		case TLM_CAN_ERROR_DELIMITER_PHASE     : os << "error delimiter"; break;
		case TLM_CAN_INTERMISSION_PHASE        : os << "interframe space intermission field"; break;
		case TLM_CAN_OVERLOAD_FLAG_PHASE       : os << "overload flag field"; break;
		case TLM_CAN_OVERLOAD_DELIMITER_PHASE  : os << "overload delimiter"; break;
	}
	
	return os << " phase";
}

// tlm_can_format

inline std::ostream& operator << (std::ostream& os, const tlm_can_format& fmt)
{
	switch(fmt)
	{
		case TLM_CAN_STD_FMT    : os << "CAN standard format"; break;
		case TLM_CAN_XTD_FMT    : os << "CAN extended format"; break;
		case TLM_CAN_FD_BASE_FMT: os << "CAN FD base format"; break;
		case TLM_CAN_FD_XTD_FMT : os << "CAN FD extended format"; break;
		default                 : os << "CAN unknown format"; break;
	}
	return os;
}

// tlm_can_message

inline tlm_can_message::tlm_can_message()
	: fmt(TLM_CAN_STD_FMT)
	, id(0)
	, rtr(false)
	, brs(false)
	, esi(false)
	, dlc(0)
	, data()
{
}

inline tlm_can_message::tlm_can_message(tlm_can_format _fmt, unsigned int _id, bool _rtr, bool _brs, bool _esi, unsigned int _dlc, uint8_t *_data)
	: fmt(_fmt)
	, id(_id)
	, rtr(_rtr)
	, brs(_brs)
	, esi(_esi)
	, dlc(_dlc)
	, data()
{
	if(!rtr && dlc) memcpy(data, _data, get_data_length());
}

inline tlm_can_message::tlm_can_message(const tlm_can_message& msg)
	: fmt(msg.fmt)
	, id(msg.id)
	, rtr(msg.rtr)
	, brs(msg.brs)
	, esi(msg.esi)
	, dlc(msg.dlc)
	, data()
{
	if(!rtr && dlc) memcpy(data, msg.data, get_data_length());
}

inline void tlm_can_message::clear()
{
	memset(data, 0, get_data_length());
	fmt = TLM_CAN_STD_FMT;
	id = 0;
	rtr = false;
	brs = false;
	esi = false;
	dlc = 0;
}

inline unsigned int tlm_can_message::get_data_length() const
{
	switch(fmt)
	{
		case TLM_CAN_STD_FMT:
		case TLM_CAN_XTD_FMT:
		case TLM_CAN_FD_BASE_FMT:
			return (dlc <= 8) ? dlc : 8;
		case TLM_CAN_FD_XTD_FMT:
			if(dlc <= 8) return dlc;
			if(dlc <= 12) return 8 + (4 * (dlc - 8));
			if(dlc <= 15) return 16 + (16 * (dlc - 12));
			break;
	}
	
	return 0;
}

inline unsigned int tlm_can_message::encode_dlc(tlm_can_format fmt, unsigned int data_length)
{
	if(data_length <= 8) return data_length;
	if(fmt == TLM_CAN_FD_XTD_FMT)
	{
		if(data_length <= 24) return 8 + ((data_length - 8) / 4);
		if(data_length <= 64) return 12 + ((data_length - 16) / 16);
	}
	return 0;
}

inline bool tlm_can_message::valid_data_length(tlm_can_format fmt, unsigned int data_length)
{
	return (data_length <= 8) ||
	          ((fmt == TLM_CAN_FD_XTD_FMT) &&
	             ((data_length == 12) || (data_length == 16) || (data_length == 20) ||
	             (data_length == 24) || (data_length == 32) || (data_length == 48) ||
	             (data_length == 64)));
}

inline std::ostream& operator << (std::ostream& os, const tlm_can_message& msg)
{
	os << "tlm_can_message(format=" << msg.fmt;
	os << ",ID=" << msg.id;
	os << ",type=" << (msg.rtr ? "request" : "data") << " frame";
	if((msg.fmt == TLM_CAN_FD_BASE_FMT) || (msg.fmt == TLM_CAN_FD_XTD_FMT))
	{
		os << ",brs=" << msg.brs;
		os << ",esi=" << msg.esi;
	}
	os << ",dlc=" << msg.dlc;
	unsigned int data_length = msg.get_data_length();
	os << ",data length=" << data_length << " bytes";
	if(!msg.rtr)
	{
		os << ",data=";
		unsigned int i;
		os << "[";
		for(i = 0; i < data_length; i++)
		{
			if(i != 0) os << " ";
			os << "0x" << std::hex << (unsigned int) msg.data[i] << std::dec;
		}
		os << "]";
	}
	os << ")";
	return os;
}

// tlm_can_node_activity

inline std::ostream& operator << (std::ostream& os, const tlm_can_node_activity& node_activity)
{
	switch(node_activity)
	{
		case TLM_CAN_NODE_ACTIVITY_SYNCHRONIZING: os << "synchronizing"; break;
		case TLM_CAN_NODE_ACTIVITY_IDLE         : os << "idle"; break;
		case TLM_CAN_NODE_ACTIVITY_RECEIVER     : os << "receiver"; break;
		case TLM_CAN_NODE_ACTIVITY_TRANSMITTER  : os << "transmitter"; break;
		default                                 : os << "unknown"; break;
	}
	
	return os;
}

// tlm_can_error

inline std::ostream& operator << (std::ostream& os, const tlm_can_error& err)
{
	switch(err)
	{
		case TLM_CAN_NO_ERROR   : os << "not error"; break;
		case TLM_CAN_STUFF_ERROR: os << "stuff error"; break;
		case TLM_CAN_FORM_ERROR : os << "form error"; break;
		case TLM_CAN_ACK_ERROR  : os << "acknowledgment error"; break;
		case TLM_CAN_BIT1_ERROR : os << "bit1 error"; break;
		case TLM_CAN_BIT0_ERROR : os << "bit0 error"; break;
		case TLM_CAN_CRC_ERROR  : os << "CRC error"; break;
		default                 : os << "unknown error"; break;
	}
	
	return os;
}

// tlm_can_state

inline std::ostream& operator << (std::ostream& os, const tlm_can_state& state)
{
	switch(state)
	{
		case TLM_CAN_ERROR_ACTIVE_STATE : os << "error active"; break;
		case TLM_CAN_ERROR_PASSIVE_STATE: os << "error passive"; break;
		case TLM_CAN_BUS_OFF_STATE      : os << "bus off"; break;
		default                         : os << "unknown"; break;
	}
	
	return os << " state";
}

// tlm_can_node (CAN core)

template <typename CAN_NODE>
tlm_can_node<CAN_NODE>::tlm_can_node(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_module(name)
	, CAN_TX("CAN_TX")
	, CAN_RX("CAN_RX")
	, logger(*this)
	, verbose(false)
	, rx_bitstream()
	, payload()
	, recessive_bit_count(0)
	, last_feed()
	, stuff_count(0)
	, stuff_old_value(TLM_CAN_RECESSIVE)
	, destuff_count(0)
	, destuff_old_value(TLM_CAN_RECESSIVE)
	, model_accuracy(TLM_CAN_TRANSMISSION_START_INACCURATE)
	, bus_off_round_count(0)
	, enabled(false)
	, send_msg(0)
	, recv_msg()
	, send_crc(CRC_INIT)
	, recv_crc(0)
	, comp_crc(CRC_INIT)
	, hot_plugged(false)
	, transmitted_last_message(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, param_model_accuracy("model-accuracy", this, model_accuracy, "model accuracy (cycle-accurate, transaction-accurate, transmission-start-inaccurate)")
{
	CAN_TX.register_nb_receive(this, &tlm_can_node::nb_receive, CAN_TX_IF);
	CAN_RX.register_nb_receive(this, &tlm_can_node::nb_receive, CAN_RX_IF);
	
	SC_HAS_PROCESS(tlm_can_node);
	
	SC_THREAD(process);
}

template <typename CAN_NODE>
tlm_can_node<CAN_NODE>::~tlm_can_node()
{
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::enable(bool v)
{
	if(!enabled && v)
	{
		logger << DebugInfo << "enabling" << EndDebugInfo;
		hot_plugged = true;
		recessive_bit_count = 0;
		__set_phase(TLM_CAN_SOF_PHASE);
	}
	else if(enabled && !v)
	{
		logger << DebugInfo << "disabling" << EndDebugInfo;
	}
	enabled = v;
}

template <typename CAN_NODE>
const sc_core::sc_time& tlm_can_node<CAN_NODE>::get_sample_point() const
{
	unimplemented("const sc_core::sc_time& get_sample_point() const");
	return sc_core::SC_ZERO_TIME;
}

template <typename CAN_NODE>
const sc_core::sc_time& tlm_can_node<CAN_NODE>::get_phase_seg2() const
{
	unimplemented("const sc_core::sc_time& get_phase_seg2() const");
	return sc_core::SC_ZERO_TIME;
}

template <typename CAN_NODE>
tlm_can_error tlm_can_node<CAN_NODE>::get_can_error() const
{
	unimplemented("tlm_can_error get_can_error() const");
	return TLM_CAN_NO_ERROR;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::set_can_error(tlm_can_error _can_error)
{
	unimplemented("void set_can_error(tlm_can_error _can_error)");
}

template <typename CAN_NODE>
tlm_can_node_activity tlm_can_node<CAN_NODE>::get_node_activity() const
{
	unimplemented("tlm_can_node_activity get_node_activity() const");
	return TLM_CAN_NODE_ACTIVITY_IDLE;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::set_node_activity(tlm_can_node_activity)
{
	unimplemented("void set_node_activity(tlm_can_node_activity)");
}

template <typename CAN_NODE>
tlm_can_phase tlm_can_node<CAN_NODE>::get_phase() const
{
	unimplemented("tlm_can_phase get_phase() const");
	return TLM_CAN_IDLE_PHASE;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::set_phase(tlm_can_phase _phase)
{
	unimplemented("void set_phase(tlm_can_phase _phase)");
}

template <typename CAN_NODE>
uint8_t tlm_can_node<CAN_NODE>::get_transmit_error_count() const
{
	unimplemented("uint8_t get_transmit_error_count() const");
	return 0;
}

template <typename CAN_NODE>
uint8_t tlm_can_node<CAN_NODE>::get_receive_error_count() const
{
	unimplemented("uint8_t get_receive_error_count() const");
	return 0;
}

template <typename CAN_NODE>
tlm_can_state tlm_can_node<CAN_NODE>::get_state() const
{
	unimplemented("tlm_can_state get_state() const");
	return TLM_CAN_BUS_OFF_STATE;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::set_transmit_error_count(uint8_t)
{
	unimplemented("void set_transmit_error_count(uint8_t)");
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::set_receive_error_count(uint8_t)
{
	unimplemented("void set_receive_error_count(uint8_t)");
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::set_state(tlm_can_state)
{
	unimplemented("void set_state(tlm_can_state)");
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::has_pending_transmission_request() const
{
	unimplemented("bool has_pending_transmission_request() const");
	return false;
}

template <typename CAN_NODE>
const tlm_can_message& tlm_can_node<CAN_NODE>::pending_transmission_request() const
{
	unimplemented("const tlm_can_message& pending_transmission_request() const");
	static tlm_can_message msg;
	return msg;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::transmission_occurred(const tlm_can_message&)
{
	unimplemented("void transmission_occurred(const tlm_can_message&)");
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::receive_message(const tlm_can_message&)
{
	unimplemented("void receive_message(const tlm_can_message&)");
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::received_bit(bool)
{
	unimplemented("void received_bit(bool)");
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::is_loopback_enabled() const
{
	unimplemented("bool is_loopback_enabled() const");
	return false;
}

template <typename CAN_NODE>
const sc_core::sc_time& tlm_can_node<CAN_NODE>::__get_sample_point() const
{
	return static_cast<const CAN_NODE *>(this)->get_sample_point();
}

template <typename CAN_NODE>
const sc_core::sc_time& tlm_can_node<CAN_NODE>::__get_phase_seg2() const
{
	return static_cast<const CAN_NODE *>(this)->get_phase_seg2();
}

template <typename CAN_NODE>
tlm_can_error tlm_can_node<CAN_NODE>::__get_can_error() const
{
	return static_cast<const CAN_NODE *>(this)->get_can_error();
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__set_can_error(tlm_can_error _can_error)
{
	static_cast<CAN_NODE *>(this)->set_can_error(_can_error);
}

template <typename CAN_NODE>
tlm_can_node_activity tlm_can_node<CAN_NODE>::__get_node_activity() const
{
	return static_cast<const CAN_NODE *>(this)->get_node_activity();
}

template <typename CAN_NODE>
uint8_t tlm_can_node<CAN_NODE>::__get_transmit_error_count() const
{
	return static_cast<const CAN_NODE *>(this)->get_transmit_error_count();
}

template <typename CAN_NODE>
uint8_t tlm_can_node<CAN_NODE>::__get_receive_error_count() const
{
	return static_cast<const CAN_NODE *>(this)->get_receive_error_count();
}

template <typename CAN_NODE>
tlm_can_state tlm_can_node<CAN_NODE>::__get_state() const
{
	return static_cast<const CAN_NODE *>(this)->get_state();
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__set_node_activity(tlm_can_node_activity _node_activity)
{
	static_cast<CAN_NODE *>(this)->set_node_activity(_node_activity);
}

template <typename CAN_NODE>
tlm_can_phase tlm_can_node<CAN_NODE>::__get_phase() const
{
	return static_cast<const CAN_NODE *>(this)->get_phase();
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__set_phase(tlm_can_phase _phase)
{
	static_cast<CAN_NODE *>(this)->set_phase(_phase);
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__set_transmit_error_count(uint8_t v)
{
	static_cast<CAN_NODE *>(this)->set_transmit_error_count(v);
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__set_receive_error_count(uint8_t v)
{
	static_cast<CAN_NODE *>(this)->set_receive_error_count(v);
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__set_state(tlm_can_state _state)
{
	static_cast<CAN_NODE *>(this)->set_state(_state);
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::__has_pending_transmission_request() const
{
	return static_cast<const CAN_NODE *>(this)->has_pending_transmission_request();
}

template <typename CAN_NODE>
const tlm_can_message& tlm_can_node<CAN_NODE>::__pending_transmission_request() const
{
	return static_cast<const CAN_NODE *>(this)->pending_transmission_request();
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__transmission_occurred(const tlm_can_message& msg)
{
	static_cast<CAN_NODE *>(this)->transmission_occurred(msg);
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__receive_message(const tlm_can_message& msg)
{
	static_cast<CAN_NODE *>(this)->receive_message(msg);
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::__received_bit(bool value)
{
	static_cast<CAN_NODE *>(this)->received_bit(value);
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::__is_loopback_enabled() const
{
	return static_cast<const CAN_NODE *>(this)->is_loopback_enabled();
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::unimplemented(const char *method_decl)
{
	throw std::runtime_error(std::string("a tlm_can_node<> derived class shall implement '") + method_decl + "'");
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::end_of_elaboration()
{
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::nb_receive(int id, tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	bool loopback = __is_loopback_enabled();
	if(((id == CAN_RX_IF) && !loopback) ||
	   ((id == CAN_TX_IF) && loopback))
	{
		tlm_serial_payload::data_type& data = payload.get_data();
		const sc_core::sc_time& period = payload.get_period();
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << "[+" << t << "]:receiving [" << data << "] over CAN_RX with a period of " << period << EndDebugInfo;
		}
		
		rx_bitstream.fill(payload, t);
	}
}

template <typename CAN_NODE>
uint16_t tlm_can_node<CAN_NODE>::update_crc(uint16_t crc_rg, bool nxtbit)
{
	bool crcnxt = nxtbit ^ ((crc_rg >> (CRC_LENGTH - 1)) & 1);
	crc_rg = (crc_rg << 1) & ((1 << CRC_LENGTH) - 1);
	if(crcnxt)
	{
		crc_rg = crc_rg ^ POLYNOMIAL;
	}
	return crc_rg & ((1 << CRC_LENGTH) - 1);
}

template <typename CAN_NODE>
const sc_core::sc_time& tlm_can_node<CAN_NODE>::feed(const sc_core::sc_time& t, bool value)
{
	assert(model_accuracy != TLM_CAN_CYCLE_ACCURATE);
	if(t > last_feed)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << (sc_core::sc_time_stamp() + qk_sample_point.get_local_time() - sample_point) << ":" << sc_core::sc_time_stamp() << "[+" << (qk_sample_point.get_local_time() - sample_point) << "]:" << __get_node_activity() << ":" << __get_state() << ":feeding until " << t << " (excluded)" << EndDebugInfo;
		}
		sc_core::sc_time period(t);
		period -= last_feed;
		
		payload.set_period(period);
		tlm_serial_payload::data_type& data = payload.get_data();
		data.clear();
		data.push_back(value);
		CAN_TX->nb_send(payload, last_feed - sc_core::sc_time_stamp());
		last_feed = t;
	}
	else
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << (sc_core::sc_time_stamp() + qk_sample_point.get_local_time() - sample_point) << ":" << sc_core::sc_time_stamp() << "[+" << (qk_sample_point.get_local_time() - sample_point) << "]:" << __get_node_activity() << ":" << __get_state() << ":already fed until " << t << " (excluded)" << EndDebugInfo;
		}
	}
	return last_feed;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::b_seek(int bit_index)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":seeking to " << qk_sample_point.get_current_time() << " (" << __get_phase();
		if(bit_index >= 0) logger << ", bit #" << bit_index;
		logger << ")" << EndDebugInfo;
	}
	if((rx_bitstream.seek(qk_sample_point.get_local_time()) == TLM_INPUT_BITSTREAM_NEED_SYNC) || qk_sample_point.need_sync())
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":need sync (" << __get_phase();
			if(bit_index >= 0) logger << ", bit #" << bit_index;
			logger << ")" << EndDebugInfo;
		}
		qk_sample_point.sync();
		rx_bitstream.latch();
	}
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::nb_read()
{
	bool recv_value = rx_bitstream.read();
	__received_bit(recv_value);
	return recv_value;
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::b_read(int bit_index)
{
	b_seek(bit_index);
	bool recv_value = nb_read();
	return recv_value;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::inc_time(sc_core::sc_time& t, unsigned int cycle)
{
	if(cycle)
	{
		do
		{
			t += baud_period;
		}
		while(--cycle);
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::restart_stuffing(bool value)
{
	stuff_count = 1;
	stuff_old_value = value;
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::need_stuff(bool value)
{
	if(stuff_old_value == value)
	{
		if(++stuff_count >= MAX_IDENTICAL_CONSECUTIVE_BITS)
		{
			stuff_count = 1;
			return true;
		}
	}
	else
	{
		stuff_count = 1;
	}
	
	return false;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::stuff(bool value)
{
	if(need_stuff(value))
	{
		value = !value;
		if(unlikely(verbose))
		{
			logger << DebugInfo << __get_node_activity() << ":" << __get_state() << ":stuffing with " << value << EndDebugInfo;
		}
		payload.set_period(baud_period);
		tlm_serial_payload::data_type& data = payload.get_data();
		data.clear();
		data.push_back(value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
	}
	stuff_old_value = value;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::stuff(tlm_serial_payload::data_type& data, bool value)
{
	if(need_stuff(value))
	{
		value = !value;
		if(unlikely(verbose))
		{
			logger << DebugInfo << __get_node_activity() << ":" << __get_state() << ":stuffing with " << value << EndDebugInfo;
		}
		data.push_back(value);
	}
	stuff_old_value = value;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::restart_destuffing(bool value)
{
	destuff_count = 1;
	destuff_old_value = value;
}

template <typename CAN_NODE>
bool tlm_can_node<CAN_NODE>::need_destuff(bool value)
{
	if(destuff_old_value == value)
	{
		if(++destuff_count >= MAX_IDENTICAL_CONSECUTIVE_BITS)
		{
			destuff_count = 1;
			return true;
		}
	}
	else
	{
		destuff_count = 1;
	}
	
	return false;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::destuff(bool value)
{
	if(need_destuff(value))
	{
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER)
		{
			if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
			{
				feed(last_feed + baud_period, TLM_CAN_RECESSIVE);
			}
			else
			{
				payload.set_period(baud_period);
				tlm_serial_payload::data_type& data = payload.get_data();
				data.clear();
				data.push_back(TLM_CAN_RECESSIVE);
				CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			}
			
			t_transmit += baud_period;
		}
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":seeking to " << qk_sample_point.get_current_time() << " (destuffing)" << EndDebugInfo;
		}
		if((rx_bitstream.seek(qk_sample_point.get_local_time()) == TLM_INPUT_BITSTREAM_NEED_SYNC) || qk_sample_point.need_sync())
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":need sync (destuffing)" << EndDebugInfo;
			}
			qk_sample_point.sync();
			rx_bitstream.latch();
		}
		
		bool stuff_value = nb_read();
		__received_bit(stuff_value);
		
		if(stuff_value == value)
		{
			__set_can_error(TLM_CAN_STUFF_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":bad stuffing from transmitter (" << stuff_value << " instead of " << !value << ")" << EndDebugInfo;
			}
		}
		
		value = stuff_value;
		
		qk_sample_point.inc(baud_period);
	}
	destuff_old_value = value;
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::inc_transmit_error_count(int delta)
{
	uint8_t tec = __get_transmit_error_count();
	uint8_t rec = __get_receive_error_count();

	if(delta >= 0)
	{
		bool tec_overflow = (delta > (0xff - tec));
		
		delta = tec_overflow ? (0xff - tec) : delta;
		
		if(tec_overflow)
		{
			// 10. A node 'is bus off' when TRANSMIT ERROR COUNT is greater than or equals to 256.
			__set_state(TLM_CAN_BUS_OFF_STATE);
			if(__get_state() == TLM_CAN_BUS_OFF_STATE)
			{
				if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
				{
					__set_node_activity(TLM_CAN_NODE_ACTIVITY_RECEIVER);
				}
			
				bus_off_round_count = 0;
			}
		}
	}
	else
	{
		bool tec_underflow = (-delta > tec);
		
		delta = tec_underflow ? tec : delta;
	}
	
	tec += delta;
	
	__set_transmit_error_count(tec);
	
	if(__get_state() != TLM_CAN_BUS_OFF_STATE)
	{
		// 9. A node is 'error passive' when the TRANSMIT ERROR COUNT equals or exceeds 128, or
		//    when the RECEIVE ERROR COUNT equals or exceeds 128.
		// 11. An 'error passive' node becomes 'error active' again when both the TRANSMIT ERROR COUNT
		//     and the RECEIVE ERROR COUNT are less than or equal to 127
		__set_state(((tec >= 128) || (rec >= 128)) ? TLM_CAN_ERROR_PASSIVE_STATE : TLM_CAN_ERROR_ACTIVE_STATE);
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::inc_receive_error_count(int delta)
{
	uint8_t tec = __get_transmit_error_count();
	uint8_t rec = __get_receive_error_count();
	
	if(delta >= 0)
	{
		bool rec_overflow = (delta > (0xff - rec));
		
		delta = rec_overflow ? (0xff - rec) : delta;
	}
	else
	{
		bool rec_underflow = (-delta > rec);
		
		delta = rec_underflow ? rec : delta;
	}
	
	rec += delta;
	
	__set_receive_error_count(rec);
	
	if(__get_state() != TLM_CAN_BUS_OFF_STATE)
	{
		// 9. A node is 'error passive' when the TRANSMIT ERROR COUNT equals or exceeds 128, or
		//    when the RECEIVE ERROR COUNT equals or exceeds 128.
		// 11. An 'error passive' node becomes 'error active' again when both the TRANSMIT ERROR COUNT
		//     and the RECEIVE ERROR COUNT are less than or equal to 127
		__set_state(((tec >= 128) || (rec >= 128)) ? TLM_CAN_ERROR_PASSIVE_STATE : TLM_CAN_ERROR_ACTIVE_STATE);
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::count_recessive_bits(bool value)
{
	if(value == TLM_CAN_RECESSIVE)
	{
		recessive_bit_count++;
	}
	else
	{
		recessive_bit_count = 0;
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::transceive_error_frame()
{
	assert((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER) || __get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER);
	__set_phase(TLM_CAN_ERROR_FLAG_PHASE);

	bool bit_error = false;
	bool send_value = false;
	bool recv_value = false;
	tlm_serial_payload::data_type& data = payload.get_data();
	
	// transmit/receive an error frame
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting a error frame over CAN_TX because of " << tlm_can_error(__get_can_error()) << EndDebugInfo;
	}
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		// 3. When a TRANSMITTER sends an ERROR FLAG the TRANSMIT ERROR COUNT is increased by 8
		if(!(__get_can_error() & TLM_CAN_ACK_ERROR))
		{
			// Exception 1:
			// If the TRANSMITTER is 'error passive' and detects an ACKNOWLEDGMENT ERROR because of not detecting a 'dominant' ACK...
			inc_transmit_error_count(8);
		}
	}
	else
	{
		// 1. When a RECEIVER detects an error, the RECEIVE ERROR COUNT will be increased by 1...
		inc_receive_error_count(1);
	}
	
	unsigned int dominant_bit_count = 0;

	for(unsigned int i = 0; i < ERROR_FLAG_LENGTH; i++)
	{
		unsigned int bit_index = ERROR_FLAG_LENGTH - i - 1;
		
		payload.set_period(baud_period);
		tlm_serial_payload::data_type& data = payload.get_data();
		data.clear();
		send_value = (__get_state() == TLM_CAN_ERROR_ACTIVE_STATE) ? TLM_CAN_DOMINANT : TLM_CAN_RECESSIVE;
		data.push_back(send_value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
		
		recv_value = b_read(bit_index);
		if(recv_value == TLM_CAN_DOMINANT)
		{
			dominant_bit_count++;
		}
		
		bit_error = (send_value != recv_value);

		if(bit_error && (__get_state() == TLM_CAN_ERROR_ACTIVE_STATE))
		{
			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				// 4. If an TRANSMITTER detects a BIT ERROR while sending an ACTIVE ERROR FLAG the TRANSMIT ERROR COUNT is increased by 8
				inc_transmit_error_count(8);
			}
			else
			{
				// 5. If an RECEIVE detects a BIT ERROR while sending an ACTIVE ERROR FLAG the RECEIVE ERROR COUNT is increased by 8
				inc_receive_error_count(8);
			}
		}
		
		qk_sample_point.inc(baud_period);
	}
	

	__set_phase(TLM_CAN_ERROR_DELIMITER_PHASE);

	bool first_dominant_sequence = true;
	bool first_bit_after_error_flag = true;
	for(unsigned int i = 0; i < ERROR_FLAG_DELIMITER_LENGTH; i = (recv_value == TLM_CAN_RECESSIVE) ? (i + 1) : 0)
	{
		unsigned int bit_index = ERROR_FLAG_DELIMITER_LENGTH - i - 1;
		
		payload.set_period(baud_period);
		data.clear();
		data.push_back(TLM_CAN_RECESSIVE);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
		
		recv_value = b_read(bit_index);
		
		if(recv_value == TLM_CAN_DOMINANT)
		{
			dominant_bit_count++;
			
			if((first_dominant_sequence && (dominant_bit_count >= (ERROR_FLAG_LENGTH + ERROR_FLAG_DELIMITER_LENGTH))) ||
				(!first_dominant_sequence && (dominant_bit_count >= ERROR_FLAG_DELIMITER_LENGTH)))
			{
				first_dominant_sequence = false;
				// 6. after detecting the 14th consecutive 'dominant' bit (in case of an ACTIVE ERROR FLAG) or
				//    after detecting 8th consecutive 'dominant' bit following a PASSIVE ERROR FLAG, and
				//    after each sequence of additional eight consecutive 'dominant' bits 
				if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
				{
					// every TRANSMITTER increases its TRANSMITTER ERROR COUNT
					inc_transmit_error_count(8);
				}
				else
				{
					// every RECEIVER increases its TRANSMITTER ERROR COUNT
					inc_receive_error_count(8);
				}
			}
		}
		else
		{
			dominant_bit_count = 0;
		}
		
		if((recv_value == TLM_CAN_DOMINANT) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER) && first_bit_after_error_flag)
		{
			// 2. When a RECEIVER detects a 'dominant' bit as the first bit after sending an ERROR FLAG the RECEIVE ERROR COUNT will be increased by 8
			first_bit_after_error_flag = false;
			inc_receive_error_count(8);
		}
		
		qk_sample_point.inc(baud_period);
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::transceive_overload_frame()
{
	assert((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER) || __get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER);
	__set_phase(TLM_CAN_OVERLOAD_FLAG_PHASE);

	bool bit_error = false;
	bool send_value = false;
	bool recv_value = false;
	tlm_serial_payload::data_type& data = payload.get_data();
	
	// transmit/receive an error frame
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting a overload frame over CAN_TX" << EndDebugInfo;
	}
	
	unsigned int dominant_bit_count = 0;
	for(unsigned int i = 0; i < OVERLOAD_FLAG_LENGTH; i++)
	{
		unsigned int bit_index = OVERLOAD_FLAG_LENGTH - i - 1;
		
		payload.set_period(baud_period);
		tlm_serial_payload::data_type& data = payload.get_data();
		data.clear();
		send_value = (__get_state() == TLM_CAN_ERROR_ACTIVE_STATE) ? TLM_CAN_DOMINANT : TLM_CAN_RECESSIVE;
		data.push_back(send_value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
		
		recv_value = b_read(bit_index);
		
		if(recv_value == TLM_CAN_DOMINANT)
		{
			dominant_bit_count++;
		}
		
		bit_error = (send_value != recv_value);

		if(bit_error)
		{
			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				// 4. If an TRANSMITTER detects a BIT ERROR while sending an OVERLOAD FLAG the TRANSMIT ERROR COUNT is increased by 8
				inc_transmit_error_count(8);
			}
			else
			{
				// 5. If an RECEIVE detects a BIT ERROR while sending an OVERLOAD FLAG the RECEIVE ERROR COUNT is increased by 8
				inc_receive_error_count(8);
			}
		}
		
		qk_sample_point.inc(baud_period);
	}
	
	bool first_dominant_sequence = true;

	__set_phase(TLM_CAN_OVERLOAD_DELIMITER_PHASE);

	for(unsigned int i = 0; i < OVERLOAD_FLAG_DELIMITER_LENGTH; i = (recv_value == TLM_CAN_RECESSIVE) ? (i + 1) : 0)
	{
		unsigned int bit_index = OVERLOAD_FLAG_DELIMITER_LENGTH - i - 1;
		
		payload.set_period(baud_period);
		data.clear();
		data.push_back(TLM_CAN_RECESSIVE);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
		
		recv_value = b_read(bit_index);
		
		if(recv_value == TLM_CAN_DOMINANT)
		{
			dominant_bit_count++;
			
			if(first_dominant_sequence && (dominant_bit_count >= (OVERLOAD_FLAG_LENGTH + OVERLOAD_FLAG_DELIMITER_LENGTH)))
			{
				first_dominant_sequence = false;
				// 6. after detecting the 14th consecutive 'dominant' bit (in case of an OVERLOAD FLAG) 
				if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
				{
					// every TRANSMITTER increases its TRANSMITTER ERROR COUNT
					inc_transmit_error_count(8);
				}
				else
				{
					// every RECEIVER increases its TRANSMITTER ERROR COUNT
					inc_receive_error_count(8);
				}
			}
		}
		else
		{
			dominant_bit_count = 0;
		}
		
		qk_sample_point.inc(baud_period);
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::transceive_intermission()
{
	assert((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER) || __get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER);
	__set_phase(TLM_CAN_INTERMISSION_PHASE);
	
	bool overload = false;
	bool send_value = false;
	bool recv_value = false;
	tlm_serial_payload::data_type& data = payload.get_data();

	// interframe space intermission
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":intermission starts" << EndDebugInfo;
	}

	for(unsigned int i = 0; !overload && (i < INTERMISSION_LENGTH); i++)
	{
		unsigned int bit_index = INTERMISSION_LENGTH - i - 1;
		
		payload.set_period(baud_period);
		data.clear();
		send_value = TLM_CAN_RECESSIVE;
		data.push_back(send_value); // intermission
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
		
		if(i == 0)
		{
			qk_sample_point.sync(); // force synchronization between frames to enable back to back frame transmission
		}

		bool intermission = recv_value = b_read(bit_index);
		
		if(verbose)
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got " << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
		}
		
		overload = (intermission == TLM_CAN_DOMINANT);
		if(overload)
		{
			// 2. Detection of a 'dominant' bit during INTERMISSION
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":overload" << EndDebugInfo;
			}
		}
		
		qk_sample_point.inc(baud_period);
	}
		
	if(overload)
	{
		// overload: dominant bits during intermission
		transceive_overload_frame();
		__set_phase(TLM_CAN_INTERMISSION_PHASE);
	}
	else
	{
		__set_phase(enabled ? TLM_CAN_SOF_PHASE : TLM_CAN_IDLE_PHASE);
		recessive_bit_count = 0;
		last_feed = t_transmit;
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::transceive_start_of_frame()
{
	// start of frame
	transmitted_last_message = false;
	__set_node_activity(TLM_CAN_NODE_ACTIVITY_SYNCHRONIZING);
	bool bit_error = false;
	bool send_value = false;
	bool recv_value = false;
	__set_can_error(TLM_CAN_NO_ERROR);
	tlm_serial_payload::data_type& data = payload.get_data();
	bool passive = false;
	
	if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
	{
		// inject enough recessive bits for interframe space suspend transmission if we were last transmitter and in error passive node
		if(!__has_pending_transmission_request())
		{
			// there's nothing to transmit: inject some recessive bits up to a limit
			if(model_accuracy == TLM_CAN_TRANSMISSION_START_INACCURATE)
			{
				// limit is up to ACK slot
				if(t_transmit >= last_feed) feed(t_transmit + (INACCURATE_TRANSMISSION_START_CYCLES * baud_period), TLM_CAN_RECESSIVE);
			}
			else
			{
				// limit is up to next cycle
				feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
			}
			passive = true;
		}
		else if(hot_plugged && (recessive_bit_count < (1 + EOF_LENGTH + INTERMISSION_LENGTH)))
		{
			// there's something to transmit but we've just been plugged: inject enough recessive bits for ack delimiter + eof + intermission
			feed(t_transmit + ((1 + EOF_LENGTH + INTERMISSION_LENGTH - recessive_bit_count) * baud_period), TLM_CAN_RECESSIVE);
			passive = true;
		}
		else if(transmitted_last_message && (__get_state() == TLM_CAN_ERROR_PASSIVE_STATE) && (recessive_bit_count < SUSPEND_TRANSMISSION_LENGTH))
		{
			// there's something to transmit be we were last transmitter and in error passive state: inject enough recessive bits for interframe space suspend transmission
			feed(t_transmit + ((SUSPEND_TRANSMISSION_LENGTH - recessive_bit_count) * baud_period), TLM_CAN_RECESSIVE);
			passive = true;
		}
		else
		{
			// there's something to transmit: don't feed with recessive bits
			passive = (t_transmit < last_feed);
		}
	}
	else if(hot_plugged || !__has_pending_transmission_request() || (__get_state() == TLM_CAN_BUS_OFF_STATE))
	{
		passive = true;
		// inject one RECESSIVE bit
		payload.set_period(baud_period);
		data.clear();
		send_value = TLM_CAN_RECESSIVE;
		data.push_back(send_value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		
	}
	
	if(passive)
	{
		t_transmit += baud_period;
		
		// monitor bus
		recv_value = b_read();
		count_recessive_bits(recv_value);
		if(verbose)
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got " << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
		}
		
		// a 'Bus off' node needs to wait 128 times 11 contiguous recessive bits before becoming again an 'error active' node
		if((__get_state() == TLM_CAN_BUS_OFF_STATE) && (recessive_bit_count >= (1 + EOF_LENGTH + INTERMISSION_LENGTH)))
		{
			recessive_bit_count = 0;
			
			bus_off_round_count++;
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":" << bus_off_round_count << " bus off rounds" << EndDebugInfo;
			}
			
			if(bus_off_round_count >= BUS_OFF_ROUNDS)
			{
				// 12. An node which is 'bus off' is permitted to become 'error active' (no longer 'bus off')
				//     with its error counters both set to 0 after 128 occurrence of 11 consecutive 'recessive' bits
				//     have been monitored on the bus
				bus_off_round_count = 0;
				__set_transmit_error_count(0);
				__set_receive_error_count(0);
				__set_state(TLM_CAN_ERROR_ACTIVE_STATE);
			}
		}
		
		// when hot plugged, monitor bus until recessive bits for ACK delimiter + EOF + intermission were received before participating to transmission/reception
		if(hot_plugged && (recessive_bit_count >= (1 + EOF_LENGTH + INTERMISSION_LENGTH)))
		{
			// a sufficient amount of recessive bits were monitored
			hot_plugged = false;
		}
		
		if(rx_bitstream.negedge() && !hot_plugged)
		{
			// receiver: start of frame
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got start of frame" << EndDebugInfo;
			}
			qk_sample_point.inc(baud_period);
			assert((model_accuracy != TLM_CAN_CYCLE_ACCURATE) || (qk_sample_point.get_current_time() - sample_point) == t_transmit);
			
			recessive_bit_count = 0;
			comp_crc = CRC_INIT;
			recv_crc = 0;
			restart_destuffing(TLM_CAN_RECESSIVE);
			
			comp_crc = update_crc(comp_crc, recv_value);
			
			__set_phase(TLM_CAN_ID_PHASE);
			__set_node_activity(TLM_CAN_NODE_ACTIVITY_RECEIVER);
			
			if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
		}
		else
		{
			qk_sample_point.inc(baud_period);
		}
	}
	else
	{
		// transmitter: start of frame
		__set_node_activity(TLM_CAN_NODE_ACTIVITY_TRANSMITTER);
		assert((qk_sample_point.get_current_time() - sample_point) == t_transmit);
		assert((model_accuracy == TLM_CAN_CYCLE_ACCURATE) || (t_transmit == last_feed));

		// get message to transmit
		send_msg = &__pending_transmission_request();
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":starting transmission over CAN_TX of " << (*send_msg) << EndDebugInfo;
		}
		
		// initialize CRC
		send_crc = CRC_INIT;
		recv_crc = 0;
		comp_crc = CRC_INIT;
		restart_stuffing(TLM_CAN_RECESSIVE);
		restart_destuffing(TLM_CAN_RECESSIVE);
		
		// transmitter: start of frame
		if(unlikely(verbose))
		{
			logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting start of frame" << EndDebugInfo;
		}
		payload.set_period(baud_period);
		data.clear();
		send_value = TLM_CAN_DOMINANT;
		data.push_back(send_value);
		send_crc = update_crc(send_crc, send_value);
		stuff(data, send_value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
		
		recv_value = b_read();
		
		if(recv_value == TLM_CAN_DOMINANT)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got start of frame" << EndDebugInfo;
			}
		}
		
		bit_error = (recv_value != TLM_CAN_DOMINANT);
		if(bit_error)
		{
			__set_can_error(TLM_CAN_BIT0_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":bit error" << EndDebugInfo;
			}
			last_feed = qk_sample_point.get_current_time() + phase_seg2;
		}
		
		qk_sample_point.inc(baud_period);
		
		if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);

		if(__get_can_error() == TLM_CAN_NO_ERROR)
		{
			__set_phase(TLM_CAN_ID_PHASE);
		}
	}
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::transceive_data_remote_frame()
{
	assert((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER) || __get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER);
	__set_phase(TLM_CAN_ID_PHASE);
	
	bool bit_error = false;
	bool crc_error = false;
	bool form_error = false;
	bool ack_error = false;
	bool send_value = false;
	bool recv_value = false;
	unsigned int data_bit_length = 0;
	tlm_serial_payload::data_type& data = payload.get_data();
	
	// arbitration
	recv_msg.clear();
		
	// identifier (bits 28-18)
	unsigned int recv_msg_id = 0;
	for(unsigned int i = 0; (__get_can_error() == TLM_CAN_NO_ERROR) && (i < STD_FMT_ID_LENGTH); i++)
	{
		unsigned int bit_index = ID_LENGTH - i - 1;
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			data.clear();
			send_value = (send_msg->get_identifier() >> bit_index) & 1;
			data.push_back(send_value);
			send_crc = update_crc(send_crc, send_value);
			if(unlikely(verbose))
			{
				logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting ID#" << bit_index << "=" << send_value << " (" << tlm_can_value(send_value) << ")" << EndDebugInfo;
			}
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		else if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
		{
			feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
		}
		else
		{
			payload.set_period(baud_period);
			data.clear();
			send_value = TLM_CAN_RECESSIVE;
			data.push_back(send_value);
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		
		t_transmit += baud_period;
		
		recv_value = b_read(bit_index);
		comp_crc = update_crc(comp_crc, recv_value);
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got ID#" << bit_index << "=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
		}
		
		recv_msg_id = recv_msg_id | (recv_value << bit_index);

		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			bit_error = (send_value != recv_value);
			if(bit_error)
			{
				// bit error while arbitration: continue as a receiver
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":lost arbitration" << EndDebugInfo;
				}
				__set_node_activity(TLM_CAN_NODE_ACTIVITY_RECEIVER);
				last_feed = qk_sample_point.get_current_time() + phase_seg2;
			}
		}
		
		qk_sample_point.inc(baud_period);
		
		if((__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
		
		if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
	}
	
	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	// SRR or RTR
	__set_phase(TLM_CAN_SRR_OR_RTR_PHASE);
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		data.clear();
		send_value = (send_msg->get_format() == TLM_CAN_STD_FMT) ? send_msg->get_remote_transmission_request() : TLM_CAN_RECESSIVE /* SRR */;
		data.push_back(send_value);
		send_crc = update_crc(send_crc, send_value);
		if(unlikely(verbose))
		{
			logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting " << ((send_msg->get_format() == TLM_CAN_STD_FMT) ? "RTR" : "SRR") << "=" << send_value << " (" << tlm_can_value(send_value) << ")" << EndDebugInfo;
		}
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
	}
	else if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
	{
		feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
	}
	else
	{
		payload.set_period(baud_period);
		data.clear();
		send_value = TLM_CAN_RECESSIVE;
		data.push_back(send_value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
	}
	
	t_transmit += baud_period;
	
	bool recv_srr_rtr = recv_value = b_read();
	comp_crc = update_crc(comp_crc, recv_value);
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got SRR or RTR=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
	}
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		bit_error = (send_value != recv_value);
		if(bit_error)
		{
			// bit error while arbitration: continue as a receiver
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":lost arbitration" << EndDebugInfo;
			}
			__set_node_activity(TLM_CAN_NODE_ACTIVITY_RECEIVER);
			last_feed = qk_sample_point.get_current_time() + phase_seg2;
		}
	}
	
	qk_sample_point.inc(baud_period);
	
	if((__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
	
	if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);

	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	// r1 or IDE
	__set_phase(TLM_CAN_IDE_OR_R1_PHASE);
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		data.clear();
		send_value = (send_msg->get_format() == TLM_CAN_STD_FMT) ? TLM_CAN_DOMINANT /* r1 */: TLM_CAN_RECESSIVE /* IDE */;
		data.push_back(send_value);
		send_crc = update_crc(send_crc, send_value);
		if(unlikely(verbose))
		{
			logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting " << ((send_msg->get_format() == TLM_CAN_STD_FMT) ? "r1" : "IDE") << "=" << send_value << " (" << tlm_can_value(send_value) << ")" << EndDebugInfo;
		}
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
	}
	else if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
	{
		feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
	}
	else
	{
		payload.set_period(baud_period);
		data.clear();
		send_value = TLM_CAN_RECESSIVE;
		data.push_back(send_value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
	}
	
	t_transmit += baud_period;
	
	bool recv_r1_ide = recv_value = b_read();
	comp_crc = update_crc(comp_crc, recv_value);
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got r1 or IDE=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
	}
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		bit_error = (send_value != recv_value);
		if(bit_error)
		{
			if(send_msg->get_format() != TLM_CAN_STD_FMT)
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":standard frame from peer has priority" << EndDebugInfo;
				}
			}
			
			__set_node_activity(TLM_CAN_NODE_ACTIVITY_RECEIVER);
			last_feed = qk_sample_point.get_current_time() + phase_seg2;
		}
	}
	
	qk_sample_point.inc(baud_period);
	
	if((__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
	
	if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
	
	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	recv_msg.set_format((recv_r1_ide == TLM_CAN_DOMINANT) ? TLM_CAN_STD_FMT : TLM_CAN_XTD_FMT);
	
	if(recv_msg.get_format() == TLM_CAN_STD_FMT)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":standard format" << EndDebugInfo;
		}
		recv_msg.set_identifier(recv_msg_id);
		recv_msg.set_remote_transmission_request(recv_srr_rtr);
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":frame being received is a " << (recv_msg.get_remote_transmission_request() ? "remote" : "data") << " frame" << EndDebugInfo;
		}
	}
	else
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":extended format" << EndDebugInfo;
		}
		
		// identifier (bits 17-0)
		__set_phase(TLM_CAN_ID_PHASE);
		for(unsigned int i = STD_FMT_ID_LENGTH; (__get_can_error() == TLM_CAN_NO_ERROR) && (i < ID_LENGTH); i++)
		{
			unsigned int bit_index = ID_LENGTH - i - 1;
			
			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				data.clear();
				send_value = (send_msg->get_identifier() >> bit_index) & 1;
				data.push_back(send_value);
				send_crc = update_crc(send_crc, send_value);
				if(unlikely(verbose))
				{
					logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting ID#" << bit_index << "=" << send_value << " (" << tlm_can_value(send_value) << ")" << EndDebugInfo;
				}
				CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			}
			else if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
			{
				feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
			}
			else
			{
				payload.set_period(baud_period);
				data.clear();
				send_value = TLM_CAN_RECESSIVE;
				data.push_back(send_value);
				CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			}
			
			t_transmit += baud_period;
			
			recv_value = b_read(bit_index);
			comp_crc = update_crc(comp_crc, recv_value);
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got ID#" << bit_index << "=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
			}
			
			recv_msg_id = recv_msg_id | (recv_value << bit_index);

			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				bit_error = (send_value != recv_value);
				if(bit_error)
				{
					if(unlikely(verbose))
					{
						logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":lost arbitration" << EndDebugInfo;
					}
					__set_node_activity(TLM_CAN_NODE_ACTIVITY_RECEIVER);
					last_feed = qk_sample_point.get_current_time() + phase_seg2;
				}
			}
			
			qk_sample_point.inc(baud_period);
			
			if((__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
			
			if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
		}
		
		if(__get_can_error() != TLM_CAN_NO_ERROR) return;
		
		recv_msg.set_identifier(recv_msg_id);
		
		// RTR
		__set_phase(TLM_CAN_RTR_PHASE);
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			data.clear();
			send_value = send_msg->get_remote_transmission_request() ? TLM_CAN_RECESSIVE : TLM_CAN_DOMINANT;
			data.push_back(send_value);
			send_crc = update_crc(send_crc, send_value);
			if(unlikely(verbose))
			{
				logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting RTR=" << send_value << " (" << tlm_can_value(send_value) << ")" << EndDebugInfo;
			}
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		else if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
		{
			feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
		}
		else
		{
			payload.set_period(baud_period);
			data.clear();
			send_value = TLM_CAN_RECESSIVE;
			data.push_back(send_value);
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		
		t_transmit += baud_period;
		
		bool recv_rtr = recv_value = b_read();
		recv_msg.set_remote_transmission_request(recv_rtr);
		comp_crc = update_crc(comp_crc, recv_value);
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got RTR=" << recv_rtr << " (" << tlm_can_value(recv_rtr) << ")" << EndDebugInfo;
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":frame being received is a " << (recv_msg.get_remote_transmission_request() ? "remote" : "data") << " frame" << EndDebugInfo;
		}
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			bit_error = (send_value != recv_value);
			if(bit_error)
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":lost arbitration" << EndDebugInfo;
				}
				__set_node_activity(TLM_CAN_NODE_ACTIVITY_RECEIVER);
				last_feed = qk_sample_point.get_current_time() + phase_seg2;
			}
		}
		
		qk_sample_point.inc(baud_period);
		
		if((__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
		
		if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
		
		if(__get_can_error() != TLM_CAN_NO_ERROR) return;
		
		// r1
		__set_phase(TLM_CAN_R1_PHASE);
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			data.clear();
			send_value = TLM_CAN_DOMINANT;
			data.push_back(send_value);
			send_crc = update_crc(send_crc, send_value);
			if(unlikely(verbose))
			{
				logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting r1=" << send_value << " (" << tlm_can_value(send_value) << ")" << EndDebugInfo;
			}
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		else if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
		{
			feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
		}
		else
		{
			payload.set_period(baud_period);
			data.clear();
			send_value = TLM_CAN_RECESSIVE;
			data.push_back(send_value);
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		
		t_transmit += baud_period;
		
		bool recv_r1 = recv_value = b_read();
		comp_crc = update_crc(comp_crc, recv_value);
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got r1=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
		}
		if(recv_r1 != TLM_CAN_DOMINANT)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":r1 should be DOMINANT" << EndDebugInfo;
			}
		}
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			bit_error = (send_value != recv_value);
			if(bit_error)
			{
				__set_can_error((send_value == TLM_CAN_RECESSIVE) ? TLM_CAN_BIT1_ERROR : TLM_CAN_BIT0_ERROR);
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":lost arbitration" << EndDebugInfo;
				}
				last_feed = qk_sample_point.get_current_time() + phase_seg2;
			}
		}
		
		qk_sample_point.inc(baud_period);
		
		if((__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
		
		if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
		
		if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	}
	
	// r0
	__set_phase(TLM_CAN_R0_PHASE);
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		data.clear();
		send_value = TLM_CAN_DOMINANT;
		data.push_back(send_value);
		send_crc = update_crc(send_crc, send_value);
		if(unlikely(verbose))
		{
			logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":transmitting r0=" << send_value << " (" << tlm_can_value(send_value) << ")" << EndDebugInfo;
		}
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
	}
	else if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
	{
		feed(t_transmit + baud_period, TLM_CAN_RECESSIVE);
	}
	else
	{
		payload.set_period(baud_period);
		data.clear();
		send_value = TLM_CAN_RECESSIVE;
		data.push_back(send_value);
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
	}
	
	t_transmit += baud_period;
	
	bool recv_r0 = recv_value = b_read();
	comp_crc = update_crc(comp_crc, recv_value);
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got r0=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
	}
	if(recv_r0 != TLM_CAN_DOMINANT)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":r0 should be DOMINANT" << EndDebugInfo;
		}
	}
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		bit_error = (send_value != recv_value);
		if(bit_error)
		{
			__set_can_error((send_value == TLM_CAN_RECESSIVE) ? TLM_CAN_BIT1_ERROR : TLM_CAN_BIT0_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":lost arbitration" << EndDebugInfo;
			}
			last_feed = qk_sample_point.get_current_time() + phase_seg2;
		}
	}
	
	qk_sample_point.inc(baud_period);
	
	if((__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
		
	if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);

	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	// DLC
	__set_phase(TLM_CAN_DLC_PHASE);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":won arbitration" << EndDebugInfo;
	}
	
	transmitted_last_message = true;
	
	if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
	{
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			data.clear();
			for(unsigned int i = 0; i < DLC_LENGTH; i++)
			{
				unsigned int bit_index = DLC_LENGTH - 1 - i;
				send_value = (send_msg->get_data_length_code() >> bit_index) & 1;
				data.push_back(send_value);
				send_crc = update_crc(send_crc, send_value);
				stuff(data, send_value);
			}
			if(!send_msg->get_remote_transmission_request())
			{
				// it's a data frame
				data_bit_length = 8 * send_msg->get_data_length();
				for(unsigned int i = 0; i < data_bit_length; i++)
				{
					unsigned int byte_index = i / 8;
					unsigned int shift = 7 - (i % 8);
					send_value = (send_msg->get_data_byte_at(byte_index) >> shift) & 1;
					data.push_back(send_value);
					send_crc = update_crc(send_crc, send_value);
					stuff(data, send_value);
				}
			}
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":CRC=0x" << std::hex << send_crc << std::dec << EndDebugInfo;
			}
			for(unsigned int i = 0; i < CRC_LENGTH; i++)
			{
				unsigned int shift = CRC_LENGTH - i - 1;
				send_value = (send_crc >> shift) & 1;
				data.push_back(send_value);
				stuff(data, send_value);
			}
			data.push_back(TLM_CAN_RECESSIVE); // CRC delimiter
			
			data.push_back(TLM_CAN_RECESSIVE); // ack slot
			data.push_back(TLM_CAN_RECESSIVE); // ack delimiter
			for(unsigned int i = 0; i < EOF_LENGTH; i++)
			{
				data.push_back(TLM_CAN_RECESSIVE); // end of frame
			}
			
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			inc_time(t_transmit, data.size());
		}
		else
		{
			// I'm not the transmitter: inject enough recessive bits for DLC
			t_transmit = feed(qk_sample_point.get_current_time() + phase_seg2 + ((DLC_LENGTH - 1) * baud_period), TLM_CAN_RECESSIVE);
		}
	}
	
	unsigned int recv_msg_dlc = 0;
	for(unsigned int i = 0; (__get_can_error() == TLM_CAN_NO_ERROR) && (i < DLC_LENGTH); i++)
	{
		unsigned int bit_index = DLC_LENGTH - 1 - i;
		
		if(model_accuracy == TLM_CAN_CYCLE_ACCURATE)
		{
			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				payload.set_period(baud_period);
				data.clear();
				send_value = (send_msg->get_data_length_code() >> bit_index) & 1;
				data.push_back(send_value);
				send_crc = update_crc(send_crc, send_value);
				CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			}
			else
			{
				payload.set_period(baud_period);
				data.clear();
				send_value = TLM_CAN_RECESSIVE;
				data.push_back(send_value);
				CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			}
			t_transmit += baud_period;
		}
		
		bool recv_value = b_read(bit_index);
		comp_crc = update_crc(comp_crc, recv_value);
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got DLC#" << bit_index << "=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
		}
		recv_msg_dlc = recv_msg_dlc | (recv_value << bit_index);
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			send_value = (send_msg->get_data_length_code() >> bit_index) & 1;
			bit_error = (send_value != recv_value);
			
			if(bit_error)
			{
				__set_can_error((send_value == TLM_CAN_RECESSIVE) ? TLM_CAN_BIT1_ERROR : TLM_CAN_BIT0_ERROR);
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":bit error" << EndDebugInfo;
				}
			}
		}
		
		qk_sample_point.inc(baud_period);
		
		if((model_accuracy == TLM_CAN_CYCLE_ACCURATE) && (__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
		
		if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
	}
	
	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	recv_msg.set_data_length_code(recv_msg_dlc);
	
	if(((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER) && !send_msg->get_remote_transmission_request()) ||
	((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER) && !recv_msg.get_remote_transmission_request()))
	{
		// data
		__set_phase(TLM_CAN_DATA_PHASE);
		
		if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
		{
			if(__get_node_activity() != TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				// I'm not the transmitter: inject enough recessive bits for DATA, CRC and CRC DELIMITER
				data_bit_length = 8 * recv_msg.get_data_length();
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":expecting " << data_bit_length << " data bits" << EndDebugInfo;
				}
				t_transmit = feed(qk_sample_point.get_current_time() + phase_seg2 + ((data_bit_length + CRC_LENGTH) * baud_period), TLM_CAN_RECESSIVE);
			}
		}
		else
		{
			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				data_bit_length = 8 * send_msg->get_data_length();
			}
			else
			{
				data_bit_length = 8 * recv_msg.get_data_length();
			}
		}
		
		uint8_t recv_msg_data_byte = 0;
		for(unsigned int i = 0; (__get_can_error() == TLM_CAN_NO_ERROR) && (i < data_bit_length); i++)
		{
			unsigned int byte_index = i / 8;
			unsigned int shift = 7 - (i % 8);
			unsigned int bit_index = data_bit_length - i - 1;
			
			if(model_accuracy == TLM_CAN_CYCLE_ACCURATE)
			{
				if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
				{
					payload.set_period(baud_period);
					data.clear();
					send_value = (send_msg->get_data_byte_at(byte_index) >> shift) & 1;
					data.push_back(send_value);
					send_crc = update_crc(send_crc, send_value);
					CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
				}
				else
				{
					payload.set_period(baud_period);
					data.clear();
					send_value = TLM_CAN_RECESSIVE;
					data.push_back(send_value);
					CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
				}
				t_transmit += baud_period;
			}
			
			recv_value = b_read(bit_index);
			comp_crc = update_crc(comp_crc, recv_value);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got DATA#" << bit_index << "=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
			}
			
			recv_msg_data_byte = ((shift == 7) ? 0 : recv_msg_data_byte) | (recv_value << shift);
			if(shift == 0) recv_msg.set_data_byte_at(byte_index, recv_msg_data_byte);
			
			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				send_value = (send_msg->get_data_byte_at(byte_index) >> shift) & 1;
				bit_error = (send_value != recv_value);
				
				if(bit_error)
				{
					__set_can_error((send_value == TLM_CAN_RECESSIVE) ? TLM_CAN_BIT1_ERROR : TLM_CAN_BIT0_ERROR);
					if(unlikely(verbose))
					{
						logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":bit error" << EndDebugInfo;
					}
				}
			}
			
			qk_sample_point.inc(baud_period);
			
			if((model_accuracy == TLM_CAN_CYCLE_ACCURATE) && (__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
			
			if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
		}
		
		if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	}
	else
	{
		if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
		{
			if(__get_node_activity() != TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				// I'm not the transmitter: inject enough recessive bits for CRC and CRC DELIMITER
				t_transmit = feed(qk_sample_point.get_current_time() + phase_seg2 + (CRC_LENGTH * baud_period), TLM_CAN_RECESSIVE);
			}
		}
	}
	
	// CRC
	__set_phase(TLM_CAN_CRC_PHASE);
	
	for(unsigned int i = 0; (__get_can_error() == TLM_CAN_NO_ERROR) && (i < CRC_LENGTH); i++)
	{
		unsigned int bit_index = CRC_LENGTH - i - 1;
		unsigned int shift = bit_index;
		
		if(model_accuracy == TLM_CAN_CYCLE_ACCURATE)
		{
			if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
			{
				payload.set_period(baud_period);
				data.clear();
				send_value = (send_crc >> shift) & 1;
				data.push_back(send_value);
				CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			}
			else
			{
				payload.set_period(baud_period);
				data.clear();
				send_value = TLM_CAN_RECESSIVE;
				data.push_back(send_value);
				CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			}
			t_transmit += baud_period;
		}
		
		recv_value = b_read(bit_index);
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got CRC#" << bit_index << "=" << recv_value << " (" << tlm_can_value(recv_value) << ")" << EndDebugInfo;
		}
		recv_crc = recv_crc | (recv_value << shift);
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			send_value = (send_crc >> shift) & 1;
			bit_error = (send_value != recv_value);
			
			if(bit_error)
			{
				__set_can_error((send_value == TLM_CAN_RECESSIVE) ? TLM_CAN_BIT1_ERROR : TLM_CAN_BIT0_ERROR);
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":bit error" << EndDebugInfo;
				}
			}
		}
		
		qk_sample_point.inc(baud_period);
		
		if((model_accuracy == TLM_CAN_CYCLE_ACCURATE) && (__get_can_error() == TLM_CAN_NO_ERROR) && (__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)) stuff(send_value);
		
		if(__get_can_error() == TLM_CAN_NO_ERROR) destuff(recv_value);
	}
	
	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	// CRC delimiter
	__set_phase(TLM_CAN_CRC_DELIMITER_PHASE);
	
	if(model_accuracy == TLM_CAN_CYCLE_ACCURATE)
	{
		payload.set_period(baud_period);
		data.clear();
		data.push_back(TLM_CAN_RECESSIVE); // CRC delimiter
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
	}
	
	bool crc_delimiter = b_read();
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got CRC DELIMITER=" << crc_delimiter << " (" << tlm_can_value(crc_delimiter) << ")" << EndDebugInfo;
	}
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		bit_error = (crc_delimiter != TLM_CAN_RECESSIVE);
		if(bit_error)
		{
			__set_can_error(TLM_CAN_BIT1_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":bit error" << EndDebugInfo;
			}
		}
	}
	else
	{
		form_error = (crc_delimiter != TLM_CAN_RECESSIVE);
		if(form_error)
		{
			__set_can_error(TLM_CAN_FORM_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":form error" << EndDebugInfo;
			}
		}
	}
	
	qk_sample_point.inc(baud_period);
	
	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	// CRC check
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":computed CRC=0x" << std::hex << comp_crc << std::dec << EndDebugInfo;
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":frame CRC=0x" << std::hex << recv_crc << std::dec << EndDebugInfo;
		}
		crc_error = (recv_crc != comp_crc);
		
		if(crc_error)
		{
			__set_can_error(TLM_CAN_CRC_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":CRC error" << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":CRC OK" << EndDebugInfo;
			}
		}
	}
	
	if(__get_can_error() != TLM_CAN_NO_ERROR) return;

	// ACK
	__set_phase(TLM_CAN_ACK_PHASE);
	
	if(model_accuracy != TLM_CAN_CYCLE_ACCURATE)
	{
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER)
		{
			payload.set_period(baud_period);
			data.clear();
			data.push_back((__get_state() != TLM_CAN_BUS_OFF_STATE) ? TLM_CAN_DOMINANT : TLM_CAN_RECESSIVE);  // ack
			data.push_back(TLM_CAN_RECESSIVE); // ack delimiter
			for(unsigned int i = 0; i < EOF_LENGTH; i++)
			{
				data.push_back(TLM_CAN_RECESSIVE); // end of frame
			}
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			inc_time(t_transmit, data.size());
		}
	}
	else
	{
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			payload.set_period(baud_period);
			data.clear();
			data.push_back(TLM_CAN_RECESSIVE);  // ack slot
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		else
		{
			payload.set_period(baud_period);
			data.clear();
			data.push_back((__get_state() != TLM_CAN_BUS_OFF_STATE) ? TLM_CAN_DOMINANT : TLM_CAN_RECESSIVE);  // ack
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		}
		t_transmit += baud_period;
	}
	
	
	bool ack = b_read();
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got ACK=" << ack << " (" << tlm_can_value(ack) << ")" << EndDebugInfo;
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":" << ((ack == TLM_CAN_DOMINANT) ? "message acknowledged" : "message not acknowledged") << EndDebugInfo;
	}
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		ack_error = !__is_loopback_enabled() && (ack != TLM_CAN_DOMINANT);
		
		if(ack_error)
		{
			__set_can_error(TLM_CAN_ACK_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":acknowledgment error" << EndDebugInfo;
			}
		}
	}
	
	qk_sample_point.inc(baud_period);
	
	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER)
	{
		// 8. After the successful reception of a message (reception without error up to the ACK SLOT and the successful sending of the ACK bit),
		if(__get_receive_error_count() <= 127)
		{
			// the RECEIVE ERROR COUNT is decreased by 1, if it was between 1 and 127.
			// If the RECEIVE ERROR COUNT was 0, it stays 0
			inc_receive_error_count(-1);
		}
		else
		{
			// and if it was greater than 127, then it will be set to a value between 119 and 127
			__set_receive_error_count(127);
		}
	}
	
	// ACK delimiter
	__set_phase(TLM_CAN_ACK_DELIMITER_PHASE);
	
	if(model_accuracy == TLM_CAN_CYCLE_ACCURATE)
	{
		payload.set_period(baud_period);
		data.clear();
		data.push_back(TLM_CAN_RECESSIVE);  // ack delimiter
		CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
		t_transmit += baud_period;
	}
	
	bool ack_delimiter = b_read();
	if(unlikely(verbose))
	{
		logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got ACK DELIMITER=" << ack_delimiter << " (" << tlm_can_value(ack_delimiter) << ")" << EndDebugInfo;
	}
	
	if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
	{
		bit_error = (ack_delimiter != TLM_CAN_RECESSIVE);
		if(bit_error)
		{
			__set_can_error(TLM_CAN_BIT1_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":bit error" << EndDebugInfo;
			}
		}
	}
	else
	{
		form_error = (ack_delimiter != TLM_CAN_RECESSIVE);
		if(form_error)
		{
			__set_can_error(TLM_CAN_FORM_ERROR);
			if(unlikely(verbose))
			{
				logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":form error" << EndDebugInfo;
			}
		}
	}
	
	qk_sample_point.inc(baud_period);

	if(__get_can_error() != TLM_CAN_NO_ERROR) return;
	
	// end of frame
	__set_phase(TLM_CAN_EOF_PHASE);
	
	for(unsigned int i = 0; (__get_can_error() == TLM_CAN_NO_ERROR) && (i < EOF_LENGTH); i++)
	{
		unsigned int bit_index = EOF_LENGTH - i - 1;
		
		if(model_accuracy == TLM_CAN_CYCLE_ACCURATE)
		{
			payload.set_period(baud_period);
			data.clear();
			send_value = TLM_CAN_RECESSIVE;
			data.push_back(send_value);
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			t_transmit += baud_period;
		}

		bool eof = b_read(bit_index);
		if(unlikely(verbose))
		{
			logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":got EOF#" << bit_index << "=" << eof << " (" << tlm_can_value(eof) << ")" << EndDebugInfo;
		}
			
		// The message is valid for the transmitter, if there is no error until the end of END OF FRAME
		// The message is valid for the receiver, if there is no error until the last but one bit of END OF FRAME
		if((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER) || (i < (EOF_LENGTH - 1)))
		{
			form_error = (eof != TLM_CAN_RECESSIVE);
			if(form_error)
			{
				__set_can_error(TLM_CAN_FORM_ERROR);
				if(unlikely(verbose))
				{
					logger << DebugInfo << qk_sample_point.get_current_time() << ":" << sc_core::sc_time_stamp() << "[+" << qk_sample_point.get_local_time() << "]:" << __get_node_activity() << ":" << __get_state() << ":form error" << EndDebugInfo;
				}
			}
		}

		if((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER) && (__get_can_error() != TLM_CAN_NO_ERROR))
		{
			// 7. After the successful transmission of a message (getting ACK and no error until END OF FRAME is finished)
			//    the TRANSMIT ERROR COUNT is decreased by 1 unless it was already 0
			inc_transmit_error_count(-1);
		}
		
		qk_sample_point.inc(baud_period);
	}
	
	if(__get_can_error() == TLM_CAN_NO_ERROR)
	{
		if((__get_node_activity() == TLM_CAN_NODE_ACTIVITY_RECEIVER) || __is_loopback_enabled())
		{
			__receive_message(recv_msg);
		}
		
		if(__get_node_activity() == TLM_CAN_NODE_ACTIVITY_TRANSMITTER)
		{
			assert(__has_pending_transmission_request());
			__transmission_occurred(*send_msg);
		}
	}
	
	recessive_bit_count = 0;
	__set_phase(TLM_CAN_INTERMISSION_PHASE);
}

template <typename CAN_NODE>
void tlm_can_node<CAN_NODE>::process()
{
	// top: list
	// list: element | element list
	// element: pattern overload-frames interframe-space | pattern interframe-space
	// pattern: frame | frame error-frame
	// frame : data-frame | remote-frame
	// error-frame: error-flag error-delimiter
	// overload-frames: overload-frame | overload-frame overload-frames
	// overload-frame: overload-flag overload-delimiter
	// interframe space-space: intermission bus-ide | intermission transmit-suspend bus-idle
	
	t_transmit = sc_core::SC_ZERO_TIME;
	tlm_serial_payload::data_type& data = payload.get_data();
	
	if(enabled)
	{
		__set_node_activity(TLM_CAN_NODE_ACTIVITY_SYNCHRONIZING);
		__set_phase(TLM_CAN_SOF_PHASE);
		sample_point = __get_sample_point();
		phase_seg2 = __get_phase_seg2();
		baud_period = sample_point + phase_seg2;
		qk_sample_point.set(sample_point);
	}
	else
	{
		__set_phase(TLM_CAN_IDLE_PHASE);
	}
	
	do
	{
		if(__get_phase() == TLM_CAN_IDLE_PHASE)
		{
			__set_node_activity(TLM_CAN_NODE_ACTIVITY_IDLE);
			if(unlikely(verbose))
			{
				logger << DebugInfo << t_transmit << ":" << sc_core::sc_time_stamp() << "[+" << (t_transmit - sc_core::sc_time_stamp()) << "]:" << __get_node_activity() << ":" << __get_state() << ":node is idle" << EndDebugInfo;
			}
			
			const sc_core::sc_time& can_global_quantum = tlm_can_global_quantum::instance().get();
			sc_core::sc_time idle_period;
			
			if(can_global_quantum != sc_core::SC_ZERO_TIME)
			{
				// idle_period = can_global_quantum - (t_transmit % can_global_quantum);
				sc_core::sc_time offset(t_transmit);
				offset %= can_global_quantum;
				idle_period = can_global_quantum;
				idle_period -= offset;
			}
			else
			{
				idle_period = sc_core::sc_get_time_resolution();
			}
			
			payload.set_period(idle_period);
			data.clear();
			data.push_back(TLM_CAN_RECESSIVE);
			CAN_TX->nb_send(payload, t_transmit - sc_core::sc_time_stamp());
			t_transmit += payload.get_period();
			wait(t_transmit - sc_core::sc_time_stamp());
			
			sample_point = __get_sample_point();
			phase_seg2 = __get_phase_seg2();
			baud_period = sample_point + phase_seg2;
			
			qk_sample_point.set(sample_point);
			__set_can_error(TLM_CAN_NO_ERROR);
			recessive_bit_count = 0;
			last_feed = t_transmit;
		}
		else if(__get_can_error() != TLM_CAN_NO_ERROR)
		{
			// eat all recessive bits we've speculatively transmitted for temporal decoupling
			if(last_feed > t_transmit) t_transmit = last_feed;
			sc_core::sc_time t_s(qk_sample_point.get_current_time());
			if(t_s < t_transmit)
			{
				assert(model_accuracy != TLM_CAN_CYCLE_ACCURATE);
				while(t_s < t_transmit)
				{
					b_read();
					qk_sample_point.inc(baud_period);
					t_s += baud_period;
				}
			}
			
			// transceive an error frame
			transceive_error_frame();
			
			__set_can_error(TLM_CAN_NO_ERROR);
			__set_phase(TLM_CAN_INTERMISSION_PHASE);
		}
		else if(__get_phase() == TLM_CAN_INTERMISSION_PHASE)
		{
			transceive_intermission();
		}
		else if(__get_phase() == TLM_CAN_SOF_PHASE)
		{
			transceive_start_of_frame();
			
			if(__get_phase() == TLM_CAN_ID_PHASE)
			{
				transceive_data_remote_frame();
			}
		}
	}
	while(1);
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace service {

using unisim::kernel::tlm2::tlm_can_model_accuracy;
using unisim::kernel::tlm2::TLM_CAN_CYCLE_ACCURATE;
using unisim::kernel::tlm2::TLM_CAN_TRANSACTION_ACCURATE;
using unisim::kernel::tlm2::TLM_CAN_TRANSMISSION_START_INACCURATE;

template <> inline Variable<tlm_can_model_accuracy>::Variable(const char *_name, Object *_object, tlm_can_model_accuracy& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Simulator::Instance()->Initialize(this);
	AddEnumeratedValue("cycle-accurate");
	AddEnumeratedValue("transaction-accurate");
	AddEnumeratedValue("transmission-start-inaccurate");
}

template <>
inline const char *Variable<tlm_can_model_accuracy>::GetDataTypeName() const
{
	return "can-model-accuracy";
}

template <>
inline unsigned int Variable<tlm_can_model_accuracy>::GetBitSize() const
{
	return 2;
}

template <> inline Variable<tlm_can_model_accuracy>::operator bool () const { return *storage != TLM_CAN_CYCLE_ACCURATE; }
template <> inline Variable<tlm_can_model_accuracy>::operator long long () const { return *storage; }
template <> inline Variable<tlm_can_model_accuracy>::operator unsigned long long () const { return *storage; }
template <> inline Variable<tlm_can_model_accuracy>::operator double () const { return (double)(*storage); }
template <> inline Variable<tlm_can_model_accuracy>::operator std::string () const
{
	switch(*storage)
	{
		case TLM_CAN_CYCLE_ACCURATE               : return std::string("cycle-accurate");
		case TLM_CAN_TRANSACTION_ACCURATE         : return std::string("transaction-accurate");
		case TLM_CAN_TRANSMISSION_START_INACCURATE: return std::string("transmission-start-inaccurate");
	}
	return std::string("?");
}

template <> inline VariableBase& Variable<tlm_can_model_accuracy>::operator = (bool value)
{
	if(IsMutable())
	{
		tlm_can_model_accuracy tmp = *storage;
		switch((unsigned int) value)
		{
			case TLM_CAN_CYCLE_ACCURATE               :
			case TLM_CAN_TRANSACTION_ACCURATE         :
			case TLM_CAN_TRANSMISSION_START_INACCURATE:
				tmp = (tlm_can_model_accuracy)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> inline VariableBase& Variable<tlm_can_model_accuracy>::operator = (long long value)
{
	if(IsMutable())
	{
		tlm_can_model_accuracy tmp = *storage;
		switch(value)
		{
			case TLM_CAN_CYCLE_ACCURATE               :
			case TLM_CAN_TRANSACTION_ACCURATE         :
			case TLM_CAN_TRANSMISSION_START_INACCURATE:
				tmp = (tlm_can_model_accuracy) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> inline VariableBase& Variable<tlm_can_model_accuracy>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		tlm_can_model_accuracy tmp = *storage;
		switch(value)
		{
			case TLM_CAN_CYCLE_ACCURATE               :
			case TLM_CAN_TRANSACTION_ACCURATE         :
			case TLM_CAN_TRANSMISSION_START_INACCURATE:
				tmp = (tlm_can_model_accuracy) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> inline VariableBase& Variable<tlm_can_model_accuracy>::operator = (double value)
{
	if(IsMutable())
	{
		tlm_can_model_accuracy tmp = *storage;
		switch((unsigned int) value)
		{
			case TLM_CAN_CYCLE_ACCURATE               :
			case TLM_CAN_TRANSACTION_ACCURATE         :
			case TLM_CAN_TRANSMISSION_START_INACCURATE:
				tmp = (tlm_can_model_accuracy)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> inline VariableBase& Variable<tlm_can_model_accuracy>::operator = (const char *value)
{
	if(IsMutable())
	{
		tlm_can_model_accuracy tmp = *storage;
		if(std::string(value) == std::string("cycle-accurate")) tmp = TLM_CAN_CYCLE_ACCURATE;
		else if(std::string(value) == std::string("transaction-accurate")) tmp = TLM_CAN_TRANSACTION_ACCURATE;
		else if(std::string(value) == std::string("transmission-start-inaccurate")) tmp = TLM_CAN_TRANSMISSION_START_INACCURATE;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

} // end of service namespace
} // end of kernel namespace
} // end of unisim namespace

#endif // __UNISIM_KERNEL_TLM2_TLM_CAN_HH__
