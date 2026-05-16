#include <stdio.h>
#include "include/TransmitBuffer.hpp"


// template<uint16_t Len>
// esp_err_t TransmitBuffer<Len>::WriteN(const char* str, size_t len) {
//     if(((sizeof(this->buf) - this->bufLen) - len) <= 0){
//         return ESP_ERR_NO_MEM;
//     }
//     if (len > sizeof(buf)){
//         return ESP_ERR_NO_MEM;
//     }
//     char* CurrentPtr = this->buf + this->bufLen;
//     strncpy(CurrentPtr,str,len);
//     this->bufLen += len;
//     return ESP_OK;
// }

// template<uint16_t Len>
// esp_err_t TransmitBuffer<Len>::Write(const char* str) {
//     size_t len = strlen(str);
//     if(((sizeof(this->buf) - this->bufLen) - len) <= 0){
//         return ESP_ERR_NO_MEM;
//     }
//     if (len > sizeof(this->buf)){
//         return ESP_ERR_NO_MEM;
//     }
//     char* CurrentPtr = this->buf + this->bufLen;
//     strncpy(CurrentPtr,str,bufLen);
//     this->bufLen += len;
//     return ESP_OK;
// }

// template<uint16_t Len>
// esp_err_t TransmitBuffer<Len>::Write(const char* str, ...) {
//      va_list args;
//     va_start(args, str);
//     char* currentPtr = this->buf + this->bufLen;
//     int  len          = vsnprintf(buf, sizeof(currentPtr), str, args);
//     this->bufLen +=len;
//     va_end(args);
//     return ESP_OK;
// }

// template<uint16_t Len>
// esp_err_t TransmitBuffer<Len>::Write(char byte) {
//     if(((sizeof(this->buf) - this->bufLen) - 1) <= 0) {
//         return ESP_ERR_NO_MEM;
//     }
//     this->buf[this->bufLen] = byte;
//     this->bufLen++;
//       return ESP_OK;
// }
