#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal_PCF8574.h>

extern LiquidCrystal_PCF8574 lcd;

void lcd_Init();
void lcd_Loop();

#endif