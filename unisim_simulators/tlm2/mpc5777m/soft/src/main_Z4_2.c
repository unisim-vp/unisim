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

#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful

extern void xcptn_xmpl(void);

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

	intc_init();
	swt_init(2);
	stm_init(2);
	pit_init(0);
	pit_init(1);
	intc_enable_external_interrupt();
}

void periodic_task(unsigned int stm_id, unsigned int chan)
{
	swt_service_sequence(2);
	stm_set_channel_compare(stm_id, chan, stm_get_channel_compare(stm_id, chan) + 10000);
	stm_clear_interrupt_flag(stm_id, chan);
}

void periodic_task2(unsigned int pit_id, unsigned int chan)
{
	pit_clear_timer_interrupt_flag(pit_id, chan);
}

int main(void)
{
	unsigned int counter = 0;
	
	swt_set_service_mode(2, SMD_KEYED_SERVICE_SEQUENCE);
	swt_set_service_key(2, 1234);
	swt_set_timeout(2, 50000);
	swt_enable(2);
	
	
	stm_set_channel_irq_priority(2, 0, 1); // set STM_2 channel #0 IRQ priority level to 1
	stm_select_channel_irq_for_processor(2, 0, 2); // select STM_2 channel #0 IRQ for processor #2

	pit_set_timer_irq_priority(0, 0, 3); // set PIT_0 timer #0 IRQ priority level to 2
	pit_select_timer_irq_for_processor(0, 0, 2);
	
	stm_set_interrupt_handler(2, 0, periodic_task); // install a hook for STM_2 channel #0 interrupts
	stm_enable_counter(2); // enable STM_2 counter
	stm_set_channel_compare(2, 0, 10000); // set STM_2 channel #0 compare value to 10000 cycles
	stm_enable_channel(2, 0); // enable STM_2 channel #0
	
	pit_set_timer_interrupt_handler(0, 0, periodic_task2);
	pit_set_timer_load_value(0, 0, 14999);
	pit_enable_timer(0, 0);
	pit_enable_timer_interrupt(0, 0);
	pit_enable_timers_clock(0);
	
	/* Loop forever */
	for(;;) {	   
	   	counter++;
	}
}
