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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MMU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MMU_HH__

#include <unisim/service/interfaces/memory.hh>
#include <unisim/component/cxx/tlb/tlb.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/component/cxx/cache/cache_interface.hh>
#include <unisim/component/cxx/processor/powerpc/exception.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/cache_power_estimator.hh>
#include <unisim/service/interfaces/power_mode.hh>
#include <unisim/service/interfaces/logger.hh>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using std::string;
using std::ostream;
using unisim::component::cxx::tlb::TLB;
using unisim::component::cxx::cache::CacheInterface;
using unisim::component::cxx::cache::BusInterface;
using unisim::util::debug::SimpleRegister;
using unisim::component::cxx::cache::CacheControl;
using unisim::component::cxx::cache::CC_NONE;
using unisim::component::cxx::cache::CC_MEMORY_COHERENCY_ENFORCED;
using unisim::component::cxx::cache::BusControl;
using unisim::component::cxx::cache::BC_NONE;
using unisim::component::cxx::cache::BC_GLOBAL;
using unisim::component::cxx::cache::CacheStatus;
using unisim::component::cxx::tlb::ReplacementPolicy;
using namespace unisim::util::endian;
using unisim::service::interfaces::CachePowerEstimator;
using unisim::service::interfaces::PowerMode;
using unisim::service::interfaces::Logger;

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::service::interfaces::Memory;


typedef enum { MMC_NONE = 0, MMC_DMMU_ENABLE = 1, MMC_IMMU_ENABLE = 2, MMC_HIGH_BAT_ENABLE = 4, MMC_SOFTWARE_TABLE_SEARCH_ENABLE = 8, MMC_ADDRESS_BROADCAST_ENABLE = 16, MMC_DEBUG_ENABLE = 32, MMC_ICACHE_ENABLE = 64, MMC_DCACHE_ENABLE = 128 } MMUControl;

typedef enum { WIMG_GUARDED_MEMORY = 1, WIMG_MEMORY_COHERENCY_ENFORCED = 2, WIMG_CACHE_INHIBITED = 4, WIMG_WRITE_THROUGH = 8 } WIMG;

template <class VIRTUAL_ADDRESS, class PHYSICAL_ADDRESS>
class MemoryPageTableEntry;

template <class VIRTUAL_ADDRESS, class PHYSICAL_ADDRESS>
ostream& operator << (ostream& os, const MemoryPageTableEntry<VIRTUAL_ADDRESS, PHYSICAL_ADDRESS>& pte);

/* A Memory Page Table Entry */
template <class VIRTUAL_ADDRESS, class PHYSICAL_ADDRESS>
class MemoryPageTableEntry
{
public:
	bool valid;                             /*< true if entry is valid, false otherwise */
	MemoryPageTableEntry<VIRTUAL_ADDRESS, PHYSICAL_ADDRESS> *prev;  /*< previous entry (unused if TLB associativity is 1) */
	MemoryPageTableEntry<VIRTUAL_ADDRESS, PHYSICAL_ADDRESS> *next;  /*< next entry (unused if TLB associativity is 1) */
	VIRTUAL_ADDRESS base_virtual_addr;      /*< 52-bit base virtual address */
	PHYSICAL_ADDRESS base_physical_addr;    /*< 32-bit base physical address */
	uint32_t c;                             /*< 1-bit C field */
	WIMG wimg;                              /*< 4-bit WIMG field */
	uint32_t pp;                            /*< 2-bit page protection field */
	uint32_t way;                           /*< TLB way */
	uint32_t lru_bits;                      /*< computed only when saving the TLBs, and restored when restoring the TLBs, */
	                                        /*< these bits are not updated during simulation. Instead next and prev pointers */
	                                        /*< are used to sort PTE according LRU replacement policy */
	PHYSICAL_ADDRESS pte_addr;
	friend ostream& operator << <VIRTUAL_ADDRESS, PHYSICAL_ADDRESS>(ostream& os, const MemoryPageTableEntry<VIRTUAL_ADDRESS, PHYSICAL_ADDRESS>& entry);
};

template <class CONFIG>
class MMU :
	public unisim::service::interfaces::Registers,
	public Client<Memory<typename CONFIG::address_t> >,
	public Client<Logger>
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::virtual_address_t virtual_address_t;
	typedef typename CONFIG::physical_address_t physical_address_t;
	
	ServiceImport<Memory<physical_address_t> > to_mem;
	ServiceImport<Memory<physical_address_t> > to_dcache;
	ServiceImport<Memory<physical_address_t> > to_icache;
	ServiceImport<CachePowerEstimator> itlb_power_estimator_import;
	ServiceImport<CachePowerEstimator> dtlb_power_estimator_import;
	ServiceImport<PowerMode> itlb_power_mode_import;
	ServiceImport<PowerMode> dtlb_power_mode_import;
	ServiceExport<PowerMode> itlb_power_mode_export;
	ServiceExport<PowerMode> dtlb_power_mode_export;
	ServiceImport<Logger> logger_import;

// MMU parameters
	static const uint32_t MEMORY_PAGE_SIZE = 4096; // DO NOT MODIFY THIS VALUE !!!!!
	static const uint32_t FSB_WIDTH = 8; // 64-bit front side bus

	MMU(const char *name,
	    CacheInterface<physical_address_t> *icache_interface,
	    CacheInterface<physical_address_t> *dcache_interface,
	    BusInterface<physical_address_t> *bus_interface,
		Object *parent = 0);
	virtual ~MMU();

	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);

	void Reset();
	void SetControl(MMUControl mmc);
	MMUControl GetControl();

	void EnableDebug();
	void DisableDebug();

	virtual unisim::util::debug::Register *GetRegister(const char *name);

	uint32_t ReadInsnMemory(address_t ea, PrivilegeLevel privilege_level, physical_address_t& physical_addr);
	uint32_t ReadInsnMemory(address_t ea, PrivilegeLevel privilege_level, void *buffer, uint32_t size, physical_address_t& physical_addr);
	void ReadDataMemory(address_t ea, void *buffer, uint32_t size, PrivilegeLevel privilege_level);
	void WriteDataMemory(address_t ea, const void *buffer, uint32_t size, PrivilegeLevel privilege_level);
	physical_address_t TranslateAddress(address_t addr, PrivilegeLevel privilege_level, MemoryAccessType memory_access_type, MemoryType memory_type, address_t& protection_boundary, WIMG& wimg);

	void InvalidateDTLB();
	void InvalidateITLB();
	void InvalidateDTLBEntry(address_t addr);
	void InvalidateITLBEntry(address_t addr);
	void InvalidateDTLBSet(address_t addr);
	void InvalidateITLBSet(address_t addr);
	void LoadITLBEntry(address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo);
	void LoadDTLBEntry(address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo);
	void EnableIMMU();
	void DisableIMMU();
	void EnableDMMU();
	void DisableDMMU();
	void EnableAddressOnlyBroadcast();
	void DisableAddressOnlyBroadcast();
	void EnableSoftwareTableSearch();
	void DisableSoftwareTableSearch();
	void EnableHighBAT();
	void DisableHighBAT();
	void EnableICache();
	void DisableICache();
	void EnableDCache();
	void DisableDCache();

	void CheckTLBConsistency(MemoryType mt);
	
	void ZeroDataCacheBlock(address_t addr, PrivilegeLevel privilege_level);
	void FlushDataCacheBlock(address_t addr, PrivilegeLevel privilege_level);
	void InvalidateDataCacheBlock(address_t addr, PrivilegeLevel privilege_level);
	void InvalidateInsnCacheBlock(address_t addr, PrivilegeLevel privilege_level);

	void DumpPageTable(ostream& os);

	inline uint32_t GetDBATL(unsigned int n) { return dbatl[n]; }
	inline void SetDBATL(unsigned int n, uint32_t value) { dbatl[n] = value; }
	
	inline uint32_t GetDBATU(unsigned int n) { return dbatu[n]; }
	inline void SetDBATU(unsigned int n, uint32_t value) { dbatu[n] = value; }
	
	inline uint32_t GetIBATL(unsigned int n) { return ibatl[n]; }
	inline void SetIBATL(unsigned int n, uint32_t value) { ibatl[n] = value; }
	
	inline uint32_t GetIBATU(unsigned int n) { return ibatu[n]; }
	inline void SetIBATU(unsigned int n, uint32_t value) { ibatu[n] = value; }
	
	// Methods for reading DBAT register bits
	inline uint32_t GetDBATU_BEPI(unsigned int n) { return (GetDBATU(n) >> 17) & 0x7fff; }
	inline uint32_t GetDBATU_BL(unsigned int n) { return (GetDBATU(n) >> 2) & 0x7ff; }
	inline uint32_t GetDBATU_VS(unsigned int n) { return (GetDBATU(n) >> 1) & 1; }
	inline uint32_t GetDBATU_VP(unsigned int n) { return GetDBATU(n) & 1; }
	inline uint32_t GetDBATL_BRPN(unsigned int n) { return (GetDBATL(n) >> 17) & 0x7fff; }
	inline uint32_t GetDBATL_WIMG(unsigned int n) { return (GetDBATL(n) >> 3) & 0xf; }
	inline uint32_t GetDBATL_PP(unsigned int n) { return GetDBATL(n) & 3; }

	// Methods for reading IBAT register bits
	inline uint32_t GetIBATU_BEPI(unsigned int n) { return (GetIBATU(n) >> 17) & 0x7fff; }
	inline uint32_t GetIBATU_BL(unsigned int n) { return (GetIBATU(n) >> 2) & 0x7ff; }
	inline uint32_t GetIBATU_VS(unsigned int n) { return (GetIBATU(n) >> 1) & 1; }
	inline uint32_t GetIBATU_VP(unsigned int n) { return GetIBATU(n) & 1; }
	inline uint32_t GetIBATL_BRPN(unsigned int n) { return (GetIBATL(n) >> 17) & 0x7fff; }
	inline uint32_t GetIBATL_WIMG(unsigned int n) { return (GetIBATL(n) >> 3) & 0xf; }
	inline uint32_t GetIBATL_PP(unsigned int n) { return GetIBATL(n) & 3; }

	inline uint32_t GetSR(unsigned int n) { return sr[n]; }
	inline void SetSR(unsigned int n, uint32_t value) { sr[n] = value; }

	// Methods for reading SR registers bits
	inline uint32_t GetSR_T(unsigned int n) { return (sr[n] >> 31) & 1; }
	inline uint32_t GetSR_KS(unsigned int n) { return (sr[n] >> 30) & 1; }
	inline uint32_t GetSR_KP(unsigned int n) { return (sr[n] >> 29) & 1; }
	inline uint32_t GetSR_N(unsigned int n) { return (sr[n] >> 28) & 1; }
	inline uint32_t GetSR_VSID(unsigned int n) { return sr[n] & 0xffffffUL; }
	
	inline uint32_t GetSDR1() { return sdr1; }
	inline void SetSDR1(uint32_t value) { sdr1 = value; }

	// Methods for reading SDR1 bits
	inline uint32_t GetSDR1_HTABORG() { return (sdr1 >> 16) & 0xffff; }
	inline uint32_t GetSDR1_HTABMASK() { return sdr1 & 0x1ff; }

	// additional bits in SDR1 for 36-bits addressing mode
//	inline uint32_t GetSDR1_HTABEXT() { return (sdr1 >> 13) & 0x7; }
//	inline uint32_t GetSDR1_HTMEXT() { return (sdr1 >> 9) & 0xf; }

private:

	/* Memory management registers */
	uint32_t dbatu[CONFIG::NUM_BATS];	/*< data block address translation registers (upper 32 bits) */
	uint32_t dbatl[CONFIG::NUM_BATS];	/*< data block address translation registers (lower 32 bits) */
	uint32_t ibatu[CONFIG::NUM_BATS];	/*< instruction block address translation registers (upper 32 bits) */
	uint32_t ibatl[CONFIG::NUM_BATS];	/*< instruction block address translation registers (lower 32 bits) */
	uint32_t sr[16];	/*< segment registers */
	uint32_t sdr1;		/*< SDR1 register */

	/* Data Translation look-aside buffers */
	unisim::component::cxx::tlb::TLB<
		MemoryPageTableEntry<typename CONFIG::virtual_address_t, typename CONFIG::physical_address_t>,
		virtual_address_t,
		CONFIG::NUM_DTLB_ENTRIES,
		MEMORY_PAGE_SIZE,
		CONFIG::DTLB_ASSOCIATIVITY,
		CONFIG::DTLB_REPLACEMENT_POLICY> dtlb;

	/* Instruction Translation look-aside buffers */
	unisim::component::cxx::tlb::TLB<
		MemoryPageTableEntry<typename CONFIG::virtual_address_t, typename CONFIG::physical_address_t>,
		virtual_address_t,
		CONFIG::NUM_ITLB_ENTRIES,
		MEMORY_PAGE_SIZE,
		CONFIG::ITLB_ASSOCIATIVITY,
		CONFIG::ITLB_REPLACEMENT_POLICY> itlb;

	CacheInterface<physical_address_t> *dcache_interface;
	CacheInterface<physical_address_t> *icache_interface;
	BusInterface<physical_address_t> *bus_interface;
	map<string, unisim::util::debug::Register *> registers_registry;

	MMUControl mmc;
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

