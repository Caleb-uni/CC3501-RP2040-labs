#include "accell.h"
#include <stdio.h>
#include <math.h>          // Include math library for atan2
#include "hardware/i2c.h"
#include "pico/stdlib.h"

// I2C configuration and conversion constants
#define I2C_INSTANCE i2c0
#define SDA_PIN 16
#define SCL_PIN 17
#define I2C_ADDRESS 0x19
#define START_ADDR 0x28
#define ACC_CONVERSION_RATIO (32768 / 2)  // Conversion ratio for ±2g full scale

// Struct to store accelerometer values
Vector3D values;  // Define the global variable declared as extern in the header

// Function to initialize the LIS3DH accelerometer
void LIS3DH_init() {
    // Initialize I2C
    i2c_init(I2C_INSTANCE, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    printf("I2C Initialized on SDA: %d, SCL: %d\n", SDA_PIN, SCL_PIN);

    // Configure accelerometer
    LIS3DH_writeRegister(0x20, 0x57);  
    LIS3DH_writeRegister(0x23, 0x00);  // CTRL_REG4: Set to ±2g (00 in FS bits)
}

// Getter function for ACC_CONVERSION_RATIO
float getAccConversionRatio() {
    return ACC_CONVERSION_RATIO;
}

// Write to an accelerometer register
int LIS3DH_writeRegister(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    if (i2c_write_blocking(I2C_INSTANCE, I2C_ADDRESS, data, 2, false) != 2) {
        printf("Failed to write to register 0x%X\n", reg);
        return -1;
    }
    return 0;
}

// Read from an accelerometer register
int LIS3DH_readRegister(uint8_t reg, uint8_t *value) {
    if (i2c_write_blocking(I2C_INSTANCE, I2C_ADDRESS, &reg, 1, true) != 1) {
        printf("Failed to write register address for read (0x%X)!\n", reg);
        return -1;
    }
    if (i2c_read_blocking(I2C_INSTANCE, I2C_ADDRESS, value, 1, false) != 1) {
        printf("Failed to read register value from 0x%X!\n", reg);
        return -1;
    }
    return 0;
}

// Read raw acceleration values from the accelerometer
int LIS3DH_readAccelValues() {
    uint8_t reg = START_ADDR | 0x80;  // Set MSB for multi-byte read
    uint8_t raw_data[6];

    if (i2c_write_blocking(I2C_INSTANCE, I2C_ADDRESS, &reg, 1, true) != 1) {
        printf("Failed to set register address for acceleration read!\n");
        return -1;
    }
    if (i2c_read_blocking(I2C_INSTANCE, I2C_ADDRESS, raw_data, 6, false) != 6) {
        printf("Failed to read acceleration data!\n");
        return -1;
    }

    // Combine the raw data into 16-bit values for x, y, z
    values.x = (int16_t)(raw_data[0] | (raw_data[1] << 8));
    values.y = (int16_t)(raw_data[2] | (raw_data[3] << 8));
    values.z = (int16_t)(raw_data[4] | (raw_data[5] << 8));

    return 0;
}

// Function to calculate X tilt (roll) in degrees
float getXTilt() {
    // Calculate roll in degrees
    return atan2((float)values.y, (float)values.z) * (180.0 / M_PI);
}

// Function to calculate Y tilt (pitch) in degrees
float getYTilt() {
    // Calculate pitch in degrees
    return atan2((float)values.x, (float)values.z) * (180.0 / M_PI);
}

// Get formatted string of current accelerometer readings
std::string LIS3DH_getCurrentReadings() {
    float x_frac = (float)values.x / ACC_CONVERSION_RATIO;
    float y_frac = (float)values.y / ACC_CONVERSION_RATIO;
    float z_frac = (float)values.z / ACC_CONVERSION_RATIO;

    char buf[50];
    snprintf(buf, sizeof(buf), "(%.2fg, %.2fg, %.2fg)", x_frac, y_frac, z_frac);
    return std::string(buf);
}

// Update accelerometer readings
int LIS3DH_updateReadings() {
    return LIS3DH_readAccelValues();
}