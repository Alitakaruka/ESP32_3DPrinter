

idf_component_register(SRCS "src/web.cpp" "src/Wifi_Auth.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES esp_http_client esp_http_server esp_netif esp_wifi nvs_flash)