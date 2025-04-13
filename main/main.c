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

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <pca9685.h>
#include <string.h>
#include <esp_log.h>

#define ADDR PCA9685_ADDR_BASE
#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define CONFIG_EXAMPLE_I2C_MASTER_SDA 21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL 22
#define CONFIG_EXAMPLE_PWM_FREQ_HZ 1000

static const char *TAG = "pca9685_test";

void pca9685_test(void *pvParameters)
{
    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));

    ESP_ERROR_CHECK(pca9685_init_desc(&dev, ADDR, 0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(pca9685_init(&dev));

    ESP_ERROR_CHECK(pca9685_restart(&dev));

    uint16_t freq;
    ESP_ERROR_CHECK(pca9685_set_pwm_frequency(&dev, CONFIG_EXAMPLE_PWM_FREQ_HZ));
    ESP_ERROR_CHECK(pca9685_get_pwm_frequency(&dev, &freq));

    ESP_LOGI(TAG, "Freq %dHz, real %d", CONFIG_EXAMPLE_PWM_FREQ_HZ, freq);


    while (1) {
        uint16_t led1_val = PCA9685_MAX_PWM_VALUE;

        for (int i = 0; i < 15; i++) {
            ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, i, led1_val));
        }
        
        ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, 0, 0)); // BLUE
        ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, 1, led1_val)); // GREEN
        ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, 2, led1_val)); // RED

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void) {
     // Init i2cdev library
     ESP_ERROR_CHECK(i2cdev_init());

     xTaskCreatePinnedToCore(pca9685_test, TAG, configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL, APP_CPU_NUM);
}
