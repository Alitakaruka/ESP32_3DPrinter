#include "SD_Card.hpp"

esp_err_t SD_Card::SPI_Init(gpio_num_t MISO,
                            gpio_num_t MOSI,
                            gpio_num_t SCL)
{
    spi_bus_config_t spiConfig = {};
    spiConfig.mosi_io_num = MOSI;
    spiConfig.miso_io_num = MISO;
    spiConfig.sclk_io_num = SCL;
    spiConfig.quadwp_io_num = -1;
    spiConfig.quadhd_io_num = -1;

    esp_err_t err = spi_bus_initialize(VSPI_HOST, &spiConfig, SPI_DMA_CH1);

    if (err != ESP_OK)
    {
        return err;
    }
    return ESP_OK;
}

bool SD_Card::TryConnectSD(gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t SCL, gpio_num_t CS)
{
    if (!is_ISPInit)
    {
        if(SPI_Init(MISO, MOSI, SCL) == ESP_OK){
            this->is_ISPInit = true;
        }else{
            return  false;
        }
    }
    sdspi_device_config_t SDConfig = SDSPI_DEVICE_CONFIG_DEFAULT();
    SDConfig.host_id = VSPI_HOST;
    SDConfig.gpio_cs = CS;

    sdspi_dev_handle_t handle; // Card

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = VSPI_HOST;
    host.max_freq_khz = 10000;
    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = false,
        .max_files = 5,
    };
    sdmmc_card_t *card;
    return esp_vfs_fat_sdspi_mount(path, &host, &SDConfig, &mount_cfg, &card);
}

void SD_Card::GetFilesList(char* buf,int BufLen){
    DIR* dir = opendir(this->path);
    if (!dir){
        return;
    }
    struct dirent *entry;
    
    while((entry = readdir(dir)) != NULL){
        size_t name_len = strlen(entry->d_name) + 1;
        if (BufLen < name_len)
            break;

        memcpy(buf, entry->d_name, name_len);
        buf += name_len;
        BufLen -= name_len;
    }
}

esp_err_t WriteToFile(char* fileName,volatile FileWriter* stream){
    FILE* file = fopen(fileName,"w");

    if(file == NULL){
        return ESP_FAIL;
    }
    while(!stream->Stop){

    }
    return ESP_OK;
}
