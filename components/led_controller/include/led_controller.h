#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback function type for LED brightness change events
 * 
 * @param channel LED channel number
 * @param brightness Brightness value (0.0-100.0)
 * @return esp_err_t ESP_OK on success, or an error code
 */
typedef esp_err_t (*led_brightness_callback_t)(uint8_t channel, float brightness);

/**
 * @brief Initialize the LED controller
 * 
 * @param sda_gpio SDA GPIO pin
 * @param scl_gpio SCL GPIO pin
 * @param i2c_addr I2C address of the PCA9685 (usually PCA9685_ADDR_BASE)
 * @param freq_hz PWM frequency in Hz
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t led_controller_init();

/**
 * @brief Set LED brightness for a specific channel
 * 
 * @param channel LED channel number (0-15)
 * @param brightness Brightness percentage (0.0-100.0)
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t led_controller_set_brightness(uint8_t channel, float brightness);

/**
 * @brief Register a callback function for LED brightness changes
 * 
 * @param callback The callback function to register
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t led_controller_register_callback(led_brightness_callback_t callback);

/**
 * @brief Unregister a previously registered callback function
 * 
 * @param callback The callback function to unregister
 * @return esp_err_t ESP_OK on success, or an error code
 */
esp_err_t led_controller_unregister_callback(led_brightness_callback_t callback);

#ifdef __cplusplus
}
#endif
