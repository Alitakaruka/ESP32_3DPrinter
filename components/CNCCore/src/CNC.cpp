#include "CNC.hpp"

void CNC::startAndServe(void* params) {
    CNC mashine;
    ESP_LOGI("Init", "Start init mashine!\n");
    mashine.Serve();
}

CNC::CNC() {
    this->Screen = new ssd1306();
    this->Events = xEventGroupCreate();

    esp_err_t err      = Screen->Init();
    if(err != ESP_OK) {
        ESP_LOGE("LCD", " display init error!");
    }
    this->Screen->setRotation(true);
    showLogo();
    SetupGPIOs();
    xTaskCreatePinnedToCore(UpdateScreen,"Screen update",4000,this,1,NULL,0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    this->ChargeUI();


    RMutex       = xSemaphoreCreateMutex();
    WMutex       = xSemaphoreCreateMutex();
    if(RMutex == NULL || WMutex == NULL) {
        ESP_ERROR_CHECK(1);
    }


    StartTimers();
    initWifi();
    if(sdCard.TryConnectSD() != ESP_OK) {
        ESP_LOGE("SPI", "Error init SPI");
    } else {
        this->flags |= (1 << FLAG_SDInit);
    }
    EventBits_t bits = xEventGroupWaitBits(this->Events, EVENT_WIFIConneced, true, false, portMAX_DELAY);
    // accept tcpConnections
    xTaskCreatePinnedToCore(AcceptConnection, "ConnectionAccept", 2048, this, 1, NULL, 0);
}

void CNC::Serve() {
    if(xPortGetCoreID() == 1) {
        while(1) {
            if(!this->buffio.isEmpty()) {
                char CurrentCommand[512] = {};
                this->buffio.ReadLine(CurrentCommand, sizeof(CurrentCommand), Commands::EndOfData);
                char ch = toupper(CurrentCommand[0]);
                switch(ch) {
                case 'G':
                    ExecuteGCode(CurrentCommand);
                    break;
                case 'M':
                    ExecuteMCode(CurrentCommand);
                    break;
                default:
                    ExecuteBase(CurrentCommand);
                    break;
                }
                SendCommand(CNC_Responce::CommandACK);
            }
        }
    }else{
        xTaskCreatePinnedToCore(ReadMemoryToBuffer, "", 2048, this, 1, NULL, 0);
        while(1) {
            if(!this->buffio.isEmpty()) {
                char CurrentCommand[512] = {};
                this->buffio.ReadLine(CurrentCommand, sizeof(CurrentCommand), Commands::EndOfData);
                char ch = toupper(CurrentCommand[0]);
                switch(ch) {
                case 'G':
                    ExecuteGCode(CurrentCommand);
                    break;
                case 'M':
                    ExecuteMCode(CurrentCommand);
                    break;
                default:
                    ExecuteBase(CurrentCommand);
                    break;
                }
                SendCommand(CNC_Responce::CommandACK);
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void CNC::ExecuteBase(const char* Command) {

    if(!strcmp(Command, Commands::Identification)) {
        ESP_LOGI("","Identification command!");
        Command_Identification();
    } else if(!strcmp(Command, Commands::SYNC)) {
        ESP_LOGI("","Sync command!");
        char buf[50] = {};
        sprintf(buf, CNC_Responce::MyBufferLen, BufferSize);
        this->SendCommand(buf);
        memset(&this->buffio, 0, sizeof(this->buffio));
    }
    // else if(!strstr(Command, Commands::StartOfTransmision)) {
    //     ESP_LOGI("","Identification command!");//todo
    //     DownloadFile(Command);
    // }
}

void CNC::ReadMemoryToBuffer(void* arg) {
    while(1) {
        vTaskDelay(1000);
    }
    vTaskDelete(NULL);
}

void CNC::ExecuteGCode(const char* Command) {
    {
        using namespace GCode;

        if(strncmp(Command, G0, strlen(G0))) {
            //
        } else if(strncmp(Command, G1, strlen(G1))) {

        } else if(strncmp(Command, G4, strlen(G4))) {
            char sleepTime[10];
            Command += strlen(G4);
            strncpy(sleepTime, Command, sizeof(sleepTime) - 1);
            vTaskDelay(pdMS_TO_TICKS(atoi(sleepTime)));
        } else if(strncmp(Command, G10, strlen(G10))) {

        } else if(strncmp(Command, G11, strlen(G11))) {

        } else if(strncmp(Command, G28, strlen(G28))) {

        } else if(strncmp(Command, G90, strlen(G90))) {

        } else if(strncmp(Command, G91, strlen(G91))) {

        } else if(strncmp(Command, G92, strlen(G92))) {

        } else if(strncmp(Command, G20, strlen(G20))) {

        } else if(strncmp(Command, G21, strlen(G21))) {

        } else if(strncmp(Command, G29, strlen(G29))) {
            ////////////////////////////////////////////
        } else if(strncmp(Command, G1, strlen(G0))) {

        } else if(strncmp(Command, G1, strlen(G0))) {

        } else if(strncmp(Command, G1, strlen(G0))) {

        } else if(strncmp(Command, G1, strlen(G0))) {
        }
    }
}

void CNC::ExecuteMCode(const char* Command) {
    {

        using namespace MCode;
        if(!strcmp(Command, StartTask)) {
            this->flags |= (1 << FLAG_ExecutingTask);
            xTaskCreatePinnedToCore(ReadMemoryToBuffer, "", 2048, this, 1, NULL, 0);
        } else if(!strcmp(Command, GetState)) {
            Command_GetState();
        } else if(!strcmp(Command, SelectFile)) {
            char filename[20] = {};
            strncpy(filename, Command + sizeof(SelectFile), 19);
            this->sdCard.SelectFile(Command);
        }
    }
}

bool CNC::DownloadFile(const char* Command) {

    char   fileName[20] = {};
    char   intBuf[11]   = {};
    size_t fileBytes    = 0;

    Command += sizeof(Commands::StartOfTransmision);

    if(!strncmp(Command, Commands::FILE_NAME, sizeof(Commands::FILE_NAME))) {

        size_t bytes = strcspn(Command, "\n");
        if(bytes > sizeof(fileName) - 1) {
            return false;
        }
        strncpy(fileName, Command, bytes); // Copy name
        Command += bytes;
    }

    if(!strncmp(Command, Commands::FILE_NAME, sizeof(Commands::FILE_NAME))) {
        Command += sizeof(Commands::FILE_NAME);
        size_t bytes = strcspn(Command, "\n");
        if(bytes > sizeof(intBuf) - 1) {
            return false;
        }
        strncpy(intBuf, Command, bytes);
        fileBytes = atoi(intBuf);
    }
    sdCard.CreateFile(fileName);

    char bytesBuffer[512] = {};

    while(fileBytes) {
        if(buffio.isEmpty()) {
            char sendbuf[25] = {};
            sprintf(sendbuf, Commands::GET_FILE_FATA, 512);
            SendCommand(sendbuf);
        }
        auto read = buffio.ReadBytes(bytesBuffer, sizeof(bytesBuffer) - 1);

        if(read) {
            sdCard.AppendToFile(fileName, bytesBuffer);
        }
        fileBytes -= sizeof(bytesBuffer) - 1;
    }
    return true;
}

void CNC::showLogo() {
    if(this->Screen == nullptr) {
        return;
    }
    this->Screen->DrawBitmap(reinterpret_cast<const char*>(Images::MainLogo),
                             sizeof(Images::MainLogo));
    this->Screen->SwapBuffers();
}

void StartCNCInTask() {
    xTaskCreatePinnedToCore(CNC::startAndServe,"", 8192*4, NULL, 1, NULL, 0);
    // xTaskCreatePinnedToCore(CNC::startAndServe,"", 8192*4, NULL, 1, NULL, 1);
    vTaskDelete(NULL);
}

void CNC::initWifi() {
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, WifiHandler, this);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, WifiHandler, this);

    esp_wifi_init(&config);

    wifi_config_t current_config;
    esp_err_t     err = esp_wifi_get_config(WIFI_IF_STA, &current_config);

    esp_netif_create_default_wifi_ap();
    this->wifi_netif = esp_netif_create_default_wifi_sta();
    if(err == ESP_OK &&
       strlen((char*)current_config.sta.ssid) > 0) {
        esp_wifi_set_mode(WIFI_MODE_STA);
        esp_wifi_start();
        esp_wifi_connect();
        return;
    } else {
        WebServer.StartWebServer("CNC Setup", "");
    }
}

void CNC::WifiHandler(void* arg, esp_event_base_t event_base,
                      int32_t event_id, void* event_data) {
    CNC* self = static_cast<CNC*>(arg);

    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Reconnecting...");
        self->WifiConnectAttempts++;
        xEventGroupClearBits(self->Events, EVENT_WIFIConneced);
        if(self->WifiConnectAttempts >= 10) {
            ESP_LOGW(TAG, "WIFI cant connected! Start auth web server!");
            memset(self->IP, 0, sizeof(self->IP));
            self->WebServer.StartWebServer("CNC Setup", "");
        } else {
            esp_wifi_connect();
        }
    } else if(event_base == IP_EVENT) {
        if(event_id == IP_EVENT_STA_GOT_IP) {
            ESP_LOGW(TAG, "Wifi connected!");
            self->getIP();
            xEventGroupSetBits(self->Events, EVENT_WIFIConneced);
            self->WifiConnectAttempts = 0;
            self->WebServer.StopWebServer();
            self->ChargeUI();
        }
    }
}

const char* CNC::getIP() {
    esp_netif_ip_info_t ipInfo;
    if(esp_netif_get_ip_info(this->wifi_netif, &ipInfo) == ESP_OK) {
        inet_ntoa_r(ipInfo, this->IP, sizeof(this->IP));
    }
    return this->IP;
}

void CNC::AcceptConnection(void* params) {
    CNC* self     = static_cast<CNC*>(params);
    auto listener = net::ListenPort("tcp", CNC_WEB_PORT);
    while(1) {
        net::Conn con = listener.AcceptConn();
        if(con.isValidConnection()) {
            self->Connection = con;
            if(self->flags & (1 << FLAG_HasConnection)) {
                ESP_LOGE("Connection", "FLAG_HasConnection is SET!");
                con.Close();
                continue;
            }
            self->flags |= (1 << FLAG_HasConnection);
            xTaskCreatePinnedToCore(HandleConnection, "Handler", 2048, self, 2, NULL, 0);
        }
    }
}

void CNC::HandleConnection(void* params) {
    auto self = static_cast<CNC*>(params);
    printf("Pinter HandleConnection:%p",self);
    char buffer[512];
    ESP_LOGW("Con", "Start listen!");

    while(1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = self->Connection.Read(buffer, sizeof(buffer) - 1);
        if(bytes == 0 || bytes == -1) {
        // ESP_LOGI("","i read:%s",buffer);
        if (bytes < 0) {
            perror("read");
            printf("errno = %d\n", errno);
        }
            ESP_LOGW("Con","Close code %d",bytes);
            ESP_LOGW("Con", "Connection closed!");
            self->flags &= ~(1 << FLAG_HasConnection);
            self->Connection.Close();
            vTaskDelete(NULL);
        }
        self->buffio.WriteNext(buffer, bytes);
    }
}

void CNC::UpdateScreen(void* param) {
    CNC* self = (CNC*)param;
    if(self->Screen == nullptr) {
        vTaskDelete(NULL);
        return;
    }

    while(1) {
        xEventGroupWaitBits(self->Events,EVENT_UI_UPDATE,true,false,portMAX_DELAY); //Wait new data
        char buf[24] = {};
        sprintf(buf, "IP:%s", !strcmp(self->IP, "") ? "No connected!" : (self->IP));
        char FlagsBuf[24];
        self->Screen->SetPosition(45, 15);
        sprintf(FlagsBuf, "Work:%s", (self->flags & (1 << FLAG_ExecutingTask)) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->SetPosition(45, 25);
        sprintf(FlagsBuf, "Connected:%s", (self->flags & (1 << FLAG_HasConnection)) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->SetPosition(45, 35);
        sprintf(FlagsBuf, "SD Card:%s", (self->flags & (1 << FLAG_SDInit)) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->DrawRect(0, 0, 127, 10, false);
        self->Screen->SetPosition(2, 2);
        self->Screen->WriteText5x7(buf);

        char PosBuf[32] = {};

        self->Screen->SetPosition(2, 15);
        snprintf(PosBuf, sizeof(PosBuf), "X:%.2f", self->Position.X);
        self->Screen->WriteText5x7(PosBuf);

        self->Screen->SetPosition(2, 25);
        snprintf(PosBuf, sizeof(PosBuf), "Y:%.2f", self->Position.Y);
        self->Screen->WriteText5x7(PosBuf);

        self->Screen->SetPosition(2, 35);
        snprintf(PosBuf, sizeof(PosBuf), "Z:%.2f", self->Position.Z);
        self->Screen->WriteText5x7(PosBuf);

        self->Screen->DrawLine(40, 10, 40, 50);
        // self->Screen->DrawSprite(Sprites::MeduzaSprite(),55,15);

        // ProgressBar
        self->Screen->DrawRect(0, 50, 127, 63, false);
        self->Screen->DrawRect(14, 53, 113, 60, false);
        self->Screen->DrawRect(14, 53, 14 + self->completion, 60, true);

        self->Screen->DrawRect(0, 0, 127, 63, 0);
        self->Screen->SwapBuffers();
    }
}

void CNC::SetupGPIOs() {
    gpio_config_t EndstopsConf = {
        .pin_bit_mask = (1ULL << X_ENDSTOP_PORT) |
                        (1ULL << Y_ENDSTOP_PORT) |
                        (1ULL << Z_ENDSTOP_PORT) |
                        (1ULL << E_ENDSTOP_PORT),
        .mode         = gpio_mode_t::GPIO_MODE_INPUT,
        .pull_up_en   = gpio_pullup_t::GPIO_PULLUP_DISABLE,
        .pull_down_en = gpio_pulldown_t::GPIO_PULLDOWN_DISABLE,
        .intr_type    = gpio_int_type_t::GPIO_INTR_DISABLE,
    };
    gpio_config_t SteppersConf = {
        .pin_bit_mask = (1ULL << X_DIR_PORT) |
                        (1ULL << Y_DIR_PORT) |
                        (1ULL << E_DIR_PORT) |
                        (1ULL << Z_DIR_PORT) |
                        (1ULL << X_STEP_PORT) |
                        (1ULL << Y_STEP_PORT) |
                        (1ULL << E_STEP_PORT) |
                        (1ULL << Z_STEP_PORT),
        .mode         = gpio_mode_t::GPIO_MODE_OUTPUT,
        .pull_up_en   = gpio_pullup_t::GPIO_PULLUP_DISABLE,
        .pull_down_en = gpio_pulldown_t::GPIO_PULLDOWN_DISABLE,
        .intr_type    = gpio_int_type_t::GPIO_INTR_DISABLE,
    };
    gpio_config(&EndstopsConf);
    gpio_config(&SteppersConf);
}

void CNC::StartTimers() {
    gptimer_config_t config = {};
    config.clk_src       = soc_periph_gptimer_clk_src_t::GPTIMER_CLK_SRC_APB;
    config.direction     = gptimer_count_direction_t::GPTIMER_COUNT_UP;
    config.resolution_hz = 10 * 1000 * 1000;  // 10 MHz (was 80MHz/8), 1000 ticks = 100 us
    config.intr_priority = 2;

    if(gptimer_new_timer(&config, &this->axis_timer) != ESP_OK) {
        exit(10);
        return;
    }
    gptimer_alarm_config_t alarm = {};
    alarm.alarm_count  = 1000;
    alarm.reload_count = 0;
    alarm.flags.auto_reload_on_alarm = true;
    gptimer_set_alarm_action(this->axis_timer, &alarm);

    gptimer_event_callbacks_t cbs = {
        .on_alarm = AxisTimerISR,
    };

    gptimer_register_event_callbacks(this->axis_timer, &cbs, this);
    gptimer_enable(this->axis_timer);
    // gptimer_start(axis_timer);
    gptimer_stop(this->axis_timer);
    gptimer_set_raw_count(this->axis_timer,0);

    //watch dog
    esp_timer_create_args_t timer_args = {};
    timer_args.callback = WatchDogTimerHandler;
    esp_timer_create(&timer_args,&this->wathcDogTimer);
    esp_timer_start_periodic(this->wathcDogTimer,1000000);

    // //UI Timer
    // timer_args.callback = UpdateScreen;
    // timer_args.arg = this;
    // esp_timer_create(&timer_args,&this->UITimer);
    // esp_timer_start_periodic(this->UITimer,1000000);
}

// Timers
/////////////////////////////////////////////////
bool IRAM_ATTR CNC::AxisTimerISR(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *edata,
    void *arg
){

    return true;
}

void CNC::WatchDogTimerHandler(void* arg){
    // esp_rom_printf("Program timer!\n");
}
