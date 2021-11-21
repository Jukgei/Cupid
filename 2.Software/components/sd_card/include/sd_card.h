#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
// file system interface 
#include "lvgl.h"

/* #define SPI_DMA_CHAN    host.slot */
#define MOUNT_POINT "/s"
#define SPI_DMA_CHAN    2

#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 12
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   25

static const char *TAG = "sd_card";

typedef struct {
    sdmmc_card_t * card;
    sdmmc_host_t host;
}sdcard_config_t;

/* void sd_card_init(); */
sdcard_config_t * sd_card_init(void);
void sd_card_example(sdcard_config_t * sdcard_config);
void sd_card_deinit(sdcard_config_t * sdcard_config);
void lv_interface_test();
void sd_card_release_spi_bus(SemaphoreHandle_t nrf_sdcard_semaphore);
bool sd_card_mount(SemaphoreHandle_t nrf_sdcard_semaphore, TickType_t wait_time);

lv_fs_res_t sd_card_close(lv_fs_drv_t * drv, void * file_p);
void * sd_card_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
/* lv_fs_res_t sd_card_open(lv_fs_drv_t * drv, void * f, const char * path, lv_fs_mode_t mode); */
lv_fs_res_t sd_card_read(lv_fs_drv_t * drv, void * f, void * ptr, uint32_t nmemb, uint32_t * br);
lv_fs_res_t sd_card_write(lv_fs_drv_t * drv, void * f, const void * ptr, uint32_t nmemb, uint32_t * bw);
lv_fs_res_t sd_card_size(lv_fs_drv_t * drv, void * f, uint32_t * size_p);

#endif


