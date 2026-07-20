#include "../includes/main.h"
#include <stdint.h>


void spi_init()
{
    // Configure MOSI, SCK, et SS as output pins
    SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);

    // Enable SPI, Master, set clock rate fck/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}


void spi_send_data(char data)
{
    //Start transmission
    SPDR = data;

    //Wait for transmission complete
    while (!(SPSR & (1<<SPIF)))
    {
    }
}

void spi_start_frame()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        spi_send_data(0);
    }
}

void spi_end_frame()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        spi_send_data(0xff);
    }
}

void spi_send_color(t_rgb_color *colors, uint16_t number, uint16_t brightness)
{
    spi_start_frame();

    for(uint16_t i = 0; i < number; i++)
    {
        spi_send_data(0b11100000 | brightness);
        spi_send_data(colors[i].blue);
        spi_send_data(colors[i].green);
        spi_send_data(colors[i].red);
    }

    spi_end_frame();
}