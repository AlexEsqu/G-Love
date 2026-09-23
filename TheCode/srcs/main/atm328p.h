#ifndef CONFIG_ATM328P_H
#define CONFIG_ATM328P_H

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUDRATE 115200


#define SENSOR_PACKET_MAGIC 0xA55A

typedef struct s_sensor_data
{
    uint16_t magic; //magic number to detect start of packet
    uint16_t sensorForce[5]; //fsr
    uint16_t sensorFlex[5]; //fsr
    int16_t  accel[3]; //imu
    int16_t  gyro[3]; //imu
    uint16_t crc16; // crc to protect and check data authenticity
} t_sensor_data;

extern t_sensor_data g_sensor;

#endif