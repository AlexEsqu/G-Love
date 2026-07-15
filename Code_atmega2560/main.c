#include "main.h"

void setup() {
  // uart_init
  uart0_init(BAUDRATE);
  uart1_init(BAUDRATE);

  // pin 7 en input
  DDRD &= ~(1 << DDD7);
  // pullup
  PORTD |= (1 << PORTD7);

  // i2c init
  i2c_init();
}

int main(void) {

  setup();

  while (1) {
    // uart0_printstr("test\n");
    if (PIND & (1 << PIND7)) {

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
        if (action == 1)
          uart0_printstr("swipe up\n");
        else if (action == 2)
          uart0_printstr("swipe down\n");
        else if (action == 3)
          uart0_printstr("swipe right\n");
        else if (action == 4)
          uart0_printstr("swipe left\n");
        else if (action == 0)
          uart0_printstr("tap\n");
        else
          uart0_printstr("autre action\n");
        uart0_printstr("Toucher detecte ! X: ");
        uart0_print_10bit(x);
        uart0_printstr(" | Y: ");
        uart0_print_10bit(y);
        uart0_printstr("\n");
      }
    }
    /*
    if (uart1_available() > 0)
    {
        uart0_tx(uart1_rx());
    }
    */
  }
  return 0;
}