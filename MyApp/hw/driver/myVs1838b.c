#include "myVs1838b.h"
#include "tim.h"
#include <stdio.h> // printf
#include "myUart.h" // uart2 tx해서 teleplot 보는 용

static uint32_t prevTime = 0;

static uint32_t necData = 0;
static uint8_t bitCount = 0;

static volatile uint8_t commandReady = 0;
static volatile uint8_t receivedCommand = 0;


void myVs1838b_Init(void)
{
    // timer2 Internal Clock + Base Timer 사용
    HAL_TIM_Base_Start(&htim2);

    prevTime = __HAL_TIM_GET_COUNTER(&htim2);

    necData = 0;
    bitCount = 0;

    commandReady = 0;
    receivedCommand = 0;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != IR_PIN)
        return;

    uint32_t now = __HAL_TIM_GET_COUNTER(&htim2);
    uint32_t pulse = now - prevTime;

    prevTime = now;

    GPIO_PinState state = HAL_GPIO_ReadPin(IR_PORT, IR_PIN);

    /*
     * =========================
     * NEC START
     * =========================
     *
     * Idle  : HIGH
     * IR ON : LOW
     *
     * Rising edge
     * → LOW 9ms가 끝난 시점
     */

    if (state == GPIO_PIN_SET)
    {
        if (pulse > 8500 && pulse < 10000)
        {
            necData = 0;
            bitCount = 0;
        }

        return;
    }

    /*
     * =========================
     * NEC DATA
     * =========================
     *
     * Falling edge
     * → HIGH pulse가 끝난 시점
     *
     * HIGH 약 560us  → 0
     * HIGH 약 1690us → 1
     */

    if (state == GPIO_PIN_RESET)
    {
        if (pulse > 350 && pulse < 800)
        {
            // 0인 경우
            necData <<= 1;
            bitCount++;
        }
        else if (pulse > 1300 && pulse < 2000)
        {
            // 1인 경우
            necData <<= 1;
            necData |= 1;
            bitCount++;
        }
        else
        {
            return;
        }

        /* 32bit 수신 완료 */
        if (bitCount == 32) // 32비트 받으면 수신 완료라고 판단
        {
            uint8_t address     = (necData >> 24) & 0xFF;
            uint8_t address_inv = (necData >> 16) & 0xFF;
            uint8_t command     = (necData >> 8)  & 0xFF;
            uint8_t command_inv = necData & 0xFF;

            /* NEC 데이터 검증
             NEC프로토콜 : Address 8 + 반전 8 + Command 8 + 반전 8 */

            uint8_t address_ok =
                ((address ^ address_inv) == 0xFF); // 반전값과 xor하여 검증

            uint8_t command_ok =
                ((command ^ command_inv) == 0xFF);

            /* VALID한 데이터만 저장 */
            if (address_ok && command_ok)
            {
                receivedCommand = command;
                commandReady = 1;
            }

            /* 다음 데이터 수신 준비 */
            bitCount = 0;
            necData = 0;
        }
    }
}


void myVs1838b_Process(void)
{
    if (!commandReady) return;

    /* Callback에서 저장한 command 가져오기 */
    uint8_t command = receivedCommand;

    /* 처리 완료 */
    commandReady = 0;

    /* BUTTON ACTION 로직 추가영역*/
    switch (command)
    {
        case 0xA2:
            // CH-
            printf("CH-\r\n");
            break;

        case 0x62:
            // CH
            printf("CH\r\n");
            break;

        case 0xE2:
            // CH+
            printf("CH+\r\n");
            break;

        case 0x22:
            // PREV
            printf("PREV\r\n");
            break;

        case 0x02:
            // NEXT
            printf("NEXT\r\n");
            break;

        case 0xC2:
            // PLAY/PAUSE
            printf("PLAY/PAUSE\r\n");
            break;

        case 0xE0:
            // VOL-
            printf("VOL-\r\n");
            break;

        case 0xA8:
            // VOL+
            printf("VOL+\r\n");
            break;

        case 0x90:
            // EQ
            printf("EQ\r\n");
            break;

        case 0x68:
            // 0
            printf("0\r\n");
            break;

        case 0x98:
            // 100+
            printf("100+\r\n");
            break;

        case 0xB0:
            // 200+
            printf("200+\r\n");
            break;

        case 0x30:
            // 1
            printf("1\r\n");
            break;

        case 0x18:
            // 2
            printf("2\r\n");
            break;

        case 0x7A:
            // 3
            printf("3\r\n");
            break;

        case 0x10:
            // 4
            printf("4\r\n");
            break;

        case 0x38:
            // 5
            printf("5\r\n");
            break;

        case 0x5A:
            // 6
            printf("6\r\n");
            break;

        case 0x42:
            // 7
            printf("7\r\n");
            break;

        case 0x4A:
            // 8
            printf("8\r\n");
            break;

        case 0x52:
            // 9
            printf("9\r\n");
            break;

        default:
            // 등록되지 않은 버튼
            printf("UNKNOWN: 0x%02X\r\n", command);
            break;
    }
}


// 신호 검증용
 // if (myVs1838b_IsReceived())
        // {
        //     uint32_t raw = myVs1838b_GetCode();

        //     uint8_t address     = (raw >> 24) & 0xFF;
        //     uint8_t address_inv = (raw >> 16) & 0xFF;
        //     uint8_t command     = (raw >> 8)  & 0xFF;
        //     uint8_t command_inv = raw & 0xFF;

        //     uint8_t address_ok = ((address ^ address_inv) == 0xFF);
        //     uint8_t command_ok = ((command ^ command_inv) == 0xFF);

        //     printf("\r\n");
        //     printf("RAW     = 0x%08lX\r\n", raw);

        //     printf("ADDR    = 0x%02X\r\n", address);
        //     printf("~ADDR   = 0x%02X\r\n", address_inv);

        //     printf("CMD     = 0x%02X\r\n", command);
        //     printf("~CMD    = 0x%02X\r\n", command_inv);

        //     printf("ADDR OK = %s\r\n", address_ok ? "YES" : "NO");
        //     printf("CMD  OK = %s\r\n", command_ok ? "YES" : "NO");

        //     if (address_ok && command_ok)
        //     {
        //         printf(">>> NEC VALID <<<\r\n");
        //     }
        //     else
        //     {
        //         printf(">>> NEC INVALID <<<\r\n");
        //     }
        // }