#include "stm32f10x.h"

void led_codinh(void) {
    GPIO_ResetBits(GPIOC, GPIO_Pin_13); // LED s�ng
    while (1) {
        // kh�ng l�m g� ca
    }
}
