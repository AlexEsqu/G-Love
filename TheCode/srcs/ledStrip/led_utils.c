#include "led.h"
#include "../../includes/main.h"

void led_effect(uint8_t finger, uint8_t effect)
{
    switch (finger)
    {
        case THUMB_LED:
            // Apply effect to thumb LEDs
            break;
        case INDEX_LED:
            // Apply effect to index LEDs
            break;
        case MIDDLE_LED:
            // Apply effect to middle LEDs
            break;
        case RING_LED:
            // Apply effect to ring LEDs
            break;
        case PINKY_LED:
            // Apply effect to pinky LEDs
            break;
        default:
            return;
            break;
    }
}

