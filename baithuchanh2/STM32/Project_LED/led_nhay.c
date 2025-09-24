#include "stm32f10x.h"

void delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

void led_nhay(void) {
    while (1) {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13); // LED sáng
        delay_ms(500);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);   // LED t?t
        delay_ms(500);
    }
}
