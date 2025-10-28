idf_component_register(SRCS "main.cpp" "main.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_wifi nvs_flash MainDrivers SSD1306 CNCCore)