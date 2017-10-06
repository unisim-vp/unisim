/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"

volatile int counter = 0;

int main_Z7_0(void)
{
	
	/* Loop forever */
	for(;;)
	{
		counter++;
		PPC_WAIT;
	}
}
