#include "MPU6050.h"
#include <math.h>

int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;
int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;
double curAccelZ = 0, preAccelZ = 0;
unsigned int thresholdCount = 0, stepCount = 0;

void MPU6050_Init (void)
{
	uint8_t check;
	uint8_t Data;
	// check device ID WHO_AM_I
	// doc tu thanh ghi WHO_AM_I_REG sang thanh ghi check
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR,WHO_AM_I_REG,1, &check, 1, 1000);

	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
	{
		// power management register 0X6B we should write all 0's to wake the sensor up
		Data = 0;
		// viet data vao thanh ghi PWR_MGMT_1_REG
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1,&Data, 1, 1000);
		
		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register ( toc do lay mau tieu chuan)
		Data = 0x07;
		// truyen so 7 vao thanh ghi SMPLRT_DIV_REG ==> 8000/(7+1) = 1000 Hz
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ± 2g
		Data = 0x00;
		// viet 0 vao thanh ghi cau hinh gia toc -> gia toc trong khoang +- 2
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);

	}

}
double MPU6050_Read_AccelZ(void)
{
	uint8_t Rec_Data[2]; // luu vao 2 thanh ghi 

	// Read 2 BYTES of data starting from ACCEL_ZOUT_H register
  // Doc 8 bit dau vao Data0, 8 bit sau vao Data1
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_ZOUT_H_REG, 1, Rec_Data, 2, 1000);
  // ghep gia tri tu 2 thanh ghi ==> gia tri tho 16 bit tu 2 thanh ghi
	Accel_Z_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);

	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register              ****/
	return Accel_Z_RAW/16384.0 * 10; // (g) khuech dai len 10 lan
}
int MPU6050_Counter(void) 
{
	curAccelZ = MPU6050_Read_AccelZ();
	
	if(fabs(curAccelZ - preAccelZ) > 0.6)
		thresholdCount++;
	
	if(thresholdCount > 3) {
		stepCount++;
		thresholdCount = 0;
	}
	
	preAccelZ = curAccelZ;
	
	return stepCount;
}
