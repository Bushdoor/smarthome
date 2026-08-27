#include "button.h"

void Button_Init(void)
{
}


// =================================
// 저수준 : 현재 눌림 여부
// =================================
uint8_t Button_Open_Pressed(void)
{
    return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET);
}

uint8_t Button_Close_Pressed(void)
{
    return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET);
}

uint8_t Button_Fan_Pressed(void)
{
    return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_RESET);
}

uint8_t Button_Blind_Pressed(void)
{
    return (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) == GPIO_PIN_RESET);
}


// =================================
// 고수준 : 눌리는 순간(Rising edge)에만 1
// =================================
uint8_t Button_Open_Edge(void)
{
    static uint8_t prev = 0;
    uint8_t now = Button_Open_Pressed();
    uint8_t edge = (now && !prev);
    prev = now;
    return edge;
}

uint8_t Button_Close_Edge(void)
{
    static uint8_t prev = 0;
    uint8_t now = Button_Close_Pressed();
    uint8_t edge = (now && !prev);
    prev = now;
    return edge;
}

uint8_t Button_Fan_Edge(void)
{
    static uint8_t prev = 0;
    uint8_t now = Button_Fan_Pressed();
    uint8_t edge = (now && !prev);
    prev = now;
    return edge;
}

uint8_t Button_Blind_Edge(void)
{
    static uint8_t prev = 0;
    uint8_t now = Button_Blind_Pressed();
    uint8_t edge = (now && !prev);
    prev = now;
    return edge;
}