#include "stm32f10x.h"

// Khai bao ham tu file khac
extern void led_nhay(void);
extern void led_codinh(void);

int main(void) {
    // Bat clock GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // Cau hinh PC13 lam output
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // --- Chon mode ---
    led_nhay();     // Goi ham nhay LED
	  //led_codinh();    // Goi ham LED co dinh

    while (1); // Giu chuong trinh chay
}
