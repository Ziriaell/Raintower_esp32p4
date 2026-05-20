#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\lcd.h"
#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal_PCF8574.h>

extern LiquidCrystal_PCF8574 lcd;

void lcd_Init();
void lcd_Loop();

#endif