#pragma once

#include "main.h"

#define IR_PORT GPIOA
#define IR_PIN  GPIO_PIN_9

void myVs1838b_Init(void);
void myVs1838b_Process(void);

uint8_t myVs1838b_IsReceived(void);
uint32_t myVs1838b_GetCode(void);