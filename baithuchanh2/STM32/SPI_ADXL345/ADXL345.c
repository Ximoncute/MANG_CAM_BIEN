#include "ADXL345.h"

// ===== SPI1 Init =====
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    // PA5 = SCK, PA6 = MISO, PA7 = MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA4 = CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    CS_HIGH();

    // SPI1 config: Mode3 (CPOL=1, CPHA=1)
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_Transfer(uint8_t data)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

// ===== ADXL345 =====
uint8_t ADXL345_ReadID(void)
{
    uint8_t id;
    CS_LOW();
    SPI1_Transfer(ADXL345_DEVID | 0x80);  // Read command
    id = SPI1_Transfer(0x00);
    CS_HIGH();
    return id;
}

void ADXL345_Write(uint8_t reg, uint8_t value)
{
    CS_LOW();
    SPI1_Transfer(reg);
    SPI1_Transfer(value);
    CS_HIGH();
}

uint8_t ADXL345_Read(uint8_t reg)
{
    uint8_t val;
    CS_LOW();
    SPI1_Transfer(reg | 0x80);
    val = SPI1_Transfer(0x00);
    CS_HIGH();
    return val;
}

void ADXL345_Init(void)
{
    ADXL345_Write(ADXL345_DATA_FORMAT, 0x0B); // ±16g, full resolution
    ADXL345_Write(ADXL345_POWER_CTL, 0x08);   // Measurement mode
}

void ADXL345_ReadXYZ(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t x0, x1, y0, y1, z0, z1;

    CS_LOW();
    SPI1_Transfer(ADXL345_DATAX0 | 0xC0); // Multi-byte read
    x0 = SPI1_Transfer(0x00);
    x1 = SPI1_Transfer(0x00);
    y0 = SPI1_Transfer(0x00);
    y1 = SPI1_Transfer(0x00);
    z0 = SPI1_Transfer(0x00);
    z1 = SPI1_Transfer(0x00);
    CS_HIGH();

    *x = (int16_t)((x1 << 8) | x0);
    *y = (int16_t)((y1 << 8) | y0);
    *z = (int16_t)((z1 << 8) | z0);
}
