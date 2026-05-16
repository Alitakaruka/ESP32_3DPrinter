#include "CNC.hpp"

void StartCNCInTask() {
    ESP_LOGI("Init", "Start init mashine!\n");
    CNC* machine = new CNC();
    xTaskCreatePinnedToCore(CNC::ExecuteCommands, "Executor", 4096, machine, 0, NULL, 1);
    xTaskCreatePinnedToCore(CNC::ReadCommands,"Reader",4096,machine,0,NULL,1);
    vTaskDelete(NULL);
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


    // RMutex       = xSemaphoreCreateMutex();
    // WMutex       = xSemaphoreCreateMutex();
    // if(RMutex == NULL || WMutex == NULL) {
    //     ESP_ERROR_CHECK(1);
    // }


    StartTimers();
    initWifi();
    if(sdCard.TryConnectSD() != ESP_OK) {
        ESP_LOGE("SPI", "Error init SPI");
    } else {
        this->flags |= FLAG_SDInit;
    }
    EventBits_t bits = xEventGroupWaitBits(this->Events, EVENT_WIFIConneced, true, false, portMAX_DELAY);
    // accept tcpConnections
    xTaskCreatePinnedToCore(AcceptConnection, "ConnectionAccept", 2048, this, 1, NULL, 0);
}

void CNC::ReadCommands(void* args) {
    CNC* self = static_cast<CNC*>(args);
    while(1) {
        char CurrentCommand[512] = {};
        self->buffio.ReadLine(CurrentCommand, sizeof(CurrentCommand), Commands::EndOfData);
        char ch = toupper(CurrentCommand[0]);
        switch(ch) {
        case 'G':
            self->ReadGCode(CurrentCommand);
            break;
        case 'M':
            self->ReadMCode(CurrentCommand);
            break;
        default:
            self->ExecuteBase(CurrentCommand);
            break;
        }
        self->WriteCommandToBuffer(CNC_Responce::CommandACK);
        self->SendBuffer();
    }
}

void CNC::ExecuteCommands(void* args){
    CNC* self = static_cast<CNC*>(args);
    while(1){
        Motion m = self->sheduler.Get();
        ESP_LOGW(pcTaskGetName(NULL),"MOOOOOOVE");

    }
}

void CNC::ExecuteBase(const char* Command) {
    if(!strcmp(Command, Commands::Identification)) {
        Command_Identification();
    } else if(!strcmp(Command, Commands::SYNC)) {
        this->WriteCommandToBuffer(CNC_Responce::MyBufferLen, BufferSize);
        this->buffio.clear();
    } else if(strcasestr(Command, Commands::SetCompletion) != NULL) {
        int completion = 0;
        if(sscanf(Command, Commands::SetCompletion, &completion)) {
            if(completion > 100) { completion = 100; }
            if(completion < 0) { completion = 0; }
            this->completion = static_cast<uint8_t>(completion);
            this->ChargeUI();
        }
    } else if(!strcmp(Command, Commands::StartTask)) {
        this->flags.fetch_or(FLAG_ExecutingTask);
        this->ChargeUI();
    } else if(!strcmp(Command, Commands::EndTask)) {

        this->flags.fetch_and(~FLAG_ExecutingTask);
        this->ChargeUI();
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

        ESP_LOGI("WIFI", "Disconnected. Reconnecting...");
        self->WifiConnectAttempts++;
        xEventGroupClearBits(self->Events, EVENT_WIFIConneced);
        if(self->WifiConnectAttempts >= 10) {
            ESP_LOGW("WIFI", "cant connected! Start auth web server!");
            memset(self->IP, 0, sizeof(self->IP));
            self->WebServer.StartWebServer("CNC Setup", "");
        } else {
            esp_wifi_connect();
        }
        self->ChargeUI();
    } else if(event_base == IP_EVENT) {
        if(event_id == IP_EVENT_STA_GOT_IP) {
            ESP_LOGW("WIFI", "connected!");
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
            if(self->flags.load() &  FLAG_HasConnection) {
                ESP_LOGE("Connection", "FLAG_HasConnection is SET!");
                con.Close();
                continue;
            }
            self->flags.fetch_or(FLAG_HasConnection);
            self->ChargeUI();//new connection
            xTaskCreatePinnedToCore(HandleConnection, "Handler", 2048*2, self, 2, NULL, 0);
        }
    }
}

void CNC::HandleConnection(void* params) {
    auto self = static_cast<CNC*>(params);
    char buffer[512];
    ESP_LOGW("Connection", "Start listen!");

    while(1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = self->Connection.Read(buffer, sizeof(buffer) - 1);
        if(bytes == 0 || bytes == -1) {
            // ESP_LOGI("","i read:%s",buffer);
            if(bytes < 0) {
                perror("read");
                printf("errno = %d\n", errno);
            }
            ESP_LOGW("Con", "Close code %d", bytes);
            ESP_LOGW("Con", "Connection closed!");
            self->flags.fetch_and(~FLAG_HasConnection);
            self->Connection.Close();
            self->ChargeUI();
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
        sprintf(FlagsBuf, "Work:%s", (self->flags.load() & FLAG_ExecutingTask) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->SetPosition(45, 25);
        sprintf(FlagsBuf, "Connected:%s", (self->flags.load() & FLAG_HasConnection) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->SetPosition(45, 35);
        sprintf(FlagsBuf, "SD Card:%s", (self->flags.load() &  FLAG_SDInit) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->DrawRect(0, 0, 127, 10, false);
        self->Screen->SetPosition(2, 2);
        self->Screen->WriteText5x7(buf);

        char PosBuf[32] = {};

        self->Screen->SetPosition(2, 15);
        snprintf(PosBuf, sizeof(PosBuf), "X:%.2f", self->CurrentPosition.X);
        self->Screen->WriteText5x7(PosBuf);

        self->Screen->SetPosition(2, 25);
        snprintf(PosBuf, sizeof(PosBuf), "Y:%.2f", self->CurrentPosition.Y);
        self->Screen->WriteText5x7(PosBuf);

        self->Screen->SetPosition(2, 35);
        snprintf(PosBuf, sizeof(PosBuf), "Z:%.2f", self->CurrentPosition.Z);
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
    timer_args.arg  = this;
    timer_args.callback = WatchDogTimerHandler;
    esp_timer_create(&timer_args,&this->wathcDogTimer);
    esp_timer_start_periodic(this->wathcDogTimer,WDTimerUs);//100 MS || 10hz

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
    // CNC* Self = static_cast<CNC*>(arg);
    // static uint8_t counter = 0;
    // static uint16_t SendTimeout = 0;
    // switch (counter) {
    // case 0:
    //     Self->WriteCommandToBuffer(CNC_Responce::MyPositionX, Self->CurrentPosition.X);
    //     break;
    // case 1:
    //     Self->WriteCommandToBuffer(CNC_Responce::MyPositionY, Self->CurrentPosition.X);
    //     break;
    // case 2:
    //     Self->WriteCommandToBuffer(CNC_Responce::MyPositionZ, Self->CurrentPosition.X);
    //     break;
    // // case 3:
    // //     Self->WriteCommandToBuffer(BedTemp, iPrinter.tempBed,
    // //                       iPrinter.BedPID.needValue);
    // //     break;
    // // case 4:
    // //     Self->WriteCommandToBuffer(ExtruderTemp, iPrinter.tempNozzle,
    // //                       iPrinter.NozzlePID.needValue);
    // //     break;
    // // case 5:
    // //     Self->WriteCommandToBuffer(FanSpeed, 1, iPrinter.fan1);
    // //     break;
    // // case 6:
    // //     Self->WriteCommandToBuffer(FanSpeed, 2, iPrinter.fan2);
    // //     break;
    // // default:
    // //     break;
    // }
    // counter = (counter + 1) % 7;
    // SendTimeout++;
    // if(SendTimeout / WDTicksInSecond >= TransmitDataTimeout_S){
    //     Self->SendBuffer();
    //     SendTimeout =0;
    // }

}
