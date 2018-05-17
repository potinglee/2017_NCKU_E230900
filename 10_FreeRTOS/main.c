#include "main.h"
unsigned long ulVar=100;
unsigned long ulVar2=10UL;

void vPrintString(const char *str){
	(void) str;
}

void USART1_Pin_Set() {
	//set usart pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}

void USART1_Config_Set(/* arguments */) {
	//set USART
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
}

void USART1_INT_Set(/* arguments */) {
	//set USART interrupt
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//set NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0f;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART1_String_Send(char* s) {
	while (*s != '\0') {
		USART_SendData(USART1, *s++);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}

void LED_Pin_Set() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void Init() {
	USART1_Pin_Set();
	USART1_Config_Set();
	USART1_INT_Set();
	USART_Cmd(USART1, ENABLE);
	LED_Pin_Set();
}

void USART1_IRQHandler() {
	if(USART_GetITStatus(USART1, USART_IT_RXNE)) {
		Receive_value = USART_ReceiveData(USART1);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		if(Receive_value == 'A') GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		else if(Receive_value == 'B') GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		else if(Receive_value == 'C') GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		else if(Receive_value == 'D') GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
	}
}

void vTask1() {
	const char *pcname = " it is ao good/r/n";
	while(1) {
		//USART1_String_Send("adc");
		vTaskDelay(1000);
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		if(xQueue!=0){
			if(xQueueSendToBack(xQueue,(void *) &ulVar,(TickType_t) 10)!=pdPASS)
			{
				ulVar=ulVar+1;
				vPrintString(pcname);
		    USART1_String_Send("AB");
			}
		}
	}      

}

void vTask2() {
	while(1) {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		vTaskDelay(2000); 
		if(xQueue!=0){
			if(xQueueReceive(xQueue,(void *) &ulVar2,(TickType_t) 10) == pdPASS && ulVar2 > 110){
				GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
				USART_SendData(USART1,64);}
			else{
				GPIO_ToggleBits(GPIOD, GPIO_Pin_15);}
		}
		
	}
}

int main()
{
	xQueue=xQueueCreate(10,sizeof(unsigned long));
	Init();
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	xTaskCreate(vTask1, "vTask1", 1000, NULL, 1, NULL);
	xTaskCreate(vTask2, "vTask2", 1000, NULL, 2, NULL);
	vTaskStartScheduler();
	while (1);
}
