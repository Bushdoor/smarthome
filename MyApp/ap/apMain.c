#include "apMain.h"
#include "myUart.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "myVs1838b.h"

void apInit(void){
    myVs1838b_Init(); // 리모콘 수신준비
}

void apMain(void){
    
    while (1)
    {
        myVs1838b_Process(); // 리모콘 수신용

        // HAL_Delay(1);
    }
}