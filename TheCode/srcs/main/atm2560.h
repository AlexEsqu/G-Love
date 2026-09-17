#ifndef CONFIG_ATM2560_H
#define CONFIG_ATM2560_H

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



#endif