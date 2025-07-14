#include <stdio.h>
#include "pico/stdlib.h"
#include "drivers/leds.h"
#include "drivers/accell.h"
#include "drivers/mic.h"

#define ACCEL_SENSITIVITY 0.004f
#define NUM_SAMPLES 256

int main() {
    stdio_init_all();
    
    LED_INIT();
    LIS3DH_init();     // Accelerometer init
    MIC_INIT();        // Microphone init

    printf("Accelerometer + MIC demo starting...\n");

    int mode = 1; // 0 = LED test, 1 = accelerometer, 2 = MIC
    uint16_t mic_buffer[NUM_SAMPLES];

    while (true) {
        if (mode == 0) {
            // LED Test Pattern
            LED_SET_ALL(255, 0, 0);
            sleep_ms(500);

            LED_SET_ALL(0, 255, 0);
            sleep_ms(500);

            LED_SET_ALL(0, 0, 255);
            sleep_ms(500);

            LED_CLEAR();
            LED_SET_IND(0, 255, 0, 0);
            LED_SET_IND(1, 0, 255, 0);
            LED_SET_IND(2, 0, 0, 255);
            LED_UPDATE();
            sleep_ms(1000);

            LED_CLEAR();
            sleep_ms(500);

        } else if (mode == 1) {
            // Accelerometer Mode
            LIS3DH_updateReadings();  // Read new accelerometer data

            float gx = values.x * ACCEL_SENSITIVITY;
            float gy = values.y * ACCEL_SENSITIVITY;
            float gz = values.z * ACCEL_SENSITIVITY;

            LED_CLEAR();

            if (gx > 0.3f) {
                LED_SET_IND(0, 255, 0, 0);     // Tilted right → Red
            } else if (gx < -0.3f) {
                LED_SET_IND(1, 0, 255, 0);     // Tilted left → Green
            } else if (gy > 0.3f) {
                LED_SET_IND(2, 0, 0, 255);     // Tilted forward → Blue
            } else if (gy < -0.3f) {
                LED_SET_IND(3, 255, 255, 0);   // Tilted back → Yellow
            } else {
                LED_SET_IND(4, 255, 255, 255); // Flat → White
            }

            LED_UPDATE();
            sleep_ms(200);

        } else if (mode == 2) {
            // Microphone Mode
            MIC_READ(mic_buffer, NUM_SAMPLES);

            // Find peak value in sample
            uint16_t peak = 0;
            for (int i = 0; i < NUM_SAMPLES; ++i) {
                if (mic_buffer[i] > peak) peak = mic_buffer[i];
            }

            // Normalize 12-bit ADC (0-4095)
            float norm = peak / 4095.0f;
            uint8_t brightness = (uint8_t)(norm * 255.0f);

            // Set LEDs based on volume (yellow hue)
            LED_SET_ALL(brightness, brightness, 0);
            sleep_ms(50);
        }
    }
}
