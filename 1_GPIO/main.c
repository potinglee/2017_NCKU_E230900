#include "main.h"

static uint8_t lastButtonStatus = RESET;//RESET = 0, in stm32f4xx.h

int main()
{
	init();
	do{
		loop();
	}while(1);
}

void init()
{
	initLeds();
	initButton();
}

void loop()
{
	static uint32_t counter = 0;

	uint8_t currentButtonStatus = GPIO_ReadInputDataBit(GPIOA, USER_BUTTON);
	//USER_BUTTON = GPIO_Pin_0 define in main.h
	//USER_Button的腳位為PA0
	//GPIO_ReadInputDataBit(GPIOA, USER_BUTTON) get PA0 status
	//press = 1, release = 0，重要重要重要！
	//lastButtonStatus = 0
	if((lastButtonStatus != currentButtonStatus) && (currentButtonStatus != RESET)){
		++counter;
		GPIO_ResetBits(GPIOD, LEDS);//LEDS define in main.h
        //Reset會先把全部的LED設為0，就是關掉
        //Reset等於"設為0"
		GPIO_SetBits(GPIOD, LED[counter % 4]);//LED define in main.h
		//set the data port bit
        //SetBits會把輸入的腳位設為1
	}
	lastButtonStatus = currentButtonStatus;
}

void initLeds()
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

void initButton()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = USER_BUTTON;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}
