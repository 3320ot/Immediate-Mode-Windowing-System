#ifndef ds1302z_h
#define ds1302z_h

#include "ch32v20x.h"

#define CLK_CLK     GPIO_Pin_13
#define CLK_IO      GPIO_Pin_14
#define CLK_RES     GPIO_Pin_15

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Time;

void DS1302_Init();

Time DS1302_GetTime();

void DS1302_SetTime(Time time);

#endif
