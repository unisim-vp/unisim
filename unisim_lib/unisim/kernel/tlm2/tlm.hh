/*
 *  Copyright (c) 2008,
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

#ifndef __UNISIM_KERNEL_TLM2_TLM_HH__
#define __UNISIM_KERNEL_TLM2_TLM_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/hash_table/hash_table.hh>
#include <unisim/util/likely/likely.hh>
#include <systemc.h>
#include <tlm.h>
#include <stack>
#include <vector>
#include <map>

// to remove asap
#include <iostream>
#include <unisim/kernel/debug/debug.hh>

namespace unisim {
namespace kernel {
namespace tlm2 {

class ManagedPayload
{
public:
	ManagedPayload() :
		extensions(tlm::max_num_extensions()),
		memory_manager(0),
		ref_count(0)
	{
	}
	
	explicit ManagedPayload(tlm::tlm_mm_interface *mm) :
		extensions(tlm::max_num_extensions()),
		memory_manager(mm),
		ref_count(0)
	{
	}
	
	void acquire()
	{
		assert(memory_manager != 0);
		ref_count++;
	}

	void release()
	{
		assert(memory_manager != 0);
		if (--ref_count==0)
			memory_manager->free(reinterpret_cast<tlm::tlm_generic_payload *>(this));
	}
	
	int get_ref_count()
	{
		return ref_count;
	}

	void set_mm(tlm::tlm_mm_interface* mm)
	{
		memory_manager = mm;
	}

	bool has_mm()
	{
		return memory_manager != 0;
	}
	
	void reset()
	{
		extensions.free_entire_cache();
	};

	void deep_copy_from(const ManagedPayload & other)
	{
		for(unsigned int i=0; i<other.extensions.size(); i++)
		{
			if(other.extensions[i])
			{
				if(!extensions[i])
				{
					tlm::tlm_extension_base *ext = other.extensions[i]->clone();
					if(ext)
					{
						if(has_mm())
						{
							set_auto_extension(i, ext);
						}
						else
						{
							set_extension(i, ext);
						}
					}
				}
				else
				{
					extensions[i]->copy_from(*other.extensions[i]);
				}
			}
		}
	}
	
	void update_extensions_from(const ManagedPayload & other)
	{
		for(unsigned int i=0; i<other.extensions.size(); i++)
		{
			if(other.extensions[i])
			{
				if(extensions[i])
				{
					extensions[i]->copy_from(*other.extensions[i]);
				}
			}
		}
	}
	
	void free_all_extensions()
	{
		extensions.free_entire_cache();
		for(unsigned int i=0; i<extensions.size(); i++)
		{
			if(extensions[i])
			{
				extensions[i]->free();
				extensions[i] = 0;
			}
		}
	}
	
	
	virtual ~ManagedPayload()
	{
		for(unsigned int i=0; i<extensions.size(); i++)
			if(extensions[i]) extensions[i]->free();
	}
	
	template <typename T> T* set_extension(T* ext)
	{
		return static_cast<T*>(set_extension(T::ID, ext));
	}
	
	tlm::tlm_extension_base* set_extension(unsigned int index, tlm::tlm_extension_base* ext)
	{
		tlm::tlm_extension_base* tmp = extensions[index];
		extensions[index] = ext;
		return tmp;
	}
	
	template <typename T> T* set_auto_extension(T* ext)
	{
		return static_cast<T*>(set_auto_extension(T::ID, ext));
	}
	
	tlm::tlm_extension_base* set_auto_extension(unsigned int index, tlm::tlm_extension_base* ext)
	{
		tlm::tlm_extension_base* tmp = extensions[index];
		extensions[index] = ext;
		if (!tmp) extensions.insert_in_cache(&extensions[index]);
		assert(memory_manager != 0);
		return tmp;
	}
	
	template <typename T> void get_extension(T*& ext) const
	{
		ext = get_extension<T>();
	}
	
	template <typename T> T* get_extension() const
	{
		return static_cast<T*>(get_extension(T::ID));
	}
	
	tlm::tlm_extension_base* get_extension(unsigned int index) const
	{
		return extensions[index];
	}
	
	template <typename T> void clear_extension(const T* ext)
	{
		clear_extension<T>();
	}
	
	template <typename T> void clear_extension()
	{
		clear_extension(T::ID);
	}
	
	template <typename T> void release_extension(T* ext)
	{
		release_extension<T>();
	}
	
	template <typename T> void release_extension()
	{
		release_extension(T::ID);
	}

    void resize_extensions()
    {
        extensions.expand(tlm::max_num_extensions());
    }

protected:

	ManagedPayload(const ManagedPayload& x) :
		extensions(tlm::max_num_extensions())
		, memory_manager(0)
		, ref_count(0)

	{
		for(unsigned int i=0; i<extensions.size(); i++)
		{
			extensions[i] = x.get_extension(i);
		}
	}
	
	ManagedPayload& operator= (const ManagedPayload& x)
	{
		for(unsigned int i=0; i<extensions.size(); i++)
		{
			extensions[i] = x.get_extension(i);
		}
		return (*this);
	}

private:
	void clear_extension(unsigned int index)
	{
		extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
	}
	
	void release_extension(unsigned int index)
	{
		if (memory_manager)
		{
			extensions.insert_in_cache(&extensions[index]);
		}
		else
		{
			extensions[index]->free();
			extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
		}
	}
	
	tlm::tlm_array<tlm::tlm_extension_base*> extensions;
	tlm::tlm_mm_interface* memory_manager;
	unsigned int ref_count;
};

template <typename PAYLOAD>
class PayloadFabric : public tlm::tlm_mm_interface
{
public:
	PayloadFabric()
	{
	}

	~PayloadFabric()
	{
		while(!free_list.empty())
		{
			PAYLOAD *payload = free_list.top();
			free_list.pop();
			delete payload;
		}
	}

	PAYLOAD *allocate()
	{
		PAYLOAD *payload;

		if(!free_list.empty())
		{
			payload = free_list.top();
			free_list.pop();
			assert(payload->get_ref_count() == 0);
			payload->acquire();
			return payload;
		}

		payload = new PAYLOAD();
		payload->set_mm(this);
		payload->acquire();
		return payload;
	}

	void free(tlm::tlm_generic_payload *_payload)
	{
		PAYLOAD *payload = reinterpret_cast<PAYLOAD *>(_payload);
		payload->free_all_extensions();
		free_list.push(payload);
	}
private:
	std::stack<PAYLOAD *, std::vector<PAYLOAD *> > free_list;
};

template <class T>
class SimplePayload : public unisim::kernel::tlm2::ManagedPayload 
{
public:
	SimplePayload() : value() {}
	SimplePayload(const T& _value) : value(_value) {}
	void SetValue(const T& _value) { value = _value; }
	const T& GetValue() const { return value; }
private:
	T value;
};

template <class T>
class SimpleProtocolTypes
{
public:
	typedef SimplePayload<T>  tlm_payload_type;
	typedef tlm::tlm_phase tlm_phase_type;
};

template <class MODULE, class TYPES = tlm::tlm_base_protocol_types>
class FwRedirector : public tlm::tlm_fw_transport_if<TYPES>
{
public:
	FwRedirector(unsigned int initiator_id,
	             MODULE *module,
	             tlm::tlm_sync_enum (MODULE::*cb_nb_transport_fw)(unsigned int, typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&),
	             void (MODULE::*cb_b_transport)(unsigned int, typename TYPES::tlm_payload_type&, sc_core::sc_time&),
	             unsigned int (MODULE::*cb_transport_dbg)(unsigned int, typename TYPES::tlm_payload_type& payload),
	             bool (MODULE::*cb_get_direct_mem_ptr)(unsigned int, typename TYPES::tlm_payload_type& payload, tlm::tlm_dmi& dmi_data)
	);
	
	virtual ~FwRedirector() {};

	virtual tlm::tlm_sync_enum nb_transport_fw(typename TYPES::tlm_payload_type& payload, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t);
	virtual void b_transport(typename TYPES::tlm_payload_type& payload, sc_core::sc_time& t);
	virtual unsigned int transport_dbg(typename TYPES::tlm_payload_type& payload);
	virtual bool get_direct_mem_ptr(typename TYPES::tlm_payload_type& payload, tlm::tlm_dmi& dmi_data);

private:
	unsigned int initiator_id;
	MODULE *module;
	tlm::tlm_sync_enum (MODULE::*cb_nb_transport_fw)(unsigned int, typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&);
	void (MODULE::*cb_b_transport)(unsigned int, typename TYPES::tlm_payload_type&, sc_core::sc_time&);
	unsigned int (MODULE::*cb_transport_dbg)(unsigned int, typename TYPES::tlm_payload_type& payload);
	bool (MODULE::*cb_get_direct_mem_ptr)(unsigned int, typename TYPES::tlm_payload_type& payload, tlm::tlm_dmi& dmi_data);
};

template <class MODULE, class TYPES>
FwRedirector<MODULE, TYPES>::FwRedirector(unsigned int _initiator_id,
                                          MODULE *_module,
                                          tlm::tlm_sync_enum (MODULE::*_cb_nb_transport_fw)(unsigned int, typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&),
                                          void (MODULE::*_cb_b_transport)(unsigned int, typename TYPES::tlm_payload_type&, sc_core::sc_time&),
                                          unsigned int (MODULE::*_cb_transport_dbg)(unsigned int, typename TYPES::tlm_payload_type& trans),
                                          bool (MODULE::*_cb_get_direct_mem_ptr)(unsigned int, typename TYPES::tlm_payload_type& payload, tlm::tlm_dmi& dmi_data))
	: initiator_id(_initiator_id)
	, module(_module)
	, cb_nb_transport_fw(_cb_nb_transport_fw)
	, cb_b_transport(_cb_b_transport)
	, cb_transport_dbg(_cb_transport_dbg)
	, cb_get_direct_mem_ptr(_cb_get_direct_mem_ptr)
{
}

template <class MODULE, class TYPES>
void FwRedirector<MODULE, TYPES>::b_transport(typename TYPES::tlm_payload_type& payload, sc_core::sc_time& t)
{
	(module->*cb_b_transport)(initiator_id, payload, t);
}

template <class MODULE, class TYPES>
tlm::tlm_sync_enum FwRedirector<MODULE, TYPES>::nb_transport_fw(typename TYPES::tlm_payload_type& payload, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t)
{
	return (module->*cb_nb_transport_fw)(initiator_id, payload, phase, t);
}

template <class MODULE, class TYPES>
unsigned int FwRedirector<MODULE, TYPES>::transport_dbg(typename TYPES::tlm_payload_type& payload)
{
	return (module->*cb_transport_dbg)(initiator_id, payload);
}

template <class MODULE, class TYPES>
bool FwRedirector<MODULE, TYPES>::get_direct_mem_ptr(typename TYPES::tlm_payload_type& payload, tlm::tlm_dmi& dmi_data)
{
	return (module->*cb_get_direct_mem_ptr)(initiator_id, payload, dmi_data);
}

template <class MODULE, class TYPES = tlm::tlm_base_protocol_types>
class BwRedirector : public tlm::tlm_bw_transport_if<TYPES>
{
public:
	BwRedirector(unsigned int target_id,
	             MODULE *module,
	             tlm::tlm_sync_enum (MODULE::*cb_nb_transport_bw)(unsigned int, typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&),
	             void (MODULE::*cb_invalidate_direct_mem_ptr)(unsigned int, sc_dt::uint64, sc_dt::uint64)
	);
	
	virtual ~BwRedirector() {}

	virtual tlm::tlm_sync_enum nb_transport_bw(typename TYPES::tlm_payload_type& payload, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

private:
	unsigned int target_id;
	MODULE *module;
	tlm::tlm_sync_enum (MODULE::*cb_nb_transport_bw)(unsigned int, typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&);
	void (MODULE::*cb_invalidate_direct_mem_ptr)(unsigned int, sc_dt::uint64, sc_dt::uint64);
};

template <class MODULE, class TYPES>
BwRedirector<MODULE, TYPES>::BwRedirector(unsigned int _target_id,
                                          MODULE *_module,
                                          tlm::tlm_sync_enum (MODULE::*_cb_nb_transport_bw)(unsigned int, typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&),
                                          void (MODULE::*_cb_invalidate_direct_mem_ptr)(unsigned int, sc_dt::uint64, sc_dt::uint64))
	: target_id(_target_id)
	, module(_module)
	, cb_nb_transport_bw(_cb_nb_transport_bw)
	, cb_invalidate_direct_mem_ptr(_cb_invalidate_direct_mem_ptr)
{
}

template <class MODULE, class TYPES>
tlm::tlm_sync_enum BwRedirector<MODULE, TYPES>::nb_transport_bw(typename TYPES::tlm_payload_type& payload, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t)
{
	return (module->*cb_nb_transport_bw)(target_id, payload, phase, t);
}

template <class MODULE, class TYPES>
void BwRedirector<MODULE, TYPES>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	(module->*cb_invalidate_direct_mem_ptr)(target_id, start_range, end_range);
}

template <class EVENT>
class Schedule
{
public:
	Schedule()
		: schedule()
		, free_list()
		, kernel_event()
	{
	}
	
	~Schedule()
	{
		typename std::multimap<typename EVENT::Key, EVENT *>::iterator it;
		for(it = schedule.begin(); it != schedule.end(); it++)
		{
			EVENT *event = (*it).second;
			
			delete event;
		}
		
		while(!free_list.empty())
		{
			EVENT *event = free_list.top();
			delete event;
			free_list.pop();
		}
	}
	
	void Notify(EVENT *event)
	{
		const sc_time& time_stamp = event->GetTimeStamp();
		schedule.insert(std::pair<typename EVENT::Key, EVENT *>(event->GetKey(), event));
		sc_time t(time_stamp);
		t -= sc_time_stamp();
//		std::cerr << "notify(" << t << ") from" << std::endl << unisim::kernel::debug::BackTrace() << std::endl;
		kernel_event.notify(t);
	}
	
	bool Empty() const
	{
		return schedule.empty();
	}
	
	EVENT *GetNextEvent()
	{
		if(schedule.empty()) return 0;
		
		typename std::multimap<typename EVENT::Key, EVENT *>::iterator it = schedule.begin();
		const sc_time& time_stamp = sc_time_stamp();
		const sc_time& event_time_stamp = (*it).first.GetTimeStamp();
		if(event_time_stamp <= time_stamp)
		{
			EVENT *event = (*it).second;
			schedule.erase(it);
			return event;
		}
		
		sc_time t(event_time_stamp);
		t -= time_stamp;
		kernel_event.notify(t);
		
		return 0;
	}
	
	EVENT *AllocEvent()
	{
		if(!free_list.empty())
		{
			EVENT *event = free_list.top();
			free_list.pop();
			return event;
		}
		
		return new EVENT();
	}

	void FreeEvent(EVENT *event)
	{
		event->Clear();
		free_list.push(event);
	}
	
	const sc_event& GetKernelEvent() const
	{
		return kernel_event;
	}

	void Clear()
	{
		typename std::multimap<typename EVENT::Key, EVENT *>::iterator it;
		for(it = schedule.begin(); it != schedule.end(); it++)
		{
			EVENT *event = (*it).second;
			
			FreeEvent(event);
		}
		schedule.clear();
		kernel_event.cancel();
	}

private:
	std::multimap<typename EVENT::Key, EVENT *> schedule;
	std::stack<EVENT *, std::vector<EVENT *> > free_list;
	sc_event kernel_event;
};

template <unsigned int BUSWIDTH = 32, class TYPES = tlm::tlm_base_protocol_types>
class InitiatorStub
	: public virtual unisim::kernel::service::Object
	, public sc_module
	, tlm::tlm_bw_transport_if<TYPES>
{
public:
	tlm::tlm_initiator_socket<BUSWIDTH, TYPES> master_sock;
	
	InitiatorStub(const sc_module_name& name, unisim::kernel::service::Object *parent = 0);

	virtual tlm::tlm_sync_enum nb_transport_bw(typename TYPES::tlm_payload_type& trans, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
private:
	unisim::kernel::logger::Logger logger;
	bool enable;
	bool verbose;
	unisim::kernel::service::Parameter<bool> param_enable;
	unisim::kernel::service::Parameter<bool> param_verbose;
};

template <unsigned int BUSWIDTH, class TYPES>
InitiatorStub<BUSWIDTH, TYPES>::InitiatorStub(const sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent, "An initiator stub")
	, sc_module(name)
	, master_sock("master-sock")
	, logger(*this)
	, enable(true)
	, verbose(false)
	, param_enable("enable", this, enable, "Enable/Disable a lazy implementation of TLM 2.0 method interface")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	master_sock(*this);
}

template <unsigned int BUSWIDTH, class TYPES>
tlm::tlm_sync_enum InitiatorStub<BUSWIDTH, TYPES>::nb_transport_bw(typename TYPES::tlm_payload_type& trans, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "nb_transport_bw has been called" << unisim::kernel::logger::EndDebugInfo;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "nb_transport_bw shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUSWIDTH, class TYPES>
void InitiatorStub<BUSWIDTH, TYPES>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "invalidate_direct_mem_ptr has been called" << unisim::kernel::logger::EndDebugInfo;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "invalidate_direct_mem_ptr shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
}

template <unsigned int BUSWIDTH = 32, class TYPES = tlm::tlm_base_protocol_types>
class TargetStub
	: public virtual unisim::kernel::service::Object
	, public sc_module
	, tlm::tlm_fw_transport_if<TYPES>
{
public:
	tlm::tlm_target_socket<BUSWIDTH, TYPES> slave_sock;
	
	TargetStub(const sc_module_name& name, unisim::kernel::service::Object *parent = 0);

	virtual void b_transport(typename TYPES::tlm_payload_type& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(typename TYPES::tlm_payload_type& trans, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t);
	virtual unsigned int transport_dbg(typename TYPES::tlm_payload_type& trans);
	virtual bool get_direct_mem_ptr(typename TYPES::tlm_payload_type& trans, tlm::tlm_dmi& dmi_data);
private:
	unisim::kernel::logger::Logger logger;
	bool enable;
	bool verbose;
	unisim::kernel::service::Parameter<bool> param_enable;
	unisim::kernel::service::Parameter<bool> param_verbose;
};

template <unsigned int BUSWIDTH, class TYPES>
TargetStub<BUSWIDTH, TYPES>::TargetStub(const sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent, "A target stub")
	, sc_module(name)
	, slave_sock("slave-sock")
	, logger(*this)
	, enable(true)
	, verbose(false)
	, param_enable("enable", this, enable, "Enable/Disable a lazy implementation of TLM 2.0 method interface")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	slave_sock(*this);
}

inline unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& logger, const tlm::tlm_generic_payload& trans)
{
	unsigned char *data_ptr = trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();
	unsigned int i;
	logger << "[";
	for(i = 0; i < data_length; i++)
	{
		logger << "0x" << std::hex << (unsigned int) data_ptr[i] << std::dec;
		if(i != (data_length - 1)) logger << " ";
	}
	logger << "]";
	return logger;
}

template <class T>
inline unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& logger, const SimplePayload<T>& trans)
{
	return logger << trans.GetValue();
}

template <unsigned int BUSWIDTH, class TYPES>
void TargetStub<BUSWIDTH, TYPES>::b_transport(typename TYPES::tlm_payload_type& trans, sc_core::sc_time& t)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "b_transport(" << trans << ", " << (sc_time_stamp() + t).to_string() << ")" << unisim::kernel::logger::EndDebugInfo;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "b_transport shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
}

template <unsigned int BUSWIDTH, class TYPES>
tlm::tlm_sync_enum TargetStub<BUSWIDTH, TYPES>::nb_transport_fw(typename TYPES::tlm_payload_type& trans, typename TYPES::tlm_phase_type& phase, sc_core::sc_time& t)
{
	if(enable)
	{
		if(verbose)
		{
			std::stringstream sstr_phase;
			sstr_phase << phase;
			logger << unisim::kernel::logger::DebugInfo << "nb_transport_fw(" << trans << ", " << sstr_phase.str() << ", " << (sc_time_stamp() + t).to_string() << ")" << unisim::kernel::logger::EndDebugInfo;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "nb_transport_fw shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUSWIDTH, class TYPES>
unsigned int TargetStub<BUSWIDTH, TYPES>::transport_dbg(typename TYPES::tlm_payload_type& trans)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "transport_dbg has been called" << unisim::kernel::logger::EndDebugInfo;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "transport_dbg shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return 0;
}
	
template <unsigned int BUSWIDTH, class TYPES>
bool TargetStub<BUSWIDTH, TYPES>::get_direct_mem_ptr(typename TYPES::tlm_payload_type& trans, tlm::tlm_dmi& dmi_data)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "get_direct_mem_ptr has been called" << unisim::kernel::logger::EndDebugInfo;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "get_direct_mem_ptr shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return false;
}

template <unsigned int BUSWIDTH>
class TargetStub<BUSWIDTH, tlm::tlm_base_protocol_types>
	: public virtual unisim::kernel::service::Object
	, public sc_module
	, tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
{
public:
	tlm::tlm_target_socket<BUSWIDTH, tlm::tlm_base_protocol_types> slave_sock;
	
	TargetStub(const sc_module_name& name, unisim::kernel::service::Object *parent = 0);

	virtual void b_transport(typename tlm::tlm_generic_payload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(typename tlm::tlm_generic_payload& trans, typename tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual unsigned int transport_dbg(typename tlm::tlm_generic_payload& trans);
	virtual bool get_direct_mem_ptr(typename tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
private:
	unisim::kernel::logger::Logger logger;
	bool enable;
	bool verbose;
	unisim::kernel::service::Parameter<bool> param_enable;
	unisim::kernel::service::Parameter<bool> param_verbose;
};

template <unsigned int BUSWIDTH>
TargetStub<BUSWIDTH, tlm::tlm_base_protocol_types>::TargetStub(const sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent, "A target stub")
	, sc_module(name)
	, slave_sock("slave-sock")
	, logger(*this)
	, enable(true)
	, verbose(false)
	, param_enable("enable", this, enable, "Enable/Disable a lazy implementation of TLM 2.0 method interface")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	slave_sock(*this);
}

template <unsigned int BUSWIDTH>
void TargetStub<BUSWIDTH, tlm::tlm_base_protocol_types>::b_transport(typename tlm::tlm_generic_payload& trans, sc_core::sc_time& t)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "b_transport has been called" << unisim::kernel::logger::EndDebugInfo;
		}
		if(trans.get_command() == tlm::TLM_READ_COMMAND)
		{
			memset(trans.get_data_ptr(), 0, trans.get_data_length());
		}
		trans.set_dmi_allowed(false);
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "b_transport shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
}

template <unsigned int BUSWIDTH>
tlm::tlm_sync_enum TargetStub<BUSWIDTH, tlm::tlm_base_protocol_types>::nb_transport_fw(typename tlm::tlm_generic_payload& trans, typename tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "nb_transport_fw has been called" << unisim::kernel::logger::EndDebugInfo;
		}
		if(trans.get_command() == tlm::TLM_READ_COMMAND)
		{
			memset(trans.get_data_ptr(), 0, trans.get_data_length());
		}
		trans.set_dmi_allowed(false);
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "nb_transport_fw shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUSWIDTH>
unsigned int TargetStub<BUSWIDTH, tlm::tlm_base_protocol_types>::transport_dbg(typename tlm::tlm_generic_payload& trans)
{
	unsigned int length = 0;
	
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "transport_dbg has been called" << unisim::kernel::logger::EndDebugInfo;
		}
		length = trans.get_data_length();
		if(trans.get_command() == tlm::TLM_READ_COMMAND)
		{
			memset(trans.get_data_ptr(), 0, length);
		}
		trans.set_dmi_allowed(false);
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "transport_dbg shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return length;
}
	
template <unsigned int BUSWIDTH>
bool TargetStub<BUSWIDTH, tlm::tlm_base_protocol_types>::get_direct_mem_ptr(typename tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "get_direct_mem_ptr has been called" << unisim::kernel::logger::EndDebugInfo;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "get_direct_mem_ptr shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return false;
}

typedef enum 
{
	DMI_ALLOW,
	DMI_DENY
} DMIGrant;

class LatencyLookupTable
{
public:
	inline LatencyLookupTable();
	inline LatencyLookupTable(const sc_time& base_lat);
	inline ~LatencyLookupTable();
	inline void SetBaseLatency(const sc_time& base_lat);
	inline const sc_time& Lookup(unsigned int n); // returns n * base_lat
private:
	static const unsigned int NUM_LATENCY_FAST_LOOKUP = 16; // 0 <= n < 16 for fast lookup
	sc_time base_lat;
	sc_time latency_fast_lookup[NUM_LATENCY_FAST_LOOKUP];
	std::map<unsigned int, sc_time> latency_slow_lookup;
};

LatencyLookupTable::LatencyLookupTable()
	: base_lat(SC_ZERO_TIME)
{
}

LatencyLookupTable::LatencyLookupTable(const sc_time& _base_lat)
{
	SetBaseLatency(_base_lat);
}

LatencyLookupTable::~LatencyLookupTable()
{
}

inline void LatencyLookupTable::SetBaseLatency(const sc_time& _base_lat)
{
	base_lat = _base_lat;
	
	unsigned int n;
	for(n = 0; n < NUM_LATENCY_FAST_LOOKUP; n++)
	{
		latency_fast_lookup[n] = n * base_lat;
	}
	
	latency_slow_lookup.clear();
}

const sc_time& LatencyLookupTable::Lookup(unsigned int n)
{
	if(n < NUM_LATENCY_FAST_LOOKUP)
	{
		return latency_fast_lookup[n];
	}
	
	do
	{
		std::map<unsigned int, sc_time>::iterator iter = latency_slow_lookup.find(n);
		
		if(iter != latency_slow_lookup.end())
		{
			return (*iter).second;
		}
		
		sc_time latency = n * base_lat;
		latency_slow_lookup[n] = latency;
	}
	while(1);
	
	return latency_fast_lookup[0]; // shall never occur
}

class DMIRegionCache;

class DMIRegion
{
public:
	inline DMIRegion(DMIGrant dmi_grant, tlm::tlm_dmi *dmi_data);
	inline ~DMIRegion();
	inline bool IsAllowed() const;
	inline bool IsDenied() const;
	inline DMIGrant GetGrant() const;
	inline tlm::tlm_dmi *GetDMI() const;
	inline const sc_time& GetReadLatency(unsigned int data_length);
	inline const sc_time& GetWriteLatency(unsigned int data_length);
private:
	friend class DMIRegionCache;
	DMIGrant dmi_grant;
	tlm::tlm_dmi *dmi_data;
	LatencyLookupTable read_lat_lut;
	LatencyLookupTable write_lat_lut;
	DMIRegion *next;
};

inline DMIRegion::DMIRegion(DMIGrant _dmi_grant, tlm::tlm_dmi *_dmi_data)
	: dmi_grant(_dmi_grant)
	, dmi_data(_dmi_data)
	, read_lat_lut(dmi_data->get_read_latency())
	, write_lat_lut(dmi_data->get_write_latency())
	, next(NULL)
{
}

inline DMIRegion::~DMIRegion()
{
	delete dmi_data;
}

inline bool DMIRegion::IsAllowed() const
{
	return dmi_grant == DMI_ALLOW;
}

inline bool DMIRegion::IsDenied() const
{
	return dmi_grant == DMI_DENY;
}

inline DMIGrant DMIRegion::GetGrant() const
{
	return dmi_grant;
}

inline tlm::tlm_dmi *DMIRegion::GetDMI() const
{
	return dmi_data;
}

inline const sc_time& DMIRegion::GetReadLatency(unsigned int data_length)
{
	return read_lat_lut.Lookup(data_length);
}

inline const sc_time& DMIRegion::GetWriteLatency(unsigned int data_length)
{
	return write_lat_lut.Lookup(data_length);
}

class DMIRegionCache
{
public:
	inline DMIRegionCache();
	inline ~DMIRegionCache();
	
	inline DMIRegion *Lookup(sc_dt::uint64 addr, sc_dt::uint64 size);
	inline void Insert(DMIGrant dmi_grant, tlm::tlm_dmi *dmi_data);
	inline void Insert(DMIRegion *dmi_region);
	inline void Remove(DMIRegion *dmi_region);
	inline void Invalidate(sc_dt::uint64 start_range = 0, sc_dt::uint64 end_range = (sc_dt::uint64) -1);
private:
	DMIRegion *mru_dmi_region;
};

inline DMIRegionCache::DMIRegionCache()
	: mru_dmi_region(0)
{
}

inline DMIRegionCache::~DMIRegionCache()
{
	Invalidate();
}

inline DMIRegion *DMIRegionCache::Lookup(sc_dt::uint64 addr, sc_dt::uint64 size)
{
	sc_dt::uint64 end_addr = addr + size - 1;
	
	DMIRegion *cur = mru_dmi_region;
	if(cur)
	{
		tlm::tlm_dmi *dmi_data = cur->GetDMI();
		sc_dt::uint64 dmi_start_address = dmi_data->get_start_address();
		sc_dt::uint64 dmi_end_address = dmi_data->get_end_address();
		
		if((addr >= dmi_start_address) && (end_addr <= dmi_end_address))
		{
			return cur;
		}
		DMIRegion *prev = cur;
		cur = cur->next;
		if(cur)
		{
			do
			{
				tlm::tlm_dmi *dmi_data = cur->GetDMI();
				sc_dt::uint64 dmi_start_address = dmi_data->get_start_address();
				sc_dt::uint64 dmi_end_address = dmi_data->get_end_address();
				
				if((addr >= dmi_start_address) && (end_addr <= dmi_end_address))
				{
					prev->next = cur->next;
					cur->next= mru_dmi_region;
					mru_dmi_region = cur;
					return cur;
				}
				prev = cur;
			} while((cur = cur->next) != 0);
		}
	}
	
	return 0;
}

inline void DMIRegionCache::Insert(DMIGrant dmi_grant, tlm::tlm_dmi *dmi_data)
{
	DMIRegion *dmi_region = new DMIRegion(dmi_grant, dmi_data);
	dmi_region->next = mru_dmi_region;
	mru_dmi_region = dmi_region;
}

inline void DMIRegionCache::Insert(DMIRegion *dmi_region)
{
	dmi_region->next = mru_dmi_region;
	mru_dmi_region = dmi_region;
}

inline void DMIRegionCache::Remove(DMIRegion *dmi_region)
{
	DMIRegion *prev;
	DMIRegion *cur = mru_dmi_region;
	
	if(cur)
	{
		if(cur == dmi_region)
		{
			mru_dmi_region = cur->next;
			return;
		}
		prev = cur;
		cur = cur->next;
		if(cur)
		{
			do
			{
				if(cur == dmi_region)
				{
					prev->next = cur->next;
					cur->next = 0;
					return;
				}
				prev = cur;
			} while((cur = cur->next) != 0);
		}
	}
}

inline void DMIRegionCache::Invalidate(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	if(end_range < start_range) return;
	
	DMIRegion *dmi_region, *next_dmi_region;
	
	dmi_region = mru_dmi_region;
	if(dmi_region)
	{
		do
		{
			next_dmi_region = dmi_region->next;
			
			tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
			sc_dt::uint64 dmi_start_address = dmi_data->get_start_address();
			sc_dt::uint64 dmi_end_address = dmi_data->get_end_address();
			if((end_range >= dmi_start_address) && (start_range <= dmi_end_address))
			{
				// collision
				if(start_range <= dmi_start_address)
				{
					if(end_range >= dmi_end_address)
					{
						// full delete
						Remove(dmi_region);
						delete dmi_region;
					}
					else
					{
						// cut lower range of region
						unsigned char *dmi_ptr = dmi_data->get_dmi_ptr();
						sc_dt::uint64 new_dmi_start_address = end_range + 1;
						unsigned char *new_dmi_ptr = dmi_ptr + (new_dmi_start_address - dmi_start_address);
						dmi_data->set_start_address(new_dmi_start_address);
						dmi_data->set_dmi_ptr(new_dmi_ptr);
					}
				}
				else
				{
					if(end_range >= dmi_end_address)
					{
						// cut upper range of region
						sc_dt::uint64 new_dmi_end_address = start_range - 1;
						dmi_data->set_end_address(new_dmi_end_address);
					}
					else
					{
						// split in two regions
						DMIGrant dmi_grant = dmi_region->GetGrant();
						tlm::tlm_dmi::dmi_access_e dmi_access = dmi_data->get_granted_access();
						sc_core::sc_time dmi_read_latency = dmi_data->get_read_latency();
						sc_core::sc_time dmi_write_latency = dmi_data->get_write_latency();
						unsigned char *dmi_ptr = dmi_data->get_dmi_ptr();
						
						Remove(dmi_region);
						delete dmi_region;
						
						// lower region
						sc_dt::uint64 dmi_start_address1 = dmi_start_address;
						sc_dt::uint64 dmi_end_address1 = start_range - 1;
						
						tlm::tlm_dmi *dmi_data1 = new tlm::tlm_dmi();
						dmi_data1->set_granted_access(dmi_access);
						dmi_data1->set_read_latency(dmi_read_latency);
						dmi_data1->set_write_latency(dmi_write_latency);
						dmi_data1->set_start_address(dmi_start_address1);
						dmi_data1->set_end_address(dmi_end_address1);
						dmi_data1->set_dmi_ptr(dmi_ptr);
						DMIRegion *dmi_region1 = new DMIRegion(dmi_grant, dmi_data1);
						Insert(dmi_region1);

						// upper region
						sc_dt::uint64 dmi_start_address2 = end_range + 1;
						sc_dt::uint64 dmi_end_address2 = dmi_end_address;
						
						tlm::tlm_dmi *dmi_data2 = new tlm::tlm_dmi();
						dmi_data2->set_granted_access(dmi_access);
						dmi_data2->set_read_latency(dmi_read_latency);
						dmi_data2->set_write_latency(dmi_write_latency);
						dmi_data2->set_start_address(dmi_start_address2);
						dmi_data2->set_end_address(dmi_end_address2);
						dmi_data1->set_dmi_ptr(dmi_ptr + (dmi_start_address2 - dmi_start_address));
						DMIRegion *dmi_region2 = new DMIRegion(dmi_grant, dmi_data2);
						Insert(dmi_region2);
					}
				}
			}
			dmi_region = next_dmi_region;
		} while(dmi_region);
	}
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_TLM_HH__
