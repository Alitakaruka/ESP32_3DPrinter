#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "dirent.h"

extern "C"{
    struct FileWriter
    {
        char fileName[50];
        char Data[100];
        unsigned char Stop; 
    };
    
}
class SD_Card
{
private:
    bool is_ISPInit = false;
    const char path[8] = "/sdcard";
private:
    esp_err_t SPI_Init(gpio_num_t MISO,
                       gpio_num_t MOSI,
                       gpio_num_t SCL);

public:
    bool TryConnectSD(gpio_num_t MISO = gpio_num_t::GPIO_NUM_23,
                      gpio_num_t MOSI = gpio_num_t::GPIO_NUM_19,
                      gpio_num_t SCL = gpio_num_t::GPIO_NUM_18,
                      gpio_num_t CS = gpio_num_t::GPIO_NUM_5);
    void GetFilesList(char* buf,int BufLen);
    esp_err_t WriteToFile(char* fileName,volatile FileWriter* stream);
};