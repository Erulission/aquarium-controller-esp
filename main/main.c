/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "freertos/event_groups.h"
#include <string.h>
#include <esp_log.h>
#include "led_controller.h"
#include "led_driver_pca9685.h"

#define ADDR PCA9685_ADDR_BASE
#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define CONFIG_EXAMPLE_I2C_MASTER_SDA 21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL 22
#define CONFIG_EXAMPLE_PWM_FREQ_HZ 1000

static const char *TAG = "led_example";

static void led_control_task(void* pvParameters)
{
    // Wait for initialization to complete
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    ESP_LOGI(TAG, "LED control task started");

    // Demonstrate setting different brightness levels
    while (1) {
        // Fade all channels up
        for (int i = 0; i < 15; i++) {
            led_controller_set_brightness(i, 0);
        }
        
        // Special RGB pattern
        ESP_LOGI(TAG, "Setting RGB pattern");
        led_controller_set_brightness(0, 100);
        led_controller_set_brightness(3, 100); 
        led_controller_set_brightness(6, 100);
        
        vTaskDelay(pdMS_TO_TICKS(3000));
        
        // Turn all channels off
        ESP_LOGI(TAG, "Turning all channels off");
        for (int i = 0; i < 15; i++) {
            led_controller_set_brightness(i, 0);
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Color 2");
        led_controller_set_brightness(1, 100);
        led_controller_set_brightness(4, 100); 
        led_controller_set_brightness(7, 100);

        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Color 3");
        for (int i = 0; i < 15; i++) {
            led_controller_set_brightness(i, 0);
        }

        led_controller_set_brightness(2, 100);
        led_controller_set_brightness(5, 100); 
        led_controller_set_brightness(8, 100);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void) {
    // Initialize i2cdev library
    ESP_ERROR_CHECK(i2cdev_init());
    
    // Initialize the LED controller
    ESP_LOGI(TAG, "Initializing LED controller");
    ESP_ERROR_CHECK(led_controller_init());

    led_driver_pca9685_init(
        CONFIG_EXAMPLE_I2C_MASTER_SDA,
        CONFIG_EXAMPLE_I2C_MASTER_SCL,
        ADDR,
        CONFIG_EXAMPLE_PWM_FREQ_HZ, true
    );
        
    // Create the LED control task
    ESP_LOGI(TAG, "Creating LED control task");
    xTaskCreatePinnedToCore(
        led_control_task,
        "led_control",
        configMINIMAL_STACK_SIZE * 3,
        NULL,
        5,
        NULL,
        APP_CPU_NUM
    );
}
