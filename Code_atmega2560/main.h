#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUDRATE 115200

// define screen port
#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9

#define TFT_MOSI 51
#define TFT_MISO 50
#define TFT_SCK 52

// define tactile port
#define I2C_ADDR_CST816D 0x15 // addresse du tactile
#define TOUCH_INT 7

// i2c.c
void i2c_init(void);
void i2c_status(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void i2c_read(void);

// uart.c
void uart0_init(unsigned long baudrate);
void uart0_tx(char c);
char uart0_rx(void);
void uart0_printstr(const char *str);
void uart0_print_10bit(uint32_t c);

void uart1_init(unsigned long baudrate);
void uart1_tx(char c);
char uart1_rx(void);
void uart1_printstr(const char *str);
void uart1_print_10bit(uint32_t c);
uint8_t uart1_available(void);

uint8_t ft_hex(uint8_t val);
void ft_uart_print_hex(uint8_t c);
#endif