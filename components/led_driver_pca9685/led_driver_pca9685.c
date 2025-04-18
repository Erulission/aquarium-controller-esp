#include "led_driver_pca9685.h"
#include <string.h>
#include <esp_log.h>

#define NUM_CHANNELS 16

static const char *TAG = "led_driver_pca9685";

static struct {
    i2c_dev_t dev;
    bool initialized;
    bool commonAnode; // true for common anode, false for common cathode
} pca9685_ctx = {
    .initialized = false,
    .commonAnode = false
};

static uint16_t get_pwm_value(float brightness) {
    // Convert brightness percentage to PWM value
    uint16_t pwm_value = (uint16_t)((brightness / 100.0f) * PCA9685_MAX_PWM_VALUE);
    
    // Adjust for common anode or common cathode
    if (pca9685_ctx.commonAnode) {
        pwm_value = PCA9685_MAX_PWM_VALUE - pwm_value;
    }

    return pwm_value;
}

esp_err_t led_driver_pca9685_init(uint8_t sda_gpio, uint8_t scl_gpio, uint8_t i2c_addr, uint16_t freq_hz, const bool commonAnode) {
    esp_err_t ret;
    
    if (pca9685_ctx.initialized) {
        ESP_LOGW(TAG, "PCA9685 driver already initialized");
        return ESP_OK;
    }

    // Initialize the context
    memset(&pca9685_ctx.dev, 0, sizeof(i2c_dev_t));

    // Initialize the PCA9685
    ret = pca9685_init_desc(&pca9685_ctx.dev, i2c_addr, 0, sda_gpio, scl_gpio);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize PCA9685 descriptor: %d", ret);
        return ret;
    }

    ret = pca9685_init(&pca9685_ctx.dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize PCA9685: %d", ret);
        return ret;
    }

    ret = pca9685_restart(&pca9685_ctx.dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to restart PCA9685: %d", ret);
        return ret;
    }

    ret = pca9685_set_pwm_frequency(&pca9685_ctx.dev, freq_hz);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set PWM frequency: %d", ret);
        return ret;
    }

    uint16_t real_freq;
    ret = pca9685_get_pwm_frequency(&pca9685_ctx.dev, &real_freq);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get PWM frequency: %d", ret);
        return ret;
    }

    pca9685_ctx.commonAnode = commonAnode;

    ESP_LOGI(TAG, "PCA9685 initialized. Requested freq: %d Hz, actual freq: %d Hz", freq_hz, real_freq);
    
    // All channels off initially
    for (int i = 0; i < NUM_CHANNELS; i++) {
        ret = pca9685_set_pwm_value(&pca9685_ctx.dev, i, get_pwm_value(0));
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set channel %d to 0: %d", i, ret);
            return ret;
        }
    }

    pca9685_ctx.initialized = true;
    return ESP_OK;
}

esp_err_t led_driver_pca9685_set_brightness(uint8_t channel, float brightness) {
    esp_err_t ret;
    
    if (!pca9685_ctx.initialized) {
        ESP_LOGE(TAG, "PCA9685 driver not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (channel >= NUM_CHANNELS) {
        ESP_LOGE(TAG, "Invalid channel number: %d", channel);
        return ESP_ERR_INVALID_ARG;
    }

    // Clamp brightness to valid range
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 100.0f) brightness = 100.0f;

    // Convert brightness percentage to PWM value
    uint16_t pwm_value = get_pwm_value(brightness);
    
    // Set the PWM value
    ret = pca9685_set_pwm_value(&pca9685_ctx.dev, channel, pwm_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set brightness for channel %d: %d", channel, ret);
        return ret;
    }

    ESP_LOGD(TAG, "Set channel %d brightness to %.1f%% (PWM value: %d)", 
             channel, brightness, pwm_value);

    return ESP_OK;
}

esp_err_t led_driver_pca9685_get_frequency(uint16_t *freq_hz) {
    if (!pca9685_ctx.initialized) {
        ESP_LOGE(TAG, "PCA9685 driver not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (!freq_hz) {
        ESP_LOGE(TAG, "Invalid frequency pointer");
        return ESP_ERR_INVALID_ARG;
    }

    return pca9685_get_pwm_frequency(&pca9685_ctx.dev, freq_hz);
}

bool led_driver_pca9685_is_initialized(void) {
    return pca9685_ctx.initialized;
}