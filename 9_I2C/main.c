#include "main.h"

void init() {
  // 初始化 systick timer
  if(SysTick_Config(SystemCoreClock / 1000)) while(1);

  // 設置 i2c腳位
  i2c_init();
}
