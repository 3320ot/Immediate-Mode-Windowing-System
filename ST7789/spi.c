#include "spi.h"
#include "ST7789.h"

void SD_LowSpeed()
{
    SPI_Cmd(SPI1, DISABLE);
    SPI1->CTLR1 &= 0xFFC7;
    SPI1->CTLR1 |= SPI_BaudRatePrescaler_128;
    SPI_Cmd(SPI1, ENABLE);
}

void SD_HighSpeed()
{
    SPI_Cmd(SPI1, DISABLE);
    SPI1->CTLR1 &= 0xFFC7;
    SPI1->CTLR1 |= SPI_BaudRatePrescaler_2;
    SPI_Cmd(SPI1, ENABLE);
}

void SPI1_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | 
        RCC_APB2Periph_GPIOD  | 
        RCC_APB2Periph_SPI1, 
        ENABLE);

    GPIO_InitStructure.GPIO_Pin = SCK | MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = RES;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DC;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);

    GPIO_ResetBits(GPIOC, RES);
    Delay_Ms(10);
    GPIO_SetBits(GPIOC, RES);
    Delay_Ms(150);

//    SPI_TransferByte(0xff);
}

void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    SPI_InitTypeDef SPI_InitStructure = {0};
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;
    SPI_Init(SPI1, &SPI_InitStructure);
}

uint8_t SPI_TransferByte(uint8_t data)
{
    GPIO_ResetBits(GPIOD, DC);

    uint8_t retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        retry++;
        if(retry>200)return 0;
    }
    SPI_I2S_SendData(SPI1, data);
    retry=0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if(retry>200)return 0;
    }

    return SPI_I2S_ReceiveData(SPI1);
}


