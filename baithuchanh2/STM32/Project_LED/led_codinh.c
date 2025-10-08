#include "stm32f10x.h"

void led_codinh(void) {
    GPIO_ResetBits(GPIOC, GPIO_Pin_13); // LED sáng
    while (1) {
        // không làm gì ca
    }
}
