#include "debug.h"
#include "ssd1306/oled_small.h"
#include "ST7789/ST7789.h"
#include "gui/ui.h"

volatile uint16_t systick_counter = 0;
volatile uint8_t i2c_send_request = 0;
char RxData[6];

Font FONT5x8 = {
    .weight = 5,
    .height = 8,
    .idx = 0
};
Font FONT9x16 = {
    .weight = 9,    
    .height = 16,
    .idx = 1 
};
Font FONT11x16 = {
    .weight = 11,    
    .height = 16,
    .idx = 2 
};

void TIM2_Init(void) {
    RCC->APB1PCENR |= RCC_TIM2EN;
    TIM2->INTFR = 0;
    TIM2->PSC = 48000 - 1;
    TIM2->ATRLR = 1000 - 1;
    TIM2->DMAINTENR |= TIM_UIE;
    TIM2->CTLR1 |= TIM_CEN;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void seconds_to_time_str(uint16_t total_seconds, char *buf) {
    uint16_t minutes = total_seconds / 60;   
    uint16_t seconds = total_seconds % 60;  

    buf[0] = '0' + (minutes / 10);           
    buf[1] = '0' + (minutes % 10);           
    buf[2] = ':';                            
    buf[3] = '0' + (seconds / 10);          
    buf[4] = '0' + (seconds % 10);           
    buf[5] = '\0';                          
    i2c_send_request = 1;
}

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) {
    if (TIM2->INTFR & TIM_UIF) {    
        TIM2->INTFR &= ~TIM_UIF;
        seconds_to_time_str(systick_counter, RxData);
        systick_counter++;
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms(1000);
    SDI_Printf_Enable();

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    TIM2_Init();

    OLED_init();
    OLED_setpos(0, 0);
    OLED_println("SSD1306 Initialized", 0);
    OLED_println("ST7789 Initialization...", 0);
    
    ST7789_Init();     
    OLED_clear(); 
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
    ST7789_Add_Window(2, offset2, rgb888_to_rgb565(249, 24, 24), rgb888_to_rgb565(220, 220, 220), 4);
    
    ST7789_InsertText(0, "FAT CAT",
        rgb888_to_rgb565(255, 0, 0), 
        10, FONT11x16);
    ST7789_InsertText(1, "CH32V003 series is an industrial-grade general-purpose microcontroller designed based on QingKe RISC-V2A core, which supports 48MHz system main frequency in the product function. The series features wide voltage,single-wire serial debug interface, low-power consumption and ultra-small package. ", 
        rgb888_to_rgb565(235, 235, 0), 
        10, FONT5x8);
    ST7789_InsertText(2, "blank",
        rgb888_to_rgb565(255, 0, 0), 
        10, FONT9x16);

    while(1) {
        if (i2c_send_request) {
            OLED_setpos(20, 20);
            OLED_printS("FAKE_RTC:", 0);
            OLED_println(RxData, 0);
        }
    }
}
