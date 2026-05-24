#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "hcsr04.h"
#include <util/delay.h>

void hcsr04_init(void) {
    DDRD |=  (1 << HCSR04_TRIG_PIN);
    DDRD &= ~(1 << HCSR04_ECHO_PIN);
    PORTD &= ~(1 << HCSR04_TRIG_PIN);
}

uint16_t hcsr04_measure_cm(void) {
    // Puls Trig de 10us
    PORTD |=  (1 << HCSR04_TRIG_PIN);
    _delay_us(10);
    PORTD &= ~(1 << HCSR04_TRIG_PIN);

    // Asteapta Echo sa devina HIGH (timeout 30ms)
    uint16_t timeout = 0;
    while (!(PIND & (1 << HCSR04_ECHO_PIN))) {
        _delay_us(1);
        if (++timeout > 30000) return 0;
    }

    // Masoara durata Echo HIGH (max ~23ms = ~400cm)
    uint16_t duration = 0;
    while (PIND & (1 << HCSR04_ECHO_PIN)) {
        _delay_us(1);
        if (++duration > 23200) break;
    }

    int factor;

    factor = 38;

    uint16_t dist = duration / factor;

    return dist;
}
