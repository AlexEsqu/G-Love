#include <stdint.h>
#include <stddef.h>

// Calcul CRC-8 
uint8_t crc8_compute(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;
    while (len--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// Calcul CRC-16
uint16_t crc16_compute(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    while (len--) {
        crc ^= (uint16_t)*data++ << 8;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}