#ifndef __ADXL345_H
#define __ADXL345_H

#include "stm32f10x.h"

// Ð?a ch? thanh ghi ADXL345
#define ADXL345_DEVID        0x00
#define ADXL345_POWER_CTL    0x2D
#define ADXL345_DATAX0       0x32
#define ADXL345_DATA_FORMAT  0x31

// CS = PA4
#define CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

void SPI1_Init(void);
uint8_t SPI1_Transfer(uint8_t data);

void ADXL345_Init(void);
uint8_t ADXL345_ReadID(void);
void ADXL345_ReadXYZ(int16_t *x, int16_t *y, int16_t *z);

#endif
