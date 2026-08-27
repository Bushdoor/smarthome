#include "apMain.h"
#include "motor.h"
#include "reed.h"
#include "button.h"
#include "rfid.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void apInit(void)
{
    Motor_Init();
    Reed_Init();
    Button_Init();
    RFID_Init();

    // 등록된 카드
    uint8_t card1[4] = {0x2E, 0x23, 0x25, 0x07};
    RFID_RegisterCard(card1);
}

void apMain(void)
{
    uint8_t tagged_uid[4];

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

        // 블라인드 토글
        if (Button_Blind_Edge())
        {
            Motor_ToggleBlind();
        }

        // ---- RFID 인증 (등록된 카드가 새로 태그됐을 때만 언락) ----
        if (RFID_CheckAndGetEdge(tagged_uid))
        {
            if (RFID_IsRegisteredUID(tagged_uid))
            {
                Motor_Unlock();
            }
        }

        // 문 리드스위치 3초 조건 (이미 잠겨있으면 동작 안 함)
        if (Reed_Door_Lock_Condition())
        {
            Motor_Lock();
        }

        HAL_Delay(10);
    }
}