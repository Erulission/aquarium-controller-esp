#include "led_controller.h"
#include "led_driver_pca9685.h"
#include <string.h>
#include <esp_log.h>

#define MAX_CALLBACKS 5
#define NUM_CHANNELS 16

static const char *TAG = "led_controller";

static struct {
    bool initialized;
    led_brightness_callback_t callbacks[MAX_CALLBACKS];
    uint8_t callback_count;
} led_controller_ctx = {
    .initialized = false,
    .callback_count = 0
};

esp_err_t led_controller_init() {
    esp_err_t ret;
    
    if (led_controller_ctx.initialized) {
        ESP_LOGW(TAG, "LED controller already initialized");
        return ESP_OK;
    }

    // Initialize the context
    memset(led_controller_ctx.callbacks, 0, sizeof(led_controller_ctx.callbacks));
    led_controller_ctx.callback_count = 0;

    led_controller_ctx.initialized = true;
    return ESP_OK;
}

esp_err_t led_controller_set_brightness(uint8_t channel, float brightness) {
    esp_err_t ret;
    
    if (!led_controller_ctx.initialized) {
        ESP_LOGE(TAG, "LED controller not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (channel >= NUM_CHANNELS) {
        ESP_LOGE(TAG, "Invalid channel number: %d", channel);
        return ESP_ERR_INVALID_ARG;
    }

    // Clamp brightness to valid range
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 100.0f) brightness = 100.0f;

    // // Set the brightness using the LED driver
    // ret = led_driver_pca9685_set_brightness(channel, brightness);
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to set brightness for channel %d: %d", channel, ret);
    //     return ret;
    // }

    ESP_LOGD(TAG, "Set channel %d brightness to %.1f%%", channel, brightness);

    // Call all registered callbacks
    for (int i = 0; i < led_controller_ctx.callback_count; i++) {
        if (led_controller_ctx.callbacks[i]) {
            ret = led_controller_ctx.callbacks[i](channel, brightness);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Callback %d returned error: %d", i, ret);
                // Continue with other callbacks even if one fails
            }
        }
    }

    return ESP_OK;
}

esp_err_t led_controller_register_callback(led_brightness_callback_t callback) {
    if (!callback) {
        ESP_LOGE(TAG, "Cannot register NULL callback");
        return ESP_ERR_INVALID_ARG;
    }

    if (led_controller_ctx.callback_count >= MAX_CALLBACKS) {
        ESP_LOGE(TAG, "Maximum number of callbacks reached");
        return ESP_ERR_NO_MEM;
    }

    // Check if callback is already registered
    for (int i = 0; i < led_controller_ctx.callback_count; i++) {
        if (led_controller_ctx.callbacks[i] == callback) {
            ESP_LOGW(TAG, "Callback already registered");
            return ESP_OK;
        }
    }

    // Add the new callback
    led_controller_ctx.callbacks[led_controller_ctx.callback_count] = callback;
    led_controller_ctx.callback_count++;

    ESP_LOGI(TAG, "Callback registered. Total callbacks: %d", led_controller_ctx.callback_count);
    return ESP_OK;
}

esp_err_t led_controller_unregister_callback(led_brightness_callback_t callback) {
    if (!callback) {
        ESP_LOGE(TAG, "Cannot unregister NULL callback");
        return ESP_ERR_INVALID_ARG;
    }

    bool found = false;
    int i;

    // Find the callback
    for (i = 0; i < led_controller_ctx.callback_count; i++) {
        if (led_controller_ctx.callbacks[i] == callback) {
            found = true;
            break;
        }
    }

    if (!found) {
        ESP_LOGW(TAG, "Callback not found");
        return ESP_ERR_NOT_FOUND;
    }

    // Shift remaining callbacks
    for (; i < led_controller_ctx.callback_count - 1; i++) {
        led_controller_ctx.callbacks[i] = led_controller_ctx.callbacks[i + 1];
    }

    led_controller_ctx.callback_count--;
    led_controller_ctx.callbacks[led_controller_ctx.callback_count] = NULL;

    ESP_LOGI(TAG, "Callback unregistered. Total callbacks: %d", led_controller_ctx.callback_count);
    return ESP_OK;
}