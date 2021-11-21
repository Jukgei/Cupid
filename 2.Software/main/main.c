/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
/* #include "sdkconfig.h" */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sd_card.h"
#include "gui.h"
#include "nrf24l01.h"

SemaphoreHandle_t nrf_sdcard_semaphore;

void app_main(void)
{
    printf("Hello world!\n");
    /* SemaphoreHandle_t nrf_sdcard_semaphore; */
    nrf_sdcard_semaphore = xSemaphoreCreateBinary();
    sdcard_config_t * sdcard = sd_card_init();
   
    /* xSemaphoreGive(nrf_sdcard_semaphore); */
    /* lv_interface_test(); */
    /* sd_card_open(); */

    /* sd_card_deinit(sdcard); */
    /* sd_card_example(sd_card_init()); */

    xTaskCreatePinnedToCore(guiTask, "guiTask", 1024*60, (void *)&nrf_sdcard_semaphore, 1, NULL, 1);
    xTaskCreatePinnedToCore(nrf_task, "nrf_task", 1024*2, (void *)&nrf_sdcard_semaphore, 1, NULL, 1);
    /* nrf_example(); */
    /* Print chip information */
    /* esp_chip_info_t chip_info; */
    /* esp_chip_info(&chip_info); */
    /* printf("This is %s chip with %d CPU cores, WiFi%s%s, ", */
    /*         CONFIG_IDF_TARGET, */
    /*         chip_info.cores, */
    /*         (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "", */
    /*         (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : ""); */

    /* printf("silicon revision %d, ", chip_info.revision); */

    /* printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024), */
    /*         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external"); */

    /* printf("Free heap: %d\n", esp_get_free_heap_size()); */

    /* for (int i = 10; i >= 0; i--) { */
    /*     printf("Restarting in %d seconds...\n", i); */
    /*     vTaskDelay(1000 / portTICK_PERIOD_MS); */
    /* } */
    /* printf("Restarting now.\n"); */
    /* fflush(stdout); */
    /* esp_restart(); */
}
