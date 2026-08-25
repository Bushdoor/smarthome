#include "apMain.h"
#include "myUart.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "myVs1838b.h"

void apInit(void){
    myVs1838b_Init();
}

void apMain(void){

    // int value = 0;

    while (1)
    {
        myVs1838b_Process();

        if (myVs1838b_IsReceived())
        {
            uint32_t raw = myVs1838b_GetCode();

            uint8_t address     = (raw >> 24) & 0xFF;
            uint8_t address_inv = (raw >> 16) & 0xFF;
            uint8_t command     = (raw >> 8)  & 0xFF;
            uint8_t command_inv = raw & 0xFF;

            uint8_t address_ok = ((address ^ address_inv) == 0xFF);
            uint8_t command_ok = ((command ^ command_inv) == 0xFF);

            printf("\r\n");
            printf("RAW     = 0x%08lX\r\n", raw);

            printf("ADDR    = 0x%02X\r\n", address);
            printf("~ADDR   = 0x%02X\r\n", address_inv);

            printf("CMD     = 0x%02X\r\n", command);
            printf("~CMD    = 0x%02X\r\n", command_inv);

            printf("ADDR OK = %s\r\n", address_ok ? "YES" : "NO");
            printf("CMD  OK = %s\r\n", command_ok ? "YES" : "NO");

            if (address_ok && command_ok)
            {
                printf(">>> NEC VALID <<<\r\n");
            }
            else
            {
                printf(">>> NEC INVALID <<<\r\n");
            }
        }

        HAL_Delay(1);
    }
}