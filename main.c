#include "gui/gui.h"
#include "debug.h"
#include "ST7789/ST7789.h"
#include "DS1302Z/ds1302z.h"

int main(void){
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    SDI_Printf_Enable();
    Delay_Ms(100);

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Ms(100);
    
    ST7789_Init();
    DS1302_Init();

    ST7789_SetBrightness(1000);
    ST7789_Clear();
    
    Offsets offset0 = {
        0, 0, 32, 240};
    Offsets offset1 = {
        42, 135, 230, 230};
    Offsets offset2 = {
        160, 0, 240, 24};
    Offsets offset3 = {
        42, 30, 230, 125};

    ST7789_Add_Window(0, offset0, rgb888_to_rgb565(255, 255, 255), rgb888_to_rgb565(20, 20, 20), 6);
    ST7789_Add_Window(1, offset1, rgb888_to_rgb565(7, 24, 24), rgb888_to_rgb565(160, 0, 160), 4);
    ST7789_Add_Window(2, offset2, background_color, background_color, 0);
    ST7789_Add_Window(3, offset3, rgb888_to_rgb565(255, 255, 255), rgb888_to_rgb565(20, 20, 20), 3);
    
    ST7789_InsertText(0, "FAT CAT",
        rgb888_to_rgb565(255, 0, 0), 
        10, FONT11x16);
    ST7789_InsertText(1, "CH32V003 series is an industrial-grade general-purpose microcontroller designed based on QingKe RISC-V2A core, which supports 48MHz system main frequency in the product function. The series features wide voltage,single-wire serial debug interface, low-power consumption and ultra-small package.",
        rgb888_to_rgb565(235, 235, 0), 
        10, FONT5x8);
    ST7789_InsertText(2, "RTC clock",
        rgb888_to_rgb565(255, 230, 250),
        4, FONT9x16);

    ST7789_EnableGraph(3);

    Time t;
    char buffer[32];

    while(1) {
        t = DS1302_GetTime();
        sprintf(buffer, "%02d:%02d:%02d",
                t.hour, t.minute, t.second);
        ST7789_ChangeText(2, buffer);

        if (adc_update_pending) {
            ST7789_UpdateGraph(3);

            DMA1_Channel1->CNTR = ADC_BUFFER_SIZE;
            DMA_ClearITPendingBit(DMA1_IT_TC1);
            DMA_Cmd(DMA1_Channel1, ENABLE);
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);

            adc_update_pending = 0;
        }
    }
}
