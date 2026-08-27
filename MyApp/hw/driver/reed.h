#pragma once

#include "main.h"

void Reed_Init(void);

uint8_t Reed_Open_IsDetected(void);
uint8_t Reed_Close_IsDetected(void);
uint8_t Reed_Door_IsDetected(void);

uint8_t Reed_Door_Lock_Condition(void);
