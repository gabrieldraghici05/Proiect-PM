#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void buzzer_init(void);
void buzzer_tone(uint16_t freq_hz, uint16_t duration_ms);
void buzzer_off(void);

#endif
