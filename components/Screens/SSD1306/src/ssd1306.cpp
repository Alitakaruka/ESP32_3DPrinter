#include "ssd1306.hpp"

ssd1306::ssd1306(uint8_t width, uint8_t height)
{
    this->Matrix = new uint8_t *[height / 8];
    for (int i = 0; i < height / 8; i++)
    {
        Matrix[i] = new uint8_t[width];
        memset(Matrix[i], 0, width);
    }

    this->width = width;
    this->height = height;
    this->pages = height / 8;
}

ssd1306::~ssd1306()
{
    for (int i = 0; i < this->height; i++)
    {
        delete[] Matrix[i];
    }
    delete[] Matrix;
}

esp_err_t ssd1306::Init(gpio_num_t SCL, gpio_num_t SDA, uint16_t deviceAdres)
{
    i2c_master_bus_config_t config = {};
    config.clk_source = I2C_CLK_SRC_DEFAULT;
    config.flags.enable_internal_pullup = true;
    config.i2c_port = I2C_NUM_0;
    config.scl_io_num = SCL;
    config.sda_io_num = SDA;
    config.glitch_ignore_cnt = 7;
    config.intr_priority = 1;

    i2c_master_bus_handle_t handle;
    esp_err_t err = i2c_new_master_bus(&config, &handle);

    if (err != NULL)
    {
        return err;
    }

    err = i2c_master_probe(handle, deviceAdres, 500);
    if (err == ESP_OK)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }else{
        return err;
    }

    i2c_device_config_t LCD_display;
    LCD_display.device_address = deviceAdres;
    LCD_display.scl_speed_hz = 400000;
    LCD_display.dev_addr_length = I2C_ADDR_BIT_LEN_7;

    err = i2c_master_bus_add_device(handle, &LCD_display, &(this->device));
    if (err != NULL)
    {
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    uint8_t ssd1306_init_cmd[] = {
        OLED_CONTROL_BYTE_CMD,
        OLED_CMD_DISPLAY_OFF,
        OLED_CMD_SET_MUX_RATIO, static_cast<uint8_t>((this->height - 1)),
        OLED_CMD_SET_VERT_DISPLAY_OFFSET, 0x00,
        OLED_MASK_DISPLAY_START_LINE | 0x00,
        OLED_CMD_COM_SCAN_DIRECTION_NORMAL,
        OLED_CMD_SEGMENT_REMAP_LEFT_TO_RIGHT,
        OLED_CMD_SET_COM_PIN_HARDWARE_MAP, 0x12,
        OLED_CMD_SET_MEMORY_ADDR_MODE, 0x02,
        OLED_CMD_SET_CONTRAST_CONTROL, 0xFF,
        OLED_CMD_SET_DISPLAY_CLK_DIVIDE, 0x80,
        OLED_CMD_ENABLE_DISPLAY_RAM,
        OLED_CMD_NORMAL_DISPLAY,
        OLED_CMD_SET_CHARGE_PUMP, 0x14,
        OLED_CMD_DISPLAY_ON};

    err = i2c_master_transmit(this->device, ssd1306_init_cmd, sizeof(ssd1306_init_cmd), 500);
    return err;
}

void ssd1306::DrawBitmap(const char *bitmap, int len)
{
    if (len > width * height)
    {
        return;
    }

    for (int i = 0; i < pages; i++)
    {
        for (int j = 0; j < width; j++)
        {
            this->Matrix[i][j] = *bitmap;
            bitmap++;
        }
    }
}

void ssd1306::DrawSprite(const char *sprite,
                         uint8_t X, uint8_t Y,
                         uint8_t width, uint8_t height, int len)
{
    int cols = len / width;
    int bitsSliced = cols * 8 - height;

    for (int Col = 0; Col < cols; Col++){
        for (int i = 0; i < width; i++){
            int x = X + i;
            uint8_t data = sprite[i];
            int Bits = (Col == cols - 1) ? cols * 8 - height : 8;
            for (int j = 0; j < Bits; j++)
            {
                SetPixel(x, (Y + j) + (Col * 8),
                         (data & (1 << j)));
            }
        }
        sprite+=width;
    }
}

void ssd1306::DrawSprite(void* img, uint8_t X,uint8_t Y){
    struct {
        int width;
        int height;
        int Len;
        const unsigned char* Img;
    } *ImgData = (decltype(ImgData))img;

    
    int cols = ImgData->Len / ImgData->width;
    int bitsSliced = cols * 8 - ImgData->height;
    auto Sprite = ImgData->Img;
    for (int Col = 0; Col < cols; Col++){
        for (int i = 0; i < ImgData->width; i++){
            int x = X + i;
            uint8_t data = Sprite[i];
            int Bits = (Col == cols - 1) ? cols * 8 - ImgData->height : 8;
            for (int j = 0; j < Bits; j++)
            {
                SetPixel(x, (Y + j) + (Col * 8),
                         (data & (1 << j)));
            }
        }
        Sprite+=ImgData->width;
    }
}


void ssd1306::WriteText5x7(const char *Text)
{
    int nowY = this->CurrentPosition.GetY();
    int nowX = this->CurrentPosition.GetX();

    while (*Text != '\0')
    {
        // Проверка выхода за границы
        if ((width - nowX) < 6)
        { // 5 столбцов + пробел
            nowX = 0;
            nowY += 8;
        }
        if (nowX >= width || nowY + 8 > height)
        {
            return;
        }

        // Получаем символ
        const uint8_t *glyph = TextTable[(unsigned char)*Text];

        // Рисуем 5 колонок символа
        for (int col = 0; col < 5; col++)
        {
            uint8_t columnBits = glyph[col]; // 7-битный столбец символа
            int page = nowY / 8;
            int bitOffset = nowY % 8;

            // Вставляем в текущую страницу
            Matrix[page][nowX] |= columnBits << bitOffset;

            // Если символ пересекает границу страницы — вставляем остаток в следующую
            if (bitOffset > 1 && page + 1 < 8)
            {
                Matrix[page + 1][nowX] |= columnBits >> (8 - bitOffset);
            }

            nowX++;
        }

        // Пробел между символами
        nowX++;

        Text++;
    }
}

void ssd1306::SwapBuffers()
{
    for (uint8_t i = 0; i < this->pages; i++)
    {
        if (page >= this->pages)
        {
            return;
        }

        uint8_t ram_addr_cmd[] = {
            OLED_CONTROL_BYTE_CMD,
            static_cast<uint8_t>(OLED_MASK_PAGE_ADDR | (page)),
            OLED_MASK_LSB_NIBBLE_SEG_ADDR | (0x00 & 0x0F),
            OLED_MASK_HSB_NIBBLE_SEG_ADDR | (0x00 >> 4 & 0x0F)};
        esp_err_t err = i2c_master_transmit(this->device, ram_addr_cmd, sizeof(ram_addr_cmd), 100);

        uint8_t ram_data_cmd[this->width + 1];
        ram_data_cmd[0] = OLED_CONTROL_BYTE_DATA;
        for (uint8_t i = 0; i < this->width; i++)
        {
            ram_data_cmd[i + 1] = this->Matrix[page][i];
        }
        err = i2c_master_transmit(this->device, ram_data_cmd, sizeof(ram_data_cmd), 100);
        if (err != ESP_OK)
        {
            // return err;
        }
        page++;
    }
    for (int i = 0; i < height / 8; i++)
    {
        memset(Matrix[i], 0, width);
    }
    page = 0;

    // return err;
}

void ssd1306::DrawGif(const char *Gif, int frames, int frameLen, bool repeat)
{
    while (repeat)
    {
        for (int frame = 0; frame < frames; frame++)
        {
            for (int i = 0; i < pages; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    this->Matrix[i][j] = Gif[frame * frameLen + j];
                }
            }
            SwapBuffers();
        }
    }
}

void ssd1306::DrawLine(int X0, int Y0, int X1, int Y1)
{
    int dx = abs(X1 - X0);
    int dY = abs(Y1 - Y0);
    int sX = (X0 < X1) ? 1 : -1;
    int sY = (Y0 < Y1) ? 1 : -1;
    int err = dx - dY;

    while (true)
    {
        SetPixel(X0, Y0, 1); // твоя функция вывода пикселя

        if (X0 == X1 && Y0 == Y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dY)
        {
            err -= dY;
            X0 += sX;
        }
        if (e2 < dx)
        {
            err += dx;
            Y0 += sY;
        }
    }
}

void ssd1306::DrawRect(int X0, int Y0, int X1, int Y1, bool fill)
{
    DrawLine(X0, Y0, X1, Y0);
    DrawLine(X0, Y0, X0, Y1);
    DrawLine(X1, Y0, X1, Y1);
    DrawLine(X0, Y1, X1, Y1);
    if(fill){
        for(int i = Y0; i< Y1;i++){
            DrawLine(X0,i,X1,i);
        }
    }
}

void ssd1306::SetPixel(int X, int Y, bool State) 
{

    if (X >= width || Y >= height)
    {
        printf("X:%d Y:%d\n", static_cast<int>(X), static_cast<int>(Y));
        return;
    }
    int ofset = Y % 8;
    this->Matrix[Y / 8][X] =
        (Matrix[Y / 8][X] & ~(1 << ofset)) | ((State & 1) << ofset);
}

void ssd1306::SendCommand(uint8_t Command){
    auto err = i2c_master_transmit(this->device,&Command,1,10);
    if (err != ESP_OK){
        printf("Oshibochcka\n");
    }
}