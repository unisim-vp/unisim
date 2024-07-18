/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_MACHINE_STATE_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_MACHINE_STATE_HH__

#include <vector>
#include <unisim/util/debug/simple_register_registry.hh>
#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/option.hh>
#include <unisim/util/blob/blob.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/stack_frame.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
class DWARF_MachineState
{
public:
	DWARF_MachineState();
	~DWARF_MachineState();
	
	// DWARF Handler registration
	void Register(const DWARF_Handler<MEMORY_ADDR> *dw_handler);
	void Unregister(const DWARF_Handler<MEMORY_ADDR> *dw_handler);
	
	// Logging interface
	void SetDebugInfoStream(std::ostream& debug_info_stream);
	void SetDebugWarningStream(std::ostream& debug_warning_stream);
	void SetDebugErrorStream(std::ostream& debug_error_stream);
	
	// Architecture interface
	void SetRegistersInterface(unsigned int prc_num, unisim::service::interfaces::Registers *regs_if);
	void SetMemoryInterface(unsigned int prc_num, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if);
	void SetArchitecture(unsigned int prc_num, const char *architecture);
	
	unisim::service::interfaces::Registers *GetRegistersInterface(unsigned int prc_num) const;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *GetMemoryInterface(unsigned int prc_num) const;
	const char *GetArchitecture(unsigned int prc_num) const;
	
	// Option management
	bool SetOption(Option opt, const char *s);
	bool SetOption(Option opt, bool flag);
	bool SetOption(Option opt, int value);

	bool GetOption(Option opt, std::string& s) const;
	bool GetOption(Option opt, bool& flag) const;
	bool GetOption(Option opt, int& value) const;
	
	const bool& GetOptionFlag(Option opt) const;
	const std::string& GetOptionString(Option opt) const;
	const int& GetOptionInt(Option opt) const;
	
	// Initialization
	void Initialize();
	
	// High level interface
	std::vector<MEMORY_ADDR> *GetBackTrace(unsigned int prc_num) const;
	bool GetReturnAddress(unsigned int prc_num, MEMORY_ADDR& ret_addr) const;
	void InvalidateFrames(unsigned int prc_num);
	void InvalidateDirtyFrames();
	bool DirtyFrames(unsigned int prc_num) const;
	bool DirtyFrames() const;
	bool SelectStackFrame(unsigned int prc_num, unsigned int frame_num);
	unsigned int GetSelectedFrame(unsigned int prc_num) const;
	bool ComputeCFA(const DWARF_Frame<MEMORY_ADDR> *dw_frame, MEMORY_ADDR& cfa) const;
	bool EnableBinary(const unisim::util::blob::Blob<MEMORY_ADDR> *blob, bool enable);
	unisim::util::debug::DataObjectRef<MEMORY_ADDR> FindDataObject(unsigned int prc_num, const char *data_object_name) const;
	unisim::util::debug::DataObjectRef<MEMORY_ADDR> GetReturnValue(unsigned int prc_num) const;
	unisim::util::debug::DataObjectRef<MEMORY_ADDR> GetSubProgramParameter(unsigned int prc_num, unsigned int index) const;
	void ScanDataObjectNames(unsigned int prc_num, unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope) const;
	bool UnwindStack(unsigned int prc_num, unsigned int frame_num);
	unisim::service::interfaces::Register *GetRegister(unsigned int prc_num, unsigned int dw_reg_num);
	unisim::service::interfaces::Register *GetRegister(unsigned int prc_num, const char *name);
	bool ReadProgramCounterRegister(unsigned int prc_num, MEMORY_ADDR& pc) const;
	void ScanRegisters(unsigned int prc_num, unisim::service::interfaces::RegisterScanner& scanner);
	void ScanStackFrameInfos(unsigned int prc_num, unisim::service::interfaces::StackFrameInfoScanner<MEMORY_ADDR>& scanner, unsigned int max_stack_frames) const;
	unsigned int GetStackFrameInfos(unsigned int prc_num, unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR> *stack_frame_infos, unsigned int max_stack_frames) const;
private:
	friend class DWARF_Handler<MEMORY_ADDR>;
	friend class DWARF_Frame<MEMORY_ADDR>;
	friend class DWARF_RegSet<MEMORY_ADDR>;
	friend class DWARF_DataObject<MEMORY_ADDR>;
	friend class DWARF_SubProgram<MEMORY_ADDR>;
	friend class DWARF_RegisterRef<MEMORY_ADDR>;
	friend class DWARF_MachineStateRegister<MEMORY_ADDR>;
	friend class DWARF_MachineStateProgramCounter<MEMORY_ADDR>;
	
	// DWARF handlers (one per blob)
	struct Handler
	{
		Handler() : enable(true), dw_handler(0) {}
		Handler(bool _enable, const DWARF_Handler<MEMORY_ADDR> *_dw_handler) : enable(_enable), dw_handler(_dw_handler) {}
		Handler(const Handler& h) : enable(h.enable), dw_handler(h.dw_handler) {}
		Handler& operator = (const Handler& h) { enable = h.enable; dw_handler = h.dw_handler; return *this; }
		
		bool enable;
		const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	};
	
	typedef std::vector<Handler> Handlers;
	Handlers handlers;
	
	// Debug stuf
	std::ostream *debug_info_stream;
	std::ostream *debug_warning_stream;
	std::ostream *debug_error_stream;
	bool verbose;
	bool debug;
	int max_stack_frames;
	
	// DWARF Register number mapping
	std::string reg_num_mapping_filename;

	// Frames (ordered from inner to outer frames)
	typedef std::vector<DWARF_Frame<MEMORY_ADDR> *> Frames;
	
	// Architectural state (per processor)
	struct Architecture
	{
		std::string architecture;
		DWARF_RegisterNumberMapping *dw_reg_num_mapping;
		unisim::service::interfaces::Registers *regs_if;
		unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
		typedef unisim::util::debug::SimpleRegisterRegistry RegistersRegistry;
		RegistersRegistry registers_registry;
		Frames frames;
		bool dirty_frames;
		unsigned int sel;
		
		Architecture()
			: architecture()
			, dw_reg_num_mapping(0)
			, regs_if(0)
			, mem_if(0)
			, registers_registry()
			, frames()
			, dirty_frames(false)
			, sel(0)
		{
		}
	};
	typedef std::vector<Architecture> Architectures;
	mutable Architectures architectures;
	
	// Debug stuff
	std::ostream& GetDebugInfoStream() const;
	std::ostream& GetDebugWarningStream() const;
	std::ostream& GetDebugErrorStream() const;
	
	// Architectural state methods per processor
	unsigned int GetFrameNumber(const DWARF_Frame<MEMORY_ADDR> *frame) const;
	DWARF_Frame<MEMORY_ADDR> *GetInnerFrame(unsigned int prc_num) const;
	DWARF_Frame<MEMORY_ADDR> *GetInnerFrame(const DWARF_Frame<MEMORY_ADDR> *frame) const;
	DWARF_Frame<MEMORY_ADDR> *GetCurrentFrame(unsigned int prc_num) const;
	DWARF_Frame<MEMORY_ADDR> *GetFrame(unsigned int prc_num, unsigned int level) const;
	DWARF_RegisterNumberMapping *GetRegisterNumberMapping(unsigned int prc_num) const;
	
	// Various helper methods
	const DWARF_CFI<MEMORY_ADDR> *FindCFIByAddr(unsigned int prc_num, MEMORY_ADDR pc) const;
	
	// Frames
	unsigned int BuildFrames(unsigned int prc_num, unsigned int depth) const;
};

template <class MEMORY_ADDR>
class DWARF_MachineStateRegister : public unisim::service::interfaces::Register
{
public:
	DWARF_MachineStateRegister(DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, unsigned int dw_reg_num, unisim::service::interfaces::Register *arch_reg);
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	DWARF_MachineState<MEMORY_ADDR> *dw_mach_state;
	unsigned prc_num;
	unsigned int dw_reg_num;
	unisim::service::interfaces::Register *arch_reg;
};

template <class MEMORY_ADDR>
class DWARF_MachineStateProgramCounter : public unisim::service::interfaces::Register
{
public:
	DWARF_MachineStateProgramCounter(DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, unisim::service::interfaces::Register *arch_reg);
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	DWARF_MachineState<MEMORY_ADDR> *dw_mach_state;
	unsigned prc_num;
	unisim::service::interfaces::Register *arch_reg;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
