#include "main.h"

static uint16_t currentLed = 0;
static uint32_t currentTime = 0;

int main() {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    initLeds();
    initButton();
    initExtInterrupt();
    initClkInterrupt();
}

void loop() {
    __NOP();
}

void delay(u32 ms) {
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

void initLeds() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Pin = LEDS;
    GPIO_Init(GPIOD, &gpio);
}

void initButton() {
    //設置輸入用的按鈕
    //注意要設為input mode!
    //要打開的時中是AHB上的！
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Pin = USER_BUTTON;
    GPIO_Init(GPIOD, &gpio);
}

void initExtInterrupt() {
    EXTI_InitTypeDef   EXTI_InitStructure;

    //外部中斷連接在APB2上，可參考gpio章節的附圖
    //注意！
    //外部中斷才須配置syscfg clock，你可以給他看成exti對應的時鐘
    //其他中斷則不需要配置此項
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    //指定要從那一個gpio，且是那條exti_line，有0到15的選擇
    //這邊選GPIOA，EXTI_Line0，對應的就是PA0，我們的user button~~
    //不知道的在說一次，去查表！
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

    //exti結構成員的設定：
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    //有event跟interrupt兩種選擇，這裡當然用interrupt
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //選擇上緣觸發。
    //注意，user_button按下時電位是升高的！
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    //最後當然是enable啦
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);


    NVIC_InitTypeDef NVIC_InitStructure;

    //nvic結構成員的設定：
    //指定中斷向量，有EXTI0, EXTI1,EXTI2,EXTI3,EXTI4,EXTI9_5,EXTI15_10可選
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    //設定主要優先權跟次要優先權。
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//注意，systick雖然也會產生中斷，但不須要設定nvic。
void initClkInterrupt() {
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, 1);
}

//實做EXTI0的IRQHandler
//名字一定不能變！！！！！
//建議寫在不同的標頭檔裡，官方建議是放在stm32f4xx_it.h中
void EXTI0_IRQHandler() {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        //由於5-9跟10-15是用同一個EXTIx_IRQHandler，
        //因此要在內部先用函式分辨是來自那條EXTI_Linex
        //可用EXTI_GetITStatus or EXTI_GetFlagStatus
        //EXTI_GetFlagStatus(uint32_t EXTI_Line)
        //不檢查中斷線是否有enable，只要中斷flag（就是pending bit）為1，
        //就回傳set(就是1)，反之是reset(就是0)。
        //EXTI_GetITStatus(uint32_t EXTI_Line)
        //也有檢查中斷線是否enable，若disable，則回傳0

        //建議用EXTI_GetITStatus()

        switchLed();
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void SysTick_Handler(void) {
    currentTime++;
    toggleLed();
}

void toggleLed(void) {
    if (currentTime % 200 == 0) {
        GPIO_ToggleBits(GPIOD, LED[currentLed]);
    }
}

void switchLed(void) {
    GPIO_ResetBits(GPIOD, LED[currentLed]);

    currentLed++;
    if (currentLed > 3) {
        currentLed = 0;
    }
}
