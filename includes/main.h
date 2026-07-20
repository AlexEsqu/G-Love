#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#if defined(__AVR_ATmega2560__)
    #define SPI_DDR   DDRB
    #define SPI_MOSI  DDB2
    #define SPI_SCK   DDB1
    #define SPI_SS    DDB0
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    #define SPI_DDR   DDRB
    #define SPI_MOSI  DDB3
    #define SPI_SCK   DDB5
    #define SPI_SS    DDB2
#endif

typedef struct s_rgb_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_rgb_color;

//adc.c
void		adc_init();
uint8_t		ft_adc_read();
uint16_t 	ft_adc_read_10bit();

// i2c.c
void 		i2c_init(void);
void 		i2c_status(void);
void 		i2c_start(void);
void 		i2c_stop(void);
void 		i2c_write(unsigned char data);
uint8_t 	i2c_read_ack(void);
uint8_t 	i2c_read_nack(void);
void 		i2c_read(void);


//spi.c
void        spi_init();
void        spi_send_data(char data);
void        spi_send_color(t_rgb_color *colors,
                uint16_t number, uint16_t brightness);

// uart.c
void 		uart0_init(unsigned long baudrate);
void 		uart0_tx(char c);
char 		uart0_rx(void);
void 		uart0_printstr(const char *str);
void 		uart0_print_10bit(uint32_t c);

void 		uart1_init(unsigned long baudrate);
void 		uart1_tx(char c);
char 		uart1_rx(void);
void 		uart1_printstr(const char *str);
void 		uart1_print_10bit(uint32_t c);
uint8_t 	uart1_available(void);

uint8_t		ft_hex(uint8_t val);
void 		ft_uart_print_hex(uint8_t c);

#endif