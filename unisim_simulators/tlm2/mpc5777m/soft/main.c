#include <inttypes.h>

unsigned int intc_psr = 0;
unsigned int stm2_cir0 = 0;

int
main()
{
  intc_psr = *((volatile unsigned int *) 0xfc040060);
  *((volatile unsigned int *) 0xfc040060) = 0x12345678;
  
  stm2_cir0 = *((volatile unsigned int *) 0xfc070014);
  *((volatile unsigned int *) 0xfc070014) = 1;
  
  int a, b;
  
  for (a = 1, b = 1; a != 0xdeadc0de;)
    {
      int r = a + b;
      a = b;
      b = r;
      __asm__ volatile ("se_nop");
    }
  
  return a;
}


