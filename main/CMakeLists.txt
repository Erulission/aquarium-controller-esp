idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES spi_flash pca9685 led_controller
                    INCLUDE_DIRS "."
                    REQUIRES unity)
