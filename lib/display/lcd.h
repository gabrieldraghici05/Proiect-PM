#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#define LCD_ADDR 0x4E

void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_print(const char* str);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_clear(void);

#endif