#ifndef MY_BUZZER_H_
#define MY_BUZZER_H_

#include "stm32f4xx_hal.h"

#define BUZZER_PORT    GPIOB
#define BUZZER_PIN     GPIO_PIN_0

void buzzerInit(void);
void buzzerOn(void);
void buzzerOff(void);

#endif /* MY_BUZZER_H_ */