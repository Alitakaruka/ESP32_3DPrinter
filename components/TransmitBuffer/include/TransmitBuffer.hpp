#include "esp_err.h"
#include "esp_log.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

template <uint16_t Len = 1024> class TransmitBuffer {
  private:
    char     buf[Len] = {};
    uint16_t bufLen   = 0;
    SemaphoreHandle_t mutex;
  public:
    esp_err_t WriteN(const char* str, size_t len) {
        xSemaphoreTake(this->mutex,portMAX_DELAY);
        if(((sizeof(this->buf) - this->bufLen) - len) <= 0) { return ESP_ERR_NO_MEM; }
        if(len > sizeof(buf)) { return ESP_ERR_NO_MEM; }
        char* CurrentPtr = this->buf + this->bufLen;
        strncpy(CurrentPtr, str, len);
        this->bufLen += len;
        xSemaphoreGive(this->mutex);
        return ESP_OK;
    }

    esp_err_t Write(char byte) {
        if(((sizeof(this->buf) - this->bufLen) - 1) <= 0) { return ESP_ERR_NO_MEM; }
        this->buf[this->bufLen] = byte;
        this->bufLen++;
        return ESP_OK;
    }

    esp_err_t Write(const char* str, va_list args) {
        xSemaphoreTake(this->mutex,portMAX_DELAY);
        char* currentPtr = this->buf + this->bufLen;
        int   len        = vsnprintf(currentPtr, sizeof(this->buf) - bufLen, str, args);
        this->bufLen += len;
        xSemaphoreGive(this->mutex);
        return ESP_OK;
    }

    esp_err_t Write(const char* str) {
        xSemaphoreTake(this->mutex,portMAX_DELAY);
        size_t len = strlen(str);
        if(((sizeof(this->buf) - this->bufLen) - len) <= 0) { return ESP_ERR_NO_MEM; }
        if(len > sizeof(buf)) { return ESP_ERR_NO_MEM; }
        char* CurrentPtr = this->buf + this->bufLen;
        strncpy(CurrentPtr, str, len);
        this->bufLen += len;
        xSemaphoreGive(this->mutex);
        return ESP_OK;
    }



    const char* GetResult() { return this->buf; }

    void Clear() {
        memset(this->buf, 0, sizeof(this->buf));
        this->bufLen = 0;
    }

    TransmitBuffer(){
        this->mutex = xSemaphoreCreateMutex();
    };
    // ~TransmitBuffer();
};
