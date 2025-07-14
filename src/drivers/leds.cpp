#include "leds.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"

#define LED_PIN 14
#define NUM_LEDS 12

static uint32_t led_data[NUM_LEDS];
static PIO pio = pio0;
static int sm = 0;

void LED_INIT() {
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LED_PIN, 800000, false);
    LED_CLEAR();
}

void LED_UPDATE() {
    for (int i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio, sm, led_data[i] << 8u); // shift to fit 24-bit GRB format
    }
}

void LED_CLEAR() {
    for (int i = 0; i < NUM_LEDS; i++) {
        led_data[i] = 0;
    }
    LED_UPDATE();
}

void LED_SET_ALL(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t color = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
    for (int i = 0; i < NUM_LEDS; i++) {
        led_data[i] = color;
    }
    LED_UPDATE();
}

void LED_SET_IND(int index, uint8_t red, uint8_t green, uint8_t blue) {
    if (index < 0 || index >= NUM_LEDS) return;
    uint32_t color = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
    led_data[index] = color;
    // NOTE: Call LED_UPDATE() manually if batching updates
}
