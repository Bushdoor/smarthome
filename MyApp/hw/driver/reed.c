#include "reed.h"
#include "motor.h"

void Reed_Init(void)
{
    // 필요 시 초기화 로직 (현재 별도 초기화 불필요)
}


uint8_t Reed_Open_IsDetected(void)
{
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET)
    {
        return 1;
    }

    return 0;
}


uint8_t Reed_Close_IsDetected(void)
{
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET)
    {
        return 1;
    }

    return 0;
}

uint8_t Reed_Door_IsDetected(void)
{
    return (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET);
}

uint8_t Reed_Door_Lock_Condition(void)
{
    static uint8_t detecting = 0;
    static uint32_t start_time = 0;

    // 이미 잠겨있으면 타이머를 진행시키지 않음 (반복 재락 방지)
    if (Motor_IsLocked())
    {
        detecting = 0;
        return 0;
    }

    if (Reed_Door_IsDetected())
    {
        if (!detecting)
        {
            detecting = 1;
            start_time = HAL_GetTick();
        }

        if ((HAL_GetTick() - start_time) >= 3000)
        {
            detecting = 0;
            return 1;
        }
    }
    else
    {
        detecting = 0;
    }

    return 0;
}