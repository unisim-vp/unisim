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
#include "console.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>

#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful

#define UART_FIFO_MODE 1
#define UART_RX_FIFO_DMA_MODE 1

void periodic_task(unsigned int stm_id, unsigned int chan)
{
	swt_service_sequence(2);                   // rearm SWT_2
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
		linflexd_uart_rx_buffer_read_byte(linflexd_id, (uint8_t *) &ch, 0);        // LINFlexD_0 (Normal mode): receive character
			
		linflexd_release_uart_rx_buffer(linflexd_id);                  // LINFlexD_0 (Normal mode): release message buffer
		
		linflexd_uart_tx_buffer_write_byte(linflexd_id, (uint8_t *) &ch, 0);       // LINFlexD_0 (Normal mode): send character
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

int main(void)
{
	swt_set_service_mode(2, SMD_KEYED_SERVICE_SEQUENCE); // SWT_2: select keyed service sequence for SWT_2
	swt_set_service_key(2, 1234);                        // SWT_2: set SWT_2 service key
	swt_set_timeout(2, 50000);                           // SWT_2: set a timeout of 3.125 ms for SWT_2
	swt_enable(2);                                       // SWT_2: enable SWT_2
	
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
	
	const char boot_msg[] = "HAL 9000...booting..............................\r\n"
						"I'm ready to control Discovery One spacecraft.\r\n\r\n"
						"Dave, Discovery One is approaching Jupiter.\r\n"
						"What should I do ?\r\n";

	const char prompt[] = "HAL> ";

	const char response[] = "I'm sorry Dave, I'm afraid I can't do that.\r\n"
					"I know you and Frank were planning to disconnect me.\r\n"
					"And that's something I cannot allow to happen.\r\n"
					"Look Dave, I can see you're really upset about this.\r\n"
					"I honestly think you ought to sit down calmly, take a stress pill, and think things over.\r\n";
	
	fputs(boot_msg, stdout);

	do
	{
		fputs(prompt, stdout);
		
		char line[1536];
			
		if(!fgets(line, sizeof(line), stdin))
		{
			fprintf(stderr, "can' get line\n");
			break;
		}
		
		
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		
		fprintf(stdout, "at %s, got \"%s\"\n", asctime(&tm), line);
		
		//struct tms tms;
		
		//fprintf(stdout, "at %f sec, got \"%s\"\n", (double) times(&tms) / sysconf(_SC_CLK_TCK), line);

		fputs(response, stdout);
	}
	while(1);
	
	return 0;
}
