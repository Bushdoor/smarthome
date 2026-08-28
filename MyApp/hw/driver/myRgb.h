#ifndef MY_RGB_LED_H_
#define MY_RGB_LED_H_

#include "stm32f4xx_hal.h"

// STM32CubeMX에서 설정한 포트 및 핀 정의 (사용환경에 맞춰 수정)
#define RGB_PORT      GPIOA
#define RED_PIN       GPIO_PIN_8
#define GREEN_PIN     GPIO_PIN_9
#define RGB_PORT_B      GPIOC
#define BLUE_PIN      GPIO_PIN_7

void rgbLedInit(void);
void rgbLedSetWhite(void);
void rgbLedSetRed(void);

#endif /* MY_RGB_LED_H_ */