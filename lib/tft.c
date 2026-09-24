void tft_spi_init()
{
    // Configure MOSI, SCK, et SS as output pins
    DDRB |= (1 << TFT_MOSI) | (1 << TFT_SCK) | (1 << TFT_CS);

    // Enable SPI, Master, set clock rate fck/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void tft_spi_master_transmit(uint8_t data)
{
	SPDR = data;

	while (!(SPSR & (1 << SPIF)))
	{
		;
	}
}

void tft_spi_set_dc_high()
{
	
}

void tft_spi_send_command(uint8_t cmd)
{

}

void tft_spi_send_data(uint8_t data)
{

}

void tft_spi_send()
{

}
