#ifndef ui_h
#define ui_h

#include "fonts/font5x8.h"
#include "fonts/font9x16.h"
#include "fonts/font11x16.h"

#define MAX_WINDOWS      20

static uint32_t background_color = 0x2472;

typedef struct {
    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t offsetX1;
    uint16_t offsetY1;
} Offsets;

typedef struct {
    Offsets window_offsets;
    uint32_t bkg;
    uint32_t col;
    char *text;
} Window;

typedef struct {
    uint8_t weight;      
    uint8_t height;      
    uint8_t idx;   
} Font;

void ST7789_InsertText(uint8_t idx, char* text, uint32_t color, uint8_t offset, Font font);

void ST7789_RemoveWindow(uint8_t idx);

void ST7789_Add_Window(uint8_t idx, 
    Offsets offsets, uint32_t color, 
    uint32_t bkg, uint8_t rim_w);

uint32_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);

uint32_t rgb_to_rgb888(uint8_t r, uint8_t g, uint8_t b);

void ST7789_FillQuickTest(uint32_t color);

void ST7789_Clear();

#endif