#include "../MainDrivers.hpp"

i2c_master_bus_config_t Driver_I2C::GetMasterConfig(gpio_num_t scl,gpio_num_t sda)
{
    i2c_master_bus_config_t config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true,
        },
    };
    return config;
}
