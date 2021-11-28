#ifndef BATTERY_H
#define BATTERY_H

#define CHARGE_PIN 18
#define STDBY_PIN  7

#define CHARGE_SEL    (1ULL<<CHARGE_PIN)
#define STDBY_SEL    (1ULL<<STDBY_PIN) 

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

void BatteryTask(void *pvParameter);
#endif

