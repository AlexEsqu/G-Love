#include "main.h"

volatile uint16_t g_state = 0;

void i2c_init(void) {
  /*
          DATASHEET PAGE 222
          TWBR = Value of the TWI Bit Rate Register
          PrescalerValue = Value of the prescale // datasheet page241 table22-7

          SCL frequency = CPU Clock frequency / (16 + 2(TWBR) * PrescalerValue)
     //datasheet page222 400.000 = 16.000.000 / (16 + 2(TWBR) * PrescalerValue);
          (16 + 2(TWBR) * PrescalerValue) = 40
          TWBR = 40-16 / 2 / PrescalerValue
          TWBR = 12 / PrescalerValue
  */

  // datasheet page 239	section 22.9.1
  TWBR |= 12; // for scl = 400k

  // datasheet page 240	section 22.9.3
  TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // set prescaler to 1? ;

  // datasheet page 226	section 22.7.1  and 22.7.3
  //  TWCR |= (1 << TWEN);
  // TWEN must be set to enable the 2-wire Serial Interface
  // TWEN must be written to one to enable the TWI
}

void ft_print_status(void) {
  uart0_printstr("Status code: 0x");
  ft_uart_print_hex(g_state);
  uart0_printstr(" : ");

  switch (g_state) {
  case 0x08:
    uart0_printstr("START condition transmitted");
    break;
  case 0x10:
    uart0_printstr("Repeated START transmitted");
    break;

  case 0x18:
    uart0_printstr("SLA+W transmitted, ACK received");
    break;

  case 0x20:
    uart0_printstr("SLA+W transmitted, NACK received");
    break;

  case 0x28:
    uart0_printstr("Data byte transmitted, ACK received");
    break;

  case 0x30:
    uart0_printstr("Data byte transmitted, NACK received");
    break;

  case 0x40:
    uart0_printstr("SLA+R transmitted, ACK received");
    break;

  case 0x48:
    uart0_printstr("SLA+R transmitted, NACK received");
    break;

  case 0x50:
    uart0_printstr("Data byte received, ACK returned");
    break;

  case 0x58:
    uart0_printstr("Data byte received, NACK returned");
    break;

  case 0x38:
    uart0_printstr("Arbitration lost");
    break;

  case 0x00:
    uart0_printstr("Bus error (Illegal START/STOP)");
    break;

  default:
    uart0_printstr("Unknown status");
    break;
  }
  uart0_printstr("\r\n");
}

void i2c_status(void) {
  // DATASHEET PAGE 240 SECTION 22.9.3
  // get status code with a mask to hide prescaler bit
  g_state = TWSR & 0xF8;
  //ft_print_status();
}

void i2c_start(void) {
  // DATAHSSET PAGE 239 SECTION 22.9.2 //note1
  // TWINT: interrupt flag
  // TWSTA: start condition bit
  // TWEN: enable bit
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

  while (!(TWCR & (1 << TWINT))) {
  }

  i2c_status();
}

void i2c_stop(void) { TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); }

/*
DATASHEET PAGE 224
The application must load a data packet into TWDR. Subsequently, a
specific value must be written to TWCR, instructing the TWI hardware to transmit
the data packet present in TWDR.
*/
void i2c_write(unsigned char data) {
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);

  while (!(TWCR & (1 << TWINT))) {
  }

  i2c_status();
}

uint8_t i2c_read_ack(void) {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  while (!(TWCR & (1 << TWINT))) {
  }

  i2c_status();
  return TWDR;
}

uint8_t i2c_read_nack(void) {
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
  }

  i2c_status();
  return TWDR;
}

// Read the next 7 bytes to get the data and display it
void i2c_read(void) {}