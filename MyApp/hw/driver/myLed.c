#include "myLED.h"

void rgbLedInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE(); // 사용 포트 클럭 활성화

    GPIO_InitStruct.Pin = RED_PIN | GREEN_PIN | BLUE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RGB_PORT, &GPIO_InitStruct);

    // 초기 상태: 흰색
    rgbLedSetWhite();
}

// R, G, B 모두 Turn ON -> 흰색
void rgbLedSetWhite(void)
{
    HAL_GPIO_WritePin(RGB_PORT, RED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RGB_PORT, GREEN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RGB_PORT, BLUE_PIN, GPIO_PIN_SET);
}

// R만 Turn ON, G/B Turn OFF -> 빨간색
void rgbLedSetRed(void)
{
    HAL_GPIO_WritePin(RGB_PORT, RED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RGB_PORT, GREEN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RGB_PORT, BLUE_PIN, GPIO_PIN_RESET);
}