#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"

#define SLAVE_ADDRESS 0x27
// HD44780 controller functions
#define LCD_RS_DATA         0x80 // lcd_transimit(0x80, data); 用來傳資料的 address
#define LCD_RS_COMMAND      0x00 // lcd_transimit(0x00, command);  用來傳指令 address

void gpio_init();
void i2c_init();
void lcd_transmit(uint8_t LCD_RS, uint8_t data);
void lcd_transmit_data(uint8_t data);
void lcd_transmit_command(uint8_t command);
void lcd_init();
void blink();
void delay_ms(uint16_t delay_input);
