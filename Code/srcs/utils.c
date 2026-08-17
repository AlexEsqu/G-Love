#include <G_Love.h>

void i2c_scan(void)
{
    uint8_t found_devices = 0;

	for (uint8_t i = 0; i < 128; i++)
	{
		i2c_start();
		i2c_write((i << 1) | 0);
		if (I2C_STATUS == MT_SLA_ACK)
		{
			uart_printstr("Device found at address: 0x");
			uart_printhex(i);
			uart_printstr("\r\n");
            found_devices++;
		}
		i2c_stop();
	}
    
    if (found_devices == 0)
    {
        uart_printstr("No devices found on the I2C bus.\r\n");
    }
}
