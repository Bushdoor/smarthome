#include "myKeypad.h"

char Keypad_GetKey(void)
{
    const char keymap[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    /* Row 배열 */
    GPIO_TypeDef *row_port[4] = {
        R1_PORT,
        R2_PORT,
        R3_PORT,
        R4_PORT
    };

    uint16_t row_pin[4] = {
        R1_PIN,
        R2_PIN,
        R3_PIN,
        R4_PIN
    };

    /* Column 배열 */
    GPIO_TypeDef *col_port[4] = {
        C1_PORT,
        C2_PORT,
        C3_PORT,
        C4_PORT
    };

    uint16_t col_pin[4] = {
        C1_PIN,
        C2_PIN,
        C3_PIN,
        C4_PIN
    };


    /* 모든 Row HIGH */
    for (int row = 0; row < 4; row++)
    {
        HAL_GPIO_WritePin(row_port[row],
                          row_pin[row],
                          GPIO_PIN_SET);
    }


    /* Row 하나씩 LOW */
    for (int row = 0; row < 4; row++)
    {
        /* 현재 Row LOW */
        HAL_GPIO_WritePin(row_port[row],
                          row_pin[row],
                          GPIO_PIN_RESET);

        /* Column 검사 */
        for (int col = 0; col < 4; col++)
        {
            if (HAL_GPIO_ReadPin(col_port[col],
                                 col_pin[col]) == GPIO_PIN_RESET)
            {
                /* Row 복구 */
                HAL_GPIO_WritePin(row_port[row],
                                  row_pin[row],
                                  GPIO_PIN_SET);

                return keymap[row][col];
            }
        }

        /* 현재 Row HIGH */
        HAL_GPIO_WritePin(row_port[row],
                          row_pin[row],
                          GPIO_PIN_SET);
    }

    return 0;
}


/* 설명

R1 LOW
 ├─ C1 LOW → '1'
 ├─ C2 LOW → '2'
 ├─ C3 LOW → '3'
 └─ C4 LOW → 'A'

R2 LOW
 ├─ C1 LOW → '4'
 ├─ C2 LOW → '5'
 ├─ C3 LOW → '6'
 └─ C4 LOW → 'B'

R3 LOW
 ├─ C1 LOW → '7'
 ├─ C2 LOW → '8'
 ├─ C3 LOW → '9'
 └─ C4 LOW → 'C'

R4 LOW
 ├─ C1 LOW → '*'
 ├─ C2 LOW → '0'
 ├─ C3 LOW → '#'
 └─ C4 LOW → 'D'

 */