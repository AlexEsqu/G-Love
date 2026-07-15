#include <G_Love.h>

uint8_t gyro[3];
uint8_t accel[3];
uint8_t data;

void select_bank(uint8_t bank)
{
	twi_write_reg(IMU_ADDR, REG_BANK_SEL, bank);
}


void set_imu()
{
	select_bank(BANK_0);
	twi_write_reg(IMU_ADDR, FIFO_CONFIG, 1); // stream to FIFO mode
	
	data = (1 << FIFO_ACCEL_EN) | (1 << FIFO_GYRO_EN) | (1 << FIFO_WM_GT_TH) | (1 << FIFO_HIRES_EN); // enrable accel, gyro, watermark, high resolution
	twi_write_reg(IMU_ADDR, FIFO_CONFIG1, data);

	data = (1 << FIFO_COUNT_REC); // enable FIFO count recovery for watermark
	twi_write_reg(IMU_ADDR, INTF_CONFIG0, data);

	twi_write_reg(IMU_ADDR, FIFO_COUNTL, 50);
	twi_write_reg(IMU_ADDR, FIFO_COUNTH, 0);
}

void read_imu_value(uint8_t *accel, uint8_t *gyro)
{
	data = twi_read_reg(IMU_ADDR, FIFO_HEADER);
	if (data & (1 << HEADER_ACCEL))
	{
		twi_read_burst(IMU_ADDR, ACCEL_X_1, accel, 6);
	}
	if (data & (1 << HEADER_GYRO))
	{
		twi_read_burst(IMU_ADDR, GYRO_X_1, gyro, 6);
	}
}
