#ifndef COMM_H
#define COMM_H

#include <stdint.h>
#include <stdbool.h>

#define SENSOR_PACKET_MAGIC_SENSOR_FROM_328 0xA55A
#define SENSOR_PACKET_MAGIC_ALL_DATA_FROM_2561 0xA55A
#define SENSOR_PACKET_MAGIC_DIRECTIVE_TO_328 0xA55A

typedef struct __attribute__((packed)) s_sensor_data
{
    uint16_t magic; //magic number to detect start of packet
    uint16_t sensorForce[5]; //fsr
    uint16_t sensorFlex[5]; //fsr
    int16_t  accel[3]; //imu
    int16_t  gyro[3]; //imu
    uint16_t crc16; // crc to protect and check data authenticity
} t_sensor_data;

// glove_directive_t.command: one-shot order for the 328P
#define GLOVE_CMD_NONE			0
#define GLOVE_CMD_IMU_RESET		1	// reset and calibrate the IMU again (glove must not move)

// atm2561 -> atm328P
typedef struct __attribute__((packed)) s_glove_directive
{
	uint16_t	magic; //magic number to detect start of packet
	uint8_t		current_mode;
	uint32_t	current_color;
	uint8_t		command; // GLOVE_CMD_xxx
	uint16_t	crc16; // crc to protect and check data authenticity
} glove_directive_t;

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)

typedef struct __attribute__((packed)) s_sensor_data_to_pc
{
    uint16_t			magic; //magic number to detect start of packet
    uint16_t			sensorForce[5]; //fsr
    uint16_t			sensorFlex[5]; //fsr
    int16_t 			accel[3]; //imu
    int16_t				gyro[3]; //imu
	tft_touch_info_t	last_touch;
	tft_state_t			touchscreen_state;
	uint16_t			crc16; // crc to protect and check data authenticity
} t_sensor_data_to_pc;

typedef enum {
	OUTPUT_UART_TO_SERIAL = 0,
	OUTPUT_UART_TO_JST = 1,
	OUTPUT_TO_BMX = 2
} comm_output_e ;

extern glove_directive_t	g_love_directive;
extern t_sensor_data_to_pc	g_packet_to_export;
extern comm_output_e		g_current_uart_output;

void	update_sensor_data(uint8_t *byte);
void	send_g_love_directive();
bool	process_packet(uint8_t *byte, t_sensor_data *packet);

#endif

#endif
