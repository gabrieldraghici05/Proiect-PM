#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "i2c.h"
#include <util/delay.h>

// Returneaza 1 la timeout (TWINT nu a fost setat in timp util)
static uint8_t i2c_wait(void) {
    uint16_t t = 10000;
    while (!(TWCR & (1 << TWINT)))
        if (--t == 0) return 1;
    return 0;
}

void i2c_init(void) {
    TWSR = 0x00;        // Prescaler = 1
    TWBR = 0x48;        // 100kHz la 16MHz: (16M/100k - 16) / 2 = 72 = 0x48
    TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (i2c_wait()) return 1;

    TWDR = address;  // adresa deja shiftata (ex: 0x4E = 0x27 << 1)
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (i2c_wait()) return 1;

    uint8_t status = TWSR & 0xF8;
    // 0x18 = SLA+W ACK, 0x40 = SLA+R ACK (repeated start)
    return (status == 0x18 || status == 0x40) ? 0 : 1;
}

uint8_t i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    return i2c_wait();
}

uint8_t i2c_read_ack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    if (i2c_wait()) return 0xFF;
    return TWDR;
}

uint8_t i2c_read_nack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (i2c_wait()) return 0xFF;
    return TWDR;
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(100);
}
