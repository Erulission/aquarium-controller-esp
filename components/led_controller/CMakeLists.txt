idf_component_register(
    SRCS "led_controller.c"
    INCLUDE_DIRS "include"
    REQUIRES i2cdev led_driver_pca9685
)