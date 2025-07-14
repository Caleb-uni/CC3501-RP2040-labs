#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

// Initializes the LED chain (PIO, state machine, etc.)
void LED_INIT(void);

// Sets all LEDs to the specified RGB color
void LED_SET_ALL(uint8_t red, uint8_t green, uint8_t blue);

// Clears all LEDs (turn all LEDs off)
void LED_CLEAR(void);

// Sets an individual LED by index with RGB values
void LED_SET_IND(int index, uint8_t red, uint8_t green, uint8_t blue);

// Sends the current led_data[] to the LEDs
void LED_UPDATE(void);

#endif // LEDS_H

