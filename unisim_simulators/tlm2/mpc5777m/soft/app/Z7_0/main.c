/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include <stdio.h>
#include "dspi.h"

volatile int counter = 0;

#define DSPI_INT_MODE 1

#if DSPI_INT_MODE
void dspi_0_tfff_rfdf(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
	static int x = 0;
	
	while(dspi_get_interrupt_request_flag(dspi_id, DSPI_REQ_RFDF))
	{
		dspi_clear_interrupt_request_flag(dspi_id, DSPI_REQ_RFDF);
		volatile uint16_t v = dspi_spi_rx_fifo_pop_half_word(dspi_id);
	}
	while(dspi_get_interrupt_request_flag(dspi_id, DSPI_REQ_TFFF))
	{
		dspi_clear_interrupt_request_flag(dspi_id, DSPI_REQ_TFFF);
		uint16_t v = x ? 0xc0ca : 0xbaba;
		dspi_spi_tx_fifo_push_data_half_word(dspi_id, v);
		x ^= 1;
	}
}
#endif

int main(void)
{
	dspi_init(0);
	dspi_enable_module(0);
	dspi_set_mode(0, DSPI_SLAVE_MODE);
	
	struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES cta0;
	cta0.double_baud_rate               = 0;
	cta0.frame_size                     = 16;
	cta0.clock_polarity                 = DSPI_CLOCK_INACTIVE_LOW;
	cta0.clock_phase                    = DSPI_CLOCK_CAPTURED_LEADING_EDGE;
	cta0.lsb_first                      = 0;
	cta0.pcs_to_sck_delay_prescaler     = 0;
	cta0.after_sck_delay_prescaler      = 0;
	cta0.delay_after_transfer_prescaler = 0;
	cta0.baud_rate_prescaler            = 0;
	cta0.pcs_to_sck_delay_scaler        = 0;
	cta0.after_sck_delay_scaler         = 0;
	cta0.delay_after_transfer_scaler    = 0;
	cta0.baud_rate_scaler               = 0;
	cta0.parity_enable                  = 0;
	cta0.parity_polarity                = 0;

	dspi_set_spi_clock_and_transfer_attributes(0, 0, &cta0);

#if DSPI_INT_MODE
	dspi_set_interrupt_handler(0, DSPI_REQ_TFFF, dspi_0_tfff_rfdf);
	dspi_set_interrupt_handler(0, DSPI_REQ_RFDF, dspi_0_tfff_rfdf);
	dspi_enable_request(0, DSPI_REQ_TFFF);
	dspi_enable_request(0, DSPI_REQ_RFDF);
#endif

	dspi_start_transfers(0);
	
#if !DSPI_INT_MODE
	int x = 0;
	
	do
	{
		if(dspi_get_interrupt_request_flag(0, DSPI_REQ_TFFF))
		{
			dspi_clear_interrupt_request_flag(0, DSPI_REQ_TFFF);
			uint16_t v = x ? 0xc0ca : 0xbaba;
			dspi_spi_tx_fifo_push_data_half_word(0, v);
			//printf("push 0x%x\n", v);
			x ^= 1;
		}
		if(dspi_get_interrupt_request_flag(0, DSPI_REQ_RFDF))
		{
			dspi_clear_interrupt_request_flag(0, DSPI_REQ_RFDF);
			uint16_t v = dspi_spi_rx_fifo_pop_half_word(0);
			//printf("pop 0x%x\n", v);
		}
	} while(1);
#endif

	printf("CPU#0 says Hello world!\n");
	/* Loop forever */
	for(;;)
	{
		counter++;
		PPC_WAIT;
	}
}
