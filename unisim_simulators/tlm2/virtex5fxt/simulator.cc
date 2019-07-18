/*
 *  Copyright (c) 2007-2011,
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

#include <simulator.hh>
#include <unisim/kernel/logger/logger_server.hh>

Simulator::Simulator(int argc, char **argv, const sc_core::sc_module_name& name)
	: unisim::kernel::tlm2::Simulator(name, argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, ram(0)
	, bram(0)
	, critical_input_interrupt_stub(0)
	, external_input_interrupt_stub(0)
	, mplb(0)
	, splb0_stub(0)
	, splb1_stub(0)
	, intc(0)
	, timer(0)
	, flash(0)
	, pwm_stub(0)
	, dcr_controller(0)
	, crossbar(0)
	, mci(0)
	, uart_lite(0)
	, gpio_dip_switches_8bit(0)
	, gpio_leds_8bit(0)
	, gpio_5_leds_positions(0)
	, gpio_push_buttons_5bit(0)
	, dip_switches_8bit(0)
	, leds_8bit(0)
	, _5_leds_positions(0)
	, push_buttons_5bit(0)
	, master1_dcr_stub(0)
	, apu_dcr_stub(0)
	, dmac0_dcr_stub(0)
	, dmac1_dcr_stub(0)
	, dmac2_dcr_stub(0)
	, dmac3_dcr_stub(0)
	, external_slave_dcr_stub(0)
	, loader(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, ram_effective_to_physical_address_translator(0)
	, flash_effective_to_physical_address_translator(0)
	, bram_effective_to_physical_address_translator(0)
	, netstreamer(0)
	, profiler(0)
	, instrumenter(0)
	, http_server(0)
	, web_terminal(0)
	, char_io_tee(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, enable_profiler(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_enable_profiler("enable-profiler", 0, enable_profiler, "Enable/Disable profiler")
	, exit_status(0)
{
	if(enable_profiler)
	{
		this->SetVariable("HARDWARE.instrumenter.enable-user-interface", true); // When profiler is enabled, enable also instrumenter user interface so that profiler interface is periodically refreshed too
	}
	
	unsigned int irq;
	unsigned int channel;
	unsigned int i;
	
	//=========================================================================
	//===                     Instrumenter instantiation                    ===
	//=========================================================================
	instrumenter = new INSTRUMENTER("instrumenter", this);
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu", this);

	//  - RAM
	ram = new RAM("ram", this);
	
	//  - RAM
	bram = new BRAM("bram", this);
	//  - IRQ Stubs
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		switch(irq)
		{
			case TIMER_IRQ:
			case UART_LITE_IRQ:
			case GPIO_DIP_SWITCHES_8BIT_IRQ:
			case GPIO_PUSH_BUTTONS_5BIT_IRQ:
				input_interrupt_stub[irq] = 0;
				break;
			default:
				{
					std::stringstream input_interrupt_stub_name_sstr;
					input_interrupt_stub_name_sstr << "input-interrupt-stub" << irq;
					input_interrupt_stub[irq] = new IRQ_STUB(input_interrupt_stub_name_sstr.str().c_str());
				}
				break;
		}
	}
	critical_input_interrupt_stub = new IRQ_STUB("critical-input-interrupt-stub", this);
	// - MPLB
	mplb = new MPLB("mplb", this);
	// - SPLB0
	splb0_stub = new SPLB0_STUB("splb0-stub", this);
	// - SPLB1
	splb1_stub = new SPLB0_STUB("splb1-stub", this);
	// - Interrupt controller
	intc = new INTC("intc", this);
	// - Timer
	timer = new TIMER("timer", this);
	// - Flash memory
	flash = new FLASH("flash", this);
	// - Capture trigger stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream capture_trigger_stub_name_sstr;
		capture_trigger_stub_name_sstr << "capture-trigger-stub" << channel;
		capture_trigger_stub[channel] = new CAPTURE_TRIGGER_STUB(capture_trigger_stub_name_sstr.str().c_str(), this);
	}
	// - Generate out stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream generate_out_stub_name_sstr;
		generate_out_stub_name_sstr << "generate-out-stub" << channel;
		generate_out_stub[channel] = new GENERATE_OUT_STUB(generate_out_stub_name_sstr.str().c_str(), this);
	}
	// - PWM stub
	pwm_stub = new PWM_STUB("pwm-stub", this);
	// - DCR controller
	dcr_controller = new DCR_CONTROLLER("dcr-controller", this);
	// - Crossbar
	crossbar = new CROSSBAR("crossbar", this);
	// - MCI
	mci = new MCI("mci", this);
	// - UART Lite
	uart_lite = new UART_LITE("uart-lite", this);
	// - GPIO DIP switches 8 Bit
	gpio_dip_switches_8bit = new GPIO_DIP_SWITCHES_8BIT("gpio-dip-switches-8bit", this);
	// - GPIO LEDs 8 Bit
	gpio_leds_8bit = new GPIO_LEDS_8BIT("gpio-leds-8bit", this);
	// - GPIO 5 LEDs Positions
	gpio_5_leds_positions = new GPIO_5_LEDS_POSITIONS("gpio-5-leds-positions", this);
	// - GPIO Push Buttons 5 bit
	gpio_push_buttons_5bit = new GPIO_PUSH_BUTTONS_5BIT("gpio-push-buttons-5bit", this);
	// - DIP Switches 8 bit
	dip_switches_8bit = new DIP_SWITCHES_8BIT("dip-switches-8bit", this);
	// - LEDs 8 bit
	leds_8bit = new LEDS_8BIT("leds-8bit", this);
	// - 5 LEDs Positions
	_5_leds_positions = new _5_LEDS_POSITIONS("5-leds-positions", this);
	// - Push buttons 5 bit
	push_buttons_5bit = new PUSH_BUTTONS_5BIT("push-buttons-5bit", this);
	// - DCR stubs
	master1_dcr_stub = new MASTER1_DCR_STUB("master1-dcr-stub", this);
	apu_dcr_stub = new APU_DCR_STUB("apu-dcr-stub", this);
	dmac0_dcr_stub = new DMAC0_DCR_STUB("dma0-dcr-stub", this);
	dmac1_dcr_stub = new DMAC1_DCR_STUB("dma1-dcr-stub", this);
	dmac2_dcr_stub = new DMAC2_DCR_STUB("dma2-dcr-stub", this);
	dmac3_dcr_stub = new DMAC3_DCR_STUB("dma3-dcr-stub", this);
	external_slave_dcr_stub = new EXTERNAL_SLAVE_DCR_STUB("external-slave-dcr-stub", this);

	//=========================================================================
	//===                          Port registration                        ===
	//=========================================================================
	
	// - MPLB
	instrumenter->RegisterPort(mplb->input_if_clock);
	instrumenter->RegisterPort(mplb->output_if_clock);
	
	//=========================================================================
	//===                         Channels creation                         ===
	//=========================================================================
	
	instrumenter->CreateClock("CLK");
	
	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = new LOADER("loader");
	//  - debugger
	debugger = (enable_inline_debugger or enable_gdb_server or enable_profiler) ? new DEBUGGER("debugger") : 0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - memory address translator from effective address to physical address
	ram_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("ram-effective-to-physical-address-translator");
	flash_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("flash-effective-to-physical-address-translator");
	bram_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("bram-effective-to-physical-address-translator");
	// - netstreamer
	netstreamer = new NETSTREAMER("netstreamer");
	// - profiler
	profiler = enable_profiler ? new PROFILER("profiler") : 0;
	// - HTTP server
	http_server = new HTTP_SERVER("http-server");
	// - Web Terminal
	web_terminal = new WEB_TERMINAL("web-terminal");
	// - Char I/O Tee
	char_io_tee = new CHAR_IO_TEE("char-io-tee");
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->icurd_plb_master_sock(crossbar->icurd_plb_slave_sock); // CPU>ICURD <-> ICURD<Crossbar
	cpu->dcuwr_plb_master_sock(crossbar->dcuwr_plb_slave_sock); // CPU>DCUWR <-> DCUWR<Crossbar
	cpu->dcurd_plb_master_sock(crossbar->dcurd_plb_slave_sock); // CPU>DCURD <-> DCURD<Crossbar
	cpu->dcr_master_sock(*dcr_controller->dcr_slave_sock[0]); // (master 0) CPU>DCR <-> DCR controller
	master1_dcr_stub->master_sock(*dcr_controller->dcr_slave_sock[1]); // master 1>DCR <-> DCR controller
	
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::APU_SLAVE_NUM])(apu_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::MCI_SLAVE_NUM])(mci->dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::CROSSBAR_SLAVE_NUM])(crossbar->crossbar_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBS0_SLAVE_NUM])(crossbar->plbs0_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBS1_SLAVE_NUM])(crossbar->plbs1_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBM_SLAVE_NUM])(crossbar->plbm_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC0_SLAVE_NUM])(dmac0_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC1_SLAVE_NUM])(dmac1_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC2_SLAVE_NUM])(dmac2_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC3_SLAVE_NUM])(dmac3_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::EXTERNAL_SLAVE_NUM])(external_slave_dcr_stub->slave_sock);
	
	crossbar->mplb_master_sock(*mplb->targ_socket[0]);   // crossbar>MPLB <-> MPLB
	crossbar->mci_master_sock(mci->mci_slave_sock);      // crossbar>MCI <-> MCI
	mci->mci_master_sock(ram->slave_sock);               // MCI <-> RAM 
	
	splb0_stub->master_sock(crossbar->splb0_slave_sock);  // SPLB0 stub <-> SPLB0<Crossbar
	splb1_stub->master_sock(crossbar->splb1_slave_sock);  // SPLB1 stub <-> SPLB1<Crossbar
	
	instrumenter->Bind("HARDWARE.mplb.input_if_clock", "HARDWARE.CLK");
	instrumenter->Bind("HARDWARE.mplb.output_if_clock", "HARDWARE.CLK");
	
	(*mplb->init_socket[INTC_CONFIG::MPLB_PORT])(intc->slave_sock);      // MPLB <-> INTC
	(*mplb->init_socket[TIMER_CONFIG::MPLB_PORT])(timer->slave_sock);     // MPLB <-> TIMER
	(*mplb->init_socket[FLASH_MPLB_PORT])(flash->slave_sock);     // MPLB <-> FLASH
	(*mplb->init_socket[BRAM_MPLB_PORT])(bram->slave_sock);      // MPLB <-> BRAM
	(*mplb->init_socket[UART_LITE_CONFIG::MPLB_PORT])(uart_lite->slave_sock); // MPLB <-> UART Lite
	(*mplb->init_socket[GPIO_DIP_SWITCHES_8BIT_CONFIG::MPLB_PORT])(gpio_dip_switches_8bit->slave_sock);      // MPLB <-> GPIO DIP switches 8 Bit
	(*mplb->init_socket[GPIO_LEDS_8BIT_CONFIG::MPLB_PORT])(gpio_leds_8bit->slave_sock);              // MPLB <-> GPIO LEDs 8 Bit
	(*mplb->init_socket[GPIO_5_LEDS_POSITIONS_CONFIG::MPLB_PORT])(gpio_5_leds_positions->slave_sock);       // MPLB <-> GPIO 5 LEDs Positions
	(*mplb->init_socket[GPIO_PUSH_BUTTONS_5BIT_CONFIG::MPLB_PORT])(gpio_push_buttons_5bit->slave_sock);      // MPLB <-> GPIO Push Buttons 5 bit
	
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		switch(irq)
		{
			case TIMER_IRQ:
				timer->interrupt_master_sock(*intc->irq_slave_sock[irq]); // TIMER>IRQ <-> INTR<INTC
				break;
			case UART_LITE_IRQ:
				uart_lite->interrupt_master_sock(*intc->irq_slave_sock[irq]); // UART Lite>IRQ <-> INTR<INTC
				break;
			case GPIO_DIP_SWITCHES_8BIT_IRQ:
				(*gpio_dip_switches_8bit->interrupt_master_sock)(*intc->irq_slave_sock[irq]); // GPIO DIP SWITCHES 8BIT>IRQ <-> INTR<INTC
				break;
			case GPIO_PUSH_BUTTONS_5BIT_IRQ:
				(*gpio_push_buttons_5bit->interrupt_master_sock)(*intc->irq_slave_sock[irq]); // GPIO PUSH BUTTONS 5BIT>IRQ <-> INTR<INTC
				break;
			default:
				(input_interrupt_stub[irq]->master_sock)(*intc->irq_slave_sock[irq]); // IRQ stub>IRQ <-> INTR<INTC
				break;
		}
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		capture_trigger_stub[channel]->master_sock(*timer->capture_trigger_slave_sock[channel]); // Capture trigger stub <-> TIMER
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		(*timer->generate_out_master_sock[channel])(generate_out_stub[channel]->slave_sock); // TIMER <-> Generate out stub
	}
	timer->pwm_master_sock(pwm_stub->slave_sock); // TIMER <-> PWM stub
	intc->irq_master_sock(cpu->external_input_interrupt_slave_sock); // INTC>IRQ <-> External Input<CPU
	critical_input_interrupt_stub->master_sock(cpu->critical_input_interrupt_slave_sock); // IRQ Stub <-> CPU
	
	for(i = 0; i < GPIO_DIP_SWITCHES_8BIT_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*dip_switches_8bit->gpio_master_sock[i])(*gpio_dip_switches_8bit->gpio_slave_sock[i]);
		(*gpio_dip_switches_8bit->gpio_master_sock[i])(*dip_switches_8bit->gpio_slave_sock[i]);
	}
	for(i = 0; i < GPIO_LEDS_8BIT_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*gpio_leds_8bit->gpio_master_sock[i])(*leds_8bit->gpio_slave_sock[i]);
		(*leds_8bit->gpio_master_sock[i])(*gpio_leds_8bit->gpio_slave_sock[i]);
	}
	for(i = 0; i < GPIO_5_LEDS_POSITIONS_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*gpio_5_leds_positions->gpio_master_sock[i])(*_5_leds_positions->gpio_slave_sock[i]);
		(*_5_leds_positions->gpio_master_sock[i])(*gpio_5_leds_positions->gpio_slave_sock[i]);
	}
	for(i = 0; i < GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*push_buttons_5bit->gpio_master_sock[i])(*gpio_push_buttons_5bit->gpio_slave_sock[i]);
		(*gpio_push_buttons_5bit->gpio_master_sock[i])(*push_buttons_5bit->gpio_slave_sock[i]);
	}

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> crossbar->memory_export;
	
	crossbar->mci_memory_import >> mci->memory_export;
	crossbar->mplb_memory_import >> mplb->memory_export;
	mci->memory_import >> ram->memory_export;
	(*mplb->memory_import[INTC_CONFIG::MPLB_PORT]) >> intc->memory_export;
	(*mplb->memory_import[TIMER_CONFIG::MPLB_PORT]) >> timer->memory_export;
	(*mplb->memory_import[FLASH_MPLB_PORT]) >> flash->memory_export;
	(*mplb->memory_import[BRAM_MPLB_PORT]) >> bram->memory_export;
	(*mplb->memory_import[UART_LITE_CONFIG::MPLB_PORT]) >> uart_lite->memory_export;
	(*mplb->memory_import[GPIO_DIP_SWITCHES_8BIT_CONFIG::MPLB_PORT]) >> gpio_dip_switches_8bit->memory_export;
	(*mplb->memory_import[GPIO_LEDS_8BIT_CONFIG::MPLB_PORT]) >> gpio_leds_8bit->memory_export;
	(*mplb->memory_import[GPIO_5_LEDS_POSITIONS_CONFIG::MPLB_PORT]) >> gpio_5_leds_positions->memory_export;
	(*mplb->memory_import[GPIO_PUSH_BUTTONS_5BIT_CONFIG::MPLB_PORT]) >> gpio_push_buttons_5bit->memory_export;

	if (debugger)
	{
		// Debugger <-> CPU Connections
		cpu->debug_yielding_import                            >> *debugger->debug_yielding_export[0];
		cpu->trap_reporting_import                            >> *debugger->trap_reporting_export[0];
		cpu->memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
		*debugger->disasm_import[0]                          >> cpu->disasm_export;
		*debugger->memory_import[0]                          >> cpu->memory_export;
		*debugger->registers_import[0]                       >> cpu->registers_export;
		*debugger->memory_access_reporting_control_import[0] >> cpu->memory_access_reporting_control_export;
			
		// Debugger <-> Loader connections
		debugger->blob_import >> loader->blob_export;
	}
		
	if (inline_debugger)
	{
		// inline-debugger <-> debugger connections
		*debugger->debug_event_listener_import[0]      >> inline_debugger->debug_event_listener_export;
		*debugger->debug_yielding_import[0]            >> inline_debugger->debug_yielding_export;
		inline_debugger->debug_yielding_request_import >> *debugger->debug_yielding_request_export[0];
		inline_debugger->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[0];
		inline_debugger->disasm_import                 >> *debugger->disasm_export[0];
		inline_debugger->memory_import                 >> *debugger->memory_export[0];
		inline_debugger->registers_import              >> *debugger->registers_export[0];
		inline_debugger->stmt_lookup_import            >> *debugger->stmt_lookup_export[0];
		inline_debugger->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[0];
		inline_debugger->backtrace_import              >> *debugger->backtrace_export[0];
		inline_debugger->debug_info_loading_import     >> *debugger->debug_info_loading_export[0];
		inline_debugger->data_object_lookup_import     >> *debugger->data_object_lookup_export[0];
		inline_debugger->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[0];
	}
	
	if (gdb_server)
	{
		// gdb-server <-> debugger connections
		*debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
		*debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
		gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
		gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
		gdb_server->memory_import                 >> *debugger->memory_export[1];
		gdb_server->registers_import              >> *debugger->registers_export[1];
	}

	if (profiler)
	{
		*debugger->debug_yielding_import[2]       >> profiler->debug_yielding_export;
		*debugger->debug_event_listener_import[2] >> profiler->debug_event_listener_export;
		profiler->debug_yielding_request_import   >> *debugger->debug_yielding_request_export[2];
		profiler->debug_event_trigger_import      >> *debugger->debug_event_trigger_export[2];
		profiler->disasm_import                   >> *debugger->disasm_export[2];
		profiler->memory_import                   >> *debugger->memory_export[2];
		profiler->registers_import                >> *debugger->registers_export[2];
		profiler->stmt_lookup_import              >> *debugger->stmt_lookup_export[2];
		profiler->symbol_table_lookup_import      >> *debugger->symbol_table_lookup_export[2];
		profiler->backtrace_import                >> *debugger->backtrace_export[2];
		profiler->debug_info_loading_import       >> *debugger->debug_info_loading_export[2];
		profiler->data_object_lookup_import       >> *debugger->data_object_lookup_export[2];
		profiler->subprogram_lookup_import        >> *debugger->subprogram_lookup_export[2];
	}
	
	ram_effective_to_physical_address_translator->memory_import >> ram->memory_export;
	flash_effective_to_physical_address_translator->memory_import >> flash->memory_export;
	bram_effective_to_physical_address_translator->memory_import >> bram->memory_export;
	*loader->memory_import[0] >> ram_effective_to_physical_address_translator->memory_export;
	*loader->memory_import[1] >> bram_effective_to_physical_address_translator->memory_export;
	*loader->memory_import[2] >> flash_effective_to_physical_address_translator->memory_export;
	loader->registers_import >> cpu->registers_export;
	cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	
	uart_lite->char_io_import >> char_io_tee->char_io_export;
	(*char_io_tee->char_io_import[0]) >> netstreamer->char_io_export;
	(*char_io_tee->char_io_import[1]) >> web_terminal->char_io_export;
	
	*http_server->http_server_import[0] >> unisim::kernel::logger::Logger::StaticServerInstance()->http_server_export;
	*http_server->http_server_import[1] >> instrumenter->http_server_export;
	*http_server->http_server_import[2] >> web_terminal->http_server_export;
	if (profiler)
	{
		*http_server->http_server_import[3] >> profiler->http_server_export;
	}

	*http_server->registers_import[0] >> cpu->registers_export;
}

Simulator::~Simulator()
{
	unsigned int irq;
	unsigned int channel;
	if(critical_input_interrupt_stub) delete critical_input_interrupt_stub;
	if(external_input_interrupt_stub) delete external_input_interrupt_stub;
	if(ram) delete ram;
	if(bram) delete bram;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(cpu) delete cpu;
	if(mplb) delete mplb;
	if(splb0_stub) delete splb0_stub;
	if(splb1_stub) delete splb1_stub;
	if(intc) delete intc;
	if(timer) delete timer;
	if(flash) delete flash;
	if(crossbar) delete crossbar;
	if(mci) delete mci;
	if(uart_lite) delete uart_lite;
	if(gpio_dip_switches_8bit) delete gpio_dip_switches_8bit;
	if(gpio_leds_8bit) delete gpio_leds_8bit;
	if(gpio_5_leds_positions) delete gpio_5_leds_positions;
	if(gpio_push_buttons_5bit) delete gpio_push_buttons_5bit;
	if(dip_switches_8bit) delete dip_switches_8bit;
	if(leds_8bit) delete leds_8bit;
	if(_5_leds_positions) delete _5_leds_positions;
	if(push_buttons_5bit) delete push_buttons_5bit;
	if(master1_dcr_stub) delete master1_dcr_stub;
	if(apu_dcr_stub) delete apu_dcr_stub;
	if(dmac0_dcr_stub) delete dmac0_dcr_stub;
	if(dmac1_dcr_stub) delete dmac1_dcr_stub;
	if(dmac2_dcr_stub) delete dmac2_dcr_stub;
	if(dmac3_dcr_stub) delete dmac3_dcr_stub;
	if(external_slave_dcr_stub) delete external_slave_dcr_stub;
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		if(input_interrupt_stub[irq]) delete input_interrupt_stub[irq];
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(capture_trigger_stub[channel]) delete capture_trigger_stub[channel];
	}
	// - Generate out stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(generate_out_stub[channel]) delete generate_out_stub[channel];
	}
	if(pwm_stub) delete pwm_stub;
	if(dcr_controller) delete dcr_controller;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(loader) delete loader;
	if(ram_effective_to_physical_address_translator) delete ram_effective_to_physical_address_translator;
	if(bram_effective_to_physical_address_translator) delete bram_effective_to_physical_address_translator;
	if(flash_effective_to_physical_address_translator) delete flash_effective_to_physical_address_translator;
	if(netstreamer) delete netstreamer;
	if(profiler) delete profiler;
	if(debugger) delete debugger;
	if(http_server) delete http_server;
	if(web_terminal) delete web_terminal;
	if(char_io_tee) delete char_io_tee;
	if(instrumenter) delete instrumenter;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM Virtex 5 FXT");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2011, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM Virtex 5 FXT, full system PPC440x5 based simulator including some Virtex 5 IPs");
	simulator->SetVariable("schematic", "virtex5fxt/fig_schematic.pdf");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "unisim/service/debug/gdb_server/gdb_powerpc_32.xml";
	const char *dwarf_register_number_mapping_filename = "unisim/util/debug/dwarf/powerpc_eabi_gcc_dwarf_register_number_mapping.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 400.0; // in Mhz
	double cpu_clock_multiplier = 2.0;
	double ext_timer_clock_divisor = 2.0;
	double cpu_ipc = 2.0; // in instructions per cycle: PPC440 is a superscalar processor that can execute "out-of-order" up to 2 instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double ext_timer_cycle_time = ext_timer_clock_divisor * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - CLK
	simulator->SetVariable( "HARDWARE.CLK.clock-period", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable( "HARDWARE.CLK.lazy-clock", true);
	
	//  - PowerPC processor
	simulator->SetVariable("HARDWARE.cpu.cpu-cycle-time", sc_core::sc_time(cpu_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.cpu.bus-cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.cpu.ext-timer-cycle-time", sc_core::sc_time(ext_timer_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.cpu.voltage", 1.0 * 1e3); // mV
	simulator->SetVariable("HARDWARE.cpu.max-inst", maxinst);
	simulator->SetVariable("HARDWARE.cpu.nice-time", "200 ns"); // 200 ns (currently geared to the minimum interval between capture trigger samples)
	simulator->SetVariable("HARDWARE.cpu.ipc", cpu_ipc);
	simulator->SetVariable("HARDWARE.cpu.enable-dmi", true); // Allow CPU to use of SystemC TLM 2.0 DMI
	simulator->SetVariable("HARDWARE.cpu.processor-version", 0x7ff21912); // PPC440x5 in Virtex-5 FXT
	simulator->SetVariable("HARDWARE.cpu.cpuid", 0); // Processor #0
	simulator->SetVariable("HARDWARE.cpu.reset-addr", 0xfffffffc); // processor starts at the last word of memory space

	//  - DCR controller
	simulator->SetVariable("HARDWARE.dcr-controller.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - Crossbar
	simulator->SetVariable("HARDWARE.crossbar.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - MCI
	simulator->SetVariable("HARDWARE.mci.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - MPLB
	simulator->SetVariable("HARDWARE.mplb.cycle_time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - Memory router
	simulator->SetVariable("HARDWARE.memory-router.cycle_time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.memory-router.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffffffffffff\" output_port=\"0\" translation=\"0x0\""); // RAM

	unsigned int mapping_num = 0;
	
	std::stringstream sstr_intc_mapping_name;
	sstr_intc_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_intc_mapping;
	sstr_intc_mapping << "range_start=\"0x" << std::hex << INTC_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << INTC_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << INTC_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << INTC_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_intc_mapping_name.str().c_str(), sstr_intc_mapping.str().c_str()); // XPS IntC

	std::stringstream sstr_timer_mapping_name;
	sstr_timer_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_timer_mapping;
	sstr_timer_mapping << "range_start=\"0x" << std::hex << TIMER_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << TIMER_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << TIMER_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << TIMER_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_timer_mapping_name.str().c_str(), sstr_timer_mapping.str().c_str()); // XPS Timer/Counter
	
	std::stringstream sstr_flash_mapping_name;
	sstr_flash_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_flash_mapping;
	sstr_flash_mapping << "range_start=\"0x" << std::hex << FLASH_BASE_ADDR << std::dec << "\" range_end=\"0x" << std::hex << (FLASH_BASE_ADDR + FLASH_BYTE_SIZE - 1) << std::dec << "\" output_port=\"" << FLASH_MPLB_PORT << "\" translation=\"0x" << std::hex << FLASH_BASE_ADDR << std::dec << "\"";
	simulator->SetVariable(sstr_flash_mapping_name.str().c_str(), sstr_flash_mapping.str().c_str()); // 32 MB Flash memory (i.e. 1 * 256 Mbits S29GL256P flash memory chips)
	
	std::stringstream sstr_bram_mapping_name;
	sstr_bram_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_bram_mapping;
	sstr_bram_mapping << "range_start=\"0x" << std::hex << BRAM_BASE_ADDR << std::dec << "\" range_end=\"0x" << std::hex << (BRAM_BASE_ADDR + BRAM_BYTE_SIZE - 1) << std::dec << "\" output_port=\"" << BRAM_MPLB_PORT << "\" translation=\"0x" << std::hex << BRAM_BASE_ADDR << std::dec << "\"";
	simulator->SetVariable(sstr_bram_mapping_name.str().c_str(), sstr_bram_mapping.str().c_str()); // 256 KB XPS BRAM

	std::stringstream sstr_uart_lite_mapping_name;
	sstr_uart_lite_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_uart_lite_mapping;
	sstr_uart_lite_mapping << "range_start=\"0x" << std::hex << UART_LITE_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << UART_LITE_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << UART_LITE_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << UART_LITE_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_uart_lite_mapping_name.str().c_str(), sstr_uart_lite_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_dip_switches_8bit_mapping_name;
	sstr_gpio_dip_switches_8bit_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_dip_switches_8bit_mapping;
	sstr_gpio_dip_switches_8bit_mapping << "range_start=\"0x" << std::hex << GPIO_DIP_SWITCHES_8BIT_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << GPIO_DIP_SWITCHES_8BIT_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << GPIO_DIP_SWITCHES_8BIT_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << GPIO_DIP_SWITCHES_8BIT_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_dip_switches_8bit_mapping_name.str().c_str(), sstr_gpio_dip_switches_8bit_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_leds_8bit_mapping_name;
	sstr_gpio_leds_8bit_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_leds_8bit_mapping;
	sstr_gpio_leds_8bit_mapping << "range_start=\"0x" << std::hex << GPIO_LEDS_8BIT_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << GPIO_LEDS_8BIT_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << GPIO_LEDS_8BIT_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << GPIO_LEDS_8BIT_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_leds_8bit_mapping_name.str().c_str(), sstr_gpio_leds_8bit_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_5_leds_positions_mapping_name;
	sstr_gpio_5_leds_positions_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_5_leds_positions_mapping;
	sstr_gpio_5_leds_positions_mapping << "range_start=\"0x" << std::hex << GPIO_5_LEDS_POSITIONS_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << GPIO_5_LEDS_POSITIONS_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << GPIO_5_LEDS_POSITIONS_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << GPIO_5_LEDS_POSITIONS_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_5_leds_positions_mapping_name.str().c_str(), sstr_gpio_5_leds_positions_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_push_buttons_5bit_mapping_name;
	sstr_gpio_push_buttons_5bit_mapping_name << "HARDWARE.mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_push_buttons_5bit_mapping;
	sstr_gpio_push_buttons_5bit_mapping << "range_start=\"0x" << std::hex << GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << GPIO_PUSH_BUTTONS_5BIT_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_push_buttons_5bit_mapping_name.str().c_str(), sstr_gpio_push_buttons_5bit_mapping.str().c_str()); // XPS Timer/Counter

	// - Loader memory router
	std::stringstream sstr_loader_mapping;
	sstr_loader_mapping << "ram-effective-to-physical-address-translator:0x" << std::hex << RAM_BASE_ADDR << std::dec << "-0x" << std::hex << (RAM_BASE_ADDR + RAM_BYTE_SIZE - 1) << std::dec << ":+0x" << std::hex << RAM_BASE_ADDR << std::dec;
	sstr_loader_mapping << ",bram-effective-to-physical-address-translator:0x" << std::hex << BRAM_BASE_ADDR << std::dec << "-0x" << std::hex << (BRAM_BASE_ADDR + BRAM_BYTE_SIZE - 1) << std::dec << ":+0x" << std::hex << BRAM_BASE_ADDR << std::dec;
	sstr_loader_mapping << ",flash-effective-to-physical-address-translator:0x" << std::hex << FLASH_BASE_ADDR << std::dec << "-0x" << std::hex << (FLASH_BASE_ADDR + FLASH_BYTE_SIZE - 1) << std::dec << ":+0x" << std::hex << FLASH_BASE_ADDR << std::dec;
	simulator->SetVariable("loader.memory-mapper.mapping", sstr_loader_mapping.str().c_str()); // 256 MB RAM / 256 KB BRAM / 32 MB Flash memory

	//  - RAM
	simulator->SetVariable("HARDWARE.ram.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.ram.read-latency", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.ram.write-latency", sc_core::SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("HARDWARE.ram.org", RAM_BASE_ADDR);
	simulator->SetVariable("HARDWARE.ram.bytesize", RAM_BYTE_SIZE);

	//  - BRAM
	simulator->SetVariable("HARDWARE.bram.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.bram.read-latency", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.bram.write-latency", sc_core::SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("HARDWARE.bram.org", BRAM_BASE_ADDR);
	simulator->SetVariable("HARDWARE.bram.bytesize", BRAM_BYTE_SIZE);
	
	//  - Interrupt controller
	simulator->SetVariable("HARDWARE.intc.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - Timer
	simulator->SetVariable("HARDWARE.timer.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - UART Lite
	simulator->SetVariable("HARDWARE.uart-lite.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - GPIOs
	simulator->SetVariable("HARDWARE.gpio-dip-switches-8bit.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.gpio-leds-8bit.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.gpio-5-leds-positions.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.gpio-push-buttons-5bit.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - Flash
	simulator->SetVariable("HARDWARE.flash.org", FLASH_BASE_ADDR);
	simulator->SetVariable("HARDWARE.flash.bytesize", FLASH_BYTE_SIZE);
	simulator->SetVariable("HARDWARE.flash.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	
	//  - Capture Trigger stubs
	simulator->SetVariable("HARDWARE.capture-trigger-stub0.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.capture-trigger-stub0.nice-time", "1 ms");
	simulator->SetVariable("HARDWARE.capture-trigger-stub1.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.capture-trigger-stub1.nice-time", "1 ms");
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", dwarf_register_number_mapping_filename);
	
	simulator->SetVariable("http-server.http-port", 12360);

	simulator->SetVariable("web-terminal.title", "Serial Terminal over UART Lite");
}

void Simulator::Run()
{
	std::cerr << "Starting simulation at supervisor privilege level" << std::endl;
	
	double time_start = host_time->GetTime();

	sc_core::sc_report_handler::set_actions(sc_core::SC_INFO, sc_core::SC_DO_NOTHING); // disable SystemC messages
	
	try
	{
		sc_core::sc_start();
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << std::endl;
		std::cerr << e.what() << std::endl;
	}

	std::cerr << "Simulation finished" << std::endl;

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	std::cerr << "Simulation run-time parameters:" << std::endl;
	DumpParameters(std::cerr);
	std::cerr << std::endl;
	std::cerr << "Simulation formulas:" << std::endl;
	DumpFormulas(std::cerr);
	std::cerr << std::endl;
	std::cerr << "Simulation statistics:" << std::endl;
	DumpStatistics(std::cerr);
	std::cerr << std::endl;

	std::cerr << "simulation time: " << spent_time << " seconds" << std::endl;
	std::cerr << "simulated time : " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << std::endl;
	std::cerr << "target speed: " << ((double) (*cpu)["instruction-counter"] / ((double) (*cpu)["run-time"] - (double) (*cpu)["idle-time"]) / 1000000.0) << " MIPS" << std::endl;
	std::cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << std::endl;
	std::cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;
}

unisim::kernel::service::Simulator::SetupStatus Simulator::Setup()
{
	if(inline_debugger || profiler)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	// Optionally get the program to load from the command line arguments
	unisim::kernel::service::VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("loader.filename", ((std::string)(*cmd_args)[0]).c_str());
	}

	unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();
	
	return setup_status;
}

bool Simulator::EndSetup()
{
  if(profiler)
  {
    http_server->AddJSAction(
      unisim::service::interfaces::ToolbarOpenTabAction(
        /* name */      profiler->GetName(), 
        /* label */     "<img src=\"/unisim/service/debug/profiler/icon_profile_cpu0.svg\">",
        /* tips */      std::string("Profile of ") + cpu->GetName(),
        /* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
        /* uri */       profiler->URI()
    ));
  }
  
  http_server->AddJSAction(
    unisim::service::interfaces::ToolbarOpenTabAction(
      /* name */      web_terminal->GetName(),
      /* label */     "<img src=\"/unisim/service/web_terminal/icon_term0.svg\">",
      /* tips */      (*web_terminal)["title"],
      /* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
      /* uri */       web_terminal->URI()
  ));

  return true;
}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_core::sc_stop();
	if(!asynchronous)
	{
		sc_core::sc_process_handle h = sc_core::sc_get_current_process_handle();
		switch(h.proc_kind())
		{
			case sc_core::SC_THREAD_PROC_: 
			case sc_core::SC_CTHREAD_PROC_:
				sc_core::wait();
				break;
			default:
				break;
		}
	}
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

void Simulator::SigInt()
{
	if(!enable_inline_debugger)
	{
		unisim::kernel::service::Simulator::Instance()->Stop(0, 0, true);
	}
}
