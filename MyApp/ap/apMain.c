#include "apMain.h"
#include "myUart.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "myVs1838b.h"
#include "myServo.h"
#include "tim.h"
#include "myKeypad.h"

#include <string.h>
#include <stdlib.h>

void apInit(void){
    myVs1838b_Init();
    myServo_Init();
    myKeypad_Init();
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

// 버퍼 초기화 함수
void BT_ClearRxBuffer(void)
{
    __disable_irq();

    rxIndex = 0;
    rxBuf[0] = '\0';

    __enable_irq();
}

// 명령 보내기
void BT_SendCmd(const char *cmd)
{
    HAL_UART_Transmit(
        &huart1,
        (uint8_t *)cmd,
        strlen(cmd),
        1000
    );
}

// MAC에서 index 뽑기
// +INQ:2 0x04A31699DCD0 가 들어왔을 때, 인덱스 출력
int BT_FindTargetIndex(void)
{
    char *p;

    p = strstr(rxBuf, "0x04A31699DCD0");

    if (p == NULL)
        return -1;

    // 주소 앞쪽에서 "+INQ:" 찾기
    char *inq = p;

    while (inq > rxBuf)
    {
        if (inq[0] == '+' &&
            inq[1] == 'I' &&
            inq[2] == 'N' &&
            inq[3] == 'Q' &&
            inq[4] == ':')
        {
            break;
        }

        inq--;
    }

    if (inq <= rxBuf)
        return -1;

    return atoi(inq + 5);
}

// 상태 머신
typedef enum
{
    BT_IDLE,
    BT_INQ,
    BT_CONNECTING,
    BT_CONNECTED
} BT_State;

BT_State btState = BT_IDLE;

uint32_t btTimer = 0;


// slave 찾기
void BT_Process(void)
{
    switch (btState)
    {
    case BT_IDLE:

        BT_ClearRxBuffer();

        printf("BT: INQ\r\n");

        BT_SendCmd("AT+INQ\r\n");

        btTimer = HAL_GetTick();

        btState = BT_INQ;

        break;


    case BT_INQ:
    {
        int index = BT_FindTargetIndex();

        if (index >= 1 && index <= 99)
        {
            char cmd[32];

            sprintf(cmd, "AT+CONN%d\r\n", index);

            printf("BT: TARGET FOUND, INDEX=%d\r\n", index);
            printf("BT: %s", cmd);

            BT_ClearRxBuffer();

            BT_SendCmd(cmd);

            btTimer = HAL_GetTick();

            btState = BT_CONNECTING;
        }
        else if (HAL_GetTick() - btTimer > 1500) // 원래 10000 이었음
        {
            printf("BT: INQ TIMEOUT\r\n");
            printf("BT RX: [%s]\r\n", rxBuf);

            btState = BT_IDLE;
        }

        break;
    }


    case BT_CONNECTING:

    if (strstr(rxBuf, "+CONN:") != NULL)
    {
        printf("BT RX = [%s]\r\n", rxBuf);

        printf("BT: CONNECTED!\r\n");

        btState = BT_CONNECTED;
    }
    else if (strstr(rxBuf, "ERROR") != NULL)
    {
        printf("BT RX = [%s]\r\n", rxBuf);

        printf("BT: CONN ERROR\r\n");

        btState = BT_IDLE;
    }
    else if (HAL_GetTick() - btTimer > 5000)
    {
        printf("BT RX = [%s]\r\n", rxBuf);

        printf("BT: CONNECT TIMEOUT\r\n");

        btState = BT_IDLE;
    }

    break;



    case BT_CONNECTED:

        // 나중에 실제 데이터 통신

        break;
    }
}





void apMain(void){

    // HC-05 AT 모드: 38400 baud
    // huart1.Init.BaudRate = 38400;
    // if (HAL_UART_Init(&huart1) != HAL_OK)
    // {
    //     Error_Handler();
    // }

    // USART1 수신 시작
    HAL_UART_Receive_IT(&huart1, &rx1, 1);
    // HAL_UART_Receive_IT(&huart6, &rx2, 1);
    btState = BT_IDLE;

    HAL_Delay(1000);

    // HC-05 #1에 AT 전송
    // uint8_t cmd[] = "AT\r\n"; // OK 왔음 (정상)
    // uint8_t cmd[] = "AT+VERSION\r\n"; // +VERSION=v5.6 왔음
    // uint8_t cmd[] = "AT+ROLE\r\n"; // +ROLE=0 (slave)
    // uint8_t cmd[] = "AT+LADDR\r\n"; // +LADD=04A3169D415C

    // HC-05 #2에 AT 전송
    // uint8_t cmd[] = "AT\r\n"; // OK 왔음 (정상)
    // uint8_t cmd[] = "AT+VERSION\r\n"; // +VERSION=v5.6 왔음
    // uint8_t cmd[] = "AT+ROLE\r\n"; // +ROLE=0 (slave)
    // uint8_t cmd[] = "AT+START\r\n";
    // uint8_t cmd[] = "AT+LADDR\r\n"; // +LADD=04A31699DCD0
    // uint8_t cmd[] = "AT+ROLE1\r\n"; // Master로 변경
    // uint8_t cmd[] = "AT+INQ\r\n"; // 주변 slave 검색
    // uint8_t cmd[] = "AT+SHOW\r\n"; // 검색결과 확인
    // uint8_t cmd[] = "AT+IMME\r\n"; // 자동연결여부 0자동 1수동
    // uint8_t cmd[] = "AT+IMME?"; // 자동연결여부 0자동 1수동
    // uint8_t cmd[] = "AT+IMME1\r\n"; // 자동연결여부 0자동 1수동
    // uint8_t cmd[] = "AT+IMME=0\r\n"; // 자동연결여부 0자동 1수동
    // uint8_t cmd[] = "AT+CONN2\r\n"; // 자동연결여부 0자동 1수동
    // uint8_t cmd[] = "AT+CONN 04A31699DCD0\r\n"; // 자동연결여부 0자동 1수동
    // uint8_t cmd[] = "ATCONN?\r\n"; // 자동연결여부 0자동 1수동
    // uint8_t cmd[] = "AT+CONN15\r\n"; // 자동연결여부 0자동 1수동
    
    // HAL_UART_Transmit(&huart1, cmd, sizeof(cmd) - 1, 1000);
    // HAL_Delay(3000);
    // printf("\r\n%s\r\n", rxBuf);

    while (1)
    {
        myVs1838b_Process();
        myKeypad_Process();

        BT_Process();

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

// AT+HELP
// ********************************************************************
// * Command             Description                                  *
// * ---------------------------------------------------------------- *
// * AT                  Check if the command terminal work normally  *
// * AT+RESET            Software reboot                              *
// * AT+VERSION          Get firmware, bluetooth, HCI and LMP version *
// * AT+HELP             List all the commands                        *
// * AT+NAME             Get/Set local device name                    *
// * AT+PIN              Get/Set pin code for pairing                 *
// * AT+BAUD             Get/Set baud rate                            *
// * AT+LADDR            Get local bluetooth address                  *
// * AT+ADDR             Get local bluetooth address                  *
// * AT+DEFAULT          Restore factory default                      *
// * AT+RENEW            Restore factory default                      *
// * AT+STATE            Get current state                            *
// * AT+PWRM             Get/Set power on mode(low power)             *
// * AT+POWE             Get/Set RF transmit power                    *
// * AT+SLEEP            Sleep mode                                   *
// * AT+ROLE             Get/Set current role.                        *
// * AT+PARI             Get/Set UART parity bit.                     *
// * AT+STOP             Get/Set UART stop bit.                       *
// * AT+INQ              Search slave model                           *
// * AT+SHOW             Show the searched slave model.               *
// * AT+CONN             Connect the index slave model.               *
// * AT+IMME             System wait for command when power on.       *
// * AT+START            System start working.                        *
// * AT+UUID             Get/Set system SERVER_UUID .                 *
// * AT+CHAR             Get/Set system CHAR_UUID .                   *
// * -----------------------------------------------------------------*
// * Note: (M) = The command support master mode only.                *



// AT+INQ

// +INQS
// Scanning...+INQ:1 0x3F4A407B3254
// +INQ:2 0x04A31699DCD0
// +INQ:3 0x004E577DC4BF
// +INQ:4 0x090CB1F9ECC5

// 04A31699DCD0 찾아야함 -> index 2임


// AT+CONN
// 1
// +CONN:2
// 1

// AT+IMME
// IMME=0 → 전원 켜면 자동으로 정상 동작/자동 연결 시도
// IMME=1 → 전원 켜면 AT 명령 대기