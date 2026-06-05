#ifndef st7789_h
#define st7789_h

#define THRESHOLD_BITS   76800UL
#define BUFFER_SIZE      40

#define MAX_WINDOWS      20

typedef struct {
    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t offsetX1;
    uint16_t offsetY1;
    uint32_t bkg;
    uint32_t col;
} Window;

extern uint32_t buffer[BUFFER_SIZE];

void ST7789_WriteCmd(uint8_t cmd);

void ST7789_WriteData(uint8_t data);

void ST7789_InsertText5x8(uint8_t idx, char* text, uint32_t color);

void ST7789_SetConfig();

void ST7789_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void ST7789_Add_Window(uint16_t offsetX, uint16_t offsetY, uint16_t offsetX1, uint16_t offsetY1, uint32_t color, uint32_t bkg, uint8_t idx);

uint32_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);

uint32_t rgb_to_rgb888(uint8_t r, uint8_t g, uint8_t b);

void sendBuffer();

void updateScreen();

void ST7789_FillQuickTest(uint32_t color);

void ST7789_Init();

#endif
