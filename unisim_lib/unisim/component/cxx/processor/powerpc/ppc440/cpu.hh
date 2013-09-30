/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_HH__

#include <unisim/kernel/logger/logger.hh>
#include <unisim/component/cxx/processor/powerpc/floating.hh>
#include <unisim/component/cxx/processor/powerpc/ppc440/isa.hh>
#include <unisim/component/cxx/processor/powerpc/ppc440/config.hh>
#include <unisim/component/cxx/cache/cache.hh>
#include <unisim/component/cxx/tlb/tlb.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/component/cxx/processor/powerpc/ppc440/exception.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/cache_power_estimator.hh>
#include <unisim/service/interfaces/power_mode.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/queue/queue.hh>
#include <map>
#include <iosfwd>

#ifdef powerpc
#undef powerpc
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ >= 4) && ((__GNUC_MINOR__ > 1) || ((__GNUC_MINOR__ >= 1) && (__GNUC_PATCHLEVEL__ >= 3)))))     // GNU C version >= 4.1.3
#if defined(ALWAYS_INLINE)
#undef ALWAYS_INLINE
#endif
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::CachePowerEstimator;
using unisim::service::interfaces::PowerMode;
using unisim::service::interfaces::Registers;
using namespace unisim::util::endian;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Object;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::LinuxOS;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Synchronizable;
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::ParameterArray;
using unisim::kernel::service::Formula;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using namespace std;
using unisim::util::queue::Queue;
using unisim::component::cxx::cache::CacheBlock;
using unisim::component::cxx::cache::CacheLine;
using unisim::component::cxx::cache::CacheSet;
using unisim::component::cxx::cache::Cache;
using unisim::component::cxx::tlb::TLBEntry;
using unisim::component::cxx::tlb::TLBSet;
using unisim::component::cxx::tlb::TLB;

template <class CONFIG>
class CPU;

template <class CONFIG>
class CacheAccess;

template <class CONFIG>
ostream& operator << (ostream& os, const CacheAccess<CONFIG>& cache_access);

template <class CONFIG>
class CacheAccess
{
public:
	typename CONFIG::ADDRESS addr;
	typename CONFIG::STORAGE_ATTR storage_attr;
	typename CONFIG::ADDRESS line_base_addr;
	typename CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t way;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;
	CacheSet<CONFIG> *set;
	CacheLine<CONFIG> *line;
	CacheLine<CONFIG> *line_to_evict;
	CacheBlock<CONFIG> *block;

	friend ostream& operator << <CONFIG> (ostream& os, const CacheAccess<CONFIG>& cache_access);
};

template <class CONFIG>
class MMUAccess;

template <class CONFIG>
ostream& operator << (ostream& os, const MMUAccess<CONFIG>& mmu_access);

template <class CONFIG>
class MMUAccess
{
public:
	// Input
	typename CONFIG::address_t addr;                      // effective address
	typename CONFIG::process_id_t pid;                    // process ID
	typename CONFIG::address_space_t as;                  // address space
	typename CONFIG::PrivilegeLevel privilege_level;      // privilege level
	typename CONFIG::MemoryAccessType memory_access_type; // memory access type
	typename CONFIG::MemoryType memory_type;              // memory type

	// EmuTranslateAddr intermediate computations
	typename CONFIG::virtual_address_t virtual_addr;  // 41-bit virtual address

	// LookupITLB/LookupDTLB intermediate computations
	TLBSet<class CONFIG::ITLB_CONFIG> *itlb_set;      // An ITLB Set
	TLBEntry<class CONFIG::ITLB_CONFIG> *itlb_entry;  // An ITLB entry
	TLBSet<class CONFIG::DTLB_CONFIG> *dtlb_set;      // A DTLB Set
	TLBEntry<class CONFIG::DTLB_CONFIG> *dtlb_entry;  // A DTLB entry
	uint32_t tlb_index;                               // An index ITLB/DTLB
	uint32_t tlb_way;                                 // A way in ITLB/DTLB
	bool tlb_hit;                                     // Whether access hits in ITLB/DTLB

	// LookupUTLB intermediate computations
	TLBSet<class CONFIG::UTLB_CONFIG> *utlb_set;      // A UTLB Set
	TLBEntry<class CONFIG::UTLB_CONFIG> *utlb_entry;  // A UTLB entry
	uint32_t utlb_index;                              // An index in UTLB
	uint32_t utlb_way;                                // A way in UTLB
	bool utlb_hit;                                    // Whether access hits in UTLB

	// LookupITLB/LookupDTLB/LookupDTLB intermediate computations
	uint32_t page_size;                               // Page size in bytes

	// Output
	typename CONFIG::TLB_ENTRY *pte;
	typename CONFIG::physical_address_t physical_addr;
	typename CONFIG::address_t protection_boundary;
	typename CONFIG::STORAGE_ATTR storage_attr;

	friend ostream& operator << <CONFIG> (ostream& os, const MMUAccess<CONFIG>& mmu_access);
};

class TimeBaseRegisterInterface : public unisim::util::debug::Register
{
public:
	typedef enum
	{
		TB_LOW,
		TB_HIGH
	} Type;
	TimeBaseRegisterInterface(const char *name, uint64_t *value, Type type);
	virtual ~TimeBaseRegisterInterface();
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	std::string name;
	uint64_t *value;
	Type type;
};

class TimeBaseRegisterView : public unisim::kernel::service::VariableBase
{
public:
	typedef enum
	{
		TB_LOW,
		TB_HIGH
	} Type;
	TimeBaseRegisterView(const char *name, unisim::kernel::service::Object *owner, uint64_t& storage, Type type, const char *description);
	virtual ~TimeBaseRegisterView();
	virtual const char *GetDataTypeName() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator std::string () const;
	virtual unisim::kernel::service::VariableBase& operator = (bool value);
	virtual unisim::kernel::service::VariableBase& operator = (long long value);
	virtual unisim::kernel::service::VariableBase& operator = (unsigned long long value);
	virtual unisim::kernel::service::VariableBase& operator = (double value);
	virtual unisim::kernel::service::VariableBase& operator = (const char * value);
private:
	uint64_t& storage;
	Type type;
};

template <class CONFIG>
class CPU :
	public unisim::component::cxx::processor::powerpc::ppc440::Decoder<CONFIG>,
	public Client<Loader>,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
	public Client<DebugControl<typename CONFIG::address_t> >,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Client<TrapReporting>,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<unisim::service::interfaces::Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::physical_address_t> >,
	public Client<LinuxOS>,
	public Client<CachePowerEstimator>,
	public Client<PowerMode>,
	public Service<Synchronizable>
{
public:
	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<typename CONFIG::address_t> > disasm_export;
	ServiceExport<unisim::service::interfaces::Registers> registers_export;
	ServiceExport<Memory<typename CONFIG::address_t> > memory_export;
	ServiceExport<MemoryInjection<typename CONFIG::address_t> > memory_injection_export;
	ServiceExport<Synchronizable> synchronizable_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<Loader> loader_import;
	ServiceImport<DebugControl<typename CONFIG::address_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<typename CONFIG::address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<typename CONFIG::address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<typename CONFIG::physical_address_t> > memory_import;
	ServiceImport<LinuxOS> linux_os_import;
	ServiceImport<TrapReporting> trap_reporting_import;
	ServiceImport<CachePowerEstimator> il1_power_estimator_import;
	ServiceImport<CachePowerEstimator> dl1_power_estimator_import;
	ServiceImport<CachePowerEstimator> itlb_power_estimator_import;
	ServiceImport<CachePowerEstimator> dtlb_power_estimator_import;
	ServiceImport<CachePowerEstimator> utlb_power_estimator_import;
	ServiceImport<PowerMode> il1_power_mode_import;
	ServiceImport<PowerMode> dl1_power_mode_import;
	ServiceImport<PowerMode> itlb_power_mode_import;
	ServiceImport<PowerMode> dtlb_power_mode_import;
	ServiceImport<PowerMode> utlb_power_mode_import;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();
	
	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool BeginSetup();
	virtual bool EndSetup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================
	
	void StepOneInstruction();
	virtual void Synchronize();
	virtual void Reset();
	virtual void Idle();

	//=====================================================================
	//=                     CPU control handling methods                  =
	//=====================================================================

	void SetSupervisorPrivilegeLevel();
	void SetUserPrivilegeLevel();
	void EnableFPU();
	void DisableFPU();
	void EnableFPUException();
	void DisableFPUException();
	inline bool IsFPUEnabled();
	inline bool IsFPUExceptionEnabled();
	inline bool IsDataCacheEnabled();
	inline bool IsInsnCacheEnabled();

	//=====================================================================
	//=                State interface (with .isa files)                  =
	//=====================================================================

	const SoftDouble& GetFPR(unsigned int n) const { return fpr[n]; }
	void SetFPR(unsigned int n, const SoftDouble& value) { fpr[n] = value; }

	bool Int8Load(unsigned int rd, typename CONFIG::address_t ea);
	bool Int16Load(unsigned int rd, typename CONFIG::address_t ea);
	bool SInt16Load(unsigned int rd, typename CONFIG::address_t ea);
	bool Int32Load(unsigned int rd, typename CONFIG::address_t ea);
	bool Fp32Load(unsigned int fd, typename CONFIG::address_t ea);
	bool Fp64Load(unsigned int fd, typename CONFIG::address_t ea);
	bool Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea);
	bool Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea);
	bool IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size);
	bool Int8Store(unsigned int rs, typename CONFIG::address_t ea);
	bool Int16Store(unsigned int rs, typename CONFIG::address_t ea);
	bool Int32Store(unsigned int rs, typename CONFIG::address_t ea);
	bool Fp32Store(unsigned int fs, typename CONFIG::address_t ea);
	bool Fp64Store(unsigned int fs, typename CONFIG::address_t ea);
	bool FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea);
	bool Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea);
	bool Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea);
	bool IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size);
	
	void MoveFromDCR(unsigned int rd, unsigned int dcrn);
	void MoveToDCR(unsigned int rs, unsigned int dcrn);

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report) ;

	// Endian interface
    virtual endian_type GetEndianess();
	
	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	virtual bool ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);
	virtual bool InjectReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=                         utility methods                           =
	//=====================================================================
	
	static uint32_t CRFieldMask(uint32_t crfD)
	{
		return (crfD != 7 ? 0xFFFFFFFFul >> ((crfD + 1) * 4) : 0) | (crfD != 0 ? (0xFFFFFFFFul << (8 - crfD) * 4) : 0);
	}

	//=====================================================================
	//=                    user level registers set/get methods           =
	//=====================================================================
	
	inline uint32_t GetGPR(unsigned int n) const { return gpr[n]; }
	inline void SetGPR(unsigned int n, uint32_t value) { gpr[n] = value; }
	inline uint32_t GetLR() const { return lr; }
	inline void SetLR(uint32_t value) { lr = value; }
	inline uint32_t GetCTR() const { return ctr; }
	inline void SetCTR(uint32_t value) { ctr = value; }
	
	//=====================================================================
	//=                  instruction address set/get methods              =
	//=====================================================================
	
	inline uint32_t GetCIA() const { return cia; }
	inline void SetCIA(uint32_t value) { cia = value; }
	inline uint32_t GetNIA() const { return nia; }
	inline void SetNIA(uint32_t value) { nia = value; }
	inline void Branch(uint32_t value) { nia = value; FlushSubsequentInstructions(); }

	//=====================================================================
	//=           special purpose registers set/get methods               =
	//=====================================================================

	bool GetSPR(unsigned int n, uint32_t& value);
	bool SetSPR(unsigned int n, uint32_t value);
	
	inline void SetPVR(uint32_t value) { pvr = value; }
	inline uint32_t GetPVR() const { return pvr; }
	inline uint32_t GetIVOR(unsigned int n) const { return ivor[n]; }
	inline void SetIVOR(unsigned int n, uint32_t value) { ivor[n] = value; }
	inline uint32_t GetIVPR() const { return ivpr; }
	inline void SetIVPR(uint32_t value) { ivpr = value; }
	inline uint32_t GetSPRG(unsigned int n) const { return sprg[n]; }
	inline void SetSPRG(unsigned int n, uint32_t value) { sprg[n] = value; }
	inline uint32_t GetSRR0() const { return srr0; }
	inline void SetSRR0(uint32_t value) { srr0 = value; }
	inline uint32_t GetSRR1() const { return srr1; }
	inline void SetSRR1(uint32_t value) { srr1 = value; }
	inline uint32_t GetCSRR0() const { return csrr0; }
	inline void SetCSRR0(uint32_t value) { csrr0 = value; }
	inline uint32_t GetCSRR1() const { return csrr1; }
	inline void SetCSRR1(uint32_t value) { csrr1 = value; }
	inline uint32_t GetMCSRR0() const { return mcsrr0; }
	inline void SetMCSRR0(uint32_t value) { mcsrr0 = value; }
	inline uint32_t GetMCSRR1() const { return mcsrr1; }
	inline void SetMCSRR1(uint32_t value) { mcsrr1 = value; }
	inline void SetDEAR(uint32_t value) { dear = value; }
	inline uint32_t GetDEAR() const { return dear; }
	inline void SetESR(uint32_t value) { esr = value; }
	inline uint32_t GetESR() const { return esr; }
	inline void SetMCSR(uint32_t value) { mcsr = value; }
	inline uint32_t GetMCSR() const { return mcsr; }
	inline void SetPIR(uint32_t value) { pir = value; }
	inline uint32_t GetPIR() const { return pir; }
	inline void SetUSPRG0(uint32_t value) { usprg0 = value; }
	inline uint32_t GetUSPRG0() const { return usprg0; }
	inline void SetDBSR(uint32_t value) { dbsr = value; }
	inline uint32_t GetDBSR() const { return dbsr; }
	inline void SetDBCR(unsigned int n, uint32_t value) { dbcr[n] = value; }
	inline uint32_t GetDBCR(unsigned int n) const { return dbcr[n]; }
	inline uint32_t GetDBCR0_IDM() const { return (dbcr[0] & CONFIG::DBCR0_IDM_MASK) >> CONFIG::DBCR0_IDM_OFFSET; }
	inline void SetIAC(unsigned int n, uint32_t value) { iac[n] = value; }
	inline uint32_t GetIAC(unsigned int n) const { return iac[n]; }
	inline void SetDAC(unsigned int n, uint32_t value) { dac[n] = value; }
	inline uint32_t GetDAC(unsigned int n) const { return dac[n]; }
	inline void SetDVC(unsigned int n, uint32_t value) { dvc[n] = value; }
	inline uint32_t GetDVC(unsigned int n) const { return dvc[n]; }
	inline void SetTSR(uint32_t value) { tsr = value; }
	inline uint32_t GetTSR() const { return tsr; }
	inline uint32_t GetTSR_ENW() const { return (GetTSR() & CONFIG::TSR_ENW_MASK) >> CONFIG::TSR_ENW_OFFSET; }
	inline uint32_t GetTSR_WIS() const { return (GetTSR() & CONFIG::TSR_WIS_MASK) >> CONFIG::TSR_WIS_OFFSET; }
	inline void SetTCR(uint32_t value) { tcr = value; }
	inline uint32_t GetTCR() const { return tcr; }
	inline uint32_t GetTCR_ARE() const { return (GetTCR() & CONFIG::TCR_ARE_MASK) >> CONFIG::TCR_ARE_OFFSET; }
	inline uint32_t GetTCR_DIE() const { return (GetTCR() & CONFIG::TCR_DIE_MASK) >> CONFIG::TCR_DIE_OFFSET; }
	inline uint32_t GetTCR_FIE() const { return (GetTCR() & CONFIG::TCR_FIE_MASK) >> CONFIG::TCR_FIE_OFFSET; }
	inline uint32_t GetTCR_WIE() const { return (GetTCR() & CONFIG::TCR_WIE_MASK) >> CONFIG::TCR_WIE_OFFSET; }
	inline uint32_t GetTCR_FP() const { return (GetTCR() & CONFIG::TCR_FP_MASK) >> CONFIG::TCR_FP_OFFSET; }
	inline uint32_t GetTCR_WP() const { return (GetTCR() & CONFIG::TCR_WP_MASK) >> CONFIG::TCR_WP_OFFSET; }
	inline uint32_t GetTCR_WRC() const { return (GetTCR() & CONFIG::TCR_WRC_MASK) >> CONFIG::TCR_WRC_OFFSET; }
	inline void SetINV(unsigned int n, uint32_t value) { inv[n] = value; }
	inline uint32_t GetINV(unsigned int n) const { return inv[n]; }
	inline void SetITV(unsigned int n, uint32_t value) { itv[n] = value; }
	inline uint32_t GetITV(unsigned int n) const { return itv[n]; }
	inline void SetCCR0(uint32_t value) { ccr0 = value; }
	inline uint32_t GetCCR0() const { return ccr0; }
	inline uint32_t GetCCR0_CRPE() const { return (GetCCR0() & CONFIG::CCR0_CRPE_MASK) >> CONFIG::CCR0_CRPE_OFFSET; }
	inline uint32_t GetCCR0_FLSTA() const { return (GetCCR0() & CONFIG::CCR0_FLSTA_MASK) >> CONFIG::CCR0_FLSTA_OFFSET; }
	inline void SetCCR1(uint32_t value) { ccr1 = value; }
	inline uint32_t GetCCR1() const { return ccr1; }
	inline uint32_t GetCCR1_FFF() const { return (GetCCR1() & CONFIG::CCR1_FFF_MASK) >> CONFIG::CCR1_FFF_OFFSET; }
	inline uint32_t GetCCR1_TCS() const { return (GetCCR1() & CONFIG::CCR1_TCS_MASK) >> CONFIG::CCR1_TCS_OFFSET; }
	inline void SetDNV(unsigned int n, uint32_t value) { dnv[n] = value; }
	inline uint32_t GetDNV(unsigned int n) const { return dnv[n]; }
	inline void SetDTV(unsigned int n, uint32_t value) { dtv[n] = value; }
	inline uint32_t GetDTV(unsigned int n) const { return dtv[n]; }
	inline void SetDVLIM(uint32_t value) { dvlim = value; }
	inline uint32_t GetDVLIM() const { return dvlim; }
	inline uint32_t GetDVLIM_TFLOOR() const { return (GetDVLIM() & CONFIG::DVLIM_TFLOOR_MASK) >> CONFIG::DVLIM_TFLOOR_OFFSET; }
	inline uint32_t GetDVLIM_TCEILING() const { return (GetDVLIM() & CONFIG::DVLIM_TCEILING_MASK) >> CONFIG::DVLIM_TCEILING_OFFSET; }
	inline uint32_t GetDVLIM_NFLOOR() const { return (GetDVLIM() & CONFIG::DVLIM_NFLOOR_MASK) >> CONFIG::DVLIM_NFLOOR_OFFSET; }
	inline void SetIVLIM(uint32_t value) { ivlim = value; }
	inline uint32_t GetIVLIM() const { return ivlim; }
	inline uint32_t GetIVLIM_TFLOOR() const { return (GetIVLIM() & CONFIG::IVLIM_TFLOOR_MASK) >> CONFIG::IVLIM_TFLOOR_OFFSET; }
	inline uint32_t GetIVLIM_TCEILING() const { return (GetIVLIM() & CONFIG::IVLIM_TCEILING_MASK) >> CONFIG::IVLIM_TCEILING_OFFSET; }
	inline uint32_t GetIVLIM_NFLOOR() const { return (GetIVLIM() & CONFIG::IVLIM_NFLOOR_MASK) >> CONFIG::IVLIM_NFLOOR_OFFSET; }
	inline void SetRSTCFG(uint32_t value) { rstcfg = value; }
	inline uint32_t GetRSTCFG() const { return rstcfg; }
	inline uint32_t GetRSTCFG_U0() const { return (GetRSTCFG() & CONFIG::RSTCFG_U0_MASK) >> CONFIG::RSTCFG_U0_OFFSET; }
	inline uint32_t GetRSTCFG_U1() const { return (GetRSTCFG() & CONFIG::RSTCFG_U1_MASK) >> CONFIG::RSTCFG_U1_OFFSET; }
	inline uint32_t GetRSTCFG_U2() const { return (GetRSTCFG() & CONFIG::RSTCFG_U2_MASK) >> CONFIG::RSTCFG_U2_OFFSET; }
	inline uint32_t GetRSTCFG_U3() const { return (GetRSTCFG() & CONFIG::RSTCFG_U3_MASK) >> CONFIG::RSTCFG_U3_OFFSET; }
	inline uint32_t GetRSTCFG_E() const { return (GetRSTCFG() & CONFIG::RSTCFG_E_MASK) >> CONFIG::RSTCFG_E_OFFSET; }
	inline uint32_t GetRSTCFG_ERPN() const { return (GetRSTCFG() & CONFIG::RSTCFG_ERPN_MASK) >> CONFIG::RSTCFG_ERPN_OFFSET; }
	inline void SetDCDBTRL(uint32_t value) { dcdbtrl = value; }
	inline uint32_t GetDCDBTRL() const { return dcdbtrl; }
	inline void SetDCDBTRH(uint32_t value) { dcdbtrh = value; }
	inline uint32_t GetDCDBTRH() const { return dcdbtrh; }
	inline void SetICDBTRL(uint32_t value) { icdbtrl = value; }
	inline uint32_t GetICDBTRL() const { return icdbtrl; }
	inline void SetICDBTRH(uint32_t value) { icdbtrh = value; }
	inline uint32_t GetICDBTRH() const { return icdbtrh; }
	inline void SetMMUCR(uint32_t value) { mmucr = value; }
	inline void SetMMUCR_STID(uint32_t value) { SetMMUCR((GetMMUCR() & ~CONFIG::MMUCR_STID_MASK) | ((value << CONFIG::MMUCR_STID_OFFSET) & CONFIG::MMUCR_STID_MASK)); }
	inline uint32_t GetMMUCR() const { return mmucr; }
	inline uint32_t GetMMUCR_STID() const { return (GetMMUCR() & CONFIG::MMUCR_STID_MASK) >> CONFIG::MMUCR_STID_OFFSET; }
	inline uint32_t GetMMUCR_STS() const { return (GetMMUCR() & CONFIG::MMUCR_STS_MASK) >> CONFIG::MMUCR_STS_OFFSET; }
	inline void SetPID(uint32_t value) { pid = value; }
	inline uint32_t GetPID() const { return pid; }
	inline typename CONFIG::process_id_t GetProcessID() const { return (typename CONFIG::process_id_t)(pid & 0xff); }
	inline void SetICDBDR(uint32_t value) { icdbdr = value; }
	inline uint32_t GetICDBDR() const { return icdbdr; }
	inline void SetDBDR(uint32_t value) { dbdr = value; }
	inline uint32_t GetDBDR() const { return dbdr; }
	inline void SetDECAR(uint32_t value) { decar = value; }
	inline uint32_t GetDECAR() const { return decar; }
	inline void SetDEC(uint32_t value) { dec = value; }
	inline void DecrementDEC()
	{
		if(GetDEC() > 0)
		{
			SetDEC(GetDEC() - 1);
			if(unlikely(GetDEC() == 0))
			{
				SetException(CONFIG::EXC_DECREMENTER);
				if(GetTCR_ARE())
				{
					SetDEC(GetDECAR());
				}
			}
		}
	}
	inline void DecrementDEC(uint64_t delta)
	{
		uint32_t old_dec = GetDEC();
		if(old_dec > 0)
		{
			uint32_t new_dec = (delta <= old_dec) ? old_dec - delta : 0;
			
			SetDEC(new_dec);
			
			if(unlikely(new_dec == 0))
			{
				SetException(CONFIG::EXC_DECREMENTER);
				if(GetTCR_ARE())
				{
					SetDEC(GetDECAR());
				}
			}
		}
	}
	inline uint32_t GetDEC() const { return dec; }
	inline void IncrementTB() { SetTB(GetTB() + 1); }
	inline void IncrementTB(uint64_t delta) { SetTB(GetTB() + delta); }
	inline uint64_t GetTB() const { return tb; }
	inline void SetTB(uint64_t value)
	{
		uint32_t tcr_fp = GetTCR_FP();
		uint32_t tb_fit_mask = 4096 << (4 * tcr_fp);
		if(!(tb & tb_fit_mask) && (value & tb_fit_mask)) // selected bit toggle from 0 to 1
		{
			//std::cerr << "tb_fit_mask=0x" << std::hex << tb_fit_mask << std::dec << std::endl;
			SetException(CONFIG::EXC_FIXED_INTERVAL_TIMER);
		}
		uint32_t tcr_wp = GetTCR_WP();
		uint32_t tb_watchdog_mask = 1048576 << (4 * tcr_wp);
		if(!(tb & tb_watchdog_mask) && (value & tb_watchdog_mask)) // selected bit toggle from 0 to 1
		{
			//std::cerr << "cnt=" << cnt << ", tb_watchdog_mask=0x" << std::hex << tb_watchdog_mask << std::dec << std::endl;
			SetException(CONFIG::EXC_WATCHDOG_TIMER);
		}
		tb = value;
	}
	inline uint32_t GetTBL() const { return (uint32_t) GetTB(); }
	inline void SetTBL(uint32_t value) { SetTB((GetTB() & 0xffffffff00000000ULL) | ((uint64_t) value)); }
	inline uint32_t GetTBU() const { return (uint32_t)(GetTB() >> 32); }
	inline void SetTBU(uint32_t value) { SetTB((GetTB() & 0x00000000ffffffffULL) | ((uint64_t) value << 32)); }

	uint64_t GetMaxIdleTime() const
	{
		uint64_t delay_dec = 0xffffffffffffffffULL;
		uint64_t delay_fit = 0xffffffffffffffffULL;
		uint64_t delay_watchdog = 0xffffffffffffffffULL;
		
		if(GetMSR_EE())
		{
			if(GetTCR_DIE())
			{
				delay_dec = GetDEC();
				//std::cerr << "Time for DEC to reach zero: " << delay_dec << std::endl;
			}
			
			if(GetTCR_FIE())
			{
				uint32_t tcr_fp = GetTCR_FP();
				uint32_t tb_fit_mask = 4096 << (4 * tcr_fp);
				
				if(GetTB() & tb_fit_mask)
				{
					// time to toogle from 1 to 0
					delay_fit = tb_fit_mask - (GetTB() & (tb_fit_mask - 1));
					// time to toogle from 0 to 1
					delay_fit += tb_fit_mask;
				}
				else
				{
					// time to toogle from 0 to 1
					delay_fit = tb_fit_mask - (GetTB() & (tb_fit_mask - 1));
				}
				//std::cerr << "Time for FIT: " << delay_fit << " (tb=0x" << std::hex << GetTB() << std::dec << ", FIT mask=" << std::hex << tb_fit_mask << std::dec << ")" << std::endl;
			}
		}
		
		if(GetTCR_WIE() && GetMSR_CE())
		{
			uint32_t tcr_wp = GetTCR_WP();
			uint32_t tb_watchdog_mask = 1048576 << (4 * tcr_wp);
			
			if(tb & tb_watchdog_mask)
			{
				// time to toogle from 1 to 0
				delay_watchdog = tb_watchdog_mask - (GetTB() & (tb_watchdog_mask - 1));
				// time to toogle from 0 to 1
				delay_watchdog += tb_watchdog_mask;
			}
			else
			{
				// time to toogle from 0 to 1
				delay_watchdog = tb_watchdog_mask - (GetTB() & (tb_watchdog_mask - 1));
			}
			//std::cerr << "Time for WDG: " << delay_watchdog << " (tb=0x" << std::hex << GetTB() << std::dec << ", WDG mask=" << std::hex << tb_watchdog_mask << std::dec << ")" << std::endl;
		}
		
		uint64_t max_idle_time = delay_dec < delay_fit ? (delay_dec < delay_watchdog ? delay_dec : delay_watchdog) : (delay_fit < delay_watchdog ? delay_fit : delay_watchdog);
		//std::cerr << "Max idle time=" << max_idle_time << std::endl;
		return max_idle_time;
	}
	
	//=====================================================================
	//=                        XER set/get methods                        =
	//=====================================================================

	inline void SetXER(uint32_t value) { xer = value & CONFIG::XER_MASK; }
	inline uint32_t GetXER() const { return xer; }

	// Deprecated methods
	inline uint32_t GetXER_SO() const { return (xer & CONFIG::XER_SO_MASK) >> CONFIG::XER_SO_OFFSET; }
	inline void SetXER_SO() { xer = xer | CONFIG::XER_SO_MASK; }
	inline void ResetXER_SO() { xer = xer & ~CONFIG::XER_SO_MASK; }
	inline uint32_t GetXER_OV() const { return (xer & CONFIG::XER_OV_MASK) >> CONFIG::XER_OV_OFFSET; }
	inline void SetXER_OV() { xer = xer | CONFIG::XER_OV_MASK; }
	inline void ResetXER_OV() { xer = xer & ~CONFIG::XER_OV_MASK; }
	inline uint32_t GetXER_CA() const { return (xer & CONFIG::XER_CA_MASK) >> CONFIG::XER_CA_OFFSET; }
	inline void SetXER_CA() { xer = xer | CONFIG::XER_CA_MASK; }
	inline void ResetXER_CA() { xer = xer & ~CONFIG::XER_CA_MASK; }
	inline uint32_t GetXER_BYTE_COUNT() const { return (xer & CONFIG::XER_BYTE_COUNT_MASK) >> CONFIG::XER_BYTE_COUNT_OFFSET; }

	
	//=====================================================================
	//=                         CR set/get methods                        =
	//=====================================================================

	inline uint32_t GetCR() const { return cr; }
	inline void SetCR(uint32_t value) { cr = value; }

	// Deprecated methods
	inline uint32_t GetCRF(unsigned int crf) const { return cr >> ((7 - crf) * 4); }
	inline void SetCRF(unsigned int crfd, uint32_t value)
	{
		uint32_t _c = value << ((7 - crfd) * 4) ; // positionning the crX field
		uint32_t _d = CRFieldMask(crfd) ;
		cr = cr & _d ;	//Deleting old sub-registrer value
		cr = cr | _c ;  //Writing the new one
	}
	
	//=====================================================================
	//=                        MSR set/get methods                        =
	//=====================================================================
	
	uint32_t GetMSR() const { return msr; }
	void SetMSR(uint32_t value);
	inline uint32_t GetMSR_WE() const { return (GetMSR() & CONFIG::MSR_WE_MASK) >> CONFIG::MSR_WE_OFFSET; }
	inline uint32_t GetMSR_CE() const { return (GetMSR() & CONFIG::MSR_CE_MASK) >> CONFIG::MSR_CE_OFFSET; }
	inline uint32_t GetMSR_EE() const { return (GetMSR() & CONFIG::MSR_EE_MASK) >> CONFIG::MSR_EE_OFFSET; }
	inline uint32_t GetMSR_PR() const { return (GetMSR() & CONFIG::MSR_PR_MASK) >> CONFIG::MSR_PR_OFFSET; }
	inline typename CONFIG::PrivilegeLevel GetPrivilegeLevel() const { return (typename CONFIG::PrivilegeLevel) GetMSR_PR(); }
	inline uint32_t GetMSR_FP() const { return (GetMSR() & CONFIG::MSR_FP_MASK) >> CONFIG::MSR_FP_OFFSET; }
	inline uint32_t GetMSR_ME() const { return (GetMSR() & CONFIG::MSR_ME_MASK) >> CONFIG::MSR_ME_OFFSET; }
	inline uint32_t GetMSR_FE0() const { return (GetMSR() & CONFIG::MSR_FE0_MASK) >> CONFIG::MSR_FE0_OFFSET; }
	inline uint32_t GetMSR_DWE() const { return (GetMSR() & CONFIG::MSR_DWE_MASK) >> CONFIG::MSR_DWE_OFFSET; }
	inline uint32_t GetMSR_DE() const { return (GetMSR() & CONFIG::MSR_DE_MASK) >> CONFIG::MSR_DE_OFFSET; }
	inline uint32_t GetMSR_FE1() const { return (GetMSR() & CONFIG::MSR_FE1_MASK) >> CONFIG::MSR_FE1_OFFSET; }
	inline uint32_t GetMSR_IS() const { return (GetMSR() & CONFIG::MSR_IS_MASK) >> CONFIG::MSR_IS_OFFSET; }
	inline typename CONFIG::address_space_t GetInstructionAddressSpace() const { return (typename CONFIG::address_space_t) GetMSR_IS(); }
	inline uint32_t GetMSR_DS() const { return (GetMSR() & CONFIG::MSR_DS_MASK) >> CONFIG::MSR_DS_OFFSET; }
	inline typename CONFIG::address_space_t GetDataAddressSpace() const { return (typename CONFIG::address_space_t) GetMSR_DS(); }
	inline void SetMSR_WE() { SetMSR(GetMSR() | CONFIG::MSR_WE_MASK); }
	inline void SetMSR_CE() { SetMSR(GetMSR() | CONFIG::MSR_CE_MASK); }
	inline void SetMSR_EE() { SetMSR(GetMSR() | CONFIG::MSR_EE_MASK); }
	inline void SetMSR_PR() { SetMSR(GetMSR() | CONFIG::MSR_PR_MASK); }
	inline void SetMSR_FP() { SetMSR(GetMSR() | CONFIG::MSR_FP_MASK); }
	inline void SetMSR_ME() { SetMSR(GetMSR() | CONFIG::MSR_ME_MASK); }
	inline void SetMSR_FE0() { SetMSR(GetMSR() | CONFIG::MSR_FE0_MASK); }
	inline void SetMSR_DWE() { SetMSR(GetMSR() | CONFIG::MSR_DWE_MASK); }
	inline void SetMSR_DE() { SetMSR(GetMSR() | CONFIG::MSR_DE_MASK); }
	inline void SetMSR_FE1() { SetMSR(GetMSR() | CONFIG::MSR_FE1_MASK); }
	inline void SetMSR_IS() { SetMSR(GetMSR() | CONFIG::MSR_IS_MASK); }
	inline void SetMSR_DS() { SetMSR(GetMSR() | CONFIG::MSR_DS_MASK); }
	inline void ResetMSR_WE() { SetMSR(GetMSR() & ~CONFIG::MSR_WE_MASK); }
	inline void ResetMSR_CE() { SetMSR(GetMSR() & ~CONFIG::MSR_CE_MASK); }
	inline void ResetMSR_EE() { SetMSR(GetMSR() & ~CONFIG::MSR_EE_MASK); }
	inline void ResetMSR_PR() { SetMSR(GetMSR() & ~CONFIG::MSR_PR_MASK); }
	inline void ResetMSR_FP() { SetMSR(GetMSR() & ~CONFIG::MSR_FP_MASK); }
	inline void ResetMSR_ME() { SetMSR(GetMSR() & ~CONFIG::MSR_ME_MASK); }
	inline void ResetMSR_FE0() { SetMSR(GetMSR() & ~CONFIG::MSR_FE0_MASK); }
	inline void ResetMSR_DWE() { SetMSR(GetMSR() & ~CONFIG::MSR_DWE_MASK); }
	inline void ResetMSR_DE() { SetMSR(GetMSR() & ~CONFIG::MSR_DE_MASK); }
	inline void ResetMSR_FE1() { SetMSR(GetMSR() & ~CONFIG::MSR_FE1_MASK); }
	inline void ResetMSR_IS() { SetMSR(GetMSR() & ~CONFIG::MSR_IS_MASK); }
	inline void ResetMSR_DS() { SetMSR(GetMSR() & ~CONFIG::MSR_DS_MASK); }

	//=====================================================================
	//=                        ESR set/get methods                        =
	//=====================================================================

	inline uint32_t GetESR_MCI() const { return (GetESR() & CONFIG::ESR_MCI_MASK) >> CONFIG::ESR_MCI_OFFSET; }
	inline void SetESR_MCI() { SetESR(GetESR() | CONFIG::ESR_MCI_MASK); }

	inline uint32_t GetESR_PIL() const { return (GetESR() & CONFIG::ESR_PIL_MASK) >> CONFIG::ESR_PIL_OFFSET; }
	inline void SetESR_PIL() { SetESR(GetESR() | CONFIG::ESR_PIL_MASK); }

	inline uint32_t GetESR_PPR() const { return (GetESR() & CONFIG::ESR_PPR_MASK) >> CONFIG::ESR_PPR_OFFSET; }
	inline void SetESR_PPR() { SetESR(GetESR() | CONFIG::ESR_PPR_MASK); }

	inline uint32_t GetESR_PTR() const { return (GetESR() & CONFIG::ESR_PTR_MASK) >> CONFIG::ESR_PTR_OFFSET; }
	inline void SetESR_PTR() { SetESR(GetESR() | CONFIG::ESR_PTR_MASK); }

	inline uint32_t GetESR_FP() const { return (GetESR() & CONFIG::ESR_FP_MASK) >> CONFIG::ESR_FP_OFFSET; }
	inline void SetESR_FP() { SetESR(GetESR() | CONFIG::ESR_FP_MASK); }

	inline uint32_t GetESR_ST() const { return (GetESR() & CONFIG::ESR_ST_MASK) >> CONFIG::ESR_ST_OFFSET; }
	inline void SetESR_ST() { SetESR(GetESR() | CONFIG::ESR_ST_MASK); }

	inline uint32_t GetESR_DLK() const { return (GetESR() & CONFIG::ESR_DLK_MASK) >> CONFIG::ESR_DLK_OFFSET; }
	inline void SetESR_DLK(uint32_t value) { SetESR((GetESR() & ~CONFIG::ESR_DLK_MASK) | ((value << CONFIG::ESR_DLK_OFFSET) & CONFIG::ESR_DLK_MASK)); }

	inline uint32_t GetESR_AP() const { return (GetESR() & CONFIG::ESR_AP_MASK) >> CONFIG::ESR_AP_OFFSET; }
	inline void SetESR_AP() { SetESR(GetESR() | CONFIG::ESR_AP_MASK); }

	inline uint32_t GetESR_PUO() const { return (GetESR() & CONFIG::ESR_PUO_MASK) >> CONFIG::ESR_PUO_OFFSET; }
	inline void SetESR_PUO() { SetESR(GetESR() | CONFIG::ESR_PUO_MASK); }

	inline uint32_t GetESR_BO() const { return (GetESR() & CONFIG::ESR_BO_MASK) >> CONFIG::ESR_BO_OFFSET; }
	inline void SetESR_BO() { SetESR(GetESR() | CONFIG::ESR_BO_MASK); }

	inline uint32_t GetESR_PIE() const { return (GetESR() & CONFIG::ESR_PIE_MASK) >> CONFIG::ESR_PIE_OFFSET; }
	inline void SetESR_PIE() { SetESR(GetESR() | CONFIG::ESR_PIE_MASK); }

	inline uint32_t GetESR_PCRE() const { return (GetESR() & CONFIG::ESR_PCRE_MASK) >> CONFIG::ESR_PCRE_OFFSET; }
	inline void SetESR_PCRE() { SetESR(GetESR() | CONFIG::ESR_PCRE_MASK); }

	inline uint32_t GetESR_PCMP() const { return (GetESR() & CONFIG::ESR_PCMP_MASK) >> CONFIG::ESR_PCMP_OFFSET; }
	inline void SetESR_PCMP() { SetESR(GetESR() | CONFIG::ESR_PCMP_MASK); }

	inline uint32_t GetESR_PCRF() const { return (GetESR() & CONFIG::ESR_PCRF_MASK) >> CONFIG::ESR_PCRF_OFFSET; }
	inline void SetESR_PCRF(uint32_t value) { SetESR((GetESR() & ~CONFIG::ESR_PCRF_MASK) | ((value << CONFIG::ESR_PCRF_OFFSET) & CONFIG::ESR_PCRF_MASK)); }
	

	//=====================================================================
	//=                   FPU registers set/get methods                   =
	//=====================================================================

	inline uint32_t GetFPSCR() const { return fpscr; }
	inline void SetFPSCR(uint32_t value) { fpscr = value; }
	
	//=====================================================================
	//=               Cache management instructions handling              =
	//=====================================================================
	
	void Dcba(typename CONFIG::address_t addr);
	void Dcbf(typename CONFIG::address_t addr);
	void Dcbi(typename CONFIG::address_t addr);
	void Dcbst(typename CONFIG::address_t addr);
	void Dcbz(typename CONFIG::address_t addr);
	void Dccci(typename CONFIG::address_t addr);
	void Dcread(typename CONFIG::address_t addr, unsigned int rd);
	void Icbi(typename CONFIG::address_t addr);
	void Icbt(typename CONFIG::address_t addr);
	void Iccci(typename CONFIG::address_t addr);
	void Icread(typename CONFIG::address_t addr);
	
	//=====================================================================
	//=  Translation look-aside buffers management instructions handling  =
	//=====================================================================
	
	void Tlbre(unsigned int rd, uint32_t way, uint8_t ws);
	void Tlbsx(unsigned int rd, typename CONFIG::address_t addr, unsigned int rc);
	void Tlbwe(uint32_t s, uint32_t way, uint8_t ws);

	//=====================================================================
	//=               Linked Load-Store instructions handling             =
	//=====================================================================
	
	void Lwarx(unsigned int rd, typename CONFIG::address_t addr);
	void Stwcx(unsigned int rs, typename CONFIG::address_t addr);

	//=====================================================================
	//=                Synchronization instructions handling              =
	//=====================================================================

	void Isync();

	//=====================================================================
	//=           Return from interrupt instructions handling             =
	//=====================================================================
	
	void Rfi();
	void Rfci();
	void Rfmci();
	
	
	void SystemCall();

	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

	virtual unisim::util::debug::Register *GetRegister(const char *name);
	virtual string Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t& next_addr);
	virtual const char *GetArchitectureName() const;
	string GetObjectFriendlyName(typename CONFIG::address_t addr);
	string GetFunctionFriendlyName(typename CONFIG::address_t addr);
	int StringLength(typename CONFIG::address_t addr);
	std::string ReadString(typename CONFIG::address_t addr, unsigned int count = 0);
	typename CONFIG::address_t linux_printk_buf_addr;
	uint32_t linux_printk_buf_size;

	
	//=====================================================================
	//=          Instruction prefetch buffer handling methods             =
	//=====================================================================
	
	void FlushSubsequentInstructions();
	
	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================
	
	inline void SetException(unsigned int _exc, typename CONFIG::address_t _addr = 0, typename CONFIG::MemoryAccessType _memory_access_type = CONFIG::MAT_READ) { SetExceptionMask(1 << _exc, _addr, _memory_access_type); }
	void ResetException(unsigned int _exc) { ResetExceptionMask(1 << _exc); }
	void SetExceptionMask(uint32_t exc, typename CONFIG::address_t addr = 0, typename CONFIG::MemoryAccessType memory_access_type = CONFIG::MAT_READ);
	void ResetExceptionMask(uint32_t exc);

	inline bool HasException() const { return exc | (GetTSR() & (CONFIG::TSR_DIS_MASK | CONFIG::TSR_FIS_MASK | CONFIG::TSR_WIS_MASK)); }
	inline bool HasCriticalInputException() const { return exc & CONFIG::EXC_MASK_CRITICAL_INPUT; }
	inline bool HasExternalInputException() const { return exc & CONFIG::EXC_MASK_EXTERNAL_INPUT; }
	inline bool HasDecrementerException() const { return GetTSR() & CONFIG::TSR_DIS_MASK; }
	inline bool HasDebugException() const { return exc & CONFIG::EXC_MASK_DEBUG; }
	inline bool HasFixedIntervalTimerException() const { return GetTSR() & CONFIG::TSR_FIS_MASK; }
	inline bool HasWatchDogTimerException() const { return GetTSR() & CONFIG::TSR_WIS_MASK; }
	inline bool HasMachineCheckException() const { return exc & CONFIG::EXC_MASK_MACHINE_CHECK; }
	inline bool HasDataStorageException() const { return exc & CONFIG::EXC_MASK_DATA_STORAGE; }
	inline bool HasInstructionStorageException() const { return exc & CONFIG::EXC_MASK_INSTRUCTION_STORAGE; }
	inline bool HasDataTLBErrorException() const { return exc & CONFIG::EXC_MASK_DATA_TLB_ERROR; }
	inline bool HasInstructionTLBErrorException() const { return exc & CONFIG::EXC_MASK_INSTRUCTION_TLB_ERROR; }
	inline bool HasAlignmentException() const { return exc & CONFIG::EXC_MASK_ALIGNMENT; }
	inline bool HasProgramException() const { return exc & CONFIG::EXC_MASK_PROGRAM; }
	inline bool HasSystemCallException() const { return exc & CONFIG::EXC_MASK_SYSTEM_CALL; }
	inline bool HasFloatingPointUnavailableException() const { return exc & CONFIG::EXC_MASK_FLOATING_POINT_UNAVAILABLE; }
	inline bool HasAuxiliaryProcessorUnavailableException() const { return exc & CONFIG::EXC_MASK_AUXILIARY_PROCESSOR_UNAVAILABLE; }

protected:

	//=====================================================================
	//=          DEC/TBL/TBU bus-time based update methods                =
	//=====================================================================
	
	void OnTimerClock();
	void RunTimers(uint64_t delta);
	
	//=====================================================================
	//=                        Debugging stuff                            =
	//=====================================================================

	Logger logger;
    /** indicates if the memory accesses require to be reported */
    bool requires_memory_access_reporting;
    /** indicates if the finished instructions require to be reported */
    bool requires_finished_instruction_reporting;
	
	inline bool IsVerboseSetup() const { return verbose_all || verbose_setup; }
	inline bool IsVerboseStep() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STEP_ENABLE && (verbose_all || verbose_step); }
	inline bool IsVerboseITLB() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_ITLB_ENABLE && (verbose_all || verbose_itlb); }
	inline bool IsVerboseDTLB() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_DTLB_ENABLE && (verbose_all || verbose_dtlb); }
	inline bool IsVerboseUTLB() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_UTLB_ENABLE && (verbose_all || verbose_utlb); }
	inline bool IsVerboseDL1() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_DL1_ENABLE && (verbose_all || verbose_dl1); }
	inline bool IsVerboseIL1() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_IL1_ENABLE && (verbose_all || verbose_il1); }
	inline bool IsVerboseLoad() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_LOAD_ENABLE && (verbose_all || verbose_load); }
	inline bool IsVerboseStore() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STORE_ENABLE && (verbose_all || verbose_store); }
	inline bool IsVerboseReadMemory() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_READ_MEMORY_ENABLE && (verbose_all || verbose_read_memory); }
	inline bool IsVerboseWriteMemory() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_WRITE_MEMORY_ENABLE && (verbose_all || verbose_write_memory); }
	inline bool IsVerboseException() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception); }
	inline bool IsVerboseSetMSR() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_MSR_ENABLE && (verbose_all || verbose_set_msr); }
	inline bool IsVerboseTlbwe() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_TLBWE_ENABLE && (verbose_all || verbose_tlbwe); }
	//=====================================================================
	//=                      Bus access methods                           =
	//=====================================================================

	virtual bool PLBInsnRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr = CONFIG::SA_DEFAULT);
	virtual bool PLBDataRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr = CONFIG::SA_DEFAULT);
	virtual bool PLBDataWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr = CONFIG::SA_DEFAULT);
	virtual void DCRRead(unsigned int dcrn, uint32_t& value);
	virtual void DCRWrite(unsigned int dcrn, uint32_t value);
	
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================
	
	uint64_t cpu_cycle_time; //!< CPU cycle time in ps
	uint64_t voltage;        //!< CPU voltage in mV
	uint64_t timer_cycle;  //!< Number of timer cycles

private:

	//=====================================================================
	//=                     Cache/MMU handling methods                    =
	//=====================================================================

	void InvalidateITLB();
	void ResetITLB();
	void InvalidateDTLB();
	void ResetDTLB();
	void InvalidateUTLB();
	void ResetUTLB();
	template <bool DEBUG> inline void LookupITLB(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	template <bool DEBUG> inline void LookupDTLB(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	template <bool DEBUG> inline void LookupUTLB(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	void UpdateITLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access);
	void UpdateDTLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access);
	void UpdateUTLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access);
	void ChooseEntryToEvictITLB(MMUAccess<CONFIG>& mmu_access);
	void ChooseEntryToEvictDTLB(MMUAccess<CONFIG>& mmu_access);
	void ChooseEntryToEvictUTLB(MMUAccess<CONFIG>& mmu_access);
	template <bool DEBUG> inline bool EmuTranslateAddress(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	void DumpITLB(std::ostream& os);
	void DumpDTLB(std::ostream& os);
	void DumpUTLB(std::ostream& os, int highlight_index = -1, int hightligh_way = -1);
	
	void InvalidateDL1Set(uint32_t index);
	void InvalidateDL1();
	void InvalidateIL1Set(uint32_t index);
	void InvalidateIL1();
	void ClearAccessDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void ClearAccessIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	template <bool DEBUG> inline void LookupDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access) ALWAYS_INLINE;
	template <bool DEBUG> inline void LookupIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access) ALWAYS_INLINE;
	bool EmuEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void EmuEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void ChooseLineToEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void ChooseLineToEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void UpdateReplacementPolicyDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void UpdateReplacementPolicyIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	bool EmuFillDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	bool EmuFillIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access, MMUAccess<CONFIG>& mmu_access);
	bool EmuFetch(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	inline bool EmuFetch(typename CONFIG::address_t addr, uint32_t& insn);
	inline bool EmuLoad(MMUAccess<CONFIG>& mmu_access, void *buffer, uint32_t size) ALWAYS_INLINE;
	inline bool EmuStore(MMUAccess<CONFIG>& mmu_access, const void *buffer, uint32_t size) ALWAYS_INLINE;
	
	template <class T, bool REVERSE, bool FORCE_BIG_ENDIAN> bool EmuLoad(T& value, typename CONFIG::address_t ea);
	template <class T, bool REVERSE, bool FORCE_BIG_ENDIAN> bool EmuStore(T value, typename CONFIG::address_t ea);
	
	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	bool ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr);
	bool WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr);
public:
	void EnterSystemCallISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterMachineCheckISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterDecrementerISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterDebugISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterExternalInputISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterCriticalInputISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterDataStorageISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterInstructionStorageISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterDataTLBErrorISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterInstructionTLBErrorISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterAlignmentISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterProgramISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterFloatingPointUnavailableISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterAuxiliaryProcessorUnavailableISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterFixedIntervalTimerISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	void EnterWatchDogTimerISR(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
private:	
	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
	bool verbose_all;
	bool verbose_setup;
	bool verbose_step;
	bool verbose_itlb;
	bool verbose_dtlb;
	bool verbose_utlb;
	bool verbose_dl1;
	bool verbose_il1;
	bool verbose_load;
	bool verbose_store;
	bool verbose_read_memory;
	bool verbose_write_memory;
	bool verbose_exception;
	bool verbose_set_msr;
	bool verbose_tlbwe;
	bool enable_linux_printk_snooping;
	bool enable_linux_syscall_snooping;
	uint64_t trap_on_instruction_counter;
	bool enable_trap_on_exception;
	bool enable_halt_on;
	typename CONFIG::address_t halt_on_addr;
	std::string halt_on;
	uint64_t max_inst;                                         //!< maximum number of instructions to execute
	uint64_t num_interrupts;
	uint64_t num_decrementer_interrupts;
	uint64_t num_fixed_interval_timer_interrupts;
	uint64_t num_watchdog_timer_interrupts;
	uint64_t num_debug_interrupts;
	uint64_t num_external_input_interrupts;
	uint64_t num_critical_input_interrupts;
	uint64_t num_machine_check_interrupts;
	uint64_t num_data_storage_interrupts;
	uint64_t num_instruction_storage_interrupts;
	uint64_t num_data_tlb_error_interrupts;
	uint64_t num_instruction_tlb_error_interrupts;
	uint64_t num_alignment_interrupts;
	uint64_t num_program_interrupts;
	uint64_t num_system_call_interrupts;
	uint64_t num_floating_point_unavailable_interrupts;
	uint64_t num_auxiliary_processor_unavailable_interrupts;

	map<string, unisim::util::debug::Register *> registers_registry;       //!< Every CPU register interfaces
	std::vector<unisim::kernel::service::VariableBase *> registers_registry2;       //!< Every CPU register
	uint64_t instruction_counter;                              //!< Number of executed instructions
	bool fp32_estimate_inv_warning;
	bool fp64_estimate_inv_sqrt_warning;

	inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	inline void MonitorLoad(typename CONFIG::address_t ea, uint32_t size);
	inline void MonitorStore(typename CONFIG::address_t ea, uint32_t size);
	bool FloatingPointSelfTest();

	//=====================================================================
	//=                    registers set/get methods                      =
	//=====================================================================



	//=====================================================================
	//=                        PowerPC registers                          =
	//=====================================================================

	// program counter
	uint32_t cia;            //!< Current instruction address (all)
	uint32_t nia;            //!< Next instruction address (all)
	
	// branch control
	uint32_t cr;
	uint32_t ctr;
	uint32_t lr;
	
	// cache control
	uint32_t dnv[CONFIG::NUM_DNVS];
	uint32_t dtv[CONFIG::NUM_DTVS];
	uint32_t dvlim;
	uint32_t inv[CONFIG::NUM_INVS];
	uint32_t itv[CONFIG::NUM_ITVS];
	uint32_t ivlim;
	
	// cache debug
	uint32_t dcdbtrh;
	uint32_t dcdbtrl;
	uint32_t icdbdr;
	uint32_t icdbtrh;
	uint32_t icdbtrl;
	
	// debug
	uint32_t dac[CONFIG::NUM_DACS];
	uint32_t dbcr[CONFIG::NUM_DBCRS];
	uint32_t dbdr;
	uint32_t dbsr;
	uint32_t dvc[CONFIG::NUM_DVCS];
	uint32_t iac[CONFIG::NUM_IACS];
	
	// integer processing
	uint32_t gpr[CONFIG::NUM_GPRS];
	uint32_t xer;
	
	// floating point
	SoftDouble fpr[CONFIG::NUM_FPRS];     //!< floating point registers (C++ objects implementing IEEE 754 floating point numbers)
	uint32_t fpscr;                       //!< floating point status and control register
	
	// interrupt processing
	uint32_t csrr0;
	uint32_t csrr1;
	uint32_t dear;
	uint32_t esr;
	uint32_t ivor[CONFIG::NUM_IVORS];
	uint32_t ivpr;
	uint32_t mcsr;
	uint32_t mcsrr0;
	uint32_t mcsrr1;
	uint32_t srr0;
	uint32_t srr1;
	
	// processor control
	uint32_t ccr0;
	uint32_t ccr1;
	uint32_t msr;
	uint32_t pir;
	uint32_t pvr;
	uint32_t rstcfg;
	uint32_t sprg[CONFIG::NUM_SPRGS];
	uint32_t usprg0;
	
	// storage control
	uint32_t mmucr;
	uint32_t pid;
	
	// timer
	uint32_t dec;
	uint32_t decar;
	uint64_t tb; // tbu || tbl
	uint32_t tcr;
	uint32_t tsr;

	//=====================================================================
	//=                     L1 Instruction Cache                          =
	//=====================================================================
	
	unisim::component::cxx::cache::Cache<class CONFIG::IL1_CONFIG> il1;
	uint64_t num_il1_accesses;
	uint64_t num_il1_misses;

	//=====================================================================
	//=                         L1 Data Cache                             =
	//=====================================================================
	
	unisim::component::cxx::cache::Cache<class CONFIG::DL1_CONFIG> dl1;
	uint64_t num_dl1_accesses;
	uint64_t num_dl1_misses;

	//=====================================================================
	//=                   Translation Look-aside buffers                  =
	//=====================================================================

	TLB<class CONFIG::ITLB_CONFIG> itlb;
	TLB<class CONFIG::DTLB_CONFIG> dtlb;
	TLB<class CONFIG::UTLB_CONFIG> utlb;
	uint64_t num_itlb_accesses;
	uint64_t num_itlb_misses;
	uint64_t num_dtlb_accesses;
	uint64_t num_dtlb_misses;
	uint64_t num_utlb_accesses;
	uint64_t num_utlb_misses;

	//=====================================================================
	//=               Instruction prefetch buffer                         =
	//=====================================================================
	
	unsigned int num_insn_in_prefetch_buffer;                  //!< Number of instructions currently in the prefetch buffer
	unsigned int cur_insn_in_prefetch_buffer;                  //!< Prefetch buffer index of the current instruction to be executed
	uint32_t prefetch_buffer[CONFIG::NUM_PREFETCH_BUFFER_ENTRIES];     //!< The instruction prefetch buffer
	
	//=====================================================================
	//=                   lwarx/stwcx. reservation                        =
	//=====================================================================
	
	bool reserve;                                             //!< true if a reservation has been set (by lwarx), otherwise false
	typename CONFIG::address_t reserve_addr;                                   //!< address of the reservation
	
	//=====================================================================
	//=                          Exceptions                               =
	//=====================================================================
	
	uint32_t exc;                                             //!< exception mask (bits are ordered according to exception priority)
	typename CONFIG::address_t exc_addr;                      //!< effective address, if any, that caused the exception
	typename CONFIG::MemoryAccessType exc_memory_access_type; //!< memory access type, if any, that caused the exception
	void (unisim::component::cxx::processor::powerpc::ppc440::CPU<CONFIG>::*enter_non_maskable_isr_table[CONFIG::NUM_NON_MASKABLE_EXCEPTIONS])(unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation);
	
	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_cpu_cycle_time;             //!< linked to member cpu_cycle_time
	Parameter<uint64_t> param_voltage;                    //!< linked to member voltage
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_setup;
	Parameter<bool> param_verbose_step;
	Parameter<bool> param_verbose_itlb;
	Parameter<bool> param_verbose_dtlb;
	Parameter<bool> param_verbose_utlb;
	Parameter<bool> param_verbose_dl1;
	Parameter<bool> param_verbose_il1;
	Parameter<bool> param_verbose_load;
	Parameter<bool> param_verbose_store;
	Parameter<bool> param_verbose_read_memory;
	Parameter<bool> param_verbose_write_memory;
	Parameter<bool> param_verbose_exception;
	Parameter<bool> param_verbose_set_msr;
	Parameter<bool> param_verbose_tlbwe;
	Parameter<bool> param_enable_linux_printk_snooping;
	Parameter<bool> param_enable_linux_syscall_snooping;
	Parameter<uint64_t> param_trap_on_instruction_counter;
	Parameter<bool> param_enable_trap_on_exception;
	Parameter<std::string> param_halt_on;

	//=====================================================================
	//=                    CPU run-time statistics                        =
	//=====================================================================

	Statistic<uint64_t> stat_instruction_counter;
	Statistic<uint64_t> stat_timer_cycle;                   //!< Number of timer cycles
	Statistic<uint64_t> stat_num_il1_accesses;
	Statistic<uint64_t> stat_num_il1_misses;
	Formula<double> formula_il1_miss_rate;
	Statistic<uint64_t> stat_num_dl1_accesses;
	Statistic<uint64_t> stat_num_dl1_misses;
	Formula<double> formula_dl1_miss_rate;
	Statistic<uint64_t> stat_num_itlb_accesses;
	Statistic<uint64_t> stat_num_itlb_misses;
	Formula<double> formula_itlb_miss_rate;
	Statistic<uint64_t> stat_num_dtlb_accesses;
	Statistic<uint64_t> stat_num_dtlb_misses;
	Formula<double> formula_dtlb_miss_rate;
	Statistic<uint64_t> stat_num_utlb_accesses;
	Statistic<uint64_t> stat_num_utlb_misses;
	Formula<double> formula_utlb_miss_rate;
	Statistic<uint64_t> stat_num_interrupts;
	Statistic<uint64_t> stat_num_decrementer_interrupts;
	Statistic<uint64_t> stat_num_fixed_interval_timer_interrupts;
	Statistic<uint64_t> stat_num_watchdog_timer_interrupts;
	Statistic<uint64_t> stat_num_debug_interrupts;
	Statistic<uint64_t> stat_num_external_input_interrupts;
	Statistic<uint64_t> stat_num_critical_input_interrupts;
	Statistic<uint64_t> stat_num_machine_check_interrupts;
	Statistic<uint64_t> stat_num_data_storage_interrupts;
	Statistic<uint64_t> stat_num_instruction_storage_interrupts;
	Statistic<uint64_t> stat_num_data_tlb_error_interrupts;
	Statistic<uint64_t> stat_num_instruction_tlb_error_interrupts;
	Statistic<uint64_t> stat_num_alignment_interrupts;
	Statistic<uint64_t> stat_num_program_interrupts;
	Statistic<uint64_t> stat_num_system_call_interrupts;
	Statistic<uint64_t> stat_num_floating_point_unavailable_interrupts;
	Statistic<uint64_t> stat_num_auxiliary_processor_unavailable_interrupts;
};

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
