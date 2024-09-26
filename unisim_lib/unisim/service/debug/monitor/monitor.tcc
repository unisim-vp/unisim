/*
 *  Copyright (c) 2016,
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
 
#ifndef __UNISIM_SERVICE_DEBUG_MONITOR_MONITOR_TCC__
#define __UNISIM_SERVICE_DEBUG_MONITOR_MONITOR_TCC__

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/stack_frame.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>

extern "C" int __u_start(int argc, char **argv, int (*callback)(void));
extern "C" int __u_set_breakpoint(const char *info, void (*callback)(int));
extern "C" int __u_set_watchpoint(const char *info, void (*callback)(int));
extern "C" int __u_delete_breakpoint(int);
extern "C" int __u_delete_watchpoint(int);
extern "C" int __u_lookup_data_object(const char *info, size_t size);
extern "C" int __u_val(int handler, void *value);
extern "C" int __u_stop(int);

namespace unisim {
namespace service {
namespace debug {
namespace monitor {

template <typename ADDRESS>
SourceCodeBreakpoint<ADDRESS>::SourceCodeBreakpoint(const unisim::util::debug::SourceCodeLocation& _source_code_location, typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *_debug_event_trigger_if, int _handle, void (*_callback)(int))
	: debug_event_trigger_if(_debug_event_trigger_if)
	, source_code_breakpoint(_debug_event_trigger_if->CreateSourceCodeBreakpoint(_source_code_location))
	, source_code_breakpoint_set(false)
	, handle(_handle)
	, callback(_callback)
{
	if(source_code_breakpoint)
	{
		source_code_breakpoint->Catch();
		source_code_breakpoint->AddEventListener(this);
	}
}

template <typename ADDRESS>
SourceCodeBreakpoint<ADDRESS>::~SourceCodeBreakpoint()
{
	if(source_code_breakpoint)
	{
		Unset();
		source_code_breakpoint->Release();
	}
}

template <typename ADDRESS>
bool SourceCodeBreakpoint<ADDRESS>::Set()
{
	return Update(true);
}

template <typename ADDRESS>
bool SourceCodeBreakpoint<ADDRESS>::Unset()
{
	return Update(false);
}

template <typename ADDRESS>
bool SourceCodeBreakpoint<ADDRESS>::Update(bool set)
{
	if(!source_code_breakpoint) return false;
	
	if(set && !source_code_breakpoint_set)
	{
		if(!debug_event_trigger_if->Listen(source_code_breakpoint)) return false;
	}
	else if(!set && source_code_breakpoint_set)
	{
		if(!debug_event_trigger_if->Unlisten(source_code_breakpoint)) return false;
	}
	source_code_breakpoint_set = set;
	return true;
}

template <typename ADDRESS>
void SourceCodeBreakpoint<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	(*callback)(handle);
}

template <typename ADDRESS>
int SourceCodeBreakpoint<ADDRESS>::GetHandle() const
{
	return handle;
}

template <typename ADDRESS>
DataObjectWatchpoint<ADDRESS>::DataObjectWatchpoint(const char *data_location, typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *_debug_event_trigger_if, typename unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if, int _handle, void (*_callback)(int))
	: handle(_handle)
	, callback(_callback)
	, debug_event_trigger_if(_debug_event_trigger_if)
	, watchpoint(0)
	, watchpoint_set(false)
{
	const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_if->FindSymbolByName(data_location, unisim::util::debug::SymbolBase::SYM_OBJECT);
	
	if(symbol)
	{
		ADDRESS watchpoint_addr = symbol->GetAddress();
		ADDRESS watchpoint_size = symbol->GetSize();
		
		watchpoint = debug_event_trigger_if->CreateWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, watchpoint_addr, watchpoint_size, /* overlook */ true);
		watchpoint->Catch();
		watchpoint->AddEventListener(this);
	}
}

template <typename ADDRESS>
DataObjectWatchpoint<ADDRESS>::~DataObjectWatchpoint()
{
	if(watchpoint)
	{
		Unset();
		watchpoint->Release();
	}
}

template <typename ADDRESS>
int DataObjectWatchpoint<ADDRESS>::GetHandle() const
{
	return handle;
}

template <typename ADDRESS>
bool DataObjectWatchpoint<ADDRESS>::Set()
{
	return Update(true);
}

template <typename ADDRESS>
bool DataObjectWatchpoint<ADDRESS>::Unset()
{
	return Update(false);
}

template <typename ADDRESS>
bool DataObjectWatchpoint<ADDRESS>::Update(bool set)
{
	if(!watchpoint) return false;
	
	if(set && !watchpoint_set)
	{
		if(!debug_event_trigger_if->Listen(watchpoint)) return false;
	}
	else if(!set && watchpoint_set)
	{
		if(!debug_event_trigger_if->Unlisten(watchpoint)) return false;
	}
	watchpoint_set = set;
	return true;
}

template <typename ADDRESS>
void DataObjectWatchpoint<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	(*callback)(handle);
}

template <typename ADDRESS>
DataObject<ADDRESS>::DataObject(const char *data_location, size_t _size, typename unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if, int _handle)
	: handle(_handle)
	, size(_size)
	, data_object(data_object_lookup_if->FindDataObject(data_location))
{
}

template <typename ADDRESS>
DataObject<ADDRESS>::~DataObject()
{
}

template <typename ADDRESS>
bool DataObject<ADDRESS>::Exists() const
{
	return data_object.IsDefined() && data_object.Exists();
}

template <typename ADDRESS>
bool DataObject<ADDRESS>::IsOptimizedOut() const
{
	return data_object.IsDefined() && data_object.IsOptimizedOut();
}

template <typename ADDRESS>
bool DataObject<ADDRESS>::GetValue(void *value) const
{
	if(data_object.IsDefined())
	{
		if(data_object.Exists())
		{
			if(!IsOptimizedOut())
			{
				if(data_object.Read(0, value, 0, size * 8))
				{
					return true;
				}
				else
				{
					std::cerr << "data object \"" << data_object.GetName() << "\" can't be read" << std::endl;
				}
			}
			else
			{
				std::cerr << "data object \"" << data_object.GetName() << "\" is optimized out" << std::endl;
			}
		}
		else
		{
			std::cerr << "data object \"" << data_object.GetName() << "\" does not exist" << std::endl;
		}
	}
	
	return false;
}

template <typename ADDRESS>
Monitor<ADDRESS>::Monitor(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, MonitorBase()
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::StackFrame<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(name, parent)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_event_trigger_import("debug-event", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, stack_frame_import("backtrace-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, data_object_lookup_import("data-object-lookup-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, logger(*this)
	, source_code_breakpoints()
	, data_object_watchpoints()
	, tracked_data_objects()
{
}

template <typename ADDRESS>
Monitor<ADDRESS>::~Monitor()
{
	typename std::vector<SourceCodeBreakpoint<ADDRESS> *>::size_type num_source_code_breakpoints = source_code_breakpoints.size();
	typename std::vector<SourceCodeBreakpoint<ADDRESS> *>::size_type source_code_breakpoint_num;
	
	for(source_code_breakpoint_num = 0; source_code_breakpoint_num < num_source_code_breakpoints; source_code_breakpoint_num++)
	{
		SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = source_code_breakpoints[source_code_breakpoint_num];
		
		if(source_code_breakpoint)
		{
			delete source_code_breakpoint;
		}
	}

	typename std::vector<DataObjectWatchpoint<ADDRESS> *>::size_type num_data_object_watchpoints = data_object_watchpoints.size();
	typename std::vector<DataObjectWatchpoint<ADDRESS> *>::size_type data_object_watchpoint_num;
	
	for(data_object_watchpoint_num = 0; data_object_watchpoint_num < num_data_object_watchpoints; data_object_watchpoint_num++)
	{
		DataObjectWatchpoint<ADDRESS> *data_object_watchpoint = data_object_watchpoints[data_object_watchpoint_num];
		
		if(data_object_watchpoint)
		{
			delete data_object_watchpoint;
		}
	}

	typename std::vector<DataObject<ADDRESS> *>::size_type num_tracked_data_objects = tracked_data_objects.size();
	typename std::vector<DataObject<ADDRESS> *>::size_type tracked_data_object_num;
	
	for(tracked_data_object_num = 0; tracked_data_object_num < num_tracked_data_objects; tracked_data_object_num++)
	{
		DataObject<ADDRESS> *tracked_data_object = tracked_data_objects[tracked_data_object_num];
		
		delete tracked_data_object;
	}
}

template <typename ADDRESS>
bool Monitor<ADDRESS>::EndSetup()
{
	return Start();
}

template <typename ADDRESS>
int Monitor<ADDRESS>::SetBreakpoint(const char *info, void (*callback)(int))
{
	int handle = source_code_breakpoints.size() + 1;
	unisim::util::debug::SourceCodeLocation source_code_location;
	if(source_code_location.Parse(info))
	{
		SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = new SourceCodeBreakpoint<ADDRESS>(source_code_location, debug_event_trigger_import, handle, callback);
		if(source_code_breakpoint->Set())
		{
			source_code_breakpoints.push_back(source_code_breakpoint);
			return handle;
		}
		
		delete source_code_breakpoint;
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::SetWatchpoint(const char *info, void (*callback)(int))
{
	int handle = data_object_watchpoints.size() + 1;
	DataObjectWatchpoint<ADDRESS> *data_object_watchpoint = new DataObjectWatchpoint<ADDRESS>(info, debug_event_trigger_import, symbol_table_lookup_import, handle, callback);
	
	if(data_object_watchpoint)
	{
		if(data_object_watchpoint->Set())
		{
			data_object_watchpoints.push_back(data_object_watchpoint);
			return handle;
		}
		
		delete data_object_watchpoint;
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::DeleteBreakpoint(int handle)
{
	if(handle > 0)
	{
		int idx = handle - 1;
	
		if((typename std::vector<SourceCodeBreakpoint<ADDRESS> *>::size_type) idx < source_code_breakpoints.size())
		{
			SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = source_code_breakpoints[idx];
			
			if(source_code_breakpoint && source_code_breakpoint->Unset())
			{
				delete source_code_breakpoint;
				source_code_breakpoints[idx] = 0;
				return 0;
			}
		}
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::DeleteWatchpoint(int handle)
{
	if(handle > 0)
	{
		int idx = handle - 1;
	
		if((typename std::vector<DataObjectWatchpoint<ADDRESS> *>::size_type) idx < data_object_watchpoints.size())
		{
			DataObjectWatchpoint<ADDRESS> *data_object_watchpoint = data_object_watchpoints[idx];
			
			if(data_object_watchpoint && data_object_watchpoint->Unset())
			{
				delete data_object_watchpoint;
				data_object_watchpoints[idx] = 0;
				return 0;
			}
		}
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::LookupDataObject(const char *info, size_t size)
{
	int handle = tracked_data_objects.size() + 1;

	DataObject<ADDRESS> *data_object = new DataObject<ADDRESS>(info, size, data_object_lookup_import, handle);
	
	if(data_object->Exists())
	{
		tracked_data_objects.push_back(data_object);
		
		return handle;
	}
	else
	{
		delete data_object;
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::GetDataObjectValue(int handle, void *value)
{
	if(handle > 0)
	{
		int idx = handle - 1;
	
		if((typename std::vector<DataObject<ADDRESS> *>::size_type) idx < tracked_data_objects.size())
		{
			DataObject<ADDRESS> *tracked_data_object = tracked_data_objects[idx];
			
			if(tracked_data_object)
			{
				if(tracked_data_object->GetValue(value))
				{
					return 0;
				}
			}
		}
		else
		{
			logger << unisim::kernel::logger::DebugWarning << "invalid data object handle" << unisim::kernel::logger::EndDebugWarning;
		}
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::StopMe(int exit_status)
{
	Object::Stop(exit_status);
	
	return -1;
}

template <typename ADDRESS>
void Monitor<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	struct Bad {};
	throw Bad();
}

} // end of namespace monitor
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
