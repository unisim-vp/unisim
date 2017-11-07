/*
 *  Copyright (c) 2017,
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

#include "linflexd.h"
#include "intc.h"
#include "MPC5777M.h"

static void __linflexd_0_int_rx__();
static void __linflexd_0_int_tx__();
static void __linflexd_0_int_err__();
static void __linflexd_1_int_rx__();
static void __linflexd_1_int_tx__();
static void __linflexd_1_int_err__();
static void __linflexd_2_int_rx__();
static void __linflexd_2_int_tx__();
static void __linflexd_2_int_err__();
static void __linflexd_14_int_rx__();
static void __linflexd_14_int_tx__();
static void __linflexd_14_int_err__();
static void __linflexd_15_int_rx__();
static void __linflexd_15_int_tx__();
static void __linflexd_15_int_err__();
static void __linflexd_16_int_rx__();
static void __linflexd_16_int_tx__();
static void __linflexd_16_int_err__();

void linflexd_int_handler_default(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int)
{
}

static volatile struct LINFlexD_tag *linflexd[17];
static linflexd_int_handler_t linflexd_int_handlers[17][3];
static unsigned int linflexd_irq_vector_table[17][3];

static unsigned int linflexd_get_irq_vector(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int)
{
	return linflexd_irq_vector_table[linflexd_id][linflexd_int];
}

void linflexd_drv_init()
{
	linflexd[0] = &LINFlexD_0;
	linflexd[1] = &LINFlexD_1;
	linflexd[2] = &LINFlexD_2;
	linflexd[3] = 0;
	linflexd[4] = 0;
	linflexd[5] = 0;
	linflexd[6] = 0;
	linflexd[7] = 0;
	linflexd[8] = 0;
	linflexd[9] = 0;
	linflexd[10] = 0;
	linflexd[11] = 0;
	linflexd[12] = 0;
	linflexd[13] = 0;
	linflexd[14] = &LINFlexD_14;
	linflexd[15] = &LINFlexD_15;
	linflexd[16] = &LINFlexD_16;
	unsigned int linflexd_id;
	for(linflexd_id = 0; linflexd_id < 17; linflexd_id++)
	{
		linflexd_int_handlers[linflexd_id][LINFlexD_INT_RX] = linflexd_int_handler_default;
		linflexd_int_handlers[linflexd_id][LINFlexD_INT_TX] = linflexd_int_handler_default;
		linflexd_int_handlers[linflexd_id][LINFlexD_INT_ERR] = linflexd_int_handler_default;
	}
	linflexd_irq_vector_table[0][LINFlexD_INT_RX]   = 376;
	linflexd_irq_vector_table[0][LINFlexD_INT_TX]   = 377;
	linflexd_irq_vector_table[0][LINFlexD_INT_ERR]  = 378;
	linflexd_irq_vector_table[1][LINFlexD_INT_RX]   = 380;
	linflexd_irq_vector_table[1][LINFlexD_INT_TX]   = 381;
	linflexd_irq_vector_table[1][LINFlexD_INT_ERR]  = 382;
	linflexd_irq_vector_table[2][LINFlexD_INT_RX]   = 384;
	linflexd_irq_vector_table[2][LINFlexD_INT_TX]   = 385;
	linflexd_irq_vector_table[2][LINFlexD_INT_ERR]  = 386;
	linflexd_irq_vector_table[3][LINFlexD_INT_RX]   = 0;
	linflexd_irq_vector_table[3][LINFlexD_INT_TX]   = 0;
	linflexd_irq_vector_table[3][LINFlexD_INT_ERR]  = 0;
	linflexd_irq_vector_table[4][LINFlexD_INT_RX]   = 0;
	linflexd_irq_vector_table[4][LINFlexD_INT_TX]   = 0;
	linflexd_irq_vector_table[4][LINFlexD_INT_ERR]  = 0;
	linflexd_irq_vector_table[5][LINFlexD_INT_RX]   = 0;
	linflexd_irq_vector_table[5][LINFlexD_INT_TX]   = 0;
	linflexd_irq_vector_table[5][LINFlexD_INT_ERR]  = 0;
	linflexd_irq_vector_table[6][LINFlexD_INT_RX]   = 0;
	linflexd_irq_vector_table[6][LINFlexD_INT_TX]   = 0;
	linflexd_irq_vector_table[6][LINFlexD_INT_ERR]  = 0;
	linflexd_irq_vector_table[7][LINFlexD_INT_RX]   = 0;
	linflexd_irq_vector_table[7][LINFlexD_INT_TX]   = 0;
	linflexd_irq_vector_table[7][LINFlexD_INT_ERR]  = 0;
	linflexd_irq_vector_table[8][LINFlexD_INT_RX]   = 0;
	linflexd_irq_vector_table[8][LINFlexD_INT_TX]   = 0;
	linflexd_irq_vector_table[8][LINFlexD_INT_ERR]  = 0;
	linflexd_irq_vector_table[9][LINFlexD_INT_RX]   = 0;
	linflexd_irq_vector_table[9][LINFlexD_INT_TX]   = 0;
	linflexd_irq_vector_table[9][LINFlexD_INT_ERR]  = 0;
	linflexd_irq_vector_table[10][LINFlexD_INT_RX]  = 0;
	linflexd_irq_vector_table[10][LINFlexD_INT_TX]  = 0;
	linflexd_irq_vector_table[10][LINFlexD_INT_ERR] = 0;
	linflexd_irq_vector_table[11][LINFlexD_INT_RX]  = 0;
	linflexd_irq_vector_table[11][LINFlexD_INT_TX]  = 0;
	linflexd_irq_vector_table[11][LINFlexD_INT_ERR] = 0;
	linflexd_irq_vector_table[12][LINFlexD_INT_RX]  = 0;
	linflexd_irq_vector_table[12][LINFlexD_INT_TX]  = 0;
	linflexd_irq_vector_table[12][LINFlexD_INT_ERR] = 0;
	linflexd_irq_vector_table[13][LINFlexD_INT_RX]  = 0;
	linflexd_irq_vector_table[13][LINFlexD_INT_TX]  = 0;
	linflexd_irq_vector_table[13][LINFlexD_INT_ERR] = 0;
	linflexd_irq_vector_table[14][LINFlexD_INT_RX]  = 432;
	linflexd_irq_vector_table[14][LINFlexD_INT_TX]  = 433;
	linflexd_irq_vector_table[14][LINFlexD_INT_ERR] = 434;
	linflexd_irq_vector_table[15][LINFlexD_INT_RX]  = 436;
	linflexd_irq_vector_table[15][LINFlexD_INT_TX]  = 437;
	linflexd_irq_vector_table[15][LINFlexD_INT_ERR] = 438;
	linflexd_irq_vector_table[16][LINFlexD_INT_RX]  = 416;
	linflexd_irq_vector_table[16][LINFlexD_INT_TX]  = 417;
	linflexd_irq_vector_table[16][LINFlexD_INT_ERR] = 418;
}

void linflexd_init(unsigned int linflexd_id)
{
	switch(linflexd_id)
	{
		case 0:
			intc_set_interrupt_handler(linflexd_get_irq_vector(0, LINFlexD_INT_RX), &__linflexd_0_int_rx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(0, LINFlexD_INT_TX), &__linflexd_0_int_tx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(0, LINFlexD_INT_ERR), &__linflexd_0_int_err__);
			break;
		case 1:
			intc_set_interrupt_handler(linflexd_get_irq_vector(1, LINFlexD_INT_RX), &__linflexd_1_int_rx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(1, LINFlexD_INT_TX), &__linflexd_1_int_tx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(1, LINFlexD_INT_ERR), &__linflexd_1_int_err__);
			break;
		case 2:
			intc_set_interrupt_handler(linflexd_get_irq_vector(2, LINFlexD_INT_RX), &__linflexd_2_int_rx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(2, LINFlexD_INT_TX), &__linflexd_2_int_tx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(2, LINFlexD_INT_ERR), &__linflexd_2_int_err__);
			break;
		case 14:
			intc_set_interrupt_handler(linflexd_get_irq_vector(14, LINFlexD_INT_RX), &__linflexd_14_int_rx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(14, LINFlexD_INT_TX), &__linflexd_14_int_tx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(14, LINFlexD_INT_ERR), &__linflexd_14_int_err__);
			break;
		case 15:
			intc_set_interrupt_handler(linflexd_get_irq_vector(15, LINFlexD_INT_RX), &__linflexd_15_int_rx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(15, LINFlexD_INT_TX), &__linflexd_15_int_tx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(15, LINFlexD_INT_ERR), &__linflexd_15_int_err__);
			break;
		case 16:
			intc_set_interrupt_handler(linflexd_get_irq_vector(16, LINFlexD_INT_RX), &__linflexd_16_int_rx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(16, LINFlexD_INT_TX), &__linflexd_16_int_tx__);
			intc_set_interrupt_handler(linflexd_get_irq_vector(16, LINFlexD_INT_ERR), &__linflexd_16_int_err__);
			break;
	}
}

void linflexd_request_to_enter_init_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.INIT = 1;
}

void linflexd_request_to_exit_init_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.INIT = 0;
}

void linflexd_request_to_enter_sleep_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.SLEEP = 1;
}

void linflexd_request_to_exit_sleep_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.SLEEP = 0;
}

void linflexd_enable_auto_wakeup(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.AUTOWU = 1;
}

void linflexd_disable_auto_wakeup(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.AUTOWU = 0;
}

void linflexd_enable_loop_back_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.LBKM = 1;
}

void linflexd_disable_loop_back_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.LBKM = 0;
}

void linflexd_enable_receiver_buffer_locked_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.RBLM = 1;
}

void linflexd_disable_receiver_buffer_locked_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINCR1.B.RBLM = 0;
}

void linflexd_set_lin_clock_divisor(unsigned int linflexd_id, unsigned int int_part, unsigned int frac_part)
{
	if((int_part > 1048575) || (frac_part > 15)) return;
	linflexd[linflexd_id]->LINIBRR.B.IBR = int_part;
	linflexd[linflexd_id]->LINFBRR.B.FBR = frac_part;
}

void linflexd_transmit_msb_first(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.TDFBM = 1;
}

void linflexd_transmit_lsb_first(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.TDFBM = 0;
}

void linflexd_receive_msb_first(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.RDFBM = 1;
}

void linflexd_receive_lsb_first(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.RDFBM = 0;
}

void linflexd_enable_inverted_data_transmission(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.TDLIS = 1;
}

void linflexd_disable_inverted_data_transmission(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.TDLIS = 0;
}

void linflexd_enable_inverted_data_reception(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.RDLIS = 1;
}

void linflexd_disable_inverted_data_reception(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.RDLIS = 0;
}

void linflexd_select_tx_stop_bits(unsigned int linflexd_id, unsigned int stop_bits)
{
	if((stop_bits != 1) && (stop_bits != 2)) return;
	linflexd[linflexd_id]->GCR.B.STOP = stop_bits - 1;
}

void linflexd_soft_reset(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->GCR.B.SR = 1;
	linflexd[linflexd_id]->GCR.B.SR = 0;
}

void linflexd_enable_uart_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.UART = 1;
}

void linflexd_disable_uart_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.UART = 0;
}

void linflexd_enable_uart_rx(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.RxEn = 1;
}

void linflexd_disable_uart_rx(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.RxEn = 0;
}

void linflexd_enable_uart_tx(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.TxEn = 1;
}

void linflexd_disable_uart_tx(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.TxEn = 0;
}

void linflexd_set_uart_over_sampling_rate(unsigned int linflexd_id, unsigned int v)
{
	if((v != 4) && (v != 5) && (v != 6) && (v != 8)) return;
	linflexd[linflexd_id]->UARTCR.B.OSR = v;
}

void linflexd_enable_uart_reduced_over_sampling(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.ROSE = 1;
}

void linflexd_disable_uart_reduced_over_sampling(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.ROSE = 0;
}

void linflexd_set_uart_number_of_expected_frame(unsigned int linflexd_id, unsigned int v)
{
	if(v > 7) return;
	linflexd[linflexd_id]->UARTCR.B.NEF = v;
}

void linflexd_disable_uart_timeout(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.DTU_PCETX = 1;
}

void linflexd_enable_uart_timeout(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.DTU_PCETX = 0;
}

void linflexd_select_uart_rx_stop_bits(unsigned int linflexd_id, unsigned int stop_bits)
{
	if(!stop_bits || (stop_bits > 3)) return;
	linflexd[linflexd_id]->UARTCR.B.SBUR = stop_bits - 1;
}

void linflexd_select_uart_rx_buffer_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.RFBM = 0;
}

void linflexd_select_uart_rx_fifo_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.RFBM = 1;
}

void linflexd_select_uart_tx_buffer_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.TFBM = 0;
}

void linflexd_select_uart_tx_fifo_mode(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.TFBM = 1;
}

void linflexd_set_uart_rx_buffer_length(unsigned int linflexd_id, unsigned int length)
{
	if(!length || (length > 4)) return;
	linflexd[linflexd_id]->UARTCR.B.RDFL_RFC = length - 1;
}

unsigned int linflexd_get_uart_rx_buffer_length(unsigned int linflexd_id)
{
	return (linflexd[linflexd_id]->UARTCR.B.RDFL_RFC & 3) + 1;
}

unsigned int linflexd_get_uart_rx_fifo_counter(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTCR.B.RDFL_RFC;
}

unsigned int linflexd_is_uart_rx_fifo_empty(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.DRFRFE;
}

void linflexd_set_uart_tx_buffer_length(unsigned int linflexd_id, unsigned int length)
{
	if(!length || (length > 4)) return;
	linflexd[linflexd_id]->UARTCR.B.TDFL_TFC = length - 1;
}

unsigned int linflexd_get_uart_tx_buffer_length(unsigned int linflexd_id)
{
	return (linflexd[linflexd_id]->UARTCR.B.TDFL_TFC & 3) + 1;
}

unsigned int linflexd_get_uart_tx_fifo_counter(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTCR.B.TDFL_TFC;
}

unsigned int linflexd_is_uart_tx_fifo_full(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.DTFTFF;
}

void linflexd_enable_uart_special_word_length(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.WLS = 1;
}

void linflexd_disable_uart_special_word_length(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.WLS = 0;
}

void linflexd_enable_uart_parity_control(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.PCE = 1;
}

void linflexd_disable_uart_parity_control(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->UARTCR.B.PCE = 0;
}

void linflexd_select_uart_parity_control(unsigned int linflexd_id, enum LINFlexD_PARITY_CONTROL linflexd_parity_control)
{
	linflexd[linflexd_id]->UARTCR.B.PC1 = (linflexd_parity_control >> 1) & 1;
	linflexd[linflexd_id]->UARTCR.B.PC0 = linflexd_parity_control & 1;
}

void linflexd_select_uart_word_length(unsigned int linflexd_id, enum LINFlexD_WORD_LENGTH wl)
{
	linflexd[linflexd_id]->UARTCR.B.WL1 = (wl >> 1) & 1;
	linflexd[linflexd_id]->UARTCR.B.WL0 = wl & 1;
}

void linflexd_uart_rx_buffer_read_byte(unsigned int linflexd_id, uint8_t *data_ptr, unsigned int byte_index)
{
	*data_ptr = *((volatile uint8_t *) &linflexd[linflexd_id]->BDRM.R + 3 - byte_index);
}

void linflexd_uart_rx_buffer_read_half_word(unsigned int linflexd_id, uint16_t *data_ptr, unsigned int half_word_index)
{
	*data_ptr = *((volatile uint16_t *) &linflexd[linflexd_id]->BDRM.R + 1 - half_word_index);
}

void linflexd_uart_rx_buffer_read_word(unsigned int linflexd_id, uint32_t *data_ptr)
{
	*data_ptr = linflexd[linflexd_id]->BDRM.R;
}

void linflexd_uart_rx_fifo_read_byte(unsigned int linflexd_id, uint8_t *data_ptr)
{
	*data_ptr = *((volatile uint8_t *) &linflexd[linflexd_id]->BDRM.R + 3); // DATA4
}

void linflexd_uart_rx_fifo_read_half_word(unsigned int linflexd_id, uint16_t *data_ptr)
{
	*data_ptr = *((volatile uint16_t *) &linflexd[linflexd_id]->BDRM.R + 1); // DATA5 DATA4
}

void linflexd_uart_tx_buffer_write_byte(unsigned int linflexd_id, const uint8_t *data_ptr, unsigned int byte_index)
{
	*((volatile uint8_t *) &linflexd[linflexd_id]->BDRL.R + 3 - byte_index) = *data_ptr;
}

void linflexd_uart_tx_buffer_write_half_word(unsigned int linflexd_id, const uint16_t *data_ptr, unsigned int half_word_index)
{
	*((volatile uint16_t *) &linflexd[linflexd_id]->BDRL.R + 1 - half_word_index) = *data_ptr;
}

void linflexd_uart_tx_buffer_write_word(unsigned int linflexd_id, const uint32_t *data_ptr)
{
	linflexd[linflexd_id]->BDRL.R = *data_ptr;
}

void linflexd_uart_tx_fifo_write_byte(unsigned int linflexd_id, const uint8_t *data_ptr)
{
	*((volatile uint8_t *) &linflexd[linflexd_id]->BDRL.R + 3) = *data_ptr; // DATA0
}

void linflexd_uart_tx_fifo_write_half_word(unsigned int linflexd_id, const uint16_t *data_ptr)
{
	*((volatile uint16_t *) &linflexd[linflexd_id]->BDRL.R + 1) = *data_ptr; // DATA1 DATA0
}

unsigned int linflexd_get_uart_byte_parity_error(unsigned int linflexd_id, unsigned int byte_index)
{
	return (linflexd[linflexd_id]->UARTSR.B.PE >> byte_index) & 1;
}

unsigned int linflexd_get_uart_half_word_parity_error(unsigned int linflexd_id, unsigned int half_word_index)
{
	return (linflexd[linflexd_id]->UARTSR.B.PE >> (2 * half_word_index)) & 1;
}

unsigned int linflexd_is_uart_rx_buffer_ready(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.RMB;
}

void linflexd_release_uart_rx_buffer(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.RMB = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

unsigned int linflexd_get_uart_receiver_data_input_signal(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.RDI;
}

unsigned int linflexd_is_uart_rx_fifo_not_empty(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.RFNE;
}

linflexd_int_handler_t linflexd_set_interrupt_handler(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, linflexd_int_handler_t linflexd_int_handler)
{
	linflexd_int_handler_t old_linflexd_int_handler = linflexd_int_handlers[linflexd_id][linflexd_int];
	linflexd_int_handlers[linflexd_id][linflexd_int] = linflexd_int_handler;
	return old_linflexd_int_handler;
}

void linflexd_set_irq_priority(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, unsigned int priority)
{
	intc_set_irq_priority(linflexd_get_irq_vector(linflexd_id, linflexd_int), priority);
}

void linflexd_select_irq_for_processor(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, unsigned int prc_num)
{
	intc_select_irq_for_processor(linflexd_get_irq_vector(linflexd_id, linflexd_int), prc_num);
}

void linflexd_deselect_irq_for_processor(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, unsigned int prc_num)
{
	intc_deselect_irq_for_processor(linflexd_get_irq_vector(linflexd_id, linflexd_int), prc_num);
}

void linflexd_enable_stuck_at_zero_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.SZIE = 1;
}

void linflexd_disable_stuck_at_zero_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.SZIE = 0;
}

unsigned int linflexd_get_uart_stuck_at_zero_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.SZF;
}

void linflexd_clear_uart_stuck_at_zero_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.SZF = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

void linflexd_enable_output_compare_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.OCIE = 1;
}

void linflexd_disable_output_compare_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.OCIE = 0;
}

unsigned int linflexd_get_uart_output_compare_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.OCF;
}

void linflexd_clear_uart_output_compare_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.OCF = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

void linflexd_enable_frame_error_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.FEIE = 1;
}

void linflexd_disable_frame_error_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.FEIE = 0;
}

unsigned int linflexd_get_uart_frame_error_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.FEF;
}

void linflexd_clear_uart_frame_error_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.FEF = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

void linflexd_enable_buffer_overrun_error_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.BOIE = 1;
}

void linflexd_disable_buffer_overrun_error_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.BOIE = 0;
}

unsigned int linflexd_get_uart_buffer_overrun_error_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.BOF;
}

void linflexd_clear_uart_buffer_overrun_error_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.BOF = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

void linflexd_enable_wakeup_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.WUIE = 1;
}

void linflexd_disable_wakeup_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.WUIE = 0;
}

unsigned int linflexd_get_uart_wakeup_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.WUF;
}

void linflexd_clear_uart_wakeup_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.WUF = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

void linflexd_enable_uart_timeout_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.DBEIETOIE = 1;
}

void linflexd_disable_uart_timeout_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.DBEIETOIE = 0;
}

unsigned int linflexd_get_uart_timeout_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.TO;
}

void linflexd_clear_uart_timeout_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.TO = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

void linflexd_enable_data_received_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.DRIE = 1;
}

void linflexd_disable_data_received_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.DRIE = 0;
}

unsigned int linflexd_get_uart_data_received_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.DRFRFE;
}

void linflexd_clear_uart_data_received_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.DRFRFE = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}

void linflexd_enable_data_transmitted_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.DTIE = 1;
}

void linflexd_disable_data_transmitted_interrupt(unsigned int linflexd_id)
{
	linflexd[linflexd_id]->LINIER.B.DTIE = 0;
}

unsigned int linflexd_get_uart_data_transmitted_interrupt_flag(unsigned int linflexd_id)
{
	return linflexd[linflexd_id]->UARTSR.B.DTFTFF;
}

void linflexd_clear_uart_data_transmitted_interrupt_flag(unsigned int linflexd_id)
{
	LINFlexD_UARTSR_tag uartsr;
	uartsr.R = 0;
	uartsr.B.DTFTFF = 1;
	linflexd[linflexd_id]->UARTSR = uartsr;
}


static void __linflexd_0_int_rx__()
{
	linflexd_int_handlers[0][LINFlexD_INT_RX](0, LINFlexD_INT_RX);
}

static void __linflexd_0_int_tx__()
{
	linflexd_int_handlers[0][LINFlexD_INT_TX](0, LINFlexD_INT_TX);
}

static void __linflexd_0_int_err__()
{
	linflexd_int_handlers[0][LINFlexD_INT_ERR](0, LINFlexD_INT_ERR);
}

static void __linflexd_1_int_rx__()
{
	linflexd_int_handlers[1][LINFlexD_INT_RX](1, LINFlexD_INT_RX);
}

static void __linflexd_1_int_tx__()
{
	linflexd_int_handlers[1][LINFlexD_INT_TX](1, LINFlexD_INT_TX);
}

static void __linflexd_1_int_err__()
{
	linflexd_int_handlers[1][LINFlexD_INT_ERR](1, LINFlexD_INT_ERR);
}

static void __linflexd_2_int_rx__()
{
	linflexd_int_handlers[2][LINFlexD_INT_RX](2, LINFlexD_INT_RX);
}

static void __linflexd_2_int_tx__()
{
	linflexd_int_handlers[2][LINFlexD_INT_TX](2, LINFlexD_INT_TX);
}

static void __linflexd_2_int_err__()
{
	linflexd_int_handlers[2][LINFlexD_INT_ERR](2, LINFlexD_INT_ERR);
}

static void __linflexd_14_int_rx__()
{
	linflexd_int_handlers[14][LINFlexD_INT_RX](14, LINFlexD_INT_RX);
}

static void __linflexd_14_int_tx__()
{
	linflexd_int_handlers[14][LINFlexD_INT_TX](14, LINFlexD_INT_TX);
}

static void __linflexd_14_int_err__()
{
	linflexd_int_handlers[14][LINFlexD_INT_ERR](14, LINFlexD_INT_ERR);
}

static void __linflexd_15_int_rx__()
{
	linflexd_int_handlers[15][LINFlexD_INT_RX](15, LINFlexD_INT_RX);
}

static void __linflexd_15_int_tx__()
{
	linflexd_int_handlers[15][LINFlexD_INT_TX](15, LINFlexD_INT_TX);
}

static void __linflexd_15_int_err__()
{
	linflexd_int_handlers[15][LINFlexD_INT_ERR](15, LINFlexD_INT_ERR);
}

static void __linflexd_16_int_rx__()
{
	linflexd_int_handlers[16][LINFlexD_INT_RX](16, LINFlexD_INT_RX);
}

static void __linflexd_16_int_tx__()
{
	linflexd_int_handlers[16][LINFlexD_INT_TX](16, LINFlexD_INT_TX);
}

static void __linflexd_16_int_err__()
{
	linflexd_int_handlers[16][LINFlexD_INT_ERR](16, LINFlexD_INT_ERR);
}
