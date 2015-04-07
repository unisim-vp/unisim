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

#ifndef __VIRTEX5FXT_SIMULATOR_TCC__
#define __VIRTEX5FXT_SIMULATOR_TCC__

template <class CONFIG>
Simulator<CONFIG>::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, linux_os_cpu(0)
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
	, dcr_slave_stub(0)
	, memory_router(0)
	, loader(0)
	, linux_os(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, profiler(0)
	, sim_time(0)
	, host_time(0)
	, il1_power_estimator(0)
	, dl1_power_estimator(0)
	, itlb_power_estimator(0)
	, dtlb_power_estimator(0)
	, utlb_power_estimator(0)
	, ram_effective_to_physical_address_translator(0)
	, flash_effective_to_physical_address_translator(0)
	, bram_effective_to_physical_address_translator(0)
	, telnet(0)
	, tee_memory_access_reporting(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, estimate_power(false)
	, enable_telnet(false)
	, enable_linux_os(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_estimate_power("estimate-power", 0, estimate_power, "Enable/Disable power estimators instantiation")
	, param_enable_telnet("enable-telnet", 0, enable_telnet, "Enable/Disable telnet instantiation")
	, param_enable_linux_os("enable-linux-os", 0, enable_linux_os, "Enable/Disable target Linux ABI to host ABI translation")
	, exit_status(0)
{
	unsigned int irq;
	unsigned int channel;
	unsigned int i;
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = enable_linux_os ? 0 : new CPU("cpu");
	linux_os_cpu = enable_linux_os ? new LINUX_OS_CPU("cpu") : 0;

	//  - RAM
	ram = new RAM("ram");
	
	if(enable_linux_os)
	{
		memory_router = new MEMORY_ROUTER("memory-router");
		critical_input_interrupt_stub = new IRQ_STUB("critical-input-interrupt-stub");
		external_input_interrupt_stub = new IRQ_STUB("external-input-interrupt-stub");
		dcr_slave_stub = new DCR_SLAVE_STUB("dcr-slave-stub");
		for(irq = 0; irq < CONFIG::INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
		{
			input_interrupt_stub[irq] = 0;
		}
		for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
		{
			capture_trigger_stub[channel] = 0;
		}
		for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
		{
			generate_out_stub[channel] = 0;
		}
	}
	else
	{
		//  - RAM
		bram = new BRAM("bram");
		//  - IRQ Stubs
		for(irq = 0; irq < CONFIG::INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
		{
			switch(irq)
			{
				case CONFIG::TIMER_IRQ:
				case CONFIG::UART_LITE_IRQ:
				case CONFIG::GPIO_DIP_SWITCHES_8BIT_IRQ:
				case CONFIG::GPIO_PUSH_BUTTONS_5BIT_IRQ:
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
		critical_input_interrupt_stub = new IRQ_STUB("critical-input-interrupt-stub");
		// - MPLB
		mplb = new MPLB("mplb");
		// - SPLB0
		splb0_stub = new SPLB0_STUB("splb0-stub");
		// - SPLB1
		splb1_stub = new SPLB0_STUB("splb1-stub");
		// - Interrupt controller
		intc = new INTC("intc");
		// - Timer
		timer = new TIMER("timer");
		// - Flash memory
		flash = new FLASH("flash");
		// - Capture trigger stubs
		for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
		{
			std::stringstream capture_trigger_stub_name_sstr;
			capture_trigger_stub_name_sstr << "capture-trigger-stub" << channel;
			capture_trigger_stub[channel] = new CAPTURE_TRIGGER_STUB(capture_trigger_stub_name_sstr.str().c_str());
		}
		// - Generate out stubs
		for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
		{
			std::stringstream generate_out_stub_name_sstr;
			generate_out_stub_name_sstr << "generate-out-stub" << channel;
			generate_out_stub[channel] = new GENERATE_OUT_STUB(generate_out_stub_name_sstr.str().c_str());
		}
		// - PWM stub
		pwm_stub = new PWM_STUB("pwm-stub");
		// - DCR controller
		dcr_controller = new DCR_CONTROLLER("dcr-controller");
		// - Crossbar
		crossbar = new CROSSBAR("crossbar");
		// - MCI
		mci = new MCI("mci");
		// - UART Lite
		uart_lite = new UART_LITE("uart-lite");
		// - GPIO DIP switches 8 Bit
		gpio_dip_switches_8bit = new GPIO_DIP_SWITCHES_8BIT("gpio-dip-switches-8bit");
		// - GPIO LEDs 8 Bit
		gpio_leds_8bit = new GPIO_LEDS_8BIT("gpio-leds-8bit");
		// - GPIO 5 LEDs Positions
		gpio_5_leds_positions = new GPIO_5_LEDS_POSITIONS("gpio-5-leds-positions");
		// - GPIO Push Buttons 5 bit
		gpio_push_buttons_5bit = new GPIO_PUSH_BUTTONS_5BIT("gpio-push-buttons-5bit");
		// - DIP Switches 8 bit
		dip_switches_8bit = new DIP_SWITCHES_8BIT("dip-switches-8bit");
		// - LEDs 8 bit
		leds_8bit = new LEDS_8BIT("leds-8bit");
		// - 5 LEDs Positions
		_5_leds_positions = new _5_LEDS_POSITIONS("5-leds-positions");
		// - Push buttons 5 bit
		push_buttons_5bit = new PUSH_BUTTONS_5BIT("push-buttons-5bit");
		// - DCR stubs
		master1_dcr_stub = new MASTER1_DCR_STUB("master1-dcr-stub");
		apu_dcr_stub = new APU_DCR_STUB("apu-dcr-stub");
		dmac0_dcr_stub = new DMAC0_DCR_STUB("dma0-dcr-stub");
		dmac1_dcr_stub = new DMAC1_DCR_STUB("dma1-dcr-stub");
		dmac2_dcr_stub = new DMAC2_DCR_STUB("dma2-dcr-stub");
		dmac3_dcr_stub = new DMAC3_DCR_STUB("dma3-dcr-stub");
		external_slave_dcr_stub = new EXTERNAL_SLAVE_DCR_STUB("external-slave-dcr-stub");
	}

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = enable_linux_os ? 0 : new MultiFormatLoader<CPU_ADDRESS_TYPE>("loader");
	//  - Linux loader and Linux ABI translator
	linux_os = enable_linux_os ? new Linux<CPU_ADDRESS_TYPE, CPU_ADDRESS_TYPE>("linux-os") : 0;
	//  - debugger
	debugger = (enable_linux_os || enable_inline_debugger || enable_gdb_server) ? new Debugger<CPU_ADDRESS_TYPE>("debugger") : 0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - profiler
	profiler = enable_inline_debugger ? new Profiler<CPU_ADDRESS_TYPE>("profiler") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - the optional power estimators
	il1_power_estimator = estimate_power ? new CachePowerEstimator("il1-power-estimator") : 0;
	dl1_power_estimator = estimate_power ? new CachePowerEstimator("dl1-power-estimator") : 0;
	itlb_power_estimator = estimate_power ? new CachePowerEstimator("itlb-power-estimator") : 0;
	dtlb_power_estimator = estimate_power ? new CachePowerEstimator("dtlb-power-estimator") : 0;
	utlb_power_estimator = estimate_power ? new CachePowerEstimator("utlb-power-estimator") : 0;
	//  - memory address translator from effective address to physical address
	ram_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("ram-effective-to-physical-address-translator");
	flash_effective_to_physical_address_translator = enable_linux_os ? 0 : new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("flash-effective-to-physical-address-translator");
	bram_effective_to_physical_address_translator = enable_linux_os ? 0 : new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("bram-effective-to-physical-address-translator");
	// - telnet
	telnet = (enable_telnet && !enable_linux_os) ? new unisim::service::telnet::Telnet("telnet") : 0;
	//  - Tee Memory Access Reporting
	tee_memory_access_reporting = enable_inline_debugger ? new unisim::service::tee::memory_access_reporting::Tee<CPU_ADDRESS_TYPE>("tee-memory-access-reporting") : 0;
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	if(enable_linux_os)
	{
		linux_os_cpu->icurd_plb_master_sock(*memory_router->targ_socket[0]); // CPU>ICURD <-> Memory Router
		linux_os_cpu->dcuwr_plb_master_sock(*memory_router->targ_socket[1]); // CPU>DCUWR <-> Memory Router
		linux_os_cpu->dcurd_plb_master_sock(*memory_router->targ_socket[2]); // CPU>DCURD <-> Memory Router
		(*memory_router->init_socket[0])(ram->slave_sock); // Memory Router <-> RAM
		external_input_interrupt_stub->master_sock(linux_os_cpu->external_input_interrupt_slave_sock);
		critical_input_interrupt_stub->master_sock(linux_os_cpu->critical_input_interrupt_slave_sock);
		linux_os_cpu->dcr_master_sock(dcr_slave_stub->slave_sock);
	}
	else
	{
		cpu->icurd_plb_master_sock(crossbar->icurd_plb_slave_sock); // CPU>ICURD <-> ICURD<Crossbar
		cpu->dcuwr_plb_master_sock(crossbar->dcuwr_plb_slave_sock); // CPU>DCUWR <-> DCUWR<Crossbar
		cpu->dcurd_plb_master_sock(crossbar->dcurd_plb_slave_sock); // CPU>DCURD <-> DCURD<Crossbar
		cpu->dcr_master_sock(*dcr_controller->dcr_slave_sock[0]); // (master 0) CPU>DCR <-> DCR controller
		master1_dcr_stub->master_sock(*dcr_controller->dcr_slave_sock[1]); // master 1>DCR <-> DCR controller
		
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::APU_SLAVE_NUM])(apu_dcr_stub->slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::MCI_SLAVE_NUM])(mci->dcr_slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::CROSSBAR_SLAVE_NUM])(crossbar->crossbar_dcr_slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::PLBS0_SLAVE_NUM])(crossbar->plbs0_dcr_slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::PLBS1_SLAVE_NUM])(crossbar->plbs1_dcr_slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::PLBM_SLAVE_NUM])(crossbar->plbm_dcr_slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::DMAC0_SLAVE_NUM])(dmac0_dcr_stub->slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::DMAC1_SLAVE_NUM])(dmac1_dcr_stub->slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::DMAC2_SLAVE_NUM])(dmac2_dcr_stub->slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::DMAC3_SLAVE_NUM])(dmac3_dcr_stub->slave_sock);
		(*dcr_controller->dcr_master_sock[CONFIG::DCR_CONTROLLER_CONFIG::EXTERNAL_SLAVE_NUM])(external_slave_dcr_stub->slave_sock);
		
		crossbar->mplb_master_sock(*mplb->targ_socket[0]);   // crossbar>MPLB <-> MPLB
		crossbar->mci_master_sock(mci->mci_slave_sock);      // crossbar>MCI <-> MCI
		mci->mci_master_sock(ram->slave_sock);               // MCI <-> RAM 
		
		splb0_stub->master_sock(crossbar->splb0_slave_sock);  // SPLB0 stub <-> SPLB0<Crossbar
		splb1_stub->master_sock(crossbar->splb1_slave_sock);  // SPLB1 stub <-> SPLB1<Crossbar
		
		(*mplb->init_socket[CONFIG::INTC_CONFIG::MPLB_PORT])(intc->slave_sock);      // MPLB <-> INTC
		(*mplb->init_socket[CONFIG::TIMER_CONFIG::MPLB_PORT])(timer->slave_sock);     // MPLB <-> TIMER
		(*mplb->init_socket[CONFIG::FLASH_MPLB_PORT])(flash->slave_sock);     // MPLB <-> FLASH
		(*mplb->init_socket[CONFIG::BRAM_MPLB_PORT])(bram->slave_sock);      // MPLB <-> BRAM
		(*mplb->init_socket[CONFIG::UART_LITE_CONFIG::MPLB_PORT])(uart_lite->slave_sock); // MPLB <-> UART Lite
		(*mplb->init_socket[CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG::MPLB_PORT])(gpio_dip_switches_8bit->slave_sock);      // MPLB <-> GPIO DIP switches 8 Bit
		(*mplb->init_socket[CONFIG::GPIO_LEDS_8BIT_CONFIG::MPLB_PORT])(gpio_leds_8bit->slave_sock);              // MPLB <-> GPIO LEDs 8 Bit
		(*mplb->init_socket[CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG::MPLB_PORT])(gpio_5_leds_positions->slave_sock);       // MPLB <-> GPIO 5 LEDs Positions
		(*mplb->init_socket[CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG::MPLB_PORT])(gpio_push_buttons_5bit->slave_sock);      // MPLB <-> GPIO Push Buttons 5 bit
		
		for(irq = 0; irq < CONFIG::INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
		{
			switch(irq)
			{
				case CONFIG::TIMER_IRQ:
					timer->interrupt_master_sock(*intc->irq_slave_sock[irq]); // TIMER>IRQ <-> INTR<INTC
					break;
				case CONFIG::UART_LITE_IRQ:
					uart_lite->interrupt_master_sock(*intc->irq_slave_sock[irq]); // UART Lite>IRQ <-> INTR<INTC
					break;
				case CONFIG::GPIO_DIP_SWITCHES_8BIT_IRQ:
					(*gpio_dip_switches_8bit->interrupt_master_sock)(*intc->irq_slave_sock[irq]); // GPIO DIP SWITCHES 8BIT>IRQ <-> INTR<INTC
					break;
				case CONFIG::GPIO_PUSH_BUTTONS_5BIT_IRQ:
					(*gpio_push_buttons_5bit->interrupt_master_sock)(*intc->irq_slave_sock[irq]); // GPIO PUSH BUTTONS 5BIT>IRQ <-> INTR<INTC
					break;
				default:
					(input_interrupt_stub[irq]->master_sock)(*intc->irq_slave_sock[irq]); // IRQ stub>IRQ <-> INTR<INTC
					break;
			}
		}
		for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
		{
			capture_trigger_stub[channel]->master_sock(*timer->capture_trigger_slave_sock[channel]); // Capture trigger stub <-> TIMER
		}
		for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
		{
			(*timer->generate_out_master_sock[channel])(generate_out_stub[channel]->slave_sock); // TIMER <-> Generate out stub
		}
		timer->pwm_master_sock(pwm_stub->slave_sock); // TIMER <-> PWM stub
		intc->irq_master_sock(cpu->external_input_interrupt_slave_sock); // INTC>IRQ <-> External Input<CPU
		critical_input_interrupt_stub->master_sock(cpu->critical_input_interrupt_slave_sock); // IRQ Stub <-> CPU
		
		for(i = 0; i < CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG::C_GPIO_WIDTH; i++)
		{
			(*dip_switches_8bit->gpio_master_sock[i])(*gpio_dip_switches_8bit->gpio_slave_sock[i]);
			(*gpio_dip_switches_8bit->gpio_master_sock[i])(*dip_switches_8bit->gpio_slave_sock[i]);
		}
		for(i = 0; i < CONFIG::GPIO_LEDS_8BIT_CONFIG::C_GPIO_WIDTH; i++)
		{
			(*gpio_leds_8bit->gpio_master_sock[i])(*leds_8bit->gpio_slave_sock[i]);
			(*leds_8bit->gpio_master_sock[i])(*gpio_leds_8bit->gpio_slave_sock[i]);
		}
		for(i = 0; i < CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG::C_GPIO_WIDTH; i++)
		{
			(*gpio_5_leds_positions->gpio_master_sock[i])(*_5_leds_positions->gpio_slave_sock[i]);
			(*_5_leds_positions->gpio_master_sock[i])(*gpio_5_leds_positions->gpio_slave_sock[i]);
		}
		for(i = 0; i < CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_GPIO_WIDTH; i++)
		{
			(*push_buttons_5bit->gpio_master_sock[i])(*gpio_push_buttons_5bit->gpio_slave_sock[i]);
			(*gpio_push_buttons_5bit->gpio_master_sock[i])(*push_buttons_5bit->gpio_slave_sock[i]);
		}
	}

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	if(enable_linux_os)
	{
		linux_os_cpu->memory_import >> ram->memory_export;
		linux_os_cpu->linux_os_import >> linux_os->linux_os_export_;
		linux_os->registers_import_ >> linux_os_cpu->registers_export;
		linux_os->memory_import_ >> ram_effective_to_physical_address_translator->memory_export;
		linux_os->memory_injection_import_ >> linux_os_cpu->memory_injection_export;
		ram_effective_to_physical_address_translator->memory_import >> ram->memory_export;
		
		// Connect debugger to CPU
		linux_os_cpu->debug_control_import >> debugger->debug_control_export;
		linux_os_cpu->trap_reporting_import >> debugger->trap_reporting_export;
		linux_os_cpu->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
		debugger->disasm_import >> linux_os_cpu->disasm_export;
		debugger->memory_import >> linux_os_cpu->memory_export;
		debugger->registers_import >> linux_os_cpu->registers_export;
		debugger->blob_import >> linux_os->blob_export_;
		
		if(enable_inline_debugger)
		{
			// Connect tee-memory-access-reporting to CPU, debugger and profiler
			linux_os_cpu->memory_access_reporting_import >> tee_memory_access_reporting->in;
			*tee_memory_access_reporting->out[0] >> profiler->memory_access_reporting_export;
			*tee_memory_access_reporting->out[1] >> debugger->memory_access_reporting_export;
			profiler->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[0];
			debugger->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[1];
			tee_memory_access_reporting->out_control >> linux_os_cpu->memory_access_reporting_control_export;
		}
		else
		{
			linux_os_cpu->memory_access_reporting_import >> debugger->memory_access_reporting_export;
			debugger->memory_access_reporting_control_import >> linux_os_cpu->memory_access_reporting_control_export;
		}
		
		if(enable_inline_debugger)
		{
			// Connect inline-debugger to debugger
			debugger->debug_event_listener_import >> inline_debugger->debug_event_listener_export;
			debugger->trap_reporting_import >> inline_debugger->trap_reporting_export;
			debugger->debug_control_import >> inline_debugger->debug_control_export;
			inline_debugger->debug_event_trigger_import >> debugger->debug_event_trigger_export;
			inline_debugger->disasm_import >> debugger->disasm_export;
			inline_debugger->memory_import >> debugger->memory_export;
			inline_debugger->registers_import >> debugger->registers_export;
			inline_debugger->stmt_lookup_import >> debugger->stmt_lookup_export;
			inline_debugger->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
			inline_debugger->backtrace_import >> debugger->backtrace_export;
			inline_debugger->debug_info_loading_import >> debugger->debug_info_loading_export;
			inline_debugger->data_object_lookup_import >> debugger->data_object_lookup_export;
			inline_debugger->subprogram_lookup_import >> debugger->subprogram_lookup_export;
			inline_debugger->profiling_import >> profiler->profiling_export;
		}
		else if(enable_gdb_server)
		{
			// Connect gdb-server to debugger
			debugger->debug_control_import >> gdb_server->debug_control_export;
			debugger->debug_event_listener_import >> gdb_server->debug_event_listener_export;
			debugger->trap_reporting_import >> gdb_server->trap_reporting_export;
			gdb_server->debug_event_trigger_import >> debugger->debug_event_trigger_export;
			gdb_server->memory_import >> debugger->memory_export;
			gdb_server->registers_import >> debugger->registers_export;
		}
		
		if(estimate_power)
		{
			// Connect everything related to power estimation
			linux_os_cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
			linux_os_cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
			linux_os_cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
			linux_os_cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;
			linux_os_cpu->itlb_power_estimator_import >> itlb_power_estimator->power_estimator_export;
			linux_os_cpu->itlb_power_mode_import >> itlb_power_estimator->power_mode_export;
			linux_os_cpu->dtlb_power_estimator_import >> dtlb_power_estimator->power_estimator_export;
			linux_os_cpu->dtlb_power_mode_import >> dtlb_power_estimator->power_mode_export;
			linux_os_cpu->utlb_power_estimator_import >> utlb_power_estimator->power_estimator_export;
			linux_os_cpu->utlb_power_mode_import >> utlb_power_estimator->power_mode_export;

			il1_power_estimator->time_import >> sim_time->time_export;
			dl1_power_estimator->time_import >> sim_time->time_export;
			itlb_power_estimator->time_import >> sim_time->time_export;
			dtlb_power_estimator->time_import >> sim_time->time_export;
			utlb_power_estimator->time_import >> sim_time->time_export;
		}
	}
	else
	{
		cpu->memory_import >> crossbar->memory_export;
		
		crossbar->mci_memory_import >> mci->memory_export;
		crossbar->mplb_memory_import >> mplb->memory_export;
		mci->memory_import >> ram->memory_export;
		(*mplb->memory_import[CONFIG::INTC_CONFIG::MPLB_PORT]) >> intc->memory_export;
		(*mplb->memory_import[CONFIG::TIMER_CONFIG::MPLB_PORT]) >> timer->memory_export;
		(*mplb->memory_import[CONFIG::FLASH_MPLB_PORT]) >> flash->memory_export;
		(*mplb->memory_import[CONFIG::BRAM_MPLB_PORT]) >> bram->memory_export;
		(*mplb->memory_import[CONFIG::UART_LITE_CONFIG::MPLB_PORT]) >> uart_lite->memory_export;
		(*mplb->memory_import[CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG::MPLB_PORT]) >> gpio_dip_switches_8bit->memory_export;
		(*mplb->memory_import[CONFIG::GPIO_LEDS_8BIT_CONFIG::MPLB_PORT]) >> gpio_leds_8bit->memory_export;
		(*mplb->memory_import[CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG::MPLB_PORT]) >> gpio_5_leds_positions->memory_export;
		(*mplb->memory_import[CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG::MPLB_PORT]) >> gpio_push_buttons_5bit->memory_export;
		cpu->loader_import >> loader->loader_export;
		
		if(enable_inline_debugger || enable_gdb_server)
		{
			if(enable_inline_debugger)
			{
				// Connect tee-memory-access-reporting to CPU, debugger and profiler
				cpu->memory_access_reporting_import >> tee_memory_access_reporting->in;
				*tee_memory_access_reporting->out[0] >> profiler->memory_access_reporting_export;
				*tee_memory_access_reporting->out[1] >> debugger->memory_access_reporting_export;
				profiler->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[0];
				debugger->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[1];
				tee_memory_access_reporting->out_control >> cpu->memory_access_reporting_control_export;
			}
			else
			{
				// Connect CPU to debugger
				cpu->memory_access_reporting_import >> debugger->memory_access_reporting_export;
				debugger->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
			}

			// Connect debugger to CPU
			cpu->debug_control_import >> debugger->debug_control_export;
			cpu->trap_reporting_import >> debugger->trap_reporting_export;
			debugger->disasm_import >> cpu->disasm_export;
			debugger->memory_import >> cpu->memory_export;
			debugger->registers_import >> cpu->registers_export;
			debugger->loader_import >> loader->loader_export;
			debugger->blob_import >> loader->blob_export;
		}
		
		if(enable_inline_debugger)
		{
			// Connect inline-debugger to debugger
			debugger->debug_event_listener_import >> inline_debugger->debug_event_listener_export;
			debugger->trap_reporting_import >> inline_debugger->trap_reporting_export;
			debugger->debug_control_import >> inline_debugger->debug_control_export;
			inline_debugger->debug_event_trigger_import >> debugger->debug_event_trigger_export;
			inline_debugger->disasm_import >> debugger->disasm_export;
			inline_debugger->memory_import >> debugger->memory_export;
			inline_debugger->registers_import >> debugger->registers_export;
			inline_debugger->stmt_lookup_import >> debugger->stmt_lookup_export;
			inline_debugger->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
			inline_debugger->backtrace_import >> debugger->backtrace_export;
			inline_debugger->debug_info_loading_import >> debugger->debug_info_loading_export;
			inline_debugger->data_object_lookup_import >> debugger->data_object_lookup_export;
			inline_debugger->subprogram_lookup_import >> debugger->subprogram_lookup_export;
			inline_debugger->profiling_import >> profiler->profiling_export;
		}
		else if(enable_gdb_server)
		{
			// Connect gdb-server to debugger
			debugger->debug_control_import >> gdb_server->debug_control_export;
			debugger->debug_event_listener_import >> gdb_server->debug_event_listener_export;
			debugger->trap_reporting_import >> gdb_server->trap_reporting_export;
			gdb_server->debug_event_trigger_import >> debugger->debug_event_trigger_export;
			gdb_server->memory_import >> debugger->memory_export;
			gdb_server->registers_import >> debugger->registers_export;
		}

		ram_effective_to_physical_address_translator->memory_import >> ram->memory_export;
		flash_effective_to_physical_address_translator->memory_import >> flash->memory_export;
		bram_effective_to_physical_address_translator->memory_import >> bram->memory_export;
		*loader->memory_import[0] >> ram_effective_to_physical_address_translator->memory_export;
		*loader->memory_import[1] >> bram_effective_to_physical_address_translator->memory_export;
		*loader->memory_import[2] >> flash_effective_to_physical_address_translator->memory_export;
		loader->registers_import >> cpu->registers_export;
		cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;

		if(enable_telnet)
		{
			uart_lite->char_io_import >> telnet->char_io_export;
		}
		
		if(estimate_power)
		{
			// Connect everything related to power estimation
			cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
			cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
			cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
			cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;
			cpu->itlb_power_estimator_import >> itlb_power_estimator->power_estimator_export;
			cpu->itlb_power_mode_import >> itlb_power_estimator->power_mode_export;
			cpu->dtlb_power_estimator_import >> dtlb_power_estimator->power_estimator_export;
			cpu->dtlb_power_mode_import >> dtlb_power_estimator->power_mode_export;
			cpu->utlb_power_estimator_import >> utlb_power_estimator->power_estimator_export;
			cpu->utlb_power_mode_import >> utlb_power_estimator->power_mode_export;

			il1_power_estimator->time_import >> sim_time->time_export;
			dl1_power_estimator->time_import >> sim_time->time_export;
			itlb_power_estimator->time_import >> sim_time->time_export;
			dtlb_power_estimator->time_import >> sim_time->time_export;
			utlb_power_estimator->time_import >> sim_time->time_export;
		}
	}
}

template <class CONFIG>
Simulator<CONFIG>::~Simulator()
{
	unsigned int irq;
	unsigned int channel;
	if(critical_input_interrupt_stub) delete critical_input_interrupt_stub;
	if(external_input_interrupt_stub) delete external_input_interrupt_stub;
	if(ram) delete ram;
	if(memory_router) delete memory_router;
	if(bram) delete bram;
	if(debugger) delete debugger;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(profiler) delete profiler;
	if(cpu) delete cpu;
	if(linux_os_cpu) delete linux_os_cpu;
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
	if(dcr_slave_stub) delete dcr_slave_stub;
	for(irq = 0; irq < CONFIG::INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		if(input_interrupt_stub[irq]) delete input_interrupt_stub[irq];
	}
	for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(capture_trigger_stub[channel]) delete capture_trigger_stub[channel];
	}
	// - Generate out stubs
	for(channel = 0; channel < CONFIG::TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(generate_out_stub[channel]) delete generate_out_stub[channel];
	}
	if(pwm_stub) delete pwm_stub;
	if(dcr_controller) delete dcr_controller;
	if(il1_power_estimator) delete il1_power_estimator;
	if(dl1_power_estimator) delete dl1_power_estimator;
	if(itlb_power_estimator) delete itlb_power_estimator;
	if(dtlb_power_estimator) delete dtlb_power_estimator;
	if(utlb_power_estimator) delete utlb_power_estimator;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(loader) delete loader;
	if(ram_effective_to_physical_address_translator) delete ram_effective_to_physical_address_translator;
	if(bram_effective_to_physical_address_translator) delete bram_effective_to_physical_address_translator;
	if(flash_effective_to_physical_address_translator) delete flash_effective_to_physical_address_translator;
	if(telnet) delete telnet;
	if(linux_os) delete linux_os;
	if(tee_memory_access_reporting) delete tee_memory_access_reporting;
}

template <class CONFIG>
void Simulator<CONFIG>::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
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
	const char *gdb_server_arch_filename = "gdb_powerpc.xml";
	const char *dwarf_register_number_mapping_filename = "powerpc_eabi_gcc_dwarf_register_number_mapping.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 400.0; // in Mhz
	double cpu_clock_multiplier = 2.0;
	double ext_timer_clock_divisor = 2.0;
	uint32_t tech_node = 65; // in nm
	double cpu_ipc = 2.0; // in instructions per cycle: PPC440 is a superscalar processor that can execute "out-of-order" up to 2 instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double ext_timer_cycle_time = ext_timer_clock_divisor * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", sc_time(cpu_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.bus-cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.ext-timer-cycle-time", sc_time(ext_timer_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.voltage", 1.0 * 1e3); // mV
	simulator->SetVariable("cpu.max-inst", maxinst);
	simulator->SetVariable("cpu.nice-time", "200 ns"); // 200 ns (currently geared to the minimum interval between capture trigger samples)
	simulator->SetVariable("cpu.ipc", cpu_ipc);
	simulator->SetVariable("cpu.enable-dmi", true); // Allow CPU to use of SystemC TLM 2.0 DMI

	//  - DCR controller
	simulator->SetVariable("dcr-controller.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Crossbar
	simulator->SetVariable("crossbar.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - MCI
	simulator->SetVariable("mci.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - MPLB
	simulator->SetVariable("mplb.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Memory router
	simulator->SetVariable("memory-router.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory-router.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffffffffffff\" output_port=\"0\" translation=\"0x0\""); // RAM

	unsigned int mapping_num = 0;
	
	std::stringstream sstr_intc_mapping_name;
	sstr_intc_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_intc_mapping;
	sstr_intc_mapping << "range_start=\"0x" << std::hex << CONFIG::INTC_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << CONFIG::INTC_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << CONFIG::INTC_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::INTC_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_intc_mapping_name.str().c_str(), sstr_intc_mapping.str().c_str()); // XPS IntC

	std::stringstream sstr_timer_mapping_name;
	sstr_timer_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_timer_mapping;
	sstr_timer_mapping << "range_start=\"0x" << std::hex << CONFIG::TIMER_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << CONFIG::TIMER_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << CONFIG::TIMER_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::TIMER_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_timer_mapping_name.str().c_str(), sstr_timer_mapping.str().c_str()); // XPS Timer/Counter
	
	std::stringstream sstr_flash_mapping_name;
	sstr_flash_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_flash_mapping;
	sstr_flash_mapping << "range_start=\"0x" << std::hex << CONFIG::FLASH_BASE_ADDR << std::dec << "\" range_end=\"0x" << std::hex << (CONFIG::FLASH_BASE_ADDR + CONFIG::FLASH_BYTE_SIZE - 1) << std::dec << "\" output_port=\"" << CONFIG::FLASH_MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::FLASH_BASE_ADDR << std::dec << "\"";
	simulator->SetVariable(sstr_flash_mapping_name.str().c_str(), sstr_flash_mapping.str().c_str()); // 32 MB Flash memory (i.e. 1 * 256 Mbits S29GL256P flash memory chips)
	
	std::stringstream sstr_bram_mapping_name;
	sstr_bram_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_bram_mapping;
	sstr_bram_mapping << "range_start=\"0x" << std::hex << CONFIG::BRAM_BASE_ADDR << std::dec << "\" range_end=\"0x" << std::hex << (CONFIG::BRAM_BASE_ADDR + CONFIG::BRAM_BYTE_SIZE - 1) << std::dec << "\" output_port=\"" << CONFIG::BRAM_MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::BRAM_BASE_ADDR << std::dec << "\"";
	simulator->SetVariable(sstr_bram_mapping_name.str().c_str(), sstr_bram_mapping.str().c_str()); // 256 KB XPS BRAM

	std::stringstream sstr_uart_lite_mapping_name;
	sstr_uart_lite_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_uart_lite_mapping;
	sstr_uart_lite_mapping << "range_start=\"0x" << std::hex << CONFIG::UART_LITE_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << CONFIG::UART_LITE_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << CONFIG::UART_LITE_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::UART_LITE_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_uart_lite_mapping_name.str().c_str(), sstr_uart_lite_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_dip_switches_8bit_mapping_name;
	sstr_gpio_dip_switches_8bit_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_dip_switches_8bit_mapping;
	sstr_gpio_dip_switches_8bit_mapping << "range_start=\"0x" << std::hex << CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_dip_switches_8bit_mapping_name.str().c_str(), sstr_gpio_dip_switches_8bit_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_leds_8bit_mapping_name;
	sstr_gpio_leds_8bit_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_leds_8bit_mapping;
	sstr_gpio_leds_8bit_mapping << "range_start=\"0x" << std::hex << CONFIG::GPIO_LEDS_8BIT_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << CONFIG::GPIO_LEDS_8BIT_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << CONFIG::GPIO_LEDS_8BIT_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::GPIO_LEDS_8BIT_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_leds_8bit_mapping_name.str().c_str(), sstr_gpio_leds_8bit_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_5_leds_positions_mapping_name;
	sstr_gpio_5_leds_positions_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_5_leds_positions_mapping;
	sstr_gpio_5_leds_positions_mapping << "range_start=\"0x" << std::hex << CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_5_leds_positions_mapping_name.str().c_str(), sstr_gpio_5_leds_positions_mapping.str().c_str()); // XPS Timer/Counter

	std::stringstream sstr_gpio_push_buttons_5bit_mapping_name;
	sstr_gpio_push_buttons_5bit_mapping_name << "mplb.mapping_" << mapping_num++;
	std::stringstream sstr_gpio_push_buttons_5bit_mapping;
	sstr_gpio_push_buttons_5bit_mapping << "range_start=\"0x" << std::hex << CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_BASEADDR << std::dec << "\" range_end=\"0x" << std::hex << CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_HIGHADDR << std::dec << "\" output_port=\"" << CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG::MPLB_PORT << "\" translation=\"0x" << std::hex << CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_BASEADDR << std::dec << "\"";
	simulator->SetVariable(sstr_gpio_push_buttons_5bit_mapping_name.str().c_str(), sstr_gpio_push_buttons_5bit_mapping.str().c_str()); // XPS Timer/Counter

	// - Loader memory router
	std::stringstream sstr_loader_mapping;
	sstr_loader_mapping << "ram-effective-to-physical-address-translator:0x" << std::hex << CONFIG::RAM_BASE_ADDR << std::dec << "-0x" << std::hex << (CONFIG::RAM_BASE_ADDR + CONFIG::RAM_BYTE_SIZE - 1) << std::dec;
	sstr_loader_mapping << ",bram-effective-to-physical-address-translator:0x" << std::hex << CONFIG::BRAM_BASE_ADDR << std::dec << "-0x" << std::hex << (CONFIG::BRAM_BASE_ADDR + CONFIG::BRAM_BYTE_SIZE - 1) << std::dec;
	sstr_loader_mapping << ",flash-effective-to-physical-address-translator:0x" << std::hex << CONFIG::FLASH_BASE_ADDR << std::dec << "-0x" << std::hex << (CONFIG::FLASH_BASE_ADDR + CONFIG::FLASH_BYTE_SIZE - 1) << std::dec;
	simulator->SetVariable("loader.memory-mapper.mapping", sstr_loader_mapping.str().c_str()); // 256 MB RAM / 256 KB BRAM / 32 MB Flash memory

	//  - RAM
	simulator->SetVariable("ram.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("ram.org", CONFIG::RAM_BASE_ADDR);
	simulator->SetVariable("ram.bytesize", CONFIG::RAM_BYTE_SIZE);

	//  - BRAM
	simulator->SetVariable("bram.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("bram.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("bram.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("bram.org", CONFIG::BRAM_BASE_ADDR);
	simulator->SetVariable("bram.bytesize", CONFIG::BRAM_BYTE_SIZE);
	
	//  - Interrupt controller
	simulator->SetVariable("intc.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Timer
	simulator->SetVariable("timer.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - UART Lite
	simulator->SetVariable("uart-lite.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - GPIOs
	simulator->SetVariable("gpio-dip-switches-8bit.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("gpio-leds-8bit.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("gpio-5-leds-positions.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("gpio-push-buttons-5bit.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Flash
	simulator->SetVariable("flash.org", CONFIG::FLASH_BASE_ADDR);
	simulator->SetVariable("flash.bytesize", CONFIG::FLASH_BYTE_SIZE);
	simulator->SetVariable("flash.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	
	//  - Capture Trigger stubs
	simulator->SetVariable("capture-trigger-stub0.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("capture-trigger-stub0.nice-time", "1 ms");
	simulator->SetVariable("capture-trigger-stub1.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("capture-trigger-stub1.nice-time", "1 ms");
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", dwarf_register_number_mapping_filename);

	//  - Cache/TLB power estimators run-time configuration
	simulator->SetVariable("il1-power-estimator.cache-size", CONFIG::CPU_CONFIG::DL1_CONFIG::CACHE_SIZE);
	simulator->SetVariable("il1-power-estimator.line-size", CONFIG::CPU_CONFIG::DL1_CONFIG::CACHE_BLOCK_SIZE);
	simulator->SetVariable("il1-power-estimator.associativity", 0); // fully associative
	simulator->SetVariable("il1-power-estimator.rw-ports", 0);
	simulator->SetVariable("il1-power-estimator.excl-read-ports", 1);
	simulator->SetVariable("il1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("il1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("il1-power-estimator.banks", 4);
	simulator->SetVariable("il1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("il1-power-estimator.output-width", 64);
	simulator->SetVariable("il1-power-estimator.tag-width", CONFIG::CPU_CONFIG::ICDBTRL_BITSIZE + CONFIG::CPU_CONFIG::ICDBTRH_BITSIZE);
	simulator->SetVariable("il1-power-estimator.access-mode", "fast");

	simulator->SetVariable("dl1-power-estimator.cache-size", CONFIG::CPU_CONFIG::IL1_CONFIG::CACHE_SIZE);
	simulator->SetVariable("dl1-power-estimator.line-size", CONFIG::CPU_CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE);
	simulator->SetVariable("dl1-power-estimator.associativity", 0); // fully associative
	simulator->SetVariable("dl1-power-estimator.rw-ports", 1);
	simulator->SetVariable("dl1-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dl1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.banks", 4);
	simulator->SetVariable("dl1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dl1-power-estimator.output-width", 64);
	simulator->SetVariable("dl1-power-estimator.tag-width", CONFIG::CPU_CONFIG::DCDBTRL_BITSIZE + CONFIG::CPU_CONFIG::DCDBTRH_BITSIZE);
	simulator->SetVariable("dl1-power-estimator.access-mode", "fast");

	simulator->SetVariable("itlb-power-estimator.cache-size", CONFIG::CPU_CONFIG::ITLB_CONFIG::TLB_NUM_ENTRIES * (CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("itlb-power-estimator.line-size", (CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("itlb-power-estimator.associativity", 0); // fully associative
	simulator->SetVariable("itlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("itlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("itlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.banks", 4);
	simulator->SetVariable("itlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("itlb-power-estimator.output-width", CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE);
	simulator->SetVariable("itlb-power-estimator.tag-width", CONFIG::CPU_CONFIG::TLBE_TAG_BITSIZE);
	simulator->SetVariable("itlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("dtlb-power-estimator.cache-size", CONFIG::CPU_CONFIG::DTLB_CONFIG::TLB_NUM_ENTRIES * (CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("dtlb-power-estimator.line-size", (CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("dtlb-power-estimator.associativity", 0); // fully associative
	simulator->SetVariable("dtlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("dtlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.banks", 4);
	simulator->SetVariable("dtlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dtlb-power-estimator.output-width", CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE);
	simulator->SetVariable("dtlb-power-estimator.tag-width", CONFIG::CPU_CONFIG::TLBE_TAG_BITSIZE);
	simulator->SetVariable("dtlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("utlb-power-estimator.cache-size", CONFIG::CPU_CONFIG::UTLB_CONFIG::TLB_NUM_ENTRIES * (CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("utlb-power-estimator.line-size", (CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("utlb-power-estimator.associativity", 0); // fully associative
	simulator->SetVariable("utlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("utlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("utlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("utlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("utlb-power-estimator.banks", 4);
	simulator->SetVariable("utlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("utlb-power-estimator.output-width", CONFIG::CPU_CONFIG::TLBE_DATA_BITSIZE);
	simulator->SetVariable("utlb-power-estimator.tag-width", CONFIG::CPU_CONFIG::TLBE_TAG_BITSIZE);
	simulator->SetVariable("utlb-power-estimator.access-mode", "fast");

	//  - Linux OS run-time configuration
	simulator->SetVariable("linux-os.endianness", "big-endian");
	simulator->SetVariable("linux-os.stack-base", 0xc0000000);
	simulator->SetVariable("linux-os.envc", 0);
	simulator->SetVariable("linux-os.system", "ppc");
	simulator->SetVariable("linux-os.endianness", "big-endian");
	simulator->SetVariable("linux-os.utsname-sysname", "Linux");
 	simulator->SetVariable("linux-os.utsname-nodename", "(none)");
 	simulator->SetVariable("linux-os.utsname-release", "3.0.4");
 	simulator->SetVariable("linux-os.utsname-version", "#1 PREEMPT Thu Jan 1 00:00:00 CEST 1970");
	simulator->SetVariable("linux-os.utsname-machine", "ppc");
	simulator->SetVariable("linux-os.utsname-domainname", "(none)");
	simulator->SetVariable("linux-os.apply-host-environment", false);
}

template <class CONFIG>
void Simulator<CONFIG>::Run()
{
	cerr << "Starting simulation at " << (enable_linux_os ? "user" : "supervisor") << " privilege level" << endl;
	
	double time_start = host_time->GetTime();

#ifndef WIN32
	void (*prev_sig_int_handler)(int) = 0;
#endif

	if(!inline_debugger)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&Simulator<CONFIG>::ConsoleCtrlHandler, TRUE);
#else
		prev_sig_int_handler = signal(SIGINT, &Simulator<CONFIG>::SigIntHandler);
#endif
	}

	sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages
	
	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	if(!inline_debugger)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&Simulator<CONFIG>::ConsoleCtrlHandler, FALSE);
#else
		signal(SIGINT, prev_sig_int_handler);
#endif
	}

	cerr << "Simulation finished" << endl;

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	cerr << "Simulation run-time parameters:" << endl;
	DumpParameters(cerr);
	cerr << endl;
	cerr << "Simulation formulas:" << endl;
	DumpFormulas(cerr);
	cerr << endl;
	cerr << "Simulation statistics:" << endl;
	DumpStatistics(cerr);
	cerr << endl;

	cerr << "simulation time: " << spent_time << " seconds" << endl;
	cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
	if(enable_linux_os)
	{
		cerr << "target speed: " << ((double) (*linux_os_cpu)["instruction-counter"] / ((double) (*linux_os_cpu)["run-time"] - (double) (*linux_os_cpu)["idle-time"]) / 1000000.0) << " MIPS" << endl;
		cerr << "host simulation speed: " << ((double) (*linux_os_cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
	}
	else
	{
		cerr << "target speed: " << ((double) (*cpu)["instruction-counter"] / ((double) (*cpu)["run-time"] - (double) (*cpu)["idle-time"]) / 1000000.0) << " MIPS" << endl;
		cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
	}
	cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
}

template <class CONFIG>
unisim::kernel::service::Simulator::SetupStatus Simulator<CONFIG>::Setup()
{
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	if(enable_linux_os)
	{
		SetVariable("ram.org", 0);
		SetVariable("ram.bytesize", 0x100000000ULL); // a 4 GB addressable RAM
		
		// Build the Linux OS arguments from the command line arguments
		
		VariableBase *cmd_args = FindVariable("cmd-args");
		unsigned int cmd_args_length = cmd_args->GetLength();
		if(cmd_args_length > 0)
		{
			SetVariable("linux-os.binary", ((string)(*cmd_args)[0]).c_str());
			SetVariable("linux-os.argc", cmd_args_length);
			
			unsigned int i;
			for(i = 0; i < cmd_args_length; i++)
			{
				std::stringstream sstr;
				sstr << "linux-os.argv[" << i << "]";
				SetVariable(sstr.str().c_str(), ((string)(*cmd_args)[i]).c_str());
			}
		}
		
		// Relax time decoupling
		SetVariable("cpu.nice-time", "1 ms");
	}
	else
	{
		// Optionally get the program to load from the command line arguments
		VariableBase *cmd_args = FindVariable("cmd-args");
		unsigned int cmd_args_length = cmd_args->GetLength();
		if(cmd_args_length > 0)
		{
			SetVariable("loader.filename", ((string)(*cmd_args)[0]).c_str());
		}
	}

	unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();
	
	// inline-debugger and gdb-server are exclusive
	if(enable_inline_debugger && enable_gdb_server)
	{
		std::cerr << "ERROR! " << inline_debugger->GetName() << " and " << gdb_server->GetName() << " shall not be used together. Use " << param_enable_inline_debugger.GetName() << " and " << param_enable_gdb_server.GetName() << " to enable only one of the two" << std::endl;
		if(setup_status != unisim::kernel::service::Simulator::ST_OK_DONT_START)
		{
			setup_status = unisim::kernel::service::Simulator::ST_ERROR;
		}
	}
	
	return setup_status;
}

template <class CONFIG>
void Simulator<CONFIG>::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	if(CONFIG::DEBUG_INFORMATION)
	{
		std::cerr << "Call stack:" << std::endl;
		std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	if(!asynchronous)
	{
		switch(sc_get_curr_simcontext()->get_curr_proc_info()->kind)
		{
			case SC_THREAD_PROC_: 
			case SC_CTHREAD_PROC_:
				wait();
				break;
			default:
				break;
		}
	}
}

template <class CONFIG>
int Simulator<CONFIG>::GetExitStatus() const
{
	return exit_status;
}

#ifdef WIN32
template <class CONFIG>
BOOL WINAPI Simulator<CONFIG>::ConsoleCtrlHandler(DWORD dwCtrlType)
{
	bool stop = false;
	switch(dwCtrlType)
	{
		case CTRL_C_EVENT:
			cerr << "Interrupted by Ctrl-C" << endl;
			stop = true;
			break;
		case CTRL_BREAK_EVENT:
			cerr << "Interrupted by Ctrl-Break" << endl;
			stop = true;
			break;
		case CTRL_CLOSE_EVENT:
			cerr << "Interrupted by a console close" << endl;
			stop = true;
			break;
		case CTRL_LOGOFF_EVENT:
			cerr << "Interrupted because of logoff" << endl;
			stop = true;
			break;
		case CTRL_SHUTDOWN_EVENT:
			cerr << "Interrupted because of shutdown" << endl;
			stop = true;
			break;
	}
	if(stop) sc_stop();
	return stop ? TRUE : FALSE;
}
#else
template <class CONFIG>
void Simulator<CONFIG>::SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}
#endif

#endif // __VIRTEX5FXT_SIMULATOR_TCC__
