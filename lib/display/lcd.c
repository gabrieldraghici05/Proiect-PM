#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

#define BACKLIGHT 0x08
#define EN        0x04
#define RS        0x01

// Trimite un nibble (4 biti) cu puls de Enable
static void lcd_write_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = (nibble & 0xF0) | BACKLIGHT | mode;
    
    i2c_start(LCD_ADDR);
    i2c_write(data | EN);   // EN = 1
    _delay_us(1);
    i2c_write(data & ~EN);  // EN = 0
    _delay_us(50);
    i2c_stop();
}

// Trimite un byte complet (2 nibble-uri)
static void lcd_send(uint8_t value, uint8_t mode) {
    lcd_write_nibble(value & 0xF0, mode);         // High nibble
    lcd_write_nibble((value << 4) & 0xF0, mode);  // Low nibble
}

void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
    _delay_ms(2);
}

void lcd_data(uint8_t data) {
    lcd_send(data, RS);
    _delay_us(50);
}

void lcd_init(void) {
    _delay_ms(50);  // Asteptam pornirea LCD

    // Secventa de initializare in mod 4-bit (din datasheet HD44780)
    lcd_write_nibble(0x30, 0); _delay_ms(5);
    lcd_write_nibble(0x30, 0); _delay_us(150);
    lcd_write_nibble(0x30, 0); _delay_us(150);
    lcd_write_nibble(0x20, 0); _delay_ms(5);  // Switch to 4-bit

    lcd_command(0x28); // 4-bit, 2 linii, 5x8
    lcd_command(0x08); // Display off
    lcd_command(0x01); // Clear
    _delay_ms(5);
    lcd_command(0x06); // Entry mode
    lcd_command(0x0C); // Display on, cursor off
}

void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(5);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_command(0x80 | (col + row_offsets[row]));
}

void lcd_print(const char* str) {
    while (*str) {
        lcd_data((uint8_t)*str++);
    }
}