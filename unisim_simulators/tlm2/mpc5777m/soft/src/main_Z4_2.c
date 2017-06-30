/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"

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
	stm_init(2);
	intc_enable_external_interrupt();
}

void periodic_task(unsigned int stm_id, unsigned int chan)
{
	stm_set_channel_compare(stm_id, chan, stm_get_channel_compare(stm_id, chan) + 10000);
	stm_clear_interrupt_flag(stm_id, chan);
}

int main(void)
{
	unsigned int counter = 0;
	
	stm_set_channel_irq_priority(2, 0, 1); // set STM_2 channel #0 IRQ priority level to 1
	stm_select_channel_irq_for_processor(2, 0, 2); // select STM_2 channel #0 IRQ for processor #2

	stm_set_interrupt_handler(2, 0, periodic_task); // install a hook for STM_2 channel #0 interrupts
	stm_enable_counter(2); // enable STM_2 counter
	stm_set_channel_compare(2, 0, 10000); // set STM_2 channel #0 compare value to 10000 cycles
	stm_enable_channel(2, 0); // enable STM_2 channel #0
	
	/* Loop forever */
	for(;;) {	   
	   	counter++;
	}
}
