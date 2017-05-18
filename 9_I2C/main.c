#include "main.h"
#include <string.h>

void init() {
  // 初始化 systick timer
  if(SysTick_Config(SystemCoreClock / 1000)) while(1);

  // 設置 i2c腳位
  gpio_init();
  i2c_init();
  lcd_init();
}

void loop() {
}



int main() {

  static int counter_shift = 0;
  char * str = "hello!";
  init();

  while(1) {
    lcd_transmit_4bit_mode_command(LCD_CLEAR_DISPLAY);
    if((strlen(str) + counter_shift) > LCD_LINE_MAX) {
      lcd_write_string(str + LCD_LINE_MAX - counter_shift );
      static int i;
      for(i = 0; i<(int)(LCD_LINE_MAX - strlen(str)); i++) {
        lcd_transmit_4bit_mode_data(' ');
      }
      for(i = 0; i<(LCD_LINE_MAX - counter_shift); i++) {
        lcd_transmit_4bit_mode_data(*(str+i));
      }
    }
    else {
      static int i;
      for(i = 0; i<counter_shift; i++) {
        lcd_transmit_4bit_mode_data(' ');
      }
      lcd_write_string(str);
    }
    counter_shift == LCD_LINE_MAX ? counter_shift = 0 : counter_shift++;
    delay_ms(100);
  }
  return 0;
}
