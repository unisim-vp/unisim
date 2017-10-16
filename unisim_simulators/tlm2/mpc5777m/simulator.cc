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

Simulator::Simulator(const sc_core::sc_module_name& name, int argc, char **argv)
	: unisim::kernel::tlm2::Simulator(name, argc, argv, LoadBuiltInConfig)
	, main_core_0(0)
	, main_core_1(0)
	, peripheral_core_2(0)
	, system_sram(0)
	, flash(0)
	, xbar_0(0)
	, xbar_1(0)
	, pbridge_a(0)
	, pbridge_b(0)
	, intc_0(0)
	, stm_0(0)
	, stm_1(0)
	, stm_2(0)
	, swt_0(0)
	, swt_1(0)
	, swt_2(0)
	, swt_3(0)
	, pit_0(0)
	, pit_1(0)
	, linflexd_0(0)
    , linflexd_1(0)
    , linflexd_2(0)
    , linflexd_14(0)
    , linflexd_15(0)
    , linflexd_16(0)
	, ebi_stub(0)
	, flash_port1_stub(0)
	, xbar_0_s6_stub(0)
	, xbar_1_m1_stub(0)
	, xbar_1_m2_stub(0)
	, loader(0)
	, debugger(0)
	, gdb_server()
	, inline_debugger()
	, profiler()
	, sim_time(0)
	, host_time(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, enable_profiler(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_enable_profiler("enable-profiler", 0, enable_profiler, "Enable/Disable profiling")
	, exit_status(0)
{
	unsigned int channel_num;
	unsigned int hw_irq_num;
	unsigned int irq_num;
	unsigned int prc_num;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC e200 processors
	main_core_0 = new Main_Core_0("Main_Core_0", this);
	main_core_1 = new Main_Core_1("Main_Core_1", this);
	peripheral_core_2 = new Peripheral_Core_2("Peripheral_Core_2", this);

	//  - System SRAM
	system_sram = new SYSTEM_SRAM("SYSTEM-SRAM", this);
	
	//  - FLASH
	flash = new FLASH("FLASH", this);
	
	//  - Crossbars
	xbar_0 = new XBAR_0("XBAR_0", this);
	xbar_1 = new XBAR_1("XBAR_1", this);
	
	// Peripheral Bridges
	pbridge_a = new PBRIDGE_A("PBRIDGE_A", this); 
	pbridge_b = new PBRIDGE_B("PBRIDGE_B", this); 
	
	//  - Interrupt Controller
	intc_0 = new INTC_0("INTC_0", this);
	
	//  - System Timer Module
	stm_0 = new STM_0("STM_0", this);
	stm_1 = new STM_1("STM_1", this);
	stm_2 = new STM_2("STM_2", this);

	//  - Software Watchdog Timers
	swt_0 = new SWT_0("SWT_0", this);
	swt_1 = new SWT_1("SWT_1", this);
	swt_2 = new SWT_2("SWT_2", this);
	swt_3 = new SWT_3("SWT_3", this);
	
	//  - Periodic Interrupt Timers
	pit_0 = new PIT_0("PIT_0", this);
	pit_1 = new PIT_1("PIT_1", this);
	
	//  - LINFlexD
	linflexd_0 = new LINFLEXD_0("LINFlexD_0", this);
	linflexd_1 = new LINFLEXD_1("LINFlexD_1", this);
	linflexd_2 = new LINFLEXD_2("LINFlexD_2", this);
	linflexd_14 = new LINFLEXD_14("LINFlexD_14", this);
	linflexd_15 = new LINFLEXD_15("LINFlexD_15", this);
	linflexd_16 = new LINFLEXD_16("LINFlexD_16", this);
	
	ebi_stub = new EBI_STUB("EBI", this);
	flash_port1_stub = new FLASH_PORT1_STUB("FLASH_PORT1", this);
	xbar_0_s6_stub = new XBAR_0_S6_STUB("XBAR_0_S6", this);
	xbar_1_m1_stub = new XBAR_1_M1_STUB("XBAR_1_M1", this);
	xbar_1_m2_stub = new XBAR_1_M2_STUB("XBAR_1_M2", this);

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = new LOADER("loader");
	//  - debugger
	debugger = (enable_inline_debugger || enable_gdb_server || enable_profiler) ? new DEBUGGER("debugger") : 0;
	//  - GDB server
	gdb_server[0] = enable_gdb_server ? new GDB_SERVER("gdb-server0") : 0;
	gdb_server[1] = enable_gdb_server ? new GDB_SERVER("gdb-server1") : 0;
	gdb_server[2] = enable_gdb_server ? new GDB_SERVER("gdb-server2") : 0;
	//  - Inline debugger
	inline_debugger[0] = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger0") : 0;
	inline_debugger[1] = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger1") : 0;
	inline_debugger[2] = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger2") : 0;
	//  - profiler
	profiler[0] = enable_profiler ? new PROFILER("profiler0") : 0;
	profiler[1] = enable_profiler ? new PROFILER("profiler1") : 0;
	profiler[2] = enable_profiler ? new PROFILER("profiler2") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	
	//=========================================================================
	//===                          Port registration                        ===
	//=========================================================================
	RegisterPort(main_core_0->m_clk);
	RegisterPort(main_core_0->m_por);
	RegisterPort(main_core_0->p_reset_b);
	RegisterPort(main_core_0->p_nmi_b);
	RegisterPort(main_core_0->p_mcp_b);
	RegisterPort(main_core_0->p_rstbase);
	RegisterPort(main_core_0->p_cpuid);
	RegisterPort(main_core_0->p_extint_b);
	RegisterPort(main_core_0->p_crint_b);
	RegisterPort(main_core_0->p_avec_b);
	RegisterPort(main_core_0->p_voffset);
	RegisterPort(main_core_0->p_iack);

	RegisterPort(main_core_1->m_clk);
	RegisterPort(main_core_1->m_por);
	RegisterPort(main_core_1->p_reset_b);
	RegisterPort(main_core_1->p_nmi_b);
	RegisterPort(main_core_1->p_mcp_b);
	RegisterPort(main_core_1->p_rstbase);
	RegisterPort(main_core_1->p_cpuid);
	RegisterPort(main_core_1->p_extint_b);
	RegisterPort(main_core_1->p_crint_b);
	RegisterPort(main_core_1->p_avec_b);
	RegisterPort(main_core_1->p_voffset);
	RegisterPort(main_core_1->p_iack);

	RegisterPort(peripheral_core_2->m_clk);
	RegisterPort(peripheral_core_2->m_por);
	RegisterPort(peripheral_core_2->p_reset_b);
	RegisterPort(peripheral_core_2->p_nmi_b);
	RegisterPort(peripheral_core_2->p_mcp_b);
	RegisterPort(peripheral_core_2->p_rstbase);
	RegisterPort(peripheral_core_2->p_cpuid);
	RegisterPort(peripheral_core_2->p_extint_b);
	RegisterPort(peripheral_core_2->p_crint_b);
	RegisterPort(peripheral_core_2->p_avec_b);
	RegisterPort(peripheral_core_2->p_voffset);
	RegisterPort(peripheral_core_2->p_iack);
	
	RegisterPort(intc_0->m_clk);
	RegisterPort(intc_0->reset_b);
	
	for(hw_irq_num = 0; hw_irq_num < INTC_0_CONFIG::NUM_HW_IRQS; hw_irq_num++)
	{
		RegisterPort(*intc_0->hw_irq[hw_irq_num]);
	}
	for(prc_num = 0; prc_num < INTC_0_CONFIG::NUM_PROCESSORS; prc_num++)
	{
		RegisterPort(*intc_0->p_iack[prc_num]);
		RegisterPort(*intc_0->p_irq_b[prc_num]);
		RegisterPort(*intc_0->p_avec_b[prc_num]);
		RegisterPort(*intc_0->p_voffset[prc_num]);
	}
	
	RegisterPort(stm_0->m_clk);
	RegisterPort(stm_0->reset_b);
	RegisterPort(stm_0->debug);
	for(channel_num = 0; channel_num < STM_0_CONFIG::NUM_CHANNELS; channel_num++)
	{
		RegisterPort(*stm_0->irq[channel_num]);
	}

	RegisterPort(stm_1->m_clk);
	RegisterPort(stm_1->reset_b);
	RegisterPort(stm_1->debug);
	for(channel_num = 0; channel_num < STM_1_CONFIG::NUM_CHANNELS; channel_num++)
	{
		RegisterPort(*stm_1->irq[channel_num]);
	}

	RegisterPort(stm_2->m_clk);
	RegisterPort(stm_2->reset_b);
	RegisterPort(stm_2->debug);
	for(channel_num = 0; channel_num < STM_2_CONFIG::NUM_CHANNELS; channel_num++)
	{
		RegisterPort(*stm_2->irq[channel_num]);
	}

	RegisterPort(swt_0->m_clk);
	RegisterPort(swt_0->swt_reset_b);
	RegisterPort(swt_0->stop);
	RegisterPort(swt_0->debug);
	RegisterPort(swt_0->irq);
	RegisterPort(swt_0->reset_b);

	RegisterPort(swt_1->m_clk);
	RegisterPort(swt_1->swt_reset_b);
	RegisterPort(swt_1->stop);
	RegisterPort(swt_1->debug);
	RegisterPort(swt_1->irq);
	RegisterPort(swt_1->reset_b);

	RegisterPort(swt_2->m_clk);
	RegisterPort(swt_2->swt_reset_b);
	RegisterPort(swt_2->stop);
	RegisterPort(swt_2->debug);
	RegisterPort(swt_2->irq);
	RegisterPort(swt_2->reset_b);
	
	RegisterPort(swt_3->m_clk);
	RegisterPort(swt_3->swt_reset_b);
	RegisterPort(swt_3->stop);
	RegisterPort(swt_3->debug);
	RegisterPort(swt_3->irq);
	RegisterPort(swt_3->reset_b);

	RegisterPort(pit_0->m_clk);
	RegisterPort(pit_0->per_clk);
	RegisterPort(pit_0->rti_clk);
	RegisterPort(pit_0->reset_b);
	RegisterPort(pit_0->debug);
	for(channel_num = 0; channel_num < PIT_0_CONFIG::MAX_CHANNELS; channel_num++)
	{
		RegisterPort(*pit_0->irq[channel_num]);
		RegisterPort(*pit_0->dma_trigger[channel_num]);
	}
	RegisterPort(pit_0->rtirq);
	
	RegisterPort(pit_1->m_clk);
	RegisterPort(pit_1->per_clk);
	RegisterPort(pit_1->rti_clk);
	RegisterPort(pit_1->reset_b);
	RegisterPort(pit_1->debug);
	for(channel_num = 0; channel_num < PIT_0_CONFIG::MAX_CHANNELS; channel_num++)
	{
		RegisterPort(*pit_1->irq[channel_num]);
		RegisterPort(*pit_1->dma_trigger[channel_num]);
	}
	RegisterPort(pit_1->rtirq);
	
	RegisterPort(linflexd_0->m_clk);
	RegisterPort(linflexd_0->lin_clk);
	RegisterPort(linflexd_0->reset_b);
	for(irq_num = 0; irq_num < LINFLEXD_0::NUM_IRQS; irq_num++)
	{
		RegisterPort(*linflexd_0->irq[irq_num]);
	}

	RegisterPort(linflexd_1->m_clk);
	RegisterPort(linflexd_1->lin_clk);
	RegisterPort(linflexd_1->reset_b);
	for(irq_num = 0; irq_num < LINFLEXD_1::NUM_IRQS; irq_num++)
	{
		RegisterPort(*linflexd_1->irq[irq_num]);
	}

	RegisterPort(linflexd_2->m_clk);
	RegisterPort(linflexd_2->lin_clk);
	RegisterPort(linflexd_2->reset_b);
	for(irq_num = 0; irq_num < LINFLEXD_2::NUM_IRQS; irq_num++)
	{
		RegisterPort(*linflexd_2->irq[irq_num]);
	}

	RegisterPort(linflexd_14->m_clk);
	RegisterPort(linflexd_14->lin_clk);
	RegisterPort(linflexd_14->reset_b);
	for(irq_num = 0; irq_num < LINFLEXD_14::NUM_IRQS; irq_num++)
	{
		RegisterPort(*linflexd_14->irq[irq_num]);
	}

	RegisterPort(linflexd_15->m_clk);
	RegisterPort(linflexd_15->lin_clk);
	RegisterPort(linflexd_15->reset_b);
	for(irq_num = 0; irq_num < LINFLEXD_15::NUM_IRQS; irq_num++)
	{
		RegisterPort(*linflexd_15->irq[irq_num]);
	}

	RegisterPort(linflexd_16->m_clk);
	RegisterPort(linflexd_16->lin_clk);
	RegisterPort(linflexd_16->reset_b);
	for(irq_num = 0; irq_num < LINFLEXD_16::NUM_IRQS; irq_num++)
	{
		RegisterPort(*linflexd_16->irq[irq_num]);
	}

	//=========================================================================
	//===                           Signal creation                         ===
	//=========================================================================
	
	CreateClock("IOP_CLK");
	CreateClock("COMP_CLK");
	CreateClock("FXBAR_CLK");
	CreateClock("SXBAR_CLK");
	CreateClock("PBRIDGEA_CLK");
	CreateClock("PBRIDGEB_CLK");
	CreateClock("PER_CLK");
	CreateClock("RTI_CLK");
	CreateClock("LIN_CLK");
	
	CreateSignal("m_por", false);
	CreateSignal("stop", false);
	CreateSignal("debug", false);
	CreateSignal("reset_b", false);
	CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_0", true);
	CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_1", true);
	CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_2", true);
	CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_3", true);
	CreateSignal("p_nmi_b", true);
	CreateSignal("p_mcp_b", true);
	CreateSignal("p_rstbase", sc_dt::sc_uint<30>(0));
	CreateSignal("p_cpuid", sc_dt::sc_uint<8>(0));
	CreateSignal("p_extint_b", true);
	CreateSignal("p_crint_b", true);
	CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_irq_b", false);
	CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_avec_b", true);
	CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_voffset", sc_dt::sc_uint<INTC_0_CONFIG::VOFFSET_WIDTH>(0));
	CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_iack", false);
	
	CreateSignalArray(INTC_0::NUM_IRQS, "irq", false);
	CreateSignalArray(STM_2_CONFIG::NUM_CHANNELS, "stm_2_cir", false);

	CreateSignalArray(64, "dma_trigger", false);
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	main_core_0->i_ahb_if(*xbar_0->targ_socket[0]);                  //       Main_Core_0>I_AHB_IF <-> XBAR_0 M0
	main_core_0->d_ahb_if(*xbar_0->targ_socket[1]);                  //       Main_Core_0>D_AHB_IF <-> XBAR_0 M1
	main_core_1->i_ahb_if(*xbar_0->targ_socket[2]);                  //       Main_Core_1>I_AHB_IF <-> XBAR_0 M2
	main_core_1->d_ahb_if(*xbar_0->targ_socket[3]);                  //       Main_Core_1>D_AHB_IF <-> XBAR_0 M3
	peripheral_core_2->i_ahb_if(*xbar_0->targ_socket[5]);            // Peripheral_Core_2>I_AHB_IF <-> XBAR_0 M5
	
	peripheral_core_2->d_ahb_if(*xbar_1->targ_socket[0]);            // Peripheral_Core_2>D_AHB_IF <-> XBAR_1 M0
	xbar_1_m1_stub->master_sock(*xbar_1->targ_socket[1]);            // TODO: XBAR_1 M1 <-> DMA and HSM
	xbar_1_m2_stub->master_sock(*xbar_1->targ_socket[2]);            // TODO: XBAR_1 M2 <-> FEC, LFAST and FlexRay
	
	(*xbar_0->init_socket[0])(flash->slave_sock);                    //       XBAR_0 S0 <-> FLASH Port 0
	(*xbar_0->init_socket[1])(flash_port1_stub->slave_sock);         // TODO: XBAR_0 S1 <-> FLASH Port 1
	(*xbar_0->init_socket[2])(main_core_0->s_ahb_if);                //       XBAR_0 S2 <-> S_AHB_IF<Main_Core_0
	(*xbar_0->init_socket[3])(main_core_1->s_ahb_if);                //       XBAR_0 S3 <-> S_AHB_IF<Main_Core_1
	(*xbar_0->init_socket[4])(system_sram->slave_sock);              //       XBAR_0 S4 <-> System SRAM
	(*xbar_0->init_socket[5])(ebi_stub->slave_sock);                 // TODO: XBAR_0 S5 <-> EBI
	(*xbar_0->init_socket[6])(xbar_0_s6_stub->slave_sock);           //       XBAR_0 S6 <-> unused
	(*xbar_0->init_socket[7])(*xbar_1->targ_socket[3]);              //       XBAR_0 S7 <-> XBAR_1 M3
                                                                     
	(*xbar_1->init_socket[0])(*xbar_0->targ_socket[4]);              //       XBAR_1 S0 <-> XBAR_0 M4
	(*xbar_1->init_socket[1])(peripheral_core_2->s_ahb_if);          //       XBAR_1 S1 <-> S_AHB_IF<Peripheral_Core_2
	(*xbar_1->init_socket[2])(*pbridge_b->targ_socket[0]);           //       XBAR_1 S2 <-> PBRIDGE_B
	(*xbar_1->init_socket[3])(*pbridge_a->targ_socket[0]);           //       XBAR_1 S3 <-> PBRIDGE_A
	
	(*pbridge_a->init_socket[0])(intc_0->peripheral_slave_if);       // PBRIDGE_A <-> INTC_0
	(*pbridge_a->init_socket[1])(stm_0->peripheral_slave_if);        // PBRIDGE_A <-> STM_0
	(*pbridge_a->init_socket[2])(stm_1->peripheral_slave_if);        // PBRIDGE_A <-> STM_1
	(*pbridge_a->init_socket[3])(stm_2->peripheral_slave_if);        // PBRIDGE_A <-> STM_2
	(*pbridge_a->init_socket[4])(swt_0->peripheral_slave_if);        // PBRIDGE_A <-> SWT_0
	(*pbridge_a->init_socket[5])(swt_1->peripheral_slave_if);        // PBRIDGE_A <-> SWT_1
	(*pbridge_a->init_socket[6])(swt_2->peripheral_slave_if);        // PBRIDGE_A <-> SWT_2
	(*pbridge_a->init_socket[7])(swt_3->peripheral_slave_if);        // PBRIDGE_A <-> SWT_3
	(*pbridge_a->init_socket[8])(pit_0->peripheral_slave_if);        // PBRIDGE_A <-> PIT_0
	(*pbridge_a->init_socket[9])(pit_1->peripheral_slave_if);        // PBRIDGE_A <-> PIT_1
	(*pbridge_a->init_socket[10])(linflexd_0->peripheral_slave_if);  // PBRIDGE_A <-> LINFlexD_0
	(*pbridge_a->init_socket[11])(linflexd_1->peripheral_slave_if);  // PBRIDGE_A <-> LINFlexD_1
	(*pbridge_a->init_socket[12])(linflexd_14->peripheral_slave_if); // PBRIDGE_A <-> LINFlexD_14
	(*pbridge_a->init_socket[13])(linflexd_16->peripheral_slave_if); // PBRIDGE_A <-> LINFlexD_16
	
	(*pbridge_b->init_socket[0])(linflexd_2->peripheral_slave_if);   // PBRIDGE_B <-> LINFlexD_2
	(*pbridge_b->init_socket[1])(linflexd_15->peripheral_slave_if);  // PBRIDGE_B <-> LINFlexD_15
	
	Bind("HARDWARE.Main_Core_0.m_clk"           , "HARDWARE.COMP_CLK");
	Bind("HARDWARE.Main_Core_0.m_por"           , "HARDWARE.m_por");
	Bind("HARDWARE.Main_Core_0.p_reset_b"       , "HARDWARE.p_reset_b_0");
	Bind("HARDWARE.Main_Core_0.p_nmi_b"         , "HARDWARE.p_nmi_b");
	Bind("HARDWARE.Main_Core_0.p_mcp_b"         , "HARDWARE.p_mcp_b");
	Bind("HARDWARE.Main_Core_0.p_rstbase"       , "HARDWARE.p_rstbase");
	Bind("HARDWARE.Main_Core_0.p_cpuid"         , "HARDWARE.p_cpuid");
	Bind("HARDWARE.Main_Core_0.p_extint_b"      , "HARDWARE.p_irq_b_0");
	Bind("HARDWARE.Main_Core_0.p_crint_b"       , "HARDWARE.p_crint_b");
	Bind("HARDWARE.Main_Core_0.p_avec_b"        , "HARDWARE.p_avec_b_0");
	Bind("HARDWARE.Main_Core_0.p_voffset"       , "HARDWARE.p_voffset_0");
	Bind("HARDWARE.Main_Core_0.p_iack"          , "HARDWARE.p_iack_0");

	Bind("HARDWARE.Main_Core_1.m_clk"           , "HARDWARE.COMP_CLK");
	Bind("HARDWARE.Main_Core_1.m_por"           , "HARDWARE.m_por");
	Bind("HARDWARE.Main_Core_1.p_reset_b"       , "HARDWARE.p_reset_b_0");
	Bind("HARDWARE.Main_Core_1.p_nmi_b"         , "HARDWARE.p_nmi_b");
	Bind("HARDWARE.Main_Core_1.p_mcp_b"         , "HARDWARE.p_mcp_b");
	Bind("HARDWARE.Main_Core_1.p_rstbase"       , "HARDWARE.p_rstbase");
	Bind("HARDWARE.Main_Core_1.p_cpuid"         , "HARDWARE.p_cpuid");
	Bind("HARDWARE.Main_Core_1.p_extint_b"      , "HARDWARE.p_irq_b_1");
	Bind("HARDWARE.Main_Core_1.p_crint_b"       , "HARDWARE.p_crint_b");
	Bind("HARDWARE.Main_Core_1.p_avec_b"        , "HARDWARE.p_avec_b_1");
	Bind("HARDWARE.Main_Core_1.p_voffset"       , "HARDWARE.p_voffset_1");
	Bind("HARDWARE.Main_Core_1.p_iack"          , "HARDWARE.p_iack_1");

	Bind("HARDWARE.Peripheral_Core_2.m_clk"           , "HARDWARE.IOP_CLK");
	Bind("HARDWARE.Peripheral_Core_2.m_por"           , "HARDWARE.m_por");
	Bind("HARDWARE.Peripheral_Core_2.p_reset_b"       , "HARDWARE.p_reset_b_2");
	Bind("HARDWARE.Peripheral_Core_2.p_nmi_b"         , "HARDWARE.p_nmi_b");
	Bind("HARDWARE.Peripheral_Core_2.p_mcp_b"         , "HARDWARE.p_mcp_b");
	Bind("HARDWARE.Peripheral_Core_2.p_rstbase"       , "HARDWARE.p_rstbase");
	Bind("HARDWARE.Peripheral_Core_2.p_cpuid"         , "HARDWARE.p_cpuid");
	Bind("HARDWARE.Peripheral_Core_2.p_extint_b"      , "HARDWARE.p_irq_b_2");
	Bind("HARDWARE.Peripheral_Core_2.p_crint_b"       , "HARDWARE.p_crint_b");
	Bind("HARDWARE.Peripheral_Core_2.p_avec_b"        , "HARDWARE.p_avec_b_2");
	Bind("HARDWARE.Peripheral_Core_2.p_voffset"       , "HARDWARE.p_voffset_2");
	Bind("HARDWARE.Peripheral_Core_2.p_iack"          , "HARDWARE.p_iack_2");
	
	Bind("HARDWARE.INTC_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.INTC_0.reset_b"         , "HARDWARE.reset_b");
	BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_irq_b"  , "HARDWARE.p_irq_b"  );
	BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_avec_b" , "HARDWARE.p_avec_b" );
	BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_voffset", "HARDWARE.p_voffset");
	BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_iack"   , "HARDWARE.p_iack"   );
	
	Bind("HARDWARE.STM_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.STM_0.reset_b"         , "HARDWARE.reset_b");
	Bind("HARDWARE.STM_0.debug"           , "HARDWARE.debug");

	Bind("HARDWARE.STM_1.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.STM_1.reset_b"         , "HARDWARE.reset_b");
	Bind("HARDWARE.STM_1.debug"           , "HARDWARE.debug");

	Bind("HARDWARE.STM_2.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.STM_2.reset_b"         , "HARDWARE.reset_b");
	Bind("HARDWARE.STM_2.debug"           , "HARDWARE.debug");
	
	Bind("HARDWARE.SWT_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.SWT_0.swt_reset_b"     , "HARDWARE.reset_b");
	Bind("HARDWARE.SWT_0.stop"            , "HARDWARE.stop");
	Bind("HARDWARE.SWT_0.debug"           , "HARDWARE.debug");
	Bind("HARDWARE.SWT_0.reset_b"         , "HARDWARE.p_reset_b_0");

	Bind("HARDWARE.SWT_1.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.SWT_1.swt_reset_b"     , "HARDWARE.reset_b");
	Bind("HARDWARE.SWT_1.stop"            , "HARDWARE.stop");
	Bind("HARDWARE.SWT_1.debug"           , "HARDWARE.debug");
	Bind("HARDWARE.SWT_1.reset_b"         , "HARDWARE.p_reset_b_1");

	Bind("HARDWARE.SWT_2.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.SWT_2.swt_reset_b"     , "HARDWARE.reset_b");
	Bind("HARDWARE.SWT_2.stop"            , "HARDWARE.stop");
	Bind("HARDWARE.SWT_2.debug"           , "HARDWARE.debug");
	Bind("HARDWARE.SWT_2.reset_b"         , "HARDWARE.p_reset_b_2");
	
	Bind("HARDWARE.SWT_3.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.SWT_3.swt_reset_b"     , "HARDWARE.reset_b");
	Bind("HARDWARE.SWT_3.stop"            , "HARDWARE.stop");
	Bind("HARDWARE.SWT_3.debug"           , "HARDWARE.debug");
	Bind("HARDWARE.SWT_3.reset_b"         , "HARDWARE.p_reset_b_3");
	
	Bind("HARDWARE.PIT_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.PIT_0.per_clk"         , "HARDWARE.PER_CLK");
	Bind("HARDWARE.PIT_0.rti_clk"         , "HARDWARE.RTI_CLK");
	Bind("HARDWARE.PIT_0.reset_b"         , "HARDWARE.reset_b");
	Bind("HARDWARE.PIT_0.debug"           , "HARDWARE.debug");
	
	Bind("HARDWARE.PIT_0.dma_trigger_0", "HARDWARE.dma_trigger_8");
	Bind("HARDWARE.PIT_0.dma_trigger_1", "HARDWARE.dma_trigger_9");
	Bind("HARDWARE.PIT_0.dma_trigger_2", "HARDWARE.dma_trigger_10");
	Bind("HARDWARE.PIT_0.dma_trigger_3", "HARDWARE.dma_trigger_11");
	Bind("HARDWARE.PIT_0.dma_trigger_4", "HARDWARE.dma_trigger_12");
	Bind("HARDWARE.PIT_0.dma_trigger_5", "HARDWARE.dma_trigger_16");
	Bind("HARDWARE.PIT_0.dma_trigger_6", "HARDWARE.dma_trigger_32");
	Bind("HARDWARE.PIT_0.dma_trigger_7", "HARDWARE.dma_trigger_48");
	
	Bind("HARDWARE.PIT_1.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.PIT_1.per_clk"         , "HARDWARE.PER_CLK");
	Bind("HARDWARE.PIT_1.rti_clk"         , "HARDWARE.RTI_CLK");
	Bind("HARDWARE.PIT_1.reset_b"         , "HARDWARE.reset_b");
	Bind("HARDWARE.PIT_1.debug"           , "HARDWARE.debug");
	
	Bind("HARDWARE.PIT_1.irq_2"           , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.irq_3"           , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.irq_4"           , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.irq_5"           , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.irq_6"           , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.irq_7"           , "HARDWARE.pull_down");
	
	Bind("HARDWARE.PIT_1.rtirq"           , "HARDWARE.pull_down");

	Bind("HARDWARE.PIT_1.dma_trigger_0"   , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.dma_trigger_1"   , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.dma_trigger_2"   , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.dma_trigger_3"   , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.dma_trigger_4"   , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.dma_trigger_5"   , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.dma_trigger_6"   , "HARDWARE.pull_down");
	Bind("HARDWARE.PIT_1.dma_trigger_7"   , "HARDWARE.pull_down");
	
	Bind("HARDWARE.LINFlexD_0.m_clk"      , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.LINFlexD_0.lin_clk"    , "HARDWARE.LIN_CLK");
	Bind("HARDWARE.LINFlexD_0.reset_b"    , "HARDWARE.reset_b");

	Bind("HARDWARE.LINFlexD_1.m_clk"      , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.LINFlexD_1.lin_clk"    , "HARDWARE.LIN_CLK");
	Bind("HARDWARE.LINFlexD_1.reset_b"    , "HARDWARE.reset_b");

	Bind("HARDWARE.LINFlexD_14.m_clk"      , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.LINFlexD_14.lin_clk"    , "HARDWARE.LIN_CLK");
	Bind("HARDWARE.LINFlexD_14.reset_b"    , "HARDWARE.reset_b");

	Bind("HARDWARE.LINFlexD_16.m_clk"      , "HARDWARE.PBRIDGEA_CLK");
	Bind("HARDWARE.LINFlexD_16.lin_clk"    , "HARDWARE.LIN_CLK");
	Bind("HARDWARE.LINFlexD_16.reset_b"    , "HARDWARE.reset_b");

	Bind("HARDWARE.LINFlexD_2.m_clk"      , "HARDWARE.PBRIDGEB_CLK");
	Bind("HARDWARE.LINFlexD_2.lin_clk"    , "HARDWARE.LIN_CLK");
	Bind("HARDWARE.LINFlexD_2.reset_b"    , "HARDWARE.reset_b");

	Bind("HARDWARE.LINFlexD_15.m_clk"      , "HARDWARE.PBRIDGEB_CLK");
	Bind("HARDWARE.LINFlexD_15.lin_clk"    , "HARDWARE.LIN_CLK");
	Bind("HARDWARE.LINFlexD_15.reset_b"    , "HARDWARE.reset_b");
	
	// Interrupt sources
	
	// IRQ # ---- Source name ------------ Description ------------------------- Note --------------
	//   0     INTC SSCIR0[CLR]     software settable flag 0            internally routed in INTC
	//  ..           ..                       ..                                   ..
	//  31    INTC SSCIR31[CLR]    software settable flag 31            internally routed in INTC
	
	//  32      SWT_0 IR[TIF]
	//  33      SWT_1 IR[TIF]
	//  34      SWT_2 IR[TIF]
	//  35      SWT_3 IR[TIF]
	
	InterruptSource(32, "HARDWARE.SWT_0.irq");
	InterruptSource(33, "HARDWARE.SWT_1.irq");
	InterruptSource(34, "HARDWARE.SWT_2.irq");
	InterruptSource(35, "HARDWARE.SWT_3.irq");

	//  36      STM_0 CIR0[CIF]     platform period timer 0_0
	//  37      STM_0 CIR1[CIF]     platform period timer 0_1
	//  38      STM_0 CIR2[CIF]     platform period timer 0_2
	//  39      STM_0 CIR3[CIF]     platform period timer 0_3
	//  40      STM_1 CIR0[CIF]     platform period timer 1_0
	//  41      STM_1 CIR1[CIF]     platform period timer 1_1
	//  42      STM_1 CIR2[CIF]     platform period timer 1_2
	//  43      STM_1 CIR3[CIF]     platform period timer 1_3
	//  44      STM_2 CIR0[CIF]     platform period timer 2_0
	//  45      STM_2 CIR1[CIF]     platform period timer 2_1
	//  46      STM_2 CIR2[CIF]     platform period timer 2_2
	//  47      STM_2 CIR3[CIF]     platform period timer 2_3
	
	InterruptSource(36, "HARDWARE.STM_0.irq_0");
	InterruptSource(37, "HARDWARE.STM_0.irq_1");
	InterruptSource(38, "HARDWARE.STM_0.irq_2");
	InterruptSource(39, "HARDWARE.STM_0.irq_3");
	InterruptSource(40, "HARDWARE.STM_1.irq_0");
	InterruptSource(41, "HARDWARE.STM_1.irq_1");
	InterruptSource(42, "HARDWARE.STM_1.irq_2");
	InterruptSource(43, "HARDWARE.STM_1.irq_3");
	InterruptSource(44, "HARDWARE.STM_2.irq_0");
	InterruptSource(45, "HARDWARE.STM_2.irq_1");
	InterruptSource(46, "HARDWARE.STM_2.irq_2");
	InterruptSource(47, "HARDWARE.STM_2.irq_3");

	// 48           TODO
	// ..           TODO
	// 964          TODO

	InterruptSource(48);
	InterruptSource(49);
	InterruptSource(50);
	InterruptSource(51);
	InterruptSource(52);
	InterruptSource(53);
	InterruptSource(54);
	InterruptSource(55);
	InterruptSource(56);
	InterruptSource(57);
	InterruptSource(58);
	InterruptSource(59);
	InterruptSource(60);
	InterruptSource(61);
	InterruptSource(62);
	InterruptSource(63);
	InterruptSource(64);
	InterruptSource(65);
	InterruptSource(66);
	InterruptSource(67);
	InterruptSource(68);
	InterruptSource(69);
	InterruptSource(70);
	InterruptSource(71);
	InterruptSource(72);
	InterruptSource(73);
	InterruptSource(74);
	InterruptSource(75);
	InterruptSource(76);
	InterruptSource(77);
	InterruptSource(78);
	InterruptSource(79);
	InterruptSource(80);
	InterruptSource(81);
	InterruptSource(82);
	InterruptSource(83);
	InterruptSource(84);
	InterruptSource(85);
	InterruptSource(86);
	InterruptSource(87);
	InterruptSource(88);
	InterruptSource(89);
	InterruptSource(90);
	InterruptSource(91);
	InterruptSource(92);
	InterruptSource(93);
	InterruptSource(94);
	InterruptSource(95);
	InterruptSource(96);
	InterruptSource(97);
	InterruptSource(98);
	InterruptSource(99);
	InterruptSource(100);
	InterruptSource(101);
	InterruptSource(102);
	InterruptSource(103);
	InterruptSource(104);
	InterruptSource(105);
	InterruptSource(106);
	InterruptSource(107);
	InterruptSource(108);
	InterruptSource(109);
	InterruptSource(110);
	InterruptSource(111);
	InterruptSource(112);
	InterruptSource(113);
	InterruptSource(114);
	InterruptSource(115);
	InterruptSource(116);
	InterruptSource(117);
	InterruptSource(118);
	InterruptSource(119);
	InterruptSource(120);
	InterruptSource(121);
	InterruptSource(122);
	InterruptSource(123);
	InterruptSource(124);
	InterruptSource(125);
	InterruptSource(126);
	InterruptSource(127);
	InterruptSource(128);
	InterruptSource(129);
	InterruptSource(130);
	InterruptSource(131);
	InterruptSource(132);
	InterruptSource(133);
	InterruptSource(134);
	InterruptSource(135);
	InterruptSource(136);
	InterruptSource(137);
	InterruptSource(138);
	InterruptSource(139);
	InterruptSource(140);
	InterruptSource(141);
	InterruptSource(142);
	InterruptSource(143);
	InterruptSource(144);
	InterruptSource(145);
	InterruptSource(146);
	InterruptSource(147);
	InterruptSource(148);
	InterruptSource(149);
	InterruptSource(150);
	InterruptSource(151);
	InterruptSource(152);
	InterruptSource(153);
	InterruptSource(154);
	InterruptSource(155);
	InterruptSource(156);
	InterruptSource(157);
	InterruptSource(158);
	InterruptSource(159);
	InterruptSource(160);
	InterruptSource(161);
	InterruptSource(162);
	InterruptSource(163);
	InterruptSource(164);
	InterruptSource(165);
	InterruptSource(166);
	InterruptSource(167);
	InterruptSource(168);
	InterruptSource(169);
	InterruptSource(170);
	InterruptSource(171);
	InterruptSource(172);
	InterruptSource(173);
	InterruptSource(174);
	InterruptSource(175);
	InterruptSource(176);
	InterruptSource(177);
	InterruptSource(178);
	InterruptSource(179);
	InterruptSource(180);
	InterruptSource(181);
	InterruptSource(182);
	InterruptSource(183);
	InterruptSource(184);
	InterruptSource(185);
	InterruptSource(186);
	InterruptSource(187);
	InterruptSource(188);
	InterruptSource(189);
	InterruptSource(190);
	InterruptSource(191);
	InterruptSource(192);
	InterruptSource(193);
	InterruptSource(194);
	InterruptSource(195);
	InterruptSource(196);
	InterruptSource(197);
	InterruptSource(198);
	InterruptSource(199);
	InterruptSource(200);
	InterruptSource(201);
	InterruptSource(202);
	InterruptSource(203);
	InterruptSource(204);
	InterruptSource(205);
	InterruptSource(206);
	InterruptSource(207);
	InterruptSource(208);
	InterruptSource(209);
	InterruptSource(210);
	InterruptSource(211);
	InterruptSource(212);
	InterruptSource(213);
	InterruptSource(214);
	InterruptSource(215);
	InterruptSource(216);
	InterruptSource(217);
	InterruptSource(218);
	InterruptSource(219);
	InterruptSource(220);
	InterruptSource(221);
	InterruptSource(222);
	InterruptSource(223);
	InterruptSource(224);
	InterruptSource(225);
	
	// 226      PIT_0_TFLG0[TIF]    periodic interrupt timer 0_0
	// 227      PIT_0_TFLG1[TIF]    periodic interrupt timer 0_1
	// 228      PIT_0_TFLG2[TIF]    periodic interrupt timer 0_2
	// 229      PIT_0_TFLG3[TIF]    periodic interrupt timer 0_3
	// 230      PIT_0_TFLG4[TIF]    periodic interrupt timer 0_4
	// 231      PIT_0_TFLG5[TIF]    periodic interrupt timer 0_5
	// 232      PIT_0_TFLG6[TIF]    periodic interrupt timer 0_6
	// 233      PIT_0_TFLG7[TIF]    periodic interrupt timer 0_7
	
	InterruptSource(226, "HARDWARE.PIT_0.irq_0");
	InterruptSource(227, "HARDWARE.PIT_0.irq_1");
	InterruptSource(228, "HARDWARE.PIT_0.irq_2");
	InterruptSource(229, "HARDWARE.PIT_0.irq_3");
	InterruptSource(230, "HARDWARE.PIT_0.irq_4");
	InterruptSource(231, "HARDWARE.PIT_0.irq_5");
	InterruptSource(232, "HARDWARE.PIT_0.irq_6");
	InterruptSource(233, "HARDWARE.PIT_0.irq_7");

	// 234          UNUSED
	//  ..          UNUSED
	// 238          UNUSED
	
	InterruptSource(234);
	InterruptSource(235);
	InterruptSource(236);
	InterruptSource(237);
	InterruptSource(238);
	
	// 239   PIT_0_RTI_TFLG[TIF]    periodic interrupt timer RTI

	InterruptSource(239, "HARDWARE.PIT_0.rtirq");
	
	// 240      PIT_1_TFLG0[TIF]    periodic interrupt timer 1_0
	// 241      PIT_1_TFLG1[TIF]    periodic interrupt timer 1_1
	
	InterruptSource(240, "HARDWARE.PIT_1.irq_0");
	InterruptSource(241, "HARDWARE.PIT_1.irq_1");

	// 242          TODO
	//  ..          TODO	
	// 964          TODO

	InterruptSource(242);
	InterruptSource(243);
	InterruptSource(244);
	InterruptSource(245);
	InterruptSource(246);
	InterruptSource(247);
	InterruptSource(248);
	InterruptSource(249);
	InterruptSource(250);
	InterruptSource(251);
	InterruptSource(252);
	InterruptSource(253);
	InterruptSource(254);
	InterruptSource(255);
	InterruptSource(256);
	InterruptSource(257);
	InterruptSource(258);
	InterruptSource(259);
	InterruptSource(260);
	InterruptSource(261);
	InterruptSource(262);
	InterruptSource(263);
	InterruptSource(264);
	InterruptSource(265);
	InterruptSource(266);
	InterruptSource(267);
	InterruptSource(268);
	InterruptSource(269);
	InterruptSource(270);
	InterruptSource(271);
	InterruptSource(272);
	InterruptSource(273);
	InterruptSource(274);
	InterruptSource(275);
	InterruptSource(276);
	InterruptSource(277);
	InterruptSource(278);
	InterruptSource(279);
	InterruptSource(280);
	InterruptSource(281);
	InterruptSource(282);
	InterruptSource(283);
	InterruptSource(284);
	InterruptSource(285);
	InterruptSource(286);
	InterruptSource(287);
	InterruptSource(288);
	InterruptSource(289);
	InterruptSource(290);
	InterruptSource(291);
	InterruptSource(292);
	InterruptSource(293);
	InterruptSource(294);
	InterruptSource(295);
	InterruptSource(296);
	InterruptSource(297);
	InterruptSource(298);
	InterruptSource(299);
	InterruptSource(300);
	InterruptSource(301);
	InterruptSource(302);
	InterruptSource(303);
	InterruptSource(304);
	InterruptSource(305);
	InterruptSource(306);
	InterruptSource(307);
	InterruptSource(308);
	InterruptSource(309);
	InterruptSource(310);
	InterruptSource(311);
	InterruptSource(312);
	InterruptSource(313);
	InterruptSource(314);
	InterruptSource(315);
	InterruptSource(316);
	InterruptSource(317);
	InterruptSource(318);
	InterruptSource(319);
	InterruptSource(320);
	InterruptSource(321);
	InterruptSource(322);
	InterruptSource(323);
	InterruptSource(324);
	InterruptSource(325);
	InterruptSource(326);
	InterruptSource(327);
	InterruptSource(328);
	InterruptSource(329);
	InterruptSource(330);
	InterruptSource(331);
	InterruptSource(332);
	InterruptSource(333);
	InterruptSource(334);
	InterruptSource(335);
	InterruptSource(336);
	InterruptSource(337);
	InterruptSource(338);
	InterruptSource(339);
	InterruptSource(340);
	InterruptSource(341);
	InterruptSource(342);
	InterruptSource(343);
	InterruptSource(344);
	InterruptSource(345);
	InterruptSource(346);
	InterruptSource(347);
	InterruptSource(348);
	InterruptSource(349);
	InterruptSource(350);
	InterruptSource(351);
	InterruptSource(352);
	InterruptSource(353);
	InterruptSource(354);
	InterruptSource(355);
	InterruptSource(356);
	InterruptSource(357);
	InterruptSource(358);
	InterruptSource(359);
	InterruptSource(360);
	InterruptSource(361);
	InterruptSource(362);
	InterruptSource(363);
	InterruptSource(364);
	InterruptSource(365);
	InterruptSource(366);
	InterruptSource(367);
	InterruptSource(368);
	InterruptSource(369);
	InterruptSource(370);
	InterruptSource(371);
	InterruptSource(372);
	InterruptSource(373);
	InterruptSource(374);
	InterruptSource(375);
	
	// 376      LINFlexD_0 RXI
	// 377      LINFlexD_0 TXI
	// 378      LINFlexD_0 ERR

	InterruptSource(376, "HARDWARE.LINFlexD_0.irq_0");
	InterruptSource(377, "HARDWARE.LINFlexD_0.irq_1");
	InterruptSource(378, "HARDWARE.LINFlexD_0.irq_2");
	
	// 379      unused
	
	InterruptSource(379);
	
	// 380      LINFlexD_1 RXI
	// 381      LINFlexD_1 TXI
	// 382      LINFlexD_1 ERR

	InterruptSource(380, "HARDWARE.LINFlexD_1.irq_0");
	InterruptSource(381, "HARDWARE.LINFlexD_1.irq_1");
	InterruptSource(382, "HARDWARE.LINFlexD_1.irq_2");

	// 383      unused
	
	InterruptSource(383);

	// 384      LINFlexD_2 RXI
	// 385      LINFlexD_2 TXI
	// 386      LINFlexD_2 ERR
	
	InterruptSource(384, "HARDWARE.LINFlexD_2.irq_0");
	InterruptSource(385, "HARDWARE.LINFlexD_2.irq_1");
	InterruptSource(386, "HARDWARE.LINFlexD_2.irq_2");

	// 387      TODO
	// ..       TODO
	// 415      TODO

	InterruptSource(387);
	InterruptSource(388);
	InterruptSource(389);
	InterruptSource(390);
	InterruptSource(391);
	InterruptSource(392);
	InterruptSource(393);
	InterruptSource(394);
	InterruptSource(395);
	InterruptSource(396);
	InterruptSource(397);
	InterruptSource(398);
	InterruptSource(399);
	InterruptSource(400);
	InterruptSource(401);
	InterruptSource(402);
	InterruptSource(403);
	InterruptSource(404);
	InterruptSource(405);
	InterruptSource(406);
	InterruptSource(407);
	InterruptSource(408);
	InterruptSource(409);
	InterruptSource(410);
	InterruptSource(411);
	InterruptSource(412);
	InterruptSource(413);
	InterruptSource(414);
	InterruptSource(415);
	
	// 416      LINFlexD_16 RXI
	// 417      LINFlexD_16 TXI
	// 418      LINFlexD_16 ERR

	InterruptSource(416, "HARDWARE.LINFlexD_16.irq_0");
	InterruptSource(417, "HARDWARE.LINFlexD_16.irq_1");
	InterruptSource(418, "HARDWARE.LINFlexD_16.irq_2");

	// 419      unused
	// ..       unused
	// 431      unused
	
	InterruptSource(419);
	InterruptSource(420);
	InterruptSource(421);
	InterruptSource(422);
	InterruptSource(423);
	InterruptSource(424);
	InterruptSource(425);
	InterruptSource(426);
	InterruptSource(427);
	InterruptSource(428);
	InterruptSource(429);
	InterruptSource(430);
	InterruptSource(431);
	
	// 432      LINFlexD_14 RXI
	// 433      LINFlexD_14 TXI
	// 434      LINFlexD_14 ERR
	
	InterruptSource(432, "HARDWARE.LINFlexD_14.irq_0");
	InterruptSource(433, "HARDWARE.LINFlexD_14.irq_1");
	InterruptSource(434, "HARDWARE.LINFlexD_14.irq_2");

	// 435      unused
	
	InterruptSource(435);
	
	// 436      LINFlexD_14 RXI
	// 437      LINFlexD_14 TXI
	// 438      LINFlexD_14 ERR
	
	InterruptSource(436, "HARDWARE.LINFlexD_15.irq_0");
	InterruptSource(437, "HARDWARE.LINFlexD_15.irq_1");
	InterruptSource(438, "HARDWARE.LINFlexD_15.irq_2");

	// 439      TODO
	// ..       TODO
	// 964
	
	InterruptSource(439);
	InterruptSource(440);
	InterruptSource(441);
	InterruptSource(442);
	InterruptSource(443);
	InterruptSource(444);
	InterruptSource(445);
	InterruptSource(446);
	InterruptSource(447);
	InterruptSource(448);
	InterruptSource(449);
	InterruptSource(450);
	InterruptSource(451);
	InterruptSource(452);
	InterruptSource(453);
	InterruptSource(454);
	InterruptSource(455);
	InterruptSource(456);
	InterruptSource(457);
	InterruptSource(458);
	InterruptSource(459);
	InterruptSource(460);
	InterruptSource(461);
	InterruptSource(462);
	InterruptSource(463);
	InterruptSource(464);
	InterruptSource(465);
	InterruptSource(466);
	InterruptSource(467);
	InterruptSource(468);
	InterruptSource(469);
	InterruptSource(470);
	InterruptSource(471);
	InterruptSource(472);
	InterruptSource(473);
	InterruptSource(474);
	InterruptSource(475);
	InterruptSource(476);
	InterruptSource(477);
	InterruptSource(478);
	InterruptSource(479);
	InterruptSource(480);
	InterruptSource(481);
	InterruptSource(482);
	InterruptSource(483);
	InterruptSource(484);
	InterruptSource(485);
	InterruptSource(486);
	InterruptSource(487);
	InterruptSource(488);
	InterruptSource(489);
	InterruptSource(490);
	InterruptSource(491);
	InterruptSource(492);
	InterruptSource(493);
	InterruptSource(494);
	InterruptSource(495);
	InterruptSource(496);
	InterruptSource(497);
	InterruptSource(498);
	InterruptSource(499);
	InterruptSource(500);
	InterruptSource(501);
	InterruptSource(502);
	InterruptSource(503);
	InterruptSource(504);
	InterruptSource(505);
	InterruptSource(506);
	InterruptSource(507);
	InterruptSource(508);
	InterruptSource(509);
	InterruptSource(510);
	InterruptSource(511);
	InterruptSource(512);
	InterruptSource(513);
	InterruptSource(514);
	InterruptSource(515);
	InterruptSource(516);
	InterruptSource(517);
	InterruptSource(518);
	InterruptSource(519);
	InterruptSource(520);
	InterruptSource(521);
	InterruptSource(522);
	InterruptSource(523);
	InterruptSource(524);
	InterruptSource(525);
	InterruptSource(526);
	InterruptSource(527);
	InterruptSource(528);
	InterruptSource(529);
	InterruptSource(530);
	InterruptSource(531);
	InterruptSource(532);
	InterruptSource(533);
	InterruptSource(534);
	InterruptSource(535);
	InterruptSource(536);
	InterruptSource(537);
	InterruptSource(538);
	InterruptSource(539);
	InterruptSource(540);
	InterruptSource(541);
	InterruptSource(542);
	InterruptSource(543);
	InterruptSource(544);
	InterruptSource(545);
	InterruptSource(546);
	InterruptSource(547);
	InterruptSource(548);
	InterruptSource(549);
	InterruptSource(550);
	InterruptSource(551);
	InterruptSource(552);
	InterruptSource(553);
	InterruptSource(554);
	InterruptSource(555);
	InterruptSource(556);
	InterruptSource(557);
	InterruptSource(558);
	InterruptSource(559);
	InterruptSource(560);
	InterruptSource(561);
	InterruptSource(562);
	InterruptSource(563);
	InterruptSource(564);
	InterruptSource(565);
	InterruptSource(566);
	InterruptSource(567);
	InterruptSource(568);
	InterruptSource(569);
	InterruptSource(570);
	InterruptSource(571);
	InterruptSource(572);
	InterruptSource(573);
	InterruptSource(574);
	InterruptSource(575);
	InterruptSource(576);
	InterruptSource(577);
	InterruptSource(578);
	InterruptSource(579);
	InterruptSource(580);
	InterruptSource(581);
	InterruptSource(582);
	InterruptSource(583);
	InterruptSource(584);
	InterruptSource(585);
	InterruptSource(586);
	InterruptSource(587);
	InterruptSource(588);
	InterruptSource(589);
	InterruptSource(590);
	InterruptSource(591);
	InterruptSource(592);
	InterruptSource(593);
	InterruptSource(594);
	InterruptSource(595);
	InterruptSource(596);
	InterruptSource(597);
	InterruptSource(598);
	InterruptSource(599);
	InterruptSource(600);
	InterruptSource(601);
	InterruptSource(602);
	InterruptSource(603);
	InterruptSource(604);
	InterruptSource(605);
	InterruptSource(606);
	InterruptSource(607);
	InterruptSource(608);
	InterruptSource(609);
	InterruptSource(610);
	InterruptSource(611);
	InterruptSource(612);
	InterruptSource(613);
	InterruptSource(614);
	InterruptSource(615);
	InterruptSource(616);
	InterruptSource(617);
	InterruptSource(618);
	InterruptSource(619);
	InterruptSource(620);
	InterruptSource(621);
	InterruptSource(622);
	InterruptSource(623);
	InterruptSource(624);
	InterruptSource(625);
	InterruptSource(626);
	InterruptSource(627);
	InterruptSource(628);
	InterruptSource(629);
	InterruptSource(630);
	InterruptSource(631);
	InterruptSource(632);
	InterruptSource(633);
	InterruptSource(634);
	InterruptSource(635);
	InterruptSource(636);
	InterruptSource(637);
	InterruptSource(638);
	InterruptSource(639);
	InterruptSource(640);
	InterruptSource(641);
	InterruptSource(642);
	InterruptSource(643);
	InterruptSource(644);
	InterruptSource(645);
	InterruptSource(646);
	InterruptSource(647);
	InterruptSource(648);
	InterruptSource(649);
	InterruptSource(650);
	InterruptSource(651);
	InterruptSource(652);
	InterruptSource(653);
	InterruptSource(654);
	InterruptSource(655);
	InterruptSource(656);
	InterruptSource(657);
	InterruptSource(658);
	InterruptSource(659);
	InterruptSource(660);
	InterruptSource(661);
	InterruptSource(662);
	InterruptSource(663);
	InterruptSource(664);
	InterruptSource(665);
	InterruptSource(666);
	InterruptSource(667);
	InterruptSource(668);
	InterruptSource(669);
	InterruptSource(670);
	InterruptSource(671);
	InterruptSource(672);
	InterruptSource(673);
	InterruptSource(674);
	InterruptSource(675);
	InterruptSource(676);
	InterruptSource(677);
	InterruptSource(678);
	InterruptSource(679);
	InterruptSource(680);
	InterruptSource(681);
	InterruptSource(682);
	InterruptSource(683);
	InterruptSource(684);
	InterruptSource(685);
	InterruptSource(686);
	InterruptSource(687);
	InterruptSource(688);
	InterruptSource(689);
	InterruptSource(690);
	InterruptSource(691);
	InterruptSource(692);
	InterruptSource(693);
	InterruptSource(694);
	InterruptSource(695);
	InterruptSource(696);
	InterruptSource(697);
	InterruptSource(698);
	InterruptSource(699);
	InterruptSource(700);
	InterruptSource(701);
	InterruptSource(702);
	InterruptSource(703);
	InterruptSource(704);
	InterruptSource(705);
	InterruptSource(706);
	InterruptSource(707);
	InterruptSource(708);
	InterruptSource(709);
	InterruptSource(710);
	InterruptSource(711);
	InterruptSource(712);
	InterruptSource(713);
	InterruptSource(714);
	InterruptSource(715);
	InterruptSource(716);
	InterruptSource(717);
	InterruptSource(718);
	InterruptSource(719);
	InterruptSource(720);
	InterruptSource(721);
	InterruptSource(722);
	InterruptSource(723);
	InterruptSource(724);
	InterruptSource(725);
	InterruptSource(726);
	InterruptSource(727);
	InterruptSource(728);
	InterruptSource(729);
	InterruptSource(730);
	InterruptSource(731);
	InterruptSource(732);
	InterruptSource(733);
	InterruptSource(734);
	InterruptSource(735);
	InterruptSource(736);
	InterruptSource(737);
	InterruptSource(738);
	InterruptSource(739);
	InterruptSource(740);
	InterruptSource(741);
	InterruptSource(742);
	InterruptSource(743);
	InterruptSource(744);
	InterruptSource(745);
	InterruptSource(746);
	InterruptSource(747);
	InterruptSource(748);
	InterruptSource(749);
	InterruptSource(750);
	InterruptSource(751);
	InterruptSource(752);
	InterruptSource(753);
	InterruptSource(754);
	InterruptSource(755);
	InterruptSource(756);
	InterruptSource(757);
	InterruptSource(758);
	InterruptSource(759);
	InterruptSource(760);
	InterruptSource(761);
	InterruptSource(762);
	InterruptSource(763);
	InterruptSource(764);
	InterruptSource(765);
	InterruptSource(766);
	InterruptSource(767);
	InterruptSource(768);
	InterruptSource(769);
	InterruptSource(770);
	InterruptSource(771);
	InterruptSource(772);
	InterruptSource(773);
	InterruptSource(774);
	InterruptSource(775);
	InterruptSource(776);
	InterruptSource(777);
	InterruptSource(778);
	InterruptSource(779);
	InterruptSource(780);
	InterruptSource(781);
	InterruptSource(782);
	InterruptSource(783);
	InterruptSource(784);
	InterruptSource(785);
	InterruptSource(786);
	InterruptSource(787);
	InterruptSource(788);
	InterruptSource(789);
	InterruptSource(790);
	InterruptSource(791);
	InterruptSource(792);
	InterruptSource(793);
	InterruptSource(794);
	InterruptSource(795);
	InterruptSource(796);
	InterruptSource(797);
	InterruptSource(798);
	InterruptSource(799);
	InterruptSource(800);
	InterruptSource(801);
	InterruptSource(802);
	InterruptSource(803);
	InterruptSource(804);
	InterruptSource(805);
	InterruptSource(806);
	InterruptSource(807);
	InterruptSource(808);
	InterruptSource(809);
	InterruptSource(810);
	InterruptSource(811);
	InterruptSource(812);
	InterruptSource(813);
	InterruptSource(814);
	InterruptSource(815);
	InterruptSource(816);
	InterruptSource(817);
	InterruptSource(818);
	InterruptSource(819);
	InterruptSource(820);
	InterruptSource(821);
	InterruptSource(822);
	InterruptSource(823);
	InterruptSource(824);
	InterruptSource(825);
	InterruptSource(826);
	InterruptSource(827);
	InterruptSource(828);
	InterruptSource(829);
	InterruptSource(830);
	InterruptSource(831);
	InterruptSource(832);
	InterruptSource(833);
	InterruptSource(834);
	InterruptSource(835);
	InterruptSource(836);
	InterruptSource(837);
	InterruptSource(838);
	InterruptSource(839);
	InterruptSource(840);
	InterruptSource(841);
	InterruptSource(842);
	InterruptSource(843);
	InterruptSource(844);
	InterruptSource(845);
	InterruptSource(846);
	InterruptSource(847);
	InterruptSource(848);
	InterruptSource(849);
	InterruptSource(850);
	InterruptSource(851);
	InterruptSource(852);
	InterruptSource(853);
	InterruptSource(854);
	InterruptSource(855);
	InterruptSource(856);
	InterruptSource(857);
	InterruptSource(858);
	InterruptSource(859);
	InterruptSource(860);
	InterruptSource(861);
	InterruptSource(862);
	InterruptSource(863);
	InterruptSource(864);
	InterruptSource(865);
	InterruptSource(866);
	InterruptSource(867);
	InterruptSource(868);
	InterruptSource(869);
	InterruptSource(870);
	InterruptSource(871);
	InterruptSource(872);
	InterruptSource(873);
	InterruptSource(874);
	InterruptSource(875);
	InterruptSource(876);
	InterruptSource(877);
	InterruptSource(878);
	InterruptSource(879);
	InterruptSource(880);
	InterruptSource(881);
	InterruptSource(882);
	InterruptSource(883);
	InterruptSource(884);
	InterruptSource(885);
	InterruptSource(886);
	InterruptSource(887);
	InterruptSource(888);
	InterruptSource(889);
	InterruptSource(890);
	InterruptSource(891);
	InterruptSource(892);
	InterruptSource(893);
	InterruptSource(894);
	InterruptSource(895);
	InterruptSource(896);
	InterruptSource(897);
	InterruptSource(898);
	InterruptSource(899);
	InterruptSource(900);
	InterruptSource(901);
	InterruptSource(902);
	InterruptSource(903);
	InterruptSource(904);
	InterruptSource(905);
	InterruptSource(906);
	InterruptSource(907);
	InterruptSource(908);
	InterruptSource(909);
	InterruptSource(910);
	InterruptSource(911);
	InterruptSource(912);
	InterruptSource(913);
	InterruptSource(914);
	InterruptSource(915);
	InterruptSource(916);
	InterruptSource(917);
	InterruptSource(918);
	InterruptSource(919);
	InterruptSource(920);
	InterruptSource(921);
	InterruptSource(922);
	InterruptSource(923);
	InterruptSource(924);
	InterruptSource(925);
	InterruptSource(926);
	InterruptSource(927);
	InterruptSource(928);
	InterruptSource(929);
	InterruptSource(930);
	InterruptSource(931);
	InterruptSource(932);
	InterruptSource(933);
	InterruptSource(934);
	InterruptSource(935);
	InterruptSource(936);
	InterruptSource(937);
	InterruptSource(938);
	InterruptSource(939);
	InterruptSource(940);
	InterruptSource(941);
	InterruptSource(942);
	InterruptSource(943);
	InterruptSource(944);
	InterruptSource(945);
	InterruptSource(946);
	InterruptSource(947);
	InterruptSource(948);
	InterruptSource(949);
	InterruptSource(950);
	InterruptSource(951);
	InterruptSource(952);
	InterruptSource(953);
	InterruptSource(954);
	InterruptSource(955);
	InterruptSource(956);
	InterruptSource(957);
	InterruptSource(958);
	InterruptSource(959);
	InterruptSource(960);
	InterruptSource(961);
	InterruptSource(962);
	InterruptSource(963);
	InterruptSource(964);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	main_core_0->memory_import >> xbar_0->memory_export;
	main_core_1->memory_import >> xbar_0->memory_export;
	peripheral_core_2->memory_import >> xbar_1->memory_export;

	(*xbar_0->memory_import[0]) >> flash->memory_export;
	(*xbar_0->memory_import[2]) >> main_core_0->memory_export;
	(*xbar_0->memory_import[3]) >> main_core_1->memory_export;
	(*xbar_0->memory_import[4]) >> system_sram->memory_export;
	(*xbar_0->memory_import[7]) >> xbar_1->memory_export;
	
	(*xbar_1->memory_import[1]) >> peripheral_core_2->memory_export;
		
	if(debugger)
	{
		// Connect debugger to CPUs
		
		// Main Core 0
		main_core_0->debug_yielding_import                   >> *debugger->debug_yielding_export[0];
		main_core_0->trap_reporting_import                   >> *debugger->trap_reporting_export[0];
		main_core_0->memory_access_reporting_import          >> *debugger->memory_access_reporting_export[0];
		*debugger->disasm_import                         [0] >> main_core_0->disasm_export;
		*debugger->memory_import                         [0] >> main_core_0->memory_export;
		*debugger->registers_import                      [0] >> main_core_0->registers_export;
		*debugger->memory_access_reporting_control_import[0] >> main_core_0->memory_access_reporting_control_export;

		// Main Core 1
		main_core_1->debug_yielding_import                   >> *debugger->debug_yielding_export[1];
		main_core_1->trap_reporting_import                   >> *debugger->trap_reporting_export[1];
		main_core_1->memory_access_reporting_import          >> *debugger->memory_access_reporting_export[1];
		*debugger->disasm_import                         [1] >> main_core_1->disasm_export;
		*debugger->memory_import                         [1] >> main_core_1->memory_export;
		*debugger->registers_import                      [1] >> main_core_1->registers_export;
		*debugger->memory_access_reporting_control_import[1] >> main_core_1->memory_access_reporting_control_export;

		// Peripheral Core 2
		peripheral_core_2->debug_yielding_import             >> *debugger->debug_yielding_export[2];
		peripheral_core_2->trap_reporting_import             >> *debugger->trap_reporting_export[2];
		peripheral_core_2->memory_access_reporting_import    >> *debugger->memory_access_reporting_export[2];
		*debugger->disasm_import                         [2] >> peripheral_core_2->disasm_export;
		*debugger->memory_import                         [2] >> peripheral_core_2->memory_export;
		*debugger->registers_import                      [2] >> peripheral_core_2->registers_export;
		*debugger->memory_access_reporting_control_import[2] >> peripheral_core_2->memory_access_reporting_control_export;
		
		// Connect debugger to loader
		debugger->blob_import >> loader->blob_export;
	}
	
	unsigned int front_end_num = 0;
	
	if(enable_gdb_server)
	{
		// Connect gdb-server to debugger
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++, front_end_num++)
		{
			*debugger->debug_yielding_import[front_end_num]       >> gdb_server[prc_num]->debug_yielding_export;
			*debugger->debug_event_listener_import[front_end_num] >> gdb_server[prc_num]->debug_event_listener_export;
			gdb_server[prc_num]->debug_yielding_request_import    >> *debugger->debug_yielding_request_export[front_end_num];
			gdb_server[prc_num]->debug_selecting_import           >> *debugger->debug_selecting_export[front_end_num];
			gdb_server[prc_num]->debug_event_trigger_import       >> *debugger->debug_event_trigger_export[front_end_num];
			gdb_server[prc_num]->memory_import                    >> *debugger->memory_export[front_end_num];
			gdb_server[prc_num]->registers_import                 >> *debugger->registers_export[front_end_num];
		}
	}
	else
	{
		front_end_num += NUM_PROCESSORS;
	}

	if(enable_inline_debugger)
	{
		// Connect inline-debugger to debugger
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++, front_end_num++)
		{
			*debugger->debug_event_listener_import[front_end_num]   >> inline_debugger[prc_num]->debug_event_listener_export;
			*debugger->debug_yielding_import[front_end_num]         >> inline_debugger[prc_num]->debug_yielding_export;
			inline_debugger[prc_num]->debug_yielding_request_import >> *debugger->debug_yielding_request_export[front_end_num];
			inline_debugger[prc_num]->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[front_end_num];
			inline_debugger[prc_num]->disasm_import                 >> *debugger->disasm_export[front_end_num];
			inline_debugger[prc_num]->memory_import                 >> *debugger->memory_export[front_end_num];
			inline_debugger[prc_num]->registers_import              >> *debugger->registers_export[front_end_num];
			inline_debugger[prc_num]->stmt_lookup_import            >> *debugger->stmt_lookup_export[front_end_num];
			inline_debugger[prc_num]->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[front_end_num];
			inline_debugger[prc_num]->backtrace_import              >> *debugger->backtrace_export[front_end_num];
			inline_debugger[prc_num]->debug_info_loading_import     >> *debugger->debug_info_loading_export[front_end_num];
			inline_debugger[prc_num]->data_object_lookup_import     >> *debugger->data_object_lookup_export[front_end_num];
			inline_debugger[prc_num]->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[front_end_num];
		}
	}
	else
	{
		front_end_num += NUM_PROCESSORS;
	}

	if(enable_profiler)
	{
		// Connect profiler to debugger
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++, front_end_num++)
		{
			*debugger->debug_event_listener_import[front_end_num] >> profiler[prc_num]->debug_event_listener_export;
			*debugger->debug_yielding_import[front_end_num]       >> profiler[prc_num]->debug_yielding_export;
			profiler[prc_num]->debug_yielding_request_import      >> *debugger->debug_yielding_request_export[front_end_num];
			profiler[prc_num]->debug_event_trigger_import         >> *debugger->debug_event_trigger_export[front_end_num];
			profiler[prc_num]->disasm_import                      >> *debugger->disasm_export[front_end_num];
			profiler[prc_num]->memory_import                      >> *debugger->memory_export[front_end_num];
			profiler[prc_num]->registers_import                   >> *debugger->registers_export[front_end_num];
			profiler[prc_num]->stmt_lookup_import                 >> *debugger->stmt_lookup_export[front_end_num];
			profiler[prc_num]->symbol_table_lookup_import         >> *debugger->symbol_table_lookup_export[front_end_num];
			profiler[prc_num]->backtrace_import                   >> *debugger->backtrace_export[front_end_num];
			profiler[prc_num]->debug_info_loading_import          >> *debugger->debug_info_loading_export[front_end_num];
			profiler[prc_num]->data_object_lookup_import          >> *debugger->data_object_lookup_export[front_end_num];
			profiler[prc_num]->subprogram_lookup_import           >> *debugger->subprogram_lookup_export[front_end_num];
		}
	}
	else
	{
		front_end_num += NUM_PROCESSORS;
	}

	(*loader->memory_import[0]) >> flash->memory_export;
	(*loader->memory_import[1]) >> system_sram->memory_export;
	(*loader->memory_import[2]) >> main_core_0->memory_export;
	(*loader->memory_import[3]) >> main_core_1->memory_export;
	(*loader->memory_import[4]) >> peripheral_core_2->memory_export;
	loader->registers_import >> peripheral_core_2->registers_export;
	peripheral_core_2->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	
	SC_HAS_PROCESS(Simulator);
	
	SC_THREAD(ResetProcess);
}

Simulator::~Simulator()
{
	if(main_core_0) delete main_core_0;
	if(main_core_1) delete main_core_1;
	if(peripheral_core_2) delete peripheral_core_2;
	if(system_sram) delete system_sram;
	if(flash) delete flash;
	if(xbar_0) delete xbar_0;
	if(xbar_1) delete xbar_1;
	if(pbridge_a) delete pbridge_a;
	if(pbridge_b) delete pbridge_b;
	if(intc_0) delete intc_0;
	if(stm_0) delete stm_0;
	if(stm_1) delete stm_1;
	if(stm_2) delete stm_2;
	if(swt_0) delete swt_0;
	if(swt_1) delete swt_1;
	if(swt_2) delete swt_2;
	if(swt_3) delete swt_3;
	if(pit_0) delete pit_0;
	if(pit_1) delete pit_1;
	if(linflexd_0) delete linflexd_0; 
	if(linflexd_1) delete linflexd_1; 
	if(linflexd_2) delete linflexd_2; 
	if(linflexd_14) delete linflexd_14;
	if(linflexd_15) delete linflexd_15;
	if(linflexd_16) delete linflexd_16;
	if(ebi_stub) delete ebi_stub;
	if(flash_port1_stub) delete flash_port1_stub;
	if(xbar_0_s6_stub) delete xbar_0_s6_stub;
	if(xbar_1_m1_stub) delete xbar_1_m1_stub;
	if(xbar_1_m2_stub) delete xbar_1_m2_stub;
	if(gdb_server[0]) delete gdb_server[0];
	if(gdb_server[1]) delete gdb_server[1];
	if(gdb_server[2]) delete gdb_server[2];
	if(inline_debugger[0]) delete inline_debugger[0];
	if(inline_debugger[1]) delete inline_debugger[1];
	if(inline_debugger[2]) delete inline_debugger[2];
	if(profiler[0]) delete profiler[0];
	if(profiler[1]) delete profiler[1];
	if(profiler[2]) delete profiler[2];
	if(debugger) delete debugger;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(loader) delete loader;
}

void Simulator::ResetProcess()
{
// 	sc_core::sc_signal<sc_dt::sc_uint<30> >& p_rstbase = GetSignal<sc_dt::sc_uint<30> >("HARDWARE.p_rstbase");
// 	p_rstbase = sc_dt::sc_uint<30>(0x404100 /*0x13a0000*/ /*0x1500000*/ >> 2);
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& p_reset_b_2 = GetSignal<bool, sc_core::SC_MANY_WRITERS>("HARDWARE.p_reset_b_2");
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& p_reset_b_0 = GetSignal<bool, sc_core::SC_MANY_WRITERS>("HARDWARE.p_reset_b_0");
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& p_reset_b_1 = GetSignal<bool, sc_core::SC_MANY_WRITERS>("HARDWARE.p_reset_b_1");

	p_reset_b_2 = false;
	wait(sc_core::sc_time(10.0, sc_core::SC_NS));
	p_reset_b_2 = true;
	
	wait(sc_core::sc_time(990.0, sc_core::SC_NS));
	p_reset_b_0 = false;
	p_reset_b_1 = false;
	wait(sc_core::sc_time(10.0, sc_core::SC_NS));
	p_reset_b_0 = true;
	p_reset_b_1 = true;

// 	wait(sc_core::sc_time(40.0, sc_core::SC_NS));
// 	sc_core::sc_signal<sc_dt::sc_uint<14> >& p_voffset = GetSignal<sc_dt::sc_uint<14> >("HARDWARE.p_voffset");
// 	p_voffset = sc_dt::sc_uint<14>(0x1234);
// 
// 	sc_core::sc_signal<bool>& p_avec_b = GetSignal<bool>("HARDWARE.p_avec_b");
// 	p_avec_b = true;
// 
// 	sc_core::sc_signal<bool>& p_extint_b = GetSignal<bool>("HARDWARE.p_extint_b");
// 	p_extint_b = false;
// 	wait(sc_core::sc_time(10.0, sc_core::SC_NS));
// 	p_extint_b = true;

// 	wait(sc_core::sc_time(10.0, sc_core::SC_US));
// 	std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Sending IRQ#33" << std::endl;
// 	sc_core::sc_signal<bool>& irq = GetSignal<bool>("HARDWARE.irq_33");
// 	irq = 1;
	
}

void Simulator::InterruptSource(unsigned int irq_num, const std::string& source)
{
	std::stringstream irq_signal_name_sstr;
	irq_signal_name_sstr << "HARDWARE.irq_" << irq_num;
	if(!source.empty())
	{
		Bind(source, irq_signal_name_sstr.str());
	}
	std::stringstream intc_port_name_sstr;
	intc_port_name_sstr << "HARDWARE.INTC_0.hw_irq_" << (irq_num - INTC_0::NUM_SW_IRQS);
	Bind(intc_port_name_sstr.str() , irq_signal_name_sstr.str());
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM MPC5777M");
	simulator->SetVariable("copyright", "Copyright (C) 2017, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM MPC5777M, MPC5777M SoC simulator");
	simulator->SetVariable("schematic", "mpc5777m/fig_schematic.pdf");

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - Clocks
	simulator->SetVariable("HARDWARE.COMP_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.COMP_CLK.clock-period", "3333 ps");   // Computationnal Shell: 300 MHz
	simulator->SetVariable("HARDWARE.IOP_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.IOP_CLK.clock-period", "5 ns");       // I/O processor: 200 MHz
	simulator->SetVariable("HARDWARE.FXBAR_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.FXBAR_CLK.clock-period", "5 ns");     // Fast crossbar: 200 Mhz
	simulator->SetVariable("HARDWARE.SXBAR_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SXBAR_CLK.clock-period", "10 ns");    // Slow crossbar: 100 Mhz
	simulator->SetVariable("HARDWARE.PBRIDGEA_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.PBRIDGEA_CLK.clock-period", "20 ns"); // PBRIDGE_A: 50 Mhz
	simulator->SetVariable("HARDWARE.PBRIDGEB_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.PBRIDGEB_CLK.clock-period", "20 ns"); // PBRIDGE_B: 50 Mhz
	simulator->SetVariable("HARDWARE.PER_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.PER_CLK.clock-period", "12500 ps");   // PER_CLK: 80 Mhz
	simulator->SetVariable("HARDWARE.RTI_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.RTI_CLK.clock-period", "1 us");       // RTI_CLK: 1 Mhz
	simulator->SetVariable("HARDWARE.LIN_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.LIN_CLK.clock-period", "12500 ps");      // LIN_CLK: 80 Mhz ((2/3) * LIN_CLK > PBRIDGEx_CLK > (1/3) * LIN_CLK)
	
	//  - e200 PowerPC cores

	// Peripheral_Core_2
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.ahb-master-id", 2);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.clock-multiplier", 1.0);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.max-inst", ~uint64_t(0));
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.nice-time", "200 ns");
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.ipc", 2.0);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.enable-dmi", true);

	simulator->SetVariable("HARDWARE.Peripheral_Core_2.local-memory-base-addr", 0x52000000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.local-memory-size", 8 * 1024 * 1024);

	simulator->SetVariable("HARDWARE.Peripheral_Core_2.DMEM.base-addr", 0x52800000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.DMEM.size", 64 * 1024);

	simulator->SetVariable("HARDWARE.Peripheral_Core_2.IMEM.base-addr", 0x52000000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.IMEM.size", 16 * 1024);
	
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.processor-version", 0x815f8000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.system-version", 0x0);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.system-information", 0x2);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.cpuid", 0x2);

	//FIXME: reset address
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.reset-addr", 0x1500000);
	simulator->SetVariable("HARDWARE.Main_Core_0.reset-addr", 0x1000000);
	simulator->SetVariable("HARDWARE.Main_Core_1.reset-addr", 0x1280000);
	
	// Main_Core_0
	simulator->SetVariable("HARDWARE.Main_Core_0.ahb-master-id", 0);
	simulator->SetVariable("HARDWARE.Main_Core_0.clock-multiplier", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_0.max-inst", ~uint64_t(0));
	simulator->SetVariable("HARDWARE.Main_Core_0.nice-time", "200 ns");
	simulator->SetVariable("HARDWARE.Main_Core_0.ipc", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_0.enable-dmi", true);

	simulator->SetVariable("HARDWARE.Main_Core_0.local-memory-base-addr", 0x50000000);
	simulator->SetVariable("HARDWARE.Main_Core_0.local-memory-size", 8 * 1024 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_0.DMEM.base-addr", 0x50800000);
	simulator->SetVariable("HARDWARE.Main_Core_0.DMEM.size", 64 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_0.IMEM.base-addr", 0x50000000);
	simulator->SetVariable("HARDWARE.Main_Core_0.IMEM.size", 16 * 1024);
	
	simulator->SetVariable("HARDWARE.Main_Core_0.processor-version", 0x81b00000);
	simulator->SetVariable("HARDWARE.Main_Core_0.system-version", 0x0);
	simulator->SetVariable("HARDWARE.Main_Core_0.system-information", 0x0);
	simulator->SetVariable("HARDWARE.Main_Core_0.cpuid", 0x0);
	
	// Main_Core_1
	simulator->SetVariable("HARDWARE.Main_Core_1.ahb-master-id", 1);
	simulator->SetVariable("HARDWARE.Main_Core_1.clock-multiplier", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_1.max-inst", ~uint64_t(0));
	simulator->SetVariable("HARDWARE.Main_Core_1.nice-time", "200 ns");
	simulator->SetVariable("HARDWARE.Main_Core_1.ipc", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_1.enable-dmi", true);

	simulator->SetVariable("HARDWARE.Main_Core_1.local-memory-base-addr", 0x51000000);
	simulator->SetVariable("HARDWARE.Main_Core_1.local-memory-size", 8 * 1024 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_1.DMEM.base-addr", 0x51800000);
	simulator->SetVariable("HARDWARE.Main_Core_1.DMEM.size", 64 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_1.IMEM.base-addr", 0x51000000);
	simulator->SetVariable("HARDWARE.Main_Core_1.IMEM.size", 16 * 1024);
	
	simulator->SetVariable("HARDWARE.Main_Core_1.processor-version", 0x81b00000);
	simulator->SetVariable("HARDWARE.Main_Core_1.system-version", 0x0);
	simulator->SetVariable("HARDWARE.Main_Core_1.system-information", 0x1);
	simulator->SetVariable("HARDWARE.Main_Core_1.cpuid", 0x1);

	//  - XBAR_0
	simulator->SetVariable("HARDWARE.XBAR_0.cycle_time", "5 ns");
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_0",  "range_start=\"0x00400000\" range_end=\"0x00407fff\" output_port=\"0\" translation=\"0x0\"");        //   0 KB: UTest NVM Block (32 KB)         -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_1",  "range_start=\"0x0060c000\" range_end=\"0x0062ffff\" output_port=\"0\" translation=\"0x8000\"");     //  32 KB: HSM Code (144 KB)               -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_2",  "range_start=\"0x00680000\" range_end=\"0x00687fff\" output_port=\"0\" translation=\"0x2c000\"");    // 176 KB: HSM Data (32 KB)                -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_3",  "range_start=\"0x00800000\" range_end=\"0x0087ffff\" output_port=\"0\" translation=\"0x34000\"");    // 208 KB: Data Flash (512 KB)             -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_4",  "range_start=\"0x00fc0000\" range_end=\"0x00ffffff\" output_port=\"0\" translation=\"0xb4000\"");    // 720 KB: Low & Mid Flash Blocks (256 KB) -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_5",  "range_start=\"0x01000000\" range_end=\"0x0177ffff\" output_port=\"0\" translation=\"0xf4000\"");    // 976 KB: Large Flash Blocks (7680 KB)    -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_6",  "range_start=\"0x50000000\" range_end=\"0x50ffffff\" output_port=\"2\" translation=\"0x0\"");        // Main_Core_0 Local Memory                -> Main_Core_0 (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_7",  "range_start=\"0x51000000\" range_end=\"0x51ffffff\" output_port=\"3\" translation=\"0x0\"");        // Main_Core_1 Local Memory                -> Main_Core_1 (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_8",  "range_start=\"0x40000000\" range_end=\"0x401fffff\" output_port=\"4\" translation=\"0x0\"");        // System SRAM                             -> SYSTEM_SRAM (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_9",  "range_start=\"0x52000000\" range_end=\"0x5fffffff\" output_port=\"7\" translation=\"0x52000000\""); // Peripheral_Core_2 Local Memory          -> XBAR_1 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_10", "range_start=\"0xf0000000\" range_end=\"0xffffffff\" output_port=\"7\" translation=\"0xf0000000\""); // Peripherals PBRIDGE_A, PBRIDGE_B        -> XBAR_1 (abs address)
	
	//  - XBAR_1
	simulator->SetVariable("HARDWARE.XBAR_1.cycle_time", "10 ns");
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_0",  "range_start=\"0x00400000\" range_end=\"0x00407fff\" output_port=\"0\" translation=\"0x00400000\""); // UTest NVM Block (32 KB)         -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_1",  "range_start=\"0x0060c000\" range_end=\"0x0062ffff\" output_port=\"0\" translation=\"0x0060c000\""); // HSM Code (144 KB)               -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_2",  "range_start=\"0x00680000\" range_end=\"0x00687fff\" output_port=\"0\" translation=\"0x00680000\""); // HSM Data (32 KB)                -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_3",  "range_start=\"0x00800000\" range_end=\"0x0087ffff\" output_port=\"0\" translation=\"0x00800000\""); // Data Flash (512 KB)             -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_4",  "range_start=\"0x00fc0000\" range_end=\"0x00ffffff\" output_port=\"0\" translation=\"0x00fc0000\""); // Low & Mid Flash Blocks (256 KB) -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_5",  "range_start=\"0x01000000\" range_end=\"0x0177ffff\" output_port=\"0\" translation=\"0x01000000\""); // Large Flash Blocks (7680 KB)    -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_6",  "range_start=\"0x50000000\" range_end=\"0x50ffffff\" output_port=\"0\" translation=\"0x50000000\""); // Main_Core_0 Local Memory        -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_7",  "range_start=\"0x51000000\" range_end=\"0x51ffffff\" output_port=\"0\" translation=\"0x51000000\""); // Main_Core_1 Local Memory        -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_8",  "range_start=\"0x40000000\" range_end=\"0x401fffff\" output_port=\"0\" translation=\"0x40000000\""); // System SRAM                     -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_9",  "range_start=\"0x52000000\" range_end=\"0x5fffffff\" output_port=\"1\" translation=\"0x0\"");        // Peripheral_Core_2 Local Memory  -> Peripheral_Core_2 (rel address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_10", "range_start=\"0xf0000000\" range_end=\"0xfbffffff\" output_port=\"2\" translation=\"0xf0000000\""); // PBRIDGE_B                       -> PBRIDGE_B (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_11", "range_start=\"0xfc000000\" range_end=\"0xffffffff\" output_port=\"3\" translation=\"0xfc000000\""); // PBRIDGE_A                       -> PBRIDGE_B (abs address)
	
	//  - PBRIDGE_A
	simulator->SetVariable("HARDWARE.PBRIDGE_A.cycle_time", "20 ns");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_0",  "range_start=\"0xfc040000\" range_end=\"0xfc04ffff\" output_port=\"0\"  translation=\"0x0\""); // INTC        -> INTC  (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_1",  "range_start=\"0xfc068000\" range_end=\"0xfc06bfff\" output_port=\"1\"  translation=\"0x0\""); // STM_0       -> STM_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_2",  "range_start=\"0xfc06c000\" range_end=\"0xfc06ffff\" output_port=\"2\"  translation=\"0x0\""); // STM_1       -> STM_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_3",  "range_start=\"0xfc070000\" range_end=\"0xfc073fff\" output_port=\"3\"  translation=\"0x0\""); // STM_2       -> STM_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_4",  "range_start=\"0xfc050000\" range_end=\"0xfc053fff\" output_port=\"4\"  translation=\"0x0\""); // SWT_0       -> SWT_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_5",  "range_start=\"0xfc054000\" range_end=\"0xfc057fff\" output_port=\"5\"  translation=\"0x0\""); // SWT_1       -> SWT_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_6",  "range_start=\"0xfc058000\" range_end=\"0xfc05bfff\" output_port=\"6\"  translation=\"0x0\""); // SWT_2       -> SWT_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_7",  "range_start=\"0xfc05c000\" range_end=\"0xfc05ffff\" output_port=\"7\"  translation=\"0x0\""); // SWT_3       -> SWT_3 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_8",  "range_start=\"0xfff84000\" range_end=\"0xfff87fff\" output_port=\"8\"  translation=\"0x0\""); // PIT_0       -> PIT_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_9",  "range_start=\"0xfff80000\" range_end=\"0xfff83fff\" output_port=\"9\"  translation=\"0x0\""); // PIT_1       -> PIT_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_10", "range_start=\"0xffe8c000\" range_end=\"0xffe8ffff\" output_port=\"10\" translation=\"0x0\""); // LINFlexD_0  -> LINFlexD_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_11", "range_start=\"0xffe90000\" range_end=\"0xffe93fff\" output_port=\"11\" translation=\"0x0\""); // LINFlexD_1  -> LINFlexD_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_12", "range_start=\"0xffea8000\" range_end=\"0xffeabfff\" output_port=\"12\" translation=\"0x0\""); // LINFlexD_14 -> LINFlexD_14 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_13", "range_start=\"0xffeac000\" range_end=\"0xffeaffff\" output_port=\"13\" translation=\"0x0\""); // LINFlexD_16 -> LINFlexD_16 (rel address)

	//  - PBRIDGE_B
	simulator->SetVariable("HARDWARE.PBRIDGE_B.cycle_time", "20 ns");
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_0",  "range_start=\"0xfbe8c000\" range_end=\"0xfbe8ffff\" output_port=\"0\" translation=\"0x0\""); //  LINFlexD_2 -> LINFlexD_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_1",  "range_start=\"0xfbea8000\" range_end=\"0xfbeabfff\" output_port=\"1\" translation=\"0x0\""); // LINFlexD_15 -> LINFlexD_15 (rel address)
	
	
	// - Loader
	simulator->SetVariable("loader.filename", "baf.bin,soft/bin/Z4_2/flash_boot.elf,soft/bin/Z7_0/flash_boot.elf,soft/bin/Z7_1/flash_boot.elf");
	simulator->SetVariable("loader.file0.base-addr", 0x00404000UL);
	
	// - Loader memory router
	simulator->SetVariable("loader.memory-mapper.mapping",
	                       "HARDWARE.SYSTEM-SRAM:0x40000000-0x401fffff:+0x0"        // System SRAM                             -> SYSTEM_SRAM       (rel address)
	                       ",HARDWARE.FLASH:0x00400000-0x00407fff:+0x0"             //   0 KB: UTest NVM Block (32 KB)         -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x0060c000-0x0062ffff:+0x8000"          //  32 KB: HSM Code (144 KB)               -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x00680000-0x00687fff:+0x2c000"         // 176 KB: HSM Data (32 KB)                -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x00800000-0x0087ffff:+0x34000"         // 208 KB: Data Flash (512 KB)             -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x00fc0000-0x00ffffff:+0xb4000"         // 720 KB: Low & Mid Flash Blocks (256 KB) -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x01000000-0x0177ffff:+0xf4000"         // 976 KB: Large Flash Blocks (7680 KB)    -> FLASH             (rel address)
	                       ",HARDWARE.Main_Core_0:0x50000000-0x50ffffff:+0x0"       // Main_Core_0 Local Memory                -> Main_Core_0       (rel address)
	                       ",HARDWARE.Main_Core_1:0x51000000-0x51ffffff:+0x0"       // Main_Core_1 Local Memory                -> Main_Core_1       (rel address)
	                       ",HARDWARE.Peripheral_Core_2:0x52000000-0x5fffffff:+0x0" // Peripheral_Core_2 Local Memory          -> Peripheral_Core_2 (rel address)
	                      );

	//  - System SRAM
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.cycle-time", "10 ns");
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.read-latency", "10 ns");
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.write-latency", "10 ns");
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.org", 0x0);
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.bytesize", 404 * 1024);
	
	//  - FLASH
	simulator->SetVariable("HARDWARE.FLASH.cycle-time", "10 ns");
	simulator->SetVariable("HARDWARE.FLASH.read-latency", "10 ns");
	simulator->SetVariable("HARDWARE.FLASH.write-latency", "10 ns");
	simulator->SetVariable("HARDWARE.FLASH.org", 0x0);
	simulator->SetVariable("HARDWARE.FLASH.bytesize", (8640 + 16) * 1024); // Note: 8640 KB (main space) + 16 KB OTP (UTEST space)
	simulator->SetVariable("HARDWARE.FLASH.read-only", true);

	//  - SWT_0
	simulator->SetVariable("HARDWARE.SWT_0.swt-cr-reset-value", 0xff00010a);
	simulator->SetVariable("HARDWARE.SWT_0.swt-to-reset-value", 0x0005fcd0);

	//  - SWT_1
	simulator->SetVariable("HARDWARE.SWT_1.swt-cr-reset-value", 0xff00010a);
	simulator->SetVariable("HARDWARE.SWT_1.swt-to-reset-value", 0x0005fcd0);

	//  - SWT_2
	simulator->SetVariable("HARDWARE.SWT_2.swt-cr-reset-value", 0xff00011b);
	simulator->SetVariable("HARDWARE.SWT_2.swt-to-reset-value", 0x0003fde0);
	
	//  - SWT_3
	simulator->SetVariable("HARDWARE.SWT_3.swt-cr-reset-value", 0xff00010a);
	simulator->SetVariable("HARDWARE.SWT_3.swt-to-reset-value", 0x0005fcd0);

	//  - PIT_0
	simulator->SetVariable("HARDWARE.PIT_0.pit-mcr-reset-value", 0x06);
	
	//  - PIT_1
	simulator->SetVariable("HARDWARE.PIT_1.pit-mcr-reset-value", 0x02);

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server0.tcp-port", 12345);
	simulator->SetVariable("gdb-server0.architecture-description-filename", "gdb_powerpc_vle.xml");
	simulator->SetVariable("gdb-server1.tcp-port", 12346);
	simulator->SetVariable("gdb-server1.architecture-description-filename", "gdb_powerpc_vle.xml");
	simulator->SetVariable("gdb-server2.tcp-port", 12347);
	simulator->SetVariable("gdb-server2.architecture-description-filename", "gdb_powerpc_vle.xml");
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", "powerpc_e500_gcc_dwarf_register_number_mapping.xml");
	simulator->SetVariable("debugger.sel-cpu[0]", 0); // gdb-server
	simulator->SetVariable("debugger.sel-cpu[1]", 1); // gdb-server
	simulator->SetVariable("debugger.sel-cpu[2]", 2); // gdb-server
	simulator->SetVariable("debugger.sel-cpu[3]", 0); // inline-debugger
	simulator->SetVariable("debugger.sel-cpu[4]", 1); // inline-debugger
	simulator->SetVariable("debugger.sel-cpu[5]", 2); // inline-debugger
	simulator->SetVariable("debugger.sel-cpu[6]", 0); // profiler
	simulator->SetVariable("debugger.sel-cpu[7]", 1); // profiler
	simulator->SetVariable("debugger.sel-cpu[8]", 2); // profiler
}

void Simulator::Run()
{
	cerr << "Starting simulation at supervisor privilege level" << endl;
	
	double time_start = host_time->GetTime();

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

	uint64_t instruction_counter = (uint64_t)(*peripheral_core_2)["instruction-counter"] + (uint64_t)(*main_core_0)["instruction-counter"] + (uint64_t)(*main_core_1)["instruction-counter"];
	double run_time = (double)(*peripheral_core_2)["run-time"] + (double)(*main_core_0)["run-time"] + (double)(*main_core_1)["run-time"];
	double idle_time = (double)(*peripheral_core_2)["idle-time"] + (double)(*main_core_0)["idle-time"] + (double)(*main_core_1)["idle-time"];
	
	cerr << "simulation time: " << spent_time << " seconds" << endl;
	cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
	cerr << "target speed: " << (instruction_counter / (run_time - idle_time) / 1000000.0) << " MIPS" << endl;
	cerr << "host simulation speed: " << ((double) instruction_counter / spent_time / 1000000.0) << " MIPS" << endl;
	cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
	
	if(enable_profiler)
	{
		profiler[0]->Output();
		profiler[1]->Output();
		profiler[2]->Output();
	}
}

unisim::kernel::service::Simulator::SetupStatus Simulator::Setup()
{
	if(enable_inline_debugger || enable_profiler)
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

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	if(DEBUG_ENABLE)
	{
		std::cerr << "Call stack:" << std::endl;
		std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
	if(sc_get_status() != SC_STOPPED)
	{
		sc_stop();
	}
	if(!asynchronous)
	{
		sc_process_handle h = sc_get_current_process_handle();
		switch(h.proc_kind())
		{
			case SC_THREAD_PROC_: 
			case SC_CTHREAD_PROC_:
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
		unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
	}
}