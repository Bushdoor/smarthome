#ifndef MY_RGB_LED_H_
#define MY_RGB_LED_H_

#include "stm32f4xx_hal.h"

// STM32CubeMX에서 설정한 포트 및 핀 정의 (사용환경에 맞춰 수정)
#define GB_PORT       GPIOA
#define GREEN_PIN     GPIO_PIN_11
#define BLUE_PIN      GPIO_PIN_8

#define R_PORT        GPIOC
#define RED_PIN       GPIO_PIN_5


#define ROOM_LED_PORT GPIOC
#define ROOM_LED_PIN  GPIO_PIN_6

extern volatile bool rgbEnabled;


void rgbLedInit(void);
void rgbLedSetWhite(void);
void rgbLedSetRed(void);

void roomLedOn(void);
void roomLedOff(void);



void rgbLedInit(void);
void rgbLedSetWhite(void);
void rgbLedSetRed(void);
void rgbLedOff(void);
/* ---------------- 방불(Room LED) ---------------- */
void roomLedOn(void);
void roomLedOff(void);

void rgbLedToggle(void);
void roomLedToggle(void);

bool rgbIsEnabled(void);
#endif /* MY_RGB_LED_H_ */