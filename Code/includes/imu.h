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

enum {
    GYRO_LOWEST = -524256,
    GYRO_HIGHEST = 524286,
    ACCEL_LOWEST = -524256,
    ACCEL_HIGHEST = 524284
};

/*****************************************************************************/
/*                              BANK SELECTION                               */
/*****************************************************************************/

#define REG_BANK_SEL 0x76
enum imu_banks {
    BANK_0 = 0,
    BANK_1 = 1,
    BANK_2 = 2,
    BANK_3 = 3,
    BANK_4 = 4
};

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

enum fifo_header_bits {
    HEADER_MSG = 7,
    HEADER_ACCEL = 6,
    HEADER_GYRO = 5,
    HEADER_20 = 4,
    HEADER_ODR_ACCEL = 1,
    HEADER_ODR_GYRO = 0
};

/*****************************************************************************/
/*                              FIFO CONFIG BITS                             */
/*****************************************************************************/

enum fifo_config_bits {
    FIFO_RESUME_PARTIAL_RD = 6,
    FIFO_WM_GT_TH = 5,
    FIFO_HIRES_EN = 4,
    FIFO_TMST_FSYNC_EN = 3,
    FIFO_TEMP_EN = 2,
    FIFO_GYRO_EN = 1,
    FIFO_ACCEL_EN = 0
};

enum fifo_config_flags {
    FIFO_HOLD_LAST_DATA_EN = 7,
    FIFO_COUNT_REC = 6,
    FIFO_COUNT_ENDIAN = 5,
    SENSOR_DATA_ENDIAN = 4
};

enum pwr_mgmt_bits {
    TEMP_DIS = 5,
    IDLE = 4,
    GYRO_MODE = 2,
    ACCEL_MODE = 0
};

#define GYRO_ODR 0
#define ACCEL_ODR 0
#define SAMPLE_COUNT 5 /* Nombre d'échantillons à moyenne (5 samples @ 1kHz = moyenne sur 5 valeurs) */

/*****************************************************************************/
/*                              FIFO DATA                                    */
/*****************************************************************************/

#define FIFO_DATA 0x30

void fifo_sampling(imu_data_t *imu_data);

#endif