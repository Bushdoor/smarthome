#include "myLED.h"

#include <stdbool.h>
// =================================
// RGB/LED 상태
// =================================
static bool RgbState = false;
static bool roomLedState = false;

volatile bool rgbEnabled;

void rgbLedInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE(); // 사용 포트 클럭 활성화
    GPIO_InitStruct.Pin = GREEN_PIN | BLUE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GB_PORT, &GPIO_InitStruct);

        /* 2. 방 LED 포트(GPIOC) 클럭 활성화 및 핀 설정 */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin = RED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(R_PORT, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = ROOM_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ROOM_LED_PORT, &GPIO_InitStruct);

    // 초기 상태: 흰색
    rgbLedSetWhite();
    roomLedOn();
}

// R, G, B 모두 Turn ON -> 흰색
void rgbLedSetWhite(void)
{
    HAL_GPIO_WritePin(R_PORT, RED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GB_PORT, GREEN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GB_PORT, BLUE_PIN, GPIO_PIN_SET);
    RgbState = true;
}

// R만 Turn ON, G/B Turn OFF -> 빨간색
void rgbLedSetRed(void)
{
    HAL_GPIO_WritePin(R_PORT, RED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GB_PORT, GREEN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GB_PORT, BLUE_PIN, GPIO_PIN_RESET);
}

// RGB 전체 OFF
void rgbLedOff(void)
{
    HAL_GPIO_WritePin(R_PORT, RED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GB_PORT, GREEN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GB_PORT, BLUE_PIN, GPIO_PIN_RESET);
    RgbState = false;
}

/* ---------------- 방불(Room LED) ---------------- */

void roomLedOn(void)
{
    HAL_GPIO_WritePin(ROOM_LED_PORT, ROOM_LED_PIN, GPIO_PIN_SET);
    roomLedState = true;
}

void roomLedOff(void)
{
    HAL_GPIO_WritePin(ROOM_LED_PORT, ROOM_LED_PIN, GPIO_PIN_RESET);
    roomLedState = false;
}


void rgbLedToggle(void)
{
    if (RgbState)
        rgbLedOff();
    else
        rgbLedSetWhite();
}

void roomLedToggle(void)
{
    if (roomLedState)
        roomLedOff();
    else
        roomLedOn();
}

/* ---------------- RGB On/Off ---------------- */
bool rgbIsEnabled(void)
{
    rgbEnabled = RgbState;
    return rgbEnabled;
}
