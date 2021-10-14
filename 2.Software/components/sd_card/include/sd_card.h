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

/* #define SPI_DMA_CHAN    host.slot */
#define MOUNT_POINT "/sdcard"
#define SPI_DMA_CHAN    1

#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

static const char *TAG = "sd_card";

/* void sd_card_init(); */
sdmmc_card_t * sd_card_init(void);
void sd_card_example(sdmmc_card_t * card);
#endif


