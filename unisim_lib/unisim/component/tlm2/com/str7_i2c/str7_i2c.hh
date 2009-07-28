/*
 *  Copyright (c) 2009,
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
 * Authors: khaled Rahmouni
 */

#ifndef __UNISIM_COMPONENT_TLM2_COM_STR7_SPI_STR7_SPI_HH__
#define __UNISIM_COMPONENT_TLM2_COM_STR7_SPI_STR7_SPI_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_get.h>
#include <inttypes.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/interrupt/types.hh"


namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace str7_spi {

using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;
using unisim::kernel::logger::Logger;
using unisim::kernel::tlm2::PayloadFabric;


#define   NB_REGS_SPI_CONTROLER                         5
#define   BSPIn_RX                                      0
#define   BSPIn_TX                                      1
#define   BSPIn_CSR1                                    2
#define   BSPIn_CSR2                                    3
#define   BSPIn_CLK                                     4


template <unsigned int BUS_WIDTH = 32>
class STR7_SPI :
	public Object,
	public sc_module,
	public tlm::tlm_fw_transport_if<>
{
private:
	typedef STR7_SPI<BUS_WIDTH> THIS_MODULE;

public:

	/* output socket for outgoing IRQ */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> irq;

	/* socket for spi management */	
	tlm_utils::simple_initiator_socket<THIS_MODULE> mosi; //master out slave in port
    tlm_utils::simple_target_socket<THIS_MODULE> miso;//master in slave out port

	/* input socket for memory transactions */
	tlm::tlm_target_socket<BUS_WIDTH> in_mem;

	STR7_SPI(const sc_module_name& name, Object* parent = 0);
	virtual ~STR7_SPI();

	virtual bool Setup();


	/* START: callback methods for the out_irq sockets */
	tlm::tlm_sync_enum OutIRQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void OutIRQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: callback methods for the out_irq sockets */



	/* START: callback methods for the spi sockets */
	tlm::tlm_sync_enum MOSINb(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void MOSIDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual void MISOB(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);

	/* END: callback methods for the spi sockets */


	/* START: methods implementing the "in_mem" socket */
	virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);
	/* END: methods implementing the "in_mem" socket */

private:

	uint16_t Registers[NB_REGS_SPI_CONTROLER]; /* Declared as 32 bits registers*/
    	uint16_t MaskR[NB_REGS_SPI_CONTROLER]; /* but 16 bits used to simplify*/
    	uint16_t MaskW[NB_REGS_SPI_CONTROLER]; /* R/W accesses*/


	/** Read register method and update interrupt controller state as necessary.
	 * @param addr		the register address
	 * @return 			the contents of the register
	 */
	uint32_t ReadRegister(uint32_t addr);
	/** Write register method and update interrupt controller state as necessary.
	 * @param addr		the register address
	 * @param value		the data to write into the register
	 */
	void WriteRegister(uint32_t addr, uint32_t value);
    	

	void manage_interrupt(); // Set interruption signals
 	void SPIHANDLER();

	/* tx and rx fifos */

	std::list<uint16_t> tx_fifo;
	std::list<uint16_t> rx_fifo;

	uint32_t tx_fifo_size;
	uint32_t rx_fifo_size;


	/* START: module parameters */
	uint64_t base_address;
	unisim::kernel::service::Parameter<uint64_t> param_base_address;
	/* END: module parameters */

	/* START: logger and logger methods and verbose parameters/methods */
	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> param_verbose_setup;
	bool verbose_run;
	unisim::kernel::service::Parameter<bool> param_verbose_run;
	bool verbose_tlm;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm;
	inline bool VerboseAll();
	inline bool VerboseSetup();
	inline bool VerboseRun();
	inline bool VerboseTLM();
	/* END: logger and logger methods and verbose parameters/methods */
};

/*************************************************************************************
**************************************************************************************
**************************************************************************************/





} // end of namespace str7_spi
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_STR7_SPI_STR7_SPI_HH__


