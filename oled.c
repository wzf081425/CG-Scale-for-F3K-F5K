#include <iostm8l101f3.h>
#include "oled.h"
#include "codetab.h"
#include "delay.h"

int I2C_WriteByte(unsigned char addr, unsigned char data)
{
  unsigned char temp;
  
  while (I2C_SR3_BUSY != 0);
  
  I2C_CR2_START = 1;
  while(I2C_SR1_SB == 0);
  
  I2C_DR = 0x78;
  while (I2C_SR1_ADDR == 0);

  temp = I2C_SR1;
  temp = I2C_SR3;
  
  while (I2C_SR1_TXE == 0);
  I2C_DR = addr;
  
  //while (I2C_SR1_BTF == 0);

  
  I2C_DR = data;
  while (I2C_SR1_TXE == 0);
  //while (I2C_SR1_BTF == 0);
  temp = I2C_SR1;
  
  I2C_CR2_STOP = 1;
  
  return 0;
}

void WriteCmd(unsigned char I2C_Command)//写命令
{
  I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(unsigned char I2C_Data)//写数据
{
  I2C_WriteByte(0x40, I2C_Data);
}

void OLED_Init(void)
{
 
  PC_ODR |= 3;                //define SDA, SCL outputs, HiZ, Open drain, Fast
  PC_DDR |= 3;
  PC_CR2 |= 3;
  
  CLK_PCKENR |= 8;
  //I2C_CR1_PE = 0;
  I2C_FREQR = 16;
  I2C_CCRL = 15;
  I2C_CCRH = 0X80;
  I2C_TRISER = 5;
  I2C_OARL = 0xa0;
  I2C_OARH |= 0x40;
  I2C_CR2 |= 0X04;
  I2C_CR1_PE = 1;
  
  Delayms(100); //这里的延时很重要

  WriteCmd(0xAE); //display off
  WriteCmd(0x20);	//Set Memory Addressing Mode	
  WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
  WriteCmd(0xc8);	//Set COM Output Scan Direction
  WriteCmd(0x00); //---set low column address
  WriteCmd(0x10); //---set high column address
  WriteCmd(0x40); //--set start line address
  WriteCmd(0x81); //--set contrast control register
  WriteCmd(0xff); //亮度调节 0x00~0xff
  WriteCmd(0xa1); //--set segment re-map 0 to 127
  WriteCmd(0xa6); //--set normal display
  WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
  WriteCmd(0x3F); //
  WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  WriteCmd(0xd3); //-set display offset
  WriteCmd(0x00); //-not offset
  WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
  WriteCmd(0xf0); //--set divide ratio
  WriteCmd(0xd9); //--set pre-charge period
  WriteCmd(0x22); //
  WriteCmd(0xda); //--set com pins hardware configuration
  WriteCmd(0x12);
  WriteCmd(0xdb); //--set vcomh
  WriteCmd(0x20); //0x20,0.77xVcc
  WriteCmd(0x8d); //--set DC-DC enable
  WriteCmd(0x14); //
  OLED_Fill(0x00);
  WriteCmd(0xaf); //--turn on oled panel
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
  unsigned char m,n;
  for(m=0;m<8;m++)
  {
    WriteCmd(0xb0+m);		//page0-page1
    WriteCmd(0x00);		//low column start address
    WriteCmd(0x10);		//high column start address
    for(n=0;n<130;n++)
    {
      WriteDat(fill_Data);
    }
  }
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
        x += 2;
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_ShowBar(unsigned char x, unsigned char y, unsigned char num)
{
  unsigned char i = 0;
  
  OLED_SetPos(x,y);
  WriteDat(0xff);
  for(i=0;i<24;i++)
  {
    if(i<num)
    {
      WriteDat(0xff);
      WriteDat(0xff);
      WriteDat(0xff);
    }
    else
    {
      WriteDat(0x81);
      WriteDat(0x81);
      WriteDat(0x81);
    }
  }
  WriteDat(0xff);
}

void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch, unsigned char TextSize)
{
  unsigned char c = 0,i = 0;
  
  switch(TextSize)
  {
    case 1:
    {

    }break;
    case 2:
    {
      c = ch - 32;
      OLED_SetPos(x,y);
      for(i=0;i<8;i++)
              WriteDat(F8X16[c*16+i]);
      OLED_SetPos(x,y+1);
      for(i=0;i<8;i++)
              WriteDat(F8X16[c*16+i+8]);
      x += 8;
    }break;
    case 3:
    {
      c = ch;
      OLED_SetPos(x,y);
      for(i=0;i<12;i++)
              WriteDat(F12x24[c*36+i]);
      OLED_SetPos(x,y+1);
      for(i=0;i<12;i++)
              WriteDat(F12x24[c*36+i+12]);
      OLED_SetPos(x,y+2);
      for(i=0;i<12;i++)
              WriteDat(F12x24[c*36+i+24]);
      x += 12;
    }break;
    case 4:
    {
      c = ch;
      OLED_SetPos(x,y);
      for(i=0;i<24;i++)
              WriteDat(F24x24[c*72+i]);
      OLED_SetPos(x,y+1);
      for(i=0;i<24;i++)
              WriteDat(F24x24[c*72+i+24]);
      OLED_SetPos(x,y+2);
      for(i=0;i<24;i++)
              WriteDat(F24x24[c*72+i+48]);
      x += 24;
    }break;
  }
}


void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{

		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

void OLED_ShowNum(unsigned char x, unsigned char y, signed int Num, unsigned char TextSize)
{
  unsigned char c = 0,i = 0;
  unsigned int j = 10000;
  
  switch(TextSize)
  {
    case 1:
    {

    }break;
    case 2:
    {
      if(Num < 0)
      {
        OLED_ShowStr(x,y,"-",TextSize);
        Num = Num * -1;
        x += 8;
      }
      while(j>0)
      {
         if(Num/j > 0)
         {
            c = Num / j % 10 + 16;
            if(x > 120)
            {
                    x = 0;
                    y++;
            }
            OLED_SetPos(x,y);
            for(i=0;i<8;i++)
                    WriteDat(F8X16[c*16+i]);
            OLED_SetPos(x,y+1);
            for(i=0;i<8;i++)
                    WriteDat(F8X16[c*16+i+8]);
            x += 8;
         }
         j /= 10;
      }
    }break;
  }
}

