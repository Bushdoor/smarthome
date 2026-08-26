#include "apMain.h"
#include "motor.h"
#include "reed.h"
#include "button.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void apInit(void)
{
    Motor_Init();
    Reed_Init();
    Button_Init();
}

void apMain(void)
{
    while (1)
    {
        // 열림 버튼 눌리는 순간 -> 열기/정지/재개 토글
        if (Button_Open_Edge())
        {
            Motor_ToggleOpen();
        }
 
        // 닫힘 버튼 눌리는 순간 -> 닫기/정지/재개 토글
        if (Button_Close_Edge())
        {
            Motor_ToggleClose();
        }
 
        // 매 루프마다 리드 스위치로 한계 도달 체크
        Motor_CheckLimits();
 
        // 팬 버튼 눌리는 순간 -> 속도 순환
        if (Button_Fan_Edge())
        {
            Motor_ToggleFan();
        }
 
        HAL_Delay(10);
    }
}