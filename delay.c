#include "delay.h"

void inerDelay_us(unsigned char n) 
{
  for(;n>0;n--)
  {
    asm("nop"); //在STM8里面，16M晶振，_nop_() 延时了 333ns
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
