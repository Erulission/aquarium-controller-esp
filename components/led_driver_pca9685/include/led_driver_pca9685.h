#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include <i2cdev.h>
#include <pca9685.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the PCA9685 LED driver
 * 
 * @param sda_gpio SDA GPIO pin
 * @param scl_gpio SCL GPIO pin
 * @param i2c_addr I2C address of the PCA9685 (usually PCA9685_ADDR_BASE)
 * @param freq_hz PWM frequency in Hz
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t led_driver_pca9685_init(uint8_t sda_gpio, uint8_t scl_gpio, uint8_t i2c_addr, uint16_t freq_hz, const bool commonAnode);

/**
 * @brief Set LED brightness for a specific channel
 * 
 * @param channel LED channel number (0-15)
 * @param brightness Brightness percentage (0.0-100.0)
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t led_driver_pca9685_set_brightness(uint8_t channel, float brightness);

/**
 * @brief Get the actual PWM frequency that was set
 * 
 * @param freq_hz Pointer to store the frequency value
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t led_driver_pca9685_get_frequency(uint16_t *freq_hz);

/**
 * @brief Check if the driver has been initialized
 * 
 * @return bool true if initialized, false otherwise
 */
bool led_driver_pca9685_is_initialized(void);

#ifdef __cplusplus
}
#endif
