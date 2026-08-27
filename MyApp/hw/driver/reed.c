#include "reed.h"

#define REED_OPEN_GPIO_Port    GPIOB
#define REED_OPEN_Pin          GPIO_PIN_14

#define REED_CLOSE_GPIO_Port   GPIOB
#define REED_CLOSE_Pin         GPIO_PIN_15


void Reed_Init(void)
{
}


uint8_t Reed_Open_IsDetected(void)
{
    if (HAL_GPIO_ReadPin(REED_OPEN_GPIO_Port, REED_OPEN_Pin) == GPIO_PIN_SET)
    {
        return 1;
    }

    return 0;
}


uint8_t Reed_Close_IsDetected(void)
{
    if (HAL_GPIO_ReadPin(REED_CLOSE_GPIO_Port, REED_CLOSE_Pin) == GPIO_PIN_SET)
    {
        return 1;
    }

    return 0;
}