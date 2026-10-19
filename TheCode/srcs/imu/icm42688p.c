#include <stdint.h>
#include "icm42688p.h"
#include "main.h"

/*//need:
a read register function
a check "WHOAMI" byte
accel and gyro LN mode? possibly a choice
set accel scaling
set gyro scaling
disable or set filters
estiamte/calibrate gyro bias

*/
t_imu *imu_conf_create(void)
{
    t_imu *imu = malloc(sizeof(*imu));
    if (!imu)
        return NULL;

    imu->address = ICM_ADDR;
    imu->bank = 0;
    imu->reset.reg = DEVICE_CONFIG;
    imu->reset.value = 0x01;
    imu->power_mgmt0.reg = PWR_MGMT0;
    imu->power_mgmt0.value = 0x0F;
    imu->accel_config0.reg = ACCEL_CONFIG0;
    imu->accel_config0.value = 0x00;
    imu->gyro_config0.reg = GYRO_CONFIG0;
    imu->gyro_config0.value = 0x00;

    return imu;
}

t_imu_data *imu_data_create(void)
{
    t_imu_data *data = malloc(sizeof(*data));
    if (!data)
        return NULL;

    data->accel[0] = 0.0f;
    data->accel[1] = 0.0f;
    data->accel[2] = 0.0f;
    data->gyro[0] = 0.0f;
    data->gyro[1] = 0.0f;
    data->gyro[2] = 0.0f;

    return data;
}
/*
t_imu imu_data_create()
{
	t_imu imu_conf;
	imu_conf.address = ICM_ADDR;
	imu_conf.bank = 0;
	imu_conf.reset.reg = DEVICE_CONFIG;
	imu_conf.reset.value = 0x01;
	imu_conf.power_mgmt0.reg = PWR_MGMT0;
	imu_conf.power_mgmt0.value = 0x0F;
	imu_conf.accel_config0.reg = ACCEL_CONFIG0;
	imu_conf.accel_config0.value = 0x00;
	imu_conf.gyro_config0.reg = GYRO_CONFIG0;
	imu_conf.gyro_config0.value = 0x00;
	imu_conf.sample.accel[0] = 0.0;
	imu_conf.sample.accel[1] = 0.0;
	imu_conf.sample.accel[2] = 0.0;
	imu_conf.sample.gyro[0] = 0.0;
	imu_conf.sample.gyro[1] = 0.0;
	imu_conf.sample.gyro[2] = 0.0;
	return imu_conf;

}*/
/*
t_imu_data imu_data_create()
{
	t_imu_data imu_data;
	imu_data.accel[0] = 0.0;
	imu_data.accel[1] = 0.0;
	imu_data.accel[2] = 0.0;
	imu_data.accel[0] = 0.0;
	imu_data.gyro[1] = 0.0;
	imu_data.gyro[2] = 0.0;
	return imu_data;
}*/

static uint8_t imu_read_reg(const t_imu *imu, uint8_t reg)
{
    uint8_t value = 0;

    i2c_start();
    i2c_write((imu->address << 1) | 0x00);
    i2c_write(reg);
    i2c_start();
    i2c_write((imu->address << 1) | 0x01);
    value = i2c_read_nack();  // AVR TWI: read 1 byte, NAK to end
    i2c_stop();

    return value;
}

static int imu_write_reg(t_imu *imu, uint8_t reg, uint8_t value)
{
    if (!imu)
        return -1;

    i2c_start();
    i2c_write((imu->address << 1) | 0x00);
    i2c_write(reg);
    i2c_write(value);
    i2c_stop();

    return (imu_read_reg(imu, reg) == value) ? 1 : -1;
}

static int imu_set_bank(t_imu *imu, uint8_t bank)
{
    if (!imu)
        return -1;

    if (imu->bank == bank)
        return 1;

    imu->bank = bank;

    return imu_write_reg(imu, REG_BANK_SEL, bank);
}

static int imu_reset(t_imu *imu)
{
    if (!imu)
        return -1;

    if (imu_set_bank(imu, 0) < 0)
        return -2;

    imu_write_reg(imu, UB0_REG_DEVICE_CONFIG, 0x01);

    _delay_ms(1);
    return 1;
}

static int imu_set_accel_fs(t_imu *imu, uint8_t fs_sel)
{
    uint8_t reg;

    if (!imu)
        return -1;

    if (imu_set_bank(imu, 0) < 0)
        return -1;

    reg = imu_read_reg(imu, ACCEL_CONFIG0);
    reg = (reg & 0x1F) | ((fs_sel & 0x07) << 5);
    imu->accel_config0.reg = ACCEL_CONFIG0;
    imu->accel_config0.value = reg;

    return imu_write_reg(imu, ACCEL_CONFIG0, reg);
}

static int imu_set_gyro_fs(t_imu *imu, uint8_t fs_sel)
{
    uint8_t reg;

    if (!imu)
        return -1;

    if (imu_set_bank(imu, 0) < 0)
        return -1;

    reg = imu_read_reg(imu, GYRO_CONFIG0);
    reg = (reg & 0x1F) | ((fs_sel & 0x07) << 5);
    imu->gyro_config0.reg = GYRO_CONFIG0;
    imu->gyro_config0.value = reg;

    return imu_write_reg(imu, GYRO_CONFIG0, reg);
}

static int imu_set_filters(t_imu *imu, uint8_t gyro_filter_enabled, uint8_t accel_filter_enabled)
{
    (void)gyro_filter_enabled;
    (void)accel_filter_enabled;

    if (!imu)
        return -1;

    // same basic behavior as C++ begin():
    // disable the internal filters for a minimal startup config
    // exact register values depend on your device map
    if (imu_set_bank(imu, 1) < 0)
        return -1;

    // disable gyro notches / AAF; exact value depends on your header
    if (imu_write_reg(imu, GYRO_CONFIG_STATIC2, 0x00) < 0)
        return -1;

    if (imu_set_bank(imu, 2) < 0)
        return -1;

    // disable accel AAF
    if (imu_write_reg(imu, ACCEL_CONFIG_STATIC2, 0x00) < 0)
        return -1;

    if (imu_set_bank(imu, 0) < 0)
        return -1;

    return 1;
}

static int imu_calibrate_gyro(t_imu *imu)
{
    uint32_t sum_x = 0, sum_y = 0, sum_z = 0;
    int i;

    if (!imu)
        return -1;

    // temporarily set gyro to lower range to improve bias estimation
    if (imu_set_gyro_fs(imu, 0x00) < 0)  // 250 dps
        return -1;

    for (i = 0; i < 100; i++)
    {
        int16_t gx, gy, gz;

        // read raw gyro registers
        gx = (int16_t)(((uint16_t)imu_read_reg(imu, GYRO_DATA_X1) << 8) |
                       imu_read_reg(imu, GYRO_DATA_X0));
        gy = (int16_t)(((uint16_t)imu_read_reg(imu, GYRO_DATA_Y1) << 8) |
                       imu_read_reg(imu, GYRO_DATA_Y0));
        gz = (int16_t)(((uint16_t)imu_read_reg(imu, GYRO_DATA_Z1) << 8) |
                       imu_read_reg(imu, GYRO_DATA_Z0));

        sum_x += (uint32_t)(gx & 0xFFFF);
        sum_y += (uint32_t)(gy & 0xFFFF);
        sum_z += (uint32_t)(gz & 0xFFFF);

        _delay_ms(1);
    }

    // store bias estimate in fields if you have them
    imu->sample.gyro[0] = (float)(sum_x / 100.0f);
    imu->sample.gyro[1] = (float)(sum_y / 100.0f);
    imu->sample.gyro[2] = (float)(sum_z / 100.0f);

    // restore full scale range
    if (imu_set_gyro_fs(imu, 0x03) < 0)  // 2000 dps
        return -1;

    return 1;
}

int ICM42688_init(t_imu *imu)
{
	int status = imu_reset(imu);
    if (status < 0)
        return status;

    // check WHO_AM_I register
    if (imu_read_reg(imu, WHO_AM_I_ADDR) != WHO_AM_I_RESPONSE)
        return -2;

    // turn on accel + gyro in low noise mode
    if (imu_write_reg(imu, PWR_MGMT0, 0x0F) < 0)
        return -3;

    // set accel FS: ±16g
    if (imu_set_accel_fs(imu, 0x03) < 0)
        return -4;

    // set gyro FS: ±2000dps
    if (imu_set_gyro_fs(imu, 0x03) < 0)
        return -5;

    // disable internal filters
    if (imu_set_filters(imu, 0, 0) < 0)
        return -6;

    // calibrate gyro bias
    if (imu_calibrate_gyro(imu) < 0)
        return -7;

    return 1;
}

int imu_get_raw_data(t_imu *imu, t_imu_data *data)
{
	if (!imu || !data)
		return -1;

	// read raw accel registers
	data->accel[0] = (float)(((uint16_t)imu_read_reg(imu, ACCEL_DATA_X1) << 8) |
							 imu_read_reg(imu, ACCEL_DATA_X0));

	data->accel[1] = (float)(((uint16_t)imu_read_reg(imu, ACCEL_DATA_Y1) << 8) |
							 imu_read_reg(imu, ACCEL_DATA_Y0));
	data->accel[2] = (float)(((uint16_t)imu_read_reg(imu, ACCEL_DATA_Z1) << 8) |
							 imu_read_reg(imu, ACCEL_DATA_Z0));

	// read raw gyro registers
	data->gyro[0] = (float)(((uint16_t)imu_read_reg(imu, GYRO_DATA_X1) << 8) |
							imu_read_reg(imu, GYRO_DATA_X0));
	data->gyro[1] = (float)(((uint16_t)imu_read_reg(imu, GYRO_DATA_Y1) << 8) |
							imu_read_reg(imu, GYRO_DATA_Y0));
	data->gyro[2] = (float)(((uint16_t)imu_read_reg(imu, GYRO_DATA_Z1) << 8) |
							imu_read_reg(imu, GYRO_DATA_Z0));

	return 1;
}
