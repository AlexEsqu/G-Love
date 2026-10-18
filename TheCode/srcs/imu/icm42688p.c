#include <stdint.h>
#include "icm42688p.h"
#include "../../includes/main.h"

/*
STEPS:
I2C BUS

WHO AM I
    read 0x75
    expected response 0x47

RESET
    write 0x01 at 0x11
    write

1ms wait

actiavte gyro and accel
    write 0x0F in 0x4E

wait 45ms
*/

void ICM42688_init()
{

}