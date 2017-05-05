#include "main.h"

volatile uint32_t i;

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

    SysTick_Config(SystemCoreClock / 1000);
}

void loop() {
    GPIO_ToggleBits(GPIOD, LEDS);

    delay(1000);
}

void delay(uint32_t delay_time) {
    i = delay_time;
    while(i != 0);
}

void SysTick_Handler(void) {
    if(i != 0) i--;
}
