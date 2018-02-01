/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include <stdio.h>

volatile int counter = 0;

int main(void)
{
	printf("CPU#0 says Hello world!\n");
	/* Loop forever */
	for(;;)
	{
		counter++;
		PPC_WAIT;
	}
}
