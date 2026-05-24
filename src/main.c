#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "i2c.h"
#include "lcd.h"
#include "hcsr04.h"
#include "mpu6050.h"
#include "buzzer.h"

typedef enum { MODE_RULETA, MODE_NIVELA } Mode;

static void buttons_init(void) {
    DDRD  &= ~((1 << PD4) | (1 << PD5));
    PORTD |=  (1 << PD4) | (1 << PD5);  // pull-up intern
}

static uint8_t btn1_pressed(void) {
    if (!(PIND & (1 << PD4))) {
        _delay_ms(30);  // debounce
        if (!(PIND & (1 << PD4))) {
            while (!(PIND & (1 << PD4)));  // asteapta eliberare
            return 1;
        }
    }
    return 0;
}

static uint8_t btn2_pressed(void) {
    if (!(PIND & (1 << PD5))) {
        _delay_ms(30);
        if (!(PIND & (1 << PD5))) {
            while (!(PIND & (1 << PD5)));
            return 1;
        }
    }
    return 0;
}

static void show_ruleta(void) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Mod: Ruleta");
    lcd_set_cursor(0, 1);
    lcd_print("Apasa Btn2");
}

static void show_nivela(void) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Mod: Nivela");
    lcd_set_cursor(0, 1);
    lcd_print("Inclinare: --");
}

static void ruleta_measure(void) {
    uint16_t dist = hcsr04_measure_cm();

    char buf[8];
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Distanta:");
    lcd_set_cursor(0, 1);

    if (dist == 0) {
        lcd_print("Fara semnal");
        buzzer_tone(400, 200);
    } else {
        utoa(dist, buf, 10);
        lcd_print(buf);
        lcd_print(" cm");
        uint16_t freq = (dist < 20) ? 1500 : (dist < 100) ? 1000 : 600;
        buzzer_tone(freq, 150);
    }
}

static void nivela_update(void) {
    int16_t angle = mpu6050_get_angle_x();

    char buf[8];
    lcd_set_cursor(0, 1);
    lcd_print("Inclinare:     ");
    lcd_set_cursor(10, 1);

    if (angle < 0) {
        lcd_print("-");
        angle = -angle;
    }
    itoa(angle, buf, 10);
    lcd_print(buf);
    lcd_print("\xDF");   // simbolul grad

    if (angle <= 3) {
        buzzer_tone(2000, 80);
    }
}

int main(void) {
    buttons_init();

    _delay_ms(200);
    i2c_init();
    _delay_ms(50);
    lcd_init();
    mpu6050_init();
    hcsr04_init();
    buzzer_init();
    sei();  // activeaza intreruperile globale pentru Timer2 (buzzer non-blocant)

    Mode mode = MODE_RULETA;
    show_ruleta();

    while (1) {
        if (btn1_pressed()) {
            mode = (mode == MODE_RULETA) ? MODE_NIVELA : MODE_RULETA;
            if (mode == MODE_RULETA)
                show_ruleta();
            else
                show_nivela();
        }

        if (mode == MODE_RULETA) {
            if (btn2_pressed())
                ruleta_measure();
        } else {
            nivela_update();
            for (uint8_t i = 0; i < 20; i++) {
                _delay_ms(10);
                if (btn1_pressed()) {
                    mode = MODE_RULETA;
                    show_ruleta();
                    break;
                }
            }
        }
    }
}
