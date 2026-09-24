#ifndef CONFIG_ATM2560_H
#define CONFIG_ATM2560_H

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define MYUBRR F_CPU/16/BAUD-1
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



#define RX_BUFFER_SIZE 64
#define SENSOR_PACKET_MAGIC2 0xA55A

typedef struct s_sensor_data_to_pc
{
    uint16_t	magic; //magic number to detect start of packet
    uint16_t	sensorForce[5]; //fsr
    uint16_t	sensorFlex[5]; //fsr
    int16_t 	accel[3]; //imu
    int16_t		gyro[3]; //imu
    uint8_t		screen_x; //touchscreen x cordinate
    uint8_t		screen_y; //touchscreen y cordinate
    uint8_t		screen_adction; //touchscreen: action= swipe left, swipe right, press etc...
    uint16_t	crc16; // crc to protect and check data authenticity
} t_sensor_data_to_pc;

extern t_sensor_data_to_pc g_packet;

#endif