#include "stm32f10x.h"

// khai b�o h�m t? file kh�c
extern void led_nhay(void);
extern void led_codinh(void);

int main(void) {
    // B?t clock GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // C?u h�nh PC13 l�m output
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // --- Ch?n mode ---
    //led_nhay();     // g?i h�m nh�y LED
    led_codinh();    // g?i h�m LED c? d?nh

    while (1); // gi? chuong tr�nh ch?y
}
