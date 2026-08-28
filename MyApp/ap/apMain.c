#include "apMain.h"
#include "myUart.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "myVs1838b.h"
#include "myServo.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "tim.h"
#include "myKeypad.h"
#include "myBt.h"

#include <string.h>
#include <stdlib.h>

void apInit(void){
    myVs1838b_Init();
    myServo_Init();
    myKeypad_Init();
    // myBt_Init();
}


// ZS-040 블루투스 모듈에서 글자 받아오는 용 (기본 통신 확인)
uint8_t rx1;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 블루투스 모듈에서 받아옴
    if (huart->Instance == USART1)
    {
        // printf("RX: %c (0x%02X)\r\n", rx1, rx1);

        HAL_UART_Receive_IT(&huart1, &rx1, 1);
    }
}


void apMain(void){

    HAL_UART_Receive_IT(&huart1, &rx1, 1);

    while (1)
    {
        // BluetoothLE 앱에서 BTHOME 찾아서 커넥트하고,
        // a, b, c, d, e, f 를 send하면 다음 케이스코드 실행
        switch (rx1)
        {
            case 'a':
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                printf("a\r\n");
                break;

            case 'b':
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                printf("b\r\n");
                break;

            case 'c':
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                printf("c\r\n");
                break;

            case 'd':
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                printf("d\r\n");
                break;

            case 'e':
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                printf("e\r\n");
                break;

            case 'f':
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                printf("f\r\n");
                break;

            default:
                break;
        }
        rx1 = 0;

        HAL_Delay(10);
    }
}