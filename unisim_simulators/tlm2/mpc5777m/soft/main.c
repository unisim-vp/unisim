#include <inttypes.h>

unsigned int intc_psr = 0;

int
main()
{
  intc_psr = *((volatile unsigned int *) 0xfc040060);
  *((volatile unsigned int *) 0xfc040060) = 0x12345678;
  
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


