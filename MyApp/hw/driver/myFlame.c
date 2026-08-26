#include "myFlame.h"

static volatile bool s_fire_detected = false;

void flameInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIOA 클럭 활성화 */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* PA4 인터럽트 모드 설정 */
    GPIO_InitStruct.Pin = FLAME_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Active Low (High -> Low 감지)
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(FLAME_PORT, &GPIO_InitStruct);

    /* EXTI Line 4 인터럽트 우선순위 설정 및 활성화 */
    HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    /* 초기 상태 확인 */
    s_fire_detected = (HAL_GPIO_ReadPin(FLAME_PORT, FLAME_PIN) == GPIO_PIN_RESET);
}

bool flameIsDetected(void)
{
    return (HAL_GPIO_ReadPin(FLAME_PORT, FLAME_PIN) == GPIO_PIN_RESET);
}

/* PA4 핀에서 인터럽트 발생 시 즉시 호출되는 콜백 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == FLAME_PIN) // GPIO_PIN_4
    {
        /* 불꽃이 감지되는 0.001초 순간 즉시 실행되는 영역 */
        s_fire_detected = true;
        
        // 예: 긴급 비상 처리 (부저 울림, LED 켜기 등)
    }
}