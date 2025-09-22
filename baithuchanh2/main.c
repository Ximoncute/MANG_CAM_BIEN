#include "stm32f10x.h"
#include <stdint.h>

// ===================== Bi?n toàn c?c =====================
int hum = 0;     // Ð? ?m nguyên (%)
int temp = 0;    // Nhi?t d? nguyên (°C)

uint8_t I_RH, D_RH, I_Temp, D_Temp;
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2, CheckSum, presence;

// ===================== Prototype =====================
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

void DHT11_Start(void);
uint8_t DHT11_Check_Response(void);
uint8_t DHT11_Read(void);

// ===================== Delay b?ng SysTick =====================
void delay_us(uint32_t us)
{
    SysTick->LOAD = 72 * us;      // 72 MHz
    SysTick->VAL  = 0;
    SysTick->CTRL = 5;            // Enable SysTick
    while ((SysTick->CTRL & (1 << 16)) == 0);
    SysTick->CTRL = 0;
}

void delay_ms(uint32_t ms)
{
    while(ms--) delay_us(1000);
}

// ===================== GPIO Setup =====================
#define DHT11_PIN 0   // PA0

static void GPIO_Config(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // Enable clock GPIOA

    // PA0 input floating
    GPIOA->CRL &= ~(0xF << (DHT11_PIN * 4));
    GPIOA->CRL |=  (0x4 << (DHT11_PIN * 4));
}

void DHT11_Set_Output(void)
{
    GPIOA->CRL &= ~(0xF << (DHT11_PIN * 4));
    GPIOA->CRL |=  (0x3 << (DHT11_PIN * 4));   // Output 50MHz push-pull
}

void DHT11_Set_Input(void)
{
    GPIOA->CRL &= ~(0xF << (DHT11_PIN * 4));
    GPIOA->CRL |=  (0x4 << (DHT11_PIN * 4));   // Input floating
}

// ===================== DHT11 Functions =====================
void DHT11_Start(void)
{
    DHT11_Set_Output();
    GPIOA->BSRR = (1 << (DHT11_PIN + 16)); // Kéo xu?ng 0
    delay_ms(20);                          // Ít nh?t 18ms
    GPIOA->BSRR = (1 << DHT11_PIN);        // Kéo lên 1
    delay_us(30);
    DHT11_Set_Input();
}

uint8_t DHT11_Check_Response(void)
{
    uint8_t response = 0;
    delay_us(40);
    if (!(GPIOA->IDR & (1 << DHT11_PIN)))
    {
        delay_us(80);
        if (GPIOA->IDR & (1 << DHT11_PIN)) response = 1;
        delay_us(40);
    }
    return response;
}

uint8_t DHT11_Read(void)
{
    uint8_t i, j = 0;
    for (i = 0; i < 8; i++)
    {
        while (!(GPIOA->IDR & (1 << DHT11_PIN))); // ch? lên
        delay_us(40);
        if (GPIOA->IDR & (1 << DHT11_PIN))
            j |= (1 << (7 - i));
        while (GPIOA->IDR & (1 << DHT11_PIN));    // ch? xu?ng
    }
    return j;
}

// ===================== MAIN =====================
int main(void)
{
    GPIO_Config();
    delay_ms(1000); // Ch? c?m bi?n ?n d?nh

    while (1)
    {
        DHT11_Start();
        presence = DHT11_Check_Response();

        Rh_byte1   = DHT11_Read();
        Rh_byte2   = DHT11_Read();
        Temp_byte1 = DHT11_Read();
        Temp_byte2 = DHT11_Read();
        CheckSum   = DHT11_Read();

        if ((Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2) == CheckSum)
        {
            I_RH   = Rh_byte1;
            D_RH   = Rh_byte2;
            I_Temp = Temp_byte1;
            D_Temp = Temp_byte2;

            hum  = I_RH;   // giá tr? % d? ?m
            temp = I_Temp; // giá tr? °C nhi?t d?
        }

        delay_ms(2000); // Ð?c m?i 2 giây
    }
}
