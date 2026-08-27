#include "button.h"

void Button_Init(void)
{
}


// =================================
// 저수준 : 현재 눌림 여부
// =================================
uint8_t Button_Open_Pressed(void)
{
    return (HAL_GPIO_ReadPin(Button_Open_GPIO_Port, Button_Open_Pin) == GPIO_PIN_RESET);
}

uint8_t Button_Close_Pressed(void)
{
    return (HAL_GPIO_ReadPin(Button_Close_GPIO_Port, Button_Close_Pin) == GPIO_PIN_RESET);
}

uint8_t Button_Fan_Pressed(void)
{
    return (HAL_GPIO_ReadPin(Button_Fan_GPIO_Port, Button_Fan_Pin) == GPIO_PIN_RESET);
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