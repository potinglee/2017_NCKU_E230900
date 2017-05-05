#include "main.h"

/* leds in the board will fade */

int main(void) {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    initButton();
    initLeds();
    initTimer();
    initPWM();
}

void loop() {
    switch(brightLed1) {
        case 299:delta=-1; break;
        case 1:delta=1; break;
    }

    brightLed1 += delta;

    TIM_SetCompare4(TIM4, 300 - brightLed1 % 300); // set brightness
    //實際就是修改CCRx register

    currentButtonStatus = GPIO_ReadInputDataBit(GPIOA, USER_BUTTON);
    if (lastButtonStatus != currentButtonStatus && currentButtonStatus != RESET) {
        brightLed2 *= 2;
        if (brightLed2 >= 500 ) {
              brightLed2 = 5;
        }
        TIM_SetCompare2(TIM4, brightLed2);
    }
    lastButtonStatus = currentButtonStatus;

    delay(4);
}

void delay(uint32_t ms) {
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

void initButton() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_Pin = USER_BUTTON;
    GPIO_Init(GPIOA, &gpio);
}

void initLeds() {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LEDS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
    //將TIM4連接到GPIOD的pin13跟pin15
    //define in stm32f4xx_rcc.c
	//GPIO_PinSource = 0~15
	//GPIO_AF_TIM4： Connect TIM4 pins to AF2
    //搭配rm0090 p.274
    //AF有AF0到AF15，每個AF能連到的裝置是不一樣的。
    //每個pin接到哪個channel參考Discovery kit with STM32F407VG MCU的table 6
    //channel不同，setcompare也不同
}

void initTimer() {
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //define in stm32f4xx_rcc.c
	//Enables or disables the Low Speed APB (APB1) peripheral clock
	//RCC_APB1Periph_TIM4： TIM4 clock
    //TIM4預設是84MHz
    //搭配system clock configuration圖

    /* Compute the prescaler value */
    u32 PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 21000000) - 1;
    //SystemCoreClock define in inc/STM32F4xx/system_stm32f4xx.h
	//SystemCoreClock implement in system_stm32f4xx.c
	//SystemCoreClock = 168000000
	//PrescalerValue = 168000000/2/2100000-1 = 40-1 = 39

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 1000;
    //TIM_Period實際就是設定ARR register
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //(1+TIM_Prescaler)*(1+TIM_Period)/84MHz ＝
	//2.1kHz, 等於PWM的頻率
    //一段PWM(高電位加低電位的時間)等於0.0005sec
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
}

void initPWM() {
    TIM_OCInitTypeDef TIM_OCInitStructure;
    //define in stm32f4xx_tim.h
	//TIM6 and TIM7 不能產生 PWM
	//TIM1 and TIM8 高級計數器 各可以同時產生 7 組 PWM
	//TIM2,3,4,5 普通計時器 各可以同時產生 4 組 PWM
	//ST32最多可以同時產生 30組PWM(2x7 + 4x4 )

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    //PWM 1 Mode ：TIMx_CNT為現在數到的值
	//在向上計數，TIMx_CNT<TIMx_CCRx時，輸出為1，否則輸出為0；
	//在向下計數，TIMx_CNT>TIMx_CCRx時，輸出為0，否則輸出為1。

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    //設定CCRx
	//決定占空比
	//占空比等於：((TIM_Pulse+1)/(TIM_ARR+1))*100%
    //0表示沒有輸出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    //輸出比較極性。
    //在PWM1模式下：
        //High：若TIM_CNT<TIM_CCRx，輸出high，反之輸出low。
        //Low：和上面相反
    //在PWM2模式下：
        //總之和PWM1模式相反

    //查詢Kit p.28的表格。
    /* PWM1 Mode configuration： Channel2 (GPIOD Pin 13)*/
    //設定PWM1 Mode要接那一個channel
	//這裡是設定TIM4的channel4(GPIOD pin 13)
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    //開啟or禁止TIMx_CCR1寄存器的预装载功能，即隨時写入TIMx_CCR1。
	//決定TIMx_CCRx能夠在任何時候通過軟體更新以控制波形，
	//該寫入波形的值是立即生效還是在定時器發生下一次更新時被更新。
	//Enable是下一次時更新；Disable是立即生效
	//在這裡用不到

    /* PWM1 Mode configuration： Channel4 (GPIOD Pin 15)*/
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_Cmd(TIM4, ENABLE);
    //TIM4開始計數
    //這行也可以放在initTimer裡。
}
