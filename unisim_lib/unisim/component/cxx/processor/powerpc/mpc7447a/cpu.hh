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
#include <unisim/component/cxx/processor/powerpc/floating.hh>
#include <unisim/component/cxx/processor/powerpc/mpc7447a/isa.hh>
#include <unisim/component/cxx/processor/powerpc/mpc7447a/config.hh>
#include <unisim/component/cxx/processor/powerpc/mpc7447a/exception.hh>
#include <unisim/component/cxx/cache/cache.hh>
#include <unisim/component/cxx/tlb/tlb.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
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

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

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


typedef enum {
	LSM_DEFAULT = 0,
	LSM_FLOATING_POINT = 1,
	LSM_BYTE_REVERSE = 2,
	LSM_SIGN_EXTEND = 4,
	LSM_MSB_FIRST = 8,
	LSM_RAW_FLOAT = 16
} LoadStoreMode;

typedef union
{
	uint8_t b[16];
	uint16_t h[8];
	uint32_t w[4];
} vr_t;

template <class CONFIG>
class CPU;

template <class CONFIG>
class Instruction;

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

template <class CONFIG>
class FetchAccess;

template <class CONFIG>
ostream& operator << (ostream& os, const FetchAccess<CONFIG>& fetch_access);

template <class CONFIG>
class FetchAccess
{
public:
	typename CONFIG::address_t cia;                      // current instruction address
	uint32_t size;                                       // size in byte of the access
	uint32_t data[CONFIG::FETCH_WIDTH];                  // loaded raw data
	MMUAccess<CONFIG> mmu_access;                        // MMU access
	CacheAccess<typename CONFIG::IL1_CONFIG> l1_access;  // L1 Cache access
	CacheAccess<typename CONFIG::L2_CONFIG> l2_access;   // L2 Cache access
	FetchAccess<CONFIG> *next_free;                      // next free element in the free list
	
	friend ostream& operator << <CONFIG> (ostream& os, const FetchAccess<CONFIG>& fetch_access);
};

template <class CONFIG>
class LoadStoreAccess;

template <class CONFIG>
ostream& operator << (ostream& os, const LoadStoreAccess<CONFIG>& load_store_access);

template <class CONFIG>
class LoadStoreAccess
{
public:
	typedef enum
	{
		INT8_LOAD                = 0x00001, // 8-bit integer load with zero extension to 32-bit integer
		INT16_LOAD               = 0x00002, // 16-bit integer load with zero extension to 32-bit integer
		SINT16_LOAD              = 0x00004, // 16-bit integer load with sign extension to 32-bit integer
		INT32_LOAD               = 0x00008, // 32-bit integer load with zero extension to 32-bit integer
		FP32_LOAD                = 0x00010, // 32-bit floating point load with conversion to 64-bit floating point
		FP64_LOAD                = 0x00020, // 64-bit floating point load
		INT16_LOAD_BYTE_REVERSE  = 0x00040, // 16-bit integer load byte reversed
		INT32_LOAD_BYTE_REVERSE  = 0x00080, // 32-bit integer load byte reversed
		INT_LOAD_MSB             = 0x00100, // 8-bit, 16-bit, 24-bit, or 32-bit integer load into most significant bits
		LOAD                     = 0x001ff, // binary mask to detect any of the above load
		INT8_STORE               = 0x00200, // 8-bit integer store
		INT16_STORE              = 0x00400, // 16-bit integer store
		INT32_STORE              = 0x00800, // 32-bit integer store
		FP32_STORE               = 0x01000, // 32-bit floating point store converted from a 64-bit floating point
		FP64_STORE               = 0x02000, // 64-bit floating point store
		FP_STORE_LSW             = 0x04000, // 32-bit raw bits from the 32 least signicative bits of a 64-bit floating point
		INT16_STORE_BYTE_REVERSE = 0x08000, // 16-bit integer store byte reversed
		INT32_STORE_BYTE_REVERSE = 0x10000, // 32-bit integer store byte reversed
		INT_STORE_MSB            = 0x20000, // 8-bit, 16-bit, 24-bit, or 32-bit integer store of most significant bits
		STORE                    = 0x3fe00  // binary mask to detect any of the above store
	} Type;

	Type type;                                           // type of Load/Store access
	unsigned int reg_num;                                // destination register
	typename CONFIG::address_t munged_ea;                // Munged Effective Address
	uint32_t offset;                                     // Offset from the actual Load/Store access
	uint32_t size;                                       // size in byte of the access
	bool valid;                                          // true if data is available (for a load)
	uint8_t data[8];                                     // data to store/loaded data
	Instruction<CONFIG> *instruction;                    // instruction that causes the access
	MMUAccess<CONFIG> mmu_access;                        // MMU access
	CacheAccess<typename CONFIG::DL1_CONFIG> l1_access;  // L1 Cache access
	CacheAccess<typename CONFIG::L2_CONFIG> l2_access;   // L2 Cache access
	LoadStoreAccess<CONFIG> *next_free;                  // next free element in the free list

	friend ostream& operator << <CONFIG> (ostream& os, const LoadStoreAccess<CONFIG>& load_store_access);
};

template <class CONFIG>
class BusAccess;

template <class CONFIG>
ostream& operator << (ostream& os, const BusAccess<CONFIG>& bus_access);

template <class CONFIG>
class BusAccess
{
public:
	typedef enum
	{
		LOAD,     // cache inhibited load
		REFILL,   // cache refill
		REFILLX,  // cache refill with intent to modify
		STORE,    // write through/cache inhibited store
		EVICTION  // block eviction
	} Type;

	bool issued;
	Type type;
	LoadStoreAccess<CONFIG> *load_store_access;
	FetchAccess<CONFIG> *fetch_access;
	typename CONFIG::physical_address_t addr;
	uint32_t size;
	uint8_t storage[CONFIG::FSB_BURST_SIZE];
	typename CONFIG::WIMG wimg;
	bool rwitm;
	BusAccess<CONFIG> *next_free;
	
	friend ostream& operator << <CONFIG> (ostream& os, const BusAccess<CONFIG>& bus_access);
};

template <class CONFIG>
class Operand;

template <class CONFIG>
ostream& operator << (ostream& os, const Operand<CONFIG>& operand);

template <class CONFIG>
class Operand
{
public:
	typedef enum { GPR, FPR, CR, LR, CTR, XER, FPSCR } Type;

	Type type;
	bool valid;             // whether the value is available
	unsigned reg_num;       // logical register number (always 0 for CR, LR, CTR, XER, FPSCR)
	int tag;                // >= 0: logical register number (same as reg_num), < 0: rename register number (1's complement)
	uint32_t int_value;     // operand value for GPR, CR, LR, CTR, XER, FPSCR
	SoftDouble float_value; // operand value for FPR
	unsigned int ref_count;

	class PendingInstructionsConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 7;
	};
	Queue<PendingInstructionsConfig> pending_instructions; // instruction waiting for that operand in a reservation station

	Operand<CONFIG> *next_free;

	CPU<CONFIG> *cpu;

	friend ostream& operator << <CONFIG> (ostream& os, const Operand<CONFIG>& operand);
};

template <class CONFIG>
class Instruction;

template <class CONFIG>
ostream& operator << (ostream& os, const Instruction<CONFIG>& instruction);

template <class CONFIG>
class Instruction
{
public:
	typedef enum { FUNCTIONAL_SIMULATION_MODE, PERFORMANCE_SIMULATION_MODE } SimulationMode;
	static const unsigned int MAX_INPUT_OPERANDS = 3;
	static const unsigned int MAX_OUTPUT_OPERANDS = 2;

	class ForwardingConfig
	{
	public:
		static const bool DEBUG = true;
		typedef bool ELEMENT;
		static const unsigned int SIZE = 3;
	};

	class InputOperandsConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operand<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 3;
	};

	class OutputOperandsConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operand<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 2;
	};

	typename CONFIG::address_t cia;
	typename CONFIG::address_t nia;
	Queue<InputOperandsConfig> input_operands;
	Queue<ForwardingConfig> forwarding;
	Queue<OutputOperandsConfig> output_operands;

	class LoadStoreAccessQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef LoadStoreAccess<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 2;
	};

	Queue<LoadStoreAccessQueueConfig> load_store_access_queue; // used only if instruction is a load/store
	unsigned int load_store_access_index;

	typedef enum { FETCH, BRANCH_EXECUTE, DISPATCH, ISSUE, EXECUTE, FINISH, COMPLETE, WRITEBACK } Stage;

	void Initialize(CPU<CONFIG> *cpu, typename CONFIG::address_t cia, uint32_t encoding);
	void Initialize(CPU<CONFIG> *cpu, const Instruction<CONFIG>& instruction, unsigned int uop_num);

	/*************************************************************************
	 *       Interface with the behavioral model generated by GenISSLib      *
	 *************************************************************************/
	void SetNIA(typename CONFIG::address_t value);
	void SetGPR(unsigned int n, uint32_t value, uint64_t latency = 1);
	void SetFPR(unsigned int n, const SoftDouble& value, uint64_t latency = 1);
	void SetCR(uint32_t value, uint64_t latency = 1);
	void SetLR(uint32_t value, uint64_t latency = 1);
	void SetCTR(uint32_t value, uint64_t latency = 1);
	void SetXER(uint32_t value);
	void SetSPR(unsigned int n, uint32_t value);
	void SetFPSCR(uint32_t value);

	typename CONFIG::address_t GetCIA() const;
	uint32_t GetGPR(unsigned int n) const;
	const SoftDouble& GetFPR(unsigned int n) const;
	uint32_t GetCR() const;
	uint32_t GetLR() const;
	uint32_t GetCTR() const;
	uint32_t GetXER() const;
	uint32_t GetSPR(unsigned int n) const;
	uint32_t GetFPSCR() const;
	uint32_t GetMSR() const;

	void Int8Load(unsigned int rd, typename CONFIG::address_t ea);
	void Int16Load(unsigned int rd, typename CONFIG::address_t ea);
	void SInt16Load(unsigned int rd, typename CONFIG::address_t ea);
	void Int32Load(unsigned int rd, typename CONFIG::address_t ea);
	void Fp32Load(unsigned int fd, typename CONFIG::address_t ea);
	void Fp64Load(unsigned int fd, typename CONFIG::address_t ea);
	void Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea);
	void Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea);
	void IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size);
	void Int8Store(unsigned int rs, typename CONFIG::address_t ea);
	void Int16Store(unsigned int rs, typename CONFIG::address_t ea);
	void Int32Store(unsigned int rs, typename CONFIG::address_t ea);
	void Fp32Store(unsigned int fs, typename CONFIG::address_t ea);
	void Fp64Store(unsigned int fs, typename CONFIG::address_t ea);
	void FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea);
	void Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea);
	void Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea);
	void IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size);

	/*************************************************************************/

	const Operand<CONFIG> *SearchInputOperand(typename Operand<CONFIG>::Type type, unsigned int reg_num = 0) const;
	Operand<CONFIG> *SearchOutputOperand(typename Operand<CONFIG>::Type type, unsigned int reg_num = 0);

	typename CONFIG::execution_unit_type_t GetExecutionUnit() const { return execution_unit; }
	typename CONFIG::serialization_t GetSerialization() const { return serialization; }
	unsigned int GetLatency() const { return operation->insn_latency; }

 	unsigned int GetNumOutputGPR() const { return num_input_gpr; }
 	unsigned int GetNumOutputFPR() const { return num_input_fpr; }
 	unsigned int GetNumOutputCR() const { return num_input_cr; }
 	unsigned int GetNumOutputLR() const { return num_input_lr; }
 	unsigned int GetNumOutputCTR() const { return num_input_ctr; }

	void Execute(CPU<CONFIG> *cpu);

	bool IsFinished() const { return finished; }
	void Finish() { finished = true; }

	bool IsAllowedToExecute() const { return allowed_to_execute; }
	bool IsReadyToExecute() const { return ready_to_execute; }
	bool CheckInputOperandsAvailability();
	void AllowExecution() { allowed_to_execute = true; }

	unsigned int GetNumUOps() const { return num_uops; }
	unsigned int GetUOpNum() const { return uop_num; }

	Instruction();
	~Instruction();

	unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation;
	CPU<CONFIG> *cpu;
	SimulationMode sim_mode;
	Stage stage;

	typename CONFIG::execution_unit_type_t execution_unit;
	typename CONFIG::serialization_t serialization;
	bool allowed_to_execute;
	bool ready_to_execute;
	bool finished;
	bool retired;
	unsigned int num_uops;
	unsigned int uop_num;
	Instruction<CONFIG> *macro_op;
	unsigned int num_input_gpr;
	unsigned int num_input_fpr;
	unsigned int num_input_cr;
	unsigned int num_input_lr;
	unsigned int num_input_ctr;

	void Initialize(CPU<CONFIG> *cpu, unsigned int uop_num);

	Instruction<CONFIG> *next_free;

	friend ostream& operator << <CONFIG> (ostream& os, const Instruction<CONFIG>& instruction);
};

template <class CONFIG>
class Event
{
public:
	typedef enum
	{
		EV_NULL,              // null event
		EV_FINISHED_INSN,     // an instruction is finished
		EV_AVAILABLE_OPERAND, // an operand is available
		EV_BUS_ACCESS,        // a cache miss causes a bus access
		EV_FINISHED_FETCH     // an instruction fetch is finished
	} Type;

	Type type;
	union
	{
		Instruction<CONFIG> *instruction;   // used if type is EV_FINISHED_INSN
		Operand<CONFIG> *operand;           // used if type is either EV_AVAILABLE_OPERAND or EV_OPERAND_WB
		BusAccess<CONFIG> *bus_access;      // used if type is EV_BUS_ACCESS
		FetchAccess<CONFIG> *fetch_access;  // used if type is EV_FINISHED_FETCH
		void *null;                         // used if null event. Should be 0
	} object;

	Event<CONFIG> *next_free;
};

template <class CONFIG>
class MappingTable
{
public:
	MappingTable();
	~MappingTable();
	bool Allocate(unsigned int reg_num, int& tag);
	int Lookup(unsigned int reg_num);
	void Free(unsigned int reg_num, int tag);
	unsigned int GetNumFreeRenameRegisters() const;
private:
	int mapping_table[CONFIG::NUM_LOGICAL_REGISTERS];

	class FreeListConfig
	{
	public:
		static const bool DEBUG = true;
		typedef int ELEMENT;
		static const unsigned int SIZE = CONFIG::NUM_RENAME_REGISTERS;
	};

	Queue<FreeListConfig> free_list;
};

class VectorRegisterView : public unisim::kernel::service::VariableBase
{
public:
	VectorRegisterView(const char *name, unisim::kernel::service::Object *owner, vr_t& storage, const char *description);
	virtual ~VectorRegisterView();
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
	vr_t& storage;
};

template <class CONFIG>
class CPU :
	public unisim::component::cxx::processor::powerpc::mpc7447a::Decoder<CONFIG>,
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
	ServiceImport<DebugControl<typename CONFIG::address_t> > debug_control_import;
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
	void StepOneCycle();
	void Run();
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
	inline bool IsFPUEnabled();
	inline bool IsFPUExceptionEnabled();
	inline bool IsDataCacheEnabled();
	inline bool IsInsnCacheEnabled();
	inline bool IsL2CacheEnabled();
	inline bool IsAddressBroadcastEnabled();

	//=====================================================================
	//=                State interface (with .isa files)                  =
	//=====================================================================

	const SoftDouble& GetFPR(unsigned int n) const { return fpr[n]; }
	void SetFPR(unsigned int n, const SoftDouble& value) { fpr[n] = value; }

	void Int8Load(unsigned int rd, typename CONFIG::address_t ea);
	void Int16Load(unsigned int rd, typename CONFIG::address_t ea);
	void SInt16Load(unsigned int rd, typename CONFIG::address_t ea);
	void Int32Load(unsigned int rd, typename CONFIG::address_t ea);
	void Fp32Load(unsigned int fd, typename CONFIG::address_t ea);
	void Fp64Load(unsigned int fd, typename CONFIG::address_t ea);
	void Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea);
	void Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea);
	void IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size);
	void Int8Store(unsigned int rs, typename CONFIG::address_t ea);
	void Int16Store(unsigned int rs, typename CONFIG::address_t ea);
	void Int32Store(unsigned int rs, typename CONFIG::address_t ea);
	void Fp32Store(unsigned int fs, typename CONFIG::address_t ea);
	void Fp64Store(unsigned int fs, typename CONFIG::address_t ea);
	void FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea);
	void Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea);
	void Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea);
	void IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size);

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
	
	inline void SetPVR(uint32_t value) { pvr = value; }
	inline uint32_t GetPVR() { return pvr; }
	
	//=====================================================================
	//=                    user level registers set/get methods           =
	//=====================================================================
	
	inline uint32_t GetGPR(unsigned int n) { return gpr[n]; }
	inline void SetGPR(unsigned int n, uint32_t value) { gpr[n] = value; }
	inline uint32_t GetLR() { return lr; }
	inline void SetLR(uint32_t value) { lr = value; }
	inline uint32_t GetCTR() { return ctr; }
	inline void SetCTR(uint32_t value) { ctr = value; }
	inline uint32_t GetVRSAVE() { return vrsave; }
	inline void SetVRSAVE(uint32_t value) { vrsave = value; }	
	
	//=====================================================================
	//=                  instruction address set/get methods              =
	//=====================================================================
	
	inline uint32_t GetCIA() const { return cia; }
	inline void SetCIA(uint32_t value) { cia = value; seq_cia = value; }
	inline uint32_t GetNIA() const { return nia; }
	inline void SetNIA(uint32_t value) { nia = value; }
	inline void Branch(uint32_t value) { nia = value; FlushSubsequentInstructions(); }

	//=====================================================================
	//=            external address register set/get methods              =
	//=====================================================================
	
	inline void SetEAR(uint32_t value) { ear = value; }
	inline uint32_t GetEAR() { return ear; }
	
	//=====================================================================
	//=           special purpose registers set/get methods               =
	//=====================================================================

	uint32_t GetSPR(unsigned int n);
	void SetSPR(unsigned int n, uint32_t value);
	
	//=====================================================================
	//=          exception handling registers set/get methods             =
	//=====================================================================
	
	inline uint32_t GetSPRG(unsigned int n) { return sprg[n]; }
	inline void SetSPRG(unsigned int n, uint32_t value) { sprg[n] = value; }
	inline uint32_t GetSRR0() { return srr0; }
	inline void SetSRR0(uint32_t value) { srr0 = value; }
	inline uint32_t GetSRR1() { return srr1; }
	inline void SetSRR1(uint32_t value) { srr1 = value; }
	inline uint32_t GetSRR1_WAY() { return (GetSRR1() >> 17) & 1; }
	inline void SetDAR(uint32_t value) { dar = value; }
	inline uint32_t GetDAR() { return dar; }
	inline void SetDSISR(uint32_t value) { dsisr = value; }
	uint32_t GetDSISR() { return dsisr; }
	
	//=====================================================================
	//=              DEC/TBL/TBU registers set/get methods                =
	//=====================================================================
	
	void SetDEC(uint32_t value);
	inline uint32_t GetDEC() { return dec; }
	inline uint32_t GetTBL() { return tbl; }
	inline void SetTBL(uint32_t value) { tbl = value; }
	inline uint32_t GetTBU() { return tbu; }
	inline void SetTBU(uint32_t value) { tbu = value; }

	//=====================================================================
	//=                        XER set/get methods                        =
	//=====================================================================

	inline void SetXER(uint32_t value) { xer = value & CONFIG::XER_MASK; }
	inline uint32_t GetXER() { return xer; }

	// Deprecated methods
	inline uint32_t GetXER_SO() { return (xer & CONFIG::XER_SO_MASK) >> CONFIG::XER_SO_OFFSET; }
	inline void SetXER_SO() { xer = xer | CONFIG::XER_SO_MASK; }
	inline void ResetXER_SO() { xer = xer & ~CONFIG::XER_SO_MASK; }
	inline uint32_t GetXER_OV() { return (xer & CONFIG::XER_OV_MASK) >> CONFIG::XER_OV_OFFSET; }
	inline void SetXER_OV() { xer = xer | CONFIG::XER_OV_MASK; }
	inline void ResetXER_OV() { xer = xer & ~CONFIG::XER_OV_MASK; }
	inline uint32_t GetXER_CA() { return (xer & CONFIG::XER_CA_MASK) >> CONFIG::XER_CA_OFFSET; }
	inline void SetXER_CA() { xer = xer | CONFIG::XER_CA_MASK; }
	inline void ResetXER_CA() { xer = xer & ~CONFIG::XER_CA_MASK; }
	inline uint32_t GetXER_BYTE_COUNT() { return (xer & CONFIG::XER_BYTE_COUNT_MASK) >> CONFIG::XER_BYTE_COUNT_OFFSET; }

	
	//=====================================================================
	//=                         CR set/get methods                        =
	//=====================================================================

	inline uint32_t GetCR() { return cr; }
	inline void SetCR(uint32_t value) { cr = value; }

	// Deprecated methods
	inline uint32_t GetCRF(unsigned int crf) { return cr >> ((7 - crf) * 4); }
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
	
	uint32_t GetMSR() { return msr; }
	void SetMSR(uint32_t value);
	inline uint32_t GetMSR_VEC() { return (GetMSR() & CONFIG::MSR_VEC_MASK) >> CONFIG::MSR_VEC_OFFSET; }
	inline uint32_t GetMSR_POW() { return (GetMSR() & CONFIG::MSR_POW_MASK) >> CONFIG::MSR_POW_OFFSET; }
	inline uint32_t GetMSR_ILE() { return (GetMSR() & CONFIG::MSR_ILE_MASK) >> CONFIG::MSR_ILE_OFFSET; }
	inline uint32_t GetMSR_EE() { return (GetMSR() & CONFIG::MSR_EE_MASK) >> CONFIG::MSR_EE_OFFSET; }
	inline uint32_t GetMSR_PR() { return (GetMSR() & CONFIG::MSR_PR_MASK) >> CONFIG::MSR_PR_OFFSET; }
	inline typename CONFIG::PrivilegeLevel GetPrivilegeLevel() { return GetMSR_PR() ? CONFIG::PR_USER : CONFIG::PR_SUPERVISOR; }
	inline uint32_t GetMSR_FP() { return (GetMSR() & CONFIG::MSR_FP_MASK) >> CONFIG::MSR_FP_OFFSET; }
	inline uint32_t GetMSR_ME() { return (GetMSR() & CONFIG::MSR_ME_MASK) >> CONFIG::MSR_ME_OFFSET; }
	inline uint32_t GetMSR_FE0() { return (GetMSR() & CONFIG::MSR_FE0_MASK) >> CONFIG::MSR_FE0_OFFSET; }
	inline uint32_t GetMSR_SE() { return (GetMSR() & CONFIG::MSR_SE_MASK) >> CONFIG::MSR_SE_OFFSET; }
	inline uint32_t GetMSR_BE() { return (GetMSR() & CONFIG::MSR_BE_MASK) >> CONFIG::MSR_BE_OFFSET; }
	inline uint32_t GetMSR_FE1() { return (GetMSR() & CONFIG::MSR_FE1_MASK) >> CONFIG::MSR_FE1_OFFSET; }
	inline uint32_t GetMSR_IP() { return (GetMSR() & CONFIG::MSR_IP_MASK) >> CONFIG::MSR_IP_OFFSET; }
	inline uint32_t GetMSR_IR() { return (GetMSR() & CONFIG::MSR_IR_MASK) >> CONFIG::MSR_IR_OFFSET; }
	inline uint32_t GetMSR_DR() { return (GetMSR() & CONFIG::MSR_DR_MASK) >> CONFIG::MSR_DR_OFFSET; }
	inline uint32_t GetMSR_PM() { return (GetMSR() & CONFIG::MSR_PM_MASK) >> CONFIG::MSR_PM_OFFSET; }
	inline uint32_t GetMSR_PMM() { return GetMSR_PM(); }
	inline uint32_t GetMSR_RI() { return (GetMSR() & CONFIG::MSR_RI_MASK) >> CONFIG::MSR_RI_OFFSET; }
	inline uint32_t GetMSR_LE() { return (GetMSR() & CONFIG::MSR_LE_MASK) >> CONFIG::MSR_LE_OFFSET; }
	inline void SetMSR_VEC() { SetMSR(GetMSR() | CONFIG::MSR_VEC_MASK); }
	inline void SetMSR_POW() { SetMSR(GetMSR() | CONFIG::MSR_POW_MASK); }
	inline void SetMSR_ILE() { SetMSR(GetMSR() | CONFIG::MSR_ILE_MASK); }
	inline void SetMSR_EE() { SetMSR(GetMSR() | CONFIG::MSR_EE_MASK); }
	inline void SetMSR_PR() { SetMSR(GetMSR() | CONFIG::MSR_PR_MASK); }
	inline void SetMSR_FP() { SetMSR(GetMSR() | CONFIG::MSR_FP_MASK); }
	inline void SetMSR_ME() { SetMSR(GetMSR() | CONFIG::MSR_ME_MASK); }
	inline void SetMSR_FE0() { SetMSR(GetMSR() | CONFIG::MSR_FE0_MASK); }
	inline void SetMSR_SE() { SetMSR(GetMSR() | CONFIG::MSR_SE_MASK); }
	inline void SetMSR_BE() { SetMSR(GetMSR() | CONFIG::MSR_BE_MASK); }
	inline void SetMSR_FE1() { SetMSR(GetMSR() | CONFIG::MSR_FE1_MASK); }
	inline void SetMSR_IP() { SetMSR(GetMSR() | CONFIG::MSR_IP_MASK); }
	inline void SetMSR_IR() { SetMSR(GetMSR() | CONFIG::MSR_IR_MASK); }
	inline void SetMSR_DR() { SetMSR(GetMSR() | CONFIG::MSR_DR_MASK); }
	inline void SetMSR_PM() { SetMSR(GetMSR() | CONFIG::MSR_PM_MASK); }
	inline void SetMSR_PMM() { SetMSR_PM(); }
	inline void SetMSR_RI() { SetMSR(GetMSR() | CONFIG::MSR_RI_MASK); }
	inline void SetMSR_LE() { SetMSR(GetMSR() | CONFIG::MSR_LE_MASK); }
	inline void ResetMSR_VEC() { SetMSR(GetMSR() & ~CONFIG::MSR_VEC_MASK); }
	inline void ResetMSR_POW() { SetMSR(GetMSR() & ~CONFIG::MSR_POW_MASK); }
	inline void ResetMSR_ILE() { SetMSR(GetMSR() & ~CONFIG::MSR_ILE_MASK); }
	inline void ResetMSR_EE() { SetMSR(GetMSR() & ~CONFIG::MSR_EE_MASK); }
	inline void ResetMSR_PR() { SetMSR(GetMSR() & ~CONFIG::MSR_PR_MASK); }
	inline void ResetMSR_FP() { SetMSR(GetMSR() & ~CONFIG::MSR_FP_MASK); }
	inline void ResetMSR_ME() { SetMSR(GetMSR() & ~CONFIG::MSR_ME_MASK); }
	inline void ResetMSR_FE0() { SetMSR(GetMSR() & ~CONFIG::MSR_FE0_MASK); }
	inline void ResetMSR_SE() { SetMSR(GetMSR() & ~CONFIG::MSR_SE_MASK); }
	inline void ResetMSR_BE() { SetMSR(GetMSR() & ~CONFIG::MSR_BE_MASK); }
	inline void ResetMSR_FE1() { SetMSR(GetMSR() & ~CONFIG::MSR_FE1_MASK); }
	inline void ResetMSR_IP() { SetMSR(GetMSR() & ~CONFIG::MSR_IP_MASK); }
	inline void ResetMSR_IR() { SetMSR(GetMSR() & ~CONFIG::MSR_IR_MASK); }
	inline void ResetMSR_DR() { SetMSR(GetMSR() & ~CONFIG::MSR_DR_MASK); }
	inline void ResetMSR_PM() { SetMSR(GetMSR() & ~CONFIG::MSR_PM_MASK); }
	inline void ResetMSR_PMM() { ResetMSR_PM(); }
	inline void ResetMSR_RI() { SetMSR(GetMSR() & ~CONFIG::MSR_RI_MASK); }
	inline void ResetMSR_LE() { SetMSR(GetMSR() & ~CONFIG::MSR_LE_MASK); }


	//=====================================================================
	//=                   FPU registers set/get methods                   =
	//=====================================================================

	inline uint32_t GetFPSCR() { return fpscr; }
	inline void SetFPSCR(uint32_t value) { fpscr = value; }
	
	//=====================================================================
	//=                  MMU registers set/get methods                    =
	//=====================================================================

	inline uint32_t GetSR(unsigned int n) { return sr[n]; }
	inline void SetSR(unsigned int n, uint32_t value) { sr[n] = value; }

	// Methods for reading SR registers bits
	inline uint32_t GetSR_T(unsigned int n) { return (sr[n] >> 31) & 1; }
	inline uint32_t GetSR_KS(unsigned int n) { return (sr[n] >> 30) & 1; }
	inline uint32_t GetSR_KP(unsigned int n) { return (sr[n] >> 29) & 1; }
	inline uint32_t GetSR_N(unsigned int n) { return (sr[n] >> 28) & 1; }
	inline uint32_t GetSR_VSID(unsigned int n) { return sr[n] & 0xffffffUL; }

	//=====================================================================
	//=                   Vector instructions handling                    =
	//=====================================================================
	
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
	
	void Tlbia();
	void Tlbie(typename CONFIG::address_t addr);
	void Tlbld(typename CONFIG::address_t addr);
	void Tlbli(typename CONFIG::address_t addr);
	void Tlbre(unsigned int rd, unsigned int ra, unsigned int ws);
	void Tlbsx(unsigned int rd, typename CONFIG::address_t addr);
	void Tlbwe(unsigned int rs, unsigned int ra, unsigned int ws);

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

	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

	typename CONFIG::address_t GetEA() { return effective_address; }
	
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
	
	void ResetIRQ(unsigned int irq);

	inline bool HasDecrementerOverflow() const { return irq & CONFIG::IRQ_DECREMENTER_OVERFLOW; }
	inline bool HasExternalInterrupt() const { return irq & CONFIG::IRQ_EXTERNAL_INTERRUPT; }
	inline bool HasHardReset() const { return irq & CONFIG::IRQ_HARD_RESET; }
	inline bool HasSoftReset() const { return irq & CONFIG::IRQ_SOFT_RESET; }
	inline bool HasMCP() const { return irq & CONFIG::IRQ_MCP; }
	inline bool HasTEA() const { return irq & CONFIG::IRQ_TEA; }
	inline bool HasSMI() const { return irq & CONFIG::IRQ_SMI; }
	inline bool HasPerformanceMonitorInterrupt() const { return irq & CONFIG::IRQ_PERFORMANCE_MONITOR_INTERRUPT; }
	inline bool HasIRQ() const { return irq; }

	//=====================================================================
	//=                    Hardware interrupt request                     =
	//=====================================================================
	
	void SetIRQ(unsigned int irq);

protected:
	//=====================================================================
	//=          DEC/TBL/TBU bus-time based update methods                =
	//=====================================================================
	
	void OnBusCycle();
	
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
	inline bool IsVerboseDTLB() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_DTLB_ENABLE && (verbose_all || verbose_dtlb); }
	inline bool IsVerboseITLB() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_ITLB_ENABLE && (verbose_all || verbose_itlb); }
	inline bool IsVerboseDL1() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_DL1_ENABLE && (verbose_all || verbose_dl1); }
	inline bool IsVerboseIL1() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_IL1_ENABLE && (verbose_all || verbose_il1); }
	inline bool IsVerboseL2() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_L2_ENABLE && (verbose_all || verbose_l2); }
	inline bool IsVerboseLoad() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_LOAD_ENABLE && (verbose_all || verbose_load); }
	inline bool IsVerboseStore() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STORE_ENABLE && (verbose_all || verbose_store); }
	inline bool IsVerboseReadMemory() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_READ_MEMORY_ENABLE && (verbose_all || verbose_read_memory); }
	inline bool IsVerboseWriteMemory() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_WRITE_MEMORY_ENABLE && (verbose_all || verbose_write_memory); }
	inline bool IsVerboseException() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception); }
	inline bool IsVerboseSetMSR() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_MSR_ENABLE && (verbose_all || verbose_set_msr); }
	inline bool IsVerboseSetHID0() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_HID0_ENABLE && (verbose_all || verbose_set_hid0); }
	inline bool IsVerboseSetHID1() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_HID1_ENABLE && (verbose_all || verbose_set_hid1); }
	inline bool IsVerboseSetHID2() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_HID2_ENABLE && (verbose_all || verbose_set_hid2); }
	inline bool IsVerboseSetL2CR() const { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SET_L2CR_ENABLE && (verbose_all || verbose_set_l2cr); }

	//=====================================================================
	//=                      Bus access methods                           =
	//=====================================================================

	virtual void BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT, bool rwitm = false);
	virtual void BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT);
	virtual void BusZeroBlock(typename CONFIG::physical_address_t physical_addr);
	virtual void BusFlushBlock(typename CONFIG::physical_address_t physical_addr);

	
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================
	
	uint64_t cpu_cycle_time; //!< CPU cycle time in ps
	uint64_t voltage;        //!< CPU voltage in mV
	uint64_t bus_cycle_time; //!< Front side bus cycle time in ps
	uint64_t cpu_cycle;      //!< Number of cpu cycles
	uint64_t bus_cycle;      //!< Number of front side bus cycles

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
	void LookupDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void LookupIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void LookupL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	void EmuEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void EmuEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void EmuEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	void ChooseLineToEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void ChooseLineToEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void ChooseLineToEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	void UpdateReplacementPolicyDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access);
	void UpdateReplacementPolicyIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access);
	void UpdateReplacementPolicyL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access);
	void EmuFillDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg, bool rwitm);
	void EmuFillIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg);
	void EmuFillL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access, typename CONFIG::WIMG wimg, bool rwitm);
	void EmuFetch(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	inline uint32_t EmuFetch(typename CONFIG::address_t addr);
	template <bool TRANSLATE_ADDR> void EmuLoad(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	template <bool TRANSLATE_ADDR> void EmuStore(typename CONFIG::address_t addr, const void *buffer, uint32_t size);
	template <class T> void EmuLoad(T& value, typename CONFIG::address_t ea);
	template <class T> void EmuStore(T value, typename CONFIG::address_t ea);
	inline typename CONFIG::address_t MungEffectiveAddress(typename CONFIG::address_t ea, unsigned int size) { return (msr & CONFIG::MSR_LE_MASK) ? ea ^ (8 - size) : ea; }
	
	
#ifdef SOCLIB
	virtual void GenLoadStore(typename LoadStoreAccess<CONFIG>::Type type, unsigned int reg_num, typename CONFIG::address_t munged_ea, uint32_t size) = 0;
#endif

	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	bool ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr);
	bool WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr);

	//=====================================================================
	//=                      L2CR set/get methods                         =
	//=====================================================================
	
	inline uint32_t GetL2CR() { return l2cr; }
	void SetL2CR(uint32_t value);
	inline uint32_t GetL2CR_L2E() { return (GetL2CR() & CONFIG::L2CR_L2E_MASK) >> CONFIG::L2CR_L2E_OFFSET; }
	inline uint32_t GetL2CR_L2I() { return (GetL2CR() & CONFIG::L2CR_L2I_MASK) >> CONFIG::L2CR_L2I_OFFSET; }
	inline void SetL2CR_L2E() { SetL2CR(GetL2CR() | CONFIG::L2CR_L2E_MASK); }
	inline void SetL2CR_L2I() { SetL2CR(GetL2CR() | CONFIG::L2CR_L2I_MASK); }
	inline void ResetL2CR_L2E() { SetL2CR(GetL2CR() & ~CONFIG::L2CR_L2E_MASK); }
	inline void ResetL2CR_L2I() { SetL2CR(GetL2CR() & ~CONFIG::L2CR_L2I_MASK); }

	//=====================================================================
	//=                      IABR set/get methods                         =
	//=====================================================================
	inline uint32_t GetIABR() { return iabr; }
	inline void SetIABR(uint32_t value) { iabr = value; }
	inline uint32_t GetIABR_ADDR() { return (GetIABR() & CONFIG::IABR_ADDR_MASK) >> CONFIG::IABR_ADDR_OFFSET; }
	inline uint32_t GetIABR_BE() { return (GetIABR() & CONFIG::IABR_BE_MASK) >> CONFIG::IABR_BE_OFFSET; }
	inline uint32_t GetIABR_TE() { return (GetIABR() & CONFIG::IABR_TE_MASK) >> CONFIG::IABR_TE_OFFSET; }

	inline uint32_t GetLDSTDB() { return ldstdb; }
	inline void SetLDSTDB(uint32_t value) { ldstdb = value; }
	
	inline uint32_t GetICTC() { return ictc; }
	inline void SetICTC(uint32_t value) { ictc = value; }

	//=====================================================================
	//=                       DABR set/get methods                        =
	//=====================================================================
	
	inline void SetDABR(uint32_t value) { dabr = value; }
	inline uint32_t GetDABR() { return dabr; }
	inline uint32_t GetDABR_DAB() { return (dabr >> 3) & 0x1fffffffUL; }
	inline uint32_t GetDABR_BT() { return (dabr >> 2) & 1; }inline uint32_t GetDABR_DW() { return (dabr >> 1) & 1; }
	inline uint32_t GetDABR_DR() { return dabr & 1; }

	//=====================================================================
	//=         performance monitor registers set/get methods             =
	//=====================================================================
	
	inline uint32_t GetPMC1() { return pmc1; }
	inline void SetPMC1(uint32_t value) { pmc1 = value; }
	inline uint32_t GetPMC2() { return pmc2; }
	inline void SetPMC2(uint32_t value) { pmc2 = value; }
	inline uint32_t GetPMC3() { return pmc3; }
	inline void SetPMC3(uint32_t value) { pmc3 = value; }
	inline uint32_t GetPMC4() { return pmc4; }
	inline void SetPMC4(uint32_t value) { pmc4 = value; }
	inline uint32_t GetPMC5() { return pmc5; }
	inline void SetPMC5(uint32_t value) { pmc5 = value; }
	inline uint32_t GetPMC6() { return pmc6; }
	inline void SetPMC6(uint32_t value) { pmc6 = value; }
	inline uint32_t GetSIA() { return sia; }
	inline void SetSIA(uint32_t value) { sia = value; }
	inline uint32_t GetSDA() { return sda; }
	inline void SetSDA(uint32_t value) { sda = value; }
	inline uint32_t GetMMCR0() { return mmcr0; }
	inline void SetMMCR0(uint32_t value) { mmcr0 = value; }
	inline uint32_t GetMMCR1() { return mmcr1; }
	inline void SetMMCR1(uint32_t value) { mmcr1 = value; }
	inline uint32_t GetMMCR2() { return mmcr2; }
	inline void SetMMCR2(uint32_t value) { mmcr2 = value; }


	//=====================================================================
	//=                  HID0 register set/get methods                    =
	//=====================================================================

	inline uint32_t GetHID0() { return hid0; }
	void SetHID0(uint32_t value);
	inline uint32_t GetHID0_TBEN() { return (GetHID0() & CONFIG::HID0_TBEN_MASK) >> CONFIG::HID0_TBEN_OFFSET; }
	inline uint32_t GetHID0_STEN() { return (GetHID0() & CONFIG::HID0_STEN_MASK) >> CONFIG::HID0_STEN_OFFSET; }
	inline uint32_t GetHID0_HIGH_BAT_EN() { return (GetHID0() & CONFIG::HID0_HIGH_BAT_EN_MASK) >> CONFIG::HID0_HIGH_BAT_EN_OFFSET; }
	inline uint32_t GetHID0_NAP() { return (GetHID0() & CONFIG::HID0_NAP_MASK) >> CONFIG::HID0_NAP_OFFSET; }
	inline uint32_t GetHID0_SLEEP() { return (GetHID0() & CONFIG::HID0_SLEEP_MASK) >> CONFIG::HID0_SLEEP_OFFSET; }
	inline uint32_t GetHID0_DPM() { return (GetHID0() & CONFIG::HID0_DPM_MASK) >> CONFIG::HID0_DPM_OFFSET; }
	inline uint32_t GetHID0_BHTCLR() { return (GetHID0() & CONFIG::HID0_BHTCLR_MASK) >> CONFIG::HID0_BHTCLR_OFFSET; }
	inline uint32_t GetHID0_XAEN() { return (GetHID0() & CONFIG::HID0_XAEN_MASK) >> CONFIG::HID0_XAEN_OFFSET; }
	inline uint32_t GetHID0_NHR() { return (GetHID0() & CONFIG::HID0_NHR_MASK) >> CONFIG::HID0_NHR_OFFSET; }
	inline uint32_t GetHID0_ICE() { return (GetHID0() & CONFIG::HID0_ICE_MASK) >> CONFIG::HID0_ICE_OFFSET; }
	inline uint32_t GetHID0_DCE() { return (GetHID0() & CONFIG::HID0_DCE_MASK) >> CONFIG::HID0_DCE_OFFSET; }
	inline uint32_t GetHID0_ILOCK() { return (GetHID0() & CONFIG::HID0_ILOCK_MASK) >> CONFIG::HID0_ILOCK_OFFSET; }
	inline uint32_t GetHID0_DLOCK() { return (GetHID0() & CONFIG::HID0_DLOCK_MASK) >> CONFIG::HID0_DLOCK_OFFSET; }
	inline uint32_t GetHID0_ICFI() { return (GetHID0() & CONFIG::HID0_ICFI_MASK) >> CONFIG::HID0_ICFI_OFFSET; }
	inline uint32_t GetHID0_DCFI() { return (GetHID0() & CONFIG::HID0_DCFI_MASK) >> CONFIG::HID0_DCFI_OFFSET; }
	inline uint32_t GetHID0_SPD() { return (GetHID0() & CONFIG::HID0_SPD_MASK) >> CONFIG::HID0_SPD_OFFSET; }
	inline uint32_t GetHID0_XBSEN() { return (GetHID0() & CONFIG::HID0_XBSEN_MASK) >> CONFIG::HID0_XBSEN_OFFSET; }
	inline uint32_t GetHID0_SGE() { return (GetHID0() & CONFIG::HID0_SGE_MASK) >> CONFIG::HID0_SGE_OFFSET; }
	inline uint32_t GetHID0_BTIC() { return (GetHID0() & CONFIG::HID0_BTIC_MASK) >> CONFIG::HID0_BTIC_OFFSET; }
	inline uint32_t GetHID0_LRSTK() { return (GetHID0() & CONFIG::HID0_LRSTK_MASK) >> CONFIG::HID0_LRSTK_OFFSET; }
	inline uint32_t GetHID0_FOLD() { return (GetHID0() & CONFIG::HID0_FOLD_MASK) >> CONFIG::HID0_FOLD_OFFSET; }
	inline uint32_t GetHID0_BHT() { return (GetHID0() & CONFIG::HID0_BHT_MASK) >> CONFIG::HID0_BHT_OFFSET; }
	inline uint32_t GetHID0_NOPDST() { return (GetHID0() & CONFIG::HID0_NOPDST_MASK) >> CONFIG::HID0_NOPDST_OFFSET; }
	inline uint32_t GetHID0_NOPTI() { return (GetHID0() & CONFIG::HID0_NOPTI_MASK) >> CONFIG::HID0_NOPTI_OFFSET; }
	inline void SetHID0_DCE() { SetHID0(GetHID0() | CONFIG::HID0_DCE_MASK); }
	inline void ResetHID0_DCE() { SetHID0(GetHID0() & ~CONFIG::HID0_DCE_MASK); }
	inline void SetHID0_ICE() { SetHID0(GetHID0() | CONFIG::HID0_ICE_MASK); }
	inline void ResetHID0_ICE() { SetHID0(GetHID0() & ~CONFIG::HID0_ICE_MASK); }
	inline void ResetHID0_ICFI() { SetHID0(GetHID0() & ~CONFIG::HID0_ICFI_MASK); }
	inline void ResetHID0_DCFI() { SetHID0(GetHID0() & ~CONFIG::HID0_DCFI_MASK); }
	inline void SetHID0_NHR() { SetHID0(GetHID0() | CONFIG::HID0_NHR_MASK); }
	inline void ResetHID0_NHR() { SetHID0(GetHID0() & ~CONFIG::HID0_NHR_MASK); }

	//=====================================================================
	//=                  HID1 register set/get methods                    =
	//=====================================================================
	
	inline uint32_t GetHID1() { return hid1; }
	void SetHID1(uint32_t value);
	inline uint32_t GetHID1_EMCP() { return (GetHID1() & CONFIG::HID1_EMCP_MASK) >> CONFIG::HID1_EMCP_OFFSET; }
	inline uint32_t GetHID1_ABE() { return (GetHID1() & CONFIG::HID1_ABE_MASK) >> CONFIG::HID1_ABE_OFFSET; }
	inline void SetHID1_ABE() { SetHID1(GetHID1() | CONFIG::HID1_ABE_MASK); }
	inline void ResetHID1_ABE() { SetHID1(GetHID1() & ~CONFIG::HID1_ABE_MASK); }
	
	inline void SetPTEHI(uint32_t value) { ptehi = value; }
	inline uint32_t GetPTEHI() { return ptehi; }
	inline void SetPTELO(uint32_t value) { ptelo = value; }
	inline uint32_t GetPTELO() { return ptelo; }

	//=====================================================================
	//=                  MMU registers set/get methods                    =
	//=====================================================================
	
	inline uint32_t GetDBATL(unsigned int n) { return dbat[n].l; }
	inline void SetDBATL(unsigned int n, uint32_t value) { dbat[n].l = value; ReconfigureFastBATLookup(); }
	
	inline uint32_t GetDBATU(unsigned int n) { return dbat[n].u; }
	inline void SetDBATU(unsigned int n, uint32_t value) { dbat[n].u = value; ReconfigureFastBATLookup(); }
	
	inline uint32_t GetIBATL(unsigned int n) { return ibat[n].l; }
	inline void SetIBATL(unsigned int n, uint32_t value) { ibat[n].l = value; ReconfigureFastBATLookup(); }
	
	inline uint32_t GetIBATU(unsigned int n) { return ibat[n].u; }
	inline void SetIBATU(unsigned int n, uint32_t value) { ibat[n].u = value; ReconfigureFastBATLookup(); }

	inline uint32_t GetBATU_BEPI(uint32_t value) { return (value >> 17) & 0x7fff; }
	inline uint32_t GetBATU_BL(uint32_t value) { return (value >> 2) & 0x7ff; }
	inline uint32_t GetBATU_VS(uint32_t value) { return (value >> 1) & 1; }
	inline uint32_t GetBATU_VP(uint32_t value) { return value & 1; }
	inline uint32_t GetBATL_BRPN(uint32_t value) { return (value >> 17) & 0x7fff; }
	inline uint32_t GetBATL_WIMG(uint32_t value) { return (value >> 3) & 0xf; }
	inline uint32_t GetBATL_PP(uint32_t value) { return value & 3; }

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

	inline uint32_t GetSDR1() { return sdr1; }
	inline void SetSDR1(uint32_t value) { sdr1 = value; }

	// Methods for reading SDR1 bits
	inline uint32_t GetSDR1_HTABORG() { return (sdr1 >> 16) & 0xffff; }
	inline uint32_t GetSDR1_HTABMASK() { return sdr1 & 0x1ff; }

	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
	typename CONFIG::address_t effective_address;

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
	bool enable_halt_on;
	typename CONFIG::address_t halt_on_addr;
	std::string halt_on;
	uint64_t trap_on_instruction_counter;
	uint64_t max_inst;                                         //!< Maximum number of instructions to execute

	map<string, unisim::util::debug::Register *> registers_registry;       //!< Every CPU register interfaces excluding MMU/FPU registers
	std::vector<unisim::kernel::service::VariableBase *> registers_registry2;       //!< Every CPU register
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

	template <bool DEBUG> void LookupBAT(MMUAccess<CONFIG>& mmu_access);
	void ReconfigureFastBATLookup();
	void LookupITLB(MMUAccess<CONFIG>& mmu_access);
	void LookupDTLB(MMUAccess<CONFIG>& mmu_access);
	void UpdateITLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access);
	void UpdateDTLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access);
	void ChooseEntryToEvictITLB(MMUAccess<CONFIG>& mmu_access);
	void ChooseEntryToEvictDTLB(MMUAccess<CONFIG>& mmu_access);
	template <bool DEBUG> void AccessTLB(MMUAccess<CONFIG>& mmu_access);
	template <bool DEBUG> void EmuHardwarePageTableSearch(MMUAccess<CONFIG>& mmu_access);
	template <bool DEBUG> void EmuTranslateAddress(MMUAccess<CONFIG>& mmu_access);
	void LoadITLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo);
	void LoadDTLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo);
	void DumpPageTable(ostream& os);

	//=====================================================================
	//=                    registers set/get methods                      =
	//=====================================================================

	inline uint32_t GetBAMR() { return bamr; }
	inline void SetBAMR(uint32_t value) { bamr = value; }

	inline uint32_t GetICTRL() { return ictrl; }
	inline void SetICTRL(uint32_t value) { ictrl = (ictrl & ~CONFIG::ICTRL_MASK) | (value & CONFIG::ICTRL_MASK); }
	inline uint32_t GetICTRL_CIRQ() { return (GetICTRL() & CONFIG::ICTRL_CIRQ_MASK) >> CONFIG::ICTRL_CIRQ_OFFSET; }
	inline uint32_t GetICTRL_EIEC() { return (GetICTRL() & CONFIG::ICTRL_EIEC_MASK) >> CONFIG::ICTRL_EIEC_OFFSET; }
	inline uint32_t GetICTRL_EDCE() { return (GetICTRL() & CONFIG::ICTRL_EDCE_MASK) >> CONFIG::ICTRL_EDCE_OFFSET; }
	inline uint32_t GetICTRL_EICP() { return (GetICTRL() & CONFIG::ICTRL_EICP_MASK) >> CONFIG::ICTRL_EICP_OFFSET; }
	inline uint32_t GetICTRL_ICWL() { return (GetICTRL() & CONFIG::ICTRL_ICWL_MASK) >> CONFIG::ICTRL_ICWL_OFFSET; }
	inline void SetICTRL_CIRQ() { SetICTRL(GetICTRL() | CONFIG::ICTRL_CIRQ_MASK); }
	inline void ResetICTRL_CIRQ() { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_CIRQ_MASK); }
	inline void SetICTRL_EIEC() { SetICTRL(GetICTRL() | CONFIG::ICTRL_EIEC_MASK); }
	inline void ResetICTRL_EIEC() { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_EIEC_MASK); }
	inline void SetICTRL_EDCE() { SetICTRL(GetICTRL() | CONFIG::ICTRL_EDCE_MASK); }
	inline void ResetICTRL_EDCE() { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_EDCE_MASK); }
	inline void SetICTRL_EICP() { SetICTRL(GetICTRL() | CONFIG::ICTRL_EICP_MASK); }
	inline void ResetICTRL_EICP() { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_EICP_MASK); }
	inline void SetICTRL_ICWL() { SetICTRL(GetICTRL() | CONFIG::ICTRL_ICWL_MASK); }
	inline void ResetICTRL_ICWL() { SetICTRL(GetICTRL() & ~CONFIG::ICTRL_ICWL_MASK); }

	inline uint32_t GetLDSTCR() { return ldstcr; }
	inline void SetLDSTCR(uint32_t value) { ldstcr = value; }
	inline uint32_t GetMSSCR0() { return msscr0; }
	inline void SetMSSCR0(uint32_t value) { msscr0 = value; }
	inline uint32_t GetMSSSR0() { return msssr0; }
	inline void SetMSSSR0(uint32_t value) { msssr0 = value; }
	inline uint32_t GetPIR() { return pir; }
	inline void SetPIR(uint32_t value) { pir = value; }
	inline uint32_t GetTLBMISS() { return tlbmiss; }
	inline void SetTLBMISS(uint32_t value) { tlbmiss = value; }

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
	uint32_t tbl;           //!< Time base register (lower 32-bits) (603e, 604e, 7xx, 7xxx)
	uint32_t tbu;           //!< Time base register (upper 32-bits) (603e, 604e, 7xx, 7xxx)

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
	//=              PowerPC hardware interrupt signals                   =
	//=====================================================================
	
	unsigned int irq;

	//=====================================================================
	//=                    Exception handling methods                     =
	//=====================================================================
	
	// System reset exception
	void HandleException(const SystemResetException<CONFIG>& exc);

	// Machine check exception
	void HandleException(const MachineCheckException<CONFIG>& exc);

	// Decrementer exception
	void HandleException(const DecrementerException<CONFIG>& exc);

	// External interrupt exception
	void HandleException(const ExternalInterruptException<CONFIG>& exc);

	// Performance monitor interrupt exception
	void HandleException(const PerformanceMonitorInterruptException<CONFIG>& exc);

	// System management interrupt exception
	void HandleException(const SystemManagementInterruptException<CONFIG>& exc);

	// Instruction address breakpoint exception
	void HandleException(const InstructionAddressBreakpointException<CONFIG>& exc);

	// ISI exception
	void HandleException(const ISIException<CONFIG>& exc);

	// DSI exception
	void HandleException(const DSIException<CONFIG>& exc);
	
	// Alignment exception
	void HandleException(const AlignmentException<CONFIG>& exc, uint32_t instruction_encoding);

	// Program exceptions
	void HandleException(const ProgramException<CONFIG>& exc);

	// Floating point unavailable exception
	void HandleException(const FloatingPointUnavailableException<CONFIG>& exc);

	// System call exception
	void HandleException(const SystemCallException<CONFIG>& exc);

	// Trace exception
	void HandleException(const TraceException<CONFIG>& exc);

	// TLB exception
	void HandleException(const TLBMissException<CONFIG>& exc);

	//=====================================================================
	//=                          FPSCR generation                         =
	//=====================================================================

	// Compute the FPSCR bits
	void ComputeFPSCR_FEX(Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_FPRF(const SoftFloat& result, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_FPRF(const SoftDouble& result, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_FI(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_OX(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_UX(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_ZX(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_VXSNAN(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_VXISI(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_VXIDI(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_VXZDZ(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_VXIMZ(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_FR(const SoftFloat& result, const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_VXCVI(const Flags& flags, Instruction<CONFIG> *insn = 0);
	void ComputeFPSCR_FR(const SoftDouble& result, const Flags& flags, Instruction<CONFIG> *insn = 0);

	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_cpu_cycle_time;             //!< linked to member cpu_cycle_time
	Parameter<uint64_t> param_voltage;                    //!< linked to member voltage
	//Parameter<uint64_t> param_bus_cycle_time;             //!< linked to member bus_cycle_time
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
#if 0
	Statistic<uint64_t> stat_cpu_cycle;                   //!< Number of cpu cycles
#endif
	Statistic<uint64_t> stat_bus_cycle;                   //!< Number of front side bus cycles
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
#if 0
	Formula<double> formula_insn_per_bus_cycle;
#endif

protected:
	//=====================================================================
	//=                       Performance model                           =
	//=====================================================================

	class InstructionFreeListConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 256;
	};

	class OperandFreeListConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operand<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 256;
	};

	class LoadStoreAccessFreeListConfig
	{
	public:
		static const bool DEBUG = true;
		typedef LoadStoreAccess<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 256;
	};

	class BusAccessFreeListConfig
	{
	public:
		static const bool DEBUG = true;
		typedef BusAccess<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 256;
	};

	class InstructionQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::IQ_SIZE;
	};
	
	class VRIssueQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::VIQ_SIZE;
	};
	
	class GPRIssueQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::GIQ_SIZE;
	};
	
	class FPRIssueQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::FIQ_SIZE;
	};
	
	class CompletionQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::CQ_SIZE;
	};

	class GPRMappingTableConfig
	{
	public:
		static const bool DEBUG = true;
		static const unsigned int NUM_LOGICAL_REGISTERS = 32;
		static const unsigned int NUM_RENAME_REGISTERS = CONFIG::NUM_GPR_RENAME_REGISTERS;
	};
	
	class FPRMappingTableConfig
	{
	public:
		static const bool DEBUG = true;
		static const unsigned int NUM_LOGICAL_REGISTERS = 32;
		static const unsigned int NUM_RENAME_REGISTERS = CONFIG::NUM_FPR_RENAME_REGISTERS;
	};

	class CRMappingTableConfig
	{
	public:
		static const bool DEBUG = true;
		static const unsigned int NUM_LOGICAL_REGISTERS = 1;
		static const unsigned int NUM_RENAME_REGISTERS = 1;
	};

	class LRMappingTableConfig
	{
	public:
		static const bool DEBUG = true;
		static const unsigned int NUM_LOGICAL_REGISTERS = 1;
		static const unsigned int NUM_RENAME_REGISTERS = 1;
	};

	class CTRMappingTableConfig
	{
	public:
		static const bool DEBUG = true;
		static const unsigned int NUM_LOGICAL_REGISTERS = 1;
		static const unsigned int NUM_RENAME_REGISTERS = 1;
	};

	class IU1ReservationStationConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::IU1_RS_SIZE;
	};

	class IU1PipelineConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::IU1_PIPELINE_DEPTH > 1 ? CONFIG::IU1_PIPELINE_DEPTH - 1 : 1;
	};

	class IU2ReservationStationConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::IU2_RS_SIZE;
	};

	class IU2PipelineConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::IU2_PIPELINE_DEPTH > 1 ? CONFIG::IU2_PIPELINE_DEPTH - 1 : 1;
	};

	class FPUReservationStationConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::FPU_RS_SIZE;
	};

	class FPUPipelineConfig
	{
	public:
		static const bool DEBUG = true;
		typedef  Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::FPU_PIPELINE_DEPTH > 1 ? CONFIG::FPU_PIPELINE_DEPTH - 1 : 1;
	};

	class LSUReservationStationConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::LSU_RS_SIZE;
	};

	class FinishedStoreQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::FSQ_SIZE;
	};

	class CommittedStoreQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::CSQ_SIZE;
	};

	class RequiredEffectiveAddressConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = 1;
	};

	class BusStoreQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef BusAccess<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::BSQ_SIZE;
	};

	class BusLoadQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef BusAccess<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::MAX_OUTSTANDING_L2_LOAD_MISS;
	};

	class WriteBackQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Instruction<CONFIG> *ELEMENT;
		static const unsigned int SIZE = CONFIG::COMPLETE_WIDTH;
	};

	// Instruction Queue
	Queue<InstructionQueueConfig> iq;

	// Vector Issue Queue
	Queue<VRIssueQueueConfig> viq;

	// General Purpose Issue Queue
	Queue<GPRIssueQueueConfig> giq;

	// Floating point Issue Queue
	Queue<FPRIssueQueueConfig> fiq;

	// Completion Queue
	Queue<CompletionQueueConfig> cq;

	// Mapping tables
	MappingTable<GPRMappingTableConfig> gpr_mapping_table;
	MappingTable<FPRMappingTableConfig> fpr_mapping_table;
	MappingTable<CRMappingTableConfig> cr_mapping_table;
	MappingTable<LRMappingTableConfig> lr_mapping_table;
	MappingTable<CTRMappingTableConfig> ctr_mapping_table;

	// Reservation stations
	Queue<IU1ReservationStationConfig> iu1_reservation_station[CONFIG::NUM_IU1];
	Queue<IU2ReservationStationConfig> iu2_reservation_station[CONFIG::NUM_IU2];
	Queue<FPUReservationStationConfig> fpu_reservation_station[CONFIG::NUM_FPU];
	Queue<LSUReservationStationConfig> lsu_reservation_station;

	// Integer/Floating Units Pipelines
	Queue<IU1PipelineConfig> iu1_pipeline[CONFIG::NUM_IU1];
	Queue<IU2PipelineConfig> iu2_pipeline[CONFIG::NUM_IU2];
	Queue<FPUPipelineConfig> fpu_pipeline[CONFIG::NUM_FPU];

	// Effective Address
	Queue<RequiredEffectiveAddressConfig> required_ea;

	// Finished Store Queue
	Queue<FinishedStoreQueueConfig> fsq;

	// Committed Store Queue
	Queue<CommittedStoreQueueConfig> csq;

	// Bus Load Queue
	Queue<BusLoadQueueConfig> blq;

	// Bus Store Queue
	Queue<BusStoreQueueConfig> bsq;

	// Write Back Cycle 0
	Queue<WriteBackQueueConfig> wb0;

	// Write Back Cycle 1
	Queue<WriteBackQueueConfig> wb1;

	// Schedules
	std::multimap<uint64_t, Event<CONFIG> *> schedule;

	Event<CONFIG> *event_free_list;
	Instruction<CONFIG> *insn_free_list;
	Operand<CONFIG> *operand_free_list;
	BusAccess<CONFIG> *bus_access_free_list;
	LoadStoreAccess<CONFIG> *load_store_access_free_list;
	FetchAccess<CONFIG> *fetch_access_free_list;

	Event<CONFIG> *AllocateEvent();
	Instruction<CONFIG> *AllocateInstruction();
	Operand<CONFIG> *AllocateOperand();
	LoadStoreAccess<CONFIG> *AllocateLoadStoreAccess();
	void GenLoadStoreAccess(typename LoadStoreAccess<CONFIG>::Type type, unsigned int reg_num, typename CONFIG::address_t munged_ea, uint32_t size, Instruction<CONFIG> *instruction);
	BusAccess<CONFIG> *AllocateBusAccess();
	FetchAccess<CONFIG> *AllocateFetchAccess();
	void AcquireOperand(Operand<CONFIG> *operand);
	void FreeEvent(Event<CONFIG> *ev);
	void FreeInstruction(Instruction<CONFIG> *instruction);
	void FreeOperand(Operand<CONFIG> *operand);
	void FreeLoadStoreAccess(LoadStoreAccess<CONFIG> *load_store_access);
	void FreeBusAccess(BusAccess<CONFIG> *bus_access);
	void FreeFetchAccess(FetchAccess<CONFIG> *fetch_access);

	// Sequential CIA
	uint32_t seq_cia;
	
	// Architectural registers
	Operand<CONFIG> *arch_gpr[GPRMappingTableConfig::NUM_LOGICAL_REGISTERS];
	Operand<CONFIG> *arch_fpr[FPRMappingTableConfig::NUM_LOGICAL_REGISTERS];
	Operand<CONFIG> *arch_cr;
	Operand<CONFIG> *arch_lr;
	Operand<CONFIG> *arch_ctr;
	Operand<CONFIG> *arch_xer;
	Operand<CONFIG> *arch_fpscr;

	// Rename registers
	Operand<CONFIG> *rename_gpr[GPRMappingTableConfig::NUM_RENAME_REGISTERS];
	Operand<CONFIG> *rename_fpr[FPRMappingTableConfig::NUM_RENAME_REGISTERS];
	Operand<CONFIG> *rename_cr[CRMappingTableConfig::NUM_RENAME_REGISTERS];
	Operand<CONFIG> *rename_lr[LRMappingTableConfig::NUM_RENAME_REGISTERS];
	Operand<CONFIG> *rename_ctr[CTRMappingTableConfig::NUM_RENAME_REGISTERS];

	uint32_t num_outstanding_l1_fetch_miss;
	uint32_t num_outstanding_l1_load_miss;
	uint32_t num_outstanding_l1_store_miss;
	uint32_t num_outstanding_l2_load_miss;
	uint32_t num_outstanding_l2_store_miss;
	unsigned int dispatch_uop_num;
	unsigned int load_store_access_num;

	void ScheduleEvents();

	void Fetch();
	void DecodeDispatch();
	void GPRIssue();
	void FPRIssue();
	void VRIssue();
	void IU1Execute();
	void IU2Execute();
	void FPUExecute();
	void LSUExecute1();
	void LSUExecute2();
	void BIU();
	void Complete();
	void WriteBack0();
	void WriteBack1();
	void Flush();
	bool HasPipelineCollision(BusAccess<CONFIG> *bus_load_access);

	void NotifyEvent(Event<CONFIG> *ev, uint64_t latency);
	void NotifyLoadResultAvailability(LoadStoreAccess<CONFIG> *load_store_access, uint64_t latency);
	void NotifyFinishedFetch(FetchAccess<CONFIG> *fetch_access, uint64_t latency);
	void NotifyOperandAvailability(Operand<CONFIG> *operand, uint64_t latency);
	void NotifyFinishedInstruction(Instruction<CONFIG> *operand, uint64_t latency);
	void NotifyBusAccess(BusAccess<CONFIG> *bus_access, uint64_t latency);
	void NotifyWriteBack(Operand<CONFIG> *operand, uint64_t latency);
	void NotifyStoreWriteBack(LoadStoreAccess<CONFIG> *load_store_access, uint64_t latency);

	void OnFinishedBusAccess(BusAccess<CONFIG> *bus_access);
	virtual void DoBusAccess(BusAccess<CONFIG> *bus_access) {}

	void DumpPipeline();
	void DumpSchedule();
	
	friend class Instruction<CONFIG>;
};

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
