#pragma once
extern "C"
{
#include "stdio.h"
#include "Logos.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "include/Commands.h"
#include "esp_log.h"
#include "esp_netif_types.h"
#include "esp_netif_ip_addr.h" // ip4addr_ntoa
#include "esp_netif.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"

}


#include "Wifi_Auth.hpp"
#include "web.hpp"
#include "ssd1306.hpp"
#include "RingBuffer.hpp"
#include "Sprites.h"
#include "SD_Card.hpp"

// #include "esp_vfs_fat.h"
// #include "driver/sdspi_host.h"
// #include "sdmmc_cmd.h"

#define EVENT_WIFIConneced 1



#define FLAG_HasConnection 0
#define FLAG_IsWorking     1
#define FLAG_SDInit        2
#define FLAG_Memory        3
#define FLAG_HasError      4
   
// #define FLAG_WIFIConnected (1 << 1)
#define CNC_WEB_PORT 8080


static const char *TAG = "CNC";

extern "C"
{
    struct Steps
    {
        uint32_t X;
        uint32_t Y;
        uint32_t Z;
        uint32_t E;
    };
    struct Position
    {
        float X;
        float Y;
        float Z;
    };
    
}

class CNC
{
private: //drivers
    EventGroupHandle_t Events;
    esp_netif_t* wifi_netif = nullptr;
    ssd1306 *Screen = nullptr;
    uint8_t WifiConnectAttempts =0;
    char IP[16] = {};
    net::Conn Connection;
    Wifi_Auth WebServer;
    SD_Card sdCard;
    FileWriter Writer;

private:
    uint16_t flags = 0;
    struct Steps Steps = {};
    RingBuffer<char[100]> Buffer;
    Position position = {};
    uint8_t completion = 0;
    SemaphoreHandle_t xMutex;
public:
    CNC();
    ~CNC() {};
    void Serve();
    void ExecuteGCode();
    static void startAndServe(void *params);
private:
    void showLogo();
    void initWifi();
    const char *getIP();
    void DownloadFile();

private: //handlers 
    static void WifiHandler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data);    
                          
    static void AcceptConnection(void* params);
    static void HandleConnection(void* params);
    static void UpdateSkreen(void* params);
};

void StartCNCInTask();