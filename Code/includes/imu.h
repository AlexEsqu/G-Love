#ifndef IMU_H
#define IMU_H

/*****************************************************************************/
/*                              BANK SELECTION                               */
/*****************************************************************************/

#define REG_BANK_SEL 0x76
#define BANK_0 0b00
#define BANK_1 0b01
#define BANK_2 0b10
#define BANK_3 0b11
#define BANK_4 0b100

void select_bank(uint8_t bank);

/*****************************************************************************/
/*                         FIFO CONFIG REGISTERS                             */
/*****************************************************************************/

#define FIFO_CONFIG 0x16
#define FIFO_CONFIG1 0x5F
#define FIFO_CONFIG2 0x60
#define FIFO_CONFIG3 0x61
#define INTF_CONFIG0 0x4C
#define FIFO_COUNTL 0x2F
#define FIFO_COUNTH 0x2E

/*****************************************************************************/
/*                              FIFO HEADER BITS                             */
/*****************************************************************************/

#define HEADER_MSG 7
#define HEADER_ACCEL 6
#define HEADER_GYRO 5
#define HEADER_20 4
#define HEADER_ODR_ACCEL 1
#define HEADER_ODR_GYRO 0

/*****************************************************************************/
/*                              FIFO CONFIG BITS                             */
/*****************************************************************************/

#define FIFO_RESUME_PARTIAL_RD 6
#define FIFO_WM_GT_TH 5
#define FIFO_HIRES_EN 4
#define FIFO_TMST_FSYNC_EN 3
#define FIFO_TEMP_EN 2
#define FIFO_GYRO_EN 1
#define FIFO_ACCEL_EN 0

#define FIFO_HOLD_LAST_DATA_EN 7
#define FIFO_COUNT_REC 6
#define FIFO_COUNT_ENDIAN 5
#define SENSOR_DATA_ENDIAN 4

/*****************************************************************************/
/*                              FIFO DATA REGISTERS                          */
/*****************************************************************************/

#define FIFO_DATA 0x30

#define FIFO_HEADER 0x00

#define ACCEL_X_1 0x01 
#define ACCEL_X_2 0x02 
#define ACCEL_Y_1  0x03 
#define ACCEL_Y_2  0x04 
#define ACCEL_Z_1  0x05 
#define ACCEL_Z_2  0x06

#define GYRO_X_1  0x07 
#define GYRO_X_2  0x08 
#define GYRO_Y_1  0x09 
#define GYRO_Y_2  0x0A 
#define GYRO_Z_1  0x0B 
#define GYRO_Z_2  0x0C

#define ACCEL_GYRO_X_3 0x11
#define ACCEL_GYRO_Y_3 0x12 
#define ACCEL_GYRO_Z_3 0x13 

#define TEMP_1 0x0D 
#define TEMP_2 0x0E 

#define TIMESTAMP_1 0x0F 
#define TIMESTAMP_2 0x10 

#endif