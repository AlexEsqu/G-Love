#include <G_Love.h>

/**Initiates the two wire interface with the parameters set in the config.h file*/
void twi_init(void)
{
	TWSR = 0;
	TWBR = (F_CPU / SCL - 16) / (2 * 1);
	TWCR = (1 << TWEN);
}

/**Sends a START condition to initiate data transfer*/
void twi_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

/**Sends a STOP condition to signal the end of the transmission*/
void twi_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO)  | (1 << TWEN);
}

/**Sends the data byte to the slave using the data register
 * @param uint8_t*/
void twi_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)));
}

/**Returns the byte in the data register and send acknowledge if passed as an argument, else no acknowledge is sent
 * @param ack_bit
 * @return uint8_t*/
uint8_t twi_read(uint8_t ack)
{
	if (ack)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	else
		TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
	return (TWDR);
}

void	twi_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t val)
{
	twi_start();
	twi_write((dev_addr << 1) | 0);
	twi_write(reg);
	twi_write(val);
	twi_stop();
}

void	twi_write_burst(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint8_t size)
{
	twi_start();
	twi_write((dev_addr << 1) | 0);
	twi_write(reg);
	for (uint8_t i = 0; i < size - 1; i++)
		twi_write(data[i]);
	twi_stop();
}

uint8_t	twi_read_reg(uint8_t dev_addr, uint8_t reg)
{
	uint8_t val;

	twi_start();
	twi_write((dev_addr << 1) | 0);
	twi_write(reg);
	twi_start();
	twi_write((dev_addr << 1) | 1);
	val = twi_read(0);
	twi_stop();
	return (val);
}

void	twi_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
	twi_start();
	twi_write((dev_addr << 1) | 0);
	twi_write(reg);
	twi_start();
	twi_write((dev_addr << 1) | 1);
	for (uint8_t i = 0; i < len; i++)
		buf[i] = twi_read(i < len - 1);
	twi_stop();
}

