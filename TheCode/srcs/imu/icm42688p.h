#ifndef ICM42688P_H
#define ICM42688P_H

#include "main.h"
#include <stdint.h>
#include <stdlib.h>

#define ICM_ADDR 0x68u

/* Example register names, aligned with the C++ example */
#define REG_BANK_SEL                  0x76u

#define UB0_REG_DEVICE_CONFIG         0x11u
#define UB0_REG_DRIVE_CONFIG          0x13u
#define UB0_REG_INT_CONFIG            0x14u
#define UB0_REG_TEMP_DATA1            0x1Du
#define UB0_REG_TEMP_DATA0            0x1Eu
#define UB0_REG_ACCEL_DATA_X1         0x1Fu
#define UB0_REG_ACCEL_DATA_X0         0x20u
#define UB0_REG_ACCEL_DATA_Y1         0x21u
#define UB0_REG_ACCEL_DATA_Y0         0x22u
#define UB0_REG_ACCEL_DATA_Z1         0x23u
#define UB0_REG_ACCEL_DATA_Z0         0x24u
#define UB0_REG_GYRO_DATA_X1          0x25u
#define UB0_REG_GYRO_DATA_X0          0x26u
#define UB0_REG_GYRO_DATA_Y1          0x27u
#define UB0_REG_GYRO_DATA_Y0          0x28u
#define UB0_REG_GYRO_DATA_Z1          0x29u
#define UB0_REG_GYRO_DATA_Z0          0x2Au
#define UB0_REG_TMST_FSYNCH           0x2Bu
#define UB0_REG_TMST_FSYNCL           0x2Cu
#define UB0_REG_INT_STATUS            0x2Du
#define UB0_REG_FIFO_COUNTH           0x2Eu
#define UB0_REG_FIFO_COUNTL           0x2Fu
#define UB0_REG_FIFO_DATA             0x30u
#define UB0_REG_APEX_DATA0            0x31u
#define UB0_REG_APEX_DATA1            0x32u
#define UB0_REG_APEX_DATA2            0x33u
#define UB0_REG_APEX_DATA3            0x34u
#define UB0_REG_APEX_DATA4            0x35u
#define UB0_REG_APEX_DATA5            0x36u
#define UB0_REG_INT_STATUS2           0x37u
#define UB0_REG_INT_STATUS3           0x38u
#define UB0_REG_SIGNAL_PATH_RESET      0x4Bu
#define UB0_REG_INTF_CONFIG0           0x4Cu
#define UB0_REG_INTF_CONFIG1           0x4Du
#define UB0_REG_PWR_MGMT0              0x4Eu

/* Compatibility aliases for the current C code */
#define DEVICE_CONFIG                 UB0_REG_DEVICE_CONFIG
#define DRIVE_CONFIG                  UB0_REG_DRIVE_CONFIG
#define INT_CONFIG                    UB0_REG_INT_CONFIG
// #define PWR_MGMT0                     UB0_REG_PWR_MGMT0
#define ACCEL_CONFIG0                 0x50u
#define GYRO_CONFIG0                  0x51u

#define WHO_AM_I_ADDR                 0x75u
#define WHO_AM_I_RESPONSE             0x47u

/* Accel data registers */
#define ACCEL_DATA_X1               0x1Fu
#define ACCEL_DATA_X0               0x20u
#define ACCEL_DATA_Y1               0x21u
#define ACCEL_DATA_Y0               0x22u
#define ACCEL_DATA_Z1               0x23u
#define ACCEL_DATA_Z0               0x24u

/* Gyro data registers */
#define GYRO_DATA_X1                0x25u
#define GYRO_DATA_X0                0x26u
#define GYRO_DATA_Y1                0x27u
#define GYRO_DATA_Y0                0x28u
#define GYRO_DATA_Z1                0x29u
#define GYRO_DATA_Z0                0x2Au


/* These are not in the example snippet; add exact datasheet values if needed */
#define GYRO_CONFIG_STATIC2           0x00u
#define ACCEL_CONFIG_STATIC2          0x00u

typedef struct s_imu_reg
{
    uint8_t reg;
    uint8_t value;
} t_imu_reg;

typedef struct s_imu_sample
{
    float accel[3];
    float gyro[3];
} t_imu_sample;

typedef struct s_imu
{
    uint8_t address;
    uint8_t bank;
    t_imu_reg reset;
    t_imu_reg power_mgmt0;
    t_imu_reg accel_config0;
    t_imu_reg gyro_config0;
    t_imu_sample sample;
} t_imu;

typedef struct s_imu_data
{
    float accel[3];
    float gyro[3];
} t_imu_data;

t_imu *imu_conf_create(void);
t_imu_data *imu_data_create(void);
int ICM42688_init(t_imu *imu);
int imu_get_raw_data(t_imu *imu, t_imu_data *data);

#endif
#define ICM_ADDR                    0x68u

/* Common registers */
#define REG_BANK_SEL                0x76u
#define WHO_AM_I_ADDR               0x75u
#define WHO_AM_I_RESPONSE           0x47u

/* User Bank 0 registers (aliases from example registers.h) */
#define UB0_REG_DEVICE_CONFIG       0x11u
#define UB0_REG_DRIVE_CONFIG        0x13u
#define UB0_REG_INT_CONFIG          0x14u
#define UB0_REG_FIFO_CONFIG         0x16u
#define UB0_REG_TEMP_DATA1          0x1Du
#define UB0_REG_TEMP_DATA0          0x1Eu

/* Accel data registers */
#define ACCEL_DATA_X1               0x1Fu
#define ACCEL_DATA_X0               0x20u
#define ACCEL_DATA_Y1               0x21u
#define ACCEL_DATA_Y0               0x22u
#define ACCEL_DATA_Z1               0x23u
#define ACCEL_DATA_Z0               0x24u

/* Gyro data registers */
#define GYRO_DATA_X1                0x25u
#define GYRO_DATA_X0                0x26u
#define GYRO_DATA_Y1                0x27u
#define GYRO_DATA_Y0                0x28u
#define GYRO_DATA_Z1                0x29u
#define GYRO_DATA_Z0                0x2Au

/* Other useful UB0 registers */
#define UB0_REG_INT_STATUS          0x2Du
#define UB0_REG_FIFO_COUNTH         0x2Eu
#define UB0_REG_FIFO_COUNTL         0x2Fu
#define UB0_REG_FIFO_DATA           0x30u

/* Power / config registers */
#define PWR_MGMT0                   0x4Eu
#define ACCEL_CONFIG0               0x50u
#define GYRO_CONFIG0                0x51u

/* Placeholder static config registers (add datasheet values if needed) */
#define GYRO_CONFIG_STATIC2         0x00u
#define ACCEL_CONFIG_STATIC2        0x00u