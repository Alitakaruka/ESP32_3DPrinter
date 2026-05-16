extern "C"
{
#include <stdio.h>
#include <esp_log.h>
}
#include "CNC.hpp"



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

#include "Configuration.h"


#define STEPS 2000
#define STEP_DELAY_US 800

void Init()
{
    StartCNCInTask();
}

void step_motor(int steps, int direction)
{
    gpio_set_level(X_DIR_PORT, direction);

    for (int i = 0; i < steps; i++)
    {
        gpio_set_level(X_STEP_PORT, 1);
        ets_delay_us(100);

        gpio_set_level(X_STEP_PORT, 0);
         ets_delay_us(100);
    }
    printf("Done\n");
}



extern "C"
{
    void app_main(void)
    {
        // esp_log_level_set("*", ESP_LOG_NONE);     // полностью убрать всё
        // esp_log_level_set("wifi", ESP_LOG_NONE);  // конкретно Wi-Fi
        // esp_log_level_set("httpd", ESP_LOG_NONE); // HTTP-сервер
        // nvs_flash_erase();


        // Init();
        // vTaskDelete(NULL);

        gpio_config_t io_conf;

        io_conf.pin_bit_mask = (1ULL << X_STEP_PORT) | (1ULL << X_DIR_PORT) | (1ULL << STEPPERS_ENABLE_PORT);

        io_conf.pull_down_en = gpio_pulldown_t::GPIO_PULLDOWN_ENABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;

    gpio_config(&io_conf);

    gpio_set_level(STEPPERS_ENABLE_PORT,0);
    while (1)
    {
        // вперед
        step_motor(STEPS, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // назад
        step_motor(STEPS, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }



    }
}
