#include "ch32v20x.h"

#define SCK GPIO_Pin_5
#define MOSI GPIO_Pin_7
#define MISO GPIO_Pin_6

#define BLK GPIO_Pin_11
#define CS GPIO_Pin_10
#define DC  GPIO_Pin_1
#define RES GPIO_Pin_0

void SPI1_Init(void);

void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);

uint8_t   SPI_TransferByte(uint8_t TxData);

void SD_LowSpeed();

void SD_HighSpeed();
