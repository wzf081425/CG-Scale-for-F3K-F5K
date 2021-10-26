#ifndef __HX711_H__
#define __HX711_H__

extern signed int offset[];
extern float ratio[];
extern unsigned long temp1,temp2;
extern signed int weight1,weight2;

void Hx711_Read (void);
void Hx711_Init (void);
void HX711_GetWeight (void);
void HX711_SetOffset (void);
void HX711_Calib (unsigned char ch);

#endif
