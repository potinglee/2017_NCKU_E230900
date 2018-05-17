#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stdlib.h"
#include "string.h"
#include "misc.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include "task.h"
#include "queue.h"

//typedef void * QueueHandle_t;
unsigned char Receive_data;
int delay_time;
uint16_t Receive_value;
const uint16_t LED[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
QueueHandle_t xQueue;

void USART2_IRQHandler();
void Init();
void delay_function(int);
void vApplicationTickHook() {}
void vApplicationStackOverflowHook() {}
void vApplicationIdleHook() {}
void vApplicationMallocFailedHook() {}
