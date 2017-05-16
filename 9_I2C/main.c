#include "main.h"

void init() {
  // 初始化 systick timer
  if(SysTick_Config(SystemCoreClock / 1000)) while(1);

  // 設置 i2c腳位
  gpio_init();
  i2c_init();
  lcd_init();
}

void loop() {
  // donothing
}

int main() {
  init();

  lcd_transmit_data('0');
  lcd_transmit_data('1');
  lcd_transmit_data('2');
  lcd_transmit_data('3');
  lcd_transmit_data('4');
  lcd_transmit_data('5');
  lcd_transmit_data('6');
  lcd_transmit_data('7');
  while(1) {
    loop();
  }
  return 0;
}
