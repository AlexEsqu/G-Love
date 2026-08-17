#include <G_Love.h>

int main(void)
{
	setup();
	imu_data_t imu_data;
	adc_data_t adc_data;

	while (1)
	{
		fifo_sampling(&imu_data);
		adc_read_data(&adc_data);
		uart_printstr("\r\n");
	}
}