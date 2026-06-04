#include "debug.h"
#include "ssd1306/oled_small.h"
#include "ST7789/ST7789.h"

volatile uint16_t systick_counter = 0;
volatile uint8_t i2c_send_request = 0;
char RxData[6];

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
    OLED_println("Check data:", 0);
    OLED_println("EMPTY", 0);
    
    ST7789_Init();      

    OLED_clear();
    int color = 0;
    ST7789_FillQuickTest(rgb_to_rgb888(0, 0, 0));
    ST7789_Add_Window(20, 20, 120, 120, rgb_to_rgb888(255, 255,255), rgb_to_rgb888(160, 120, 160), 0);
    ST7789_Add_Window(20, 130, 239, 222, rgb_to_rgb888(7, 172,24), rgb_to_rgb888(160, 120, 160), 1);
    Delay_Ms(3000);

    while(1) {
        ST7789_FillQuickTest(rgb_to_rgb888(color, 255-color, color*0.8));
        color = (color < 255) ? (color + 10) : 0;
        if(!color)
            printf("STOP\n");
        if (i2c_send_request) {
            OLED_setpos(20, 20);
            OLED_printS("FAKE_RTC:", 0);
            OLED_println(RxData, 0);
        }
    }
}
