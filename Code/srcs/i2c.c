#include <G_Love.h>

/**Initiates the two wire interface with the parameters set in the config.h file*/
void i2c_init(void)
{
	TWSR = 0;
	TWBR = (F_CPU / SCL - 16) / (2 * 1);
	TWCR = (1 << TWEN);
}

/**Sends a START condition to initiate data transfer*/
void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

/**Sends a STOP condition to signal the end of the transmission*/
void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/**Sends the data byte to the slave using the data register
 * @param uint8_t*/
void i2c_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

/**Returns the byte in the data register and send acknowledge if passed as an argument, else no acknowledge is sent
 * @param ack_bit
 * @return uint8_t*/
uint8_t i2c_read(uint8_t ack)
{
	if (ack)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	else
		TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
	return (TWDR);
}

int i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t val)
{
	i2c_start();
	i2c_write((dev_addr << 1) | 0);
	if (I2C_STATUS != MT_SLA_ACK)
	{
		i2c_stop();
		return -1;
	}
	i2c_write(reg);
	if (I2C_STATUS != MT_DATA_ACK)
	{
		i2c_stop();
		return -1;
	}
	i2c_write(val);
	if (I2C_STATUS != MT_DATA_ACK)
	{
		i2c_stop();
		return -1;
	}
	i2c_stop();
	return 0;
}

int i2c_write_burst(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint8_t size)
{
	i2c_start();
	i2c_write((dev_addr << 1) | 0);
	if (I2C_STATUS != MT_SLA_ACK)
	{
		i2c_stop();
		return -1;
	}
	i2c_write(reg);
	if (I2C_STATUS != MT_DATA_ACK)
	{
		i2c_stop();
		return -1;
	}
	for (uint8_t i = 0; i < size; i++)
	{
		i2c_write(data[i]);
		if (I2C_STATUS != MT_DATA_ACK && i < size - 1)
		{
			i2c_stop();
			return -1;
		}
	}
	i2c_stop();
	return 0;
}

uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg)
{
	uint8_t val;

	i2c_start();
	i2c_write((dev_addr << 1) | 0);
	if (I2C_STATUS != MT_SLA_ACK)
	{
		i2c_stop();
		return 0;
	}
	i2c_write(reg);
	if (I2C_STATUS != MT_DATA_ACK)
	{
		i2c_stop();
		return 0;
	}
	i2c_start();
	i2c_write((dev_addr << 1) | 1);
	if (I2C_STATUS != MR_SLA_ACK)
	{
		i2c_stop();
		return 0;
	}
	val = i2c_read(0);
	i2c_stop();
	return (val);
}

void i2c_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
	i2c_start();
	i2c_write((dev_addr << 1) | 0);
	if (I2C_STATUS != MT_SLA_ACK)
	{
		i2c_stop();
		return;
	}
	i2c_write(reg);
	if (I2C_STATUS != MT_DATA_ACK)
	{
		i2c_stop();
		return;
	}
	i2c_start();
	i2c_write((dev_addr << 1) | 1);
	if (I2C_STATUS != MR_SLA_ACK)
	{
		i2c_stop();
		return;
	}
	for (uint8_t i = 0; i < len; i++)
		buf[i] = i2c_read(i < len - 1);
	i2c_stop();
}
