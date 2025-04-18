idf_component_register(
    SRCS "led_driver_pca9685.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_common i2cdev pca9685
)