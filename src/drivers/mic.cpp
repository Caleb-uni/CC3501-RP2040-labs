#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mic.h"

void MIC_INIT() {
    adc_init();

    // Configure GPIO26 (ADC0) for analog input
    adc_gpio_init(26);
    adc_select_input(0);  // Use ADC0

    // Configure the FIFO
    adc_fifo_setup(
        true,    // Write each result to the FIFO
        true,    // Enable DMA data request (not used here)
        1,       // DREQ when 1 sample available
        false,   // No error bit
        false    // Don't shift results to 8 bits
    );

    // Set the ADC clock divider to get 44.1 kHz sampling
    // clkdiv = (48MHz / 44100Hz) - 1 ≈ 1088
    adc_set_clkdiv(1088.0f);
}

void MIC_READ(uint16_t* buffer, uint16_t num_samples) {
    // Start ADC in free-running mode
    adc_run(true);

    for (uint16_t i = 0; i < num_samples; ++i) {
        buffer[i] = adc_fifo_get_blocking();  // Read one sample
    }

    // Stop ADC
    adc_run(false);

    // Drain any leftover samples in the FIFO (safety flush)
    while (adc_fifo_get_level() > 0) {
        (void)adc_fifo_get();
    }
}
