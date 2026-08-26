#pragma once

#include "main.h"

/* =========================
 * Keypad Pin Definition
 * ========================= */

// Row
#define R1_PORT     GPIOA
#define R1_PIN      GPIO_PIN_11

#define R2_PORT     GPIOB
#define R2_PIN      GPIO_PIN_12

#define R3_PORT     GPIOB
#define R3_PIN      GPIO_PIN_2

#define R4_PORT     GPIOB
#define R4_PIN      GPIO_PIN_1

// Column
#define C1_PORT     GPIOA
#define C1_PIN      GPIO_PIN_12

#define C2_PORT     GPIOC
#define C2_PIN      GPIO_PIN_5

#define C3_PORT     GPIOC
#define C3_PIN      GPIO_PIN_6

#define C4_PORT     GPIOC
#define C4_PIN      GPIO_PIN_8


char Keypad_GetKey(void);