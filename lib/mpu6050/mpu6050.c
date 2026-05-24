#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include "mpu6050.h"
#include "i2c.h"
#include <util/delay.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void mpu6050_write_reg(uint8_t reg, uint8_t value) {
    i2c_start(MPU6050_ADDR);
    i2c_write(reg);
    i2c_write(value);
    i2c_stop();
}

void mpu6050_init(void) {
    _delay_ms(100);
    mpu6050_write_reg(MPU6050_PWR_MGMT, 0x00);
    _delay_ms(10);
}

void mpu6050_read_accel(int16_t *ax, int16_t *ay, int16_t *az) {
    // Seteaza registrul de start, apoi repeated start pentru citire
    i2c_start(MPU6050_ADDR);
    i2c_write(MPU6050_ACCEL_OUT);
    i2c_start(MPU6050_ADDR | 0x01);  // repeated start

    uint8_t buf[6];
    for (uint8_t i = 0; i < 5; i++)
        buf[i] = i2c_read_ack();
    buf[5] = i2c_read_nack();
    i2c_stop();

    *ax = (int16_t)((buf[0] << 8) | buf[1]);
    *ay = (int16_t)((buf[2] << 8) | buf[3]);
    *az = (int16_t)((buf[4] << 8) | buf[5]);
}

int16_t mpu6050_get_angle_x(void) {
    static int16_t buf[8];
    static uint8_t idx = 0;
    static uint8_t ready = 0;

    int16_t ax, ay, az;
    mpu6050_read_accel(&ax, &ay, &az);
    int16_t val = (int16_t)(atan2f((float)ay, (float)az) * (180.0f / M_PI));

    if (!ready) {
        for (uint8_t i = 0; i < 8; i++) buf[i] = val;
        ready = 1;
    }

    buf[idx & 7] = val;
    idx++;

    int32_t sum = 0;
    for (uint8_t i = 0; i < 8; i++) sum += buf[i];
    return (int16_t)(sum / 8);
}

int16_t mpu6050_get_angle_y(void) {
    static int16_t buf[8];
    static uint8_t idx = 0;
    static uint8_t ready = 0;

    int16_t ax, ay, az;
    mpu6050_read_accel(&ax, &ay, &az);
    int16_t val = (int16_t)(atan2f((float)ax, (float)az) * (180.0f / M_PI));

    if (!ready) {
        for (uint8_t i = 0; i < 8; i++) buf[i] = val;
        ready = 1;
    }

    buf[idx & 7] = val;
    idx++;

    int32_t sum = 0;
    for (uint8_t i = 0; i < 8; i++) sum += buf[i];
    return (int16_t)(sum / 8);
}
