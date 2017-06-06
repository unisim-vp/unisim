#include <inttypes.h>

int
main()
{
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


