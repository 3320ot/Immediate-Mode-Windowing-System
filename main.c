#include "debug.h"
#include "ST7789/ST7789.h"
#include "DS1302Z/ds1302z.h"
#include "gui/gui.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    SDI_Printf_Enable();
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("This is printf example\r\n");

    ST7789_Init();
    DS1302_Init();
    ST7789_Clear();

    Offsets offset0 = {
        0, 0, 32, 480};
    Offsets offset1 = {
        42, 135, 310, 470};
    Offsets offset2 = {
        230, 0, 310, 24};
    Offsets offset3 = {
        42, 30, 310, 125};

    ST7789_Add_Window(0, offset0, rgb888_to_rgb565(255, 255, 255), rgb888_to_rgb565(20, 20, 20), 6);
    ST7789_Add_Window(1, offset1, rgb888_to_rgb565(255, 24, 0), rgb888_to_rgb565(0, 0, 0), 4);
    ST7789_Add_Window(2, offset2, background_color, background_color, 0);
    ST7789_Add_Window(3, offset3, rgb888_to_rgb565(255, 255, 255), rgb888_to_rgb565(20, 20, 20), 3);

    ST7789_InsertText(0, "FAT CAT",
        rgb888_to_rgb565(255, 0, 0),
        10, FONT11x16);
    ST7789_InsertText(1, "CH32V series are industrial-grade general-purpose microcontrollers designed based on QingKe 32-bit RISCV. The whole series of products into the hardware stack area, fast interrupt entry and other designs, compared to the standard greatly improved the interrupt response speed. CH32V203 is based on 32-bit RISC-V core design of industrial-grade enhanced low-power general-purpose microcontrollers, high-performance, in the product features support 144MHz main frequency zero-wait operation, equipped with V4B core, work and sleep power consumption significantly reduced year-on-year. CH32V203 series integrated dual USB interface, support USB Host and USB Device function, with 1 CAN interface (2.0B active), dual OPA, 4 groups of USART, dual I2C, 12-bit ADC, 10-way Touchkey and other rich peripheral resources.",
        rgb888_to_rgb565(235, 235, 0),
        10, FONT5x8);
    ST7789_InsertText(2, "RTC clock",
        rgb888_to_rgb565(255, 230, 250),
        4, FONT9x16);

    Time t;
    char buffer[32];
    while(1){
//        t = DS1302_GetTime();
        Delay_Ms(100);
        sprintf(buffer, "%02d:%02d:%02d",
                t.hour, t.minute, t.second);
        ST7789_ChangeText(2, buffer);
        ST7789_UpdateGraph(3);
    }
}
