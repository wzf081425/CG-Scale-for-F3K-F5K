#ifndef __OLED_H__
#define __OLED_H__

int I2C_WriteByte(unsigned char addr, unsigned char data);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_Fill(unsigned char fill_Data);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowNum(unsigned char x, unsigned char y, signed int Num, unsigned char TextSize);
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch, unsigned char TextSize);
void OLED_ShowBar(unsigned char x, unsigned char y, unsigned char num);

#endif