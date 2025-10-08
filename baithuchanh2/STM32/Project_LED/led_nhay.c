#include "stm32f10x.h"

void delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

void led_nhay(void) {
    while (1) {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13); // LED sang
        delay_ms(100);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);   // LED tat
        delay_ms(100);
    }
}
