#include "../MainDrivers.hpp"

i2c_master_bus_config_t Driver_I2C::GetMasterConfig(gpio_num_t scl, gpio_num_t sda) {
    // i2c_master_bus_config_t config = {
    //     .i2c_port = I2C_NUM_0,
    //     .sda_io_num = sda,
    //     .scl_io_num = scl,
    //     .clk_source = I2C_CLK_SRC_DEFAULT,
    //     .glitch_ignore_cnt = 7,
    //     .flags = {
    //         .enable_internal_pullup = true,
    //     },
    // };
    i2c_master_bus_config_t config      = {};
    config.i2c_port                     = I2C_NUM_0;
    config.sda_io_num                   = sda;
    config.scl_io_num                   = scl;
    config.clk_source                   = I2C_CLK_SRC_DEFAULT;
    config.glitch_ignore_cnt            = 7;
    config.intr_priority                = 0;
    config.trans_queue_depth            = 0;
    config.flags.enable_internal_pullup = true;

    return config;
}
