#include "myUart.h"

// printf 쓰면 hal_uart_tranmit으로 usart 호출해서 시리얼로 나감
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  /* Polling 방식으로 1바이트 전송 (전송 완료될 때까지 대기) */
  // HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}