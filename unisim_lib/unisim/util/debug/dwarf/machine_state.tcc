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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_MACHINE_STATE_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_MACHINE_STATE_TCC__

#include <cstring>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_MachineState<MEMORY_ADDR>::DWARF_MachineState()
	: handlers()
	, debug_info_stream(&std::cout)
	, debug_warning_stream(&std::cerr)
	, debug_error_stream(&std::cerr)
	, verbose(false)
	, debug(false)
	, max_stack_frames(256)
	, reg_num_mapping_filename()
	, architectures()
{
}

template <class MEMORY_ADDR>
DWARF_MachineState<MEMORY_ADDR>::~DWARF_MachineState()
{
	for(typename Architectures::iterator architectures_it = architectures.begin(); architectures_it != architectures.end(); ++architectures_it)
	{
		Architecture& architecture = *architectures_it;
		
		Frames& frames = architecture.frames;
		for(typename Frames::iterator frames_it = frames.begin(); frames_it != frames.end(); ++frames_it)
		{
			DWARF_Frame<MEMORY_ADDR> *frame = *frames_it;
			delete frame;
		}
		
		if(architecture.dw_reg_num_mapping)
		{
			delete architecture.dw_reg_num_mapping;
		}
	}
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::Register(const DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	handlers.push_back(Handler(true, dw_handler));
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::Unregister(const DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	typename Handlers::iterator it = handlers.begin();
	while(it != handlers.end())
	{
		if((*it).dw_handler == dw_handler)
		{
			handlers.erase(it);
		}
		else
		{
			++it;
		}
	}
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::Initialize()
{
	if(!reg_num_mapping_filename.empty())
	{
		unsigned int num_processors = architectures.size();
		unsigned int prc_num;
		
		for(prc_num = 0; prc_num  < num_processors; prc_num++)
		{
			unisim::service::interfaces::Registers *_regs_if = GetRegistersInterface(prc_num);
			
			if(_regs_if)
			{
				const char *architecture = GetArchitecture(prc_num);
				
				if(verbose)
				{
					GetDebugInfoStream() << "Loading DWARF register number mapping from \"" << reg_num_mapping_filename << "\" for Processor #" << prc_num << " (\"" << architecture << "\" architecture)" << std::endl;
				}

				if(strcmp(architecture, "") != 0)
				{
					architectures[prc_num].dw_reg_num_mapping = new DWARF_RegisterNumberMapping(GetDebugInfoStream(), GetDebugWarningStream(), GetDebugErrorStream(), _regs_if);
					
					if(architectures[prc_num].dw_reg_num_mapping->Load(reg_num_mapping_filename.c_str(), architecture))
					{
						std::set<unsigned int> dw_reg_nums;
						architectures[prc_num].dw_reg_num_mapping->EnumRegisterNumbers(dw_reg_nums);
						for(std::set<unsigned int>::const_iterator dw_reg_num_iter = dw_reg_nums.begin(); dw_reg_num_iter != dw_reg_nums.end(); ++dw_reg_num_iter)
						{
							unsigned int dw_reg_num = *dw_reg_num_iter;
							unisim::service::interfaces::Register *arch_reg = architectures[prc_num].dw_reg_num_mapping->GetRegister(dw_reg_num);
							DWARF_MachineStateRegister<MEMORY_ADDR> *dw_reg = new DWARF_MachineStateRegister<MEMORY_ADDR>(this, prc_num, dw_reg_num, arch_reg);
							architectures[prc_num].registers_registry.AddRegisterInterface(dw_reg);
						}
						
						unisim::service::interfaces::Register *arch_pc_reg = architectures[prc_num].dw_reg_num_mapping->GetProgramCounterRegister();
						DWARF_MachineStateProgramCounter<MEMORY_ADDR> *dw_reg = new DWARF_MachineStateProgramCounter<MEMORY_ADDR>(this, prc_num, arch_pc_reg);
						architectures[prc_num].registers_registry.AddRegisterInterface(dw_reg);
					}
					else
					{
						GetDebugWarningStream() << "Can't load DWARF register number mapping for architecture \"" << architecture << "\" from \"" << reg_num_mapping_filename << "\"" << std::endl;
						architectures[prc_num].dw_reg_num_mapping = 0;
					}
				}
				else
				{
					GetDebugWarningStream() << "Unknown architecture" << std::endl;
					architectures[prc_num].dw_reg_num_mapping = 0;
				}
				
				struct ArchRegsScanner : unisim::service::interfaces::RegisterScanner
				{
					ArchRegsScanner(typename Architecture::RegistersRegistry& _registers_registry) : registers_registry(_registers_registry) {}
					
					virtual void Append(unisim::service::interfaces::Register *arch_reg)
					{
						if(!registers_registry.GetRegister(arch_reg->GetName()))
						{
							registers_registry.AddRegisterInterface(arch_reg, /* is_owner */ false);
						}
					}
					
					typename Architecture::RegistersRegistry& registers_registry;
				};
				
				ArchRegsScanner arch_regs_scanner(architectures[prc_num].registers_registry);
				_regs_if->ScanRegisters(arch_regs_scanner);
			}
			
			Architecture& architecture = architectures[prc_num];
			DWARF_Frame<MEMORY_ADDR> *inner_frame = new DWARF_Frame<MEMORY_ADDR>(this, prc_num);
			inner_frame->LoadArchRegs();
			architecture.frames.push_back(inner_frame);
			architecture.sel = 0;
		}
	}
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::SetDebugInfoStream(std::ostream& _debug_info_stream)
{
	debug_info_stream = &_debug_info_stream;
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::SetDebugWarningStream(std::ostream& _debug_warning_stream)
{
	debug_warning_stream = &_debug_warning_stream;
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::SetDebugErrorStream(std::ostream& _debug_error_stream)
{
	debug_error_stream = &_debug_error_stream;
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::SetRegistersInterface(unsigned int prc_num, unisim::service::interfaces::Registers *regs_if)
{
	if(prc_num >= architectures.size())
	{
		architectures.resize(prc_num + 1);
	}
	architectures[prc_num].regs_if = regs_if;
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::SetMemoryInterface(unsigned int prc_num, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if)
{
	if(prc_num >= architectures.size())
	{
		architectures.resize(prc_num + 1);
	}
	architectures[prc_num].mem_if = mem_if;
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::SetArchitecture(unsigned int prc_num, const char *architecture)
{
	if(prc_num >= architectures.size())
	{
		architectures.resize(prc_num + 1);
	}
	architectures[prc_num].architecture = architecture;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::Registers *DWARF_MachineState<MEMORY_ADDR>::GetRegistersInterface(unsigned int prc_num) const
{
	return (prc_num < architectures.size()) ? architectures[prc_num].regs_if : 0;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::Memory<MEMORY_ADDR> *DWARF_MachineState<MEMORY_ADDR>::GetMemoryInterface(unsigned int prc_num) const
{
	return (prc_num < architectures.size()) ? architectures[prc_num].mem_if : 0;
}

template <class MEMORY_ADDR>
const char *DWARF_MachineState<MEMORY_ADDR>::GetArchitecture(unsigned int prc_num) const
{
	return (prc_num < architectures.size()) ? architectures[prc_num].architecture.c_str() : 0;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::SetOption(Option opt, const char *s)
{
	switch(opt)
	{
		case OPT_REG_NUM_MAPPING_FILENAME:
			reg_num_mapping_filename = s;
			return true;
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::SetOption(Option opt, bool flag)
{
	switch(opt)
	{
		case OPT_VERBOSE:
			verbose = flag;
			return true;
		case OPT_DEBUG:
			debug = flag;
			return true;
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::SetOption(Option opt, int value)
{
	switch(opt)
	{
		case OPT_MAX_STACK_FRAMES:
			if((value >= 0))
			{
				max_stack_frames = value;
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::GetOption(Option opt, std::string& s) const
{
	switch(opt)
	{
		case OPT_REG_NUM_MAPPING_FILENAME:
			s = reg_num_mapping_filename;
			return true;
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::GetOption(Option opt, bool& flag) const
{
	switch(opt)
	{
		case OPT_VERBOSE:
			flag = verbose;
			return true;
		case OPT_DEBUG:
			flag = debug;
			return true;
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::GetOption(Option opt, int& value) const
{
	switch(opt)
	{
		case OPT_MAX_STACK_FRAMES:
			value = max_stack_frames;
			return true;
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
const bool& DWARF_MachineState<MEMORY_ADDR>::GetOptionFlag(Option opt) const
{
	switch(opt)
	{
		case OPT_VERBOSE:
			return verbose;
		case OPT_DEBUG:
			return debug;
		default:
			break;
	}
	static bool dummy_flag = false;
	return dummy_flag;
}

template <class MEMORY_ADDR>
const std::string& DWARF_MachineState<MEMORY_ADDR>::GetOptionString(Option opt) const
{
	switch(opt)
	{
		case OPT_REG_NUM_MAPPING_FILENAME:
			return reg_num_mapping_filename;
		default:
			break;
	}
	static std::string dummy_string;
	return dummy_string;
}

template <class MEMORY_ADDR>
const int& DWARF_MachineState<MEMORY_ADDR>::GetOptionInt(Option opt) const
{
	switch(opt)
	{
		case OPT_MAX_STACK_FRAMES:
			return max_stack_frames;
		default:
			break;
	}
	static int dummy_int = 0;
	return dummy_int;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MachineState<MEMORY_ADDR>::GetDebugInfoStream() const
{
	return *debug_info_stream;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MachineState<MEMORY_ADDR>::GetDebugWarningStream() const
{
	return *debug_warning_stream;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MachineState<MEMORY_ADDR>::GetDebugErrorStream() const
{
	return *debug_error_stream;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MachineState<MEMORY_ADDR>::GetFrameNumber(const DWARF_Frame<MEMORY_ADDR> *_frame) const
{
	unsigned int prc_num = _frame->GetProcessorNumber();
	Architecture& architecture = architectures[prc_num];
	Frames& frames = architecture.frames;
	for(typename Frames::iterator frames_it = frames.begin(); frames_it != frames.end(); ++frames_it)
	{
		DWARF_Frame<MEMORY_ADDR> *frame = *frames_it;
		if(frame == _frame) return frames_it - frames.begin();
	}
	
	throw std::runtime_error("Internal error");
	return 0;
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR> *DWARF_MachineState<MEMORY_ADDR>::GetInnerFrame(unsigned int prc_num) const
{
	if(prc_num >= architectures.size()) return 0;
	Frames& frames = architectures[prc_num].frames;
	DWARF_Frame<MEMORY_ADDR> *inner_frame = 0;
	if(frames.size())
	{
		inner_frame = frames[0];
	}
	
	return inner_frame;
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR> *DWARF_MachineState<MEMORY_ADDR>::GetInnerFrame(const DWARF_Frame<MEMORY_ADDR> *_frame) const
{
	unsigned int prc_num = _frame->GetProcessorNumber();
	Frames& frames = architectures[prc_num].frames;
	DWARF_Frame<MEMORY_ADDR> *inner_frame = 0;
	bool found = false;
	for(typename Frames::iterator frames_it = frames.begin(); frames_it != frames.end(); ++frames_it)
	{
		DWARF_Frame<MEMORY_ADDR> *frame = *frames_it;
		if(frame == _frame)
		{
			found = true;
			break;
		}
		inner_frame = frame;
	}
	if(!found) inner_frame = 0;
	
	return inner_frame;
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR> *DWARF_MachineState<MEMORY_ADDR>::GetCurrentFrame(unsigned int prc_num) const
{
	if(prc_num >= architectures.size()) return 0;
	unsigned int sel = architectures[prc_num].sel;
	Frames& frames = architectures[prc_num].frames;
	return frames[sel];
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR> *DWARF_MachineState<MEMORY_ADDR>::GetFrame(unsigned int prc_num, unsigned int level) const
{
	if(prc_num >= architectures.size()) return 0;
	Frames& frames = architectures[prc_num].frames;
	if(level >= frames.size()) return 0;
	return frames[level];
}
	
template <class MEMORY_ADDR>
DWARF_RegisterNumberMapping *DWARF_MachineState<MEMORY_ADDR>::GetRegisterNumberMapping(unsigned int prc_num) const
{
	if(prc_num >= architectures.size()) return 0;
	return architectures[prc_num].dw_reg_num_mapping;
}

template <class MEMORY_ADDR>
const DWARF_CFI<MEMORY_ADDR> *DWARF_MachineState<MEMORY_ADDR>::FindCFIByAddr(unsigned int prc_num, MEMORY_ADDR pc) const
{
	const char *architecture = GetArchitecture(prc_num);
	for(typename Handlers::const_iterator it = handlers.begin(); it != handlers.end(); ++it)
	{
		const Handler& handler = *it;
		if(handler.enable)
		{
			const DWARF_Handler<MEMORY_ADDR> *dw_handler = handler.dw_handler;
			
			if(!architecture || (strcmp(architecture, dw_handler->GetArchitecture()) == 0))
			{
				const DWARF_CFI<MEMORY_ADDR> *found_cfi = dw_handler->FindCFIByAddr(pc);
				if(found_cfi) return found_cfi;
			}
		}
	}
	
	return 0;
}

template <class MEMORY_ADDR>
std::vector<MEMORY_ADDR> *DWARF_MachineState<MEMORY_ADDR>::GetBackTrace(unsigned int prc_num) const
{
	struct StackFrameInfoScanner : unisim::service::interfaces::StackFrameInfoScanner<MEMORY_ADDR>
	{
		std::vector<MEMORY_ADDR> *backtrace;
		StackFrameInfoScanner() : backtrace(0) {}
		virtual void Append(const unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR>& stack_frame_info)
		{
			if(!backtrace) backtrace = new std::vector<MEMORY_ADDR>();
			backtrace->push_back(stack_frame_info.pc);
		}
	};
	
	StackFrameInfoScanner stack_frame_info_scanner;
	ScanStackFrameInfos(prc_num, stack_frame_info_scanner, 0);
	return stack_frame_info_scanner.backtrace;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::GetReturnAddress(unsigned int prc_num, MEMORY_ADDR& ret_addr) const
{
	struct StackFrameInfoScanner : unisim::service::interfaces::StackFrameInfoScanner<MEMORY_ADDR>
	{
		MEMORY_ADDR& ret_addr;
		bool status;
		StackFrameInfoScanner(MEMORY_ADDR& _ret_addr) : ret_addr(_ret_addr), status(false) {}
		virtual void Append(const unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR>& stack_frame_info)
		{
			ret_addr = stack_frame_info.ret_addr;
			status = true;
		}
	};
	
	StackFrameInfoScanner stack_frame_info_scanner(ret_addr);
	ScanStackFrameInfos(prc_num, stack_frame_info_scanner, 1);
	return stack_frame_info_scanner.status;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::SelectStackFrame(unsigned int prc_num, unsigned int frame_num)
{
	if(BuildFrames(prc_num, frame_num + 1) <= frame_num) return false;
	Architecture& architecture = architectures[prc_num];
	architecture.sel = frame_num;
	return true;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MachineState<MEMORY_ADDR>::GetSelectedFrame(unsigned int prc_num) const
{
	Architecture& architecture = architectures[prc_num];
	return architecture.sel;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::ComputeCFA(const DWARF_Frame<MEMORY_ADDR> *dw_frame, MEMORY_ADDR& cfa) const
{
	unsigned int prc_num = dw_frame->GetProcessorNumber();
	unsigned int level = GetFrameNumber(dw_frame) + 1;
	if(BuildFrames(prc_num, level + 1) <= level) return false;
	
	cfa = dw_frame->ReadCFA();
	return true;
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::InvalidateFrames(unsigned int prc_num)
{
	if(prc_num >= architectures.size()) return;
	Architecture& architecture = architectures[prc_num];
	Frames& frames = architecture.frames;
	if(frames.size() > 1)
	{
		do
		{
			DWARF_Frame<MEMORY_ADDR> *frame = frames.back();
			delete frame;
			frames.pop_back();
		}
		while(frames.size() > 1);
		architecture.sel = 0;
	}
	architecture.dirty_frames = false;
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::InvalidateDirtyFrames()
{
	unsigned int num_processors = architectures.size();
	for(unsigned int prc_num = 0; prc_num < num_processors; ++prc_num)
	{
		if(DirtyFrames(prc_num))
		{
			InvalidateFrames(prc_num);
		}
	}
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::DirtyFrames(unsigned int prc_num) const
{
	if(prc_num >= architectures.size()) return false;
	Architecture& architecture = architectures[prc_num];
	return architecture.dirty_frames;
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::DirtyFrames() const
{
	unsigned int num_processors = architectures.size();
	for(unsigned int prc_num = 0; prc_num < num_processors; ++prc_num)
	{
		Architecture& architecture = architectures[prc_num];
		if(architecture.dirty_frames) return true;
	}
	return false;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MachineState<MEMORY_ADDR>::BuildFrames(unsigned int prc_num, unsigned int depth) const
{
	if(prc_num >= architectures.size()) return 0;
	Frames& frames = architectures[prc_num].frames;
	unsigned int level = frames.size();
	if(depth && (level >= depth)) return level;
	
	DWARF_Frame<MEMORY_ADDR> *frame = frames[level - 1];
	MEMORY_ADDR pc = 0;
	if(!frame->ReadProgramCounterRegister(pc)) return level;
	
	while(!depth || (level < depth))
	{
		frame->ClearCFA();
		
		if(debug)
		{
			GetDebugInfoStream() << "Searching CFI for PC=0x" << std::hex << pc << std::dec << std::endl;
		}
		
		const DWARF_CFI<MEMORY_ADDR> *cfi = FindCFIByAddr(prc_num, pc);
		if(!cfi)
		{
			if(debug)
			{
				GetDebugInfoStream() << "Nore more CFI found" << std::endl;
			}
			break;
		}
			
		const DWARF_FDE<MEMORY_ADDR> *dw_fde = cfi->GetFDE();
		const DWARF_Handler<MEMORY_ADDR> *dw_handler = dw_fde->GetHandler();
		const DWARF_CIE<MEMORY_ADDR> *dw_cie = dw_fde->GetCIE();
		if(debug)
		{
			GetDebugInfoStream() << "In File \"" << dw_handler->GetFilename() << "\", computed call frame information:" << std::endl << *cfi << std::endl;
		}
		
		typename unisim::util::debug::dwarf::DWARF_CFIRow<MEMORY_ADDR> *cfi_row = cfi->GetLowestRow(pc);
		
		if(debug)
		{
			GetDebugInfoStream() << "In File \"" << dw_handler->GetFilename() << "\", lowest Rule Matrix Row:" << *cfi_row << std::endl;
		}
		
		unsigned int dw_ret_addr_reg_num = dw_cie->GetReturnAddressRegister();

		DWARF_Frame<MEMORY_ADDR> *outer_frame = new DWARF_Frame<MEMORY_ADDR>(this, prc_num, dw_handler, cfi_row, dw_ret_addr_reg_num);
		
		frames.push_back(outer_frame);
		architectures[prc_num].dirty_frames = true;
		
		if(!outer_frame->Unwind())
		{
			if(debug)
			{
				GetDebugInfoStream() << "In File \"" << dw_handler->GetFilename() << "\", no more unwinding context" << std::endl;
			}
			frames.pop_back();
			delete outer_frame;
			break;
		}
		
		frame = outer_frame;

		MEMORY_ADDR ret_addr = 0;
		if(!frame->ReadProgramCounterRegister(ret_addr)) break;
		
		if(debug)
		{
			GetDebugInfoStream() << "In File \"" << dw_handler->GetFilename() << "\", return address: 0x" << std::hex << ret_addr << std::dec << std::endl;
		}

		pc = ret_addr - 1; // we take return address - 1 in the hope it is in the same context as the caller
		
		++level;
	}
	
	return frames.size();
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::EnableBinary(const char *filename, bool enable)
{
	bool found = false;
	for(typename Handlers::iterator it = handlers.begin(); it != handlers.end(); ++it)
	{
		Handler& handler = *it;
		if(strcmp(handler.dw_handler->GetFilename(), filename) == 0)
		{
			handler.enable = enable;
			found = true;
		}
	}
	
	return found;
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObjectRef<MEMORY_ADDR> DWARF_MachineState<MEMORY_ADDR>::FindDataObject(unsigned int prc_num, const char *data_object_name) const
{
	const char *architecture = GetArchitecture(prc_num);
	for(typename Handlers::const_iterator it = handlers.begin(); it != handlers.end(); ++it)
	{
		const Handler& handler = *it;
		if(handler.enable)
		{
			const DWARF_Handler<MEMORY_ADDR> *dw_handler = handler.dw_handler;
			
			if(!architecture || (strcmp(architecture, dw_handler->GetArchitecture()) == 0))
			{
				unisim::util::debug::DataObject<MEMORY_ADDR> *data_object = dw_handler->FindDataObject(this, prc_num, data_object_name);
				return unisim::util::debug::DataObjectRef<MEMORY_ADDR>(data_object);
			}
		}
	}
	
	return unisim::util::debug::DataObjectRef<MEMORY_ADDR>();
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObjectRef<MEMORY_ADDR> DWARF_MachineState<MEMORY_ADDR>::GetReturnValue(unsigned int prc_num) const
{
	const char *architecture = GetArchitecture(prc_num);
	for(typename Handlers::const_iterator it = handlers.begin(); it != handlers.end(); ++it)
	{
		const Handler& handler = *it;
		if(handler.enable)
		{
			const DWARF_Handler<MEMORY_ADDR> *dw_handler = handler.dw_handler;
			
			if(!architecture || (strcmp(architecture, dw_handler->GetArchitecture()) == 0))
			{
				unisim::util::debug::DataObject<MEMORY_ADDR> *data_object = dw_handler->GetReturnValue(this, prc_num);
				return unisim::util::debug::DataObjectRef<MEMORY_ADDR>(data_object);
			}
		}
	}
	
	return unisim::util::debug::DataObjectRef<MEMORY_ADDR>();
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObjectRef<MEMORY_ADDR> DWARF_MachineState<MEMORY_ADDR>::GetSubProgramParameter(unsigned int prc_num, unsigned int index) const
{
	const char *architecture = GetArchitecture(prc_num);
	for(typename Handlers::const_iterator it = handlers.begin(); it != handlers.end(); ++it)
	{
		const Handler& handler = *it;
		if(handler.enable)
		{
			const DWARF_Handler<MEMORY_ADDR> *dw_handler = handler.dw_handler;
			
			if(!architecture || (strcmp(architecture, dw_handler->GetArchitecture()) == 0))
			{
				unisim::util::debug::DataObject<MEMORY_ADDR> *data_object = dw_handler->GetSubProgramParameter(this, prc_num, index);
				return unisim::util::debug::DataObjectRef<MEMORY_ADDR>(data_object);
			}
		}
	}
	
	return unisim::util::debug::DataObjectRef<MEMORY_ADDR>();
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::EnumerateDataObjectNames(unsigned int prc_num, std::set<std::string>& name_set, typename unisim::service::interfaces::DataObjectLookup<MEMORY_ADDR>::Scope scope) const
{
	const char *architecture = GetArchitecture(prc_num);
	for(typename Handlers::const_iterator it = handlers.begin(); it != handlers.end(); ++it)
	{
		const Handler& handler = *it;
		if(handler.enable)
		{
			const DWARF_Handler<MEMORY_ADDR> *dw_handler = handler.dw_handler;
			
			if(!architecture || (strcmp(architecture, dw_handler->GetArchitecture()) == 0))
			{
				dw_handler->EnumerateDataObjectNames(this, prc_num, name_set, scope);
			}
		}
	}
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::UnwindStack(unsigned int prc_num, unsigned int frame_num)
{
	if(frame_num < 1) return true;
	if(BuildFrames(prc_num, frame_num + 1) <= frame_num) return false;
	Architecture& architecture = architectures[prc_num];
	Frames& frames = architecture.frames;
	DWARF_Frame<MEMORY_ADDR> *frame = frames[frame_num];
	if(frame->Commit())
	{
		typename Frames::iterator first = frames.begin() + 1;
		typename Frames::iterator last = frames.begin() + frame_num;
		typename Frames::iterator end = last + 1;
		for(typename Frames::iterator frames_it = first; frames_it != end; ++frames_it)
		{
			DWARF_Frame<MEMORY_ADDR> *dropped_frame = *frames_it;
			delete dropped_frame;
		}
		frames.erase(first, end); 
		architecture.sel = 0;
		return true;
	}
	
	return false;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::Register *DWARF_MachineState<MEMORY_ADDR>::GetRegister(unsigned int prc_num, unsigned int dw_reg_num)
{
	DWARF_RegisterNumberMapping *dw_reg_num_mapping = architectures[prc_num].dw_reg_num_mapping;
	return dw_reg_num_mapping ? dw_reg_num_mapping->GetRegister(dw_reg_num) : 0;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::Register *DWARF_MachineState<MEMORY_ADDR>::GetRegister(unsigned int prc_num, const char *name)
{
	typename Architecture::RegistersRegistry& registers_registry = architectures[prc_num].registers_registry;
	return registers_registry.GetRegister(name);
}

template <class MEMORY_ADDR>
bool DWARF_MachineState<MEMORY_ADDR>::ReadProgramCounterRegister(unsigned int prc_num, MEMORY_ADDR& pc) const
{
	DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = GetCurrentFrame(prc_num);
	return dw_curr_frame && dw_curr_frame->ReadProgramCounterRegister(pc);
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::ScanRegisters(unsigned int prc_num, unisim::service::interfaces::RegisterScanner& scanner)
{
	typename Architecture::RegistersRegistry& registers_registry = architectures[prc_num].registers_registry;
	registers_registry.ScanRegisters(scanner);
}

template <class MEMORY_ADDR>
void DWARF_MachineState<MEMORY_ADDR>::ScanStackFrameInfos(unsigned int prc_num, unisim::service::interfaces::StackFrameInfoScanner<MEMORY_ADDR>& scanner, unsigned int _max_stack_frames) const
{
	if(prc_num >= architectures.size()) return;
	if(!_max_stack_frames || (_max_stack_frames >= (unsigned int) max_stack_frames)) _max_stack_frames = max_stack_frames;
	if(_max_stack_frames < 1) _max_stack_frames = 1;
	unsigned int depth = _max_stack_frames + 1;
	if(BuildFrames(prc_num, depth) < 1) return;
	Architecture& architecture = architectures[prc_num];
	Frames& frames = architecture.frames;
	depth = frames.size();
	unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR> stack_frame_infos[depth];
	if(_max_stack_frames > depth) _max_stack_frames = depth;
	MEMORY_ADDR pc = 0;
	for(int rev_frame_num = depth - 1; rev_frame_num >= 0; --rev_frame_num)
	{
		MEMORY_ADDR ret_addr = pc;
		unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR>& stack_frame_info = stack_frame_infos[rev_frame_num];
		DWARF_Frame<MEMORY_ADDR> *frame = frames[rev_frame_num];
		if(!frame->ReadProgramCounterRegister(pc)) return;
		MEMORY_ADDR cfa = frame->ReadCFA();
		stack_frame_info.frame_num = rev_frame_num;
		stack_frame_info.addr = cfa;
		stack_frame_info.pc = pc;
		stack_frame_info.ret_addr = ret_addr;
	}
	for(unsigned int frame_num = 0; frame_num < _max_stack_frames; ++frame_num)
	{
		scanner.Append(stack_frame_infos[frame_num]);
	}
}

template <class MEMORY_ADDR>
unsigned int DWARF_MachineState<MEMORY_ADDR>::GetStackFrameInfos(unsigned int prc_num, unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR> *stack_frame_infos, unsigned int max_stack_frames) const
{
	struct StackFrameInfoScanner : unisim::service::interfaces::StackFrameInfoScanner<MEMORY_ADDR>
	{
		unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR> *stack_frame_infos;
		unsigned int max_stack_frames;
		unsigned int depth;
		
		StackFrameInfoScanner(unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR> *_stack_frame_infos, unsigned int _max_stack_frames)
			: stack_frame_infos(_stack_frame_infos)
			, max_stack_frames(_max_stack_frames)
			, depth(0)
		{
		}
		
		virtual void Append(const unisim::service::interfaces::StackFrameInfo<MEMORY_ADDR>& stack_frame_info)
		{
			if(depth >= max_stack_frames) return;
			stack_frame_infos[depth] = stack_frame_info;
			++depth;
		}
	};
	
	StackFrameInfoScanner stack_frame_info_scanner(stack_frame_infos, max_stack_frames);
	ScanStackFrameInfos(prc_num, stack_frame_info_scanner, max_stack_frames);
	return stack_frame_info_scanner.depth;
}

template <class MEMORY_ADDR>
DWARF_MachineStateRegister<MEMORY_ADDR>::DWARF_MachineStateRegister(DWARF_MachineState<MEMORY_ADDR> *_dw_mach_state, unsigned int _prc_num, unsigned int _dw_reg_num, unisim::service::interfaces::Register *_arch_reg)
	: dw_mach_state(_dw_mach_state)
	, prc_num(_prc_num)
	, dw_reg_num(_dw_reg_num)
	, arch_reg(_arch_reg)
{
}

template <class MEMORY_ADDR>
const char *DWARF_MachineStateRegister<MEMORY_ADDR>::GetName() const
{
	return arch_reg->GetName();
}

template <class MEMORY_ADDR>
void DWARF_MachineStateRegister<MEMORY_ADDR>::GetValue(void *buffer) const
{
	unsigned int curr_frame_num = dw_mach_state->GetSelectedFrame(prc_num);
	if(curr_frame_num != 0)
	{
		DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
		DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetRegister(dw_reg_num);
		dw_reg->GetValue(buffer);
	}
	else
	{
		arch_reg->GetValue(buffer);
	}
}

template <class MEMORY_ADDR>
void DWARF_MachineStateRegister<MEMORY_ADDR>::SetValue(const void *buffer)
{
	unsigned int curr_frame_num = dw_mach_state->GetSelectedFrame(prc_num);
	if(curr_frame_num != 0)
	{
		DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
		DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetRegister(dw_reg_num);
		dw_reg->SetValue(buffer);
	}
	else
	{
		arch_reg->SetValue(buffer);
	}
}

template <class MEMORY_ADDR>
int DWARF_MachineStateRegister<MEMORY_ADDR>::GetSize() const
{
	return arch_reg->GetSize();
}

template <class MEMORY_ADDR>
DWARF_MachineStateProgramCounter<MEMORY_ADDR>::DWARF_MachineStateProgramCounter(DWARF_MachineState<MEMORY_ADDR> *_dw_mach_state, unsigned int _prc_num, unisim::service::interfaces::Register *_arch_reg)
	: dw_mach_state(_dw_mach_state)
	, prc_num(_prc_num)
	, arch_reg(_arch_reg)
{
}

template <class MEMORY_ADDR>
const char *DWARF_MachineStateProgramCounter<MEMORY_ADDR>::GetName() const
{
	return arch_reg->GetName();
}

template <class MEMORY_ADDR>
void DWARF_MachineStateProgramCounter<MEMORY_ADDR>::GetValue(void *buffer) const
{
	unsigned int curr_frame_num = dw_mach_state->GetSelectedFrame(prc_num);
	if(curr_frame_num != 0)
	{
		DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
		DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetProgramCounterRegister();
		dw_reg->GetValue(buffer);
	}
	else
	{
		arch_reg->GetValue(buffer);
	}
}

template <class MEMORY_ADDR>
void DWARF_MachineStateProgramCounter<MEMORY_ADDR>::SetValue(const void *buffer)
{
	unsigned int curr_frame_num = dw_mach_state->GetSelectedFrame(prc_num);
	if(curr_frame_num != 0)
	{
		DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
		DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetProgramCounterRegister();
		dw_reg->SetValue(buffer);
	}
	else
	{
		arch_reg->SetValue(buffer);
	}
}

template <class MEMORY_ADDR>
int DWARF_MachineStateProgramCounter<MEMORY_ADDR>::GetSize() const
{
	return arch_reg->GetSize();
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
