#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"

#define SLAVE_ADDRESS 0x27
// HD44780 controller functions
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x0D
#define LCD_CURSORSHIFT         0x14
#define LCD_FUNCTIONSET         0x38
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// flags for LCD_ENTRYMODESET
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for LCD_DISPLAYCONTROL
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// flags for LCD_CURSORSHIFT
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

// flags for LCD_FUNCTIONSET
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_1LINE               0x00
#define LCD_2LINE               0x08
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

#define LCD_BACKLIGHT_ON    0b00000000 // ON  = data is LOW
#define LCD_BACKLIGHT_OFF   0b10000000 // OFF = data is HIGH

#define LCD_RS_DATA         0x80 // lcd_transimit(0x80, data); 用來傳資料的 address
#define LCD_RS_COMMAND      0x00 // lcd_transimit(0x00, command);  用來傳指令 address

#define LCD_RW_HIGH         0x20 // READ from LCD
#define LCD_RW_LOW          0x00 // WRITE to LCD (character or command)

#define LCD_PULSE_HIGH      0x10 // Enable Pulse on pin 6
#define LCD_PULSE_LOW       0b00000000 // when pulse goes from high to low the data is read


// COMMANDS: set DDRAM address ( display 2 x 16 )
#define LCD_CURSOR_LINE_MAX    15
#define LCD_CURSOR_TO_LINE_1   BIN_TO_BYTE(1,0,0,0,0,0,0,0)
#define LCD_CURSOR_TO_LINE_2   BIN_TO_BYTE(1,1,0,0,0,0,0,0)

void gpio_init();
void i2c_init();
void lcd_transmit(uint8_t LCD_RS, uint8_t data);
void lcd_transmit_data(uint8_t data);
void lcd_transmit_command(uint8_t command);
void lcd_init();
