#include "esp32_drivers.h"

void i2c_init()
{
    // 1. Configuration
    i2c_config_t i2c_param = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_SPEED,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
    };
    ESP_ERROR_CHECK(i2c_param_config(MINI_WATCH_I2C, &i2c_param));
    /* i2c_param_config(I2C_NUM_0, &i2c_param); */


    // 2. Install Driver
    ESP_ERROR_CHECK(i2c_driver_install(MINI_WATCH_I2C,I2C_MODE_MASTER,0,0,0));
    /* i2c_driver_install(MINI_WATCH_I2C,I2C_MODE_MASTER,0,0,0); */
}

void i2c_delete()
{
    i2c_driver_delete(MINI_WATCH_I2C);
}


