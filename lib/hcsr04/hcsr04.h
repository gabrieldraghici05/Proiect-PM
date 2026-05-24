#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h>

#define HCSR04_TRIG_PIN PD2
#define HCSR04_ECHO_PIN PD3

void hcsr04_init(void);
uint16_t hcsr04_measure_cm(void);

#endif
