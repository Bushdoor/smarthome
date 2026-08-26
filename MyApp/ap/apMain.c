#include "apMain.h"
#include "myUart.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "myVs1838b.h"
#include "myServo.h"
#include "tim.h"

void apInit(void){
    myVs1838b_Init(); // 리모콘 수신준비
    myServo_Init();
}

// void apMain(void){

//     while (1)
//     {
//         myVs1838b_Process(); // 리모콘 수신용

//         myServo_SetAngle(0);

//         // HAL_Delay(1000);

//         // myServo_SetAngle(90);

//         // HAL_Delay(1000);

//         // myServo_SetAngle(180);

//         HAL_Delay(100);
//     }
// }

void apMain(void){

    while (1)
    {
        myVs1838b_Process();
        // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 500);
        HAL_Delay(300);

        // 550 0도
        // 500µs 증가할 때 약 45° 이동

        // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 1000);
        // HAL_Delay(2000);

        // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 1500);
        // HAL_Delay(2000);

        // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2000);
        // HAL_Delay(2000);

        // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2300);
        // HAL_Delay(2000);
    }
}