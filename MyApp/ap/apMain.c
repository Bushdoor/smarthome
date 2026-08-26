#include "apMain.h"
#include "myUart.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "myVs1838b.h"
#include "myServo.h"
#include "tim.h"
#include "myKeypad.h"

void apInit(void){
    myVs1838b_Init();
    myServo_Init();
    myKeypad_Init();
}


void apMain(void){

    while (1)
    {
        myVs1838b_Process();
        myKeypad_Process();

        HAL_Delay(10);
    }
}


/*
main
 ↓
IR 확인 → 없음 → 즉시 return
 ↓
Keypad 확인 → 없음 → 즉시 return
 ↓
10ms 대기
 ↓
반복
*/