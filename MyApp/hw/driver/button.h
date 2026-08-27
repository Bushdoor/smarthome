#pragma once

#include "main.h"
#include <stdint.h>

void Button_Init(void);

// 현재 눌려있는지 (Level)
uint8_t Button_Open_Pressed(void);
uint8_t Button_Close_Pressed(void);
uint8_t Button_Fan_Pressed(void);

// 눌리는 "순간"에만 1을 리턴 (Rising edge, 내부에서 prev 상태 관리)
uint8_t Button_Open_Edge(void);
uint8_t Button_Close_Edge(void);
uint8_t Button_Fan_Edge(void);

uint8_t Button_Blind_Pressed(void);
uint8_t Button_Blind_Edge(void);