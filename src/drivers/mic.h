#ifndef MIC_H
#define MIC_H

#include <stdint.h>

// Initialise the ADC for microphone sampling
void MIC_INIT();

// Read `num_samples` from the microphone into the provided buffer
void MIC_READ(uint16_t* buffer, uint16_t num_samples);

#endif // MIC_H
