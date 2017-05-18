#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"

#define SLAVE_ADDRESS 0x27
#define write_command 0x00
#define write_data 0x01
#define ENABLE_HIGH 0x04
#define ENABLE_LOW 0x00
#define RS_command 0x0
#define RS_data 0x1
#define LCD_LINE_MAX 16

/* command */
#define LCD_GOTO_LINE_1 0x80
#define LCD_GOTO_LINE_2 0xc0
#define LCD_CLEAR_DISPLAY 0x01

/* delay function */
void delay_ms(uint16_t delay_input);

/* init function */
void gpio_init();
void i2c_init();
void lcd_init();

/* lcd write function */
static inline void lcd_transmit(uint8_t input) {
  /* 傳送 high nibble */
  while(I2C_GetFlagStatus(I2C3, I2C_FLAG_BUSY));                            // 等待 BUSY FLAG
  I2C_GenerateSTART(I2C3, ENABLE);                                          // 開始通訊
  while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_MODE_SELECT));               // 等待 EV5
  I2C_Send7bitAddress(I2C3, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);   // 傳送 slave address
  while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // 等待 EV6
  I2C_SendData(I2C3, input);                                                // 傳送資料
  while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_BYTE_TRANSMITTED));          // 等待 EV8
  I2C_GenerateSTOP(I2C3, ENABLE);                                           // 結束通訊
}
void lcd_write_4bit_mode(uint8_t RS, uint8_t input);
void lcd_transmit_4bit_mode_data(uint8_t data);
void lcd_transmit_4bit_mode_command(uint8_t command);

/* lcd write string */
void lcd_write_string(char * str);
