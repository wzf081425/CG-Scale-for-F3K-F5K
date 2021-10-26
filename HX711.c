#include <iostm8l101f3.h>
#include "delay.h"
#include "HX711.h"

#include "oled.h"

#define SCK1 PB_ODR_ODR1
#define SCK2 PB_ODR_ODR3
#define DOUT1 PB_IDR_IDR0
#define DOUT2 PB_IDR_IDR2

#define RATIO_ADDR1 0x9fc0
#define RATIO_ADDR2 0x9fc4

signed int offset[2];
float ratio[2]={151.0,151.0};
unsigned long temp1,temp2;
signed int weight1,weight2;

void Hx711_Init (void)
{
  unsigned char *p;
  unsigned char byte0,byte1,byte2,byte3;
  float temp;
  
  p = (unsigned char *) RATIO_ADDR1;
  byte0 = *p;
  p = (unsigned char *) RATIO_ADDR1 + 1;
  byte1 = *p;
  p = (unsigned char *) RATIO_ADDR1 + 2;
  byte2 = *p;
  p = (unsigned char *) RATIO_ADDR1 + 3;
  byte3 = *p;
  ((unsigned char *)&temp)[0]=byte0;
  ((unsigned char *)&temp)[1]=byte1;
  ((unsigned char *)&temp)[2]=byte2;
  ((unsigned char *)&temp)[3]=byte3;
  if (temp != 0)
  {
    ratio[0] = temp;
  }
   
  p = (unsigned char *) RATIO_ADDR2;
  byte0 = *p;
  p = (unsigned char *) RATIO_ADDR2 + 1;
  byte1 = *p;
  p = (unsigned char *) RATIO_ADDR2 + 2;
  byte2 = *p;
  p = (unsigned char *) RATIO_ADDR2 + 3;
  byte3 = *p;
  ((unsigned char *)&temp)[0]=byte0;
  ((unsigned char *)&temp)[1]=byte1;
  ((unsigned char *)&temp)[2]=byte2;
  ((unsigned char *)&temp)[3]=byte3;
  if (temp != 0)
  {
    ratio[1] = temp;
  }
  
  //SCK1
  PB_DDR_DDR1 = 1;
  PB_CR1_C11 = 1;
  PB_CR2_C21 = 1;
  PB_ODR_ODR1 = 0;
  //SCK2
  PB_DDR_DDR3 = 1;
  PB_CR1_C13 = 1;
  PB_CR2_C23 = 1;
  PB_ODR_ODR3 = 0;
  //DOUT1
  PB_DDR_DDR0 = 0;
  PB_CR1_C10 = 1;
  PB_CR2_C20 = 0;
  //DOUT2
  PB_DDR_DDR2 = 0;
  PB_CR1_C12 = 1;
  PB_CR2_C22 = 0;
  
  Hx711_Read();
}

void Hx711_Read (void)
{
  unsigned char i = 0;
  
  temp1 = 0;
  temp2 = 0;
  
  SCK1 = 0;
  SCK2 = 0;
  while(DOUT1 || DOUT2);
  inerDelay_us(1);
  SCK1 = 1;
  SCK2 = 1;
  for(i=0;i<24;i++)
  {
    inerDelay_us(5);
    SCK1 = 0;
    SCK2 = 0;
    inerDelay_us(2);
    if(DOUT1)
    {
      temp1 ++;
    }
    if(DOUT2)
    {
      temp2 ++;
    }
    temp1 <<= 1;
    temp2 <<= 1;
    inerDelay_us(3);
    SCK1 = 1;
    SCK2 = 1;
  }
  inerDelay_us(5);
  SCK1 = 0;
  SCK2 = 0;
}


void HX711_SetOffset (void)
{
  unsigned char i;
  
  Hx711_Read();
  offset[0] = ((signed int)(temp1/500));
  offset[1] = ((signed int)(temp2/500));
  for(i=0;i<15;i++)
  {
    Delayms(50);
    OLED_ShowBar(28,4,i);
  }
  for(i=15;i<25;i++)
  {
    Delayms(50);
    Hx711_Read();
    offset[0] += ((signed int)(temp1/500));
    offset[0] /= 2;
    offset[1] += ((signed int)(temp2/500));
    offset[1] /= 2;
    OLED_ShowBar(28,4,i);
  }
}

void HX711_Calib (unsigned char ch)
{
  signed int temp02,temp12;
  unsigned char i,byte0,byte1,byte2,byte3;
  unsigned char *p;
  
  if (ch == 1)
  {
    
    temp02 = ((signed int) (temp1/500)) - offset[0];
    Delayms(50);
    for (i=0;i<9;i++)
    {
      Hx711_Read();
      temp02 += ((signed int) (temp1/500)) - offset[0];
      Delayms(50);
    }
    ratio[0] = temp02 / 50.0;
    
    byte0=((unsigned char *)&ratio[0])[0];
    byte1=((unsigned char *)&ratio[0])[1];
    byte2=((unsigned char *)&ratio[0])[2];
    byte3=((unsigned char *)&ratio[0])[3];
    
    FLASH_DUKR = 0xae;
    FLASH_DUKR = 0x56;
    while((FLASH_IAPSR & 0x08) == 0);
    p = (unsigned char *) RATIO_ADDR1;
    *p = byte0;
    while((FLASH_IAPSR & 0x04) == 0);
    p = (unsigned char *) RATIO_ADDR1 + 1;
    *p = byte1;
    while((FLASH_IAPSR & 0x04) == 0);
    p = (unsigned char *) RATIO_ADDR1 + 2;
    *p = byte2;
    while((FLASH_IAPSR & 0x04) == 0);
    p = (unsigned char *) RATIO_ADDR1 + 3;
    *p = byte3;
    while((FLASH_IAPSR & 0x04) == 0);
    
    FLASH_DUKR = 0x56;
    FLASH_DUKR = 0xae;
  }
  else if(ch == 2)
  {
    Hx711_Read();
    temp12 = ((signed int) (temp2/500)) - offset[1];
    Delayms(50);
    for (i=0;i<9;i++)
    {
      Hx711_Read();
      temp12 += ((signed int) (temp2/500)) - offset[1];
      Delayms(50);
    }
    ratio[1] = temp12 / 50.0;
    
    byte0=((unsigned char *)&ratio[1])[0];
    byte1=((unsigned char *)&ratio[1])[1];
    byte2=((unsigned char *)&ratio[1])[2];
    byte3=((unsigned char *)&ratio[1])[3];
    
    FLASH_DUKR = 0xae;
    FLASH_DUKR = 0x56;
    while((FLASH_IAPSR & 0x08) == 0);
    p = (unsigned char *) RATIO_ADDR2;
    *p = byte0;
    while((FLASH_IAPSR & 0x04) == 0);
    p = (unsigned char *) RATIO_ADDR2 + 1;
    *p = byte1;
    while((FLASH_IAPSR & 0x04) == 0);
    p = (unsigned char *) RATIO_ADDR2 + 2;
    *p = byte2;
    while((FLASH_IAPSR & 0x04) == 0);
    p = (unsigned char *) RATIO_ADDR2 + 3;
    *p = byte3;
    while((FLASH_IAPSR & 0x04) == 0);
    
    FLASH_DUKR = 0x56;
    FLASH_DUKR = 0xae;
  }
}

void HX711_GetWeight (void)
{
  double temp01,temp11;
  signed int temp02,temp12;
  
  Hx711_Read();
  temp02 = ((signed int) (temp1/500)) - offset[0];
  temp12 = ((signed int) (temp2/500)) - offset[1];
  Delayms(50);
  Hx711_Read();
  temp02 += ((signed int) (temp1/500)) - offset[0];
  temp12 += ((signed int) (temp2/500)) - offset[1];
  temp01 = temp02 / ratio[0];
  temp11 = temp12 / ratio[1];
  
  weight1 = (signed int)(temp01 * 100);
  weight2 = (signed int)(temp11 * 100);
}
