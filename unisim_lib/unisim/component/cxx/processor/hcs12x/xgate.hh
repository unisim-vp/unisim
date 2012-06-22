/*
 *  Copyright (c) 2012,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_XGATE_HH_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_XGATE_HH_

#include <map>
#include <iostream>
#include <string>

#include <inttypes.h>
#include <stdlib.h>

#include "unisim/kernel/service/service.hh"

#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"

#include "unisim/kernel/logger/logger.hh"

#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/debug/register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/util/endian/endian.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/mmc.hh>
#include <unisim/component/cxx/processor/hcs12x/exception.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace s12xgate {

using std::string;
using std::stringstream;
using std::map;
using std::ostream;
using std::vector;

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::CallBackObject;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;

using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;

using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Register;
using unisim::util::debug::Symbol;
using unisim::util::endian::endian_type;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::MMC;
using unisim::component::cxx::processor::hcs12x::MMC_DATA;
using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::component::cxx::processor::hcs12x::Exception;
using unisim::component::cxx::processor::hcs12x::AsynchronousException;

#define MAX_INS_SIZE 2

class XGATE :
	public CallBackObject,
	public Client<DebugControl<physical_address_t> >,
	public Service<Registers>,
	public Service<Memory<physical_address_t> >,
	public Service<MemoryAccessReportingControl>,
	public Client<Memory<physical_address_t> >,
	public Client<MemoryAccessReporting<physical_address_t> >,
	public Client<SymbolTableLookup<physical_address_t> >,
	public Client<TrapReporting >

{
public:

	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================

		ServiceExport<Registers> registers_export;
		ServiceExport<Memory<physical_address_t> > memory_export;
		ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

		ServiceImport<DebugControl<physical_address_t> > debug_control_import;
		ServiceImport<MemoryAccessReporting<physical_address_t> > memory_access_reporting_import;
		ServiceImport<Memory<physical_address_t> > memory_import;
		ServiceImport<TrapReporting > trap_reporting_import;
		ServiceImport<SymbolTableLookup<physical_address_t> > symbol_table_lookup_import;

		XGATE(const char *name, Object *parent);
		~XGATE();

		uint8_t step();	// Return the number of cpu cycles consumed by the operation
		virtual void Stop(int ret);
		virtual void Sync();

		virtual address_t getIntVector(uint8_t &ipl) = 0;
		virtual double  GetSimulatedTime() = 0;

		virtual void busWrite(address_t addr, const void *buffer, uint32_t size) = 0;
		virtual void busRead(address_t addr, void *buffer, uint32_t size) = 0;

		inline void reportTrap();

		//==========================================
		//=     ISA - MEMORY ACCESS ROUTINES       =
		//==========================================

		inline uint8_t memRead8(address_t logicalAddress, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);
		inline void memWrite8(address_t logicalAddress,uint8_t val, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);

		inline uint16_t memRead16(address_t logicalAddress, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);
		inline void memWrite16(address_t logicalAddress,uint16_t val, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);

		inline void monitorStore(address_t logicalAddress, uint32_t size, bool isGlobal);
		inline void monitorLoad(address_t logicalAddress, uint32_t size, bool isGlobal);

		//=====================================================================
		//=                  Memory Access Reportings methods                 =
		//=====================================================================

		virtual void RequiresMemoryAccessReporting(bool report);
		virtual void RequiresFinishedInstructionReporting(bool report) ;

		//=====================================================================
		//=                  Client/Service setup methods                     =
		//=====================================================================

		virtual bool BeginSetup();
		virtual bool Setup(ServiceExportBase *srv_export);
		virtual bool EndSetup();

		virtual void OnDisconnect();
		virtual void Reset();


		//=====================================================================
		//=             memory interface methods                              =
		//=====================================================================

		virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
		virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

		//=====================================================================
		//=             Registers Interface methods                           =
		//=====================================================================

		/**
		 * Gets a register interface to the register specified by name.
		 *
		 * @param name The name of the requested register.
		 * @return A pointer to the RegisterInterface corresponding to name.
		 */
	    virtual Register *GetRegister(const char *name);


		//=====================================================================
		//=             Internal Registers access methods                     =
		//=====================================================================


		//=====================================================================
		//=             memory map access methods                             =
		//=====================================================================
		virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length);
		virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length);

		//=====================================================================
		//=                   Debugging methods                               =
		//=====================================================================

		string getObjectFriendlyName(physical_address_t addr);
		string getFunctionFriendlyName(physical_address_t addr);
		inline uint64_t getInstructionCounter() const { return (instruction_counter); }
		inline bool isVerboseException() const { return (debug_enabled && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception)); }

		inline void setRegPC(address_t val) { xgpc_register = val; }
		inline address_t getRegPC() { return (xgpc_register); }
		inline address_t getLastPC() {return (lastPC); }

protected:

	// the kernel logger
	unisim::kernel::logger::Logger *logger;

	//=====================================================================
	//=              XGATE Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================

	uint64_t cpu_cycle_time; //!< XGATE cycle time in ps
	uint64_t voltage;        //!< XGATE voltage in mV
	uint64_t bus_cycle_time; //!< Front side bus cycle time in ps
	uint64_t cpu_cycle;      //!< Number of cpu cycles
	uint64_t bus_cycle;      //!< Number of front side bus cycles

	//utils attributes
	bool verbose_all;
	Parameter<bool> param_verbose_all;
	bool verbose_setup;
	Parameter<bool> param_verbose_setup;
	bool verbose_step;
	Parameter<bool> param_verbose_step;
	bool verbose_dump_regs_start;
	Parameter<bool> param_verbose_dump_regs_start;
	bool verbose_dump_regs_end;
	Parameter<bool> param_verbose_dump_regs_end;
	bool verbose_exception;
	Parameter<bool> param_verbose_exception;

	bool trace_enable;
	Parameter<bool> param_trace_enable;

	uint64_t trap_on_instruction_counter;
	Parameter<uint64_t> param_trap_on_instruction_counter;

	/** indicates if the memory accesses require to be reported */
	bool requires_memory_access_reporting;
	Parameter<bool> param_requires_memory_access_reporting;

	/** indicates if the finished instructions require to be reported */
	bool requires_finished_instruction_reporting;
	Parameter<bool> param_requires_finished_instruction_reporting;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	// verbose methods
	inline bool VerboseSetup() GCC_INLINE;
	inline bool VerboseStep() GCC_INLINE;
	inline void VerboseDumpRegs() GCC_INLINE;
	inline void VerboseDumpRegsStart() GCC_INLINE;
	inline void VerboseDumpRegsEnd() GCC_INLINE;

	void reqAsynchronousInterrupt();

private:

		// Registers map
		std::map<std::string, Register *> registers_registry;
		std::vector<unisim::kernel::service::VariableBase*> extended_registers_registry;

		bool asynchronous_interrupt;

		uint8_t				interrupt_software_trigger_base;
		Parameter<uint8_t>	param_interrupt_software_trigger_base;
		uint8_t				interrupt_software_error;
		Parameter<uint8_t>	param_interrupt_software_error;

		uint64_t	max_inst;
		Parameter<uint64_t>	   param_max_inst;

		/** the instruction counter */
		uint64_t instruction_counter;
		Statistic<uint64_t> stat_instruction_counter;
		uint64_t cycles_counter;
		Statistic<uint64_t> stat_cycles_counter;
		uint64_t data_load_counter;
		Statistic<uint64_t> stat_load_counter;
		uint64_t data_store_counter;
		Statistic<uint64_t> stat_store_counter;

		//===============================================================
		//=         XGATE Memory Map                                    =
		//===============================================================

		uint8_t xgchid_register, reserverd_register[6], xgccr_register;
		uint16_t xgmctl_register, xgif_register[8], xgvbr_register, xgswt_register, xgsemm_register, xgpc_register, xgr_register[8];

		address_t lastPC;

		void fetchInstruction(address_t addr, uint8_t* ins, uint8_t nByte);

		inline bool hasAsynchronousInterrupt() const { return (asynchronous_interrupt); }

		// Asynchronous Interrupts (including Resets, I-bit, XIRQ, IRQ)
		void handleException(const AsynchronousException& exc);

public:
		//=========================================================
		//=                REGISTERS OFFSETS                      =
		//=========================================================

		static const unsigned int XGMCTL = 0; //2-bytes
		static const unsigned int XGCHID = 2; // 1-byte
		static const unsigned int RESERVED1 = 3; // 1-byte
		static const unsigned int RESERVED2 = 4; // 1-byte
		static const unsigned int RESERVED3 = 5; // 1-byte
		static const unsigned int XGVBR = 6; // 2-bytes
		static const unsigned int XGIF_7F_70 = 8; // 2-bytes
		static const unsigned int XGIF_6F_60 = 10; // 2-bytes
		static const unsigned int XGIF_5F_50 = 12; // 2-bytes
		static const unsigned int XGIF_4F_40 = 14; // 2-bytes
		static const unsigned int XGIF_3F_30 = 16; // 2-bytes
		static const unsigned int XGIF_2F_20 = 18; // 2-bytes
		static const unsigned int XGIF_1F_10 = 20; // 2-bytes
		static const unsigned int XGIF_0F_00 = 22; // 2-bytes
		static const unsigned int XGSWT = 24; // 2-bytes
		static const unsigned int XGSEMM = 26; // 2-bytes
		static const unsigned int RESERVED4 = 28; // 1-byte
		static const unsigned int XGCCR = 29; // 1-byte
		static const unsigned int XGPC = 30; // 2-bytes
		static const unsigned int RESERVED5 = 32; // 1-byte
		static const unsigned int RESERVED6 = 33; // 1-byte
		static const unsigned int XGR1 = 34; // 2-bytes
		static const unsigned int XGR2 = 36; // 2-bytes
		static const unsigned int XGR3 = 38; // 2-bytes
		static const unsigned int XGR4 = 40; // 2-bytes
		static const unsigned int XGR5 = 42; // 2-bytes
		static const unsigned int XGR6 = 44; // 2-bytes
		static const unsigned int XGR7 = 46; // 2-bytes
		static const unsigned int XGINTERNAL = 48; // 8*2-bytes internal registers not visible outside XGATE

};

inline uint8_t XGATE::memRead8(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal) {

	uint8_t data;
	MMC_DATA mmc_data;

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 1;

	busRead(logicalAddress, &mmc_data, sizeof(MMC_DATA));

	monitorLoad(logicalAddress, sizeof(data), isGlobal);

	return (data);
}

inline uint16_t XGATE::memRead16(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal) {

	uint16_t data;
	MMC_DATA mmc_data;

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 2;

	busRead(logicalAddress, &mmc_data, sizeof(MMC_DATA));

	data = BigEndian2Host(data);

	monitorLoad(logicalAddress, sizeof(data), isGlobal);

	return (data);
}

inline void XGATE::memWrite8(address_t logicalAddress,uint8_t data, ADDRESS::MODE type, bool isGlobal) {

	MMC_DATA mmc_data;

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 1;

	busWrite( logicalAddress, &mmc_data, sizeof(MMC_DATA));

	monitorStore(logicalAddress, sizeof(data), isGlobal);

}

inline void XGATE::memWrite16(address_t logicalAddress,uint16_t data, ADDRESS::MODE type, bool isGlobal) {

	MMC_DATA mmc_data;

	data = Host2BigEndian(data);

	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 2;

	busWrite( logicalAddress, &mmc_data, sizeof(MMC_DATA));

	monitorStore(logicalAddress, sizeof(data), isGlobal);

}

// ==============================================
// =          MEMORY ACCESS ROUTINES            =
// ==============================================

inline void XGATE::monitorLoad(address_t logicalAddress, uint32_t size, bool isGlobal)
{
	physical_address_t pea = MMC::getCPU12XPhysicalAddress(logicalAddress, ADDRESS::EXTENDED,isGlobal,false, 0x00);

	data_load_counter++;

	// Memory access reporting
	if(memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<physical_address_t>::MAT_READ, MemoryAccessReporting<physical_address_t>::MT_DATA, pea, size);
	}
}

inline void XGATE::monitorStore(address_t logicalAddress, uint32_t size, bool isGlobal)
{
	physical_address_t pea = MMC::getCPU12XPhysicalAddress(logicalAddress, ADDRESS::EXTENDED,isGlobal,false, 0x00);

	data_store_counter++;

	// Memory access reporting
	if(memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<physical_address_t>::MAT_WRITE, MemoryAccessReporting<physical_address_t>::MT_DATA, pea, size);
	}
}

inline void XGATE::reportTrap() {
	if (trap_reporting_import) {
		trap_reporting_import->ReportTrap();
	}

}

} // end of namespace s12xgate
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_XGATE_HH_ */
