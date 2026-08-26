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


// ZS-040 블루투스 모듈에서 글자 받아오는 용 (기본 통신 확인)
uint8_t rx;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        printf("RX: 0x%02X (%c)\r\n", rx, rx);

        // 다음 1바이트 수신 대기
        HAL_UART_Receive_IT(&huart1, &rx, 1);
    }
}


void apMain(void){

    // USART1 수신 시작
    HAL_UART_Receive_IT(&huart1, &rx, 1);

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