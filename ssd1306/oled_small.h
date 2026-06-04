// ===================================================================================
// SSD1306 128x64 Pixels OLED Terminal Functions                              * v1.5 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an SSD1306 128x64 pixels
// I2C OLED for the display of text in the context of emulating a terminal output.
//
// References:
// -----------
// - Neven Boyanov: https://github.com/tinusaur/ssd1306xled
//
// 2024 by Enol Matilla: https://github.com/rotura

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_tx.h"
#include <stdbool.h>
#include "font5x8.h"
  
// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          32
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           64
#endif

// OLED definitions
#define OLED_ADDR         0x78    // OLED write address (0x3C << 1)
#define OLED_CMD_MODE     0x00    // set command mode
#define OLED_DAT_MODE     0x40    // set data mode

// OLED commands
#define OLED_DISPLAY_OFF   0xAE
#define OLED_DISPLAY_ON    0xAF

#define OLED_CLOCKDIV      0xD5
#define OLED_MULTIPLEX     0xA8
#define OLED_OFFSET        0x00
#define OLED_STARTLINE     0x40
#define OLED_CHARGEPUMP    0x8D
#define OLED_MEMORYMODE    0x20
#define OLED_XFLIP_OFF     0xA0
#define OLED_XFLIP         0xA1
#define OLED_YFLIP_OFF     0xC0
#define OLED_YFLIP         0xC8
#define OLED_COMPINS       0xDA
#define OLED_CONTRAST      0x81
#define OLED_PRECHARGE     0xD9
#define OLED_VCOMH         0xDB
#define OLED_DISPLAY_RESUME 0xA4
#define OLED_INVERT_OFF    0xA6
#define OLED_INVERT        0xA7
#define OLED_SCROLL_DEACTIVATE 0x2E
#define OLED_COLUMNS       0x21
#define OLED_PAGES         0x22

#define OLED_COLUMN_LOW    0x00   // lower nibble of start column
#define OLED_COLUMN_HIGH   0x10   // higher nibble of start column
#define OLED_PAGE          0xB0

// OLED functions
void OLED_init(void);                       		// OLED init function
void OLED_clear(void);                      		// OLED clear screen
void OLED_write(char c, bool inverted);     		// OLED write a character or handle control characters
void OLED_print(char* str);                 		// OLED print string
void OLED_printS(char* str, bool inverted); 		// OLED print string
void OLED_println(char* str, bool inverted);        // OLED print string with newline
void OLED_printD(uint32_t value, bool inverted);    // Print decimal value
void OLED_printL(uint32_t value, bool inverted);    // Print hex long value
void OLED_printW(uint16_t value, bool inverted);    // Print hex word value
void OLED_printB(uint8_t value, bool inverted);     // Print hex byte value
void OLED_fill(uint8_t p);                  		// Fill OLED with a character
void OLED_setpos(uint16_t x, uint16_t y);     		// Set OLED cursor
void OLED_fill(uint8_t p);                  		// Fill OLED with a character
void OLED_DrawBitmap(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, const uint8_t* bmp, bool inverted); // Draw a bitmap

#ifdef __cplusplus
};
#endif
