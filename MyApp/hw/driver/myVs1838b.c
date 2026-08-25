#include "myVs1838b.h"
#include "tim.h"

static uint32_t prevTime = 0;

static uint32_t necData = 0;
static uint8_t bitCount = 0;

static volatile uint8_t received = 0;
static volatile uint32_t receivedCode = 0;


void myVs1838b_Init(void)
{
    HAL_TIM_Base_Start(&htim2);

    prevTime = __HAL_TIM_GET_COUNTER(&htim2);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != IR_PIN)
        return;

    uint32_t now = __HAL_TIM_GET_COUNTER(&htim2);
    uint32_t pulse = now - prevTime;

    prevTime = now;

    /*
     * 현재 PA9 상태
     *
     * VS1838B:
     *
     * Idle  = HIGH
     * IR ON = LOW
     */

    GPIO_PinState state = HAL_GPIO_ReadPin(IR_PORT, IR_PIN);


    /*
     * =========================
     * NEC START
     * =========================
     *
     * LOW  ≈ 9000us
     * HIGH ≈ 4500us
     *
     * LOW 9ms가 들어오면 새로운 frame 시작
     */

    if (state == GPIO_PIN_SET)
    {
        // Rising edge → LOW가 끝남

        if (pulse > 8500 && pulse < 10000)
        {
            necData = 0;
            bitCount = 0;
            return;
        }
    }


    /*
     * =========================
     * NEC DATA
     * =========================
     *
     * LOW  ≈ 560us
     *
     * 그 다음 HIGH:
     *
     * 560us  → 0
     * 1690us → 1
     *
     * Falling edge에서 HIGH가 끝났으므로
     * HIGH pulse를 측정한다.
     */

    if (state == GPIO_PIN_RESET)
    {
        // Falling edge → HIGH가 끝남

        if (pulse > 350 && pulse < 800)
        {
            // 0
            necData <<= 1;
            bitCount++;
        }
        else if (pulse > 1300 && pulse < 2000)
        {
            // 1
            necData <<= 1;
            necData |= 1;
            bitCount++;
        }
        else
        {
            return;
        }


        /*
         * 32bit 완성
         */

        if (bitCount == 32)
        {
            receivedCode = necData;
            received = 1;

            bitCount = 0;
            necData = 0;
        }
    }
}


uint8_t myVs1838b_IsReceived(void)
{
    return received;
}


uint32_t myVs1838b_GetCode(void)
{
    received = 0;

    return receivedCode;
}


void myVs1838b_Process(void)
{
    // 현재는 별도 처리 없음
}