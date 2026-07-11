#include <G_Love.h>

/**Initiates the two wire interface with the parameters set in the config.h file*/
void tw_init(void)
{
	switch (TW_PRESCALER)
	{
		case 1:
			TW_STATUS_REG = 0;
			break;
		
		case 4:
			TW_STATUS_REG = 1;
			break;

		case 16:
			TW_STATUS_REG = (1 << 1);
			break;

		case 64:
			TW_STATUS_REG = (1 << 2) - 1;
			break;

		default:
			TW_STATUS_REG = 0;
			break;
	}

	TW_BIT_RATE_REG = (F_CPU / SCL - 16) / (2 * TW_PRESCALER);
}

/**Sends a START condition to initiate data transfer*/
void tw_start(void)
{
	TW_CONTROL_REG = TW_INTERRUPT_BIT | TW_START_BIT | TW_ENABLE_BIT;

    while (!(TW_CONTROL_REG & TW_INTERRUPT_BIT)); // Wait for the tw to aknowledge
}

/**Sends a STOP condition to signal the end of the transmission*/
void tw_stop(void)
{
	TW_CONTROL_REG = TW_INTERRUPT_BIT | TW_STOP_BIT | TW_ENABLE_BIT;
}

/**Sends the data byte to the slave using the data register
 * @param uint8_t
*/
void tw_write(uint8_t data)
{
	TW_DATA_REG = data;
	TW_CONTROL_REG = TW_INTERRUPT_BIT | TW_ENABLE_BIT;
	while (!(TW_CONTROL_REG & TW_INTERRUPT_BIT));
}

/**Returns the byte in the data register and send acknowledge if passed as an argument, else no acknowledge is sent
 * @param ack_bit
 * @return uint8_t
*/
uint8_t tw_read(uint8_t ack_bit)
{
	TW_CONTROL_REG = TW_INTERRUPT_BIT | TW_ENABLE_BIT | ack_bit;
	while (!(TW_CONTROL_REG & TW_INTERRUPT_BIT));
	return (TW_DATA_REG);
}