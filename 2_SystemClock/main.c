#include "main.h"

//本code實際上是示範透過改變system clock來改變頻率的目的。
int main()
{
	init();

	do {
		loop(1);
	} while(1);
}

void init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = LEDS;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void loop()
{
	i++;//i define in main.h
	switch(i) {
		case 10: slowMode(); break;
		case 15: defaultMode(); break;
		case 25: fastMode(); break;
		case 40: i = 0; break;
	}
	GPIO_ToggleBits(GPIOD, LEDS);

	delay(300);
}

void RCC_WaitForPLLStartUp(void) {
    while ( (RCC->CR & RCC_CR_PLLRDY) == 0 ) {
        __NOP();
    }
}

void delay(uint32_t ms)
{
	ms = ms*1000;
	while(ms--) {
		__NOP();
	}
}

void slowMode()
{
	RCC_DeInit();
	//define in stm32f4xx_rcc.c
	//Resets the RCC clock configuration to the default reset state.
	RCC_HSEConfig(RCC_HSE_ON);
	//define in stm32f4xx_rcc.c
	//Configures the External High Speed oscillator
	//RCC_HSE_ON: turn ON the HSE oscillator
	if(RCC_WaitForHSEStartUp() == ERROR) {
	//define in stm32f4xx_rcc.c
	//return SUCCESS if HSE oscillator is stable and ready to use
	//return ERROR if HSE oscillator not yet ready
		return;
	}

	//設置PLL
	RCC_PLLConfig(RCC_PLLSource_HSE, 8, 192, 8, 15);
	//                          HSE  M    N  P   Q
	//define in stm32f4xx_rcc.c
	//configure the main PLL CLOCK Source
	//must be used only when the main PLL is disabled
	//RCC_PLLSource
		//RCC_PLLSource_HSI: HSI oscillator clock
			//高速内部時鐘, STM32F429xx默認為16MHz
		//RCC_PLLSource_HSE: HSE oscillator clock
			//高速外部時鐘, 最大可接受50MHz時鐘,4-26MHz
			//define 8MHz in stm32f4xx.h
		//由RCC_PLLSource提供SYSCLK
	//f(VCO clock) = f(PLL clock input) × (PLLN / PLLM)
	//f(PLL general clock output) = f(VCO clock) / PLLP
		//f(PLL general clock output)提供給SYSCLK
	//f(USB OTG FS, SDIO, RNG clock output) = f(VCO clock) / PLLQ
		//f(USB OTG FS, SDIO, RNG clock output)通常固定在48MHz
	//P = {2, 4, 6, 8}
	//SYSCLK = 8M * 192 / 8 / 8 = 24MHz
	RCC_PLLCmd(ENABLE);

	RCC_WaitForPLLStartUp();

	RCC_HCLKConfig(RCC_SYSCLK_Div16);
	//define in stm32f4xx_rcc.c
	//Configures the AHB clock (HCLK)
	//RCC_SYSCLK_Div16: AHB clock = SYSCLK/16
	//AHB clock(HCLK) = 24M / 16 = 1.5MHz
	RCC_PCLK1Config(RCC_HCLK_Div16);
	//define in stm32f4xx_rcc.c
	//Configures the Low Speed APB clock (PCLK1).
	//RCC_HCLK_Div16: APB1 clock = HCLK/16
	//APB clock = 1.5M / 16 = 93750Hz
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	//define in stm32f4xx_rcc.c
	//Configures the system clock (SYSCLK)
	//RCC_SYSCLKSource_HSI: HSI selected as system clock source
	//RCC_SYSCLKSource_HSE: HSE selected as  clock source
	//RCC_SYSCLKSource_PLLCLK: PLL selected as  clock sourc

	//RCC_SYSCLKConfig將sysclk的source設為pll(對照圖)
	//RCC_PLLConfig將pll的source設為HSE(對照圖)
	//RCC_HCLKConfig將ahb的clock設為sysclk除頻16
	//RCC_PCLK1Config將apb1的clock設為hclk除頻16
}

void defaultMode() {
	RCC_DeInit();
	// 預設使用 HSI，因此 SYSCLK約為 16MHz附近
}

void fastMode() {
	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);

	if (RCC_WaitForHSEStartUp() == ERROR) {
		return;
	}
	//設置PLL
	RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 15);
	//f(VCO clock) = f(PLL clock input) × (PLLN / PLLM)
	//f(PLL general clock output) = f(VCO clock) / PLLP
	//SYSCLK = 8M * 336 / 8 / 2 = 168MHz
	RCC_PLLCmd(ENABLE);

	RCC_WaitForPLLStartUp();
	//設置AHB時鐘為SYSCLK/4
	RCC_HCLKConfig(RCC_SYSCLK_Div4);
	//AHB clock(HCLK) = 168M / 4 = 42MHz

	//設置APB1時鐘為SYSCLK/1
	RCC_PCLK1Config(RCC_HCLK_Div1);
	//APB clock = 42M / 1 = 42MHz
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}
