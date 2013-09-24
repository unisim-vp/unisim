/*
 *  Copyright (c) 2007,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_EXCEPTION_HANDLING_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_EXCEPTION_HANDLING_TCC__

#include <sstream>
#include <unisim/component/cxx/processor/powerpc/exception.tcc>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

template <class CONFIG>
int CPU<CONFIG>::StringLength(typename CONFIG::address_t addr)
{
	int len = 0;
	char buffer;

	while(1)
	{
		buffer = 0;
		ReadMemory(addr, &buffer, 1);
		if(buffer == 0) return len;
		len++;
		addr += 1;
	}
}

template <class CONFIG>
std::string CPU<CONFIG>::ReadString(typename CONFIG::address_t addr, unsigned int count)
{
	if(!count)
	{
		count = StringLength(addr);
	}
	
	char *buf = (char *) calloc(count + 1, 1);
	ReadMemory(addr, buf, count);

	std::stringstream sstr;

	bool open_string = false;
	unsigned int i;
	for(i = 0; i <= count; i++)
	{
		char c = buf[i];
		if(c < ' ') // control character
		{
			switch(c)
			{
				case '\n':
				case '\r':
				case '\e':
				case '\a':
				case '\f':
				case '\t':
				case '\v':
				case '\'':
				case '\\':
					if(!open_string)
					{
						sstr << "\"";
						open_string = true;
					}
					switch(c)
					{
						case '\n': sstr << "\\n"; break;
						case '\r': sstr << "\\r"; break;
						case '\e': sstr << "\\e"; break;
						case '\a': sstr << "\\a"; break;
						case '\f': sstr << "\\f"; break;
						case '\t': sstr << "\\t"; break;
						case '\v': sstr << "\\v"; break;
						case '\'': sstr << "\\'"; break;
						case '\\': sstr << "\\\\"; break;
					}
					if(i == count) sstr << "\"";
					break;
				default:
					if(open_string) sstr << "\"";
					sstr << " \\0x" << std::hex << (unsigned int) c << std::dec;
					if((i != count) && open_string) sstr << "\"";
					break;
			}
		}
		else
		{
			if(!open_string)
			{
				sstr << "\"";
				open_string = true;
			}
			sstr << c;
			if(i == count) sstr << "\"";
		}
	}
		
	free(buf);
	
	return sstr.str();
}

/* System reset exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemResetException<CONFIG>& exc)
{
	if(linux_os_import)
	{
		logger << DebugInfo << "System reset" << EndDebugInfo;
		Stop(0);
		return;
	}
	SetSRR0(GetNIA()); // save NIA
	
	// clear SRR1[0-5], SRR1[6]=MSR[6], clear SSR1[7-15], SRR1[16-31]=MSR[16-31]
	SetSRR1(GetMSR() & 0x0200ffffUL);
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	if(HasHardReset())
	{
		ResetHID0_NHR(); // reset HID0[NHR]
		ResetIRQ(CONFIG::IRQ_HARD_RESET);
	}
	else if(HasSoftReset())
	{
		SetHID0_NHR(); // set HID0[NHR]
		ResetIRQ(CONFIG::IRQ_SOFT_RESET);
	}
	
	Branch(CONFIG::EXC_SYSTEM_RESET_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
		logger << DebugInfo << exc.what() << endl << EndDebugInfo;
}

/* Machine check exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const MachineCheckException<CONFIG>& exc)
{
	if(linux_os_import)
	{
		if(HasMCP())
		{
			logger << DebugError << "Machine check" << EndDebugError;
		}
	
		if(HasTEA())
		{
			logger << DebugError << "Bus error" << EndDebugError;
		}
		Stop(-1);
		return;
	}
	SetSRR0(GetNIA()); // save NIA
		
	// clear SRR1[0-15], SRR1[16-31]=MSR[16-31]
	SetSRR1(GetMSR() & 0x0000ffffUL);
	
	if(HasMCP())
	{
		// set SRR1[12]
		SetSRR1(GetSRR1() | 0x00080000UL);
		ResetIRQ(CONFIG::IRQ_MCP);
	}
	
	if(HasTEA())
	{
		// set SRR1[13]
		SetSRR1(GetSRR1() | 0x00040000UL);
		ResetIRQ(CONFIG::IRQ_TEA);
	}
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));

	Branch(CONFIG::EXC_MACHINE_CHECK_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
		logger << DebugInfo << exc.what() << endl << EndDebugInfo;
}

/* Decrementer exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const DecrementerException<CONFIG>& exc)
{
	if(linux_os_import) return; // silently ignore the IRQ when Linux ABI translation is enabled
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	Branch(CONFIG::EXC_DECREMENTER_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
		logger << DebugInfo << "bus cycle " << bus_cycle << ": " << exc.what() << endl << EndDebugInfo;
	
	ResetIRQ(CONFIG::IRQ_DECREMENTER_OVERFLOW);
}

/* External interrupt exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const ExternalInterruptException<CONFIG>& exc)
{
	if(linux_os_import) return; // silently ignore the IRQ when Linux ABI translation is enabled
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	Branch(CONFIG::EXC_EXTERNAL_INTERRUPT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
		logger << DebugInfo << exc.what() << endl << EndDebugInfo;

	ResetIRQ(CONFIG::IRQ_EXTERNAL_INTERRUPT);
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const PerformanceMonitorInterruptException<CONFIG>& exc)
{
	if(linux_os_import) return; // silently ignore the IRQ when Linux ABI translation is enabled
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	Branch(CONFIG::EXC_PERFORMANCE_MONITOR_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	ResetIRQ(CONFIG::IRQ_PERFORMANCE_MONITOR_INTERRUPT);
	
	if(unlikely(IsVerboseException()))
		logger << DebugInfo << exc.what() << endl << EndDebugInfo;
}

/* System management interrupt exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemManagementInterruptException<CONFIG>& exc)
{
	if(linux_os_import) return; // silently ignore the IRQ when Linux ABI translation is enabled
	SetSRR0(GetNIA()); // save NIA
	
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	Branch(CONFIG::EXC_SYSTEM_MANAGEMENT_INTERRUPT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	ResetIRQ(CONFIG::IRQ_SMI);

	if(unlikely(IsVerboseException()))
		logger << DebugInfo << exc.what() << endl << EndDebugInfo;
}

/* ISI exception */
// SRR1 indicates to the operating system the cause of the ISI exception:
//
//   0   1   2   3   4   
// +---+---+---+---+---+--
// | ? |   | 0 |   |   | ...
// +-+-+-+-+---+-+-+-+-+--
//       |       |   |
//       |       |   +-----> protection violation
//       |       |
//       |       +---------> either direct store exception (SR[T]=1)
//       |                       or no execute segment (SR[NOEXECUTE]=1)
//       |                       or guarded memory (PTE[G]=1 or DBAT[G]=1)
//       |
//       +-----------------> page fault
template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISIException<CONFIG>& exc)
{
	if(linux_os_import)
	{
		logger << DebugError << exc.what() << EndDebugError;
		Stop(-1);
		return;
	}
	
	SetSRR0(GetCIA()); // save CIA
	
	switch(exc.GetType())
	{
		case ISIException<CONFIG>::ISI_PROTECTION_VIOLATION:
			// keep SRR1[0], clear SRR1[1-3], set SRR1[4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
			// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
			SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x80000000UL); 
			break;
		case ISIException<CONFIG>::ISI_NO_EXECUTE:
		case ISIException<CONFIG>::ISI_DIRECT_STORE:
		case ISIException<CONFIG>::ISI_GUARDED_MEMORY:
			// keep SRR1[0], clear SRR1[1-2], set SRR1[3], clear SRR1[4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
			// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
			SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x10000000UL); 
			break;
		case ISIException<CONFIG>::ISI_PAGE_FAULT:
			// keep SRR1[0], set SRR1[1], clear SRR1[2-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
			// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
			SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x40000000UL); 
			break;
	}
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	Branch(CONFIG::EXC_ISI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));

	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) logger << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << endl;
		logger << EndDebugInfo;
	}
}

/* DSI exception */
// DAR indicates the faulting address to the operating system
// DSISR indicates to the operating system the cause of the DSI exception:
//
//   0   1   2   3   4   5   6   7   8   9   10  11  12 .... 31
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   | 0 | 0 |   |   |   | 0 | 0 |   | 0 |   | 0  .... 0 |
// +-+-+-+-+---+---+-+-+-+-+-+-+---+---+-+-+---+-+-+---+---+---+
//   |   |           |   |   |           |       |
//   |   |           |   |   |           |       +----> eciwx/ecowx and EAR[E]=0
//   |   |           |   |   |           |
//   |   |           |   |   |           +------------> DABR match
//   |   |           |   |   |
//   |   |           |   |   +------------------------> 1 for a store, 0 for a load
//   |   |           |   |
//   |   |           |   +----------------------------> write through or cache inhibited (lwarx, stwcx.)
//   |   |           |
//   |   |           +--------------------------------> protection violation
//   |   |
//   |   +--------------------------------------------> page fault
//   |
//   +------------------------------------------------> direct store exception (load/store)
template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIException<CONFIG>& exc)
{
	if(linux_os_import)
	{
		logger << DebugError << exc.what() << EndDebugError;
		Stop(-1);
		return;
	}
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
	
	switch(exc.GetType())
	{
		case DSIException<CONFIG>::DSI_DIRECT_STORE:
			if(exc.GetAccessType() == CONFIG::MAT_WRITE)
				SetDSISR(0x82000000UL); // set DSISR[0], DSISR[6], clear other bits
			else
				SetDSISR(0x80000000UL); // set DSISR[0], clear other bits
			break;
		case DSIException<CONFIG>::DSI_PROTECTION_VIOLATION:
			if(exc.GetAccessType() == CONFIG::MAT_WRITE)
				SetDSISR(0x0a000000UL); // set DSISR[4], set DSISR[6], clear other bits
			else
				SetDSISR(0x08000000UL); // set DSISR[4], clear other bits
			break;
		case DSIException<CONFIG>::DSI_PAGE_FAULT:
			if(exc.GetAccessType() == CONFIG::MAT_WRITE)
				SetDSISR(0x42000000UL); // set DSISR[1], set DSISR[6], clear other bits
			else
				SetDSISR(0x40000000UL); // set DSISR[1], clear other bits
			break;
		case DSIException<CONFIG>::DSI_DATA_ADDRESS_BREAKPOINT:
			if(exc.GetAccessType() == CONFIG::MAT_WRITE)
				SetDSISR(0x02400000UL); // set DSISR[6], set DSISR[9], clear other bits
			else
				SetDSISR(0x00400000UL); // set DSISR[9], clear other bits
			break;
		case DSIException<CONFIG>::DSI_EXTERNAL_ACCESS_DISABLED:
			if(exc.GetAccessType() == CONFIG::MAT_WRITE)
				SetDSISR(0x02100000UL); // set DSISR[6], DSISR[11], clear other bits
			else
				SetDSISR(0x02100000UL); // set DSISR[11], clear other bits
			break;
		case DSIException<CONFIG>::DSI_WRITE_THROUGH_LINKED_LOAD_STORE:
			if(exc.GetAccessType() == CONFIG::MAT_WRITE)
				SetDSISR(0x06000000UL); // set DSISR[5], DSISR[6], clear other bits
			else
				SetDSISR(0x04000000UL); // set DSISR[5], clear other bits
			break;
	}
		
	Branch(CONFIG::EXC_DSI_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(func_symbol) logger << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << " while " << (exc.GetAccessType() == CONFIG::MAT_WRITE ? "writing" : "reading")
				<< " data at 0x" << std::hex << exc.GetAddress() << std::dec;
		const Symbol<typename CONFIG::address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) logger << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		logger << endl;
		logger << EndDebugInfo;
	}
}

/* Alignment exception */
// DSISR helps the operating system to implement misaligned memory accesses not handled in the hardware:
//
//   0 ..... 14  15  16  17  18  29  20  21  22  23  24  25  26  27  28  29  30  31
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// | 0 ..... 0 |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+-+-+-+-+-+-+-+-+---+---+-+-+-+-+---+---+---+-+-+-+-+---+---+---+-+-+
//               |   |   |   |           |   |               |   |               |
//               +-+-+   |   +-----+-----+   +-------+-------+   +-------+-------+
//                 |     |         |                 |                   |
//                 |     |         |                 |                   +-----------> bits 11-15 of instruction encoding
//                 |     |         |                 |                                 for update forms, and lmw, lswi, lswx
//                 |     |         |                 |
//                 |     |         |                 +-------------------------------> bits 6-10 of instruction encoding. undefined for dcbz
//                 |     |         |
//                 |     |         +-------------------------------------------------> index addressing: bits 21-24 of instruction encoding
//                 |     |                                                             immediate index addressing: bits 1-4 of instruction encoding
//                 |     |
//                 |     +-----------------------------------------------------------> index addressing: bits 25 of instruction encoding
//                 |                                                                   immediate index addressing: bits 5 of instruction encoding
//                 |
//                 +-----------------------------------------------------------------> index addressing: bits 29-30 of instruction encoding
//                                                                                     immediate index addressing: cleared
template <class CONFIG>
void CPU<CONFIG>::HandleException(const AlignmentException<CONFIG>& exc, uint32_t instruction_encoding)
{
	if(linux_os_import)
	{
		logger << DebugError << "Data storage interrupt: misalignment" << EndDebugError;
		Stop(-1);
		return;
	}
	SetDAR(exc.GetAddress()); // DAR=effective address
	
	SetSRR0(GetCIA()); // save CIA
		
	// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
	// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); 
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	// clear DSISR[0-14]
	SetDSISR(GetDSISR() & 0x0001ffffUL);
	
	// Look at primary opcode (bits 0-5)
	bool xform = ((instruction_encoding >> 26) & 0x3fUL) == 31;
		
	if(xform)
	{
		// X-Form: index addressing
		
		// copy bits 29-30 of instruction encoding to DSISR[15-16]
		SetDSISR((GetDSISR() & 0xfffe7fffUL) | ((instruction_encoding << 14) & 0x00018000UL));
		
		 // copy bit 25 of instruction encoding to bits 17 of DSISR
		SetDSISR((GetDSISR() & 0xffffbfffUL) | ((instruction_encoding << 8) & 0x00004000UL));
		
		 // copy bits 21-24 of instruction encoding to bits 18-21 of DSISR
		SetDSISR((GetDSISR() & 0xffffc3ffUL) | ((instruction_encoding << 3) & 0x00003c00UL));
	}
	else
	{
		// immediate index addressing
		
		// clear DSISR[15-16]
		SetDSISR(GetDSISR() & 0xfffe7fffUL);
		
		// copy bit 5 of instruction encoding to DSISR[17]
		SetDSISR((GetDSISR() & 0xffffbfffUL) | ((instruction_encoding >> 12) & 0x00004000UL));
		
		// copy bits 1-4 of instruction encoding to DSISR[18-21]
		SetDSISR((GetDSISR() & 0xffffc3ffUL) | ((instruction_encoding >> 17) & 0x00003c00UL));
	}
		
	// copy bits 6-10 of instruction encoding to DSISR[22-26]
	SetDSISR((GetDSISR() & 0xfffffc1fUL) | ((instruction_encoding >> 16) & 0x000003e0UL));
		
	// copy bits 11-15 of instruction encoding into DSISR[27-31]
	SetDSISR((GetDSISR() & 0xffffffe0UL) | ((instruction_encoding >> 16) & 0x0000001fUL));
		
	Branch(CONFIG::EXC_ALIGNMENT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(func_symbol) logger << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << " while accessing data at 0x" << std::hex << exc.GetAddress() << std::dec;
		const Symbol<typename CONFIG::address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) logger << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		logger << endl;
		logger << EndDebugInfo;
	}
}

/* Program exceptions */
// SRR1 indicates to the operating system the cause of the program exception:
//
//   0   1 ....   4   5 ....  9  10  11  12  13  14  15                             
// +---+---+---+---+---+---+---+---+---+---+---+---+---+--
// | ? | 0 ....  0 | ?  .... ? | 0 |   |   |   |   |   | ...
// +---+---+---+-+-+-+-+-+-+-+-+---+-+-+-+-+-+-+-+-+-+-+--
//                                   |   |   |   |   |
//                                   |   |   |   |   +----> cleared if SSR0=address of faulting instruction
//                                   |   |   |   |          set if subsequent instructions
//                                   |   |   |   |
//                                   |   |   |   +--------> trap
//                                   |   |   |
//                                   |   |   +------------> privilege violation
//                                   |   | 
//                                   |   +----------------> illegal instruction
//                                   |
//                                   +--------------------> IEEE floating point exception
template <class CONFIG>
void CPU<CONFIG>::HandleException(const ProgramException<CONFIG>& exc)
{
	if(linux_os_import)
	{
		logger << DebugError << exc.what() << EndDebugError;
		Stop(-1);
		return;
	}
	SetSRR0(GetCIA()); // save CIA
	
	switch(exc.GetType())
	{
		case ProgramException<CONFIG>::PX_UNIMPLEMENTED_INSTRUCTION:
			// should never occur as all supported instructions but altivec instructions are implemented and if such exist there's no way to tell it to the software
			logger << DebugError << "Unimplemented instruction at 0x" << std::hex << GetCIA() << std::dec << EndDebugError;
			Stop(-1);
			break;
		case ProgramException<CONFIG>::PX_ILLEGAL_INSTRUCTION:
			// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-11], set SRR1[12], clear SRR1[13-15],
			// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
			SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00080000UL); 
			break;
		case ProgramException<CONFIG>::PX_PRIVILEGE_VIOLATION:
			// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-12], set SRR1[13], clear SRR1[14-15],
			// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
			SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00040000UL); 
			break;
		case ProgramException<CONFIG>::PX_TRAP:
			// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10-13], set SRR1[14], clear SRR1[15],
			// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
			SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00020000UL); 
			break;
		case ProgramException<CONFIG>::PX_FLOATING_POINT:
			// keep SRR1[0], clear SRR1[1-4], keep SRR1[5-9], clear SRR1[10], set SRR1[11], clear SRR1[12-15],
			// SRR1[16-23]=MSR[16-23], keep SRR1[24], SRR1[25-27]=MSR[25-27], keep SRR1[28-29], SRR1[30-31]=MSR[30-31]
			SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL) | 0x00100000UL); 
			break;
	}
	
	// MSR[LE]=MSR[ILE], MSR[POW]=0, MSR[EE]=0, MSR[PR]=0, MSR[FP]=0, MSR[FE0]=0,
	// MSR[SE]=0, MSR[BE]=0, MSR[FE1]=0, MSR[IR]=0, MSR[DR]=0, MSR[RI]=0
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1));
		
	Branch(CONFIG::EXC_PROGRAM_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) logger << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << endl;
		logger << EndDebugInfo;
	}
}

/* Floating point unavailable exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const FloatingPointUnavailableException<CONFIG>& exc)
{
	if(linux_os_import)
	{
		logger << DebugError << "Floating-point unavailable exception" << EndDebugError;
		Stop(-1);
		return;
	}
	SetSRR0(GetCIA()); // save CIA
	
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], SRR1[10-15], copy MSR[16-23], MSR[25-27], and MSR[30-31]
	
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
	
	Branch(CONFIG::EXC_FLOATING_POINT_UNAVAILABLE_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) logger << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << endl;
		logger << EndDebugInfo;
	}
}

/* System call exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemCallException<CONFIG>& exc)
{
	if(unlikely(IsVerboseException() || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();

		if(unlikely(enable_linux_syscall_snooping))
		{
			sstr << ": ";
			
			switch(GetGPR(0))
			{
				case 1:
					sstr << "exit(0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 2:
					sstr << "fork()";
					break;
			
				case 3:
					sstr << "read(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec
							<< ",count=" << GetGPR(5) << ");";
					break;
			
				case 4:
					{
						int fd; 
		
						fd = GetGPR(3);
						
						sstr << "write(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec;
						if((fd == 1) || (fd == 2))
						{
							std::string str = ReadString(GetGPR(4), GetGPR(5));
							sstr << " " << str;
						}
						
						sstr << ",count=" << GetGPR(5) << ");";
					}
					break;
					
				case 5:
					sstr << "open(pathname=" << ReadString(GetGPR(3)) << ",flags=0x" << std::hex << GetGPR(4) 
								<< ",mode=0x" << GetGPR(5) << std::dec << ");";
					break;
				
				case 6:
					sstr << "close(fd=" << (signed) GetGPR(3) << ");";
					break;
					
				case 7:
					sstr << "waitpid(pid=" << (int32_t) GetGPR(3) << ",status=@0x" << std::hex << GetGPR(4) << std::dec << ",options=0x" << std::hex << GetGPR(5) << std::dec << ");";
					break;
					
				case 8:
					sstr << "creat(filename=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 9:
					sstr << "link(old_path=" << ReadString(GetGPR(3)) << ",new_path=" << ReadString(GetGPR(4)) << ");";
					break;
					
				case 10:
					sstr << "unlink(path=" << ReadString(GetGPR(3)) << ");";
					break;

				case 11:
					sstr << "execve(filename=" << ReadString(GetGPR(3)) << ",argv=@0x" << std::hex << GetGPR(4) << ",envp=@0x" << GetGPR(5) << std::dec << ");";
					break;
					
				case 12:
					sstr << "chdir(path=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 13:
					sstr << "time(t=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;

				case 14:
					sstr << "mknod(pathname=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << ",dev=0x" << GetGPR(5) << std::dec << ");";
					break;
					
				case 15:
					sstr << "chmod(pathname=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
				
				case 16:
					sstr << "lchown(path=" << ReadString(GetGPR(3)) << ",owner=" << GetGPR(4) << ",group=" << GetGPR(5) << ");";
					break;
					
				case 18:
					sstr << "oldstat(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 19:
					sstr << "lseek(fd=" << (signed) GetGPR(3) << ",offset=" << (signed) GetGPR(4) << ",whence=" << (signed) GetGPR(5) << ");";
					break;
					
				case 20:
					sstr << "getpid();";
					break;
					
				case 21:
					sstr << "mount(source=" << ReadString(GetGPR(3)) << ",target=" << ReadString(GetGPR(4)) << ",filesystemtype="
							<< ReadString(GetGPR(5)) << ",mountflags=0x" << std::hex << GetGPR(6) << ",data=@0x"
							<< GetGPR(7) << std::dec << ");";
					break;
				
				case 22:
					sstr << "umount(target=" << ReadString(GetGPR(4)) << ");";
					break;
					
				case 23:
					sstr << "setuid(uid=" << GetGPR(3) << ");";
					break;
					
				case 24:
					sstr << "getuid();";
					break;
					
				case 25:
					sstr << "stime(t=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
				
				case 26:
					sstr << "ptrace(request=" << GetGPR(3) << ",pid=" << GetGPR(4) << ",addr=@" << std::hex << GetGPR(5) << std::dec << ",data=@" << std::hex << GetGPR(6) << std::dec << ");";
					break;

				case 27:
					sstr << "alarm(seconds=" << GetGPR(3) << ");";
					break;

				case 28:
					sstr << "oldfstat(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;

				case 29:
					sstr << "pause();";
					break;
					
				case 30:
					sstr << "utime(filename=" << ReadString(GetGPR(3)) << ",times=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;

				case 33:
					sstr << "access(pathname=" << ReadString(GetGPR(3))
						<< ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 34:
					sstr << "nice(inc=" << (signed) GetGPR(3) << ");";
					break;

				case 35:
					sstr << "ftime(tp=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 36:
					sstr << "sync();";
					break;
					
				case 37:
					sstr << "kill(pid=" << GetGPR(3) << ",sig=" << (signed) GetGPR(4) << ");";
					break;
					
				case 38:
					sstr << "rename(oldpath=" << ReadString(GetGPR(3)) << ",newpath=" << ReadString(GetGPR(4)) << ");";
					break;

				case 39:
					sstr << "mkdir(pathname=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 40:
					sstr << "rmdir(pathname=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 41:
					sstr << "dup(oldfd=" << (signed) GetGPR(3) << ");";
					break;
				
				case 42:
					sstr << "pipe(pipefd=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 43:
					sstr << "times(buf=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 45:
					sstr << "brk(end_data_segment=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 46:
					sstr << "setgid(gid=" << GetGPR(3) << ");";
					break;
				
				case 47:
					sstr << "getgid();";
					break;
					
				case 48:
					sstr << "signal(signum=" << (signed) GetGPR(3) << ",handler=@" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 49:
					sstr << "geteuid();";
					break;
					
				case 50:
					sstr << "getegid();";
					break;
					
				case 51:
					sstr << "acct(filename=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 52:
					sstr << "umount2(target=" << ReadString(GetGPR(3)) << ",flags=" << (signed) GetGPR(4) << ");";
					break;
					
				case 54:
					sstr << "ioctl(d=" << (signed) GetGPR(3) << ",request=" << (signed) GetGPR(4) << ",argp=@" << std::hex << GetGPR(5) << std::dec << ");";
					break;
				
				case 55:
					sstr << "fcntl(fd=" << (signed) GetGPR(3) << ",cmd=0x" << std::hex << GetGPR(4) << std::dec << ",arg=@0x" << std::hex << GetGPR(5) << std::dec << ");";
					break;
					
				case 57:
					sstr << "setpgid(pid=" << GetGPR(3) << ",pgid=" << GetGPR(4) << ");";
					break;
					
				case 58:
					sstr << "ulimit(cmd=" << (signed) GetGPR(3) << ",newlimit=" << (signed) GetGPR(4) << ");";
					break;

				case 59:
					sstr << "oldolduname(buf=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;

				case 60:
					sstr << "umask(mask=0" << std::oct << GetGPR(3) << std::dec << ");";
					break;
					
				case 61:
					sstr << "chroot(path=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 62:
					sstr << "ustat(dev=" << GetGPR(3) << ",ubuf=@" << GetGPR(4) << ");";
					break;
					
				case 63:
					sstr << "dup2(old=" << (signed) GetGPR(3) << ",new=" << (signed) GetGPR(4) << ");";
					break;
					
				case 64:
					sstr << "getppid();";
					break;
				
				case 65:
					sstr << "getpgrp();";
					break;
					
				case 66:
					sstr << "setsid();";
					break;
					
				case 78:
					sstr << "gettimeofday(tv=@" << GetGPR(3) << ",tz=@" << GetGPR(4) << ");";
					break;
					
				case 88:
					sstr << "reboot(magic=" << (signed) GetGPR(3) << ",magic2=" << (signed) GetGPR(4) << ",cmd=" << (signed) GetGPR(5) << ",arg=@0x" << std::hex << GetGPR(6) << std::dec << ");";
					break;
					
				case 90:
					sstr << "mmap(addr=@0x" << std::hex << GetGPR(3) << std::dec << ",length=" << GetGPR(4) << ",prot=0x"
							<< std::hex << GetGPR(5) << ",flags=0x" << GetGPR(6) << std::dec << ",fd=" << (signed) GetGPR(7)
							<< ",offset=" << GetGPR(8) << ");";
					break;
					
				case 91:
					sstr << "munmap(addr=@0x" << std::hex << GetGPR(3) << std::dec << ",length=" << GetGPR(4) << ");";
					break;
					
				case 102:
					sstr << "socketcall(call=" << (signed) GetGPR(3) << ",args=@0x" << GetGPR(4) << ");";
					break;
					
				case 106:
					sstr << "stat(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 114:
					sstr << "wait4(pid=" << GetGPR(3) << ",status=@0x" << std::hex << GetGPR(4) << ",options=0x" << GetGPR(5) << ",rusage=@0x" << GetGPR(6) << std::dec << ");";
					break;
				
				case 120:
					sstr << "clone(fn=0x" << std::hex << GetGPR(3) << ",child_stack=0x" << GetGPR(4) << ",flags=0x" << GetGPR(5) << ",arg=@0x" << GetGPR(6) << std::dec << ");";
					break;
					
				case 122:
					sstr << "uname(buf=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 140:
					sstr << "llseek(fd=" << (signed) GetGPR(3) << ",offset_high=" << GetGPR(4) << ",offset_low=" << GetGPR(5) << ",result=@0x" << GetGPR(6) << ",whence=" << GetGPR(7) << ");";
					break;
					
				case 141:
					sstr << "getdents(fd=" << (signed) GetGPR(3) << ",dirp=@0x" << std::hex << GetGPR(4) << std::dec << ",count=" << GetGPR(5) << ");";
					break;
					
				case 146:
					sstr << "writev(fd=" << (signed) GetGPR(3) << ",iov=@0x" << std::hex << GetGPR(4) << std::dec << ",iovcnt=" << (signed) GetGPR(5) << ");";
					break;
					
				case 162:
					sstr << "nanosleep(req=@" << std::hex << GetGPR(3) << std::dec << ",rem=@" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 167:
					sstr << "poll(fds=@" << std::hex << GetGPR(3) << std::dec << ",nfds=" << GetGPR(4) << ",timeout=" << (signed) GetGPR(5) << ");";
					break;
					
				case 173:
					sstr << "rt_sigaction(signum=" << (signed) GetGPR(3) << ",act=@0x" << std::hex << GetGPR(4) << ",oldact=@0x" << GetGPR(5) << std::dec << ",sigsetsize=" << GetGPR(6) << ");";
					break;
					
				case 174:
					sstr << "rt_sigprocmask(how=" << (signed) GetGPR(3) << ",set=@0x" << std::hex << GetGPR(4) << ",oldset=@0x" << GetGPR(5) << std::dec << ",sigsetsize=" << GetGPR(6) << ");";
					break;
					
				case 182:
					sstr << "getcwd(buf=@0x" << std::hex << GetGPR(3) << std::dec << ",size=" << GetGPR(4) << ");";
					break;
					
				case 189:
					sstr << "vfork();";
					break;
					
				case 195:
					sstr << "stat64(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 196:
					sstr << "lstat64(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 197:
					sstr << "fstat64(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 202:
					sstr << "getdents64(fd=" << (signed) GetGPR(3) << ",dirp=@0x" << std::hex << GetGPR(4) << std::dec << ",count=" << GetGPR(5) << ");";
					break;
					
				case 204:
					sstr << "fcntl64(fd=" << (signed) GetGPR(3) << ",cmd=0x" << std::hex << GetGPR(4) << std::dec << ",arg=@0x" << std::hex << GetGPR(5) << std::dec << ");";
					break;
					
				case 207:
					sstr << "gettid();";
					break;
					
				case 208:
					sstr << "tkill(tid=" << (signed) GetGPR(3) << ",sig=" << (signed) GetGPR(4) << ");";
					break;

				case 234:
					sstr << "exit_group(" << (signed) GetGPR(3) << ");";
					break;
					
				case 250:
					sstr << "tgkill(tgid=" << (signed) GetGPR(3) << ",tid=" << (signed) GetGPR(4) << ",sig=" << (signed) GetGPR(5) << ");";
					break;
					
				default:
					sstr << "unknown_syscall_" << GetGPR(0) << "();";
					break;
			}
		}
		
		std::string msg = sstr.str();
		
		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		//if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}

	if(linux_os_import)
	{
		// Do Linux ABI translation
		linux_os_import->ExecuteSystemCall(GetGPR(0));
	}
	else
	{
		SetSRR0(GetNIA()); // save NIA
			
		SetSRR1((GetSRR1() & 0x87c008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], clear SRR1[10-15], save MSR[16-23], MSR[25-27], MSR[30-31]
					
		SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
			
		Branch(CONFIG::EXC_SYSTEM_CALL_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	}
}

/* Trace exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const TraceException<CONFIG>& exc)
{
	if(linux_os_import) return; // silently ignore trace exception when Linux ABI translation is enabled
	SetSRR0(GetNIA()); // save NIA
	
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], SRR1[10-15], copy MSR[16-23], MSR[25-27], and MSR[30-31]
	
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
	
	Branch(CONFIG::EXC_TRACE_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) logger << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << endl;
		logger << EndDebugInfo;
	}
}

/* Instruction Address Breakpoint exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const InstructionAddressBreakpointException<CONFIG>& exc)
{
	if(linux_os_import) return; // silently ignore the instruction address breakpoint exception when Linux ABI translation is enabled
	SetSRR0(GetCIA()); // save CIA
	
	SetSRR1((GetSRR1() & 0x87c0008cUL) | (GetMSR() & 0x0000ff73UL)); // clear SRR1[1-4], SRR1[10-15], copy MSR[16-23], MSR[25-27], and MSR[30-31]
	
	SetMSR((GetMSR() & 0x00011040UL) | ((GetMSR() >> 16) & 1)); // copy MSR[ILE] into MSR[LE], clear MSR[POW], MSR[EE], MSR[PR], MSR[FP], MSR[FE0], MSR[SE], MSR[BE], MSR[FE1], MSR[IR], MSR[DR] and MSR[RI]
	
	Branch(CONFIG::EXC_INSTRUCTION_ADDRESS_BREAKPOINT_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) logger << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << endl;
		logger << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const TLBMissException<CONFIG>& exc)
{
	if(linux_os_import)
	{
		logger << DebugError << "TLB Miss exception" << EndDebugError;
		Stop(-1);
		return;
	}
	SetSRR0(GetCIA()); // save CIA
	
	uint32_t pte_hi = ((exc.GetVSID() & 0x00ffffffUL) << 7) | (exc.GetAPI() & 0x3fUL) | 0x80000000UL;
	
	// SRR1[0-3]=CR0, clear SRR1[4-11], SRR1[12]=key, SRR1[13]=0, SRR1[14]=way, SRR1[15]=0, SRR1[16-31]=MSR[16-31]
	SetSRR1((GetCR() & 0xf0000000UL) | ((exc.GetKey() & 1) << 19) | ((exc.GetWay() & 1) << 17) | (GetMSR() & 0x0000ffffUL));
	
	if(exc.GetMemoryType() == CONFIG::MT_INSN)
	{
		SetTLBMISS((exc.GetAddress() & 0xfffffffeUL) | (exc.GetWay() & 1));
		SetPTEHI(pte_hi);
		// SRR1[13]=1
		SetSRR1(GetSRR1() | 0x00040000UL);
		Branch(CONFIG::EXC_ITLB_MISS_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
	}
	else
	{
		SetTLBMISS((exc.GetAddress() & 0xfffffffeUL) | (exc.GetWay() & 1));
		SetPTEHI(pte_hi);
		if(exc.GetMemoryAccessType() == CONFIG::MAT_WRITE)
		{
			// SRR1[15]=1
			SetSRR1(GetSRR1() | 0x00010000UL);
			Branch(CONFIG::EXC_DTLB_STORE_MISS_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
		}
		else
		{
			Branch(CONFIG::EXC_DTLB_LOAD_MISS_VECTOR | (GetMSR_IP() ? 0xfff00000UL : 0x00000000UL));
		}
	}
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo;
		logger << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) logger << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		logger << ":" << exc.what() << endl;
		logger << EndDebugInfo;
	}
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
