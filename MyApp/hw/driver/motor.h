#pragma once

#include "main.h"
#include <stdint.h>

// =================================
// 저수준 모터 제어 (기존)
// =================================
void Motor_Init(void);

void Motor1_Forward(uint16_t speed);
void Motor1_Backward(uint16_t speed);
void Motor1_Stop(void);

void Motor2_Forward(uint16_t speed);
void Motor2_Backward(uint16_t speed);
void Motor2_Stop(void);

// =================================
// 창문 상태
// =================================
typedef enum
{
    WINDOW_STOP = 0,
    WINDOW_OPENING,
    WINDOW_CLOSING
} WindowState_t;

// =================================
// 고수준 제어 (apMain에서 버튼 엣지만 넘겨주면 됨)
// =================================
void Motor_ToggleOpen(void);   // 열림 버튼 눌렸을 때 호출: 열기 -> 정지 -> 재개 토글
void Motor_ToggleClose(void);  // 닫힘 버튼 눌렸을 때 호출: 닫기 -> 정지 -> 재개 토글
void Motor_CheckLimits(void);  // 매 루프마다 호출: 리드 스위치로 한계 도달 시 자동 정지
void Motor_ToggleFan(void);    // 팬 버튼 눌렸을 때 호출: 0 -> 300 -> 600 -> 999 -> 0 순환