#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#define MPU6050_ADDR      0xD0  // 0x68 << 1
#define MPU6050_PWR_MGMT  0x6B
#define MPU6050_ACCEL_OUT 0x3B

void mpu6050_init(void);
void mpu6050_read_accel(int16_t *ax, int16_t *ay, int16_t *az);
int16_t  mpu6050_get_angle_x(void);
int16_t  mpu6050_get_angle_y(void);

#endif
