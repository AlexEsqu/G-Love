#include "../../includes/main.h"

volatile uint16_t g_state = 0;

void i2c_init(void) {
  // Pull-up intern pour SDA et SCL
  // #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
  //   PORTC |= (1 << PORTC4) | (1 << PORTC5); // PC4=SDA, PC5=SCL
  // #elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
  //   PORTD |= (1 << PORTD1) | (1 << PORTD0); // PD1=SDA, PD0=SCL
  // #endif

  // 100 kHz F_CPU = 8 MHz:
  // SCL = 8MHz / (16 + 2 * TWBR * Prescaler) => TWBR = (80 - 16)/2 = 32
  TWBR = 32;

  // Prescaler = 1
  TWSR &= ~((1 << TWPS1) | (1 << TWPS0));

	// datasheet page 226	section 22.7.1	and 22.7.3
	//	TWCR |= (1 << TWEN);
	// TWEN must be set to enable the 2-wire Serial Interface
	// TWEN must be written to one to enable the TWI
}

void ft_print_status(void)
{
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

void i2c_status(void)
{
	// DATASHEET PAGE 240 SECTION 22.9.3
	// get status code with a mask to hide prescaler bit
	g_state = TWSR & 0xF8;
	//ft_print_status();
}

void i2c_start(void) {
  uint16_t timeout = I2C_TIMEOUT;

  // DATAHSSET PAGE 239 SECTION 22.9.2 //note1
  // TWINT: interrupt flag
  // TWSTA: start condition bit
  // TWEN: enable bit
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

  while (!(TWCR & (1 << TWINT)) && --timeout)
  {
  }

  if (!timeout)
  {
    g_state = 0xFF;
    return;
  }

	i2c_status();
}

void i2c_stop(void) {
  uint16_t timeout = I2C_TIMEOUT;

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while ((TWCR & (1 << TWSTO)) && --timeout)
  {
  }
}

/*
DATASHEET PAGE 224
The application must load a data packet into TWDR. Subsequently, a
specific value must be written to TWCR, instructing the TWI hardware to transmit
the data packet present in TWDR.
*/
void i2c_write(unsigned char data) {
  uint16_t timeout = I2C_TIMEOUT;

  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);

  while (!(TWCR & (1 << TWINT)) && --timeout)
  {
  }

  if (!timeout)
  {
    g_state = 0xFF;
    return;
  }

	i2c_status();
}

uint8_t i2c_read_ack(void) {
  uint16_t timeout = I2C_TIMEOUT;

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  while (!(TWCR & (1 << TWINT)) && --timeout)
  {
  }

  if (!timeout)
  {
    g_state = 0xFF;
    return 0;
  }

	i2c_status();
	return TWDR;
}

uint8_t i2c_read_nack(void) {
  uint16_t timeout = I2C_TIMEOUT;

  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)) && --timeout)
  {
  }

  if (!timeout)
  {
    g_state = 0xFF;
    return 0;
  }

	i2c_status();
	return TWDR;
}

// Read the next 7 bytes to get the data and display it
void i2c_read(void) {}


void i2c_scan(void) {
  uint8_t count = 0;

  uart0_printstr("\r\nI2C Scan:\r\n");

  for (uint8_t addr = 1; addr < 127; addr++) {
    i2c_start();

    // Check if START / Repeated START was sent
    if ((TWSR & 0xF8) != 0x08 && (TWSR & 0xF8) != 0x10) {
      i2c_stop();
      continue;
    }

    // Write address + write bit (SLA+W)
    i2c_write((addr << 1) | 0);

    // 0x18 = SLA+W transmitted, ACK received
    if ((TWSR & 0xF8) == 0x18) {
      uart0_printstr("Found device at: 0x");
      ft_uart_print_hex(addr);
      uart0_printstr("\r\n");
      count++;
    }

    i2c_stop();
    _delay_us(50);
  }

  if (count == 0) {
    uart0_printstr("No I2C devices found\r\n");
  } else {
    uart0_printstr("Scan done, nbr device found: ");
    uart0_print_10bit(count);
    uart0_printstr("\r\n");
  }
}
