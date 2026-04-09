#include "dirent.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

class SD_Card {
  private:
    bool       is_ISPInit  = false;
    const char path[8]     = "/sdcard";
    FILE*      currentFile = NULL;

  private:
    esp_err_t SPI_Init(gpio_num_t MISO,
                       gpio_num_t MOSI,
                       gpio_num_t SCL);

  public:
    bool      TryConnectSD(gpio_num_t MISO = gpio_num_t::GPIO_NUM_23,
                           gpio_num_t MOSI = gpio_num_t::GPIO_NUM_19,
                           gpio_num_t SCL  = gpio_num_t::GPIO_NUM_18,
                           gpio_num_t CS   = gpio_num_t::GPIO_NUM_5);
    void      GetFilesList(char* buf, int BufLen);
    esp_err_t AppendToFile(char* fileName, const char* stream);
    esp_err_t CreateFile(const char* fileName);
    esp_err_t SelectFile(const char* filename);
};