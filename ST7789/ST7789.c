#include "ch32v20x.h"
#include "ST7789.h"
#include "spi.h"

uint8_t counter = 0;
uint16_t GPIOB_Data;

void ST7789_BlkButton_Init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = BGR;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BUTTON1 |
            BUTTON2 |
            BUTTON3 |
            BUTTON4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    TIM_Cmd(TIM2, DISABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 999;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 47;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 999;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_CCxCmd(TIM2, TIM_Channel_2, TIM_CCx_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) {
    if (TIM2->INTFR & TIM_UIF) {
        TIM2->INTFR &= ~TIM_UIF;
        if (counter < 50) {
            counter++;
        } else {
            counter=0;
            GPIOB_Data = GPIO_ReadInputData(GPIOB);
        };
    }
}

void ST7789_SetBrightness(uint16_t brightness){
    TIM2->PSC = 47;
    TIM2->ATRLR = 999;
    TIM2->CH2CVR = brightness;
    TIM_GenerateEvent(TIM2, TIM_EventSource_Update);
}

void ST7789_WriteCmd(uint8_t cmd) {
    GPIOB->BCR = DC;
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, cmd);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_WriteData(uint8_t data) {
    GPIO_SetBits(GPIOB, DC);
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_SetConfig(){
    ST7789_WriteCmd(0x01); Delay_Ms(150);  // Soft Reset
    ST7789_WriteCmd(0x11); Delay_Ms(150);  // Sleep Out
    ST7789_WriteCmd(0x20);                 // INVON

    ST7789_WriteCmd(0x3A);
    ST7789_WriteData(0x55);

    ST7789_WriteCmd(0x36);
    ST7789_WriteData(BGR | VERTICAL);

    ST7789_WriteCmd(0xB0);
    ST7789_WriteData(0x00);

    ST7789_WriteCmd(0xB1);
    ST7789_WriteData(0xC0);
    ST7789_WriteData(0x0B);

    ST7789_WriteCmd(0xB4);
    ST7789_WriteData(0x02);

    ST7789_WriteCmd(0xB6);
    ST7789_WriteData(0x02);
    ST7789_WriteData(0x02);
    ST7789_WriteData(0x3B);

    ST7789_WriteCmd(0xB7);
    ST7789_WriteData(0xC6);

    ST7789_WriteCmd(0xC0);
    ST7789_WriteData(0x10);
    ST7789_WriteData(0x10);

    ST7789_WriteCmd(0xC1);
    ST7789_WriteData(0x41);

    ST7789_WriteCmd(0xC5);
    ST7789_WriteData(0x00);
    ST7789_WriteData(0x28);

    ST7789_WriteCmd(0xE0);
    ST7789_WriteData(0x0F);
    ST7789_WriteData(0x18);
    ST7789_WriteData(0x16);
    ST7789_WriteData(0x0B);
    ST7789_WriteData(0x10);
    ST7789_WriteData(0x07);
    ST7789_WriteData(0x06);
    ST7789_WriteData(0x07);
    ST7789_WriteData(0x06);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x1C);
    ST7789_WriteData(0x1E);
    ST7789_WriteData(0x27);
    ST7789_WriteData(0x32);

    ST7789_WriteCmd(0xE1);
    ST7789_WriteData(0x0F);
    ST7789_WriteData(0x18);
    ST7789_WriteData(0x16);
    ST7789_WriteData(0x0B);
    ST7789_WriteData(0x10);
    ST7789_WriteData(0x07);
    ST7789_WriteData(0x06);
    ST7789_WriteData(0x07);
    ST7789_WriteData(0x06);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x1C);
    ST7789_WriteData(0x1E);
    ST7789_WriteData(0x27);
    ST7789_WriteData(0x32);

    ST7789_WriteCmd(0x29);
    Delay_Ms(50);
}

void ST7789_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    ST7789_WriteCmd(0x2A);
    ST7789_WriteData(x1 >> 8); ST7789_WriteData(x1 & 0xFF);
    ST7789_WriteData(x2 >> 8); ST7789_WriteData(x2 & 0xFF);

    ST7789_WriteCmd(0x2B);
    ST7789_WriteData(y1 >> 8); ST7789_WriteData(y1 & 0xFF);
    ST7789_WriteData(y2 >> 8); ST7789_WriteData(y2 & 0xFF);

    ST7789_WriteCmd(0x2C);
}

uint32_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0b11111000) << 8) |
            ((g & 0b11111100) << 3) |
            (b >> 3);
}

uint32_t rgb_to_rgb888(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void sendBuffer(){
    for (uint8_t cnt = 0; cnt < BUFFER_SIZE; ++cnt) {
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = buffer[cnt] >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = buffer[cnt] >> 16;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = buffer[cnt] & 0xFF;
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void updateScreen(){
    GPIO_SetBits(GPIOB, DC);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

volatile uint16_t * const spi_stat = &SPI1->STATR;
volatile uint16_t * const spi_data = &SPI1->DATAR;
const uint32_t txe_mask = SPI_I2S_FLAG_TXE;

void ST7789_Init(){
    SPI1_Init();
    ST7789_SetConfig();
    ST7789_SetWindow(0, 0, RESOLUTION_X-1, RESOLUTION_Y-1);
//    ST7789_BlkButton_Init();
    SD_HighSpeed();
}
