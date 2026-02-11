#pragma once
extern "C" {
#include "Logos.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "include/Commands.h"
#include "nvs_flash.h"
#include "stdio.h"

#include "driver/timer.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
}

#include "Configuration.h"
#include "RingBuffer.hpp"
#include "SD_Card.hpp"
#include "Sprites.h"
#include "Wifi_Auth.hpp"
#include "ssd1306.hpp"
#include "web.hpp"

// #include "esp_vfs_fat.h"
// #include "driver/sdspi_host.h"c
// #include "sdmmc_cmd.h"

#define EVENT_WIFIConneced 1

#define FLAG_HasConnection 0
#define FLAG_ExecutingTask 1
#define FLAG_SDInit 2
#define FLAG_Memory 3
#define FLAG_HasError 4

// #define FLAG_WIFIConnected (1 << 1)
#define CNC_WEB_PORT 8080

static const char* TAG = "CNC";

extern "C" {
struct Steps {
    uint32_t X;
    uint32_t Y;
    uint32_t Z;
    uint32_t E;
};
struct Position {
    float X;
    float Y;
    float Z;
};
}

class CNC {
  private: // drivers
    EventGroupHandle_t Events;
    esp_netif_t*       wifi_netif          = nullptr;
    ssd1306*           Screen              = nullptr;
    uint8_t            WifiConnectAttempts = 0;
    char               IP[16]              = {};
    net::Conn          Connection;
    Wifi_Auth          WebServer;
    SD_Card            sdCard;
    uint16_t           flags = 0;
    Buffio<1024>       buffio;
    struct {
        float X;
        float Y;
        float Z;
    } Position = {};
    struct {
        int32_t X;
        int32_t Y;
        int32_t Z;
        int32_t E;

        int CurrentX_Counter;
        int CurrentY_Counter;
        int CurrentZ_Counter;
        int CurrentE_Counter;
    } Steps = {};

    uint8_t           completion = 0;
    SemaphoreHandle_t RMutex;
    SemaphoreHandle_t WMutex;

  public:
    CNC();
    ~CNC() {};
    void Serve();
    void ExecuteGCode(const char* Command);
    void ExecuteMCode(const char* Command);
    void ExecuteBase(const char* Command);

    static void startAndServe(void* params);

  private:
    void        showLogo();
    void        initWifi();
    const char* getIP();
    bool        DownloadFile(const char* Command);
    void        SetupGPIOs();
    void        SendCommand(const char* command) {
        if(!this->Connection.IsClosed()) {
            Connection.Write(command);
            Connection.Write(Commands::EndOfData);
        }
    };
    void StartTimers() {
        timer_config_t config = {
            .alarm_en    = timer_alarm_t::TIMER_ALARM_EN,
            .counter_en  = timer_start_t::TIMER_PAUSE,
            .intr_type   = timer_intr_mode_t::TIMER_INTR_LEVEL,
            .counter_dir = timer_count_dir_t::TIMER_COUNT_UP,
            .auto_reload = timer_autoreload_t::TIMER_AUTORELOAD_EN,
            .clk_src     = timer_src_clk_t::TIMER_SRC_CLK_APB,
            .divider     = 8};
        timer_init(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, &config);
        timer_set_counter_value(TIMER_GROUP_0, timer_idx_t::TIMER_0, 0);
        timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000);
        timer_enable_intr(TIMER_GROUP_0, TIMER_0);

        timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, AxisTimerISR, this, 0);
    }

  private: // handlers
    static void WifiHandler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
    static void AcceptConnection(void* params);
    static void HandleConnection(void* params);
    static void UpdateScreen(void* params);
    static void ReadMemoryToBuffer(void* arg);
    static bool AxisTimerISR(void* arg);

  private: // Commands
    bool ExecuteTask();

    inline void Command_Identification() {
        char transmitBuffer[100] = {};
        // Name
        sprintf(transmitBuffer, CNC_Responce::MyName, TARGET_MACHINE_NAME);
        SendCommand(transmitBuffer);

        // Type
        sprintf(transmitBuffer, CNC_Responce::MyType, MACHINE_TYPE);
        SendCommand(transmitBuffer);

        // Chip
        sprintf(transmitBuffer, CNC_Responce::Device_chip_Name, CNC_Responce::ChipName);
        SendCommand(transmitBuffer);

        //
        sprintf(transmitBuffer, "Switch_Timeout:%d", Switch_Timeout);
        SendCommand(transmitBuffer);

        // Position
        sprintf(transmitBuffer, CNC_Responce::MyPositionXYZ,
                this->Position.X,
                this->Position.Y,
                this->Position.Z);
        SendCommand(transmitBuffer);

        // Gabs
        sprintf(transmitBuffer, CNC_Responce::MyWidth,
                Width);
        SendCommand(transmitBuffer);
        sprintf(transmitBuffer, CNC_Responce::MyLength,
                Length);
        SendCommand(transmitBuffer);
        sprintf(transmitBuffer, CNC_Responce::MyHeight,
                Heigh);
        SendCommand(transmitBuffer);
    }

    inline void Command_GetState() {
        char buf[35];
        sprintf(buf, CNC_Responce::MyPositionXYZ,
                Position.X,
                Position.Y,
                Position.Z);
        SendCommand(buf);
    }

    void C_G1(const char* command) {}
};

void StartCNCInTask();