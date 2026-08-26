#include "apMain.h"
#include "myUart.h"
#include "myI2c.h"
#include "mySsd1306.h"
#include "myRtc.h"
#include "myLcd1602.h"
#include "myKeypad.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>


uint8_t mod = 0;

void apInit(void)
{
    ds1302Init(); // RTC 초기화
    lcd1602Init(); // CLCD 초기화
    ssd1306Init(); // OLED 초기화

    i2cScan(); // i2c 송신 존재 유무 확인
    uartInit(); // uart 송신 초기화
}


void apMain(void)
{
    uint32_t tick_1000 = 0;
    uint32_t current_tick = 0;
    int mod_num = 0;
    
    ssd1306Clear();
    
    while(1) {
        current_tick = HAL_GetTick();

        int num = get_keynum();
        if (num != -1) {
            mod_num = num;
        }
        ssd1306Show(mod_num);
        // ssd1306Video();
        if (current_tick - tick_1000 >= 1000) {
            tick_1000 = current_tick;
            // lcdCalender();
            lcdchangemod(mod_num);
        }
    
    }
}