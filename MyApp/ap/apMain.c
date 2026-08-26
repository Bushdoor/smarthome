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
}


void apMain(void){

    while (1)
    {
        myVs1838b_Process();

        char key;
        key = Keypad_GetKey();
        if (key != 0)
        {
            printf("%c\r\n",key);
        }

        HAL_Delay(300);
    }
}