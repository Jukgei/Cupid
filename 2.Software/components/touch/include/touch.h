#ifndef TOUCH_H
#define TOUCH_H

#include "driver/touch_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_sleep.h"

#define     KEY_CONFIRM         TOUCH_PAD_NUM0
#define     KEY_LEFT            TOUCH_PAD_NUM2
#define     KEY_RIGHT           TOUCH_PAD_NUM7
#define     TOUCH_THRESH_NO_USE (0)

void TouchTask(void *pvParameter);

void touch_wakeup_init(void);
void AllModuleLowPower();

#endif

