#include "lcd.h"

uint8_t delay_time;

// gpio_init()：初始化 i2c腳位
// i2c_init()：設定 i2c，時脈設為 50kHz
// lcd_transmit(uint8_t LCD_RS, uint8_t data)：底層函式，用來傳資料跟指令，會透過其他函式使用它
// lcd_transmit_data(uint8_t data)：傳送資料
// lcd_transmit_command(uint8_t command)：傳送指令

void SysTick_Handler() {
  if(delay_time > 0) delay_time--;
}

void delay_ms(uint8_t delay_input) {
  delay_time = delay_input;
  while(delay_time != 0);
}

void gpio_init() {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_Pin_8, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_Pin_9, GPIO_AF_I2C1);
}

void i2c_init() {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  I2C_InitTypeDef I2C_InitStructure;

  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed ​​= 50000; // set clock to 50kHz

  I2C_Init(I2C1, &I2C_InitStructure);
  I2C_Cmd(I2C1, ENABLE);
}

// LCD_RS有兩種選擇：
      // LCD_RS_DATA：用來傳資料
      // LCD_RS_COMMAND：用來傳指令
void lcd_transmit(uint8_t LCD_RS, uint8_t data) {
  // 開始通訊
  I2C_GenerateSTART(I2C1, ENABLE);
  While (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  // 傳送 slave address
  I2C_Send7bitAddress(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter);
  While(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  // 傳送指令
  I2C_SendData(I2C1, LCD_RS);
  While(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  //  傳送資料
  I2C_SendData(I2C1, data);
  While(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  // 結束通訊
  I2C_GenerateSTOP(I2C1, ENABLE);
}

// 用來傳資料
void lcd_transimit_data(uint8_t data) {
  lcd_transimit(LCD_RS_DATA, data);
}

// 用來傳指令
void lcd_transimit_data(uint8_t command) {
  lcd_transimit(LCD_RS_COMMAND, command);
}

void lcd_init() {// 初始化 HD44780
  // 開頭先等待 40ms
  delay_ms(40);

  lcd_transmit_command(0x30);
  delay_ms(1);
  lcd_transmit_command(0x30);
  delay_ms(1);
  lcd_transmit_command(0x30);
  delay_ms(1);

  lcd_transmit_command(0x38);
  delay_ms(1);
}
