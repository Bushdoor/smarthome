#include "apMain.h"
#include "myUart.h"
#include "myI2c.h"
#include "mySsd1306.h"
#include "myRtc.h"
#include "myLcd1602.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>




void apInit(void)
{
    ds1302Init();
    lcd1602Init();
    ssd1306Init();

    i2cScan();
    uartInit();
}


void apMain(void)
{
    uint32_t tick_1000 = 0;
    uint32_t current_tick = 0;

    ssd1306Clear();
    ssd1306DrawRect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, SSD1306_COLOR_WHITE);
    ssd1306DrawString(8, 3, "STM32 MULTI-SENSOR", SSD1306_COLOR_WHITE);
    ssd1306DrawLine(4, 13, 124, 13, SSD1306_COLOR_WHITE);
    ssd1306Update();
    
    while(1) {
        lcdCalender();
        HAL_Delay(500);
        if (current_tick - tick_1000 >= 1000) {
            tick_1000 = current_tick;
        }
    }
}