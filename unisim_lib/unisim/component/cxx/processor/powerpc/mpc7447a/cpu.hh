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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_HH__

#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/component/cxx/processor/powerpc/floating.hh>
#include <unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh>
#include <unisim/component/cxx/processor/powerpc/mpc7447a/config.hh>
#include <unisim/component/cxx/cache/cache.hh>
#include <unisim/component/cxx/tlb/tlb.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/kernel.hh>
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
#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <map>
#include <iosfwd>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

using unisim::service::interfaces::DebugYielding;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::MemoryAccessReportingType;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::CachePowerEstimator;
using unisim::service::interfaces::PowerMode;
using unisim::service::interfaces::Registers;
using namespace unisim::util::endian;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::LinuxOS;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Synchronizable;
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::ParameterArray;
using unisim::kernel::variable::Formula;
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

typedef union
{
	uint8_t b[16];
	uint16_t h[8];
	uint32_t w[4];
} vr_t;

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

	typename CONFIG::address_t addr;
	typename CONFIG::PrivilegeLevel privilege_level;
	typename CONFIG::MemoryAccessType memory_access_type;
	typename CONFIG::MemoryType memory_type;

	// LookupBAT intermediate computations
	uint32_t bepi;							// BEPI bit field

	// LookupTLB intermediate computations
	uint32_t sr_num;						// Segment register number
	uint32_t virtual_segment_id;			// Virtual segment id
	uint32_t sr_ks;							// Supervisor key of a segment register
	uint32_t sr_kp;							// User key of a segment register
	uint32_t sr_noexecute;					// No-execute bit of a segment register
	typename CONFIG::virtual_address_t virtual_addr;			// Virtual address
	typename CONFIG::virtual_address_t base_virtual_addr;	// Base virtual address
	TLBSet<class CONFIG::ITLB_CONFIG> *itlb_set;		// An ITLB Set
	TLBEntry<class CONFIG::ITLB_CONFIG> *itlb_entry;	// An ITLB entry
	TLBSet<class CONFIG::DTLB_CONFIG> *dtlb_set;		// A DTLB Set
	TLBEntry<class CONFIG::DTLB_CONFIG> *dtlb_entry;	// A DTLB entry
	uint32_t tlb_index;						// An index in DTLB/ITLB
	uint32_t tlb_way;
	bool key;
	bool force_page_table_walk;

	// Hardware Page table Search intermediate computations
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index

	// Output
	bool bat_hit;
	bool tlb_hit;
	typename CONFIG::physical_address_t physical_addr;
	typename CONFIG::address_t protection_boundary;
	typename CONFIG::WIMG wimg;

	friend ostream& operator << <CONFIG> (ostream& os, const MMUAccess<CONFIG>& mmu_access);
};

class VectorRegisterView : public unisim::kernel::VariableBase
{
public:
	VectorRegisterView(const char *name, unisim::kernel::Object *owner, vr_t& storage, const char *description);
	virtual ~VectorRegisterView();
	virtual const char *GetDataTypeName() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator std::string () const;
	virtual unisim::kernel::VariableBase& operator = (bool value);
	virtual unisim::kernel::VariableBase& operator = (long long value);
	virtual unisim::kernel::VariableBase& operator = (unsigned long long value);
	virtual unisim::kernel::VariableBase& operator = (double value);
	virtual unisim::kernel::VariableBase& operator = (const char * value);
private:
	vr_t& storage;
};

class TimeBaseRegisterInterface : public unisim::service::interfaces::Register
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

class TimeBaseRegisterView : public unisim::kernel::VariableBase
{
public:
	typedef enum
	{
		TB_LOW,
		TB_HIGH
	} Type;
	TimeBaseRegisterView(const char *name, unisim::kernel::Object *owner, uint64_t& storage, Type type, const char *description);
	virtual ~TimeBaseRegisterView();
	virtual const char *GetDataTypeName() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator std::string () const;
	virtual unisim::kernel::VariableBase& operator = (bool value);
	virtual unisim::kernel::VariableBase& operator = (long long value);
	virtual unisim::kernel::VariableBase& operator = (unsigned long long value);
	virtual unisim::kernel::VariableBase& operator = (double value);
	virtual unisim::kernel::VariableBase& operator = (const char * value);
private:
	uint64_t& storage;
	Type type;
};

template <class CONFIG>
class CPU :
	public unisim::component::cxx::processor::powerpc::mpc7447a::Decoder<CONFIG>,
	public Client<Loader>,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
	public Client<DebugYielding>,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Client<TrapReporting>,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<unisim::service::interfaces::Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::address_t> >,
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

	ServiceImport<Loader> kernel_loader_import;
	ServiceImport<DebugYielding> debug_yielding_import;
	ServiceImport<MemoryAccessReporting<typename CONFIG::address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<typename CONFIG::address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<typename CONFIG::physical_address_t> > memory_import;
	ServiceImport<LinuxOS> linux_os_import;
	ServiceImport<TrapReporting> trap_reporting_import;
	ServiceImport<CachePowerEstimator> il1_power_estimator_import;
	ServiceImport<CachePowerEstimator> dl1_power_estimator_import;
	ServiceImport<CachePowerEstimator> l2_power_estimator_import;
	ServiceImport<CachePowerEstimator> itlb_power_estimator_import;
	ServiceImport<CachePowerEstimator> dtlb_power_estimator_import;
	ServiceImport<PowerMode> il1_power_mode_import;
	ServiceImport<PowerMode> dl1_power_mode_import;
	ServiceImport<PowerMode> l2_power_mode_import;
	ServiceImport<PowerMode> dtlb_power_mode_import;
	ServiceImport<PowerMode> itlb_power_mode_import;

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
	void EnableDataCache();
	void DisableDataCache();
	void EnableInsnCache();
	void DisableInsnCache();
	void EnableL2Cache();
	void DisableL2Cache();
	void EnableAddressBroadcast();
	void DisableAddressBroadcast();
	inline bool IsFPUEnabled() const ALWAYS_INLINE { return GetMSR_FP() != 0; }
	inline bool IsFPUExceptionEnabled() const ALWAYS_INLINE { return GetMSR_FE0() || GetMSR_FE1(); }
	inline bool IsDataCacheEnabled() const ALWAYS_INLINE { return CONFIG::DL1_CONFIG::ENABLE && GetHID0_DCE(); }
	inline bool IsInsnCacheEnabled() const ALWAYS_INLINE { return CONFIG::IL1_CONFIG::ENABLE && GetHID0_ICE(); }
	inline bool IsL2CacheEnabled() const ALWAYS_INLINE { return CONFIG::L2_CONFIG::ENABLE && GetL2CR_L2E(); }
	inline bool IsAddressBroadcastEnabled() const ALWAYS_INLINE { return GetHID1_ABE() != 0; }

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

	//=====================================================================
	//=                      Exception processing methods                 =
	//=====================================================================
	
	inline void ProcessExceptions(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterSystemResetISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterPerformanceMonitorISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterSystemManagementISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterInstructionAddressBreakpointISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterAltivecUnavailableISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterSystemCallISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterMachineCheckISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterDecrementerISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterExternalISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterDataStorageISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterInstructionStorageISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterTLBMissISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterAlignmentISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterProgramISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterFloatingPointUnavailableISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterTraceISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	void EnterAltivecAssistISR(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);
	
	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report);

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
	
/*	static uint32_t Mask(uint32_t mb, uint32_t me)
	{
		return (mb > me) ? ~(((uint32_t) -1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1))) : (((uint32_t)-1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1)));
	}*/
	
	static uint32_t CRFieldMask(uint32_t crfD)
	{
		return (crfD != 7 ? 0xFFFFFFFFul >> ((crfD + 1) * 4) : 0) | (crfD != 0 ? (0xFFFFFFFFul << (8 - crfD) * 4) : 0);
	}

	//=====================================================================
	//=           processor version register set/get methods              =
	//=====================================================================
	
	inline void SetPVR(uint32_t value) ALWAYS_INLINE { pvr = value; }
	inline uint32_t GetPVR() const ALWAYS_INLINE { return pvr; }
	
	//=====================================================================
	//=                    user level registers set/get methods           =
	//=====================================================================
	
	inline uint32_t GetGPR(unsigned int n) const ALWAYS_INLINE { return gpr[n]; }
	inline void SetGPR(unsigned int n, uint32_t value) ALWAYS_INLINE { gpr[n] = value; }
	inline uint32_t GetLR() const ALWAYS_INLINE { return lr; }
	inline void SetLR(uint32_t value) ALWAYS_INLINE { lr = value; }
	inline uint32_t GetCTR() const ALWAYS_INLINE { return ctr; }
	inline void SetCTR(uint32_t value) ALWAYS_INLINE { ctr = value; }
	inline uint32_t GetVRSAVE() const ALWAYS_INLINE { return vrsave; }
	inline void SetVRSAVE(uint32_t value) ALWAYS_INLINE { vrsave = value; }	
	
	//=====================================================================
	//=                  instruction address set/get methods              =
	//=====================================================================
	
	inline uint32_t GetCIA() const ALWAYS_INLINE { return cia; }
	inline void SetCIA(uint32_t value) ALWAYS_INLINE { cia = value; }
	inline uint32_t GetNIA() const ALWAYS_INLINE { return nia; }
	inline void SetNIA(uint32_t value) ALWAYS_INLINE { nia = value; }
	inline void Branch(uint32_t value) ALWAYS_INLINE { nia = value; FlushSubsequentInstructions(); }

	//=====================================================================
	//=            external address register set/get methods              =
	//=====================================================================
	
	inline void SetEAR(uint32_t value) ALWAYS_INLINE { ear = value; }
	inline uint32_t GetEAR() const ALWAYS_INLINE { return ear; }
	
	//=====================================================================
	//=           special purpose registers set/get methods               =
	//=====================================================================

	bool GetSPR(unsigned int n, uint32_t& value);
	bool SetSPR(unsigned int n, uint32_t value);
	
	//=====================================================================
	//=          exception handling registers set/get methods             =
	//=====================================================================
	
	inline uint32_t GetSPRG(unsigned int n) const ALWAYS_INLINE { return sprg[n]; }
	inline void SetSPRG(unsigned int n, uint32_t value) ALWAYS_INLINE { sprg[n] = value; }
	inline uint32_t GetSRR0() const ALWAYS_INLINE { return srr0; }
	inline void SetSRR0(uint32_t value) ALWAYS_INLINE { srr0 = value; }
	inline uint32_t GetSRR1() const ALWAYS_INLINE { return srr1; }
	inline void SetSRR1(uint32_t value) ALWAYS_INLINE { srr1 = value; }
	inline uint32_t GetSRR1_WAY() const ALWAYS_INLINE { return (GetSRR1() >> 17) & 1; }
	inline void SetDAR(uint32_t value) ALWAYS_INLINE { dar = value; }
	inline uint32_t GetDAR() const ALWAYS_INLINE { return dar; }
	inline void SetDSISR(uint32_t value) ALWAYS_INLINE { dsisr = value; }
	uint32_t GetDSISR() const ALWAYS_INLINE { return dsisr; }
	
	//=====================================================================
	//=              DEC/TBL/TBU registers set/get methods                =
	//=====================================================================
	
	void SetDEC(uint32_t value) ALWAYS_INLINE
	{
		bool overflow = (!(dec & 0x80000000UL) && (value & 0x80000000UL));
		dec = value;

		if(overflow)
		{
			SetException(CONFIG::EXC_DECREMENTER);
		}
	}	
	inline uint32_t GetDEC() ALWAYS_INLINE { RunInternalTimers(); return dec; }
	inline uint64_t GetTB() ALWAYS_INLINE { RunInternalTimers(); return tb; }
	inline void SetTB(uint64_t value) ALWAYS_INLINE { tb = value; }
	inline uint32_t GetTBL() ALWAYS_INLINE { return (uint32_t) GetTB(); }
	inline void SetTBL(uint32_t value) ALWAYS_INLINE { SetTB((GetTB() & 0xffffffff00000000ULL) | ((uint64_t) value)); }
	inline uint32_t GetTBU() ALWAYS_INLINE { return (uint32_t)(GetTB() >> 32); }
	inline void SetTBU(uint32_t value) ALWAYS_INLINE { SetTB((GetTB() & 0x00000000ffffffffULL) | ((uint64_t) value << 32)); }
	inline void IncrementTB(uint64_t delta) ALWAYS_INLINE { tb = tb + delta; }
	inline void DecrementDEC(uint64_t delta) ALWAYS_INLINE
	{
		SetDEC(dec - delta);
	}
	inline void RunTimers(uint64_t delta) ALWAYS_INLINE
	{
		timer_cycle += delta;
		
		/* update the time base */
		IncrementTB(delta);

		/* decrement the decrementer each timer cycle */
		DecrementDEC(delta);
	}
	inline uint64_t GetMaxIdleTime() const
	{
		uint64_t delay_dec = 0xffffffffffffffffULL;
		if(GetMSR_EE())
		{
			uint32_t dec_value = dec;
			delay_dec = dec_value + 1;
			//std::cerr << "Time for DEC to overflow: " << delay_dec << std::endl;
		}
		
		return delay_dec;
	}
	inline uint64_t GetTimersDeadline() const
	{
		return dec + 1; // delay for a decrementer overflow (0 -> -1)
	}
	virtual void RunInternalTimers();
	
	//=====================================================================
	//=                        XER set/get methods                        =
	//=====================================================================

	inline void SetXER(uint32_t value) ALWAYS_INLINE { xer = value & CONFIG::XER_MASK; }
	inline uint32_t GetXER() const ALWAYS_INLINE { return xer; }

	// Deprecated methods
	inline uint32_t GetXER_SO() const ALWAYS_INLINE { return (xer & CONFIG::XER_SO_MASK) >> CONFIG::XER_SO_OFFSET; }
	inline void SetXER_SO() ALWAYS_INLINE { xer = xer | CONFIG::XER_SO_MASK; }
	inline void ResetXER_SO() ALWAYS_INLINE { xer = xer & ~CONFIG::XER_SO_MASK; }
	inline uint32_t GetXER_OV() const ALWAYS_INLINE { return (xer & CONFIG::XER_OV_MASK) >> CONFIG::XER_OV_OFFSET; }
	inline void SetXER_OV() ALWAYS_INLINE { xer = xer | CONFIG::XER_OV_MASK; }
	inline void ResetXER_OV() ALWAYS_INLINE { xer = xer & ~CONFIG::XER_OV_MASK; }
	inline uint32_t GetXER_CA() const ALWAYS_INLINE { return (xer & CONFIG::XER_CA_MASK) >> CONFIG::XER_CA_OFFSET; }
	inline void SetXER_CA() ALWAYS_INLINE { xer = xer | CONFIG::XER_CA_MASK; }
	inline void ResetXER_CA() ALWAYS_INLINE { xer = xer & ~CONFIG::XER_CA_MASK; }
	inline uint32_t GetXER_BYTE_COUNT() const ALWAYS_INLINE { return (xer & CONFIG::XER_BYTE_COUNT_MASK) >> CONFIG::XER_BYTE_COUNT_OFFSET; }

	
	//=====================================================================
	//=                         CR set/get methods                        =
	//=====================================================================

	inline uint32_t GetCR() const ALWAYS_INLINE { return cr; }
	inline void SetCR(uint32_t value) ALWAYS_INLINE { cr = value; }

	// Deprecated methods
	inline uint32_t GetCRF(unsigned int crf) const ALWAYS_INLINE { return cr >> ((7 - crf) * 4); }
	inline void SetCRF(unsigned int crfd, uint32_t value) ALWAYS_INLINE
	{
		uint32_t _c = value << ((7 - crfd) * 4) ; // positionning the crX field
		uint32_t _d = CRFieldMask(crfd) ;
		cr = cr & _d ;	//Deleting old sub-registrer value
		cr = cr | _c ;  //Writing the new one
	}
	
	//=====================================================================
	//=                        MSR set/get methods                        =
	//=====================================================================
	
	uint32_t GetMSR() const ALWAYS_INLINE { return msr; }
	void SetMSR(uint32_t value);
	inline uint32_t GetMSR_VEC() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_VEC_MASK) >> CONFIG::MSR_VEC_OFFSET; }
	inline uint32_t GetMSR_POW() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_POW_MASK) >> CONFIG::MSR_POW_OFFSET; }
	inline uint32_t GetMSR_ILE() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_ILE_MASK) >> CONFIG::MSR_ILE_OFFSET; }
	inline uint32_t GetMSR_EE() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_EE_MASK) >> CONFIG::MSR_EE_OFFSET; }
	inline uint32_t GetMSR_PR() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_PR_MASK) >> CONFIG::MSR_PR_OFFSET; }
	inline typename CONFIG::PrivilegeLevel GetPrivilegeLevel() const ALWAYS_INLINE { return GetMSR_PR() ? CONFIG::PR_USER : CONFIG::PR_SUPERVISOR; }
	inline uint32_t GetMSR_FP() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_FP_MASK) >> CONFIG::MSR_FP_OFFSET; }
	inline uint32_t GetMSR_ME() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_ME_MASK) >> CONFIG::MSR_ME_OFFSET; }
	inline uint32_t GetMSR_FE0() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_FE0_MASK) >> CONFIG::MSR_FE0_OFFSET; }
	inline uint32_t GetMSR_SE() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_SE_MASK) >> CONFIG::MSR_SE_OFFSET; }
	inline uint32_t GetMSR_BE() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_BE_MASK) >> CONFIG::MSR_BE_OFFSET; }
	inline uint32_t GetMSR_FE1() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_FE1_MASK) >> CONFIG::MSR_FE1_OFFSET; }
	inline uint32_t GetMSR_IP() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_IP_MASK) >> CONFIG::MSR_IP_OFFSET; }
	inline uint32_t GetMSR_IR() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_IR_MASK) >> CONFIG::MSR_IR_OFFSET; }
	inline uint32_t GetMSR_DR() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_DR_MASK) >> CONFIG::MSR_DR_OFFSET; }
	inline uint32_t GetMSR_PM() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_PM_MASK) >> CONFIG::MSR_PM_OFFSET; }
	inline uint32_t GetMSR_PMM() const ALWAYS_INLINE { return GetMSR_PM(); }
	inline uint32_t GetMSR_RI() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_RI_MASK) >> CONFIG::MSR_RI_OFFSET; }
	inline uint32_t GetMSR_LE() const ALWAYS_INLINE { return (GetMSR() & CONFIG::MSR_LE_MASK) >> CONFIG::MSR_LE_OFFSET; }
	inline void SetMSR_VEC() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_VEC_MASK); }
	inline void SetMSR_POW() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_POW_MASK); }
	inline void SetMSR_ILE() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_ILE_MASK); }
	inline void SetMSR_EE() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_EE_MASK); }
	inline void SetMSR_PR() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_PR_MASK); }
	inline void SetMSR_FP() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_FP_MASK); }
	inline void SetMSR_ME() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_ME_MASK); }
	inline void SetMSR_FE0() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_FE0_MASK); }
	inline void SetMSR_SE() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_SE_MASK); }
	inline void SetMSR_BE() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_BE_MASK); }
	inline void SetMSR_FE1() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_FE1_MASK); }
	inline void SetMSR_IP() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_IP_MASK); }
	inline void SetMSR_IR() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_IR_MASK); }
	inline void SetMSR_DR() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_DR_MASK); }
	inline void SetMSR_PM() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_PM_MASK); }
	inline void SetMSR_PMM() ALWAYS_INLINE { SetMSR_PM(); }
	inline void SetMSR_RI() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_RI_MASK); }
	inline void SetMSR_LE() ALWAYS_INLINE { SetMSR(GetMSR() | CONFIG::MSR_LE_MASK); }
	inline void ResetMSR_VEC() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_VEC_MASK); }
	inline void ResetMSR_POW() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_POW_MASK); }
	inline void ResetMSR_ILE() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_ILE_MASK); }
	inline void ResetMSR_EE() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_EE_MASK); }
	inline void ResetMSR_PR() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_PR_MASK); }
	inline void ResetMSR_FP() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_FP_MASK); }
	inline void ResetMSR_ME() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_ME_MASK); }
	inline void ResetMSR_FE0() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_FE0_MASK); }
	inline void ResetMSR_SE() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_SE_MASK); }
	inline void ResetMSR_BE() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_BE_MASK); }
	inline void ResetMSR_FE1() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_FE1_MASK); }
	inline void ResetMSR_IP() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_IP_MASK); }
	inline void ResetMSR_IR() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_IR_MASK); }
	inline void ResetMSR_DR() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_DR_MASK); }
	inline void ResetMSR_PM() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_PM_MASK); }
	inline void ResetMSR_PMM() ALWAYS_INLINE { ResetMSR_PM(); }
	inline void ResetMSR_RI() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_RI_MASK); }
	inline void ResetMSR_LE() ALWAYS_INLINE { SetMSR(GetMSR() & ~CONFIG::MSR_LE_MASK); }


	//=====================================================================
	//=                   FPU registers set/get methods                   =
	//=====================================================================

	inline uint32_t GetFPSCR() const ALWAYS_INLINE { return fpscr; }
	inline void SetFPSCR(uint32_t value) ALWAYS_INLINE { fpscr = value; }
	
	//=====================================================================
	//=                  MMU registers set/get methods                    =
	//=====================================================================

	inline uint32_t GetSR(unsigned int n) const ALWAYS_INLINE { return sr[n]; }
	inline void SetSR(unsigned int n, uint32_t value) ALWAYS_INLINE { sr[n] = value; }

	// Methods for reading SR registers bits
	inline uint32_t GetSR_T(unsigned int n) const ALWAYS_INLINE { return (sr[n] >> 31) & 1; }
	inline uint32_t GetSR_KS(unsigned int n) const ALWAYS_INLINE { return (sr[n] >> 30) & 1; }
	inline uint32_t GetSR_KP(unsigned int n) const ALWAYS_INLINE { return (sr[n] >> 29) & 1; }
	inline uint32_t GetSR_N(unsigned int n) const ALWAYS_INLINE { return (sr[n] >> 28) & 1; }
	inline uint32_t GetSR_VSID(unsigned int n) const ALWAYS_INLINE { return sr[n] & 0xffffffUL; }

	//=====================================================================
	//=                   Vector instructions handling                    =
	//=====================================================================
	
	//=====================================================================
	//=               Cache management instructions handling              =
	//=====================================================================
	
	bool Dcba(typename CONFIG::address_t addr);
	bool Dcbf(typename CONFIG::address_t addr);
	bool Dcbi(typename CONFIG::address_t addr);
	bool Dcbst(typename CONFIG::address_t addr);
	bool Dcbz(typename CONFIG::address_t addr);
	bool Icbi(typename CONFIG::address_t addr);
	bool Icbt(typename CONFIG::address_t addr);
	
	//=====================================================================
	//=  Translation look-aside buffers management instructions handling  =
	//=====================================================================
	
	bool Tlbia();
	bool Tlbie(typename CONFIG::address_t addr);
	bool Tlbld(typename CONFIG::address_t addr);
	bool Tlbli(typename CONFIG::address_t addr);

	//=====================================================================
	//=               Linked Load-Store instructions handling             =
	//=====================================================================
	
	bool Lwarx(unsigned int rd, typename CONFIG::address_t addr);
	bool Stwcx(unsigned int rs, typename CONFIG::address_t addr);

	//=====================================================================
	//=                Synchronization instructions handling              =
	//=====================================================================

	bool Isync();

	//=====================================================================
	//=           Return from interrupt instructions handling             =
	//=====================================================================
	
	bool Rfi();

	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
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
	//=               Exception check/acknowledgement methods             =
	//=====================================================================

	inline void SetException(unsigned int _exc) ALWAYS_INLINE { SetExceptionFlags(1 << _exc); }
	void SetExceptionAddress(typename CONFIG::address_t _exc_addr) { exc_addr = _exc_addr; }
	void SetExceptionVSID(uint32_t _exc_vsid) { exc_vsid = _exc_vsid; }
	void SetExceptionAPI(uint32_t _exc_api) { exc_api = _exc_api; }
	void SetExceptionWay(uint32_t _exc_way) { exc_way = _exc_way; }
	void SetExceptionKey(uint32_t _exc_key) { exc_key = _exc_key; }
	void SetExceptionMemoryAccessType(typename CONFIG::MemoryAccessType _exc_memory_access_type) { exc_memory_access_type = _exc_memory_access_type; }
	void SetExceptionMemoryType(typename CONFIG::MemoryType _exc_memory_type) { exc_memory_type = _exc_memory_type; }
	inline bool HasException() const { return exc_flags; }

	void ResetException(unsigned int _exc) ALWAYS_INLINE { ResetExceptionFlags(1 << _exc); }
	void SetExceptionFlags(uint32_t _exc_flags);
	void ResetExceptionFlags(uint32_t _exc_flags);
	void SetExceptionMask(uint32_t _exc_mask);
	void ResetExceptionMask(uint32_t _exc_mask);
	void UpdateExceptionMask();

protected:
	//=====================================================================
	//=          DEC/TBL/TBU bus-time based update methods                =
	//=====================================================================
	
	void OnBusCycle();
	
	//=====================================================================
	//=                        Debugging stuff                            =
	//=====================================================================

	Logger logger;
	bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
	bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
	bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported
  
	inline bool IsVerboseSetup() const ALWAYS_INLINE { return verbose_all || verbose_setup; }
	inline bool IsVerboseStep() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STEP_ENABLE && (verbose_all || verbose_step); }
	inline bool IsVerboseDTLB() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_DTLB_ENABLE && (verbose_all || verbose_dtlb); }
	inline bool IsVerboseITLB() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_ITLB_ENABLE && (verbose_all || verbose_itlb); }
	inline bool IsVerboseDL1() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_DL1_ENABLE && (verbose_all || verbose_dl1); }
	inline bool IsVerboseIL1() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_IL1_ENABLE && (verbose_all || verbose_il1); }
	inline bool IsVerboseL2() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_L2_ENABLE && (verbose_all || verbose_l2); }
	inline bool IsVerboseLoad() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_LOAD_ENABLE && (verbose_all || verbose_load); }
	inline bool IsVerboseStore() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STORE_ENABLE && (verbose_all || verbose_store); }
	inline bool IsVerboseReadMemory() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_READ_MEMORY_ENABLE && (verbose_all || verbose_read_memory); }
	inline bool IsVerboseWriteMemory() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_WRITE_MEMORY_ENABLE && (verbose_all || verbose_write_memory); }
	inline bool IsVerboseException() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception); }
	inline bool IsVerboseSetMSR() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_MSR_ENABLE && (verbose_all || verbose_set_msr); }
	inline bool IsVerboseSetHID0() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_HID0_ENABLE && (verbose_all || verbose_set_hid0); }
	inline bool IsVerboseSetHID1() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_HID1_ENABLE && (verbose_all || verbose_set_hid1); }
	inline bool IsVerboseSetHID2() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_HID2_ENABLE && (verbose_all || verbose_set_hid2); }
	inline bool IsVerboseSetL2CR() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_L2CR_ENABLE && (verbose_all || verbose_set_l2cr); }

	//=====================================================================
	//=                      Bus access methods                           =
	//=====================================================================

	virtual bool BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT, bool rwitm = false);
	virtual bool BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT);
	virtual bool BusZeroBlock(typename CONFIG::physical_address_t physical_addr);
	virtual bool BusFlushBlock(typename CONFIG::physical_address_t physical_addr);

	
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================
	
	uint64_t cpu_cycle_time; //!< CPU cycle time in ps
	uint64_t voltage;        //!< CPU voltage in mV
	uint64_t cpu_cycle;      //!< Number of cpu cycles
	uint64_t timer_cycle;    //!< Numver of timer cycles

private:

	//=====================================================================
	//=                     Cache/MMU handling methods                    =
	//=====================================================================

	void InvalidateITLBSet(uint32_t index);
	void InvalidateITLB();
	void InvalidateDTLBSet(uint32_t index);
	void InvalidateDTLB();
	void InvalidateDL1Set(uint32_t index);
	void InvalidateDL1();
	void InvalidateIL1Set(uint32_t index);
	void InvalidateIL1();
	void InvalidateL2Set(uint32_t index);
	void InvalidateL2();
	void ClearAccessDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void ClearAccessIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void ClearAccessL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	inline void LookupDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access) ALWAYS_INLINE;
	inline void LookupIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access) ALWAYS_INLINE;
	void LookupL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	bool EmuEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void EmuEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	bool EmuEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	void ChooseLineToEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void ChooseLineToEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void ChooseLineToEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	void UpdateReplacementPolicyDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void UpdateReplacementPolicyIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void UpdateReplacementPolicyL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	bool EmuFillDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg, bool rwitm);
	bool EmuFillIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg);
	bool EmuFillL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access, typename CONFIG::WIMG wimg, bool rwitm);
	bool EmuFetch(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	inline bool EmuFetch(typename CONFIG::address_t addr, uint32_t& insn) ALWAYS_INLINE;
	template <bool TRANSLATE_ADDR> bool EmuLoad(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	template <bool TRANSLATE_ADDR> bool EmuStore(typename CONFIG::address_t addr, const void *buffer, uint32_t size);
	template <class T> bool EmuLoad(T& value, typename CONFIG::address_t ea);
	template <class T> bool EmuStore(T value, typename CONFIG::address_t ea);
	inline typename CONFIG::address_t MungEffectiveAddress(typename CONFIG::address_t ea, unsigned int size) ALWAYS_INLINE { return (msr & CONFIG::MSR_LE_MASK) ? ea ^ (8 - size) : ea; }
	
	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	bool ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr);
	bool WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr);

	//=====================================================================
	//=                      L2CR set/get methods                         =
	//=====================================================================
	
	inline uint32_t GetL2CR() const ALWAYS_INLINE { return l2cr; }
	void SetL2CR(uint32_t value);
	inline uint32_t GetL2CR_L2E() const ALWAYS_INLINE { return (GetL2CR() & CONFIG::L2CR_L2E_MASK) >> CONFIG::L2CR_L2E_OFFSET; }
	inline uint32_t GetL2CR_L2I() const ALWAYS_INLINE { return (GetL2CR() & CONFIG::L2CR_L2I_MASK) >> CONFIG::L2CR_L2I_OFFSET; }
	inline void SetL2CR_L2E() ALWAYS_INLINE { SetL2CR(GetL2CR() | CONFIG::L2CR_L2E_MASK); }
	inline void SetL2CR_L2I() ALWAYS_INLINE { SetL2CR(GetL2CR() | CONFIG::L2CR_L2I_MASK); }
	inline void ResetL2CR_L2E() ALWAYS_INLINE { SetL2CR(GetL2CR() & ~CONFIG::L2CR_L2E_MASK); }
	inline void ResetL2CR_L2I() ALWAYS_INLINE { SetL2CR(GetL2CR() & ~CONFIG::L2CR_L2I_MASK); }

	//=====================================================================
	//=                      IABR set/get methods                         =
	//=====================================================================
	inline uint32_t GetIABR() const ALWAYS_INLINE { return iabr; }
	inline void SetIABR(uint32_t value) ALWAYS_INLINE { iabr = value; }
	inline uint32_t GetIABR_ADDR() const ALWAYS_INLINE { return (GetIABR() & CONFIG::IABR_ADDR_MASK) >> CONFIG::IABR_ADDR_OFFSET; }
	inline uint32_t GetIABR_BE() const ALWAYS_INLINE { return (GetIABR() & CONFIG::IABR_BE_MASK) >> CONFIG::IABR_BE_OFFSET; }
	inline uint32_t GetIABR_TE() const ALWAYS_INLINE { return (GetIABR() & CONFIG::IABR_TE_MASK) >> CONFIG::IABR_TE_OFFSET; }

	inline uint32_t GetLDSTDB() const ALWAYS_INLINE { return ldstdb; }
	inline void SetLDSTDB(uint32_t value) ALWAYS_INLINE { ldstdb = value; }
	
	inline uint32_t GetICTC() const ALWAYS_INLINE { return ictc; }
	inline void SetICTC(uint32_t value) ALWAYS_INLINE { ictc = value; }

	//=====================================================================
	//=                       DABR set/get methods                        =
	//=====================================================================
	
	inline void SetDABR(uint32_t value) { dabr = value; }
	inline uint32_t GetDABR() const ALWAYS_INLINE { return dabr; }
	inline uint32_t GetDABR_DAB() const ALWAYS_INLINE { return (dabr >> 3) & 0x1fffffffUL; }
	inline uint32_t GetDABR_BT() const ALWAYS_INLINE { return (dabr >> 2) & 1; }inline uint32_t GetDABR_DW() { return (dabr >> 1) & 1; }
	inline uint32_t GetDABR_DR() const ALWAYS_INLINE { return dabr & 1; }

	//=====================================================================
	//=         performance monitor registers set/get methods             =
	//=====================================================================
	
	inline uint32_t GetPMC1() const ALWAYS_INLINE { return pmc1; }
	inline void SetPMC1(uint32_t value) { pmc1 = value; }
	inline uint32_t GetPMC2() const ALWAYS_INLINE { return pmc2; }
	inline void SetPMC2(uint32_t value) { pmc2 = value; }
	inline uint32_t GetPMC3() const ALWAYS_INLINE { return pmc3; }
	inline void SetPMC3(uint32_t value) { pmc3 = value; }
	inline uint32_t GetPMC4() const ALWAYS_INLINE { return pmc4; }
	inline void SetPMC4(uint32_t value) { pmc4 = value; }
	inline uint32_t GetPMC5() const ALWAYS_INLINE { return pmc5; }
	inline void SetPMC5(uint32_t value) { pmc5 = value; }
	inline uint32_t GetPMC6() const ALWAYS_INLINE { return pmc6; }
	inline void SetPMC6(uint32_t value) { pmc6 = value; }
	inline uint32_t GetSIA() const ALWAYS_INLINE { return sia; }
	inline void SetSIA(uint32_t value) { sia = value; }
	inline uint32_t GetSDA() const ALWAYS_INLINE { return sda; }
	inline void SetSDA(uint32_t value) { sda = value; }
	inline uint32_t GetMMCR0() const ALWAYS_INLINE { return mmcr0; }
	inline void SetMMCR0(uint32_t value) { mmcr0 = value; }
	inline uint32_t GetMMCR1() const ALWAYS_INLINE { return mmcr1; }
	inline void SetMMCR1(uint32_t value) { mmcr1 = value; }
	inline uint32_t GetMMCR2() const ALWAYS_INLINE { return mmcr2; }
	inline void SetMMCR2(uint32_t value) { mmcr2 = value; }


	//=====================================================================
	//=                  HID0 register set/get methods                    =
	//=====================================================================

	inline uint32_t GetHID0() const ALWAYS_INLINE { return hid0; }
	void SetHID0(uint32_t value);
	inline uint32_t GetHID0_TBEN() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_TBEN_MASK) >> CONFIG::HID0_TBEN_OFFSET; }
	inline uint32_t GetHID0_STEN() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_STEN_MASK) >> CONFIG::HID0_STEN_OFFSET; }
	inline uint32_t GetHID0_HIGH_BAT_EN() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_HIGH_BAT_EN_MASK) >> CONFIG::HID0_HIGH_BAT_EN_OFFSET; }
	inline uint32_t GetHID0_NAP() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_NAP_MASK) >> CONFIG::HID0_NAP_OFFSET; }
	inline uint32_t GetHID0_SLEEP() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_SLEEP_MASK) >> CONFIG::HID0_SLEEP_OFFSET; }
	inline uint32_t GetHID0_DPM() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_DPM_MASK) >> CONFIG::HID0_DPM_OFFSET; }
	inline uint32_t GetHID0_BHTCLR() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_BHTCLR_MASK) >> CONFIG::HID0_BHTCLR_OFFSET; }
	inline uint32_t GetHID0_XAEN() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_XAEN_MASK) >> CONFIG::HID0_XAEN_OFFSET; }
	inline uint32_t GetHID0_NHR() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_NHR_MASK) >> CONFIG::HID0_NHR_OFFSET; }
	inline uint32_t GetHID0_ICE() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_ICE_MASK) >> CONFIG::HID0_ICE_OFFSET; }
	inline uint32_t GetHID0_DCE() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_DCE_MASK) >> CONFIG::HID0_DCE_OFFSET; }
	inline uint32_t GetHID0_ILOCK() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_ILOCK_MASK) >> CONFIG::HID0_ILOCK_OFFSET; }
	inline uint32_t GetHID0_DLOCK() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_DLOCK_MASK) >> CONFIG::HID0_DLOCK_OFFSET; }
	inline uint32_t GetHID0_ICFI() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_ICFI_MASK) >> CONFIG::HID0_ICFI_OFFSET; }
	inline uint32_t GetHID0_DCFI() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_DCFI_MASK) >> CONFIG::HID0_DCFI_OFFSET; }
	inline uint32_t GetHID0_SPD() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_SPD_MASK) >> CONFIG::HID0_SPD_OFFSET; }
	inline uint32_t GetHID0_XBSEN() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_XBSEN_MASK) >> CONFIG::HID0_XBSEN_OFFSET; }
	inline uint32_t GetHID0_SGE() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_SGE_MASK) >> CONFIG::HID0_SGE_OFFSET; }
	inline uint32_t GetHID0_BTIC() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_BTIC_MASK) >> CONFIG::HID0_BTIC_OFFSET; }
	inline uint32_t GetHID0_LRSTK() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_LRSTK_MASK) >> CONFIG::HID0_LRSTK_OFFSET; }
	inline uint32_t GetHID0_FOLD() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_FOLD_MASK) >> CONFIG::HID0_FOLD_OFFSET; }
	inline uint32_t GetHID0_BHT() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_BHT_MASK) >> CONFIG::HID0_BHT_OFFSET; }
	inline uint32_t GetHID0_NOPDST() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_NOPDST_MASK) >> CONFIG::HID0_NOPDST_OFFSET; }
	inline uint32_t GetHID0_NOPTI() const ALWAYS_INLINE { return (GetHID0() & CONFIG::HID0_NOPTI_MASK) >> CONFIG::HID0_NOPTI_OFFSET; }
	inline void SetHID0_DCE() ALWAYS_INLINE { SetHID0(GetHID0() | CONFIG::HID0_DCE_MASK); }
	inline void ResetHID0_DCE() ALWAYS_INLINE { SetHID0(GetHID0() & ~CONFIG::HID0_DCE_MASK); }
	inline void SetHID0_ICE() ALWAYS_INLINE { SetHID0(GetHID0() | CONFIG::HID0_ICE_MASK); }
	inline void ResetHID0_ICE() ALWAYS_INLINE { SetHID0(GetHID0() & ~CONFIG::HID0_ICE_MASK); }
	inline void ResetHID0_ICFI() ALWAYS_INLINE { SetHID0(GetHID0() & ~CONFIG::HID0_ICFI_MASK); }
	inline void ResetHID0_DCFI() ALWAYS_INLINE { SetHID0(GetHID0() & ~CONFIG::HID0_DCFI_MASK); }
	inline void SetHID0_NHR() ALWAYS_INLINE { SetHID0(GetHID0() | CONFIG::HID0_NHR_MASK); }
	inline void ResetHID0_NHR() ALWAYS_INLINE { SetHID0(GetHID0() & ~CONFIG::HID0_NHR_MASK); }

	//=====================================================================
	//=                  HID1 register set/get methods                    =
	//=====================================================================
	
	inline uint32_t GetHID1() const ALWAYS_INLINE { return hid1; }
	void SetHID1(uint32_t value);
	inline uint32_t GetHID1_EMCP() const ALWAYS_INLINE { return (GetHID1() & CONFIG::HID1_EMCP_MASK) >> CONFIG::HID1_EMCP_OFFSET; }
	inline uint32_t GetHID1_ABE() const ALWAYS_INLINE { return (GetHID1() & CONFIG::HID1_ABE_MASK) >> CONFIG::HID1_ABE_OFFSET; }
	inline void SetHID1_ABE() ALWAYS_INLINE { SetHID1(GetHID1() | CONFIG::HID1_ABE_MASK); }
	inline void ResetHID1_ABE() ALWAYS_INLINE { SetHID1(GetHID1() & ~CONFIG::HID1_ABE_MASK); }
	
	inline void SetPTEHI(uint32_t value) ALWAYS_INLINE { ptehi = value; }
	inline uint32_t GetPTEHI() const ALWAYS_INLINE { return ptehi; }
	inline void SetPTELO(uint32_t value) ALWAYS_INLINE { ptelo = value; }
	inline uint32_t GetPTELO() const ALWAYS_INLINE{ return ptelo; }

	//=====================================================================
	//=                  MMU registers set/get methods                    =
	//=====================================================================
	
	inline uint32_t GetDBATL(unsigned int n) const ALWAYS_INLINE { return dbat[n].l; }
	inline void SetDBATL(unsigned int n, uint32_t value) ALWAYS_INLINE{ dbat[n].l = value; ReconfigureFastBATLookup(); }
	
	inline uint32_t GetDBATU(unsigned int n) const ALWAYS_INLINE { return dbat[n].u; }
	inline void SetDBATU(unsigned int n, uint32_t value) ALWAYS_INLINE { dbat[n].u = value; ReconfigureFastBATLookup(); }
	
	inline uint32_t GetIBATL(unsigned int n) const ALWAYS_INLINE { return ibat[n].l; }
	inline void SetIBATL(unsigned int n, uint32_t value) ALWAYS_INLINE { ibat[n].l = value; ReconfigureFastBATLookup(); }
	
	inline uint32_t GetIBATU(unsigned int n) const ALWAYS_INLINE { return ibat[n].u; }
	inline void SetIBATU(unsigned int n, uint32_t value) ALWAYS_INLINE { ibat[n].u = value; ReconfigureFastBATLookup(); }

	inline uint32_t GetBATU_BEPI(uint32_t value) const ALWAYS_INLINE { return (value >> 17) & 0x7fff; }
	inline uint32_t GetBATU_BL(uint32_t value) const ALWAYS_INLINE { return (value >> 2) & 0x7ff; }
	inline uint32_t GetBATU_VS(uint32_t value) const ALWAYS_INLINE { return (value >> 1) & 1; }
	inline uint32_t GetBATU_VP(uint32_t value) const ALWAYS_INLINE { return value & 1; }
	inline uint32_t GetBATL_BRPN(uint32_t value) const ALWAYS_INLINE { return (value >> 17) & 0x7fff; }
	inline uint32_t GetBATL_WIMG(uint32_t value) const ALWAYS_INLINE { return (value >> 3) & 0xf; }
	inline uint32_t GetBATL_PP(uint32_t value) const ALWAYS_INLINE { return value & 3; }

	// Methods for reading DBAT register bits
	inline uint32_t GetDBATU_BEPI(unsigned int n) const ALWAYS_INLINE { return (GetDBATU(n) >> 17) & 0x7fff; }
	inline uint32_t GetDBATU_BL(unsigned int n) const ALWAYS_INLINE { return (GetDBATU(n) >> 2) & 0x7ff; }
	inline uint32_t GetDBATU_VS(unsigned int n) const ALWAYS_INLINE { return (GetDBATU(n) >> 1) & 1; }
	inline uint32_t GetDBATU_VP(unsigned int n) const ALWAYS_INLINE { return GetDBATU(n) & 1; }
	inline uint32_t GetDBATL_BRPN(unsigned int n) const ALWAYS_INLINE { return (GetDBATL(n) >> 17) & 0x7fff; }
	inline uint32_t GetDBATL_WIMG(unsigned int n) const ALWAYS_INLINE { return (GetDBATL(n) >> 3) & 0xf; }
	inline uint32_t GetDBATL_PP(unsigned int n) const ALWAYS_INLINE { return GetDBATL(n) & 3; }

	// Methods for reading IBAT register bits
	inline uint32_t GetIBATU_BEPI(unsigned int n) const ALWAYS_INLINE { return (GetIBATU(n) >> 17) & 0x7fff; }
	inline uint32_t GetIBATU_BL(unsigned int n) const ALWAYS_INLINE { return (GetIBATU(n) >> 2) & 0x7ff; }
	inline uint32_t GetIBATU_VS(unsigned int n) const ALWAYS_INLINE { return (GetIBATU(n) >> 1) & 1; }
	inline uint32_t GetIBATU_VP(unsigned int n) const ALWAYS_INLINE { return GetIBATU(n) & 1; }
	inline uint32_t GetIBATL_BRPN(unsigned int n) const ALWAYS_INLINE { return (GetIBATL(n) >> 17) & 0x7fff; }
	inline uint32_t GetIBATL_WIMG(unsigned int n) const ALWAYS_INLINE { return (GetIBATL(n) >> 3) & 0xf; }
	inline uint32_t GetIBATL_PP(unsigned int n) const ALWAYS_INLINE { return GetIBATL(n) & 3; }

	inline uint32_t GetSDR1() const ALWAYS_INLINE { return sdr1; }
	inline void SetSDR1(uint32_t value) ALWAYS_INLINE { sdr1 = value; }

	// Methods for reading SDR1 bits
	inline uint32_t GetSDR1_HTABORG() const ALWAYS_INLINE { return (sdr1 >> 16) & 0xffff; }
	inline uint32_t GetSDR1_HTABMASK() const ALWAYS_INLINE { return sdr1 & 0x1ff; }

	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
	bool verbose_all;
	bool verbose_setup;
	bool verbose_step;
	bool verbose_dtlb;
	bool verbose_itlb;
	bool verbose_dl1;
	bool verbose_il1;
	bool verbose_l2;
	bool verbose_load;
	bool verbose_store;
	bool verbose_read_memory;
	bool verbose_write_memory;
	bool verbose_exception;
	bool verbose_set_msr;
	bool verbose_set_hid0;
	bool verbose_set_hid1;
	bool verbose_set_hid2;
	bool verbose_set_l2cr;
	bool enable_linux_printk_snooping;
	bool enable_linux_syscall_snooping;
	typename CONFIG::address_t halt_on_addr;
	std::string halt_on;
	uint64_t trap_on_instruction_counter;
	uint64_t max_inst;                                         //!< Maximum number of instructions to execute
	uint64_t num_interrupts;
	uint64_t num_system_reset_interrupts;
	uint64_t num_machine_check_interrupts;
	uint64_t num_data_storage_interrupts;
	uint64_t num_instruction_storage_interrupts;
	uint64_t num_external_interrupts;
	uint64_t num_alignment_interrupts;
	uint64_t num_program_interrupts;
	uint64_t num_floating_point_unavailable_interrupts;
	uint64_t num_decrementer_interrupts;
	uint64_t num_system_call_interrupts;
	uint64_t num_trace_interrupts;
	uint64_t num_performance_monitor_interrupts;
	uint64_t num_instruction_address_breakpoint_interrupts;
	uint64_t num_system_management_interrupts;
	uint64_t num_itlb_miss_interrupts;
	uint64_t num_dtlb_miss_on_load_interrupts;
	uint64_t num_dtlb_miss_on_store_interrupts;
	uint64_t num_altivec_unavailable_interrupts;
	uint64_t num_altivec_assist_interrupts;

	map<string, unisim::service::interfaces::Register *> registers_registry;       //!< Every CPU register interfaces excluding MMU/FPU registers
	std::vector<unisim::kernel::VariableBase *> registers_registry2;       //!< Every CPU register
	uint64_t instruction_counter;                              //!< Number of executed instructions
	bool fp32_estimate_inv_warning;
	bool fp64_estimate_inv_sqrt_warning;

	inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	inline void MonitorLoad(typename CONFIG::address_t ea, uint32_t size);
	inline void MonitorStore(typename CONFIG::address_t ea, uint32_t size);
	bool FloatingPointSelfTest();

	//=====================================================================
	//=                     MMU Address translation                       =
	//=====================================================================

	template <bool DEBUG> inline bool LookupBAT(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	void ReconfigureFastBATLookup();
	inline void LookupITLB(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	inline void LookupDTLB(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	void UpdateITLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access);
	void UpdateDTLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access);
	void ChooseEntryToEvictITLB(MMUAccess<CONFIG>& mmu_access);
	void ChooseEntryToEvictDTLB(MMUAccess<CONFIG>& mmu_access);
	template <bool DEBUG> inline bool AccessTLB(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	template <bool DEBUG> bool EmuHardwarePageTableSearch(MMUAccess<CONFIG>& mmu_access);
	template <bool DEBUG> inline bool EmuTranslateAddress(MMUAccess<CONFIG>& mmu_access) ALWAYS_INLINE;
	void LoadITLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo);
	void LoadDTLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo);
	void DumpPageTable(ostream& os);

	//=====================================================================
	//=                    registers set/get methods                      =
	//=====================================================================

	inline uint32_t GetBAMR() ALWAYS_INLINE { return bamr; }
	inline void SetBAMR(uint32_t value) ALWAYS_INLINE { bamr = value; }

	inline uint32_t GetICTRL() ALWAYS_INLINE { return ictrl; }
	inline void SetICTRL(uint32_t value) ALWAYS_INLINE { ictrl = (ictrl & ~CONFIG::ICTRL_MASK) | (value & CONFIG::ICTRL_MASK); }
	inline uint32_t GetICTRL_CIRQ() ALWAYS_INLINE { return (GetICTRL() & CONFIG::ICTRL_CIRQ_MASK) >> CONFIG::ICTRL_CIRQ_OFFSET; }
	inline uint32_t GetICTRL_EIEC() ALWAYS_INLINE { return (GetICTRL() & CONFIG::ICTRL_EIEC_MASK) >> CONFIG::ICTRL_EIEC_OFFSET; }
	inline uint32_t GetICTRL_EDCE() ALWAYS_INLINE { return (GetICTRL() & CONFIG::ICTRL_EDCE_MASK) >> CONFIG::ICTRL_EDCE_OFFSET; }
	inline uint32_t GetICTRL_EICP() ALWAYS_INLINE { return (GetICTRL() & CONFIG::ICTRL_EICP_MASK) >> CONFIG::ICTRL_EICP_OFFSET; }
	inline uint32_t GetICTRL_ICWL() ALWAYS_INLINE { return (GetICTRL() & CONFIG::ICTRL_ICWL_MASK) >> CONFIG::ICTRL_ICWL_OFFSET; }
	inline void SetICTRL_CIRQ() ALWAYS_INLINE { SetICTRL(GetICTRL() | CONFIG::ICTRL_CIRQ_MASK); }
	inline void ResetICTRL_CIRQ() ALWAYS_INLINE { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_CIRQ_MASK); }
	inline void SetICTRL_EIEC() ALWAYS_INLINE { SetICTRL(GetICTRL() | CONFIG::ICTRL_EIEC_MASK); }
	inline void ResetICTRL_EIEC() ALWAYS_INLINE { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_EIEC_MASK); }
	inline void SetICTRL_EDCE() ALWAYS_INLINE { SetICTRL(GetICTRL() | CONFIG::ICTRL_EDCE_MASK); }
	inline void ResetICTRL_EDCE() ALWAYS_INLINE { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_EDCE_MASK); }
	inline void SetICTRL_EICP() ALWAYS_INLINE { SetICTRL(GetICTRL() | CONFIG::ICTRL_EICP_MASK); }
	inline void ResetICTRL_EICP() ALWAYS_INLINE { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_EICP_MASK); }
	inline void SetICTRL_ICWL() ALWAYS_INLINE { SetICTRL(GetICTRL() | CONFIG::ICTRL_ICWL_MASK); }
	inline void ResetICTRL_ICWL() ALWAYS_INLINE { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_ICWL_MASK); }

	inline uint32_t GetLDSTCR() ALWAYS_INLINE { return ldstcr; }
	inline void SetLDSTCR(uint32_t value) ALWAYS_INLINE { ldstcr = value; }
	inline uint32_t GetMSSCR0() ALWAYS_INLINE { return msscr0; }
	inline void SetMSSCR0(uint32_t value) ALWAYS_INLINE { msscr0 = value; }
	inline uint32_t GetMSSSR0() ALWAYS_INLINE { return msssr0; }
	inline void SetMSSSR0(uint32_t value) ALWAYS_INLINE { msssr0 = value; }
	inline uint32_t GetPIR() ALWAYS_INLINE { return pir; }
	inline void SetPIR(uint32_t value) ALWAYS_INLINE { pir = value; }
	inline uint32_t GetTLBMISS() ALWAYS_INLINE { return tlbmiss; }
	inline void SetTLBMISS(uint32_t value) ALWAYS_INLINE { tlbmiss = value; }

	//=====================================================================
	//=                        PowerPC registers                          =
	//=====================================================================

	uint32_t cia;            //!< Current instruction address (all)
	uint32_t nia;            //!< Next instruction address (all)
	
	// UISA
	uint32_t cr;            //!< condition register (all)
	uint32_t ctr;           //!< count register (all)
	SoftDouble fpr[CONFIG::NUM_FPRS];     //!< floating point registers (C++ objects implementing IEEE 754 floating point numbers) (604e, 7xx, 7xxx)
	uint32_t fpscr;         //!< floating point status and control register (604e, 7xx, 7xxx)
	uint32_t gpr[CONFIG::NUM_GPRS];       //!< general purpose registers (all)
	uint32_t lr;            //!< link register (all)
	vr_t vr[CONFIG::NUM_VRS]; //!< vector registers (7xxx)
	uint32_t vrsave;        //!< vector save/restore register (7xxx)
	uint32_t vscr;          //!< vector status and control register (7xxx)
	uint32_t xer;           //!< XER register (all)

	// VEA
	uint64_t tb; // Time base register (tbu || tbl) (603e, 604e, 7xx, 7xxx)

	// OEA
	uint32_t bamr;          //!< breakpoint address mask register (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t dabr;          //!< data address breakpoint register (all)
	uint32_t dar;           //!< data address register (all)
	uint32_t dec;           //!< decrementer (all)
	uint32_t dsisr;         //!< DSISR register (all)
	uint32_t ear;           //!< external access register (all)
	uint32_t hid0;          //!< hardware implementation dependent register 0 (all)
	uint32_t hid1;          //!< hardware implementation dependent register 1 (all)
	uint32_t iabr;          //!< instruction address breakpoint register (603e, 604e, 7xx, 7xxx)
	uint32_t ictc;          //!< instruction cache throttling control register (7xx, 7xxx)
	uint32_t ictrl;         //!< instruction cache and interrupt control register (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t l2cr;          //!< L2 Control Register (7xx, 7xxx)
	uint32_t ldstcr;        //!< load/store control register (7441, 7445, 7447, 7447A, 7448, 7451, 7455, 7457)
	uint32_t ldstdb;		//!< load/store debug register (factory-use only register)
	uint32_t mmcr0;         //!< monitor control (604e, 7xx, 7xxx)
	uint32_t mmcr1;         //!< monitor control (604e, 7xx, 7xxx)
	uint32_t mmcr2;         //!< monitor control (744x, 745x)
	uint32_t msr;           //!< Machine status register (all)
	uint32_t msscr0;        //!< Memory subsystem control register (7441, 7445, 7447, 7447A, 7448, 7451, 7455, 7457)
	uint32_t msssr0;        //!< Memory subsystem status register (7441, 7445, 7447, 7447A, 7448, 7451, 7455, 7457)
	uint32_t pir;           //!< Processor identification register (604e, 7xxx)
	uint32_t pmc1;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc2;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc3;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc4;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc5;          //!< performance counter (744x, 745x)
	uint32_t pmc6;          //!< performance counter (744x, 745x)
	uint32_t ptehi;         //!< PTEHI (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t ptelo;         //!< PTELO (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t pvr;           //!< processor version register (all)
	uint32_t sda;           //!< Sampled data address (604e, 7xx, 7xxx)
	uint32_t sia;           //!< Sampled instruction address (604e, 7xx, 7xxx)
	uint32_t sprg[CONFIG::NUM_SPRGS];   //!< SPRGs registers (all)
	uint32_t srr0;          //!< Save and restore register 0 (all)
	uint32_t srr1;          //!< Save and restore register 1 (all)
	uint32_t tlbmiss;       //!< TLBMISS register (7441, 7445, 7447, 7447A, 7448, 7451, 7455)

	//=====================================================================
	//=                            MMU registers                          =
	//=====================================================================
	
	typedef struct BAT
	{
		BAT *prev;
		BAT *next;
		uint32_t u;
		uint32_t l;
	} BAT;
	
	BAT dbat[CONFIG::NUM_BATS];  /*< data block address translation registers */
	BAT *mru_dbat;
	BAT ibat[CONFIG::NUM_BATS];  /*< instruction block address translation registers */
	BAT *mru_ibat;
	uint32_t sr[CONFIG::NUM_SRS];	/*< segment registers */
	uint32_t sdr1;		/*< SDR1 register */

	uint32_t last_ibat_miss_bepi;
	typename CONFIG::PrivilegeLevel last_ibat_miss_privilege_level;
	uint32_t last_dbat_miss_bepi;
	typename CONFIG::PrivilegeLevel last_dbat_miss_privilege_level;
	uint64_t num_ibat_accesses;
	uint64_t num_ibat_misses;
	uint64_t num_dbat_accesses;
	uint64_t num_dbat_misses;

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
	//=                       L2 Unified Cache                            =
	//=====================================================================
		
	unisim::component::cxx::cache::Cache<class CONFIG::L2_CONFIG> l2;
	uint64_t num_l2_accesses;
	uint64_t num_l2_misses;
	
	//=====================================================================
	//=                  MMU Translation Look-aside buffers               =
	//=====================================================================

	/* Data Translation look-aside buffer */
	TLB<class CONFIG::DTLB_CONFIG> dtlb;
	uint64_t num_dtlb_accesses;
	uint64_t num_dtlb_misses;

	/* Instruction Translation look-aside buffer */
	TLB<class CONFIG::ITLB_CONFIG> itlb;
	uint64_t num_itlb_accesses;
	uint64_t num_itlb_misses;

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
	
	uint32_t exc_flags;                                       //!< exception flags (bits are ordered according to exception priority)
	uint32_t exc_mask;                                        //!< exception mask  (bits are ordered according to exception priority)
	typename CONFIG::address_t exc_addr;                      //!< effective address, if any, that caused the exception
	uint32_t exc_vsid;                                        //!< VSID, if any, that caused the exception
	uint32_t exc_api;                                         //!< API, if any, that caused the exception
	uint32_t exc_way;                                         //!< TLB way, if any, that caused the exception
	uint32_t exc_key;                                         //!< hash key, if any, that caused the exception
	typename CONFIG::MemoryAccessType exc_memory_access_type; //!< memory access type, if any, that caused the exception
	typename CONFIG::MemoryType exc_memory_type;              //!< memory type, if any, that caused the exception
	void (unisim::component::cxx::processor::powerpc::mpc7447a::CPU<CONFIG>::*enter_isr_table[CONFIG::NUM_EXCEPTIONS])(unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation);

	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_cpu_cycle_time;             //!< linked to member cpu_cycle_time
	Parameter<uint64_t> param_voltage;                    //!< linked to member voltage
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_setup;
	Parameter<bool> param_verbose_step;
	Parameter<bool> param_verbose_dtlb;
	Parameter<bool> param_verbose_itlb;
	Parameter<bool> param_verbose_dl1;
	Parameter<bool> param_verbose_il1;
	Parameter<bool> param_verbose_l2;
	Parameter<bool> param_verbose_load;
	Parameter<bool> param_verbose_store;
	Parameter<bool> param_verbose_read_memory;
	Parameter<bool> param_verbose_write_memory;
	Parameter<bool> param_verbose_exception;
	Parameter<bool> param_verbose_set_msr;
	Parameter<bool> param_verbose_set_hid0;
	Parameter<bool> param_verbose_set_hid1;
	Parameter<bool> param_verbose_set_hid2;
	Parameter<bool> param_verbose_set_l2cr;
	Parameter<bool> param_enable_linux_printk_snooping;
	Parameter<bool> param_enable_linux_syscall_snooping;
	Parameter<uint64_t> param_trap_on_instruction_counter;
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
	Statistic<uint64_t> stat_num_l2_accesses;
	Statistic<uint64_t> stat_num_l2_misses;
	Formula<double> formula_l2_miss_rate;
	Statistic<uint64_t> stat_num_ibat_accesses;
	Statistic<uint64_t> stat_num_ibat_misses;
	Formula<double> formula_ibat_miss_rate;
	Statistic<uint64_t> stat_num_dbat_accesses;
	Statistic<uint64_t> stat_num_dbat_misses;
	Formula<double> formula_dbat_miss_rate;
	Statistic<uint64_t> stat_num_itlb_accesses;
	Statistic<uint64_t> stat_num_itlb_misses;
	Formula<double> formula_itlb_miss_rate;
	Statistic<uint64_t> stat_num_dtlb_accesses;
	Statistic<uint64_t> stat_num_dtlb_misses;
	Formula<double> formula_dtlb_miss_rate;
	Statistic<uint64_t> stat_num_interrupts;
	Statistic<uint64_t> stat_num_system_reset_interrupts;
	Statistic<uint64_t> stat_num_machine_check_interrupts;
	Statistic<uint64_t> stat_num_data_storage_interrupts;
	Statistic<uint64_t> stat_num_instruction_storage_interrupts;
	Statistic<uint64_t> stat_num_external_interrupts;
	Statistic<uint64_t> stat_num_alignment_interrupts;
	Statistic<uint64_t> stat_num_program_interrupts;
	Statistic<uint64_t> stat_num_floating_point_unavailable_interrupts;
	Statistic<uint64_t> stat_num_decrementer_interrupts;
	Statistic<uint64_t> stat_num_system_call_interrupts;
	Statistic<uint64_t> stat_num_trace_interrupts;
	Statistic<uint64_t> stat_num_performance_monitor_interrupts;
	Statistic<uint64_t> stat_num_instruction_address_breakpoint_interrupts;
	Statistic<uint64_t> stat_num_system_management_interrupts;
	Statistic<uint64_t> stat_num_itlb_miss_interrupts;
	Statistic<uint64_t> stat_num_dtlb_miss_on_load_interrupts;
	Statistic<uint64_t> stat_num_dtlb_miss_on_store_interrupts;
	Statistic<uint64_t> stat_num_altivec_unavailable_interrupts;
	Statistic<uint64_t> stat_num_altivec_assist_interrupts;
};

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
