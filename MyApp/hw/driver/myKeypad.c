#include "myKeypad.h"

#define ROW1_GPIO_Port   GPIOB
#define ROW1_Pin         GPIO_PIN_1

#define ROW2_GPIO_Port   GPIOB
#define ROW2_Pin         GPIO_PIN_15

#define ROW3_GPIO_Port   GPIOB
#define ROW3_Pin         GPIO_PIN_14

#define ROW4_GPIO_Port   GPIOB
#define ROW4_Pin         GPIO_PIN_13



#define COL1_GPIO_Port   GPIOB
#define COL1_Pin         GPIO_PIN_2

#define COL2_GPIO_Port   GPIOB
#define COL2_Pin         GPIO_PIN_12

#define COL3_GPIO_Port   GPIOA
#define COL3_Pin         GPIO_PIN_11

#define COL4_GPIO_Port   GPIOA
#define COL4_Pin         GPIO_PIN_12


static const char keypadMap[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


static void keypadSetRow(uint8_t row)
{
    HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_SET);

    switch (row)
    {
        case 0:
            HAL_GPIO_WritePin(ROW1_GPIO_Port,
                              ROW1_Pin,
                              GPIO_PIN_RESET);
            break;

        case 1:
            HAL_GPIO_WritePin(ROW2_GPIO_Port,
                              ROW2_Pin,
                              GPIO_PIN_RESET);
            break;

        case 2:
            HAL_GPIO_WritePin(ROW3_GPIO_Port,
                              ROW3_Pin,
                              GPIO_PIN_RESET);
            break;

        case 3:
            HAL_GPIO_WritePin(ROW4_GPIO_Port,
                              ROW4_Pin,
                              GPIO_PIN_RESET);
            break;
    }
}


char keypadGetKey(void)
{
    for (uint8_t row = 0; row < 4; row++)
    {
        keypadSetRow(row);

        if (HAL_GPIO_ReadPin(COL1_GPIO_Port, COL1_Pin)
            == GPIO_PIN_RESET)
        {
            return keypadMap[row][0];
        }

        if (HAL_GPIO_ReadPin(COL2_GPIO_Port, COL2_Pin)
            == GPIO_PIN_RESET)
        {
            return keypadMap[row][1];
        }

        if (HAL_GPIO_ReadPin(COL3_GPIO_Port, COL3_Pin)
            == GPIO_PIN_RESET)
        {
            return keypadMap[row][2];
        }

        if (HAL_GPIO_ReadPin(COL4_GPIO_Port, COL4_Pin)
            == GPIO_PIN_RESET)
        {
            return keypadMap[row][3];
        }
    }

    return 0;
}


int get_keynum(void) {
    char key = keypadGetKey();

    if (key >= '0' && key <= '9') {
        return key - '0';
    }

    return -1;
}