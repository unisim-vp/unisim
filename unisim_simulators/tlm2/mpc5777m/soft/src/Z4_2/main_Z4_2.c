/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include "intc.h"
#include "stm.h"
#include "swt.h"
#include "pit.h"
#include "linflexd.h"
#include "dmamux.h"
#include "edma.h"

#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful

#define UART_FIFO_MODE 1
#define UART_RX_FIFO_DMA_MODE 1

void hw_init(void)
{
#if defined(DEBUG_SECONDARY_CORES)
	uint32_t mctl = MC_ME.MCTL.R;
#if defined(TURN_ON_CPU0)
	/* enable core 0 in all modes */
	MC_ME.CCTL1.R = 0x00FE;
	/* Set Start address for core 0: Will reset and start */
#if defined(START_FROM_FLASH)
    MC_ME.CADDR1.R = 0x1000000 | 0x1;
#else
    MC_ME.CADDR1.R = 0x40000000 | 0x1;
#endif /* defined(START_FROM_FLASH) */ 

#endif
#if defined(TURN_ON_CPU1)
	/* enable core 1 in all modes */
	MC_ME.CCTL3.R = 0x00FE;
	/* Set Start address for core 1: Will reset and start */
#if defined(START_FROM_FLASH)
    MC_ME.CADDR3.R = 0x1280000 | 0x1;
#else
    MC_ME.CADDR3.R = 0x40020000 | 0x1;
#endif /* defined(START_FROM_FLASH) */ 
    
#endif
	MC_ME.MCTL.R = (mctl & 0xffff0000ul) | KEY_VALUE1;
	MC_ME.MCTL.R =  mctl; /* key value 2 always from MCTL */
#endif /* defined(DEBUG_SECONDARY_CORES) */

	intc_drv_init();
	dmamux_drv_init();
	edma_drv_init();
	stm_drv_init();
	swt_drv_init();
	pit_drv_init();
	linflexd_drv_init();

	intc_init();      // initialize interrupt controller
	edma_init(0);     // initialize eDMA_0
	edma_init(1);     // initialize eDMA_1
	swt_init(2);      // initialize SWT_2
	stm_init(2);      // initialize STM_2
	pit_init(0);      // initialize PIT_0
	pit_init(1);      // initialize PIT_1
	linflexd_init(0); // initialize LINFlexD_0
	intc_enable_external_interrupt(); // Enable processor external interrupts
}

uint32_t ticks_1us;
uint32_t stm_period_us;

void periodic_task(unsigned int stm_id, unsigned int chan)
{
	swt_service_sequence(2);                   // rearm SWT_2
	uint32_t old_ticks_1us = ticks_1us;
	ticks_1us = pit_get_lifetime(1) >> 32;     // read PIT_1 lifetime (high 32-bit counts down 1 us ticks)
	stm_period_us = old_ticks_1us - ticks_1us; // compute effective STM_2 period in us using PIT_1 (should be 200 but first time)
	stm_set_channel_compare(stm_id, chan, stm_get_channel_compare(stm_id, chan) + 10000); // schedule next STM_2 tic 200 us later (i.e. 10000 cycles at 50 MHz)
	stm_clear_interrupt_flag(stm_id, chan);    // clear STM_2 interrupt flag
}

void periodic_task2(unsigned int pit_id, unsigned int chan)
{
	pit_clear_timer_interrupt_flag(pit_id, chan); // clear PIT_0 interrupt flag
}

void linflexd_0_int_rx(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int)
{
	if(linflexd_get_uart_data_received_interrupt_flag(linflexd_id))
	{
		linflexd_clear_uart_data_received_interrupt_flag(linflexd_id); // LINFlexD_0 (Normal mode): clear data received interrupt flag
		
		char ch = 0;
		linflexd_uart_rx_buffer_read_byte(linflexd_id, &ch, 0);        // LINFlexD_0 (Normal mode): receive character
			
		linflexd_release_uart_rx_buffer(linflexd_id);                  // LINFlexD_0 (Normal mode): release message buffer
		
		linflexd_uart_tx_buffer_write_byte(linflexd_id, &ch, 0);       // LINFlexD_0 (Normal mode): send character
	}
}

void linflexd_0_int_tx(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int)
{
	if(linflexd_get_uart_data_transmitted_interrupt_flag(linflexd_id))
	{
		linflexd_clear_uart_data_transmitted_interrupt_flag(linflexd_id);       // LINFlexD_0 (Normal mode): clear transmission complete flag
	}
}

void linflexd_0_int_err(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int)
{
	while(1);
}

volatile unsigned int counter = 0;

uint32_t source[4];
uint32_t dest[4];


int main_Z4_2(void)
{
	ticks_1us = 0;
	
	swt_set_service_mode(2, SMD_KEYED_SERVICE_SEQUENCE); // SWT_2: select keyed service sequence for SWT_2
	swt_set_service_key(2, 1234);                        // SWT_2: set SWT_2 service key
	swt_set_timeout(2, 50000);                           // SWT_2: set a timeout of 3.125 ms for SWT_2
	swt_enable(2);                                       // SWT_2: enable SWT_2
	
	
	stm_set_channel_irq_priority(2, 0, 1);          // STM_2: set STM_2 channel #0 IRQ priority level to 1
	stm_select_channel_irq_for_processor(2, 0, 2);  // STM_2: select STM_2 channel #0 IRQ for processor #2

	pit_set_timer_irq_priority(0, 0, 2);            // PIT_0: set PIT_0 timer #0 IRQ priority level to 2
	pit_select_timer_irq_for_processor(0, 0, 2);    // PIT_0: select PIT_0 timer #0 IRQ for Processor #2
	
	linflexd_set_irq_priority(0, LINFlexD_INT_RX, 4);         // LINFlexD_0: set LINFlexD_0 INT_RX priority level to 4
	linflexd_select_irq_for_processor(0, LINFlexD_INT_RX, 2); // LINFlexD_0: select LINFlexD_0 INT_RX for Processor #2
	linflexd_set_irq_priority(0, LINFlexD_INT_TX, 5);         // LINFlexD_0: set LINFlexD_0 INT_TX priority level to 5
	linflexd_select_irq_for_processor(0, LINFlexD_INT_TX, 2); // LINFlexD_0: select LINFlexD_0 INT_TX for Processor #2
	linflexd_set_irq_priority(0, LINFlexD_INT_ERR, 3);         // LINFlexD_0: set LINFlexD_0 INT_ERR priority level to 3
	linflexd_select_irq_for_processor(0, LINFlexD_INT_ERR, 2); // LINFlexD_0: select LINFlexD_0 INT_ERR for Processor #2
	
	// trigger periodic_task every 200 us (with a 50 Mhz clock)
	stm_set_interrupt_handler(2, 0, periodic_task); // STM_2: install a hook for STM_2 channel #0 interrupts
	stm_enable_counter(2);                          // STM_2: enable STM_2 counter
	stm_set_channel_compare(2, 0, 10000);           // STM_2: set STM_2 channel #0 compare value to 200 us (i.e. 10000 cycles at 50 MHz)
	stm_enable_channel(2, 0);                       // STM_2: enable STM_2 channel #0
	
	// trigger periodic_task2 every 300 us (with a 80 MHz clock)
	pit_set_timer_interrupt_handler(0, 0, periodic_task2); // PIT_0: install a hook for PIT_0 timer #0 interrupts
	pit_set_timer_load_value(0, 0, 24000 - 1);             // PIT_0: set PIT_0 timer #0 down counter to zero every 300 us (24000 cycles at 80 MHz)
	pit_enable_timer(0, 0);                                // PIT_0: enable timer #0
	pit_enable_timer_interrupt(0, 0);                      // PIT_0: enable interrupts for timer #0
	pit_enable_timers_clock(0);                            // PIT_0: enable PIT_0 timers clock
	
	// lifetime
	pit_set_timer_load_value(1, 0, 80 - 1);      // PIT_1: timer #0 down counter to zero every 1 us period (i.e. 80 cycles at 80 MHz)
	pit_set_timer_load_value(1, 1, 0xffffffff);  // PIT_1: timer #1 down counter by 1 every 1 us
	pit_chain_timer(1, 1);                       // PIT_1: chain timer #0 and timer #1
	pit_enable_timer(1, 0);                      // PIT_1: enable timer #0
	pit_enable_timer(1, 1);                      // PIT_1: enable timer #1
	pit_enable_timers_clock(1);                  // PIT_1: enable PIT_1 timers clock
	
	// install LINFlexD_0 interrupt handlers
	linflexd_set_interrupt_handler(0, LINFlexD_INT_RX, &linflexd_0_int_rx);
	linflexd_set_interrupt_handler(0, LINFlexD_INT_TX, &linflexd_0_int_tx);
	linflexd_set_interrupt_handler(0, LINFlexD_INT_ERR, &linflexd_0_int_err);
	
	// configure LINFlexD_0
	linflexd_request_to_enter_init_mode(0);                      // LINFlexD_0  (sleep mode): request enter init mode
	linflexd_request_to_exit_sleep_mode(0);                      // LINFlexD_0  (sleep mode): request leaving sleep mode
	linflexd_enable_uart_mode(0);                                // LINFlexD_0   (init mode): enable UART mode
	linflexd_enable_uart_tx(0);                                  // LINFlexD_0   (init mode): enable UART Tx
	linflexd_enable_uart_rx(0);                                  // LINFlexD_0   (init mode): enable UART Rx
	
	linflexd_receive_msb_first(0);                               // LINFlexD_0   (init mode): receive MSB first
	linflexd_transmit_msb_first(0);                              // LINFlexD_0   (init mode): transmit MSB first
	
	// set baud rate to 10000 bauds (assuming LIN_CLK = 80 Mhz)
	// ROSE=0: baud rate = LIN_CLK / ( 16 * LDIV)
	// ROSE=1: baud rate = LIN_CLK / (OSR * LDIV)
	linflexd_disable_uart_reduced_over_sampling(0);              // LINFlexD_0   (init mode): disable UART reduced over sampling (ROSE=0)
	linflexd_set_lin_clock_divisor(0, 500, 0);                   // LINFlexD_0   (init mode): set LDIV (LDIV=500+0/16=500.0)
	                                                             
	linflexd_select_tx_stop_bits(0, 1);                          // LINFlexD_0   (init mode): 1 stop bit for transmission
	linflexd_select_uart_rx_stop_bits(0, 1);                     // LINFlexD_0   (init mode): 1 stop bit for reception
	
#if UART_FIFO_MODE
	linflexd_select_uart_tx_fifo_mode(0);                        // LINFlexD_0   (init mode): select UART Tx FIFO mode
#else
	linflexd_select_uart_tx_buffer_mode(0);                      // LINFlexD_0   (init mode): select UART Tx buffer mode
	linflexd_set_uart_tx_buffer_length(0, 1);                    // LINFlexD_0   (init mode): set UART Tx buffer length to 1 byte
#endif
	                                                             
#if UART_FIFO_MODE
	linflexd_select_uart_rx_fifo_mode(0);                        // LINFlexD_0   (init mode): select UART Rx FIFO
#else
	linflexd_select_uart_rx_buffer_mode(0);                      // LINFlexD_0   (init mode): select UART Rx buffer mode
	linflexd_set_uart_rx_buffer_length(0, 1);                    // LINFlexD_0   (init mode): set UART Rx buffer length to 1 byte
#endif
	
#if UART_FIFO_MODE && UART_RX_FIFO_DMA_MODE
	linflexd_enable_dma_rx(0, 0);                                // LINFlexD_0   (init mode): enable DMA Rx (#0 of 1)
	dmamux_set_dma_channel_source(0, 0, 16);                     // DMAMUX_0                : route source #16 (LINFlexD_0 DMA Rx) to channel #0
	dmamux_enable_dma_channel(0, 0);                             // DMAMUX_0                : enable DMA channel #0
	edma_enable_request(0, 0);                                   // EDMA_0                  : enable DMA request #0 (DMAMUX_0 channel #0)
#endif
	
	linflexd_select_uart_word_length(0, LINFlexD_WORD_LENGTH_8); // LINFlexD_0   (init mode): 8-bit data word length without parity
	
	linflexd_enable_data_received_interrupt(0);                  // LINFlexD_0   (init mode): enable data received interrupt (caught by linflexd_0_int_rx)
	linflexd_enable_data_transmitted_interrupt(0);               // LINFlexD_0   (init mode): enable data transmitted interrupt (caught by linflexd_0_int_tx)
	
	linflexd_request_to_exit_init_mode(0);                       // LINFlexD_0   (init mode): request leaving init mode (i.e. entering normal mode)

	unsigned int grp;
	for(grp = 0; grp < 4; grp++)
	{
		edma_set_channel_group_priority(0, grp, 3 - grp);        // eDMA0                   : set channel group priority (higher to lower)
	}
	unsigned int chan;
	for(chan = 0; chan < 64; chan++)
	{
		edma_set_channel_arbitration_priority(0, chan, 15 - (chan & 15)); // eDMA0          : set channel priority in group (higher to lower)
	}

#if UART_FIFO_MODE
#if UART_RX_FIFO_DMA_MODE
	edma_set_tcd_starting_major_iteration_count(0, 0, 1);                                      // eDMA_0: channel #0, BITER=1
	edma_set_tcd_current_major_iteration_count(0, 0, 1);                                       // eDMA_0: channel #0, CITER=1
	edma_set_tcd_minor_byte_count(0, 0, 1);                                                    // eDMA_0: channel #0, NBYTES=1
	edma_set_tcd_source_address(0, 0, (uint32_t)((uint8_t *) &LINFlexD_0.BDRM.R + 3));         // eDMA_0: channel #0, SOURCE ADDR=@LINFlexD_0 Rx FIFO
	edma_set_tcd_signed_source_address_offset(0, 0, 1);                                        // eDMA_0: channel #0, SOURCE OFS=+1
	edma_set_tcd_source_data_transfer_size(0, 0, 1);                                           // eDMA_0: channel #0, SOURCE TRANSFER SIZE=1 byte
	edma_set_tcd_last_source_address_adjustment(0, 0, -1);                                     // eDMA_0: channel #0, LAST SOURCE ADDR ADJUSTMENT=-1
	edma_set_tcd_destination_address(0, 0, (uint32_t)((uint8_t *) &LINFlexD_0.BDRL.R + 3));    // eDMA_0: channel #0, DEST ADDR=@LINFlexD_0 Tx FIFO
	edma_set_tcd_signed_destination_address_offset(0, 0, 1);                                   // eDMA_0: channel #0, DEST OFS=+1
	edma_set_tcd_destination_data_transfer_size(0, 0, 1);                                      // eDMA_0: channel #0, DEST TRANSFER SIZE=1 byte
	edma_set_tcd_last_destination_address_adjustment(0, 0, -1);                                // eDMA_0: channel #0, LAST DEST ADDR ADJUSTMENT=-1
#else
	char msg[4] = "helo";
	linflexd_uart_tx_buffer_write_byte(0, &msg[0], 0);
	linflexd_uart_tx_buffer_write_byte(0, &msg[1], 0);
	linflexd_uart_tx_buffer_write_byte(0, &msg[2], 0);
	linflexd_uart_tx_buffer_write_byte(0, &msg[3], 0);
	
	while(1)
	{
		if(linflexd_is_uart_rx_fifo_not_empty(0))
		{
			char ch = 0;
			linflexd_uart_rx_buffer_read_byte(0, &ch, 0);
			
			while(linflexd_is_uart_tx_fifo_full(0));
			
			linflexd_uart_tx_buffer_write_byte(0, &ch, 0);
		}
	}
#endif // UART_RX_FIFO_DMA_MODE
#endif // UART_FIFO_MODE
	
#if 0
	source[0] = 0x1234;
	source[1] = 0x5678;
	source[2] = 0x4321;
	source[3] = 0x8765;
	dest[0] = 0;
	dest[1] = 0;
	dest[2] = 0;
	dest[3] = 0;
	unsigned int grp;
	for(grp = 0; grp < 4; grp++)
	{
		edma_set_channel_group_priority(0, grp, 3 - grp);
	}
	unsigned int chan;
	for(chan = 0; chan < 64; chan++)
	{
		edma_set_channel_arbitration_priority(0, chan, 15 - (chan & 15));
	}
	
	edma_set_tcd_starting_major_iteration_count(0, 0, 1);
	edma_set_tcd_current_major_iteration_count(0, 0, 1);
	edma_set_tcd_minor_byte_count(0, 0, 16);
	edma_set_tcd_source_address(0, 0, (uint32_t) source);
	edma_set_tcd_signed_source_address_offset(0, 0, 1);
	edma_set_tcd_source_data_transfer_size(0, 0, 1);
	edma_set_tcd_last_source_address_adjustment(0, 0, -16);
	edma_set_tcd_destination_address(0, 0, (uint32_t) dest);
	edma_set_tcd_signed_destination_address_offset(0, 0, 4);
	edma_set_tcd_destination_data_transfer_size(0, 0, 4);
	edma_set_tcd_last_destination_address_adjustment(0, 0, -16);
	edma_enable_tcd_major_complete_interrupt(0, 0);
	edma_set_start_bit(0, 0);
#endif
	
	/* Loop forever */
	for(;;)
	{   
		counter++;
		PPC_WAIT;
	}
}
