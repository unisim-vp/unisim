/*
 *  Copyright (c) 2018,
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

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include "linflexd.h"
#include "dmamux.h"
#include "edma.h"
#include "console.h"

static unsigned int con_get_core_id()
{
	return MFSPR(286); // read PIR: our processor number
}

static void con_write_end_of_major_loop_int(unsigned int edma_id, unsigned int chan)
{
	edma_clear_interrupt_request(edma_id, chan);
}

static void con_read_end_of_major_loop_int(unsigned int edma_id, unsigned int chan)
{
	edma_clear_interrupt_request(edma_id, chan);
}

static unsigned int con_to_linflexd_mapping(unsigned int con_id)
{
	switch(con_id)
	{
		case 0: return 0;
		case 1: return 1;
		case 2: return 2;
		case 3: return 14;
		case 4: return 15;
		case 5: return 16;
	}
	return 0;
}

static unsigned int linflexd_to_dmamux_mapping(unsigned int linflexd_id)
{
	switch(linflexd_id)
	{
		case 0: return 0;
		case 1: return 1;
		case 2: return 3;
		case 14: return 4;
		case 15: return 5;
		case 16: return 6;
	}
	return 0;
}

static unsigned int dmamux_to_edma_mapping(unsigned int dmamux_id)
{
	switch(dmamux_id)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			return 0;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			return 1;
	}
	return 0;
}

static unsigned int linflexd_rx_to_dmamux_source_mapping(unsigned int linflexd_id)
{
	switch(linflexd_id)
	{
		case 0: return 16;
		case 1: return 5;
		case 2: return 4;
		case 14: return 9;
		case 15: return 8;
		case 16: return 16;
	}
	return 0;
}

static unsigned int linflexd_tx_to_dmamux_source_mapping(unsigned int linflexd_id)
{
	switch(linflexd_id)
	{
		case 0: return 17;
		case 1: return 6;
		case 2: return 5;
		case 14: return 10;
		case 15: return 9;
		case 16: return 17;
	}
	return 0;
}

static unsigned int linflexd_rx_to_dmamux_channel_custom_mapping(unsigned int linflexd_id)
{
	// for each DMAMUX, we choose channel #0 for LINFlexD RX
	return 0;
}

static unsigned int linflexd_rx_to_edma_channel_custom_mapping(unsigned int linflexd_id)
{
	switch(linflexd_id)
	{
		case 0: return 0;
		case 1: return 2;
		case 2: return 4;
		case 14: return 6;
		case 15: return 8;
		case 16: return 10;
	}
	return 0;
}

static unsigned int linflexd_tx_to_dmamux_channel_custom_mapping(unsigned int linflexd_id)
{
	// for each DMAMUX, we choose channel #1 for LINFlexD TX
	return 1;
}

static unsigned int linflexd_tx_to_edma_channel_custom_mapping(unsigned int linflexd_id)
{
	switch(linflexd_id)
	{
		case 0: return 1;
		case 1: return 3;
		case 2: return 5;
		case 14: return 7;
		case 15: return 9;
		case 16: return 11;
	}
	return 0;
}

void con_init(struct con_t *con, unsigned int con_id)
{
	unsigned int prc_num = con_get_core_id();
	
	con->con_id = con_id;
	con->linflexd_id = con_to_linflexd_mapping(con_id);
	con->dmamux_id = linflexd_to_dmamux_mapping(con->linflexd_id);
	con->linflexd_rx_dmamux_source = linflexd_rx_to_dmamux_source_mapping(con->linflexd_id);
	con->linflexd_tx_dmamux_source = linflexd_tx_to_dmamux_source_mapping(con->linflexd_id);
	con->edma_id = dmamux_to_edma_mapping(con->dmamux_id);
	con->linflexd_rx_dmamux_channel = linflexd_rx_to_dmamux_channel_custom_mapping(con->linflexd_id);
	con->linflexd_tx_dmamux_channel = linflexd_tx_to_dmamux_channel_custom_mapping(con->linflexd_id);
	con->linflexd_rx_edma_channel = linflexd_rx_to_edma_channel_custom_mapping(con->linflexd_id);
	con->linflexd_tx_edma_channel = linflexd_tx_to_edma_channel_custom_mapping(con->linflexd_id);
	
	// configure LINFlexD
	linflexd_init(con->linflexd_id);                                            // LINFlexD_0: initialize
	linflexd_request_to_enter_init_mode(con->linflexd_id);                      // LINFlexD_0  (sleep mode): request enter init mode
	linflexd_request_to_exit_sleep_mode(con->linflexd_id);                      // LINFlexD_0  (sleep mode): request leaving sleep mode
	linflexd_enable_uart_mode(con->linflexd_id);                                // LINFlexD_0   (init mode): enable UART mode
	linflexd_enable_uart_tx(con->linflexd_id);                                  // LINFlexD_0   (init mode): enable UART Tx
	linflexd_enable_uart_rx(con->linflexd_id);                                  // LINFlexD_0   (init mode): enable UART Rx
	
	linflexd_receive_msb_first(con->linflexd_id);                               // LINFlexD_0   (init mode): receive MSB first
	linflexd_transmit_msb_first(con->linflexd_id);                              // LINFlexD_0   (init mode): transmit MSB first
	
	// set baud rate to 10000 bauds (assuming LIN_CLK = 80 Mhz)
	// ROSE=0: baud rate = LIN_CLK / ( 16 * LDIV)
	// ROSE=1: baud rate = LIN_CLK / (OSR * LDIV)
	linflexd_disable_uart_reduced_over_sampling(con->linflexd_id);              // LINFlexD_0   (init mode): disable UART reduced over sampling (ROSE=0)
	linflexd_set_lin_clock_divisor(con->linflexd_id, 500, 0);                   // LINFlexD_0   (init mode): set LDIV (LDIV=500+0/16=500.0)
	                                                             
	linflexd_select_tx_stop_bits(con->linflexd_id, 1);                          // LINFlexD_0   (init mode): 1 stop bit for transmission
	linflexd_select_uart_rx_stop_bits(con->linflexd_id, 1);                     // LINFlexD_0   (init mode): 1 stop bit for reception
	linflexd_select_uart_tx_fifo_mode(con->linflexd_id);                        // LINFlexD_0   (init mode): select UART Tx FIFO mode
	linflexd_select_uart_rx_fifo_mode(con->linflexd_id);                        // LINFlexD_0   (init mode): select UART Rx FIFO
	linflexd_enable_dma_rx(con->linflexd_id, 0);                                // LINFlexD_0   (init mode): enable DMA Rx (#0 of 1)
	linflexd_enable_dma_tx(con->linflexd_id, 0);                                // LINFlexD_0   (init mode): enable DMA Tx (#0 of 1)
	linflexd_select_uart_word_length(con->linflexd_id, LINFlexD_WORD_LENGTH_8); // LINFlexD_0   (init mode): 8-bit data word length without parity
	
	// configure DMAMUX and eDMA
	edma_set_interrupt_handler(con->edma_id, con->linflexd_rx_edma_channel, con_read_end_of_major_loop_int);  // eDMA_0: channel #0, set INTMAJOR interrupt handler
	edma_enable_tcd_major_complete_interrupt(con->edma_id, con->linflexd_rx_edma_channel);                    // eDMA_0: channel #0, ask for an interrupt when major loop completes
	edma_set_interrupt_handler(con->edma_id, con->linflexd_tx_edma_channel, con_write_end_of_major_loop_int); // eDMA_1: channel #1, set INTMAJOR interrupt handler
	edma_enable_tcd_major_complete_interrupt(con->edma_id, con->linflexd_tx_edma_channel);                    // eDMA_1: channel #1, ask for an interrupt when major loop completes

	dmamux_set_dma_channel_source(con->dmamux_id, con->linflexd_rx_dmamux_channel, con->linflexd_rx_dmamux_source);  // DMAMUX_0                : route source #16 (LINFlexD_0 DMA Rx) to channel #0
	dmamux_enable_dma_channel(con->dmamux_id, con->linflexd_rx_dmamux_channel);                                      // DMAMUX_0                : enable DMA channel #0
	dmamux_set_dma_channel_source(con->dmamux_id, con->linflexd_tx_dmamux_channel, con->linflexd_tx_dmamux_source);  // DMAMUX_0                : route source #17 (LINFlexD_0 DMA Tx) to channel #1
	dmamux_enable_dma_channel(con->dmamux_id, con->linflexd_tx_dmamux_channel);                                      // DMAMUX_0                : enable DMA channel #1
	edma_enable_request(con->edma_id, con->linflexd_rx_edma_channel);                                                // EDMA_0                  : enable DMA request #0 (DMAMUX_0 channel #0)
	
	edma_set_irq_priority(con->edma_id, con->linflexd_rx_edma_channel, 10);              // eDMA_0: channel #0, set IRQ priority to 10
	edma_select_irq_for_processor(con->edma_id, con->linflexd_rx_edma_channel, prc_num); // eDMA_0: channel #0, select IRQ for our processor
	edma_set_irq_priority(con->edma_id, con->linflexd_tx_edma_channel, 10);              // eDMA_0: channel #1, set IRQ priority to 10
	edma_select_irq_for_processor(con->edma_id, con->linflexd_tx_edma_channel, prc_num); // eDMA_0: channel #0, select IRQ for our processor

	
	linflexd_request_to_exit_init_mode(con->linflexd_id);                                                            // LINFlexD_0   (init mode): request leaving init mode (i.e. entering normal mode)
}

int con_write(struct con_t *con, const void *ptr, int len)
{
	unsigned int cnt = len;
	if(cnt >= 32768) cnt = 32768;

	edma_set_tcd_starting_major_iteration_count(con->edma_id, con->linflexd_tx_edma_channel, cnt);                 // eDMA_0: channel #1, BITER=cnt
	edma_set_tcd_current_major_iteration_count(con->edma_id, con->linflexd_tx_edma_channel, cnt);                  // eDMA_0: channel #1, CITER=cnt
	edma_set_tcd_minor_byte_count(con->edma_id, con->linflexd_tx_edma_channel, 1);                                 // eDMA_0: channel #1, NBYTES=1
	edma_set_tcd_source_address(con->edma_id, con->linflexd_tx_edma_channel, (uint32_t) ptr);                      // eDMA_0: channel #1, SOURCE ADDR=ptr
	edma_set_tcd_signed_source_address_offset(con->edma_id, con->linflexd_tx_edma_channel, 1);                     // eDMA_0: channel #1, SOURCE OFS=+1
	edma_set_tcd_source_data_transfer_size(con->edma_id, con->linflexd_tx_edma_channel, 1);                        // eDMA_0: channel #1, SOURCE TRANSFER SIZE=1 byte
	edma_set_tcd_last_source_address_adjustment(con->edma_id, con->linflexd_tx_edma_channel, 0);                   // eDMA_0: channel #1, LAST SOURCE ADDR ADJUSTMENT=0
	edma_set_tcd_destination_address(con->edma_id, con->linflexd_tx_edma_channel,
	                                 (uint32_t) linflexd_uart_tx_fifo_byte_direct_ptr(con->linflexd_id));          // eDMA_0: channel #1, DEST ADDR=@LINFlexD_0 Tx FIFO
	edma_set_tcd_signed_destination_address_offset(con->edma_id, con->linflexd_tx_edma_channel, 0);                // eDMA_0: channel #1, DEST OFS=+0
	edma_set_tcd_destination_data_transfer_size(con->edma_id, con->linflexd_tx_edma_channel, 1);                   // eDMA_0: channel #1, DEST TRANSFER SIZE=1 byte
	edma_set_tcd_last_destination_address_adjustment(con->edma_id, con->linflexd_tx_edma_channel, 0);              // eDMA_0: channel #1, LAST DEST ADDR ADJUSTMENT=0
	edma_tcd_disable_request_automatically_after_major_loop_complete(con->edma_id, con->linflexd_tx_edma_channel); // eDMA_0: channel #1, disable request automatically after major loop complete
	edma_clear_done_bit(con->edma_id, con->linflexd_tx_edma_channel);                                              // eDMA_0: channel #1, clear DONE bit
	edma_clear_error(con->edma_id, con->linflexd_tx_edma_channel);                                                 // eDMA_0: channel #1, clear ERROR indicator
	edma_enable_request(con->edma_id, con->linflexd_tx_edma_channel);                                              // eDMA_0: enable DMA request #1 (DMAMUX_0 channel #1)
	
	do
	{
		PPC_WAIT;
	}
	while(!edma_tcd_channel_done(con->edma_id, con->linflexd_tx_edma_channel) && !edma_error(con->edma_id, con->linflexd_tx_edma_channel)); // eDMA_0: channel #1, loop while !DONE and !ERROR
	
	unsigned int rem = edma_error(con->edma_id, con->linflexd_tx_edma_channel) ? edma_get_tcd_current_major_iteration_count(con->edma_id, con->linflexd_tx_edma_channel) : 0; // eDMA_0: channel #1, get CITER
	
	return cnt - rem;
}

int con_read(struct con_t *con, void *ptr, int len)
{
	unsigned int cnt = len;
	if(cnt >= 32768) cnt = 32768;
	
	edma_set_tcd_starting_major_iteration_count(con->edma_id, con->linflexd_rx_edma_channel, cnt);                 // eDMA_0: channel #0, BITER=cnt
	edma_set_tcd_current_major_iteration_count(con->edma_id, con->linflexd_rx_edma_channel, cnt);                  // eDMA_0: channel #0, CITER=cnt
	edma_set_tcd_minor_byte_count(con->edma_id, con->linflexd_rx_edma_channel, 1);                                 // eDMA_0: channel #0, NBYTES=1
	edma_set_tcd_source_address(con->edma_id, con->linflexd_rx_edma_channel,
	                            (uint32_t) linflexd_uart_rx_fifo_byte_direct_ptr(con->linflexd_id));               // eDMA_0: channel #0, SOURCE ADDR=@LINFlexD_0 Rx FIFO
	edma_set_tcd_signed_source_address_offset(con->edma_id, con->linflexd_rx_edma_channel, 0);                     // eDMA_0: channel #0, SOURCE OFS=+0
	edma_set_tcd_source_data_transfer_size(con->edma_id, con->linflexd_rx_edma_channel, 1);                        // eDMA_0: channel #0, SOURCE TRANSFER SIZE=1 byte
	edma_set_tcd_last_source_address_adjustment(con->edma_id, con->linflexd_rx_edma_channel, 0);                   // eDMA_0: channel #0, LAST SOURCE ADDR ADJUSTMENT=0
	edma_set_tcd_destination_address(con->edma_id, con->linflexd_rx_edma_channel, (uint32_t) ptr);                 // eDMA_0: channel #0, DEST ADDR=ptr
	edma_set_tcd_signed_destination_address_offset(con->edma_id, con->linflexd_rx_edma_channel, 1);                // eDMA_0: channel #0, DEST OFS=+1
	edma_set_tcd_destination_data_transfer_size(con->edma_id, con->linflexd_rx_edma_channel, 1);                   // eDMA_0: channel #0, DEST TRANSFER SIZE=1 byte
	edma_set_tcd_last_destination_address_adjustment(con->edma_id, con->linflexd_rx_edma_channel, 0);              // eDMA_0: channel #0, LAST DEST ADDR ADJUSTMENT=0
	edma_tcd_disable_request_automatically_after_major_loop_complete(con->edma_id, con->linflexd_rx_edma_channel); // eDMA_0: channel #0, disable request automatically after major loop complete
	edma_clear_done_bit(con->edma_id, con->linflexd_rx_edma_channel);                                              // eDMA_0: channel #0, clear DONE bit
	edma_clear_error(con->edma_id, con->linflexd_rx_edma_channel);                                                 // eDMA_0: channel #0, clear ERROR indicator
	edma_enable_request(con->edma_id, con->linflexd_rx_edma_channel);                                              // eDMA_0: enable DMA request #0 (DMAMUX_0 channel #0)
	
	do
	{
		PPC_WAIT;
	}
	while(!edma_tcd_channel_done(con->edma_id, con->linflexd_rx_edma_channel) && !edma_error(con->edma_id, con->linflexd_rx_edma_channel));  // eDMA_0: loop while !DONE and !ERROR
	
	unsigned int rem = edma_error(con->edma_id, con->linflexd_rx_edma_channel) ? edma_get_tcd_current_major_iteration_count(con->edma_id, con->linflexd_tx_dmamux_channel) : 0;  // eDMA_0: channel #0, get CITER
	
	return cnt - rem;
}
