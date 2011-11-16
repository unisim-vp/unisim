/*
 * s12xeetx.hh
 *
 *  Created on: 10 oct. 2011
 *      Author: rnouacer
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_

#include <queue>

#include <systemc.h>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/registers.hh"

#include "unisim/util/debug/register.hh"
#include "unisim/util/debug/simple_register.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include "unisim/component/tlm2/memory/ram/memory.hh"
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::VariableBase;
using unisim::kernel::logger::Logger;

using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::service::interfaces::Registers;

using unisim::util::debug::Register;
using unisim::util::debug::SimpleRegister;

using unisim::component::tlm2::memory::ram::DEFAULT_BURST_LENGTH;
using unisim::component::tlm2::memory::ram::DEFAULT_BUSWIDTH;
using unisim::component::tlm2::memory::ram::DEFAULT_DEBUG;
using unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE;
using unisim::component::tlm2::memory::ram::DEFAULT_ADDRESS;

using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

#define DEFAULT_CMD_PIPELINE_SIZE 2
#define SECTOR_SIZE 4
#define WORD_SIZE 2

template <unsigned int CMD_PIPELINE_SIZE = DEFAULT_CMD_PIPELINE_SIZE, unsigned int BUSWIDTH = DEFAULT_BUSWIDTH, class ADDRESS = DEFAULT_ADDRESS, unsigned int BURST_LENGTH = DEFAULT_BURST_LENGTH, uint32_t PAGE_SIZE = DEFAULT_PAGE_SIZE, bool DEBUG = DEFAULT_DEBUG>
class S12XEETX :
	public unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>

{
public:
	typedef unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG> inherited;


	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {ECLKDIV, RESERVED1, RESERVED2, ECNFG, EPROT, ESTAT, ECMD, RESERVED3, EADDRHI, EADDRLO, EDATAHI, EDATALO};

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12XEETX> bus_clock_socket;

	// interface with bus
	tlm_utils::simple_target_socket<S12XEETX> slave_socket;

	/**
	 * Constructor.
	 *
	 * @param name the name of the module
	 * @param parent the parent service
	 */
	S12XEETX(const sc_module_name& name, Object *parent = 0);
	/**
	 * Destructor
	 */
	virtual ~S12XEETX();

	void assertInterrupt(uint8_t interrupt_offset);

	void Process();
	void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	sc_time getBusClock() { return bus_cycle_time; }

	void setEEPROMClock();

	void abort_write_sequence() {

		// remove the last command from the pipeline to abort command write sequence

		queue<TCommand*> tmpQueue;
		while (!cmd_queue.empty()) {
			TCommand * el = cmd_queue.front();
			cmd_queue.pop();
			if (el != cmd_queue_back) {
				tmpQueue.push(el);
			} else {
				if (!tmpQueue.empty()) {
					cmd_queue_back = tmpQueue.back();
				} else {
					cmd_queue_back = NULL;
				}
			}
		}

		while (!tmpQueue.empty()) {
			TCommand * el = tmpQueue.front();
			tmpQueue.pop();
			cmd_queue.push(el);
		}
	};

	void erase_verify_cmd() {
		/**
		 * Verify all memory bytes in the EEPROM block are erased.
		 * If the EEPROM block is erased,
		 * the BLANK flag in the ESTAT register will set upon command completion
		 */
		wait(bus_cycle_time * (inherited::bytesize/2 + 14));

		// is all words erased successfully ?
		if ((rand() % 101) > (erase_fail_ratio * 100)) {
			// set ESTAT::BLANK
			estat_reg = estat_reg | 0x04;
		}
	}
	void word_program_cmd() {
		/**
		 * Program a word (two bytes) in the EEPROM block
		 */

		/**
		 * EPROT::EPDIS = 0 protection enabled
		 * EPROT::EPDIS = 1 protection disabled
		 */
		bool protection_enabled = ((eprot_reg & 0x04) == 0);

		if (protection_enabled && ((eaddr_reg + WORD_SIZE - 1) >= protected_area_start_address)) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Try to word program at protected area @ 0x" << std::hex << eaddr_reg
				<< std::endl << EndDebugWarning;

			// Set ESTAT::PVIOL flag
			estat_reg = estat_reg | 0x20;
			return;
		}


		/**
		 * is aligned word ?
		 * Writing a byte or misaligned word to a valid EEPROM address is illegal and ACCERR flag is set.
		 */

		WriteMemory(inherited::hi_addr + eaddr_reg, &edata_reg, 2);

		wait(bus_cycle_time * (1 + 14));

	}
	void sector_erase_cmd() {
		/**
		 * Erase all four memory bytes in a sector of the EEPROM block
		 */

		/**
		 * EPROT::EPDIS = 0 protection enabled
		 * EPROT::EPDIS = 1 protection disabled
		 */
		bool protection_enabled = ((eprot_reg & 0x04) == 0);

		if (protection_enabled && ((eaddr_reg + SECTOR_SIZE - 1) >= protected_area_start_address)) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Try to erase sector at protected area.@ 0x" << std::hex << eaddr_reg
				<< std::endl << EndDebugWarning;

			// Set ESTAT::PVIOL flag
			estat_reg = estat_reg | 0x20;
			return;
		}

		sector_erase_modify_active = true;

		uint8_t data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

		for (uint8_t i=0; i<4; i++) {

			/**
			 * If the sector erase abort command is launched resulting in the early termination of an active sector erase or scetor modify operation, the ACCERR flag will set once the operation completes as indicated by the CCIF flag being set.
			 * Else if the sector erase or modify is completed before then ACCERR isn't set
			 */

			if (sector_erase_abort_command_req) {
				// set the ACCERR flag
				setACCERR();
				break;
			}

			WriteMemory(inherited::hi_addr + (eaddr_reg & 0xFC) + i, &data[i], 1);

			wait(bus_cycle_time);
		}

		if (sector_erase_abort_command_req) {
			sector_erase_abort_command_req = false;
		}

		wait(bus_cycle_time * 14);

		sector_erase_modify_active = false;

	}
	void mass_erase_cmd() {
		/**
		 * Erase all memory bytes in the EEPROM block.
		 * A mass erase of the full EEPROM block is only possible when EPOPEN and EPDIS bits in the EPROT register are set prior launching the command.
		 */

		/**
		 * EPROT::EPDIS = 0 protection enabled
		 * EPROT::EPDIS = 1 protection disabled
		 */
		bool protection_enabled = ((eprot_reg & 0x04) == 0);

		if (protection_enabled) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Try to mass erase while EEPROM protection enabled." << std::endl << EndDebugWarning;

			// Set ESTAT::PVIOL flag
			estat_reg = estat_reg | 0x20;
			return;
		}

		wait(bus_cycle_time * (inherited::bytesize/2 + 14));

	}
	void sector_erase_abort_cmd() {
		/**
		 * Abort the sector erase operation.
		 * The sector erase operation will terminate according to set procedure.
		 * The EEPROM sector should not be considered erased if the ACCERR flag is set upon command completion.
		 */

		sector_erase_abort_command_req = false;

		sector_erase_abort_active = true;

		wait(bus_cycle_time * (1 + 14));

		sector_erase_abort_active = false;
	}
	void sector_modify_cmd() {
		/**
		 * Erase all four memory bytes in a sector of the EEPROM block and reprogram the addressed word.
		 */

		/**
		 * EPROT::EPDIS = 0 protection enabled
		 * EPROT::EPDIS = 1 protection disabled
		 */
		bool protection_enabled = ((eprot_reg & 0x04) == 0);

		if (protection_enabled && ((eaddr_reg + SECTOR_SIZE - 1) >= protected_area_start_address)) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Try to modify sector at protected area @ 0x" << std::hex << eaddr_reg
				<< std::endl << EndDebugWarning;

			// Set ESTAT::PVIOL flag
			estat_reg = estat_reg | 0x20;
			return;
		}

		sector_erase_modify_active = true;

		// erase both words in a sector
		uint8_t data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

		for (uint8_t i=0; i<4; i++) {

			/**
			 * If the sector erase abort command is launched resulting in the early termination of an active sector erase or scetor modify operation, the ACCERR flag will set once the operation completes as indicated by the CCIF flag being set.
			 * Else if the sector erase or modify is completed before then ACCERR isn't set
			 */

			if (sector_erase_abort_command_req) {
				// set the ACCERR flag
				setACCERR();
				break;
			}

			WriteMemory(inherited::hi_addr + (eaddr_reg & 0xFC) + i, &data[i], 1);

			wait(bus_cycle_time);
		}

		if (sector_erase_abort_command_req) {
			sector_erase_abort_command_req = false;
		} else {
			// program word at global_address while byte address bit 0 is ignored
			WriteMemory(inherited::hi_addr + (eaddr_reg & 0xFE), &edata_reg, 2);
		}

		wait(bus_cycle_time * 14);

		sector_erase_modify_active = false;

	}


	/* Service methods */
	/** BeginSetup
	 * Initializes the service interface. */
	virtual bool BeginSetup();

	virtual void Reset();

	/**
	 * TLM2 Slave methods
	 */
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

	void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             EEPROM Registers Interface interface methods               =
	//=====================================================================

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
    virtual Register *GetRegister(const char *name);

	//=====================================================================
	//=             registers setters and getters                         =
	//=====================================================================
	bool read(uint8_t offset, void *buffer, unsigned int data_length);
	bool write(uint8_t offset, const void *buffer, unsigned int data_length);


protected:

private:

	class  TCommand {
	public:
		TCommand() : cmd(0x00), addr(0x0000), data(0x0000), cmdWrite(false) { }
		~TCommand() {}

		void setCmd(uint8_t _cmd) { this->cmd = _cmd; cmdWrite = true; }
		uint8_t getCmd() { return cmd; }
		void setAddr(uint16_t _addr) { this->addr = _addr; }
		uint16_t getAddr() { return addr; }
		void setData(uint16_t _data) { this->data = _data; }
		uint16_t getData() { return data; }
		bool isCmdWrite() { return cmdWrite; }
		void invalidateCmdWrite() { cmdWrite = false; }

	private:
		bool    cmdWrite;
		uint8_t cmd;
		uint16_t addr;
		uint16_t data;
	} ;

	queue<TCommand*> cmd_queue;
	TCommand* cmd_queue_back;

	void write_to_eeprom(physical_address_t address, void* data_ptr, unsigned int data_length) {

		std::cerr << "E2P:: write to => 0x" << std::hex << address << "  length: " << data_length << endl;
		// Illegal operation: Writing to an EEPROM address before initializing the ECLKDIV register
		if ((eclkdiv_reg & 0x80) == 0) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Writing to an EEPROM address before initializing the ECLKDIV register is an illegal operation. " << std::endl << EndDebugWarning;

			setACCERR();
//			abort_write_sequence();
			return;
		}

		// Illegal operation: Writing a byte or misaligned word to a valid EEPROM address
		if ((data_length == 1) || ((address & 0x1) != 0)) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Writing a byte or misaligned word to a valid EEPROM address is an illegal operation " << data_length << " bytes @ 0x" << std::hex << address
				<< std::endl << EndDebugWarning;

			setACCERR();
//			abort_write_sequence();
			return;
		}

		// Illegal operation: Starting a command write sequence while a sector erase abort operation is active.
		if (sector_erase_abort_active) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Starting a command write sequence while a sector erase abort operation is active is an illegal operation " << std::endl << EndDebugWarning;

			setACCERR();
//			abort_write_sequence();
			return;
		}

		if ((cmd_queue_back != NULL) && !cmd_queue_back->isCmdWrite()) {
			cmd_queue_back->setAddr((uint16_t) (inherited::org - address) & 0x07FF);
			if (data_length == 2) {
				cmd_queue_back->setData((uint16_t) *((uint16_t*) data_ptr));
			} else {
				cmd_queue_back->setData((uint8_t) *((uint8_t*) data_ptr));
			}
		} else {
			if ((cmd_queue_back != NULL) && cmd_queue_back->isCmdWrite() && ((estat_reg & 0x80) != 0)) {
				inherited::logger << DebugWarning << " : " << inherited::name() << ":: Writing to an EEPROM address after writing to the ECMD register and before launching the command is an illegal operation " << std::endl << EndDebugWarning;

				setACCERR();
//				abort_write_sequence();
				return;
			}
			else if (cmd_queue.size() < CMD_PIPELINE_SIZE) {
				TCommand * command = new TCommand();
				command->setAddr((uint16_t) (inherited::org - address) & 0x07FF);
				if (data_length == 2) {
					command->setData((uint16_t) *((uint16_t*) data_ptr));
				} else {
					command->setData((uint8_t) *((uint8_t*) data_ptr));
				}

				cmd_queue_back = command;
				cmd_queue.push(command);
			} else {
				cmd_queue_back->setAddr((uint16_t) (inherited::org - address) & 0x07FF);
				if (data_length == 2) {
					cmd_queue_back->setData((uint16_t) *((uint16_t*) data_ptr));
				} else {
					cmd_queue_back->setData((uint8_t) *((uint8_t*) data_ptr));
				}
				cmd_queue_back->invalidateCmdWrite();

				inherited::logger << DebugWarning << " : " << inherited::name() << ":: Command Write Sequence Lost/overwritten! " << std::endl << EndDebugWarning;

			}
		}

		if (((estat_reg & 0x80) ==0) && (cmd_queue.size() == 1)) {
			/**
			 * If  the CBEIF flag in the ESTAT register is clear when the first EEPROM array write occurs,
			 * Then the contents of the address and data buffers will be overwritten and the CBEIF flag will be set
			 */
			estat_reg = estat_reg | 0x80;
			eaddr_reg = cmd_queue_back->getAddr();
			edata_reg = cmd_queue_back->getData();
		}

	}

	void fetchCommand() {

		if (!cmd_queue.empty()) {
			TCommand *cmd = cmd_queue.front();
			cmd_queue.pop();
			ecmd_reg = cmd->getCmd();
			eaddr_reg = cmd->getAddr();
			edata_reg = cmd->getData();

			if (cmd_queue_back == cmd) {
				cmd_queue_back = NULL;
			}
			delete cmd;
		}
	}

	TCommand* writeCmd(uint8_t _cmd) {
		if (cmd_queue.empty()) {
//			address_t pc;
//			std::list<VariableBase *> lst;
//
//			Object::GetSimulator()->GetRegisters(lst);
//
//			for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {
//
//				string var_name(((VariableBase *) *it)->GetName());
//				if (var_name == "CPU.PC") {
//					std::cerr << "PC =0x" << std::hex << (unsigned int) *((VariableBase *) *it) << endl;
//				}
//			}

			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Writing to the ECMD register before writing to an EEPROM address is an illegal operation. " << std::endl << EndDebugWarning;

			setACCERR();
//			abort_write_sequence();

			return NULL;
		}
		else if ((cmd_queue_back != NULL) && cmd_queue_back->isCmdWrite()) {
			inherited::logger << DebugWarning << " : " << inherited::name() << ":: Writing a second command to the ECMD register in the same command write sequence is an illegal operation. " << std::endl << EndDebugWarning;

			setACCERR();
			abort_write_sequence();

			return NULL;
		} else {
			cmd_queue_back->setCmd(_cmd);

			// is sector erase abort command ?
			if ((_cmd == 0x47) && sector_erase_modify_active) {
				/**
				 * Set ESTAT::ACCERR because Launching the sector erase abort command while a sector erase or sector modify operation is active
				 * results in the early termination of the sector erase or sector modify operation.
				 */
				setACCERR();
				sector_erase_abort_command_req = true;
			}
		}

		if (cmd_queue.size() == 1) {
			ecmd_reg = cmd_queue_back->getCmd();
		}
	}

	void empty_cmd_queue() {
		while (!cmd_queue.empty()) {
			TCommand *cmd = cmd_queue.front();
			cmd_queue.pop();

			delete cmd;
		}
		cmd_queue_back = NULL;
	}

	void setACCERR() { estat_reg = estat_reg | 0x10; }

	void ComputeInternalTime();

	tlm_quantumkeeper quantumkeeper;

	PayloadFabric<XINT_Payload> xint_payload_fabric;

	double	bus_cycle_time_int;	// The time unit is PS
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	double	oscillator_cycle_time_int;
	Parameter<double>	param_oscillator_cycle_time_int;
	sc_time		oscillator_cycle_time;

	sc_time		eeclk_time;
	sc_time		min_eeclk_time;

	sc_event	command_launch_event;

	bool sector_erase_abort_active;
	bool sector_erase_modify_active;
	bool sector_erase_abort_command_req;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint64_t protected_area_start_address;

	uint8_t cmd_interruptOffset;
	Parameter<uint8_t> param_cmd_interruptOffset;

	physical_address_t eeprom_protection_byte_addr;
	Parameter<physical_address_t> param_eeprom_protection_byte_addr;

	double erase_fail_ratio;
	Parameter<double> param_erase_fail_ratio;

	// Registers map
	map<string, Register *> registers_registry;

	std::vector<unisim::kernel::service::VariableBase*> extended_registers_registry;

	uint8_t eclkdiv_reg, reserved1_reg, reserved2_reg, ecnfg_reg, eprot_reg, estat_reg, ecmd_reg, reserved3_reg;
	uint16_t eaddr_reg, edata_reg;

};

} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_ */
