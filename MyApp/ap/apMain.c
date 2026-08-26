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
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
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
    adcInit();
    dht11Init();
    flameInit();
    uartInit(); // uart 송신 초기화
}
extern ADC_HandleTypeDef hadc1;

float water_depth = 0;
dht11Data_t dht_data = {0};
bool dht_status=false;


void apMain(void)
{
    uint32_t tick_1000 = 0;
    uint32_t current_tick = 0;
    int mod_num = 0;
    uint32_t last_dht_tick = 0;

    ssd1306Clear();

    while (1) {
        current_tick = HAL_GetTick();

        adcUpdate();
        bool is_fire = flameIsDetected();
        
        int num = get_keynum();
        if (num != -1) {
            mod_num = num;
        }
        ssd1306Show(mod_num);
        // ssd1306Video();
        if (current_tick - tick_1000 >= 1000) {
            tick_1000 = current_tick;
            lcdThermometer(dht_data.temperature, dht_data.humidity);
            // lcdchangemod(mod_num);
        }

        if (current_tick - last_dht_tick >= 1000)
        {
            last_dht_tick = HAL_GetTick();

            /* DHT11 데이터 읽기 */
            dht11Read(&dht_data);

            /* Teleplot 전송 포맷 (>변수명:값\n) */
            printf("water_mm:%.2f | ", adcGetWater());
            printf("water_raw:%lu\r\n", Adc_GetWaterRaw());
            printf("temperature:%.1f | ", dht_data.temperature);
            printf("humidity:%.1f\r\n", dht_data.humidity);
            printf("fire_alarm:%d\n", is_fire ? 1 : 0); // 화재 시 1, 정상 시 0
        }

        /* 3. 화재 발생 시 즉시 긴급 처리 (예: 능동 부저 울림, 모터 차단 등) */
        if (is_fire)
        {
            // TODO: 키트에 있는 Active Buzzer(능동 부저)나 RGB LED 제어 코드 추가 가능
        }
        HAL_Delay(10);
    }
}