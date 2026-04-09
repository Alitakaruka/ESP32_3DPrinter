idf_component_register(SRCS "src/MainDrivers.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_driver_gpio esp_driver_i2c esp_driver_spi esp_driver_uart esp_wifi nvs_flash)
