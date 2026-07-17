#ifndef IMU_H
#define IMU_H

#define IMU_ADDR 0x68

typedef struct imu_data_s {
    int32_t accel_x;
    int32_t accel_y;
    int32_t accel_z;
    int32_t gyro_x;
    int32_t gyro_y;
    int32_t gyro_z;
} imu_data_t;

#define GYRO_LOWEST -524256
#define GYRO_HIGHEST 524286
#define ACCEL_LOWEST -524256
#define ACCEL_HIGHEST 524284

/*****************************************************************************/
/*                              BANK SELECTION                               */
/*****************************************************************************/

#define REG_BANK_SEL 0x76
#define BANK_0 0b00
#define BANK_1 0b01
#define BANK_2 0b10
#define BANK_3 0b11
#define BANK_4 0b100

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
#define PWR_MGMT0 0x4E
#define GYRO_CONFIG0 0x4F
#define ACCEL_CONFIG0 0x50

void set_imu();

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

#define TEMP_DIS 5
#define IDLE 4
#define GYRO_MODE 2
#define ACCEL_MODE 0

#define GYRO_ODR 0
#define ACCEL_ODR 0
#define SAMPLE_COUNT 5

/*****************************************************************************/
/*                              FIFO DATA                                    */
/*****************************************************************************/

#define FIFO_DATA 0x30

void fifo_sampling(imu_data_t *imu_data);

#endif