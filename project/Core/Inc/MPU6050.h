#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

#define MPU6050_ADDR 0xD0

#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define ACCEL_YOUT_H_REG 0x3D
#define ACCEL_ZOUT_H_REG 0x3F
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define GYRO_YOUT_H_REG 0x45
#define GYRO_ZOUT_H_REG 0x47
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

void MPU6050_Init(void);
double MPU6050_Read_AccelX(void);
double MPU6050_Read_AccelY(void);
double MPU6050_Read_AccelZ(void);
double MPU6050_Read_GyroX(void);
double MPU6050_Read_GyroY(void);
double MPU6050_Read_GyroZ(void);
int MPU6050_Counter(void);
#endif
