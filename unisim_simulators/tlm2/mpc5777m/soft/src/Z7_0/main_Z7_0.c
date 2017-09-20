/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"

int main(void)
{
	volatile int counter = 0;
	
	/* Loop forever */
	for(;;) {	   
	   	counter++;
//		PPC_WAIT;
	}
}
