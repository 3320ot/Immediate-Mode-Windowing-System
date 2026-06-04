#include "ch32v00x.h"

extern GPIO_TypeDef* SD_CD_PORT; //CD port (GPIOx)
extern uint16_t      SD_CD_PIN; //CD pin (GPIO_Pin_x)

#define SCK GPIO_Pin_5
#define MOSI GPIO_Pin_6

#define BLK GPIO_Pin_3
#define DC  GPIO_Pin_2
#define RES GPIO_Pin_7

void SPI1_Init(void);

void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);

uint8_t   SPI_TransferByte(uint8_t TxData);

void SD_LowSpeed();

void SD_HighSpeed();
