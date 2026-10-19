#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stddef.h>

#if defined(__AVR_ATmega2560__)
    #define SPI_DDR   DDRB
    #define SPI_MOSI  DDB2
    #define SPI_SCK   DDB1
    #define SPI_SS    DDB0
	#include "atm2560.h"
#elif defined(__AVR_ATmega2561__)
    #define SPI_DDR   DDRB
    #define SPI_MOSI  DDB2
    #define SPI_SCK   DDB1
    #define SPI_SS    DDB0
	#include "atm2561.h"
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    #define SPI_DDR   DDRB
    #define SPI_MOSI  DDB3
    #define SPI_SCK   DDB5
    #define SPI_SS    DDB2
	#include "atm328p.h"
#endif

int main_ATM2560();
int main_ATM328P();

#include "touchscreen.h"
#include "comms.h"
#include "led.h"

#define I2C_ADDR_CST816D 0x15 // tactile driver address

// Globals
extern tft_state_t			g_touchscreen_mode;
extern uint8_t				g_animation_frame;
extern uint16_t				g_ms_counter;
extern uint8_t				g_touch_debounce;
extern volatile uint16_t	g_i2C_state;

//adc.c
void		adc_init();
uint8_t		ft_adc_read();
uint16_t	ft_adc_read_10bit();
uint16_t    ft_adc_read_channel(uint8_t channel);

//crc.c
uint8_t     crc8_compute(const uint8_t *data, size_t len);
uint16_t    crc16_compute(const uint8_t *data, size_t len);

// i2c.c
#define I2C_TIMEOUT 10000U

void		i2c_init(void);
void		i2c_status(void);
void		i2c_start(void);
void		i2c_stop(void);
void		i2c_write(unsigned char data);
uint8_t		i2c_read_ack(void);
uint8_t		i2c_read_nack(void);
void		i2c_read(void);
void 		i2c_scan(void);


//spi.c
void		spi_init();
uint8_t 	spi_send_data(uint8_t data);
void		spi_send_color(rgb_color_t *colors,
				uint16_t number, uint16_t brightness);

// uart.c
void 		uart0_init(unsigned long baudrate);
void 		uart0_tx(char c);
char 		uart0_rx(void);
void 		uart0_printstr(const char *str);
void 		uart0_print_10bit(uint32_t c);
void        uart0_send_data(const uint8_t *data, size_t len);
uint8_t     uart0_available(void);

void 		uart1_init(unsigned long baudrate);
void 		uart1_tx(char c);
char 		uart1_rx(void);
void 		uart1_printstr(const char *str);
void 		uart1_print_10bit(uint32_t c);
void        uart1_send_data(const uint8_t *data, size_t len);
uint8_t 	uart1_available(void);

void        uart_init_test(void);


uint8_t		ft_hex(uint8_t val);
void 		ft_uart_print_hex(uint8_t c);

// readability
void	    set_pin_as_input(volatile uint8_t* reg, uint8_t pin);
void	    set_pin_as_output(volatile uint8_t* reg, uint8_t pin);
void	    set_pin_high(volatile uint8_t* reg, uint8_t pin);
void	    set_pin_low(volatile uint8_t* reg, uint8_t pin);

// watchdog.c
void        watchdog_disable(void);

//utils.c
void        ft_memcpy(void *dest, const void *src, size_t n);

#endif
