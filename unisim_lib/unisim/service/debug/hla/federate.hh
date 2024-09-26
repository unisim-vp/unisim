/*
 *  Copyright (c) 2022,
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

#ifndef __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_HH__
#define __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_HH__

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if HAVE_HLA_RTI1516E

#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/stack_frame.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>
#include <unisim/service/interfaces/stubbing.hh>
#include <unisim/service/interfaces/hooking.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
#include <unisim/service/interfaces/debug_timing.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/debug/source_code_breakpoint.hh>
#include <unisim/util/ieee754/ieee754.hh>
#include <unisim/util/json/json.hh>
#include <unisim/util/hla/hla.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <string>
#include <set>
#include <map>

namespace unisim {
namespace service {
namespace debug {
namespace hla {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

///////////////////////////////////////////////////////////////////////////////
//                          Forward declarations                             //
///////////////////////////////////////////////////////////////////////////////

template <typename CONFIG> struct Federate;
struct ObjectClass;
struct InstrumentBase;
template <typename CONFIG, typename TYPE> struct Instrument;
template <typename CONFIG> struct FederateHookStub;
template <typename CONFIG> struct FederateHook;
template <typename CONFIG> struct FederateStub;

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////// Federate<> //////////////////////////////////

template <typename CONFIG>
struct Federate
	: unisim::util::hla::Federate<typename CONFIG::TIME_TRAIT>
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::DebugSelecting>
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Disassembly<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Registers>
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::StackFrame<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Profiling<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Stubbing<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Hooking<typename CONFIG::ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::DebugTiming<typename CONFIG::TIME_TRAIT::TIME_TYPE> >
{
	typedef unisim::util::hla::Federate<typename CONFIG::TIME_TRAIT> Super;
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                  debug_yielding_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> >   debug_event_listener_export;
	
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>           debug_yielding_request_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugSelecting>                 debug_selecting_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >    debug_event_trigger_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >          disasm_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >               memory_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>                      registers_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >    symbol_table_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StatementLookup<ADDRESS> >      stmt_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StackFrame<ADDRESS> >            stack_frame_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Profiling<ADDRESS> >            profiling_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugInfoLoading>               debug_info_loading_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >     data_object_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >     subprogram_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Stubbing<ADDRESS> >             stubbing_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Hooking<ADDRESS> >              hooking_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugTiming<typename CONFIG::TIME_TRAIT::TIME_TYPE> > debug_timing_import;
	
	Federate(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Federate();

	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();
	
	// unisim::service::interfaces::DebugEventListener<ADDRESS>
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);

	// unisim::kernel::Object
	virtual bool BeginSetup();
	virtual bool EndSetup();
	virtual void Kill();
	bool Killed() const;
	
private:
	friend struct FederateHookStub<CONFIG>;
	friend struct FederateHook<CONFIG>;
	friend struct FederateStub<CONFIG>;
	friend struct Instrument<CONFIG, uint8_t>;
	friend struct Instrument<CONFIG, uint16_t>;
	friend struct Instrument<CONFIG, uint32_t>;
	friend struct Instrument<CONFIG, uint64_t>;
	friend struct Instrument<CONFIG, int8_t>;
	friend struct Instrument<CONFIG, int16_t>;
	friend struct Instrument<CONFIG, int32_t>;
	friend struct Instrument<CONFIG, int64_t>;
	friend struct Instrument<CONFIG, float>;
	friend struct Instrument<CONFIG, double>;
	friend struct Instrument<CONFIG, bool>;
	
	unisim::kernel::logger::Logger logger;
	unisim::util::json::JSON_Value *p_config;
	int prc_num;
	
	std::string config_file;
	unisim::kernel::variable::Parameter<std::string> param_config_file;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<bool> param_debug;

	typedef std::vector<ObjectClass *> ObjectClasses;
	ObjectClasses object_classes;
	typedef std::vector<unisim::util::hla::ObjectInstance *> ObjectInstances;
	ObjectInstances object_instances;
	typedef std::vector<FederateHook<CONFIG> *> FederateHooks;
	FederateHooks federate_hooks;
	typedef std::vector<FederateStub<CONFIG> *> FederateStubs;
	FederateStubs federate_stubs;
	
	bool HookConflicts(FederateHook<CONFIG> *federate_hook) const;
	bool StubConflicts(FederateStub<CONFIG> *federate_stub) const;
	bool SetHook(FederateHook<CONFIG> *federate_hook);
	bool SetStub(FederateStub<CONFIG> *federate_stub);
	FederateHook<CONFIG> *GetHook(const std::string& hook_name) const;
	FederateStub<CONFIG> *GetStub(const std::string& stub_name) const;
	void Wait();
};

//////////////////////////////// ObjectClass //////////////////////////////////

struct ObjectClass : unisim::util::hla::ObjectClass
{
	ObjectClass(unisim::util::hla::FederateBase& federate, const std::string& name);
	virtual ~ObjectClass();
};

////////////////////////////// InstrumentBase /////////////////////////////////

struct InstrumentBase
{
	enum Direction
	{
		READ,
		WRITE
	};
	
	InstrumentBase(const std::string& expr, bool read, bool write);
	virtual ~InstrumentBase();
	virtual void Do(Direction direction) = 0;
	bool IsRead() const;
	bool IsWrite() const;
protected:
	std::string expr;
	bool read;
	bool write;
};

/////////////////////////////// Instrument<> //////////////////////////////////

template <typename CONFIG, typename TYPE>
struct Instrument : InstrumentBase
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	Instrument(Federate<CONFIG>& federate, unisim::util::hla::AttributeValue<TYPE>& attr_value, const std::string& expr, bool read, bool write);
	virtual void Do(Direction direction);
private:
	Federate<CONFIG>& federate;
	unisim::util::hla::AttributeValue<TYPE>& attr_value;
	unisim::util::debug::DataObjectRef<ADDRESS> data_object;
};

//////////////////////////// FederateHookStub<> ///////////////////////////////

template <typename CONFIG>
struct FederateHookStub
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	FederateHookStub(Federate<CONFIG>& federate, const std::string& name, const unisim::util::json::JSON_Value *json_return_value);
	virtual ~FederateHookStub();
	
	const std::string& GetName() const;
	
	void AddInstrument(InstrumentBase *Instrument);
	void DoInstrument(InstrumentBase::Direction direction);
protected:
	Federate<CONFIG>& federate;
	std::string name;
	const unisim::util::json::JSON_Value *json_return_value;
	typedef std::vector<InstrumentBase *> Instruments;
	Instruments instruments;
	Instruments input_instruments;
	Instruments output_instruments;
};

////////////////////////////// FederateHook<> /////////////////////////////////

template <typename CONFIG>
struct FederateHook
	: public unisim::util::debug::SourceCodeHook<typename CONFIG::ADDRESS>
	, public FederateHookStub<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	FederateHook(Federate<CONFIG>& _federate, const std::string& _name, const std::string& _file, const unisim::util::debug::SourceCodeLocation& _source_code_location, const unisim::util::json::JSON_Value *_json_return_value);
	virtual void Run();
};

////////////////////////////// FederateStub<> /////////////////////////////////

template <typename CONFIG>
struct FederateStub
	: public unisim::util::debug::Stub<typename CONFIG::ADDRESS>
	, public FederateHookStub<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	FederateStub(Federate<CONFIG>& _federate, const std::string& _name, const typename unisim::util::debug::SubProgram<typename CONFIG::ADDRESS> *_subprogram, const unisim::util::json::JSON_Value *_json_return_value);
	virtual void Run(typename unisim::util::debug::Stub<typename CONFIG::ADDRESS>::Parameters& parameters);
};

} // end of namespace hla
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // HAVE_HLA_RTI1516E

#endif // __UNISIM_SERVICE_DEBUG_HLA_FEDERATE_HH__

