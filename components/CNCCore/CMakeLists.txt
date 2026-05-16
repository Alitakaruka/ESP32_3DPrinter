

idf_component_register(SRCS "src/CNC.cpp" "src/GCode.cpp" "src/MCode.cpp"
                       INCLUDE_DIRS "." "include/assets"
                       REQUIRES  TransmitBuffer driver esp_timer esp_driver_gptimer sdmmc SSD1306 Web RingBuffer SD_Card)
