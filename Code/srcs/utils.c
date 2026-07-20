#include <G_Love.h>

void twi_scan(void)
{
    uint8_t found_devices = 0;

	for (uint8_t i = 0; i < 128; i++)
	{
		twi_start();
		twi_write((i << 1) | 0);
		if (TW_STATUS == MT_SLA_ACK)
		{
			uart_printstr("Device found at address: 0x");
			uart_printhex(i);
			uart_printstr("\r\n");
            found_devices++;
		}
		twi_stop();
	}
    
    if (found_devices == 0)
    {
        uart_printstr("No devices found on the I2C bus.\r\n");
    }
}
