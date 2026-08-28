#include "apMain.h"
#include "myAdcWater.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "myDht11.h"
#include "myFlame.h"
#include "myUart.h"
#include "myBle.h"   // 추가
#include "myRgb.h"   // 추가
#include "myBuzzer.h"   // 추가

#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern ADC_HandleTypeDef hadc1;
void apInit(void)
{
    uartInit();
    adcInit();
    dht11Init();
    flameInit();
    bleInit();       // 추가
    rgbLedInit();
    buzzerInit();    // 추가: 부저 초기화
}

float water_depth = 0;
dht11Data_t dht_data = {0};
bool dht_status=false;


void apMain(void)
{
    uint32_t last_dht_tick = 0;
    dht11Data_t dht_data = {0};

        /* 연결 확인용 최초 인사 메시지 */
    bleSend("STM32 BLE Ready\r\n");
    while (1) {
        uint32_t current_tick = HAL_GetTick();

        adcUpdate();
        bool is_fire = flameIsDetected();
        //water_depth = adcGetWater();

        // printf(">water_mm:%.2f\n", adcGetWater());
        // printf(">water_raw:%lu\n", Adc_GetWaterRaw());
        // dht_status = dht11Read(&dht_data);

        // HAL_Delay(10);

                /* BLE 수신 확인 */
        if (bleAvailable())
        {
            char line[BLE_RX_BUF_SIZE];
            bleGetLine(line, sizeof(line));
            printf("BLE RX: %s\r\n", line);   // USART2(디버그)로 로그

            /* 받은 문자열 그대로 폰에 에코 (동작 확인용) */
            bleSend("Echo: ");
            bleSend(line);
            bleSend("\r\n");
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
            rgbLedSetRed();
            buzzerOn();       // 부저 삐- 경보음
        }
        else
        {
            rgbLedSetWhite(); // 평상시 -> 흰색
            buzzerOff();      // 부저 끔
        }

        HAL_Delay(10);
    
        
    }
    
}