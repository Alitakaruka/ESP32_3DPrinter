#pragma once
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "esp_netif_types.h"
#include "esp_netif_ip_addr.h" // ip4addr_ntoa
#include "esp_netif.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"

extern const char html[];
extern const char htmlSucses[];
extern const char htmlError[];

class Wifi_Auth
{
private:
    httpd_handle_t server = nullptr;
private:
    void static start_webserver(httpd_handle_t server);
    esp_err_t static connect_post_handler(httpd_req_t *req);
    esp_err_t static root_get_handler(httpd_req_t *req);
    void static ConnectionHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
public:
    void  StartWebServer(const char *SSID, const char *Password);
    void  StopWebServer();
};
