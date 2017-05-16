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


static uint16_t led_array[4] = {
  GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15
};
static int i = 0;


void SysTick_Handler() {
  if(delay_time > 0) delay_time--;
}

void delay_ms(uint16_t delay_input) {
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

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void i2c_init() {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  I2C_InitTypeDef I2C_InitStructure;

  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 50000;

  I2C_Init(I2C1, &I2C_InitStructure);
  I2C_Cmd(I2C1, ENABLE);
}

// LCD_RS有兩種選擇：
      // LCD_RS_DATA：用來傳資料
      // LCD_RS_COMMAND：用來傳指令
void lcd_transmit(uint8_t LCD_RS, uint8_t data) {
  uint8_t device_address = SLAVE_ADDRESS<<1;
  // 開始通訊
  GPIO_ResetBits(GPIOD, led_array[0]|led_array[1]|led_array[2]|led_array[3]);
  I2C_GenerateSTART(I2C1, ENABLE);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)){
    GPIO_SetBits(GPIOD, led_array[0]);
  }
  // 傳送 slave address
  I2C_Send7bitAddress(I2C1, device_address, I2C_Direction_Transmitter);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
    GPIO_SetBits(GPIOD, led_array[1]);
  }
  // 傳送指令
  I2C_SendData(I2C1, LCD_RS);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
    GPIO_SetBits(GPIOD, led_array[2]);
  }
  //  傳送資料
  I2C_SendData(I2C1, data);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
    GPIO_SetBits(GPIOD, led_array[3]);
  }
  // 結束通訊
  I2C_GenerateSTOP(I2C1, ENABLE);
}

// 用來傳資料
void lcd_transmit_data(uint8_t data) {
  lcd_transmit(LCD_RS_DATA, data);
}

// 用來傳指令
void lcd_transmit_command(uint8_t command) {
  lcd_transmit(LCD_RS_COMMAND, command);
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

  lcd_transmit_command(0x38); //設定為 8bit mode，兩行，5x7顯示
  delay_ms(1);
  lcd_transmit_command(0x0c); //設定為顯示開，有光標，光標不閃爍
  delay_ms(1);
  lcd_transmit_command(0x01); //清屏，光標歸位回左上角，並將地址計數器(AC)歸零
  delay_ms(1);
  lcd_transmit_command(0x06); //寫入數據後光標右移一位
  delay_ms(1);
}

void blink() {
  GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
  GPIO_SetBits(GPIOD, led_array[i%4]);
  i++;
  delay_ms(1000);
}
