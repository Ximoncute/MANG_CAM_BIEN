#include "stm32f10x.h"
#include "AHT10.h"

// Chân I2C bit-bang (PB6=SCL, PB7=SDA)
#define SCL_H() GPIOB->BSRR = GPIO_BSRR_BS6
#define SCL_L() GPIOB->BSRR = GPIO_BSRR_BR6
#define SDA_H() GPIOB->BSRR = GPIO_BSRR_BS7
#define SDA_L() GPIOB->BSRR = GPIO_BSRR_BR7
#define SDA_IN()  {GPIOB->CRL &= ~(GPIO_CRL_MODE7); GPIOB->CRL |= GPIO_CRL_CNF7_0;}   // Input floating
#define SDA_OUT() {GPIOB->CRL &= ~(GPIO_CRL_CNF7);  GPIOB->CRL |= GPIO_CRL_MODE7_0;} // Output 10MHz

#define AHT10_ADDR 0x70   // 0x38 << 1

static void delay_us(uint32_t t) {
    for(uint32_t i=0;i<t*8;i++) __NOP();   // chinh cho F_CPU=72MHz
}

// I2C bit-banging
void I2C_Delay(void){ delay_us(5); }

void I2C_Start(void){
    SDA_OUT(); SDA_H(); SCL_H(); I2C_Delay();
    SDA_L(); I2C_Delay();
    SCL_L(); 
}
void I2C_Stop(void){
    SDA_OUT();
    SCL_L(); SDA_L(); I2C_Delay();
    SCL_H(); I2C_Delay();
    SDA_H(); I2C_Delay();
}
uint8_t I2C_WriteByte(uint8_t data){
    SDA_OUT();
    for(uint8_t i=0;i<8;i++){
        if(data & 0x80) SDA_H(); else SDA_L();
        I2C_Delay();
        SCL_H(); I2C_Delay();
        SCL_L();
        data <<= 1;
    }
    SDA_IN(); I2C_Delay();
    SCL_H(); I2C_Delay();
    uint8_t ack = (GPIOB->IDR & GPIO_IDR_IDR7)?0:1;
    SCL_L(); 
    SDA_OUT();
    return ack;
}
uint8_t I2C_ReadByte(uint8_t ack){
    uint8_t data=0;
    SDA_IN();
    for(uint8_t i=0;i<8;i++){
        data <<=1;
        SCL_H(); I2C_Delay();
        if(GPIOB->IDR & GPIO_IDR_IDR7) data |= 0x01;
        SCL_L(); I2C_Delay();
    }
    SDA_OUT();
    if(ack) SDA_L(); else SDA_H();
    SCL_H(); I2C_Delay();
    SCL_L();
    SDA_H();
    return data;
}

// ============ AHT10 DRIVER ============
void AHT10_Init(void){
    // enable clock GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    // PB6, PB7 output open-drain
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOB->CRL |= (GPIO_CRL_MODE6_0 | GPIO_CRL_CNF6_0 | GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_0);

    // send init
    I2C_Start();
    I2C_WriteByte(AHT10_ADDR);
    I2C_WriteByte(0xE1);
    I2C_WriteByte(0x08);
    I2C_WriteByte(0x00);
    I2C_Stop();
}

uint8_t AHT10_Read(float *temp, float *hum){
    uint8_t raw[6];
    uint32_t hum_raw, temp_raw;

    // trigger measure
    I2C_Start();
    I2C_WriteByte(AHT10_ADDR);
    I2C_WriteByte(0xAC);
    I2C_WriteByte(0x33);
    I2C_WriteByte(0x00);
    I2C_Stop();

    for(volatile int i=0;i<720000;i++); // ~80ms delay

    // read 6 bytes
    I2C_Start();
    I2C_WriteByte(AHT10_ADDR | 1);
    for(int i=0;i<5;i++) raw[i]=I2C_ReadByte(1);
    raw[5]=I2C_ReadByte(0);
    I2C_Stop();

    if(raw[0] & 0x80) return 0;

    hum_raw = ((uint32_t)raw[1]<<12)|((uint32_t)raw[2]<<4)|((raw[3]&0xF0)>>4);
    temp_raw= ((uint32_t)(raw[3]&0x0F)<<16)|((uint32_t)raw[4]<<8)|raw[5];

    *hum = hum_raw*100.0/1048576.0;
    *temp= temp_raw*200.0/1048576.0 - 50.0;
    return 1;
}
