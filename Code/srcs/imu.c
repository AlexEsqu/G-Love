#include <G_Love.h>

static void select_bank(uint8_t bank)
{
	twi_write_reg(IMU_ADDR, REG_BANK_SEL, bank);
}

void set_imu()
{
	uint8_t data;
	select_bank(BANK_0);
	twi_write_reg(IMU_ADDR, FIFO_CONFIG, 1); // stream to FIFO mode

	data = (1 << FIFO_ACCEL_EN) | (1 << FIFO_GYRO_EN) | (1 << FIFO_WM_GT_TH) | (1 << FIFO_HIRES_EN); // enrable accel, gyro, watermark, high resolution
	twi_write_reg(IMU_ADDR, FIFO_CONFIG1, data);

	data = (1 << FIFO_COUNT_REC); // enable FIFO count recovery for watermark
	twi_write_reg(IMU_ADDR, INTF_CONFIG0, data);

	twi_write_reg(IMU_ADDR, FIFO_COUNTL, 50); // set FIFO watermark to 50 FIFO samples
	twi_write_reg(IMU_ADDR, FIFO_COUNTH, 0);

	data = (1 << TEMP_DIS) | (3 << GYRO_MODE) | (3 << ACCEL_MODE); // disable temperature sensor, set gyro and accel to Low Noise mode
	twi_write_reg(IMU_ADDR, PWR_MGMT0, 0);						   // disable sleep mode

	data = (6 << GYRO_ODR);
	twi_write_reg(IMU_ADDR, GYRO_CONFIG0, data); // set gyro ODR to 1kHz
	data = (6 << ACCEL_ODR);
	twi_write_reg(IMU_ADDR, ACCEL_CONFIG0, data); // set accel ODR to 1kHz
}

static uint16_t get_fifo_count()
{
	select_bank(BANK_0);
	uint16_t count = 0;
	count = twi_read_reg(IMU_ADDR, FIFO_COUNTL);
	count |= (twi_read_reg(IMU_ADDR, FIFO_COUNTH) << 8);
	return (count);
}

static void read_fifo_data(imu_data_t *imu_data)
{
	select_bank(BANK_0);
	uint8_t fifo_data[20];
	twi_read_burst(IMU_ADDR, FIFO_DATA, fifo_data, 20);

	if (!(fifo_data[0] & (1 << HEADER_20) || fifo_data[0] & (1 << HEADER_MSG)))
		return;

	if (fifo_data[0] & (1 << HEADER_ACCEL))
	{
		imu_data->accel_x = (fifo_data[0x01] << 12) | (fifo_data[0x02] << 4) | (fifo_data[0x11] >> 4);
		imu_data->accel_y = (fifo_data[0x03] << 12) | (fifo_data[0x04] << 4) | (fifo_data[0x12] >> 4);
		imu_data->accel_z = (fifo_data[0x05] << 12) | (fifo_data[0x06] << 4) | (fifo_data[0x13] >> 4);
	}
	if (fifo_data[0] & (1 << HEADER_GYRO))
	{
		imu_data->gyro_x = (fifo_data[0x07] << 12) | (fifo_data[0x08] << 4) | fifo_data[0x11];
		imu_data->gyro_y = (fifo_data[0x09] << 12) | (fifo_data[0x0A] << 4) | fifo_data[0x12];
		imu_data->gyro_z = (fifo_data[0x0B] << 12) | (fifo_data[0x0C] << 4) | fifo_data[0x13];
	}
}

static uint8_t is_valid_gyro(int32_t value)
{
	if (value < GYRO_LOWEST || value > GYRO_HIGHEST)
		return (0);
	else
		return (1);
}

static uint8_t is_valid_accel(int32_t value)
{
	if (value < ACCEL_LOWEST || value > ACCEL_HIGHEST)
		return (0);
	else
		return (1);
}

void fifo_sampling(imu_data_t *imu_data)
{
	/* Ne rien faire si pas assez d'échantillons dans le FIFO */
	uint16_t fifo_count = get_fifo_count();
	if (fifo_count < SAMPLE_COUNT)
		return;

	/* Lire les échantillons du FIFO */
	imu_data_t sample[SAMPLE_COUNT];
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++)
	{
		read_fifo_data(&sample[i]);
	}

	int32_t sums[6] = {0};
	uint8_t valid_count[6] = {0};

	/* Calculer les sommes et les compteurs pour chaque type de données */
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++)
	{
		if (is_valid_accel(sample[i].accel_x))
		{
			sums[0] += sample[i].accel_x;
			valid_count[0]++;
		}
		if (is_valid_accel(sample[i].accel_y))
		{
			sums[1] += sample[i].accel_y;
			valid_count[1]++;
		}
		if (is_valid_accel(sample[i].accel_z))
		{
			sums[2] += sample[i].accel_z;
			valid_count[2]++;
		}
		if (is_valid_gyro(sample[i].gyro_x))
		{
			sums[3] += sample[i].gyro_x;
			valid_count[3]++;
		}
		if (is_valid_gyro(sample[i].gyro_y))
		{
			sums[4] += sample[i].gyro_y;
			valid_count[4]++;
		}
		if (is_valid_gyro(sample[i].gyro_z))
		{
			sums[5] += sample[i].gyro_z;
			valid_count[5]++;
		}
	}

	/* Calculer les valeurs moyennes */
	imu_data->accel_x = valid_count[0] ? (int32_t)(sums[0] / valid_count[0]) : 0;
	imu_data->accel_y = valid_count[1] ? (int32_t)(sums[1] / valid_count[1]) : 0;
	imu_data->accel_z = valid_count[2] ? (int32_t)(sums[2] / valid_count[2]) : 0;
	imu_data->gyro_x  = valid_count[3] ? (int32_t)(sums[3] / valid_count[3]) : 0;
	imu_data->gyro_y  = valid_count[4] ? (int32_t)(sums[4] / valid_count[4]) : 0;
	imu_data->gyro_z  = valid_count[5] ? (int32_t)(sums[5] / valid_count[5]) : 0;
}
