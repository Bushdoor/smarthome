#include "myTest.h"

int mod_num = 0;

/* PA4 핀에서 인터럽트 발생 시 즉시 호출되는 콜백 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == FLAME_PIN) // GPIO_PIN_4
    {
        /* 불꽃이 감지되는 0.001초 순간 즉시 실행되는 영역 */
        // is_fired = true;
      printf("PIN_CHECKED\n");
        
        // 예: 긴급 비상 처리 (부저 울림, LED 켜기 등)
    }

    
  if(GPIO_Pin==GPIO_PIN_13){
    mod_num++;
    if (mod_num == 4) {mod_num = 0;}
  }
}