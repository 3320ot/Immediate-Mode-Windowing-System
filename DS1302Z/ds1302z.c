#include "ds1302z.h"

GPIO_InitTypeDef GPIO_InitStructure = {0};

void DS1302_GpioInit(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,
        ENABLE);

    GPIO_InitStructure.GPIO_Pin = CLK_IO | CLK_CLK | CLK_RES;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void DS1302_ReadMode(){
    GPIO_InitStructure.GPIO_Pin = CLK_IO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void DS1302_WriteMode(){
    GPIO_InitStructure.GPIO_Pin = CLK_IO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void DS1302_WriteByte(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        GPIO_ResetBits(GPIOC, CLK_CLK);
        if ((data >> i) & 1)
            GPIO_SetBits(GPIOC, CLK_IO);
        else
            GPIO_ResetBits(GPIOC, CLK_IO);
        Delay_Ms(1);
        GPIO_SetBits(GPIOC, CLK_CLK);
        Delay_Ms(1);
    }
    GPIO_ResetBits(GPIOC, CLK_CLK);
}

uint8_t DS1302_ReadByte(void) {
    uint8_t data = 0;
    GPIO_InitStructure.GPIO_Pin = CLK_IO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    for (uint8_t i = 0; i < 8; i++) {
        GPIO_ResetBits(GPIOC, CLK_CLK);
        Delay_Ms(1);
        if (GPIO_ReadInputDataBit(GPIOC, CLK_IO))
            data |= (1 << i);
        GPIO_SetBits(GPIOC, CLK_CLK);
        Delay_Ms(1);
    }
    GPIO_ResetBits(GPIOC, CLK_CLK);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    return data;
}

void DS1302_WriteReg(uint8_t addr, uint8_t data) {
    uint8_t cmd = 0x80 | (addr << 1);
    GPIO_ResetBits(GPIOC, CLK_CLK | CLK_RES);
    Delay_Ms(2);

    GPIO_SetBits(GPIOC, CLK_RES);
    Delay_Ms(2);

    DS1302_WriteByte(cmd);
    DS1302_WriteByte(data);
    GPIO_ResetBits(GPIOC, CLK_CLK | CLK_RES);
}

uint8_t DS1302_ReadReg(uint8_t addr) {
    uint8_t cmd = 0x81 | (addr << 1);
    uint8_t data;
    GPIO_ResetBits(GPIOC, CLK_CLK | CLK_RES);
    Delay_Ms(1);

    GPIO_SetBits(GPIOC, CLK_RES);
    Delay_Ms(1);

    DS1302_WriteByte(cmd);
    data = DS1302_ReadByte();
    GPIO_ResetBits(GPIOC, CLK_CLK | CLK_RES);
    return data;
}

uint8_t dec_to_bcd(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

uint8_t bcd_to_dec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

Time DS1302_GetTime(){
    Time time;

    uint8_t sec_reg   = DS1302_ReadReg(0);
    uint8_t min_reg   = DS1302_ReadReg(1);
    uint8_t hour_reg  = DS1302_ReadReg(2);
    uint8_t day_reg   = DS1302_ReadReg(3);
    uint8_t month_reg = DS1302_ReadReg(4);
    uint8_t wday_reg  = DS1302_ReadReg(5);
    uint8_t year_reg  = DS1302_ReadReg(6);

    time.second  = bcd_to_dec(sec_reg & 0x7F);
    time.minute  = bcd_to_dec(min_reg);
    time.hour    = bcd_to_dec(hour_reg & 0x3F);
    time.day     = bcd_to_dec(day_reg);
    time.month   = bcd_to_dec(month_reg);
    time.weekday = wday_reg;
    time.year    = bcd_to_dec(year_reg);

    return time;
}

void DS1302_SetTime(Time time){
    DS1302_WriteReg(7, 0x00);

    uint8_t sec_stop = dec_to_bcd(time.second) | 0x80;
    DS1302_WriteReg(0, sec_stop);
    Delay_Ms(2);

    DS1302_WriteReg(1, dec_to_bcd(time.minute));
    DS1302_WriteReg(2, dec_to_bcd(time.hour) & 0x3F);
    DS1302_WriteReg(3, dec_to_bcd(time.day));
    DS1302_WriteReg(4, dec_to_bcd(time.month));
    DS1302_WriteReg(5, time.weekday);
    DS1302_WriteReg(6, dec_to_bcd(time.year));

    DS1302_WriteReg(0, dec_to_bcd(time.second) & 0x7F);
}

void DS1302_Init(){
    DS1302_GpioInit();
    DS1302_WriteMode();
    Delay_Ms(100); 

//    Time default_time = {
//            .second = 0,
//            .minute = 57,
//            .hour   = 0,
//            .day    = 16,
//            .month  = 6,
//            .year   = 26,
//            .weekday = 1
//        };
//    DS1302_SetTime(default_time);
//    Time t = DS1302_GetTime();
    DS1302_WriteReg(7, 0x00);
    DS1302_WriteReg(0, 0x00);
}
