#include <avr/pgmspace.h>
// NON NECESSARY - MANUAL IMPLEMENTATION OF PGM_READ AND PGM_ADDRESS

// Getting 24 bit pointers for reaching into deep flash memory
// equivalent to GET_FAR_ADDRESS(var)
// See https://forum.arduino.cc/t/compile-array-larger-than-65k/408878/12
#define ft_get_far_address(var)			\
({										\
	uint_farptr_t	address;			\
	__asm__ __volatile__ (				\
		"ldi %A0, lo8(%1)"	"\n\t"		\
		"ldi %B0, hi8(%1)"	"\n\t"		\
		"ldi %C0, hh8(%1)"	"\n\t"		\
		"clr %D0"			"\n\t"		\
		: "=d" (address)				\
		: "p"  (&(var))					\
	);									\
	address;							\
})

// Reads into flash memory further than the basic 64kB
// Extends 16bit pointer in memory with RAMPZ register to 24bit addressing
uint8_t		ft_read_byte_far(uint32_t far_address)
{
	uint8_t	result;

	// Copies last third of address (%C1) into RAMPZ register
	// 		 first third of address (%A1) in Z register low
	// 		 second third of address (%B1) in Z register high
	// Extended Load Program Memory (extends 16bit mem pointer to 24 bit Z + RAMPZ)
	// equivalent to _SFR_IO_ADDR(sfr) to skip address of registers in data address
    __asm__ __volatile__ (		\
		"out %2, %C1"	"\n\t"	\
		"mov r30, %A1"	"\n\t"	\
		"mov r31, %B1"	"\n\t"	\
		"elpm %0, Z"	"\n\t"	\
		: "=r" (result)			\
		: "r" (far_address), "I" (0x1B)	\
		: "r30", "r31"	\
	);

	return result;
}

uint16_t	ft_read_16bit_far(uint_farptr_t far_address)
{
	uint8_t	low = ft_read_byte_far(far_address);
	uint8_t	high = ft_read_byte_far(far_address + 1);

	return (((uint16_t)high << 8) | low);
}
