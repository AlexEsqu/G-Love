#ifndef ICM42688P_H
 #define ICM42688P_H

#include "../../includes/main.h"

#define ICM_ADDR 0x68
#define ICM_ADDR_VDD 0x69

#define WHO_AM_I_ADDR 0x75
#define WHO_AM_I_RESPONSE 0x47

#define DEVICE_CONFIG 0x11

#define PWR_MGMT0 0x4E // ACTIVATE gyro and accel

#define REG_BANK_SEL 0x76 //selection de registre

#define ACCEL_CONFIG0 0x50
#define GYRO_CONFIG0 0x4F

#endif