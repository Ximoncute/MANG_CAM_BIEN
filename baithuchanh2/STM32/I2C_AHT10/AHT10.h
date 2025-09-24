#ifndef __AHT10_H
#define __AHT10_H

#include <stdint.h>

void AHT10_Init(void);
uint8_t AHT10_Read(float *temp, float *hum);

#endif
