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
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "stdarg.h"

#include "driver/gptimer.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
}

#include "Configuration.h"
#include "ringBuffer.hpp"
#include "SD_Card.hpp"
#include "Sprites.h"
#include "Wifi_Auth.hpp"
#include "ssd1306.hpp"
#include "web.hpp"
#include "Sheduler.hpp"
#include "TransmitBuffer.hpp"

#include <atomic>

// #include "esp_vfs_fat.h"
// #include "driver/sdspi_host.h"c
// #include "sdmmc_cmd.h"

#define EVENT_WIFIConneced 1
#define EVENT_UI_UPDATE 4

#define FLAG_HasConnection (1 << 0)
#define FLAG_ExecutingTask (1 << 1)
#define FLAG_SDInit (1 << 2)
#define FLAG_Memory (1 << 3)
#define FLAG_AsixCharge (1 << 4)
#define FLAG_IsAbsalute (1 << 5)
#define FLAG_ExtruderIsAbsalute (1 << 6)

// #define FLAG_WIFIConnected (1 << 1)
#define CNC_WEB_PORT 8080


#define WDTimerUs 100000
#define WDTicksInSecond (1000000 / WDTimerUs)

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
    std::atomic<uint16_t>           flags = 0;
    Buffio<BufferSize>       buffio;
    TransmitBuffer<2048> transmitBuffer;



    struct {
        float X = 0.0;
        float Y = 0.0;
        float Z = 0.0;
    } CurrentPosition = {};
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
    float speed = 0;

    uint8_t           completion = 0;
    // SemaphoreHandle_t RMutex;
    // SemaphoreHandle_t WMutex;


    gptimer_handle_t axis_timer = nullptr;
    esp_timer_handle_t wathcDogTimer = nullptr;

public:
    Sheduler sheduler;

  public:
    CNC();
    ~CNC() {};
    static void ReadCommands(void* args);
    static void ExecuteCommands(void* args);

    float GetSpeed(){
        return this->speed;
    }

    void SetSpeed(float speed){
        if(speed > 200.0f){
            speed = 200;
        }
        if(speed < 1){
            speed = 1;
        }
        this->speed = speed;
    }

    void ChargeUI(){
        xEventGroupSetBits(this->Events,EVENT_UI_UPDATE);
    }

    bool CheckFlag(uint16_t flag){
        return (this->flags.load() & flag);
    }
  private:

    void ReadGCode(const char* Command);
    void ReadMCode(const char* Command);
    void ExecuteBase(const char* Command);

    void        showLogo();
    void        initWifi();
    const char* getIP();
    bool        DownloadFile(const char* Command);
    void        SetupGPIOs();

    void SendCommand(const char* command,...) {
        if(this->Connection.IsClosed()) {
            return;
        }
        va_list args;
        va_start(args, command);
        this->transmitBuffer.Write(command,args);
        this->transmitBuffer.WriteN(Commands::EndOfData,sizeof(Commands::EndOfData));
        this->SendBuffer();
    };

    void WriteToBuffer(const char* command,...){
        va_list args;
        va_start(args, command);
        this->transmitBuffer.Write(command,args);
        va_end(args);
    }

    void WriteCommandToBuffer(const char* command,...){
        va_list args;
        va_start(args, command);
        this->transmitBuffer.Write(command,args);
        this->transmitBuffer.Write(Commands::EndOfData);
        va_end(args);
    }

    void SendBuffer(){
        if(this->Connection.IsClosed()) {
            this->transmitBuffer.Clear();
            return;
        }

        if(this->flags.fetch_and(~FLAG_AsixCharge) & FLAG_AsixCharge) {
            this->WriteCommandToBuffer(CNC_Responce::MyPositionXYZ, this->CurrentPosition.X, this->CurrentPosition.Y,
                                       this->CurrentPosition.Z);
        }

        Connection.Write(this->transmitBuffer.GetResult());

        if (strcmp(this->transmitBuffer.GetResult(),"ok\r\n")){
            ESP_LOGI("","Buffer:%s",this->transmitBuffer.GetResult());
        }

        this->transmitBuffer.Clear();
    }

    void StartTimers();

  private: // handlers
    static void WifiHandler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
    static void AcceptConnection(void* params);
    static void HandleConnection(void* params);
    static void UpdateScreen(void* params);
    static bool IRAM_ATTR AxisTimerISR(gptimer_handle_t timer, const gptimer_alarm_event_data_t*, void* arg);
    static void  WatchDogTimerHandler(void* arg);

  private: // Commands
    bool ExecuteTask();

    inline void Command_Identification() {
        this->WriteCommandToBuffer(CNC_Responce::MyName, TARGET_MACHINE_NAME);
        this->WriteCommandToBuffer(CNC_Responce::MyType, MACHINE_TYPE);
        this->WriteCommandToBuffer(CNC_Responce::Device_chip_Name, CNC_Responce::ChipName);
        this->WriteCommandToBuffer(CNC_Responce::Switch_Timeout_C, 1);
        this->WriteCommandToBuffer(CNC_Responce::MyPositionXYZ,  this->CurrentPosition.X,
                this->CurrentPosition.Y,
                this->CurrentPosition.Z);
        this->WriteCommandToBuffer(CNC_Responce::MyWidth, Width);
        this->WriteCommandToBuffer(CNC_Responce::MyLength, Length);
        this->WriteCommandToBuffer(CNC_Responce::MyHeight, Heigh);
    }

    inline void Command_GetState() {
        char buf[35];
        sprintf(buf, CNC_Responce::MyPositionXYZ,
                CurrentPosition.X,
                CurrentPosition.Y,
                CurrentPosition.Z);
        SendCommand(buf);
    }

    void C_G1(const char* command) {}
    void Move(const float X,const float Y,const float Z,const float E,const float F){
        this->flags.fetch_or(FLAG_AsixCharge);
    }

    // #pragma region Logs
public:
    enum LogLevel{
        Error,
        Warning,
        Information,
        Success
    };

    // #define Error "Error:"
    // #define Warning "Warning:"
    // #define Information "Information:"
    // #define Success "Success:"

    void WriteLog(LogLevel level, const char* text, ...) {
        char LogPrefix[20];
        switch(level) {
        case Success:
            strcpy(LogPrefix, "Success:");
            break;
        case Error:
            strcpy(LogPrefix, "Error:");
            break;
        case Warning:
            strcpy(LogPrefix, "Warning:");
            break;
        case Information:
            strcpy(LogPrefix, "Information:");
            break;
        }
        this->WriteToBuffer(LogPrefix);

        va_list args;
        va_start(args,text);
        this->transmitBuffer.Write(text,args);
        this->transmitBuffer.Write(Commands::EndOfData);
        va_end(args);
    }

// #pragma endregion

};

void StartCNCInTask();
