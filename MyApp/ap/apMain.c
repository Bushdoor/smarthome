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
    
    ssd1306Clear();
    // ssd1306DrawRect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, SSD1306_COLOR_WHITE);
    // ssd1306DrawString(14, 3, "Team : DISMISSED", SSD1306_COLOR_WHITE);
    // ssd1306DrawString(12, 11, "SmartHome Project", SSD1306_COLOR_WHITE);
    // ssd1306DrawLine(0, 21, 126, 21, SSD1306_COLOR_WHITE);
    // ssd1306Update();
    
    while(1) {
        current_tick = HAL_GetTick();


        if (current_tick - tick_1000 >= 1000) {
            tick_1000 = current_tick;
            lcdCalender();
        }

    //badapple
     /*
      * 정확히 1024바이트 수신 완료
      */
    if (frame_ready) {
      frame_ready = 0;
      /*
      * UART DMA로 받은 프레임을
      * SSD1306 화면 버퍼로 복사
      */
      ssd1306DrawFrame(frame_buf);

      /*
      * OLED에 실제 출력
      */
      ssd1306Update();

      /*
        * Python에게
        * "이 프레임 출력 끝났음"
        * ACK 전송
        */
      uint8_t ack = 'A';
      HAL_UART_Transmit( &huart2, &ack,1, 100 );

      /*
      * 다음 프레임 1024바이트 DMA 수신 시작
      */
      HAL_UART_Receive_DMA( &huart2, frame_buf, FRAME_SIZE );
    }
    }
}