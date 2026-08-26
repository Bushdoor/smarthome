#include "motor.h"
#include "reed.h"
#include "tim.h"

#define MOTOR1_IN1_GPIO_Port   GPIOC
#define MOTOR1_IN1_Pin         GPIO_PIN_0

#define MOTOR1_IN2_GPIO_Port   GPIOC
#define MOTOR1_IN2_Pin         GPIO_PIN_1

#define MOTOR2_IN3_GPIO_Port   GPIOC
#define MOTOR2_IN3_Pin         GPIO_PIN_2

#define MOTOR2_IN4_GPIO_Port   GPIOC
#define MOTOR2_IN4_Pin         GPIO_PIN_3


// =================================
// 창문/팬 상태 (모터 모듈 내부에서만 관리)
// =================================
static WindowState_t window_state       = WINDOW_STOP;
static uint8_t       window_fully_open   = 0;  // 열림 리드가 감지되면 1, 닫힘 버튼 눌러야 해제
static uint8_t       window_fully_closed = 0;  // 닫힘 리드가 감지되면 1, 열림 버튼 눌러야 해제
static uint16_t      fan_speed           = 0;


// =================================
// 저수준 모터 제어 (기존과 동일)
// =================================
void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);   // ENA
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);   // ENB

    Motor1_Stop();
    Motor2_Stop();
}

void Motor1_Forward(uint16_t speed)
{
    HAL_GPIO_WritePin(MOTOR1_IN1_GPIO_Port, MOTOR1_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR1_IN2_GPIO_Port, MOTOR1_IN2_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
}

void Motor1_Backward(uint16_t speed)
{
    HAL_GPIO_WritePin(MOTOR1_IN1_GPIO_Port, MOTOR1_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR1_IN2_GPIO_Port, MOTOR1_IN2_Pin, GPIO_PIN_SET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
}

void Motor1_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    HAL_GPIO_WritePin(MOTOR1_IN1_GPIO_Port, MOTOR1_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR1_IN2_GPIO_Port, MOTOR1_IN2_Pin, GPIO_PIN_RESET);
}

void Motor2_Forward(uint16_t speed)
{
    HAL_GPIO_WritePin(MOTOR2_IN3_GPIO_Port, MOTOR2_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR2_IN4_GPIO_Port, MOTOR2_IN4_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
}

void Motor2_Backward(uint16_t speed)
{
    HAL_GPIO_WritePin(MOTOR2_IN3_GPIO_Port, MOTOR2_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR2_IN4_GPIO_Port, MOTOR2_IN4_Pin, GPIO_PIN_SET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
}

void Motor2_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    HAL_GPIO_WritePin(MOTOR2_IN3_GPIO_Port, MOTOR2_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR2_IN4_GPIO_Port, MOTOR2_IN4_Pin, GPIO_PIN_RESET);
}


// =================================
// 고수준 창문 제어 (토글 + 리드 한계)
// =================================
void Motor_ToggleOpen(void)
{
    if (window_state == WINDOW_OPENING)
    {
        // 열리는 중에 다시 누르면 정지
        Motor1_Stop();
        window_state = WINDOW_STOP;
    }
    else if (!window_fully_open)
    {
        // 정지 상태거나 닫히는 중이면 열기 시작 (방향 전환 포함)
        Motor1_Forward(250);
        window_state = WINDOW_OPENING;
        window_fully_closed = 0;
    }
}

void Motor_ToggleClose(void)
{
    if (window_state == WINDOW_CLOSING)
    {
        // 닫히는 중에 다시 누르면 정지
        Motor1_Stop();
        window_state = WINDOW_STOP;
    }
    else if (!window_fully_closed)
    {
        // 정지 상태거나 열리는 중이면 닫기 시작 (방향 전환 포함)
        Motor1_Backward(250);
        window_state = WINDOW_CLOSING;
        window_fully_open = 0;
    }
}

void Motor_CheckLimits(void)
{
    // 열리는 중일 때만 열림 리드 체크
    if (window_state == WINDOW_OPENING && Reed_Open_IsDetected())
    {
        Motor1_Stop();
        window_state = WINDOW_STOP;
        window_fully_open = 1;
    }

    // 닫히는 중일 때만 닫힘 리드 체크
    if (window_state == WINDOW_CLOSING && Reed_Close_IsDetected())
    {
        Motor1_Stop();
        window_state = WINDOW_STOP;
        window_fully_closed = 1;
    }
}


// =================================
// 고수준 팬 제어
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