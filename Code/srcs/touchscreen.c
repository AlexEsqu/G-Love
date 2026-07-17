#include <G_Love.h>

void touchscreen_read(touchscreen_data_t *data)
{
    uint8_t buf[5];
    twi_read_burst(TOUCHSCREEN_ADDR, 0x02, buf, 5);
    if (buf[0] > 0) // Vérifie si un point de contact est détecté
    {
        data->x = ((buf[1] & 0x0F) << 8) | buf[2];
        data->y = ((buf[3] & 0x0F) << 8) | buf[4];
    }
}

