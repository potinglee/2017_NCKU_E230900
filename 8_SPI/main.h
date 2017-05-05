#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"
#include "math.h"

const u16 LEDS = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
const u16 LED1 = GPIO_Pin_12;
const u16 LED2 = GPIO_Pin_13;
const u16 LED3 = GPIO_Pin_14;
const u16 LED4 = GPIO_Pin_15;
const u32 LIS302DL_FLAG_TIMEOUT = 0x1000;
const u32 PWM_PERIOD = 4000;

void init(void);
void loop(void);
void delay(u32 ms);

void initLeds(void);
void initSpi(void);
void LIS302DL_Init(void);
void initTimer(void);
void initPWM(void);
void SysInit(void);
void mySPI_SendData(uint8_t, uint8_t);
uint8_t mySPI_GetData(uint8_t);
