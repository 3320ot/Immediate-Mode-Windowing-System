#include "ch32v00x.h"
#include "ui.h"
#include "ST7789/ST7789.h"
#include "ST7789/spi.h"
#include <stdlib.h>
#include <string.h>

Window windows[MAX_WINDOWS];

#define SPI_BUSY_WAIT() while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY))
#define SPI_TXE_WAIT()  while (!(SPI1->STATR & SPI_I2S_FLAG_TXE))

static inline void spi_send_color(uint16_t color) {
    SPI_TXE_WAIT();
    SPI1->DATAR = color >> 8;
    SPI_TXE_WAIT();
    SPI1->DATAR = color & 0xFF;
}

static void fill_window(uint16_t w, uint16_t h, uint16_t color) {
    for (uint32_t i = 0; i < (uint32_t)w * h; i++)
        spi_send_color(color);
    SPI_BUSY_WAIT();
}

static void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    GPIO_ResetBits(GPIOD, DC);
    SD_LowSpeed();
    ST7789_SetWindow(x0, y0, x1, y1);
    SD_HighSpeed();
    GPIO_SetBits(GPIOD, DC);
}

static uint16_t get_font_pixel(uint8_t font_idx, uint8_t sym, uint8_t col, uint8_t row) {
    switch (font_idx) {
        case 0: return (uint16_t)(font5x8[sym][col] >> (row % 8));
        case 1: return font9x16[sym][col] >> (row % 16);
        case 2: return font11x16[sym][col] >> (row % 16);
        default: return 0;
    }
}

void ST7789_FillQuickTest(uint32_t color) {
    GPIO_SetBits(GPIOD, DC);
    fill_window(x, y, (uint16_t)color);
}

void ST7789_Clear(void) {
    GPIO_SetBits(GPIOD, DC);
    fill_window(x, y, background_color);
}

void ST7789_Add_Window(uint8_t idx, Offsets off, uint32_t color, uint32_t bkg, uint8_t rim) {
    set_window(off.offsetX, off.offsetY, off.offsetX1 - 1, off.offsetY1 - 1);
    fill_window(off.offsetX1 - off.offsetX, off.offsetY1 - off.offsetY, (uint16_t)color);

    set_window(off.offsetX + rim, off.offsetY + rim,
                          off.offsetX1 - 1 - rim, off.offsetY1 - 1 - rim);
    fill_window(off.offsetX1 - off.offsetX - 2 * rim,
                off.offsetY1 - off.offsetY - 2 * rim, (uint16_t)bkg);

    windows[idx].window_offsets = off;
    windows[idx].bkg = (uint16_t)bkg;
    windows[idx].col = (uint16_t)color;
}

void ST7789_SetOrienaionAndFormat(uint8_t orientation, uint8_t format) {
    ST7789_WriteCmd(0x36);
    ST7789_WriteData(orientation | format);
}

void ST7789_InsertText(uint8_t idx, char *text, uint32_t color, uint8_t offset, Font font) {
    Window *win = &windows[idx];
    uint16_t inner_w = win->window_offsets.offsetX1 - win->window_offsets.offsetX - 2 * offset;
    uint16_t inner_h = win->window_offsets.offsetY1 - win->window_offsets.offsetY - 2 * offset;

    uint16_t len = strlen(text);
    uint16_t chars_per_line = inner_w / font.weight;
    uint16_t max_lines = inner_h / font.height;
    uint16_t lines = (len > chars_per_line) ? (len / chars_per_line + 1) : 1;

    win->config.offset = offset;
    win->config.color = (uint16_t)color;
    win->config.font = font;

    size_t alloc = (max_lines > len) ? (max_lines * 2) : (len * 2);
    win->text = (char *)malloc(alloc);
    memset(win->text, ' ', alloc);
    strcpy(win->text, text);

    set_window(win->window_offsets.offsetX + offset,
                          win->window_offsets.offsetY + offset,
                          win->window_offsets.offsetX1 - 1 - offset,
                          win->window_offsets.offsetY1 - 1 - offset);

    for (uint16_t row = 0; row < lines * font.height; row++) {
        uint16_t line = row / font.height;
        for (uint16_t col = 0; col < inner_w; col++) {
            uint8_t ch = col / font.weight;
            uint8_t pix_in_ch = col % font.weight;
            char sym = win->text[ch + line * chars_per_line] - 32;
            uint16_t data = get_font_pixel(font.idx, sym, pix_in_ch, row);

            if ((data & 1) && (ch < len) && (ch < chars_per_line))
                spi_send_color(win->config.color);
            else
                spi_send_color(win->bkg);
        }
    }
    SPI_BUSY_WAIT();
}

void ST7789_ScrollText(uint8_t idx, uint16_t pos) {
    Window *win = &windows[idx];
    uint16_t inner_w = win->window_offsets.offsetX1 - win->window_offsets.offsetX - 2 * win->config.offset;

    uint16_t len = strlen(win->text);
    Font font = win->config.font;
    uint16_t chars_per_line = inner_w / font.weight;
    uint16_t lines = (len > chars_per_line) ? (len / chars_per_line + 1) : 1;

    set_window(win->window_offsets.offsetX + win->config.offset,
                          win->window_offsets.offsetY + win->config.offset,
                          win->window_offsets.offsetX1 - 1 - win->config.offset,
                          win->window_offsets.offsetY1 - 1 - win->config.offset);

    for (uint16_t row = 0; row < lines * font.height; row++) {
        uint16_t line = row / font.height + pos;
        for (uint16_t col = 0; col < inner_w; col++) {
            uint8_t ch = col / font.weight;
            uint8_t pix_in_ch = col % font.weight;
            char sym = win->text[ch + line * chars_per_line] - 32;
            uint16_t data = get_font_pixel(font.idx, sym, pix_in_ch, row);

            if ((data & 1) && (ch < len) && (ch < chars_per_line))
                spi_send_color(win->config.color);
            else
                spi_send_color(win->bkg);
        }
    }
    SPI_BUSY_WAIT();
}

void ST7789_ChangeText(uint8_t idx, char *text) {
    Window *win = &windows[idx];
    uint16_t inner_w = win->window_offsets.offsetX1 - win->window_offsets.offsetX - 2 * win->config.offset;
    uint16_t inner_h = win->window_offsets.offsetY1 - win->window_offsets.offsetY - 2 * win->config.offset;

    uint16_t len = strlen(text);
    uint16_t chars_per_line = inner_w / win->config.font.weight;
    uint16_t max_lines = inner_h / win->config.font.height;
    uint16_t lines = (len > chars_per_line) ? (len / chars_per_line + 1) : 1;

    size_t alloc = (max_lines > len) ? (max_lines * 2) : (len * 2);
    free(win->text);
    win->text = (char *)malloc(alloc);
    memset(win->text, ' ', alloc);
    strcpy(win->text, text);

    set_window(win->window_offsets.offsetX + win->config.offset,
                          win->window_offsets.offsetY + win->config.offset,
                          win->window_offsets.offsetX1 - 1 - win->config.offset,
                          win->window_offsets.offsetY1 - 1 - win->config.offset);

    for (uint16_t row = 0; row < max_lines * win->config.font.height; row++) {
        uint16_t line = row / win->config.font.height;
        for (uint16_t col = 0; col < inner_w; col++) {
            uint8_t ch = col / win->config.font.weight;
            uint8_t pix_in_ch = col % win->config.font.weight;
            char sym = win->text[ch + line * chars_per_line] - 32;
            uint16_t data = get_font_pixel(win->config.font.idx, sym, pix_in_ch, row);

            if ((data & 1) && (ch < len) && (ch < chars_per_line) && (line < lines))
                spi_send_color(win->config.color);
            else
                spi_send_color(win->bkg);
        }
    }
    SPI_BUSY_WAIT();
}

void ST7789_RemoveWindow(uint8_t idx) {
    Window *win = &windows[idx];
    set_window(win->window_offsets.offsetX, win->window_offsets.offsetY,
                          win->window_offsets.offsetX1, win->window_offsets.offsetY1);
    fill_window(win->window_offsets.offsetX1 - win->window_offsets.offsetX,
                win->window_offsets.offsetY1 - win->window_offsets.offsetY,
                background_color);
    free(win->text);
}
