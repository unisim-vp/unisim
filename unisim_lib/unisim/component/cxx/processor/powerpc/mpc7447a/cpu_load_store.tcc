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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_LOAD_STORE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_LOAD_STORE_TCC__

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
template <bool TRANSLATE_ADDR>
void CPU<CONFIG>::EmuLoad(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;

	memset(buffer, 0, size);
	
	if(TRANSLATE_ADDR && GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ;
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}
	
	if(likely(!(wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled()))
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);

		if(unlikely(!l1_access.line))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}
			
			EmuEvictDL1(l1_access);
		}

		if(unlikely(!l1_access.block))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillDL1(l1_access, wimg, false /* not a rwitm */);
		}

		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);
		//UpdateReplacementPolicyDL1(l1_access);

		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 disabled
		BusRead(physical_addr, buffer, size, wimg);
	}
	if(unlikely(IsVerboseLoad()))
	{
		uint32_t i;
		logger << DebugInfo << "instruction #" << instruction_counter << " (at 0x" << std::hex << GetCIA() << std::dec << "): Loading ";
		for(i = 0; i < size; i++)
		{
			uint8_t value = ((uint8_t *) buffer)[i];
			uint8_t l = value & 15;
			uint8_t h = value >> 4;
			logger << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10) << " ";
		}
		logger << "(" << size << " bytes) at 0x" << std::hex << addr << std::dec << endl << EndDebugInfo;
	}
}

template <class CONFIG>
template <bool TRANSLATE_ADDR>
void CPU<CONFIG>::EmuStore(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	if(unlikely(CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_PRINTK_ENABLE && enable_linux_printk_snooping))
	{
		if(unlikely(enable_linux_printk_snooping))
		{
			if(linux_printk_buf_addr && (addr >= linux_printk_buf_addr) && (addr < (linux_printk_buf_addr + linux_printk_buf_size)))
			{
				uint32_t i;
				//cout << "\033[31m";
				for(i = 0; i < size; i++)
				{
					char c = ((const char *) buffer)[i];
					if(c != 0) {
							//cerr << c;
							cout << c;
					}
				}
				//cout << "\033[37m";
			}
		}
	}

	if(unlikely(IsVerboseStore()))
	{
		uint32_t i;
		logger << DebugInfo << "insn #" << instruction_counter << ":Storing ";
		for(i = 0; i < size; i++)
		{
			uint8_t value = ((uint8_t *) buffer)[i];
			uint8_t l = value & 15;
			uint8_t h = value >> 4;
			logger << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10) << " ";
		}
		logger << "(" << size << " bytes) at 0x" << std::hex << addr << std::dec << endl << EndDebugInfo;
	}
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(TRANSLATE_ADDR && GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE;
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	if(likely(!(wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled()))
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);

		if(unlikely(!l1_access.line))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}

			EmuEvictDL1(l1_access);
		}
		
		if(unlikely(!l1_access.block))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillDL1(l1_access, wimg, true);
		}
	
		// DL1 hit
		memcpy(&(*l1_access.block)[l1_access.offset], buffer, size);
		l1_access.block->status.dirty = true;
		//UpdateReplacementPolicyDL1(l1_access);

		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportWriteAccess();

		if(unlikely(wimg & CONFIG::WIMG_WRITE_THROUGH))
		{
			if(likely(IsL2CacheEnabled()))
			{
				// DL1 block fill from L2
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
				l2_access.addr = physical_addr;

				LookupL2(l2_access);

				memcpy(&(*l2_access.block)[l2_access.offset], buffer, size);
				l2_access.block->status.dirty = true;
				//UpdateReplacementPolicyL2(l2_access);
				if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportWriteAccess();
			}
			BusWrite(physical_addr, buffer, size, wimg);
		}
	}
	else
	{
		// DL1 disabled
		BusWrite(physical_addr, buffer, size, wimg);
	}
}

template <class CONFIG>
template <class T>
inline void CPU<CONFIG>::EmuLoad(T& value, typename CONFIG::address_t ea)
{
	// Data Address Breakpoint handling
	if(CONFIG::DABR_ENABLE)
	{
		if(unlikely(GetDABR_DR() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT()))
		{
			throw DSIDataAddressBreakpointException<CONFIG>(ea, CONFIG::MAT_READ);
		}
	}

	// Address munging
	typename CONFIG::address_t munged_ea = MungEffectiveAddress(ea, sizeof(T));

	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (munged_ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory load does not cross a FSB boundary
		EmuLoad<true>(munged_ea, &value, sizeof(T));
	}
	else
	{
		// Memory load crosses a FSB boundary
		EmuLoad<true>(munged_ea, &value, size_to_fsb_boundary);
		EmuLoad<true>(munged_ea + size_to_fsb_boundary, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary);
	}
}

template <class CONFIG>
template <class T>
inline void CPU<CONFIG>::EmuStore(T value, typename CONFIG::address_t ea)
{
	// Data Address	Breakpoint handling
	if(CONFIG::DABR_ENABLE)
	{
		if(unlikely(GetDABR_DW() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT()))
		{
			throw DSIDataAddressBreakpointException<CONFIG>(ea, CONFIG::MAT_WRITE);
		}
	}

	// Address munging
	typename CONFIG::address_t munged_ea = MungEffectiveAddress(ea, sizeof(T));

	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (munged_ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory store does not cross a FSB boundary
		EmuStore<true>(munged_ea, &value, sizeof(T));
	}
	else
	{
		// Memory store crosses a FSB boundary
		EmuStore<true>(munged_ea, &value, size_to_fsb_boundary);
		EmuStore<true>(munged_ea + size_to_fsb_boundary, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorLoad(typename CONFIG::address_t ea, uint32_t size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<typename CONFIG::address_t>::MAT_READ, MemoryAccessReporting<typename CONFIG::address_t>::MT_DATA, ea, size);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorStore(typename CONFIG::address_t ea, uint32_t size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<typename CONFIG::address_t>::MAT_WRITE, MemoryAccessReporting<typename CONFIG::address_t>::MT_DATA, ea, size);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Int8Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT8_LOAD, rd, MungEffectiveAddress(ea, 1), 1);
#else
	uint8_t value;
	EmuLoad<uint8_t>(value, ea);
	gpr[rd] = (uint32_t) value; // 8-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_LOAD, rd, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value;
	EmuLoad<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) BigEndian2Host(value); // 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::SInt16Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::SINT16_LOAD, rd, ea, 2);
#else
	uint16_t value;
	EmuLoad<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) (int16_t) BigEndian2Host(value); // 16-bit to 32-bit sign extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_LOAD, rd, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value;
	EmuLoad<uint32_t>(value, ea);
	gpr[rd] = BigEndian2Host(value);
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp32Load(unsigned int fd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP32_LOAD, fd, MungEffectiveAddress(ea, 4), 4);
#else
	// check alignment
	if(unlikely(ea & 3))
	{
		if(!linux_os_import) throw AlignmentException<CONFIG>(ea);
	}
	uint32_t value;
	EmuLoad<uint32_t>(value, ea);
	Flags flags;
	flags.setRoundingMode((fpscr & CONFIG::FPSCR_RN_MASK) >> CONFIG::FPSCR_RN_OFFSET);
	fpr[fd].assign(SoftFloat(BigEndian2Host(value)), flags);
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp64Load(unsigned int fd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP64_LOAD, fd, MungEffectiveAddress(ea, 8), 8);
#else
	// check alignment
	if(unlikely(ea & 3))
	{
		if(!linux_os_import) throw AlignmentException<CONFIG>(ea);
	}
	uint64_t value;
	EmuLoad<uint64_t>(value, ea);
	fpr[fd] = SoftDouble(BigEndian2Host(value));
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE, rd, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value;
	EmuLoad<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) LittleEndian2Host(value); // reverse bytes and 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE, rd, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value;
	EmuLoad<uint32_t>(value, ea);
	gpr[rd] = LittleEndian2Host(value); // reverse bytes
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size)
{
	if(unlikely(GetMSR_LE()))
	{
		if(!linux_os_import) throw AlignmentException<CONFIG>(ea);
	}

#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT_LOAD_MSB, rd, ea, size);
#else
	switch(size)
	{
		case 1:
		{
			uint8_t value;
			EmuLoad<uint8_t>(value, ea);
			gpr[rd] = (uint32_t) value << 24;
			break;
		}

		case 2:
		{
			uint16_t value;
			EmuLoad<uint16_t>(value, ea);
			gpr[rd] = (uint32_t) BigEndian2Host(value) << 16;
			break;
		}

		case 3:
		{
			uint8_t buffer[3];
			EmuLoad<typeof(buffer)>(buffer, ea);
			uint32_t value = ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8);
			gpr[rd] = value;
			break;
		}

		case 4:
		{
			uint32_t value;
			EmuLoad<uint32_t>(value, ea);
			gpr[rd] = BigEndian2Host(value);
			break;
		}
	}
	MonitorLoad(ea, size);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int8Store(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT8_STORE, rs, MungEffectiveAddress(ea, 1), 1);
#else
	uint8_t value = gpr[rs];
	EmuStore<uint8_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16Store(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_STORE, rs, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value = Host2BigEndian((uint16_t) gpr[rs]);
	EmuStore<uint16_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32Store(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_STORE, rs, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value = Host2BigEndian(gpr[rs]);
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp32Store(unsigned int fs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP32_STORE, fs, MungEffectiveAddress(ea, 4), 4);
#else
	// check alignment
	if(unlikely(ea & 3))
	{
		if(!linux_os_import) throw AlignmentException<CONFIG>(ea);
	}
	Flags flags;
	flags.setRoundingMode(RN_ZERO);
	uint32_t value = Host2BigEndian(SoftFloat(fpr[fs], flags).queryValue());
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp64Store(unsigned int fs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP64_STORE, fs, MungEffectiveAddress(ea, 8), 8);
#else
	// check alignment
	if(unlikely(ea & 3))
	{
		if(!linux_os_import) throw AlignmentException<CONFIG>(ea);
	}
	uint64_t value = Host2BigEndian(fpr[fs].queryValue());
	EmuStore<uint64_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP_STORE_LSW, fs, MungEffectiveAddress(ea, 4), 4);
#else
	// check alignment
	if(unlikely(ea & 3))
	{
		if(!linux_os_import) throw AlignmentException<CONFIG>(ea);
	}
	uint32_t value = Host2BigEndian((uint32_t) fpr[fs].queryValue());
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE, rs, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value = Host2LittleEndian((uint16_t) gpr[rs]);
	EmuStore<uint16_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE, rs, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value = Host2LittleEndian(gpr[rs]);
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size)
{
	if(unlikely(GetMSR_LE()))
	{
		if(!linux_os_import) throw AlignmentException<CONFIG>(ea);
	}

#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT_STORE_MSB, rs, ea, size);
#else
	switch(size)
	{
		case 1:
			{
				uint8_t value = gpr[rs] >> 24;
				EmuStore<uint8_t>(value, ea);
				break;
			}

		case 2:
			{
				uint16_t value = Host2BigEndian((uint16_t)(gpr[rs] >> 16));
				EmuStore<uint16_t>(value, ea);
				break;
			}

		case 3:
			{
				uint32_t value = gpr[rs];
				uint8_t buffer[3];
				buffer[0] = value >> 24;
				buffer[1] = value >> 16;
				buffer[2] = value >> 8;
				EmuStore<typeof(buffer)>(buffer, ea);
				break;
			}

		case 4:
			{
				uint32_t value = Host2BigEndian(gpr[rs]);
				EmuStore<uint32_t>(value, ea);
				break;
			}
	}

	MonitorStore(ea, size);
#endif
}

/* Linked Load-Store */
template <class CONFIG>
void CPU<CONFIG>::Lwarx(unsigned int rd, typename CONFIG::address_t addr)
{
	Int32Load(rd, addr);

	reserve = true;
	reserve_addr = addr;
}

template <class CONFIG>
void CPU<CONFIG>::Stwcx(unsigned int rs, typename CONFIG::address_t addr)
{
	// TBD
	if(reserve)
	{
		if(reserve_addr == addr)
		{
			Int32Store(rs, addr);

			/* clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO] */
			SetCR((GetCR() & ~CONFIG::CR0_MASK) | CONFIG::CR0_EQ_MASK | ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0));
			reserve = false;
			return;
		}
	}

	/* clear CR0 and copy XER[SO] to CR0[SO] */
	SetCR((GetCR() & ~CONFIG::CR0_MASK) | ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0));
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
