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
    // myBt_Init();
}


// ZS-040 블루투스 모듈에서 글자 받아오는 용 (기본 통신 확인)
uint8_t rx1;
uint8_t rx2;
#define RX_BUF_SIZE 512
char rxBuf[RX_BUF_SIZE];
volatile uint16_t rxIndex = 0;

#define UART2_BUF_SIZE 512
uint8_t uart2Buf[UART2_BUF_SIZE];
volatile uint16_t uart2Index = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // uart1으로 송신
    if (huart->Instance == USART1)
    {
        if (rxIndex < RX_BUF_SIZE - 1)
        {
            rxBuf[rxIndex++] = rx1;
            rxBuf[rxIndex] = '\0';
        }
        HAL_UART_Receive_IT(&huart1, &rx1, 1);
    }

    // // UART2 수신 -> UART1 송신
    // if (huart->Instance == USART2)
    // {
    //     // UART2로 들어온 1바이트를 UART1로 그대로 전송
    //     HAL_UART_Transmit(&huart1, &rx2, 1, 100);

    //     // UART2 다음 1바이트 수신
    //     HAL_UART_Receive_IT(&huart2, &rx2, 1);
    // }

    if (huart->Instance == USART2)
    {
        // UART2로 들어온 데이터를 버퍼에 저장
        if (uart2Index < UART2_BUF_SIZE - 2)
        {
            // '\r'이 들어오면 명령어 끝으로 판단
            if (rx2 == '\r')
            {
                // 문자열 끝에 \r 추가
                uart2Buf[uart2Index++] = '\r';

                // \n 추가
                uart2Buf[uart2Index++] = '\n';

                // UART2에서 받은 문자열 전체를 UART1로 전송
                HAL_UART_Transmit(&huart1,
                                uart2Buf,
                                uart2Index,
                                1000);

                // 전송 후 버퍼 초기화
                memset(uart2Buf, 0, UART2_BUF_SIZE);
                uart2Index = 0;
            }
            else
            {
                // 일반 문자는 버퍼에 저장
                uart2Buf[uart2Index++] = rx2;
            }
        }
        else
        {
            // 버퍼가 가득 차면 초기화
            uart2Index = 0;
            memset(uart2Buf, 0, UART2_BUF_SIZE);
        }

        // UART2 다음 1바이트 수신
        HAL_UART_Receive_IT(&huart2, &rx2, 1);
    }
}


void apMain(void){

    // myBt_Connect();

    // 매뉴얼 명령
    HAL_UART_Receive_IT(&huart1, &rx1, 1);
    HAL_UART_Receive_IT(&huart2, &rx2, 1);

    // uint8_t cmd[] = "AT+TYPE?\r\n"; // 0 - not need password
    // uint8_t cmd[] = "AT+IMME?\r\n"; // 모듈 동작타입 0(자동), 1(수동)
    // uint8_t cmd[] = "AT+IMME1\r\n";
    // uint8_t cmd[] = "AT+IMME1\r\n";
    // uint8_t cmd[] = "AT+INQ\r\n";
    // uint8_t cmd[] = "AT+CONN4\r\n";
    // uint8_t cmd[] = "AT+LADDR\r\n";

    // uint8_t cmd[] = "AT+BIND=98da,50,044e8b\r\n"; // 98da:50:044e8b
    // uint8_t cmd[] = "AT+BIND?\r\n"; // 98da:50:044e8b
    // HAL_UART_Transmit(&huart1, cmd, sizeof(cmd) - 1, 1000);
    // HAL_Delay(3000);
    // printf("%s\r\n", rxBuf);

    // HAL_Delay(1000);
    // +VERSION:2.0-20100601
    // 98da:50:044cc1

    uint8_t cmd[] = "AT+ADDR?\r\n"; // 04A31699DCD0
    HAL_UART_Transmit(&huart1, cmd, sizeof(cmd) - 1, 1000);
    HAL_Delay(5000);
    printf("%s\r\n", rxBuf);

    // uint8_t cmd1[] = "AT+RESET\r\n"; // 04A31699DCD0
    // HAL_UART_Transmit(&huart1, cmd1, sizeof(cmd1) - 1, 1000);
    // HAL_Delay(2000);
    // printf("%s\r\n", rxBuf);

    // uint8_t cmd2[] = "AT+INIT\r\n"; // 04A31699DCD0
    // HAL_UART_Transmit(&huart1, cmd2, sizeof(cmd2) - 1, 1000);
    // HAL_Delay(2000);
    // printf("%s\r\n", rxBuf);

    // uint8_t cmd3[] = "AT+INQ\r\n"; // 04A31699DCD0
    // HAL_UART_Transmit(&huart1, cmd3, sizeof(cmd3) - 1, 1000);
    // HAL_Delay(2000);
    // printf("%s\r\n", rxBuf);

    // uint8_t data[] = "ABC123";
    // HAL_StatusTypeDef ret;
    // ret = HAL_UART_Transmit(&huart1, data, sizeof(data)-1, 1000);
    // printf("ret=%d\r\n", ret);

    while (1)
    {
        myVs1838b_Process();
        myKeypad_Process();

        HAL_Delay(10);
    }
}