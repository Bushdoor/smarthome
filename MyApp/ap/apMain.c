#include "apMain.h"
#include "myUart.h"
#include "myI2c.h"
#include "mySsd1306.h"
#include "myRtc.h"
#include "myLcd1602.h"
#include "myKeypad.h"
#include "myAdcWater.h"
#include "myDht11.h"
#include "myFlame.h"
#include "motor.h"
#include "reed.h"
#include "button.h"

#include "myTest.h"

#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_gpio.h"


#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern ADC_HandleTypeDef hadc1;

uint8_t mod = 0;
float water_depth = 0;
dht11Data_t dht_data = {0};
bool dht_status=false;
int change_screen = 0;

extern int mod_num;

void apInit(void)
{
    ds1302Init(); // RTC 초기화
    lcd1602Init(); // CLCD 초기화
    ssd1306Init(); // OLED 초기화

    Motor_Init();
    Reed_Init();
    Button_Init();

    i2cScan(); // i2c 송신 존재 유무 확인
    adcInit();
    dht11Init();
    flameInit();
    uartInit(); // uart 송신 초기화

    dht11Read(&dht_data); // 처음에 온도 0뜨는거 보기싫어서 Init할때 한번 읽기

}


void apMain(void)
{
    uint32_t tick_250 = 0;
    uint32_t tick_1000 = 0;
    uint32_t current_tick = 0;
    // int mod_num = 0;

    ssd1306Clear();

    while (1) {
        current_tick = HAL_GetTick();
        adcUpdate();
        
        // 4x4패드 입력받아서 모드변경
        // int num = get_keynum();
        // if (num != -1) {
        //     mod_num = num;
        // }

        
        if (current_tick - tick_1000 >= 1000) {
            tick_1000 = current_tick;
            
            /* DHT11 데이터 읽기 */
            dht11Read(&dht_data);

            change_screen++;
            if (change_screen == 5) { change_screen = 0;}
        }
        ssd1306Show(mod_num);
        
        if (current_tick - tick_250 >= 250) {
            tick_250 = current_tick;

            lcdchangemod(mod_num);
            printf("%lu\n", Adc_GetWaterRaw());


        }

        /*--------------모터부분----------------------*/
        // 열림 버튼 눌리는 순간 -> 열기/정지/재개 토글
        if (Button_Open_Edge())
        {
            Motor_ToggleOpen();
        }
 
        // 닫힘 버튼 눌리는 순간 -> 닫기/정지/재개 토글
        if (Button_Close_Edge())
        {
            Motor_ToggleClose();
        }
 
        // 매 루프마다 리드 스위치로 한계 도달 체크
        Motor_CheckLimits();
 
        // 팬 버튼 눌리는 순간 -> 속도 순환
        if (Button_Fan_Edge())
        {
            Motor_ToggleFan();
        }

        /*--------------------------*/
        
    }
}





/* Debug
    ------------DHT11------------------------
    Teleplot 전송 포맷 (>변수명:값\n)
    printf("water_mm:%.2f | ", adcGetWater());
    printf("water_raw:%lu\r\n", Adc_GetWaterRaw());
    printf("temperature:%.1f | ", dht_data.temperature);
    printf("humidity:%.1f\r\n", dht_data.humidity);
    printf("fire_alarm:%d\n", is_fire ? 1 : 0); // 화재 시 1, 정상 시 0




*/
