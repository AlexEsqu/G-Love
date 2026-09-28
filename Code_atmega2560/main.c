#include "../includes/config_atm2560.h"
#include "../includes/main.h"

void setup()
{
	uart0_init(BAUDRATE);
	uart1_init(BAUDRATE);
	i2c_init();
	spi_init();

	cst816d_init();
	gc9a01_init();

	gc9a01_fill_screen(0x800000);

}

int main(void) {

	setup();

	int mode = 0;

	while (1)
	{
		// gc9a01_fill_screen(0xFF0000);
		if (received_data_from_cst816d())
		{
			i2c_start();
			i2c_write(I2C_ADDR_CST816D << 1 | 0);
			i2c_write(0x01);
			// i2c_read_ack();

			i2c_start();
			i2c_write((I2C_ADDR_CST816D << 1) | 1);

			uint8_t action = i2c_read_ack();
			uint8_t points = i2c_read_ack();
			uint8_t x_high = i2c_read_ack();
			uint8_t x_low = i2c_read_ack();
			uint8_t y_high = i2c_read_ack();
			uint8_t y_low = i2c_read_nack();

			i2c_stop();

			int x = ((x_high & 0x0F) << 8) | x_low;
			int y = ((y_high & 0x0F) << 8) | y_low;

			if (points > 0) {
				uart0_printstr("action: ");
				uart0_print_10bit(action);
				if (action == TFT_UP)
					uart0_printstr("swipe up\r\n");
				else if (action == TFT_DOWN)
					uart0_printstr("swipe down\r\n");
				else if (action == TFT_RIGHT)
					uart0_printstr("swipe right\r\n");
				else if (action == TFT_LEFT)
					uart0_printstr("swipe left\r\n");
				else if (action == TFT_TAP)
					uart0_printstr("tap\r\n");
				else
					uart0_printstr("autre action\r\n");
				uart0_printstr("Toucher detecte ! X: ");
				uart0_print_10bit(x);
				uart0_printstr(" | Y: ");
				uart0_print_10bit(y);
				uart0_printstr("\r\n");
			}

			if (action != mode)
			{
				mode = action;
				if (mode == TFT_LEFT)
					gc9a01_fill_screen(0x008000);
				if (mode == TFT_RIGHT)
					gc9a01_fill_screen(0x800000);
				if (mode == TFT_UP)
					gc9a01_fill_screen(0x000900);
			}
		}
	}
	return 0;
}
