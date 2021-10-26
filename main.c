#include <iostm8l101f3.h>
#include "oled.h"
#include "delay.h"
#include "HX711.h"

void Update(void);

unsigned int time = 0;
unsigned int time1 = 0;
unsigned char Calib_flag = 0;

int main(void)
{
  CLK_CKDIVR_HSIDIV = 0;
  
  PB_DDR_DDR4 = 1;
  PB_CR1_C14 = 1;
  PB_CR2_C24 = 1;
  PB_ODR_ODR4 = 1;
  
  OLED_Init();
  OLED_ShowStr(10,2,"CG Scale V1.2",2);
  OLED_ShowStr(18,4,"F3K Version",2);
  Delayms(2000);
  OLED_Fill(0x00);
  OLED_ShowBar(28,4,0);
  
  Hx711_Init();
  Delayms(100);
  HX711_SetOffset();
  OLED_Fill(0x00);
  
  OLED_ShowChar(8,1,1,4);
  OLED_ShowChar(8,5,0,4);
  
  while(1)
  {
    Update();
    //time++;
    //if(time > 100)
    //  PB_ODR_ODR4 = 0;
    Delayms(100);
  }
}

void Update(void)
{
  double cg_ratio;
  signed int cg;
  signed int weight;
  
  HX711_GetWeight();
  if((weight1 == 1)||(weight1 == -1))
  {
    weight1 = 0;
  }
  if((weight2 == 1)||(weight2 == -1))
  {
    weight2 = 0;
  }
  if((weight1 >= 0)&&(weight2 >= 0))
  {
    weight = weight1 + weight2;
    OLED_ShowChar(40,1,weight/1000%10,3);
    OLED_ShowChar(52,1,weight/100%10,3);
    OLED_ShowChar(64,1,weight/10%10,3);
    OLED_ShowChar(76,1,10,3);
    OLED_ShowChar(88,1,weight%10,3);
    OLED_ShowChar(100,1,12,3);
    
    if ((weight1 >= 900)&(weight1 <= 1100)&(weight2 == 0))
    {
      if(time > 65)
      {
        OLED_Fill(0x00);
        OLED_ShowStr(8,3,"CH1 Calibrated",2);
        HX711_Calib(1);
        Delayms(50);
        HX711_GetWeight();
        while(weight1 != 0)
        {
          Delayms(50);
          HX711_GetWeight();
        }
        OLED_Fill(0x00);
        OLED_ShowChar(8,1,1,4);
        OLED_ShowChar(8,5,0,4);
        time = 0;
      }
      else
      {
        time++;
      }
    }
    else
    {
      time = 0;
    }
    
    if ((weight2 >= 900)&(weight2 <= 1100)&(weight1 == 0))
    {
      if(time1 > 65)
      {
        OLED_Fill(0x00);
        OLED_ShowStr(8,3,"CH2 Calibrated",2);
        HX711_Calib(2);
        Delayms(50);
        HX711_GetWeight();
        while(weight2 != 0)
        {
          Delayms(50);
          HX711_GetWeight();
        }
        OLED_Fill(0x00);
        OLED_ShowChar(8,1,1,4);
        OLED_ShowChar(8,5,0,4);
        time = 0;
      }
      else
      {
        time1++;
      }
    }
    else
    {
      time1 = 0;
    }
    
    if(weight1 && weight2)
    {
      cg_ratio = (weight1 / 100.0) / (weight /100.0);
      cg = (signed int)(cg_ratio * 842.0);
      cg += 197.5;
      OLED_ShowChar(40,5,cg/1000%10,3);
      OLED_ShowChar(52,5,cg/100%10,3);
      OLED_ShowChar(64,5,cg/10%10,3);
      OLED_ShowChar(76,5,10,3);
      OLED_ShowChar(88,5,cg%10,3);
      OLED_ShowChar(100,5,11,3);
      OLED_ShowChar(112,5,11,3);
    }
    else
    {
      OLED_ShowChar(40,5,13,3);
      OLED_ShowChar(52,5,13,3);
      OLED_ShowChar(64,5,13,3);
      OLED_ShowChar(76,5,10,3);
      OLED_ShowChar(88,5,13,3);
      OLED_ShowChar(100,5,11,3);
      OLED_ShowChar(112,5,11,3);
    }
  }
  else
  {
    OLED_ShowChar(40,1,13,3);
    OLED_ShowChar(52,1,13,3);
    OLED_ShowChar(64,1,13,3);
    OLED_ShowChar(76,1,10,3);
    OLED_ShowChar(88,1,13,3);
    OLED_ShowChar(100,1,12,3);
    
    OLED_ShowChar(40,5,13,3);
    OLED_ShowChar(52,5,13,3);
    OLED_ShowChar(64,5,13,3);
    OLED_ShowChar(76,5,10,3);
    OLED_ShowChar(88,5,13,3);
    OLED_ShowChar(100,5,11,3);
    OLED_ShowChar(112,5,11,3);
  }
}
