#include "CNC.hpp"

void CNC::startAndServe(void *params)
{
    CNC mashine;
    printf("Start init mashine!\n");
    mashine.Serve();
}

CNC::CNC() : Buffer(10)
{
    this->Screen = new ssd1306();
    int err = Screen->Init();
    // this->Screen->ssd1306_setRotation(true);
    if (err != ESP_OK){
        ESP_LOGE("","LCD display init error!");
    }
    showLogo();

    this->Events = xEventGroupCreate();
    xMutex = xSemaphoreCreateMutex();
    if(xMutex == NULL){
        ESP_ERROR_CHECK(err);
    } 

    vTaskDelay(pdMS_TO_TICKS(1000));
    xTaskCreatePinnedToCore(UpdateSkreen, "UpdateSkreen", 4000, this, 1, NULL, 0);

    initWifi();
    if (sdCard.TryConnectSD() != ESP_OK) {
        ESP_LOGE("SPI", "Error init SPI");
    }else{
        this->flags |= (1 << FLAG_SDInit);
    }
    EventBits_t bits = xEventGroupWaitBits(this->Events, EVENT_WIFIConneced, true, false, portMAX_DELAY);
    // accept tcpConnections
    xTaskCreatePinnedToCore(AcceptConnection, "ConnectionAccept", 2048, this, 1, NULL, 0);

}

void CNC::Serve()
{
    while (1)
    {
        if (!this->Buffer.isEmpty())
        {
            Connection.Write(CommandACK);
            const char *Command = this->Buffer.GetNowElement();    

            if (!strcmp(Command, Identification)){
                Connection.Write("Ok! I Esp32 Laser CNC. Created by Slava");
            }
            else if (!strcmp(Command, SYNC)){
                Connection.Write("Sync");
            }
            else if (!strcmp(Command, StartPrint)){
                Connection.Write("Start print!");
            }
            else if (!strcmp(Command, GetState)){
                char buf[100];
                auto test = "X:%f Y:%f Z:%f";
                sprintf(buf, test, position.X, position.Y, position.Z);
                Connection.Write(buf);
            }else if(!strcmp(Command,"")){
                FileWriter writer;

                strcmp(writer.fileName,"test");
                this->sdCard.WriteToFile("1",&writer);
            }
            else{
                Connection.Write("No command!");
            }

            this->Buffer.Next();
        }
    }
}

void CNC::ExecuteGCode()
{
}

void CNC::showLogo()
{
    if (this->Screen == nullptr)
    {
        return;
    }
    this->Screen->DrawBitmap(reinterpret_cast<const char *>(Images::MainLogo),
                             sizeof(Images::MainLogo));
    this->Screen->SwapBuffers();
}

void StartCNCInTask()
{
    xTaskCreatePinnedToCore(CNC::startAndServe, "CNC_Mashine", 8192, NULL, 1, NULL, 1);
    vTaskDelete(NULL);
}

void CNC::initWifi()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
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
    esp_err_t err = esp_wifi_get_config(WIFI_IF_STA, &current_config);

    esp_netif_create_default_wifi_ap();
    this->wifi_netif = esp_netif_create_default_wifi_sta();
    if (err == ESP_OK &&
        strlen((char *)current_config.sta.ssid) > 0)
    {
        esp_wifi_set_mode(WIFI_MODE_STA);
        esp_wifi_start();
        esp_wifi_connect();
        return;
    }
    else
    {
        WebServer.StartWebServer("CNC Setup", "");
    }
}

void CNC::WifiHandler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    CNC *self = static_cast<CNC *>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG, "Disconnected. Reconnecting...");
        self->WifiConnectAttempts++;
        xEventGroupClearBits(self->Events, EVENT_WIFIConneced);
        if (self->WifiConnectAttempts >= 10)
        {
            ESP_LOGW(TAG, "WIFI cant connected! Start auth web server!");
            memset(self->IP, 0, sizeof(self->IP));
            self->WebServer.StartWebServer("CNC Setup", "");
        }else{
            esp_wifi_connect();
        }
    }
    else if (event_base == IP_EVENT)
    {
        if (event_id == IP_EVENT_STA_GOT_IP){
            ESP_LOGW(TAG, "Wifi connected!");
            self->getIP();
            xEventGroupSetBits(self->Events, EVENT_WIFIConneced);
            self->WifiConnectAttempts = 0;
            self->WebServer.StopWebServer();
        }
    }
}

const char *CNC::getIP()
{
    esp_netif_ip_info_t ipInfo;
    if (esp_netif_get_ip_info(this->wifi_netif, &ipInfo) == ESP_OK)
    {
        inet_ntoa_r(ipInfo, this->IP, sizeof(this->IP));
    }
    return this->IP;
}

void CNC::AcceptConnection(void *params)
{
    CNC *self = static_cast<CNC *>(params);
    auto listener = net::ListenPort("tcp", CNC_WEB_PORT);
    while (1)
    {
        net::Conn con = listener.AcceptConn();
        if (con.isValidConnection())
        {
            self->Connection = con;
            if (self->flags & (1 << FLAG_HasConnection))
            {
                // todo: Error msg
                con.Close();
                continue;
            }
            xTaskCreatePinnedToCore(HandleConnection, "Handler", 2048, self, 2, NULL, 0);
            self->flags |= (1 << FLAG_HasConnection);
        }
    }
}

void CNC::HandleConnection(void *params)
{
    auto self = static_cast<CNC *>(params);
    char buffer[100];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes = self->Connection.Read(buffer, sizeof(buffer) - 1);
        if (bytes == 0 || bytes == -1)
        {
            self->Connection.Close();
            self->flags &= ~(1 << FLAG_HasConnection);
            break;
        }
        buffer[bytes + 1] = '\0';
            for(int i = 0; i < strlen(buffer); i++){
                   printf("%d ", (int)buffer[i]);
            }
            printf("\n");

        auto el = self->Buffer.GetNowElement();
        self->Buffer.AddElement(buffer,sizeof(buffer));
    }
    vTaskDelete(NULL);
}

void CNC::UpdateSkreen(void *param)
{
    CNC *self = (CNC *)param;
    if (self->Screen == nullptr)
    {
        vTaskDelete(NULL);
        return;
    }
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));

        char buf[24] = {};
        sprintf(buf, "IP:%s",
                !strcmp(self->IP, "") ? "No connected!" : (self->IP));        
        char FlagsBuf[24];
        self->Screen->SetPosition(45, 15);
        sprintf(FlagsBuf, "Work:%s",
                (self->flags & (1 << FLAG_IsWorking)) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->SetPosition(45, 25);
        sprintf(FlagsBuf, "Connected:%s",
                (self->flags & (1 << FLAG_HasConnection)) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->SetPosition(45, 35);
        sprintf(FlagsBuf, "SD Card:%s",
                (self->flags & (1 << FLAG_SDInit)) ? "Yes" : "No");
        self->Screen->WriteText5x7(FlagsBuf);

        self->Screen->DrawRect(0, 0, 127, 10, false);
        self->Screen->SetPosition(2, 2);
        self->Screen->WriteText5x7(buf);

        char PosBuf[32] = {};

        self->Screen->SetPosition(2, 15);
        snprintf(PosBuf, sizeof(PosBuf), "X:%.2f", self->position.X);
        self->Screen->WriteText5x7(PosBuf);

        self->Screen->SetPosition(2, 25);
        snprintf(PosBuf, sizeof(PosBuf), "Y:%.2f", self->position.Y);
        self->Screen->WriteText5x7(PosBuf);

        self->Screen->SetPosition(2, 35);
        snprintf(PosBuf, sizeof(PosBuf), "Z:%.2f", self->position.Z);
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


