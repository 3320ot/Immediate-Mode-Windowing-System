#ifndef st7789_h
#define st7789_h

#define THRESHOLD_BITS   76800UL
#define BUFFER_SIZE      40

#define VERTICAL         0x00
#define HORIZONTAL       0x60
#define VERICAL_inv      0xC0
#define HORIZONTAL_inv   0xA0 
#define RGB              0x00
#define BGR              0x08

#define x                240
#define y                240

extern uint32_t buffer[BUFFER_SIZE];

void ST7789_WriteCmd(uint8_t cmd);

void ST7789_WriteData(uint8_t data);

void ST7789_SetConfig();

void ST7789_SetOrienaion(uint8_t orienation, uint8_t format);

void ST7789_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void sendBuffer();

void updateScreen();

void ST7789_Init();

#endif
