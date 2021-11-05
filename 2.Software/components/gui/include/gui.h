#ifndef GUI_H
#define GUI_H

#define LV_TICK_PERIOD_MS 1

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "lvgl.h"
#include "lv_png.h"
#include "lv_core/lv_obj.h"
#include "lvgl_helpers.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "sd_card.h" 
#include <stdlib.h>

#define TAG "GUI"
#define CONFIG_LOG_DEFAULT_LEVEL 3

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

void lv_tick_task(void *arg);
void guiTask(void *pvParameter);

void create_demo_application(void);
static void demo_event_cb(lv_obj_t * obj, lv_event_t e);
void demo_data_update(int value);


uint8_t get_val_num_bit(int value);
void file_test(SemaphoreHandle_t nrf_sdcard_semaphore, uint32_t num);

#endif

