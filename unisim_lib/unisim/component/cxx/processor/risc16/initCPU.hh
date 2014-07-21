
#ifndef __initCPU_
#define __initCPU_

#include <iostream>
#include <fstream>

#include <inttypes.h>
#include <stdio.h>

#include "unisim/kernel/logger/logger.hh"
#include <unisim/kernel/service/service.hh>

#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include <unisim/service/interfaces/memory.hh>
#include "unisim/service/interfaces/registers.hh"

using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Registers;

using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceExportBase;

typedef uint32_t PHYSICAL_ADDR;

using namespace std;

class CPU :
	 public Client<DebugControl<PHYSICAL_ADDR> >
	, public Client<MemoryAccessReporting<PHYSICAL_ADDR> >
	, public Service<MemoryAccessReportingControl>
	, public Service<Memory<PHYSICAL_ADDR> >
	, public Client<Memory<PHYSICAL_ADDR> >
	, public Service<Disassembly<PHYSICAL_ADDR> >

{

public:
	static const int MAX_INS_SIZE = 2;

	ServiceExport<Memory<PHYSICAL_ADDR> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<DebugControl<PHYSICAL_ADDR> > debug_control_import;
	ServiceImport<MemoryAccessReporting<PHYSICAL_ADDR> > memory_access_reporting_import;
	ServiceImport<Memory<PHYSICAL_ADDR> > memory_import;

	ServiceImport<TrapReporting > trap_reporting_import;
	ServiceExport<Disassembly<PHYSICAL_ADDR> > disasm_export;

	ServiceExport<Registers> registers_export;

	/** indicates if the memory accesses require to be reported */
	bool requires_memory_access_reporting;
	Parameter<bool> param_requires_memory_access_reporting;

	/** indicates if the finished instructions require to be reported */
	bool requires_finished_instruction_reporting;
	Parameter<bool> param_requires_finished_instruction_reporting;

public:

    	CPU(const char* name, Object *_parent=0);
    	virtual ~CPU();

    	virtual uint16_t fetch(uint16_t addr) = 0;

    	virtual void mem_write16(uint16_t addr,uint16_t data) = 0;

    	virtual uint16_t mem_read16(uint16_t addr) = 0;

		int generateBinary(const char* fname);

		void mem_loader(const char* fname,PHYSICAL_ADDR addr);

		void rti();

		void handleinterrupt();

		int Step();

		//=====================================================================
		//=                  Client/Service setup methods                     =
		//=====================================================================

		virtual bool BeginSetup();
		virtual bool Setup(ServiceExportBase *srv_export);
		virtual bool EndSetup();

		virtual void OnDisconnect();

		//=====================================================================
		//=             memory access reporting control interface methods     =
		//=====================================================================

		virtual void RequiresMemoryAccessReporting(bool report);
		virtual void RequiresFinishedInstructionReporting(bool report);

		//=====================================================================
		//=             memory interface methods                              =
		//=====================================================================

		virtual void Reset();

		virtual bool ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size);
		virtual bool WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size);

		//=====================================================================
		//=                   DebugDisasmInterface methods                    =
		//=====================================================================

		/**
		 * Returns a string with the disassembling of the instruction found
		 *   at address addr.
		 *
		 * @param addr The address of the instruction to disassemble.
		 * @param next_addr The address following the requested instruction.
		 * @return The disassembling of the requested instruction address.
		 */
		virtual string Disasm(PHYSICAL_ADDR service_addr, PHYSICAL_ADDR &next_addr);

		uint16_t cia;
		uint16_t nia;
		uint16_t gpr[16];

		uint16_t load_address;
		Parameter<uint16_t> param_load_address;

		uint16_t entry_point;
		Parameter<uint16_t> param_entry_point;

protected:
		uint16_t received_vect;
		bool has_interrupt;

private:
		std::ofstream tracefile;

		// the kernel logger
		unisim::kernel::logger::Logger *logger;

		std::vector<unisim::kernel::service::VariableBase*> extended_registers_registry;

};


#endif
