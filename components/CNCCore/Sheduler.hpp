#pragma once

#include "algorithm"
#include "cstring"
#include "stdio.h"
#include "freertos/task.h"
#include "esp_log.h"

class Motion
{
private:
    float dX,dY,dZ,dE= 0;
    float speed_mms = 0;
    float аccelerationX = 0;
    float аccelerationY = 0;
    float аccelerationZ = 0;
    float аccelerationE = 0;
    float MaxSpeedX = 0;
    float MaxSpeedY = 0;
    float MaxSpeedZ = 0;
    float MaxSpeedE = 0;

public:
    Motion(/* args */) = default;
};


#define IsFullEvent 1
#define IsEmptyEvent 2
#define BasePlan 10
class Sheduler
{
private:
    QueueHandle_t ch;
public:

    Sheduler(){
        ch = xQueueCreate(BasePlan,sizeof(Motion));
    }
    void Push(float X, float Y,float Z, float E, float F){

        Motion m;
        // ESP_LOGW("Sheduler","PUSH");
        xQueueSend(this->ch,&m,portMAX_DELAY);
        // ESP_LOGW("Sheduler","PUSH END");

    }
    Motion Get(){
        Motion m;
        // ESP_LOGW("Sheduler","Sleep");
        xQueueReceive(this->ch,&m,portMAX_DELAY);
        // ESP_LOGW("Sheduler","Awake");
        return m;
    }
    ~Sheduler(){}
};
