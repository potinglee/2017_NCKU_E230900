#include "main.h"
#define slave 0x27<<1

static int counter = 0;
void init() {
  // 初始化 systick timer
  if(SysTick_Config(SystemCoreClock / 1000)) while(1);

  // 設置 i2c腳位
  gpio_init();
  I2C_DeInit(I2C3);
  i2c_init();
  // lcd_init();
}


void loop() {
  while(I2C_GetFlagStatus(I2C3, I2C_FLAG_BUSY)) {
    GPIO_ResetBits(GPIOD, 0xffff);
    GPIO_SetBits(GPIOD, 0x1000);
  }
  // 開始通訊
  I2C_GenerateSTART(I2C3, ENABLE);
    while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_MODE_SELECT)){
      GPIO_ResetBits(GPIOD, 0xffff);
      GPIO_SetBits(GPIOD, 0x2000);
    }
    // 傳送 slave address

    do {
      I2C_Send7bitAddress(I2C3, slave, I2C_Direction_Transmitter);
      delay_ms(10);
      GPIO_ResetBits(GPIOD, 0xffff);
      GPIO_SetBits(GPIOD, 0x4000);
      counter ++;
    }while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter<2000);
    //  傳送資料
    delay_ms(10);

    I2C_SendData(I2C3, 0x01);
    while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
      GPIO_ResetBits(GPIOD, 0xffff);
      GPIO_SetBits(GPIOD, 0x8000);
    }
    // 結束通訊
    delay_ms(10);
    I2C_GenerateSTOP(I2C3, ENABLE);
}

int main() {
  init();
  loop();
  while(1) {
    I2C_GenerateSTART(I2C3, ENABLE);
  }
  return 0;
}
