#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include "buzzer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint16_t buzzer_remaining_ms = 0;

// Timer2 CTC, 1ms tick: 16MHz / 64 = 250kHz, OCR2A=249 → 250 ticks = 1ms
ISR(TIMER2_COMPA_vect) {
    if (buzzer_remaining_ms > 0 && --buzzer_remaining_ms == 0)
        buzzer_off();
}

void buzzer_init(void) {
    DDRB |= (1 << PB1);

    // Timer1 Fast PWM mode 14 (TOP=ICR1), OC1A non-inverting, prescaler=1
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);
    OCR1A = 0;
    ICR1  = 0;

    // Timer2 CTC pentru numararea duratei (1ms per tick)
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22);   // prescaler 64
    OCR2A  = 249;
    TIMSK2 = (1 << OCIE2A);
}

void buzzer_tone(uint16_t freq_hz, uint16_t duration_ms) {
    if (freq_hz == 0) return;
    uint16_t top = (uint16_t)(F_CPU / (uint32_t)freq_hz) - 1;
    ICR1  = top;
    OCR1A = top / 2;  // duty cycle 50%
    buzzer_remaining_ms = duration_ms;
}

void buzzer_off(void) {
    OCR1A = 0;
    ICR1  = 0;
    PORTB &= ~(1 << PB1);
}
