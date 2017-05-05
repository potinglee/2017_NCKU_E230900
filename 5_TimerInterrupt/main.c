#include "main.h"

int delay_time = 0;

void delay(int n) {
    delay_time = n;
    while(delay_time != 0);
}

int main() {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Pin = LEDS;
    GPIO_Init(GPIOD, &gpio);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 2099;
    TIM_TimeBaseStructure.TIM_Prescaler = 39;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void loop() {
    GPIO_ToggleBits(GPIOD, LEDS);

    delay(1000);
}

void TIM4_IRQHandler() {
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        delay_time--;
    }
}
