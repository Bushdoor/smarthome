#include "myBt.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define RX_BUF_SIZE       512

// 연결할 Slave의 MAC 주소
#define BT_TARGET_ADDR    "0x04A31699DCD0"

// INQ 검색 대기 시간
#define BT_INQ_TIMEOUT    1500

// CONN 연결 대기 시간
#define BT_CONN_TIMEOUT   5000


// main.c에 있는 변수 사용
extern uint8_t rx1;
extern char rxBuf[RX_BUF_SIZE];
extern volatile uint16_t rxIndex;


//--------------------------------------------------
// RX Buffer Clear
//--------------------------------------------------

static void BT_ClearRxBuffer(void)
{
    __disable_irq();

    rxIndex = 0;
    rxBuf[0] = '\0';

    __enable_irq();
}


//--------------------------------------------------
// AT Command Send
//--------------------------------------------------

static void BT_SendCmd(const char *cmd)
{
    HAL_UART_Transmit(
        &huart1,
        (uint8_t *)cmd,
        strlen(cmd),
        1000
    );
}


//--------------------------------------------------
// INQ 결과에서 Target Index 찾기
//
// 예:
// +INQ:2 0x04A31699DCD0
//
// → 2 반환
//--------------------------------------------------

static int BT_FindTargetIndex(void)
{
    char *p;

    p = strstr(rxBuf, BT_TARGET_ADDR);

    if (p == NULL)
    {
        return -1;
    }


    // MAC 주소 앞쪽에서 "+INQ:" 찾기

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
    {
        return -1;
    }


    return atoi(inq + 5);
}


//--------------------------------------------------
// Init
//--------------------------------------------------

void myBt_Init(void)
{
    rxIndex = 0;
    rxBuf[0] = '\0';

    HAL_UART_Receive_IT(
        &huart1,
        &rx1,
        1
    );
}


//--------------------------------------------------
// Slave 검색 후 연결
//
// 연결될 때까지 INQ 반복
//--------------------------------------------------

void myBt_Connect(void)
{
    while (1)
    {
        //--------------------------------------------------
        // 검색 시작
        //--------------------------------------------------

        BT_ClearRxBuffer();

        printf("BT: INQ\r\n");

        BT_SendCmd("AT+INQ\r\n");

        uint32_t startTime = HAL_GetTick();


        //--------------------------------------------------
        // 검색 결과 기다리기
        //--------------------------------------------------

        while (HAL_GetTick() - startTime < BT_INQ_TIMEOUT)
        {
            int index = BT_FindTargetIndex();

            if (index >= 1 && index <= 99)
            {
                char cmd[32];

                printf(
                    "BT: TARGET FOUND, INDEX=%d\r\n",
                    index
                );


                //--------------------------------------------------
                // 해당 Index로 연결
                //--------------------------------------------------

                sprintf(
                    cmd,
                    "AT+CONN%d\r\n",
                    index
                );

                printf(
                    "BT: AT+CONN%d\r\n",
                    index
                );


                BT_ClearRxBuffer();

                BT_SendCmd(cmd);


                //--------------------------------------------------
                // 연결 응답 기다리기
                //--------------------------------------------------

                uint32_t connStart = HAL_GetTick();

                while (HAL_GetTick() - connStart < BT_CONN_TIMEOUT)
                {
                    if (strstr(rxBuf, "+CONN:") != NULL)
                    {
                        printf("BT: CONNECTED!\r\n");

                        return;
                    }


                    if (strstr(rxBuf, "ERROR") != NULL)
                    {
                        printf("BT: CONN ERROR\r\n");

                        break;
                    }
                }

                break;
            }
        }


        //--------------------------------------------------
        // 검색 실패 → 다시 INQ
        //--------------------------------------------------

        printf("BT: INQ TIMEOUT\r\n");
    }
}
