idf_component_register(SRCS "src/SD_Card.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES  fatfs vfs driver esp_wifi nvs_flash)
