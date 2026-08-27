#pragma once
#include "main.h"
#include <stdbool.h>
#include <stdio.h>

// 불꽃 감지 센서 연결 핀 정의 (프로젝트 환경에 맞춰 수정)
#define FLAME_PORT GPIOA
#define FLAME_PIN  GPIO_PIN_4

extern volatile bool is_fired;


void flameInit(void);
bool flameIsDetected(void);
void flameUpdate(void);