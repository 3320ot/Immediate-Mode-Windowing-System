#ifndef ds1302z_h
#define ds1302z_h

#include "ch32v00x.h"

#define CLK_CLK     GPIO_Pin_4
#define CLK_IO      GPIO_Pin_3
#define CLK_RES     GPIO_Pin_0

void DS1302_Init();

#endif