#include "stm32f10x.h"
#include "aht10.h"

float Temperature, Humidity;

int main(void){
    AHT10_Init();
    while(1){
        if(AHT10_Read(&Temperature,&Humidity)){
            // xem Temperature, Humidity trong Watch Window
        }
        for(volatile int d=0; d<720000; d++); // delay ~100ms
    }
}
