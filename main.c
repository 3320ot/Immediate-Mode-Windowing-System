#include "debug.h"
#include "ST7789/ST7789.h"
#include "gui/ui.h"
#include "DS1302Z/ds1302z.h"

char RxData[6];

void seconds_to_time_str(uint16_t total_seconds, char *buf) {
    uint16_t minutes = total_seconds / 60;   
    uint16_t seconds = total_seconds % 60;  

    buf[0] = '0' + (minutes / 10);           
    buf[1] = '0' + (minutes % 10);           
    buf[2] = ':';                            
    buf[3] = '0' + (seconds / 10);          
    buf[4] = '0' + (seconds % 10);           
    buf[5] = '\0';
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    SDI_Printf_Enable();

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Ms(1000);

    DS1302_Init();
    
    ST7789_Init();     
    ST7789_Clear();
    
    Offsets offsets = {
        0, 0, 32, 239
    };
    Offsets offsets1 = {
        42, 10, 230, 115
    };
    Offsets offset2 = {
        42, 125, 230, 230
    };

    ST7789_Add_Window(0, offsets, rgb888_to_rgb565(255, 255, 255), rgb888_to_rgb565(20, 20, 20), 6);
    ST7789_Add_Window(1, offsets1, rgb888_to_rgb565(7, 24, 24), rgb888_to_rgb565(160, 0, 160), 4);
    ST7789_Add_Window(2, offset2, rgb888_to_rgb565(249, 24, 222), rgb888_to_rgb565(220, 220, 220), 4);
    
    ST7789_InsertText(0, "FAT CAT",
        rgb888_to_rgb565(255, 0, 0), 
        10, FONT11x16);
    ST7789_InsertText(1, "CH32V003 series is an industrial-grade general-purpose microcontroller designed based on QingKe RISC-V2A core, which supports 48MHz system main frequency in the product function. The series features wide voltage,single-wire serial debug interface, low-power consumption and ultra-small package.", 
        rgb888_to_rgb565(235, 235, 0), 
        10, FONT5x8);
    ST7789_InsertText(2, "Reserved space for long text test",
        rgb888_to_rgb565(255, 0, 0), 
        10, FONT9x16);

    uint16_t pos = 0;
    uint16_t b = 1000;

    ST7789_ChangeText(2, "Here was a text");
    Delay_Ms(1000);

    while(1) {
        pos = pos<8 ? pos+1 : 0;
        b = b>0 ? b-25 : 1000;
        ST7789_ScrollText(1, pos);
        ST7789_SetBrightness(b);
    }
}
