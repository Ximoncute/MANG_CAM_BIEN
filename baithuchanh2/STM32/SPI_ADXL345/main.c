#include "stm32f10x.h"
#include "ADXL345.h"

int16_t x, y, z;
uint8_t devid;

int main(void)
{
    SystemInit();
    SPI1_Init();
    ADXL345_Init();

    devid = ADXL345_ReadID();  // Ph?i = 0xE5

    while (1)
    {
        ADXL345_ReadXYZ(&x, &y, &z);
        // Debug: xem x, y, z, devid trong Watch Window
    }
}
