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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_LOAD_STORE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_LOAD_STORE_TCC__

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

template <class CONFIG>
bool CPU<CONFIG>::EmuLoad(MMUAccess<CONFIG>& mmu_access, void *buffer, uint32_t size)
{
	if(CONFIG::DEBUG_ENABLE)
	{
		memset(buffer, 0, size);
	}
	
	if(likely(!(mmu_access.storage_attr & CONFIG::SA_I) && IsDataCacheEnabled())) // cache inhibited ?
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = mmu_access.physical_addr;
		l1_access.storage_attr = mmu_access.storage_attr;
		LookupDL1<false>(l1_access);

		if(unlikely(!l1_access.line))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: index=" << l1_access.index << ", choosen way=" << l1_access.way << endl << EndDebugInfo;
			}
			
			if(unlikely(!EmuEvictDL1(l1_access))) return false;
		}

		if(unlikely(!l1_access.block))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			if(unlikely(!EmuFillDL1(l1_access))) return false;
		}

		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);

		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 disabled
		if(unlikely(!PLBDataRead(mmu_access.physical_addr, buffer, size, mmu_access.storage_attr)))
		{
			SetException(CONFIG::EXC_MACHINE_CHECK_DATA_ASYNCHRONOUS);
			return false;
		}
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
		logger << "(" << size << " bytes) at 0x" << std::hex << mmu_access.addr << std::dec << endl << EndDebugInfo;
	}
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::EmuStore(MMUAccess<CONFIG>& mmu_access, const void *buffer, uint32_t size)
{
	if(unlikely(CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_PRINTK_ENABLE && enable_linux_printk_snooping))
	{
		if(unlikely(enable_linux_printk_snooping))
		{
			if(linux_printk_buf_addr && (mmu_access.addr >= linux_printk_buf_addr) && (mmu_access.addr < (linux_printk_buf_addr + linux_printk_buf_size)))
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
		logger << "(" << size << " bytes) at 0x" << std::hex << mmu_access.addr << std::dec << endl << EndDebugInfo;
	}
	
	if(likely(!(mmu_access.storage_attr & CONFIG::SA_I) && IsDataCacheEnabled())) // cache inhibited ?
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = mmu_access.physical_addr;
		l1_access.storage_attr = mmu_access.storage_attr;
		LookupDL1<false>(l1_access);

		if(unlikely(!l1_access.line))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: index=" << l1_access.index << ", choosen way=" << l1_access.way << endl << EndDebugInfo;
			}

			if(unlikely(!EmuEvictDL1(l1_access))) return false;
		}
		
		if(unlikely(!l1_access.block))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			if(unlikely(!EmuFillDL1(l1_access))) return false;
		}
	
		// DL1 hit
		memcpy(&(*l1_access.block)[l1_access.offset], buffer, size);

		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportWriteAccess();

		if(unlikely(mmu_access.storage_attr & CONFIG::SA_W)) // write-through ?
		{
			// Note: dirty bit is not set when storage attribute W is set
			if(unlikely(!PLBDataWrite(mmu_access.physical_addr, buffer, size, mmu_access.storage_attr)))
			{
				SetException(CONFIG::EXC_MACHINE_CHECK_DATA_ASYNCHRONOUS);
				return false;
			}
		}
		else
		{
			l1_access.block->status.dirty |= (1 << (l1_access.offset / 8)); // one 64-bit word is dirty
		}
	}
	else
	{
		// DL1 disabled
		if(unlikely(!PLBDataWrite(mmu_access.physical_addr, buffer, size, mmu_access.storage_attr)))
		{
			SetException(CONFIG::EXC_MACHINE_CHECK_DATA_ASYNCHRONOUS);
			return false;
		}
	}
	
	return true;
}

template <class CONFIG>
template <typename T, bool REVERSE, bool FORCE_BIG_ENDIAN>
inline bool CPU<CONFIG>::EmuLoad(T& value, typename CONFIG::address_t ea)
{
	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (ea & (CONFIG::FSB_WIDTH - 1));

	MMUAccess<CONFIG> mmu_access;
	
	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory load does not cross a FSB boundary
		// Address translation
		mmu_access.addr = ea;
		mmu_access.pid = GetProcessID();
		mmu_access.as = GetDataAddressSpace();
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ;
		mmu_access.memory_type = CONFIG::MT_DATA;

		if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;
		
		if(unlikely(!EmuLoad(mmu_access, &value, sizeof(T)))) return false;
		
	}
	else
	{
		// Memory load crosses a FSB boundary
		// Address translation
		
		mmu_access.addr = ea;
		mmu_access.pid = GetProcessID();
		mmu_access.as = GetDataAddressSpace();
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ;
		mmu_access.memory_type = CONFIG::MT_DATA;

		if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;

		if(unlikely(!EmuLoad(mmu_access, &value, size_to_fsb_boundary))) return false;

		mmu_access.addr = ea + size_to_fsb_boundary;
		if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;
		if(unlikely(!EmuLoad(mmu_access, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary))) return false;
	}

#if BYTE_ORDER == LITTLE_ENDIAN
	if((REVERSE && !FORCE_BIG_ENDIAN && (mmu_access.storage_attr & CONFIG::SA_E)) || (!REVERSE && (FORCE_BIG_ENDIAN || !(mmu_access.storage_attr & CONFIG::SA_E))))
#else
	if((!REVERSE && !FORCE_BIG_ENDIAN && (mmu_access.storage_attr & CONFIG::SA_E)) || (REVERSE && (FORCE_BIG_ENDIAN || !(mmu_access.storage_attr & CONFIG::SA_E))))
#endif
	{
		BSwap(value);
	}
	
	return true;
}

template <class CONFIG>
template <typename T, bool REVERSE, bool FORCE_BIG_ENDIAN>
inline bool CPU<CONFIG>::EmuStore(T value, typename CONFIG::address_t ea)
{
	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (ea & (CONFIG::FSB_WIDTH - 1));

	MMUAccess<CONFIG> mmu_access;

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory store does not cross a FSB boundary
		// Address translation
		
		mmu_access.addr = ea;
		mmu_access.pid = GetProcessID();
		mmu_access.as = GetDataAddressSpace();
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE;
		mmu_access.memory_type = CONFIG::MT_DATA;

		if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;
		
#if BYTE_ORDER == LITTLE_ENDIAN
		if((REVERSE && !FORCE_BIG_ENDIAN && (mmu_access.storage_attr & CONFIG::SA_E)) || (!REVERSE && (FORCE_BIG_ENDIAN || !(mmu_access.storage_attr & CONFIG::SA_E))))
#else
		if((!REVERSE && !FORCE_BIG_ENDIAN && (mmu_access.storage_attr & CONFIG::SA_E)) || (REVERSE && (FORCE_BIG_ENDIAN || !(mmu_access.storage_attr & CONFIG::SA_E))))
#endif
		{
			BSwap(value);
		}
		if(unlikely(!EmuStore(mmu_access, &value, sizeof(T)))) return false;
	}
	else
	{
		// Memory store crosses a FSB boundary
		// Address translation for the first memory access
		mmu_access.addr = ea;
		mmu_access.pid = GetProcessID();
		mmu_access.as = GetDataAddressSpace();
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE;
		mmu_access.memory_type = CONFIG::MT_DATA;

		if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;
		
#if BYTE_ORDER == LITTLE_ENDIAN
		if((REVERSE && !FORCE_BIG_ENDIAN && (mmu_access.storage_attr & CONFIG::SA_E)) || (!REVERSE && (FORCE_BIG_ENDIAN || !(mmu_access.storage_attr & CONFIG::SA_E))))
#else
		if((!REVERSE && !FORCE_BIG_ENDIAN && (mmu_access.storage_attr & CONFIG::SA_E)) || (REVERSE && (FORCE_BIG_ENDIAN || !(mmu_access.storage_attr & CONFIG::SA_E))))
#endif
		{
			BSwap(value);
		}

		if(unlikely(!EmuStore(mmu_access, &value, size_to_fsb_boundary))) return false;

		// Address translation for the first memory access
		mmu_access.addr = ea + size_to_fsb_boundary;
		if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;

		if(unlikely(!EmuStore(mmu_access, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary))) return false;
	}
	
	return true;
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorLoad(typename CONFIG::address_t ea, uint32_t size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, ea, size);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorStore(typename CONFIG::address_t ea, uint32_t size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, ea, size);
	}
}

template <class CONFIG>
bool CPU<CONFIG>::Int8Load(unsigned int rd, typename CONFIG::address_t ea)
{
	uint8_t value;
	bool status = EmuLoad<uint8_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 8-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int16Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 1) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint16_t value;
	bool status = EmuLoad<uint16_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::SInt16Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 1) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint16_t value;
	bool status = EmuLoad<uint16_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) (int16_t) value; // 16-bit to 32-bit sign extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int32Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 3) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint32_t value;
	bool status = EmuLoad<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = value;
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Fp32Load(unsigned int fd, typename CONFIG::address_t ea)
{
	// check alignment
	if(unlikely((ea & 3) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint32_t value;
	bool status = EmuLoad<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	Flags flags;
	flags.setRoundingMode((fpscr & CONFIG::FPSCR_RN_MASK) >> CONFIG::FPSCR_RN_OFFSET);
	fpr[fd].assign(SoftFloat(value), flags);
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Fp64Load(unsigned int fd, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 7) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint64_t value;
	bool status = EmuLoad<uint64_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	fpr[fd] = SoftDouble(value);
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 1) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint16_t value;
	bool status = EmuLoad<uint16_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 3) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint32_t value;
	bool status = EmuLoad<uint32_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	gpr[rd] = value;
	MonitorLoad(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size)
{
	switch(size)
	{
		case 1:
		{
			uint8_t value;
			bool status = EmuLoad<uint8_t, false, true>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = (uint32_t) value << 24;
			break;
		}

		case 2:
		{
			uint16_t value;
			bool status = EmuLoad<uint16_t, false, true>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = (uint32_t) BigEndian2Host(value) << 16;
			break;
		}

		case 3:
		{
			typedef uint8_t array_uint8_3_t[3];
			uint8_t buffer[3];
			bool status = EmuLoad<array_uint8_3_t, false, true>(buffer, ea);
			if(unlikely(!status)) return false;
			uint32_t value = ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8);
			gpr[rd] = value;
			break;
		}

		case 4:
		{
			uint32_t value;
			bool status = EmuLoad<uint32_t, false, true>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = BigEndian2Host(value);
			break;
		}
		
		default:
			return false;
	}
	MonitorLoad(ea, size);
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int8Store(unsigned int rs, typename CONFIG::address_t ea)
{
	uint8_t value = gpr[rs];
	bool status = EmuStore<uint8_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int16Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 1) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint16_t value = (uint16_t) gpr[rs];
	bool status = EmuStore<uint16_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int32Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 3) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint32_t value = gpr[rs];
	bool status = EmuStore<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Fp32Store(unsigned int fs, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 3) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	Flags flags;
	flags.setRoundingMode(RN_ZERO);
	uint32_t value = SoftFloat(fpr[fs], flags).queryValue();
	bool status = EmuStore<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Fp64Store(unsigned int fs, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 7) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint64_t value = fpr[fs].queryValue();
	bool status = EmuStore<uint64_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 3) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint32_t value = (uint32_t) fpr[fs].queryValue();
	bool status = EmuStore<uint32_t, false, false>(value, ea);
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
	if(unlikely((ea & 1) && GetCCR0_FLSTA() && !linux_os_import))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint16_t value = (uint16_t) gpr[rs];
	bool status = EmuStore<uint16_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
	if(unlikely(GetCCR0_FLSTA() && (ea & 3)))
	{
		SetException(CONFIG::EXC_ALIGNMENT, ea);
		return false;
	}
	uint32_t value = gpr[rs];
	bool status = EmuStore<uint32_t, true, false>(value, ea); // reversed
	if(unlikely(!status)) return false;
	MonitorStore(ea, sizeof(value));
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size)
{
	switch(size)
	{
		case 1:
			{
				uint8_t value = gpr[rs] >> 24;
				bool status = EmuStore<uint8_t, false, true>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 2:
			{
				uint16_t value = Host2BigEndian((uint16_t)(gpr[rs] >> 16));
				bool status = EmuStore<uint16_t, false, true>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 3:
			{
				typedef uint8_t array_uint8_3_t[3];
				uint32_t value = gpr[rs];
				uint8_t buffer[3];
				buffer[0] = value >> 24;
				buffer[1] = value >> 16;
				buffer[2] = value >> 8;
				bool status = EmuStore<array_uint8_3_t, false, true>(buffer, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 4:
			{
				uint32_t value = Host2BigEndian(gpr[rs]);
				bool status = EmuStore<uint32_t, false, true>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}
			
		default:
			return false;
	}

	MonitorStore(ea, size);
	return true;
}

/* Linked Load-Store */
template <class CONFIG>
bool CPU<CONFIG>::Lwarx(unsigned int rd, typename CONFIG::address_t addr)
{
	if(unlikely(!Int32Load(rd, addr))) return false;

	reserve = true;
	reserve_addr = addr;
	
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Stwcx(unsigned int rs, typename CONFIG::address_t addr)
{
	// TBD
	if(reserve)
	{
		if(reserve_addr == addr)
		{
			if(unlikely(!Int32Store(rs, addr))) return false;

			/* clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO] */
			SetCR((GetCR() & ~CONFIG::CR0_MASK) | CONFIG::CR0_EQ_MASK | ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0));
			reserve = false;
			return true;
		}
	}

	/* clear CR0 and copy XER[SO] to CR0[SO] */
	SetCR((GetCR() & ~CONFIG::CR0_MASK) | ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0));
	
	return true;
}

/* Memory injection */
template <class CONFIG>
bool CPU<CONFIG>::InjectReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	if(size > 0)
	{
		uint32_t sz;
		uint8_t *dst = (uint8_t *) buffer;
		do
		{
			uint32_t size_to_word_boundary = 8 - (addr & 7);
			sz = size > size_to_word_boundary ? size_to_word_boundary : size;

			MMUAccess<CONFIG> mmu_access;
			mmu_access.addr = addr;
			mmu_access.pid = GetProcessID();
			mmu_access.as = GetDataAddressSpace();
			mmu_access.privilege_level = GetPrivilegeLevel();
			mmu_access.memory_access_type = CONFIG::MAT_READ;
			mmu_access.memory_type = CONFIG::MT_DATA;

			if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;

			if(unlikely(!EmuLoad(mmu_access, dst, sz))) return false;
			
			MonitorLoad(addr, sz);
			dst += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::InjectWriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		uint32_t sz;
		const uint8_t *src = (const uint8_t *) buffer;
		do
		{
			uint32_t size_to_word_boundary = 8 - (addr & 7);
			sz = size > size_to_word_boundary ? size_to_word_boundary : size;
			
			MMUAccess<CONFIG> mmu_access;
			mmu_access.addr = addr;
			mmu_access.pid = GetProcessID();
			mmu_access.as = GetDataAddressSpace();
			mmu_access.privilege_level = GetPrivilegeLevel();
			mmu_access.memory_access_type = CONFIG::MAT_WRITE;
			mmu_access.memory_type = CONFIG::MT_DATA;

			if(unlikely(!EmuTranslateAddress<false>(mmu_access))) return false;
			
			if(unlikely(!EmuStore(mmu_access, src, sz))) return false;
			
			MonitorStore(addr, sz);
			src += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::PLBInsnRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::PLBDataRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::PLBDataWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	return memory_import->WriteMemory(physical_addr, buffer, size);
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
