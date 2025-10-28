extern "C"
{
#include <stdio.h>
#include <esp_log.h>
}
#include "CNC.hpp"


void Init()
{
    StartCNCInTask();
}

extern "C"
{
    void app_main(void)
    {
        // esp_log_level_set("*", ESP_LOG_NONE);     // полностью убрать всё
        // esp_log_level_set("wifi", ESP_LOG_NONE);  // конкретно Wi-Fi
        // esp_log_level_set("httpd", ESP_LOG_NONE); // HTTP-сервер
        // nvs_flash_erase();
        Init();
        vTaskDelete(NULL);
    }
}