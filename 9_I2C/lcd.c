#include "lcd.h"

uint16_t delay_time;

// gpio_init()：初始化 i2c腳位
// i2c_init()：設定 i2c，時脈設為 50kHz
// lcd_transmit(uint8_t LCD_RS, uint8_t data)：
              // 底層函式，用來傳資料跟指令，會透過其他函式使用它。
              // 會在 command跟 data之前加上對應的 LCD_RS
// lcd_transmit_data(uint8_t data)：傳送資料
// lcd_transmit_command(uint8_t command)：傳送指令
// lcd_init()：設定 lcd，並且初始化它

void SysTick_Handler() {
  if(delay_time > 0) delay_time--;
}

void delay_ms(uint16_t delay_input) {
  delay_time = delay_input;
  while(delay_time != 0);
}

void gpio_init() {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);
}

void i2c_init() {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
  I2C_InitTypeDef I2C_InitStructure;

  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 50000;

  I2C_Init(I2C3, &I2C_InitStructure);
  I2C_Cmd(I2C3, ENABLE);
}

// 會先傳送 high nibble，再傳送 low nibble
// 輸入想輸入的資料 data或 command
// lcd_transmit_4bit_mode(RS_data, data) or
// lcd_transmit_4bit_mode(RS_command, command)
void lcd_write_4bit_mode(uint8_t RS, uint8_t input) {
  /* 將 input分成 high nibble 跟 low nibble */
  uint8_t high_nibble = (input & 0xf0);
  uint8_t low_nibble = (input & 0x0f)<<4;
  /* 決定資料是屬於 command還是 data */
  if(RS == RS_data) {
    high_nibble |= write_data;
    low_nibble |= write_data;
  }
  /* 傳送 high nibble */
  lcd_transmit(high_nibble | ENABLE_HIGH);      // 傳送資料
  /* 傳縙 ENABLE_LOW, 寫入資料*/
  lcd_transmit(high_nibble | ENABLE_LOW);       // 傳送資料
  delay_ms(5);
  /* 傳送 low nibble */
  lcd_transmit(low_nibble | ENABLE_HIGH);       // 傳送資料
  /* 傳縙 ENABLE_LOW, 寫入資料*/
  lcd_transmit(low_nibble | ENABLE_LOW);        // 傳送資料
  delay_ms(5);
}

// 用來傳資料
void lcd_transmit_4bit_mode_data(uint8_t data) {
  lcd_write_4bit_mode(RS_data, data);
}

// 用來傳指令
void lcd_transmit_4bit_mode_command(uint8_t command) {
  lcd_write_4bit_mode(RS_command, command);
}

void lcd_write_string(char * str) {
  while (*str) {
    lcd_transmit_4bit_mode_data(*str);
    str++;
  }
}

void lcd_init() {// 初始化 HD44780
  /* 開頭先等待 40ms */
  delay_ms(40);
  lcd_transmit_4bit_mode_command(0x03);
  delay_ms(15);
  lcd_transmit_4bit_mode_command(0x03);
  delay_ms(15);
  lcd_transmit_4bit_mode_command(0x03);
  delay_ms(15);
  lcd_transmit_4bit_mode_command(0x02);
  delay_ms(1);

  lcd_transmit_4bit_mode_command(0x01); //clear display
  delay_ms(1);
}
