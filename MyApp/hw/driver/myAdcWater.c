#include "myAdcWater.h"
#include "adc.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_ll_adc.h"

#include <stdint.h>
#include <stdbool.h>

/* 수위 센서(SZH-EK057) 캘리브레이션 설정값 */
#define WATER_RAW_MIN            0       /* 공기 중 일 때 Raw 값 */
#define WATER_RAW_MAX            2500    /* 물에 최대 감지선까지 잠겼을 때 Raw 값 */
#define WATER_MAX_MM             40.0f   /* 센서 최대 감지 길이 (40mm) */

/* DMA 전송용 버퍼 및 공유 변수 */
uint32_t adc_multi_values[1] = {0};
static volatile uint32_t water_sensor_raw = 0;
static volatile bool is_conv_done = false;

/* 수위 계산 결과 변수 */
static float calculated_water_mm = 0.0f;
static bool is_running = false;

void adcInit(void)
{
  water_sensor_raw = 0;
  is_conv_done = false;
  calculated_water_mm = 0.0f;
  is_running = true;
  
  HAL_ADC_Start_DMA(&hadc1, adc_multi_values, 1);
}



/* DMA 변환 완료 콜백 함수 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    is_conv_done = true;
    water_sensor_raw = adc_multi_values[0]; // 1번째 채널 값 수신
  }
}

/* 수위 계산 갱신 함수 (메인 루프에서 주기적 호출) */
void adcUpdate(void)
{
  if (!is_running)
    return;

  if (is_conv_done)
  {
    is_conv_done = false;

    float raw_w = (float)water_sensor_raw;
    if (raw_w < WATER_RAW_MIN) raw_w = WATER_RAW_MIN;
    if (raw_w > WATER_RAW_MAX) raw_w = WATER_RAW_MAX;

    calculated_water_mm = ((raw_w - WATER_RAW_MIN) / (float)(WATER_RAW_MAX - WATER_RAW_MIN)) * WATER_MAX_MM;
  }
}

/* 수위 계산 값 반환 (단위: mm) */
float adcGetWater(void)
{
  return calculated_water_mm;
}

/* 수위 센서 Raw ADC 값 반환 */
uint32_t Adc_GetWaterRaw(void)
{
  return water_sensor_raw;
}