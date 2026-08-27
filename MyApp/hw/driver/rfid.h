#pragma once

#include "main.h"

#define RFID_MAX_UID_LEN     4
#define RFID_MAX_CARDS       10

void RFID_Init(void);
uint8_t RFID_ReadUID(void);
uint8_t RFID_IsAuthorized(void);

uint8_t RFID_CheckAndGetEdge(uint8_t *out_uid);
void RFID_RegisterCard(const uint8_t *uid);
uint8_t RFID_IsRegisteredUID(const uint8_t *uid);

extern uint8_t rfid_uid[RFID_MAX_UID_LEN];
extern uint8_t rfid_uid_size;
extern uint8_t rfid_card_detected;