#include "delay.h"

void inerDelay_us(unsigned char n) 
{
  for(;n>0;n--)
  {
    asm("nop"); //��STM8���棬16M����_nop_() ��ʱ�� 333ns
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

void Delayms(unsigned int time)
{
  unsigned int i;
  while(time--)
  {
    for(i=900;i>0;i--)
    {
      inerDelay_us(1);
    }
  }
}
