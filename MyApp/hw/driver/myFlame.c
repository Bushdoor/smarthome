#include "myFlame.h"

void flameInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIOB 클럭 활성화 (사용하는 포트에 맞춰 수정) */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* GPIO 입력 모드 설정 */
    GPIO_InitStruct.Pin = FLAME_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // 기본 HIGH 유지
    HAL_GPIO_Init(FLAME_PORT, &GPIO_InitStruct);
}

/**
 * @brief 불꽃 감지 여부 확인
 * @return true: 화재(불꽃) 감지됨, false: 정상 상태
 */
bool flameIsDetected(void)
{
    // 액티브 LOW: 불꽃 감지 시 LOW(0) 반환
    return (HAL_GPIO_ReadPin(FLAME_PORT, FLAME_PIN) == GPIO_PIN_RESET);
}