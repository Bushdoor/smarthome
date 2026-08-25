#include "apMain.h"
#include "myUart.h"
#include "myI2c.h"
#include "mySsd1306.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>


void apInit(void)
{

    ssd1306Init();

    i2cScan();
    uartInit();
}


void apMain(void)
{
    ssd1306Clear();
    while(1) {
        
        ssd1306DrawRect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, SSD1306_COLOR_WHITE);
        ssd1306DrawString(8, 3, "STM32 MULTI-SENSOR", SSD1306_COLOR_WHITE);
        ssd1306DrawLine(4, 13, 124, 13, SSD1306_COLOR_WHITE);
        ssd1306Update();
    }
}