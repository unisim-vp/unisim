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
		kernel_event.notify(t);
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
	: unisim::kernel::service::Object(name, parent)
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
	: unisim::kernel::service::Object(name, parent)
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

template <unsigned int BUSWIDTH, class TYPES>
void TargetStub<BUSWIDTH, TYPES>::b_transport(typename TYPES::tlm_payload_type& trans, sc_core::sc_time& t)
{
	if(enable)
	{
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << "b_transport has been called" << unisim::kernel::logger::EndDebugInfo;
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
			logger << unisim::kernel::logger::DebugInfo << "nb_transport_fw has been called" << unisim::kernel::logger::EndDebugInfo;
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
	: unisim::kernel::service::Object(name, parent)
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

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_TLM_HH__
