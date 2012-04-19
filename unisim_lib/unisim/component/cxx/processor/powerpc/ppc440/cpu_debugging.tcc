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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_DEBUGGING_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_DEBUGGING_TCC__

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
bool CPU<CONFIG>::ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr)
{
	uint32_t read_offset = 0;

	do
	{
		typename CONFIG::STORAGE_ATTR storage_attr;
		typename CONFIG::physical_address_t physical_addr;
		typename CONFIG::address_t protection_boundary;
		if(translate_addr)
		{
			// Address translation
			MMUAccess<CONFIG> mmu_access;
		
			mmu_access.addr = addr;
			mmu_access.pid = GetProcessID();
			mmu_access.as = GetDataAddressSpace();
			mmu_access.privilege_level = GetPrivilegeLevel();
			mmu_access.memory_access_type = CONFIG::MAT_READ;
			mmu_access.memory_type = mt;
	
			try
			{
				EmuTranslateAddress<true>(mmu_access); // debug is enabled
			}
			catch(DSIException<CONFIG>& exc) { return false; }
			catch(ISIException<CONFIG>& exc) { return false; }
			catch(DataTLBErrorException<CONFIG>& exc) { return false; }
			catch(InstructionTLBErrorException<CONFIG>& exc) { return false; }

			storage_attr = mmu_access.storage_attr;
			physical_addr = mmu_access.physical_addr;
			protection_boundary = mmu_access.protection_boundary;
		}
		else
		{
			storage_attr = (typename CONFIG::STORAGE_ATTR)(CONFIG::SA_DEFAULT);
			physical_addr = addr;
			protection_boundary = addr + size;
		}

		uint32_t size_to_protection_boundary = protection_boundary - addr;

		do
		{
			uint32_t sz = 0;

			if(unlikely(IsVerboseReadMemory()))
			{
				logger << DebugInfo << "0x" << std::hex << addr << std::dec << ":";
			}

			switch(mt)
			{
				case CONFIG::MT_DATA:
					if(IsDataCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupDL1<true>(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy((uint8_t *) buffer + read_offset, &(*l1_access.block)[l1_access.offset], l1_sz);
							sz = l1_sz;
							if(unlikely(IsVerboseReadMemory()))
							{
								logger << "DL1: ";
							}
						}
					}
					break;
				case CONFIG::MT_INSN:
					if(IsInsnCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupIL1<true>(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy((uint8_t *) buffer + read_offset, &(*l1_access.block)[l1_access.offset], l1_sz);
							sz = l1_sz;
							if(unlikely(IsVerboseReadMemory()))
							{
								logger << "IL1: ";
							}
						}
					}
					break;
			}

			if(!sz)
			{
				sz = (size_to_protection_boundary > size) ? size : size_to_protection_boundary;
				if(!memory_import->ReadMemory(physical_addr, (uint8_t *) buffer + read_offset, sz)) return false;
				if(unlikely(IsVerboseReadMemory()))
				{
					logger << "MEM: ";
				}
			}

			if(unlikely(IsVerboseReadMemory()))
			{
				uint32_t i;
				for(i = 0; i < sz; i++)
				{
					logger << std::hex << (unsigned int)((((uint8_t *) buffer)[read_offset + i]) >> 4) << (unsigned int)((((uint8_t *) buffer)[read_offset + i]) & 15) << std::dec << " ";
				}
				logger << endl << EndDebugInfo;
			}

			size -= sz;
			read_offset += sz;
			addr += sz;
			physical_addr += sz;
			size_to_protection_boundary -= sz;
		} while(size > 0 && size_to_protection_boundary > 0);
	} while(size > 0);

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr)
{
	uint32_t write_offset = 0;

	do
	{
		typename CONFIG::STORAGE_ATTR storage_attr;
		typename CONFIG::physical_address_t physical_addr;
		typename CONFIG::address_t protection_boundary;
		if(translate_addr)
		{
			// Address translation
			MMUAccess<CONFIG> mmu_access;
		
			mmu_access.addr = addr;
			mmu_access.pid = GetProcessID();
			mmu_access.as = GetDataAddressSpace();
			mmu_access.privilege_level = CONFIG::PR_SUPERVISOR;
			mmu_access.memory_access_type = CONFIG::MAT_WRITE;
			mmu_access.memory_type = mt;
	
			try
			{
				EmuTranslateAddress<true>(mmu_access); // debug is enabled
			}
			catch(DSIException<CONFIG>& exc) { return false; }
			catch(ISIException<CONFIG>& exc) { return false; }
			catch(DataTLBErrorException<CONFIG>& exc) { return false; }
			catch(InstructionTLBErrorException<CONFIG>& exc) { return false; }
	
			storage_attr = mmu_access.storage_attr;
			physical_addr = mmu_access.physical_addr;
			protection_boundary = mmu_access.protection_boundary;
		}
		else
		{
			storage_attr = (typename CONFIG::STORAGE_ATTR)(CONFIG::SA_DEFAULT);
			physical_addr = addr;
			protection_boundary = addr + size;
		}

		uint32_t size_to_protection_boundary = protection_boundary - addr;

		do
		{
			uint32_t sz = 0;

			if(unlikely(IsVerboseWriteMemory()))
			{
				logger << DebugInfo << "0x" << std::hex << addr << std::dec << ":";
			}

			switch(mt)
			{
				case CONFIG::MT_DATA:
					if(IsDataCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupDL1<true>(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy(&(*l1_access.block)[l1_access.offset], (uint8_t *) buffer + write_offset, l1_sz);
							sz = l1_sz;
							if(unlikely(IsVerboseWriteMemory()))
							{
								logger << "DL1: ";
							}
						}
					}
					break;
				case CONFIG::MT_INSN:
					if(IsInsnCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupIL1<true>(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy(&l1_access.block[l1_access.offset], (uint8_t *) buffer + write_offset, l1_sz);
							sz = l1_sz;
							if(unlikely(IsVerboseWriteMemory()))
							{
								logger << "IL1: ";
							}
						}
					}
					break;
			}

			if(!sz)
			{
				sz = (size_to_protection_boundary > size) ? size : size_to_protection_boundary;
				if(!memory_import->WriteMemory(physical_addr, (uint8_t *) buffer + write_offset, sz)) return false;
				if(unlikely(IsVerboseWriteMemory()))
				{
					logger << "MEM: ";
				}
			}

			if(unlikely(IsVerboseWriteMemory()))
			{
				uint32_t i;
				for(i = 0; i < sz; i++)
				{
					logger << std::hex << (unsigned int)((((uint8_t *) buffer)[write_offset + i]) >> 4) << (unsigned int)((((uint8_t *) buffer)[write_offset + i]) & 15) << std::dec << " ";
				}
				logger << endl << EndDebugInfo;
			}

			size -= sz;
			write_offset += sz;
			addr += sz;
			physical_addr += sz;
			size_to_protection_boundary -= sz;
		} while(size > 0 && size_to_protection_boundary > 0);
	} while(size > 0);

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	return ReadMemory(addr, buffer, size, CONFIG::MT_DATA, true);
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	return WriteMemory(addr, buffer, size, CONFIG::MT_DATA, true);
}

template <class CONFIG>
string CPU<CONFIG>::Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t& next_addr)
{
	stringstream sstr;
	unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation;
	uint32_t insn;

	// Address translation
	MMUAccess<CONFIG> mmu_access;

	mmu_access.addr = addr;
	mmu_access.pid = GetProcessID();
	mmu_access.as = GetInstructionAddressSpace();
	mmu_access.privilege_level = GetPrivilegeLevel();
	mmu_access.memory_access_type = CONFIG::MAT_READ;
	mmu_access.memory_type = CONFIG::MT_INSN;

	try
	{
		EmuTranslateAddress<true>(mmu_access); // debug is enabled
	}
	catch(InstructionTLBErrorException<CONFIG>& exc) { return string("not mapped ?"); }

	bool hit = false;

	// Is IL1 enabled ?
	if(likely(!(mmu_access.storage_attr & CONFIG::SA_I) && IsInsnCacheEnabled())) // cache inhibited ?
	{
		// IL1 Access
		CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
	
		l1_access.addr = mmu_access.virtual_addr;
		LookupIL1<true>(l1_access);

		if(l1_access.block)
		{
			// L1 hit
			memcpy(&insn, &(*l1_access.block)[l1_access.offset], 4);
			hit = true;
		}
	}

	if(!hit)
	{
		if(!memory_import->ReadMemory(mmu_access.physical_addr, &insn, 4)) return string("unreadable ?");
	}

	if(mmu_access.storage_attr & CONFIG::SA_E) // little-endian ?
	{
#if BYTE_ORDER == BIG_ENDIAN
		BSwap(insn);
#endif
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(insn);
#endif
	}

	operation = unisim::component::cxx::processor::powerpc::ppc440::Decoder<CONFIG>::Decode(addr, insn);

	// disassemble the instruction
	sstr << "0x" << std::hex;
	sstr.fill('0'); sstr.width(8); 
	sstr << operation->GetEncoding() << std::dec << " ";
	operation->disasm(this, sstr);

	next_addr = addr + 4;
	return sstr.str();
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
