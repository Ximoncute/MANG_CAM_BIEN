#include "stm32f10x.h"
#include <stdio.h>

// ----------------------
// Bien toan cuc
// ----------------------
int temperature = 0;   // luu nhiet do do duoc
int humidity = 0;      // luu do am do duoc
int status = 0;        // luu trang thai doc du lieu tu DHT11 (0=OK,1=loi phan hoi,2=loi checksum)

// ----------------------
// Ham delay bang SysTick
// ----------------------
void Delay_Init(void) {
    SysTick->CTRL = 0;         // tat SysTick
    SysTick->LOAD = 0xFFFFFF;  // dat gia tri dem toi da
    SysTick->VAL = 0;          // reset gia tri hien tai
}

void Delay_us(uint32_t us) {
    // moi micro giay can (SystemCoreClock/8000000) chu ky
    SysTick->LOAD = (SystemCoreClock / 8000000) * us;
    SysTick->VAL = 0;                                 // reset bo dem
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;          // bat SysTick
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // cho den khi co co bao het thoi gian
    SysTick->CTRL = 0;                                // tat SysTick
}

void Delay_ms(uint32_t ms) {
    while (ms--) Delay_us(1000); // goi 1000us = 1ms
}

// ----------------------
// UART1 tren PA9, PA10
// ----------------------
void UART1_Init(uint32_t baud) {
    // bat clock cho GPIOA va USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    // TX - PA9
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;       // chan TX o che do output push-pull
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // RX - PA10
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // chan RX o che do input floating
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // cau hinh USART1
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baud;                       // toc do baud truyen
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;      // 8 bit du lieu
    USART_InitStruct.USART_StopBits = USART_StopBits_1;           // 1 bit stop
    USART_InitStruct.USART_Parity = USART_Parity_No;              // khong parity
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // khong kiem soat luu luong
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // bat ca truyen va nhan
    USART_Init(USART1, &USART_InitStruct);
    USART_Cmd(USART1, ENABLE);                                    // bat USART1
}

void UART1_SendChar(char c) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // cho den khi thanh ghi TX trong
    USART_SendData(USART1, c);                                   // gui 1 ky tu
}

void UART1_SendString(const char *s) {
    while (*s) UART1_SendChar(*s++); // gui tung ky tu trong chuoi
}

// ----------------------
// DHT11 tren chan PA0
// ----------------------
#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_Pin_0

// chuyen chan PA0 sang che do output push-pull
static void DHT11_SetPinOutput(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

// chuyen chan PA0 sang che do input floating
static void DHT11_SetPinInput(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

// ham khoi tao DHT11
void DHT11_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // bat clock cho GPIOA
    DHT11_SetPinOutput();                                 // dat chan PA0 lam output
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);                  // keo len muc cao
}

// ----------------------
// Ham doc DHT11 fix chuan
// ----------------------
int DHT11_Read(int *temp, int *humi) {
    uint8_t data[5] = {0}; // mang luu 5 byte du lieu nhan duoc
    uint8_t i, j;
    uint32_t timeout;

    // ----- Gui tin hieu start -----
    DHT11_SetPinOutput();                     // chuyen chan sang output
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);    // keo xuong muc thap
    Delay_ms(18);                             // giu muc thap it nhat 18ms
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);      // keo len muc cao
    Delay_us(40);                             // cho 20-40us
    DHT11_SetPinInput();                      // chuyen sang input de doc phan hoi

    // ----- Cho DHT11 phan hoi -----
    timeout = 10000;
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--); // cho DHT keo xuong muc thap
    if (timeout == 0) return 1; // khong phan hoi

    timeout = 10000;
    while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--); // cho DHT keo len
    if (timeout == 0) return 1;

    timeout = 10000;
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--); // cho DHT keo xuong lai
    if (timeout == 0) return 1;

    // ----- Doc 5 byte du lieu -----
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 8; i++) {
            timeout = 10000;
            while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--); // cho bat dau bit
            if (timeout == 0) return 1;

            Delay_us(40); // doi 40us de xac dinh gia tri bit
            if (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)) // neu muc cao sau 40us
                data[j] |= (1 << (7 - i));                    // ghi la bit 1
            timeout = 10000;
            while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--); // cho ket thuc bit
            if (timeout == 0) return 1;
        }
    }

    // ----- Kiem tra checksum -----
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
        return 2; // loi checksum

    *humi = data[0]; // do am nguyen
    *temp = data[2]; // nhiet do nguyen
    return 0;        // doc thanh cong
}

// ----------------------
// Main
// ----------------------
int main(void) {
    Delay_Init();            // khoi tao delay
    DHT11_Init();            // khoi tao chan DHT11
    UART1_Init(115200);      // khoi tao UART1 toc do 115200
    char buf[50];            // bo dem xuat UART

    while (1) {
        status = DHT11_Read(&temperature, &humidity); // doc du lieu tu DHT11

        if (status == 0) { // neu doc thanh cong
            sprintf(buf, "Temp=%d Hum=%d\r\n", temperature, humidity);
            UART1_SendString(buf); // gui du lieu len may tinh
        } else { // neu loi
            sprintf(buf, "Read Error (code=%d)\r\n", status);
            UART1_SendString(buf); // gui thong bao loi
        }

        Delay_ms(2000); // doc moi 2 giay
    }
}
