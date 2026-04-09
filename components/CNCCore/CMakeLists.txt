

idf_component_register(SRCS "src/CNC.cpp"
                       INCLUDE_DIRS "." "include/assets"
                       REQUIRES driver esp_driver_gptimer sdmmc SSD1306 Web RingBuffer SD_Card)
