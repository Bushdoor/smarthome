#include "myKeypad.h"
#include "gpio.h"
#include "tim.h"
#include <stdio.h>
#include "myServo.h"


/* =========================
 * Keymap
 * ========================= */

static const char keymap[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


/* =========================
 * Row / Column 배열
 * ========================= */

static GPIO_TypeDef *rowPort[4] =
{
    R1_PORT,
    R2_PORT,
    R3_PORT,
    R4_PORT
};

static uint16_t rowPin[4] =
{
    R1_PIN,
    R2_PIN,
    R3_PIN,
    R4_PIN
};

static GPIO_TypeDef *colPort[4] =
{
    C1_PORT,
    C2_PORT,
    C3_PORT,
    C4_PORT
};

static uint16_t colPin[4] =
{
    C1_PIN,
    C2_PIN,
    C3_PIN,
    C4_PIN
};


/* =========================
 * 내부 변수
 * ========================= */

static uint8_t currentRow = 0;

/* 현재 전체 스캔에서 발견된 키 */
static char detectedKey = 0;

/* 디바운싱 중인 키 */
static char debounceKey = 0;

/* 현재 안정적으로 눌려있는 키 */
static char pressedKey = 0;

/* 디바운싱 카운터 */
static uint8_t debounceCount = 0;

/* 새로운 키 입력 이벤트 */
static volatile char keyEvent = 0;


/* =========================
 * Row 제어
 * ========================= */

static void Keypad_SetAllRowsHigh(void)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        HAL_GPIO_WritePin(rowPort[i],
                          rowPin[i],
                          GPIO_PIN_SET);
    }
}


static void Keypad_SetRow(uint8_t row)
{
    Keypad_SetAllRowsHigh();

    if (row < 4)
    {
        HAL_GPIO_WritePin(rowPort[row],
                          rowPin[row],
                          GPIO_PIN_RESET);
    }
}


/* =========================
 * Column 읽기
 * ========================= */

static char Keypad_ReadColumn(void)
{
    for (uint8_t col = 0; col < 4; col++)
    {
        if (HAL_GPIO_ReadPin(colPort[col],
                             colPin[col]) == GPIO_PIN_RESET)
        {
            return keymap[currentRow][col];
        }
    }

    return 0;
}


/* =========================
 * 초기화
 * ========================= */

void myKeypad_Init(void)
{
    currentRow = 0;

    detectedKey = 0;
    debounceKey = 0;
    pressedKey = 0;
    debounceCount = 0;
    keyEvent = 0;

    /* 모든 Row HIGH */
    Keypad_SetAllRowsHigh();

    /* R1 LOW */
    Keypad_SetRow(currentRow);

    /* TIM4 인터럽트 시작 */
    HAL_TIM_Base_Start_IT(&htim4);
}


/* =========================
 * 1ms Scan
 * ========================= */

static void myKeypad_Scan1ms(void)
{
    char key;

    /* =========================
     * 현재 Row 검사
     * ========================= */

    key = Keypad_ReadColumn();

    if (key != 0)
    {
        detectedKey = key;
    }


    /* =========================
     * 다음 Row
     * ========================= */

    currentRow++;

    if (currentRow >= 4)
    {
        currentRow = 0;


        /* =========================
         * 전체 Row 스캔 완료
         * 약 4ms마다 실행
         * ========================= */

        if (detectedKey != 0)
        {
            /* =========================
             * 디바운싱
             * ========================= */

            if (detectedKey == debounceKey)
            {
                /*
                 * 같은 키가 계속 검출됨
                 */
                if (debounceCount < 5)
                {
                    debounceCount++;
                }
            }
            else
            {
                /*
                 * 새로운 키 검출
                 */
                debounceKey = detectedKey;
                debounceCount = 1;
            }


            /* =========================
             * 디바운싱 완료
             * ========================= */

            if (debounceCount >= 5)
            {
                /*
                 * 아직 눌림 처리되지 않은 경우
                 * 이벤트를 딱 한 번 발생
                 */
                if (pressedKey == 0)
                {
                    keyEvent = debounceKey;
                    pressedKey = debounceKey;
                }
            }
        }
        else
        {
            /* =========================
             * 키를 뗌
             * ========================= */

            debounceKey = 0;
            debounceCount = 0;
            pressedKey = 0;
        }


        /* =========================
         * 다음 전체 스캔 준비
         * ========================= */

        detectedKey = 0;
    }


    /* =========================
     * 다음 Row 설정
     * ========================= */

    Keypad_SetRow(currentRow);
}


/* =========================
 * Keypad Process
 * ========================= */

void myKeypad_Process(void)
{
    char key;

    /* 새로운 이벤트가 없으면 바로 종료 */
    if (keyEvent == 0)
    {
        return;
    }


    /* 이벤트 가져오기 */
    key = keyEvent;


    /* 이벤트 처리 완료 */
    keyEvent = 0;


    /* =========================
     * KEY ACTION
     * ========================= */

    switch (key)
    {
        case '0':
            printf("0\r\n");
            break;

        case '1':
            printf("1\r\n");
            break;

        case '2':
            printf("2\r\n");
            break;

        case '3':
            printf("3\r\n");
            break;

        case '4':
            printf("4\r\n");
            break;

        case '5':
            printf("5\r\n");
            break;

        case '6':
            printf("6\r\n");
            break;

        case '7':
            printf("7\r\n");
            break;

        case '8':
            printf("8\r\n");
            break;

        case '9':
            printf("9\r\n");
            break;

        case 'A':
            printf("A\r\n");
            myServo_SetAngle(0);
            break;

        case 'B':
            printf("B\r\n");
            myServo_SetAngle(90);
            break;

        case 'C':
            printf("C\r\n");
            myServo_SetAngle(180);
            break;

        case 'D':
            printf("D\r\n");
            break;

        case '*':
            printf("*\r\n");
            break;

        case '#':
            printf("#\r\n");
            break;

        default:
            break;
    }
}


/* =========================
 * TIM4 Callback
 * ========================= */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        myKeypad_Scan1ms();
    }
}
