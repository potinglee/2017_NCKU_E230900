#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

const uint16_t LEDS = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
uint32_t i = 1;

void init();
void loop();
void delay();
void slowMode();
void defaultMode();
void fastMode();
void RCC_WaitForPLLStartUp(void);
