#include "ds1302z.h"

GPIO_InitTypeDef GPIO_InitStructure = {0};

void DS1302_GpioInit(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | 
        RCC_APB2Periph_GPIOD,
        ENABLE);

    GPIO_InitStructure.GPIO_Pin = CLK_IO | CLK_CLK;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CLK_RES;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void DS1302_ReadMode(){
    GPIO_InitStructure.GPIO_Pin = CLK_IO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void DS1302_WriteMode(){
    GPIO_InitStructure.GPIO_Pin = CLK_IO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void DS1302_WriteByte(uint8_t data){
    for(uint8_t i = 0; i < 8; i++){
        GPIO_ResetBits(GPIOC, CLK_CLK);
        if (data >> i)
            GPIO_SetBits(GPIOC, CLK_IO);
        else
            GPIO_ResetBits(GPIOC, CLK_IO);
        Delay_Ms(2);
        GPIO_SetBits(GPIOC, CLK_CLK);
        Delay_Ms(2);
    }
}

void DS1302_WriteReg(uint8_t addr, uint8_t data){
    GPIO_ResetBits(GPIOD, CLK_RES);
    GPIO_ResetBits(GPIOC, CLK_CLK);
    Delay_Ms(2);

    GPIO_SetBits(GPIOD, CLK_RES);
    Delay_Ms(2);

    DS1302_WriteByte(addr);
    DS1302_WriteByte(data);

    GPIO_ResetBits(GPIOD, CLK_RES);
    GPIO_ResetBits(GPIOC, CLK_IO);
}

void DS1302_Init(){
    DS1302_GpioInit();
    DS1302_WriteMode();
    Delay_Ms(100); 
    
    DS1302_WriteReg(0x8E, 0x00);
    DS1302_WriteReg(0x80, 0x00);
    DS1302_WriteReg(0x8E, 0x80);
}