#include "ch32v00x.h"
#include "ST7789.h"
#include "spi.h"
#include "ssd1306/font5x8.h"

Window windows[MAX_WINDOWS];

const uint32_t x = 240;
const uint32_t y = 240;

void ST7789_WriteCmd(uint8_t cmd) {
    GPIOD->BCR = DC;
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, cmd);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_WriteData(uint8_t data) {
    GPIO_SetBits(GPIOD, DC);
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_SetConfig(){
    ST7789_WriteCmd(0x01); Delay_Ms(150);  // Soft Reset
    ST7789_WriteCmd(0x11); Delay_Ms(150);  // Sleep Out
    ST7789_WriteCmd(0x21);                 // INVON
    ST7789_WriteCmd(0x13); Delay_Ms(10);   // NORON

    ST7789_WriteCmd(0x36); ST7789_WriteData(0x08); // RGB
    ST7789_WriteCmd(0x3A); ST7789_WriteData(0x06); // COLMOD

    ST7789_WriteCmd(0xB2);  // Porch setting
    ST7789_WriteData(0x0C);
    ST7789_WriteData(0x0C);
    ST7789_WriteData(0x00);
    ST7789_WriteData(0x33);
    ST7789_WriteData(0x33);

    ST7789_WriteCmd(0xB7);  // Gate control
    ST7789_WriteData(0x35);

    ST7789_WriteCmd(0xBB);  // VCOM
    ST7789_WriteData(0x19);

    ST7789_WriteCmd(0xC0);  // LCM control
    ST7789_WriteData(0x2C);

    ST7789_WriteCmd(0xC2);  // VDV and VRH command enable
    ST7789_WriteData(0x01);

    ST7789_WriteCmd(0xC3);  // VRH set
    ST7789_WriteData(0x12);

    ST7789_WriteCmd(0xC4);  // VDV set
    ST7789_WriteData(0x20);

    ST7789_WriteCmd(0xC6);  // Frame rate control
    ST7789_WriteData(0x0F);

    ST7789_WriteCmd(0xD0);  // Power control
    ST7789_WriteData(0xA4);
    ST7789_WriteData(0xA1);

    ST7789_WriteCmd(0xE0);  // Positive gamma
    ST7789_WriteData(0xD0);
    ST7789_WriteData(0x04);
    ST7789_WriteData(0x0D);
    ST7789_WriteData(0x11);
    ST7789_WriteData(0x13);
    ST7789_WriteData(0x2B);
    ST7789_WriteData(0x3F);
    ST7789_WriteData(0x54);
    ST7789_WriteData(0x4C);
    ST7789_WriteData(0x18);
    ST7789_WriteData(0x0D);
    ST7789_WriteData(0x0B);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x23);

    ST7789_WriteCmd(0xE1);  // Negative gamma
    ST7789_WriteData(0xD0);
    ST7789_WriteData(0x04);
    ST7789_WriteData(0x0C);
    ST7789_WriteData(0x11);
    ST7789_WriteData(0x13);
    ST7789_WriteData(0x2C);
    ST7789_WriteData(0x3F);
    ST7789_WriteData(0x44);
    ST7789_WriteData(0x51);
    ST7789_WriteData(0x2F);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x20);
    ST7789_WriteData(0x23);

    ST7789_WriteCmd(0x29);  // Display on
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
    return (((255 - r) & 0b11111000) << 8) |
            (((255 - g) & 0b11111100) << 3) |
            ((255 - b) >> 3);
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
    GPIO_SetBits(GPIOD, DC);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

volatile uint16_t * const spi_stat = &SPI1->STATR;
volatile uint16_t * const spi_data = &SPI1->DATAR;
const uint32_t txe_mask = SPI_I2S_FLAG_TXE;

void ST7789_FillQuickTest(uint32_t color) {
    GPIO_SetBits(GPIOD, DC);
    for(uint32_t i = 0; i < x*y; i++)
    {
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color >> 16;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color & 0xFF;
    }

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_Add_Window(
    uint16_t offsetX, 
    uint16_t offsetY, 
    uint16_t offsetX1, 
    uint16_t offsetY1, 
    uint32_t color,
    uint32_t bkg,
    uint8_t idx
){
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(offsetX, offsetY, offsetX1-1, offsetY1-1);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w = offsetX1-offsetX;
    uint16_t h = offsetY1-offsetY;
    for (uint32_t pix = 0; pix < w*h; pix++){
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color >> 16;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color & 0xFF;
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    SD_LowSpeed();
    ST7789_SetWindow(offsetX+3, offsetY+3, offsetX1-4, offsetY1-4);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w1 = offsetX1-offsetX;
    uint16_t h1 = offsetY1-offsetY;
    for (uint32_t pix = 0; pix < w1*h1; pix++){
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = bkg >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = bkg >> 16;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = bkg & 0xFF;
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    windows[idx].offsetX = offsetX;
    windows[idx].offsetY = offsetY;
    windows[idx].offsetX1 = offsetX1;
    windows[idx].offsetY1 = offsetY1;
    windows[idx].bkg = bkg;
    windows[idx].col = color;
}

void ST7789_InsertText5x8(uint8_t idx, char* text, uint32_t color){
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(windows[idx].offsetX+8, windows[idx].offsetY+8, 
        windows[idx].offsetX1-9, windows[idx].offsetY1-9);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w = windows[idx].offsetX1-windows[idx].offsetX-16;
    uint16_t h = windows[idx].offsetY1-windows[idx].offsetY-16;

    uint16_t len = 0;
    while(text[len]) len++;
    uint16_t max_len_in_line = w/5;
    uint16_t text_lines = len>max_len_in_line ? len/max_len_in_line : 1;
    uint16_t current_line = 0;
    printf("%d\n", len);
    printf("%d\n", max_len_in_line);
    printf("%d\n", text_lines);
    for (uint16_t y = 0; y < text_lines*8; y++){
        current_line = y/8;
        for (uint16_t pix = 0; pix < w; pix++) {
            uint8_t char_in_line = pix/5;
            uint8_t pix_in_char = pix%5;
            if (((font5x8[text[char_in_line+(current_line*max_len_in_line)]-32][pix_in_char] >> y%8) & 1) &&
                (char_in_line<len)
            ){
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = color >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = color >> 16;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = color & 0xFF;
            } else {
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].bkg >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].bkg >> 16;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].bkg & 0xFF;
            }
        }
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_Init(){
    SPI1_Init();
    ST7789_SetConfig();
    ST7789_SetWindow(0, 0, x-1, y-1);
    SD_HighSpeed();
}
