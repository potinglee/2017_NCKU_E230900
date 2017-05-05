#include "main.h"

void SysTick_Init() {
	SysTick_Config(SystemCoreClock / 1000);
}

void SysTick_Handler() {
	delay_time--;
}

void delay_function(int d) {
	delay_time = d;
	while(delay_time != 0);
}

void Init() {
	//set usart pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	//set led pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//set USART
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);

	//set USART interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	//set NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0f;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//enable USART
	USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler() {
	if(USART_GetITStatus(USART2, USART_IT_RXNE)) {
		Receive_value = USART_ReceiveData(USART2);

		GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

		if(Receive_value == 'A') GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		else if(Receive_value == 'B') GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		else if(Receive_value == 'C') GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		else if(Receive_value == 'D') GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
	}
}

int main()
{
	SysTick_Init();
	Init();

	GPIO_SetBits(GPIOD, GPIO_Pin_12);
	delay_function(5000);
	GPIO_ResetBits(GPIOD, GPIO_Pin_12);

	while(1);
}
