#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stdlib.h"
#include "string.h"
#include "misc.h"
#include <stdio.h>

unsigned char Receive_data;
int delay_time;
uint16_t Receive_value;

void USART2_IRQHandler();
void Init();
void delay_function(int);
