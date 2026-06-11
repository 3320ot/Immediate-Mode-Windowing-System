#include "ch32v00x.h"
#include "ui.h"
#include "ST7789/ST7789.h"
#include "ST7789/spi.h"
#include <stdlib.h>
#include <string.h>

Window windows[MAX_WINDOWS];

void ST7789_FillQuickTest(uint32_t color) {
    GPIO_SetBits(GPIOD, DC);
    for(uint32_t i = 0; i < x*y; i++)
    {
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR =  color >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color & 0xFF;
    }

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_Clear() {
    GPIO_SetBits(GPIOD, DC);
    for(uint32_t i = 0; i < x*y; i++)
    {
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR =  background_color >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = background_color & 0xFF;
    }

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_Add_Window(
    uint8_t idx,
    Offsets window_offsets, 
    uint32_t color,
    uint32_t bkg,
    uint8_t rim_w
){
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(window_offsets.offsetX, 
        window_offsets.offsetY, 
        window_offsets.offsetX1-1, 
        window_offsets.offsetY1-1);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w = window_offsets.offsetX1-window_offsets.offsetX;
    uint16_t h = window_offsets.offsetY1-window_offsets.offsetY;
    for (uint32_t pix = 0; pix < w*h; pix++){
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR =  color >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = color & 0xFF;
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));

    SD_LowSpeed();
    ST7789_SetWindow(window_offsets.offsetX+rim_w, 
        window_offsets.offsetY+rim_w, 
        window_offsets.offsetX1-1-rim_w, 
        window_offsets.offsetY1-1-rim_w);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w1 = window_offsets.offsetX1-window_offsets.offsetX;
    uint16_t h1 = window_offsets.offsetY1-window_offsets.offsetY;
    for (uint32_t pix = 0; pix < w1*h1; pix++){
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR =  bkg >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = bkg & 0xFF;
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));

    windows[idx].window_offsets.offsetX = window_offsets.offsetX;
    windows[idx].window_offsets.offsetY = window_offsets.offsetY;
    windows[idx].window_offsets.offsetX1 = window_offsets.offsetX1;
    windows[idx].window_offsets.offsetY1 = window_offsets.offsetY1;
    windows[idx].bkg = bkg;
    windows[idx].col = color;
}

void ST7789_SetOrienaionAndFormat(uint8_t orienation, uint8_t format){
    ST7789_WriteCmd(0x36);
    ST7789_WriteData(orienation | format);
}

void ST7789_InsertText(uint8_t idx, char* text, uint32_t color, uint8_t offset, Font font){
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(windows[idx].window_offsets.offsetX+offset, 
        windows[idx].window_offsets.offsetY+offset, 
        windows[idx].window_offsets.offsetX1-1-offset, 
        windows[idx].window_offsets.offsetY1-1-offset);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w = windows[idx].window_offsets.offsetX1 - windows[idx].window_offsets.offsetX - offset*2;
    uint16_t h = windows[idx].window_offsets.offsetY1 - windows[idx].window_offsets.offsetY - offset*2;

    uint16_t len = 0;
    while(text[len]) len++;
    uint16_t max_len_in_line = w/font.weight;
    uint16_t max_len = h/font.height;
    uint16_t text_lines = len>=max_len_in_line ? len/max_len_in_line+1 : 1;
    uint16_t current_line = 0;

    printf("%d\n", len);
    printf("%d\n", max_len_in_line);
    printf("%d\n", text_lines);

    windows[idx].config = (TextConfig){
        .offset = offset,
        .color = color,
        .font = font
    };
    uint16_t size = max_len>len ? max_len*2 : len*2;
    windows[idx].text = (char*)malloc(size);
    memset(windows[idx].text, (char)32, size);
    strcpy(windows[idx].text, text);

    for (uint16_t line_p = 0; line_p < text_lines*font.height; line_p++){
        current_line = line_p/font.height;
        for (uint16_t pix = 0; pix < w; pix++) {
            uint8_t char_in_line = pix/font.weight;
            uint8_t pix_in_char = pix%font.weight;
            char symbol_idx = windows[idx].text[char_in_line+(current_line*max_len_in_line)]-32;
            uint16_t data;
            switch(font.idx)
            {
                case 0: data = (uint16_t)(font5x8[symbol_idx][pix_in_char] >> line_p%font.height);
                    break;
                case 1: data = font9x16[symbol_idx][pix_in_char] >> line_p%font.height;
                    break;
                case 2: data = font11x16[symbol_idx][pix_in_char] >> line_p%font.height;
                    break;
                default:
                    return;
            }

            if ((data & 1) &&
                (char_in_line < len) &&
                (char_in_line < max_len_in_line)
            ){
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR =  color >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = color & 0xFF;
            } else {    
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR =  windows[idx].bkg >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].bkg & 0xFF;
            }
        }
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_ScrollText(uint8_t idx, uint16_t pos){
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(windows[idx].window_offsets.offsetX+windows[idx].config.offset, 
        windows[idx].window_offsets.offsetY+windows[idx].config.offset, 
        windows[idx].window_offsets.offsetX1-1-windows[idx].config.offset, 
        windows[idx].window_offsets.offsetY1-1-windows[idx].config.offset);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w = windows[idx].window_offsets.offsetX1 - windows[idx].window_offsets.offsetX - windows[idx].config.offset*2;
    // uint16_t h = windows[idx].window_offsets.offsetY1 - windows[idx].window_offsets.offsetY - windows[idx].config.offset*2;
    
    uint16_t len = 0;
    while(windows[idx].text[len]) len++;
    Font font = windows[idx].config.font;
    uint16_t max_len_in_line = w/font.weight;
    uint16_t text_lines = len>=max_len_in_line ? len/max_len_in_line+1 : 1;
    uint16_t current_line = 0;

    for (uint16_t line_p = 0; line_p < text_lines*font.height; line_p++){
        current_line = line_p/font.height+pos;
        for (uint16_t pix = 0; pix < w; pix++) {
            uint8_t char_in_line = pix/font.weight;
            uint8_t pix_in_char = pix%font.weight;
            char symbol_idx = windows[idx].text[char_in_line+(current_line*max_len_in_line)]-32;
            uint16_t data;
            switch(font.idx){
                case 0: data = (uint16_t)(font5x8[symbol_idx][pix_in_char] >> line_p%font.height);
                    break;
                case 1: data = font9x16[symbol_idx][pix_in_char] >> line_p%font.height;
                    break;
                case 2: data = font11x16[symbol_idx][pix_in_char] >> line_p%font.height;
                    break;
                default:
                    return;
            }

            if ((data & 1) &&
                (char_in_line < len) &&
                (char_in_line < max_len_in_line)
            ){
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].config.color >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].config.color & 0xFF;
            } else {    
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR =  windows[idx].bkg >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].bkg & 0xFF;
            }
        }
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_ChangeText(uint8_t idx, char* text){
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(windows[idx].window_offsets.offsetX+windows[idx].config.offset, 
        windows[idx].window_offsets.offsetY+windows[idx].config.offset, 
        windows[idx].window_offsets.offsetX1-1-windows[idx].config.offset, 
        windows[idx].window_offsets.offsetY1-1-windows[idx].config.offset);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w = windows[idx].window_offsets.offsetX1 - windows[idx].window_offsets.offsetX - 
        windows[idx].config.offset*2;
    uint16_t h = windows[idx].window_offsets.offsetY1 - windows[idx].window_offsets.offsetY - 
        windows[idx].config.offset*2;

    uint16_t len = 0;
    while(text[len]) len++;
    uint16_t max_len_in_line = w/windows[idx].config.font.weight;
    uint16_t max_len = h/windows[idx].config.font.height;
    uint16_t text_lines = len>=max_len_in_line ? len/max_len_in_line+1 : 1;
    uint16_t current_line = 0;
    uint16_t size = max_len>len ? max_len*2 : len*2;
    free(windows[idx].text);
    windows[idx].text = (char*)malloc(size);
    memset(windows[idx].text, (char)32, size);
    strcpy(windows[idx].text, text);

    for (uint16_t line_p = 0; line_p < max_len*windows[idx].config.font.height; line_p++){
        current_line = line_p/windows[idx].config.font.height;
        for (uint16_t pix = 0; pix < w; pix++) {
            uint8_t char_in_line = pix/windows[idx].config.font.weight;
            uint8_t pix_in_char = pix%windows[idx].config.font.weight;
            char symbol_idx = windows[idx].text[char_in_line+(current_line*max_len_in_line)]-32;
            uint16_t data;
            switch(windows[idx].config.font.idx)
            {
                case 0: data = (uint16_t)(font5x8[symbol_idx][pix_in_char] >> line_p%windows[idx].config.font.height);
                    break;
                case 1: data = font9x16[symbol_idx][pix_in_char] >> line_p%windows[idx].config.font.height;
                    break;
                case 2: data = font11x16[symbol_idx][pix_in_char] >> line_p%windows[idx].config.font.height;
                    break;
                default:
                    return;
            }

            if ((data & 1) &&
                (char_in_line < len) &&
                (char_in_line < max_len_in_line) && 
                (current_line < text_lines)
            ){
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR =  windows[idx].config.color >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].config.color & 0xFF;
            } else {    
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR =  windows[idx].bkg >> 8;
                while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
                SPI1->DATAR = windows[idx].bkg & 0xFF;
            }
        }
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
}

void ST7789_RemoveWindow(uint8_t idx){
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(windows[idx].window_offsets.offsetX, windows[idx].window_offsets.offsetY, 
        windows[idx].window_offsets.offsetX1, windows[idx].window_offsets.offsetY1);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
    uint16_t w = windows[idx].window_offsets.offsetX1-windows[idx].window_offsets.offsetX;
    uint16_t h = windows[idx].window_offsets.offsetY1-windows[idx].window_offsets.offsetY;

    free(windows[idx].text);

    for(uint32_t i = 0; i < w*h; i++)
    {
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR =  background_color >> 8;
        while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
        SPI1->DATAR = background_color & 0xFF;
    }

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));   
}