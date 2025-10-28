
#ifndef MAIN_DRIVERS_H
#define MAIN_DRIVERS_H
extern "C"{
    #include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "esp_log.h"
#include "esp_netif_types.h"
#include "esp_netif_ip_addr.h" // ip4addr_ntoa
#include "esp_netif.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_sleep.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "driver/timer.h"

#include "driver/i2c_master.h"

#include "esp_wifi.h"
#include "nvs_flash.h"

#include "esp_netif_types.h"
#include "esp_netif_ip_addr.h" // ip4addr_ntoa
#include "esp_netif.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"

#include "string.h"
}


class Driver_I2C
{
private:
    /* data */
public:
    static i2c_master_bus_config_t GetMasterConfig(gpio_num_t scl = GPIO_NUM_21, gpio_num_t sda = GPIO_NUM_22);
    static i2c_device_config_t GetDeviceConfig(uint16_t deviceAdress,
                                               uint16_t sclSpeed = 100000,
                                               bool disableACK = false,
                                               i2c_addr_bit_len_t addrLen = I2C_ADDR_BIT_LEN_7)
    {
        i2c_device_config_t config;
        config.dev_addr_length = addrLen;
        config.device_address = deviceAdress;
        config.scl_speed_hz = sclSpeed;
        config.flags.disable_ack_check = disableACK;
        return config;
    }

    static i2c_master_bus_handle_t InitDevice()
    {
        return i2c_master_bus_handle_t{};
    }
};

namespace Driver_WIFI{
class Wifi_STA
{
private:
    static void wifiEventHandler(void *args, esp_event_base_t base, int32_t eventId, void *eventData)
{
    if ((base == WIFI_EVENT) && (eventId == WIFI_EVENT_STA_START))
    {
    }
}
public:
    static int InitDriverDefault()
    {
        nvs_flash_init();
        esp_netif_init();
        int err;
        err = !esp_event_loop_create_default();
        if (err != NULL) {
            return err;
        }
        esp_netif_create_default_wifi_sta();
        wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
        err = esp_wifi_init(&wifiConfig);
        if (err != NULL){
            return err;
        }
        esp_wifi_set_mode(WIFI_MODE_STA);
        err = esp_wifi_start();
        return NULL;
    }
    static bool Connect_Await(const char *SSID, const char *Password){
    wifi_config_t config = {};
    memcpy((void*)config.sta.ssid,(void*)SSID,32);
    memcpy((void*)config.sta.password,(void*)Password,64);
    esp_wifi_set_config(WIFI_IF_STA,&config);
    esp_wifi_connect();
    return true;
}
    static bool Connect(const char* SSID, const char* Password){
  esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_init();
    esp_event_loop_create_default();
    // esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,WifiHandler,this);
    // esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,WifiHandler,this);
    esp_wifi_init(&config);
  

    wifi_config_t wifi = {};
    strcpy(reinterpret_cast<char*>(wifi.sta.ssid),"Gay_Gang_12143");
    strcpy(reinterpret_cast<char*>(wifi.sta.password),"14886662281");
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA,&wifi);
    esp_wifi_start();
    esp_wifi_connect();

};
};

class Wifi_AP
{
private:
   
public:

};



}

#endif // I2C_CONNECTOR_H