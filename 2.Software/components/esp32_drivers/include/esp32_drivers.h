#ifndef ESP32_DRIVERS_H
#define ESP32_DRIVERS_H

#include "driver/i2c.h"

#define I2C_SDA_PIN     9
#define I2C_SCL_PIN     10
/* #define I2C_SDA_PIN     27 */
/* #define I2C_SCL_PIN     16 */
#define I2C_SPEED       50000
#define ACK_CHECK_EN    true
#define MINI_WATCH_I2C  I2C_NUM_1

void i2c_delete();
void i2c_init();

#endif

