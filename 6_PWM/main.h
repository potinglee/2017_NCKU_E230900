#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

const uint16_t LEDS = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
const uint16_t LED[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
const uint16_t USER_BUTTON = GPIO_Pin_0;

int brightLed1 = 0;
int brightLed2 = 5;
int delta = 1;
int lastButtonStatus = RESET;
int currentButtonStatus;

void init();
void loop();

void delay();

void initLeds();
void initButton();
void initTimer();
void initPWM();
