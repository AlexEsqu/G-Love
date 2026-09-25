#include <stdint.h>
#include "mux.h"
#include "../../includes/main.h"
#include "../main/atm328p.h"


#ifdef PROTO
//Channel function used for multiplexer CD4067: 4 select pins and 16 channels
void selectChannel(int channel)
{
	if (channel & 1)
		PORTD |= (1 << S0);
	else
		PORTD &= ~(1 << S0);

	if (channel & 2)
		PORTD |= (1 << S1);
	else
		PORTD &= ~(1 << S1);

	if (channel & 4)
		PORTD |= (1 << S2);
	else	
		PORTD &= ~(1 << S2);

	if (channel & 8)
		PORTD |= (1 << S3);
	else
		PORTD &= ~(1 << S3);
}

int readChannel(int channel)
{
	selectChannel(channel);
	_delay_ms(50);
	sensorState[channel] = ft_adc_read_10bit();

	//uart0_printstr("channel:");
	//uart0_print_10bit(channel);
	//uart0_printstr(" value:");
	//uart0_print_10bit(sensorState[channel]);
	//uart0_printstr("\n");

	return sensorState[channel];
}

void readAllChannels(int max) {
	for (int i = 0; i < max && i < 16; i++)
	{
		readChannel(i);
	}
}
#endif

#ifndef PROTO
void selectMuxChannel(uint8_t channel)
{
	if (channel & 1)
		PORTC |= (1 << PIN_SEL_C);
	else
		PORTC &= ~(1 << PIN_SEL_C);

	if (channel & 2)
		PORTC |= (1 << PIN_SEL_B);
	else
		PORTC &= ~(1 << PIN_SEL_B);

	if (channel & 4)
		PORTC |= (1 << PIN_SEL_A);
	else	
		PORTC &= ~(1 << PIN_SEL_A);
}

int readMuxChannel(uint8_t channel, uint8_t adcPin)
{
	selectMuxChannel(channel);
	_delay_ms(50);


	//uart0_printstr("channel:");
	//uart0_print_10bit(channel);
	//uart0_printstr(" value:");
	if (adcPin == 6)
	{
		g_sensor.sensorForce[channel] = ft_adc_read_channel(adcPin);

		//uart0_print_10bit(g_sensor.sensorForce[channel]);
		//uart0_printstr("\n");
		return g_sensor.sensorForce[channel];
	}
	else 
	{
		g_sensor.sensorFlex[channel] = ft_adc_read_channel(adcPin);

		//uart0_print_10bit(g_sensor.sensorFlex[channel]);
		//uart0_printstr("\n");
		return g_sensor.sensorFlex[channel];
	}
	return (-1);
}

void readAllMuxChannels(int max) {
	for (int i = 0; i < max && i < MAX_CHANNEL; i++)
	{
		readMuxChannel(i, PIN_ADC_X);
		readMuxChannel(i, PIN_ADC_Y);
	}
}
#endif


void mux_setup()
{
	#ifdef PROTO
		// init selector pins
		DDRD |= (1 << S0);
		DDRD |= (1 << S1);
		DDRD |= (1 << S2);
		DDRD |= (1 << S3);

		DDRD |= (1 << EN);
		PORTD &= ~(1 << EN);

		//sig is our read pin (A0 on Uno corresponds to PC0 / Channel 0)
		DDRC &= ~(1 << 0);
		PORTC &= ~(1 << 0);
	#else
		//to redo and check
		DDRC |= (1 << PIN_SEL_A);
		DDRC |= (1 << PIN_SEL_B);
		DDRC |= (1 << PIN_SEL_C);

		
		DDRC &= ~(1 << PIN_ADC_X);
		DDRC &= ~(1 << PIN_ADC_Y);
	#endif
}

void mux_loop() 
{
	#ifdef PROTO
		// read adc pin with multiplexer
		readAllChannels(2);
	#else
		readAllMuxChannels(5);
	#endif
}