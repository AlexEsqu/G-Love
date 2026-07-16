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

uint16_t get_fifo_count()
{
	uint16_t count = 0;
	count = twi_read_reg(IMU_ADDR, FIFO_COUNTL);
	count |= (twi_read_reg(IMU_ADDR, FIFO_COUNTH) << 8);
	return (count);
}

void read_fifo_data(int32_t *imu_data)
{
	uint8_t fifo_data[20];
	twi_read_burst(IMU_ADDR, FIFO_DATA, fifo_data, 20);
	
	for (int i = 0; i < 6; i++)
		imu_data[i] = DEF_VALUE;

	if (!(fifo_data[0] & (1 << HEADER_20) || fifo_data[0] & (1 << HEADER_MSG)))
		return;
	
	if (fifo_data[0] & (1 << HEADER_ACCEL))
	{
		imu_data[0] = (fifo_data[1] << 12) | (fifo_data[2] << 4) | (fifo_data[18] >> 4);
		imu_data[1] = (fifo_data[3] << 12) | (fifo_data[4] << 4) | (fifo_data[19] >> 4);
		imu_data[2] = (fifo_data[5] << 12) | (fifo_data[6] << 4) | (fifo_data[20] >> 4);
	}
	if (fifo_data[0] & (1 << HEADER_GYRO))
	{
		imu_data[3] = (fifo_data[7] << 12) | (fifo_data[8] << 4) | fifo_data[18];
		imu_data[4] = (fifo_data[9] << 12) | (fifo_data[10] << 4) | fifo_data[19];
		imu_data[5] = (fifo_data[11] << 12) | (fifo_data[12] << 4) | fifo_data[20];
	}
}
