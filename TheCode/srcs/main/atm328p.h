#ifndef CONFIG_ATM328P_H
#define CONFIG_ATM328P_H

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

//#define BAUDRATE 115200

#define BAUDRATE 57600UL
#define MYUBRR ((F_CPU + BAUDRATE * 4) / (BAUDRATE * 8) - 1)
#endif