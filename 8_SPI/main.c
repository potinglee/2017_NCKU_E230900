#include "main.h"

static volatile uint32_t data_x;
static volatile uint32_t data_y;
static volatile uint8_t sign_x;
static volatile uint8_t sign_y;

int main() {
    init();

    while(1);
}

void init(void) {
    SysInit();
    initLeds();
    initSpi();
    LIS302DL_Init();
}

void SysInit() {
    SysTick_Config(SystemCoreClock / 1000);
}

void SysTick_Handler() {
    // OUT X的暫存器位址為 29h
    data_x = (uint32_t)mySPI_GetData(0x29);
    // OUT Y的暫存器位址為 2bh
    data_y = (uint32_t)mySPI_GetData(0x2b);

    sign_x = 0;
    sign_y = 0;

    if(data_x & 0x80) sign_x = 1;
    if(data_y & 0x80) sign_y = 1;

    data_x = data_x & 0x7f;
    data_y = data_y & 0x7f;

    GPIO_ResetBits(GPIOD, LEDS);
    if(data_x > 8) {
        if(sign_x) GPIO_SetBits(GPIOD, GPIO_Pin_12);
        else GPIO_SetBits(GPIOD, GPIO_Pin_14);
    }
    if(data_y > 8) {
        if(sign_y) GPIO_SetBits(GPIOD, GPIO_Pin_15);
        else GPIO_SetBits(GPIOD, GPIO_Pin_13);
    }
}

void initLeds() {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LEDS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void initSpi(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    // 初始化 SPI的腳位 SCLK, MOSI, MISO
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

    // 初始化 LIS302DL的 NSS腳位
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    GPIO_SetBits(GPIOE, GPIO_Pin_3);

    // 初始化 SPI設定
    SPI_InitTypeDef SPI_InitStructure;
    // 以防原本有其他的設定，所以先清空
    SPI_I2S_DeInit(SPI1);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 0;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

void LIS302DL_Init() {
    mySPI_SendData(0x20, 0x47);
}

void mySPI_SendData(uint8_t adress, uint8_t data){
    // 拉低 LIS302DL的 NSS腳位以開始通訊
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);

    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, adress);
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    SPI_I2S_ReceiveData(SPI1);

    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, data);
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    SPI_I2S_ReceiveData(SPI1);

    GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

uint8_t mySPI_GetData(uint8_t adress){
    // 拉低 LIS302DL的 NSS腳位以開始通訊
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
    // RW bit設為高電位，告訴 LIS302DL要讀取數值
    adress = 0x80 | adress;

    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, adress);
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    SPI_I2S_ReceiveData(SPI1); //Clear RXNE bit

    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, 0x00); //Dummy byte to generate clock
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

    GPIO_SetBits(GPIOE, GPIO_Pin_3);

    return  SPI_I2S_ReceiveData(SPI1);
}
