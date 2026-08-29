#pragma once

#include "main.h"
#include <stdint.h>

// STM32CubeMX에서 설정한 포트 및 핀 정의 (사용환경에 맞춰 수정)
#define BUTTON          GPIOB
#define RGB_BTN_PIN     GPIO_PIN_1   // RGB On/Off 스위치
#define ROOM_BTN_PIN    GPIO_PIN_2   // 방불(LED) On/Off 스위치

void Button_Init(void);

// 현재 눌려있는지 (Level)
uint8_t Button_Open_Pressed(void);
uint8_t Button_Close_Pressed(void);
uint8_t Button_Fan_Pressed(void);

uint8_t Button_RGB_Pressed(void);
uint8_t Button_LED_Pressed(void);

// 눌리는 "순간"에만 1을 리턴 (Rising edge, 내부에서 prev 상태 관리)
uint8_t Button_Open_Edge(void);
uint8_t Button_Close_Edge(void);
uint8_t Button_Fan_Edge(void);

uint8_t Button_Blind_Pressed(void);
uint8_t Button_Blind_Edge(void);

uint8_t Button_RGB_Edge(void);
uint8_t Button_LED_Edge(void);