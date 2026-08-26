#include "myServo.h"
#include "tim.h"

#define SERVO_MIN_US    500
#define SERVO_MAX_US    2500

void myServo_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

    myServo_SetAngle(90);
}

void myServo_SetAngle(uint8_t angle)
{
    if (angle > 180)
        angle = 180;

    uint16_t pulse;

    pulse = SERVO_MIN_US
          + ((SERVO_MAX_US - SERVO_MIN_US) * angle) / 180;

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
}
