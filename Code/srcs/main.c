#include <G_Love.h>

int main(void)
{
	uart_init();
	twi_init();

	while (1)
	{
		uart_printhex(twi_read_reg(IMU_ADDR, 0));
		uart_printstr("\r\n");
		_delay_ms(1000);
	}
}