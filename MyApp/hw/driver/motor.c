#include "motor.h"
#include "reed.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"

// =================================
// 창문/팬 상태
// =================================
static WindowState_t window_state        = WINDOW_STOP;
static uint8_t       window_fully_open   = 0;
static uint8_t       window_fully_closed = 0;
static uint16_t      fan_speed           = 0;
// 서보 상태
static uint8_t blind_state = 0;
static uint8_t lock_state = 0;


// =================================
// 모터 제어
// =================================
void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // ENA
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // ENB
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // Blind Servo
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2); // Door Servo

    // Motor1_Stop();
    // Motor2_Stop();

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 500);
    blind_state = 0;

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 500);
    lock_state = 1;
}

void Motor1_Forward(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
}

void Motor1_Backward(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
}

void Motor1_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
}

void Motor2_Forward(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
}

void Motor2_Backward(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
}

void Motor2_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
}


// =================================
// 창문 제어 (토글 + 리드 한계)
// =================================
void Motor_ToggleOpen(void)
{
    if (window_state == WINDOW_OPENING)
    {
        Motor1_Stop();
        window_state = WINDOW_STOP;
    }
    else if (!window_fully_open)
    {
        Motor1_Forward(250);
        window_state = WINDOW_OPENING;
        window_fully_closed = 0;
    }
}

void Motor_ToggleClose(void)
{
    if (window_state == WINDOW_CLOSING)
    {
        Motor1_Stop();
        window_state = WINDOW_STOP;
    }
    else if (!window_fully_closed)
    {
        Motor1_Backward(250);
        window_state = WINDOW_CLOSING;
        window_fully_open = 0;
    }
}

void Motor_CheckLimits(void)
{
    if (window_state == WINDOW_OPENING && Reed_Open_IsDetected())
    {
        Motor1_Stop();
        window_state = WINDOW_STOP;
        window_fully_open = 1;
    }

    if (window_state == WINDOW_CLOSING && Reed_Close_IsDetected())
    {
        Motor1_Stop();
        window_state = WINDOW_STOP;
        window_fully_closed = 1;
    }
}


// =================================
// 팬 제어
// =================================
void Motor_ToggleFan(void)
{
    if (fan_speed == 0)
    {
        fan_speed = 300;
    }
    else if (fan_speed == 300)
    {
        fan_speed = 600;
    }
    else if (fan_speed == 600)
    {
        fan_speed = 999;
    }
    else
    {
        fan_speed = 0;
    }

    if (fan_speed == 0)
    {
        Motor2_Stop();
    }
    else
    {
        Motor2_Forward(fan_speed);
    }
}

void Motor_ToggleBlind(void)
{
    if (blind_state == 0)
    {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 1500);
        blind_state = 1;
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 500);
        blind_state = 0;
    }
}


// =================================
// 도어락 서보 제어
// =================================
void Motor_Lock(void)
{
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 500);
    lock_state = 1;
}

void Motor_Unlock(void)
{
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 1500);
    lock_state = 0;
}

uint8_t Motor_IsLocked(void)
{
    return lock_state;
}