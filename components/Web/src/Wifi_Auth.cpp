#include "Wifi_Auth.hpp"



void Wifi_Auth::StartWebServer(const char *SSID, const char *Password)
{
    esp_wifi_set_mode(WIFI_MODE_AP);
    wifi_config_t wifi = {};
    strcpy((char *)wifi.ap.ssid, SSID);
    wifi.ap.ssid_len = strlen(SSID);
    wifi.ap.channel = 1;
    wifi.ap.max_connection = 1;
    if (!strcmp(Password, ""))
    {
        wifi.ap.authmode = WIFI_AUTH_OPEN;
    }
    else
    {
        wifi.ap.authmode = WIFI_AUTH_WPA2_PSK;
        strcpy((char *)wifi.ap.password, Password);
    }
    esp_wifi_set_config(WIFI_IF_AP, &wifi);
    esp_wifi_start();
    start_webserver(this->server);
}

esp_err_t Wifi_Auth::root_get_handler(httpd_req_t *req)
{
    return httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
}

esp_err_t Wifi_Auth::connect_post_handler(httpd_req_t *req)
{
    char buf[128];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    buf[ret] = '\0';

    // Тут можно распарсить SSID и пароль (например через sscanf)
    char ssid[32], pass[64];
    sscanf(buf, "ssid=%31[^&]&password=%63s", ssid, pass);

    wifi_config_t wifi_cfg = {};
    strcpy((char *)wifi_cfg.sta.ssid, ssid);
    strcpy((char *)wifi_cfg.sta.password, pass);
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);

    auto Event = xEventGroupCreate();
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, ConnectionHandler, Event);

    esp_wifi_start();
    esp_wifi_connect();

    auto bits = xEventGroupWaitBits(Event, 1, true, false, pdMS_TO_TICKS(10000));
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, ConnectionHandler);

    if (bits & 1){
        ESP_LOGI("", "Connection sucses!");
        httpd_resp_send(req, htmlSucses, HTTPD_RESP_USE_STRLEN);
        vTaskDelay(pdMS_TO_TICKS(500)); 
        esp_wifi_set_mode(wifi_mode_t::WIFI_MODE_STA);
    }else{
        ESP_LOGE("", "Connection lost!");
        httpd_resp_send(req, htmlError, HTTPD_RESP_USE_STRLEN);
        esp_wifi_set_mode(wifi_mode_t::WIFI_MODE_AP);
    }

    return ESP_OK;
}

void Wifi_Auth::ConnectionHandler(void *arg, esp_event_base_t event_base,
                       int32_t event_id, void *event_data)
{
    auto Events = static_cast<EventGroupHandle_t>(arg);
    xEventGroupSetBits(Events, 1);
}

void Wifi_Auth::start_webserver(httpd_handle_t server)
{
    if(server != nullptr)return;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = nullptr};
        httpd_register_uri_handler(server, &root);

        httpd_uri_t connect = {
            .uri = "/connect",
            .method = HTTP_POST,
            .handler = connect_post_handler,
            .user_ctx = nullptr};
        httpd_register_uri_handler(server, &connect);
    }
}

void Wifi_Auth::StopWebServer(){
    if(this->server != nullptr){
         httpd_stop(server);
         server = nullptr;
    }
}

const char html[] = R"(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>WiFi Setup</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;background:#4a90e2;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:10px}
.container{background:#fff;border-radius:10px;padding:20px;width:100%;max-width:300px;box-shadow:0 4px 10px rgba(0,0,0,0.2)}
h1{text-align:center;color:#333;margin-bottom:20px;font-size:20px}
.form-group{margin-bottom:15px}
label{display:block;margin-bottom:5px;color:#555;font-size:14px}
input{width:100%;padding:10px;border:1px solid #ddd;border-radius:5px;font-size:14px}
input:focus{outline:none;border-color:#4a90e2}
button{width:100%;padding:12px;background:#4a90e2;color:#fff;border:none;border-radius:5px;font-size:16px;cursor:pointer}
button:hover{background:#357abd}
@media(max-width:400px){.container{padding:15px}}
</style>
</head>
<body>
<div class="container">
<h1>📶 WiFi Setup</h1>
<form action="/connect" method="post">
<div class="form-group">
<label>SSID:</label>
<input name="ssid"placeholder="Network name"required>
</div>
<div class="form-group">
<label>Password:</label>
<input name="password" type="password" placeholder="Password">
</div>
<button type="submit">Connect</button>
</form>
</div>
</body>
</html>
)";

const char htmlSucses[] = R"(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Success</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:20px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%)}
.card{background:#fff;border-radius:20px;padding:40px;max-width:400px;width:100%;text-align:center;box-shadow:0 20px 60px rgba(0,0,0,0.3);animation:slideIn .5s ease}
@keyframes slideIn{from{opacity:0;transform:translateY(-30px)}to{opacity:1;transform:translateY(0)}}
.icon{font-size:80px;margin-bottom:20px;animation:bounce .6s ease}
@keyframes bounce{0%,100%{transform:scale(1)}50%{transform:scale(1.1)}}
h1{font-size:28px;margin-bottom:10px;color:#28a745}
p{color:#666;font-size:16px;line-height:1.6;margin:20px 0}
.btn{display:inline-block;margin-top:20px;padding:12px 30px;background:#667eea;color:#fff;text-decoration:none;border-radius:25px;font-weight:bold;transition:all .3s;box-shadow:0 4px 15px rgba(102,126,234,.4)}
.btn:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(102,126,234,.6)}
</style>
</head>
<body>
<div class="card">
<div class="icon">✅</div>
<h1>Success!</h1>
<p>Operation completed successfully.<br>Data received and processed.</p>
<a href="/" class="btn">← Back</a>
</div>
</body>
</html>
)";

const char htmlError[] = R"(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Error</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:20px;background:linear-gradient(135deg,#f093fb 0%,#f5576c 100%)}
.card{background:#fff;border-radius:20px;padding:40px;max-width:400px;width:100%;text-align:center;box-shadow:0 20px 60px rgba(0,0,0,0.3);animation:slideIn .5s ease}
@keyframes slideIn{from{opacity:0;transform:translateY(-30px)}to{opacity:1;transform:translateY(0)}}
.icon{font-size:80px;margin-bottom:20px;animation:bounce .6s ease}
@keyframes bounce{0%,100%{transform:scale(1)}50%{transform:scale(1.1)}}
h1{font-size:28px;margin-bottom:10px;color:#dc3545}
p{color:#666;font-size:16px;line-height:1.6;margin:20px 0}
.btn{display:inline-block;margin-top:20px;padding:12px 30px;background:#f5576c;color:#fff;text-decoration:none;border-radius:25px;font-weight:bold;transition:all .3s;box-shadow:0 4px 15px rgba(245,87,108,.4)}
.btn:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(245,87,108,.6)}
</style>
</head>
<body>
<div class="card">
<div class="icon">❌</div>
<h1>Error!</h1>
<p>Failed to complete operation.<br>Please check data and try again.</p>
<a href="/" class="btn">← Back</a>
</div>
</body>
</html>
)";