#include "apMain.h"
#include "myUart.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "myVs1838b.h"
#include "myServo.h"
#include "tim.h"
#include "myKeypad.h"
#include "myBt.h"

#include <string.h>
#include <stdlib.h>

void apInit(void){
    myVs1838b_Init();
    myServo_Init();
    myKeypad_Init();
    myBt_Init();
}


// ZS-040 블루투스 모듈에서 글자 받아오는 용 (기본 통신 확인)
uint8_t rx1;
uint8_t rx2;
#define RX_BUF_SIZE 512
char rxBuf[RX_BUF_SIZE];
volatile uint16_t rxIndex = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // if (huart->Instance == USART1) // BT모듈 1
    // {
    //     printf("RX1: 0x%02X (%c)\r\n", rx1, rx1);

    //     // 다음 1바이트 수신 대기
    //     HAL_UART_Receive_IT(&huart1, &rx1, 1);
    // }

    // if (huart->Instance == USART1)
    // {
    //     printf("%c", rx1);

    //     HAL_UART_Receive_IT(&huart1, &rx1, 1);
    // }

    if (huart->Instance == USART1)
    {
        if (rxIndex < RX_BUF_SIZE - 1)
        {
            rxBuf[rxIndex++] = rx1;
            rxBuf[rxIndex] = '\0';
        }
        HAL_UART_Receive_IT(&huart1, &rx1, 1);
    }

    // if (huart->Instance == USART6) // BT모듈 2
    // {
    //     printf("RX2: 0x%02X (%c)\r\n", rx2, rx2);

    //     // 다음 1바이트 수신 대기
    //     HAL_UART_Receive_IT(&huart6, &rx2, 1);
    // }

    // if (huart->Instance == USART6)
    // {
    //     printf("%c", rx2);

    //     HAL_UART_Receive_IT(&huart6, &rx2, 1);
    // }

    // if (huart->Instance == USART6)
    // {
    //     if (rxIndex < sizeof(rxBuf) - 1)
    //     {
    //         rxBuf[rxIndex++] = rx2;
    //         rxBuf[rxIndex] = '\0';
    //     }
    //     HAL_UART_Receive_IT(&huart6, &rx2, 1);
    // }
}


void apMain(void){

    myBt_Connect();

    while (1)
    {
        myVs1838b_Process();
        myKeypad_Process();

        HAL_Delay(10);
    }
}