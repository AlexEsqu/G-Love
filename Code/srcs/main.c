#include <G_Love.h>

int main(void)
{
	uart_init();
	i2c_init();
	set_imu();

	imu_data_t imu_data;

	while (1)
	{
		fifo_sampling(&imu_data);
		uart_printstr("\r\n");
	}
}