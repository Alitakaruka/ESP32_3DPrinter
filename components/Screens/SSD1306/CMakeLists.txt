idf_component_register(SRCS "src/ssd1306.cpp"
                       INCLUDE_DIRS "." "../"
                       REQUIRES driver esp_driver_gpio esp_driver_i2c esp_wifi nvs_flash)
