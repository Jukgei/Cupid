#include "sd_card.h"

sdcard_config_t * g_sdcard = NULL;

/* sdcard_config_t * sd_card_init(SemaphoreHandle_t nrf_sdcard_semaphore) */
sdcard_config_t * sd_card_init()
{
    
    esp_err_t ret;
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t* card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    host.slot = SPI3_HOST;
    ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SPI_DMA_CHAN);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return NULL;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return NULL;
    }
    sdcard_config_t * sdcard_config = (sdcard_config_t * )malloc(sizeof(sdcard_config_t));
    sdcard_config->card = card;
    sdcard_config->host = host;
    g_sdcard = sdcard_config;
    return sdcard_config;
}

bool sd_card_mount(SemaphoreHandle_t nrf_sdcard_semaphore, TickType_t wait_time)
{
    if (xQueueSemaphoreTake(nrf_sdcard_semaphore, wait_time) == pdTRUE)
    {
        const char mount_point[] = MOUNT_POINT;
        sdmmc_host_t host = SDSPI_HOST_DEFAULT();
        host.slot = SPI3_HOST;
        
        sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
        slot_config.gpio_cs = PIN_NUM_CS;
        slot_config.host_id = host.slot;
   
        esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
            .format_if_mount_failed = true,
#else
            .format_if_mount_failed = false,
#endif  // EXAMPLE_FORMAT_IF_MOUNT_FAILED
            .max_files = 5,
            .allocation_unit_size = 16 * 1024
        };
    
        sdmmc_card_t* card;
        esp_err_t ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
        sdcard_config_t * sdcard_config = (sdcard_config_t * )malloc(sizeof(sdcard_config_t));
        sdcard_config->card = card;
        sdcard_config->host = host;
        g_sdcard = sdcard_config;
        if (ret == ESP_OK)
            return true;
        else
        {
            printf("sd card mount error\n");
            return false;
        }
    }
    else
        return false;
}

/* void sd_card_release_spi_bus(sdcard_config_t * sdcard_config) */
void sd_card_release_spi_bus(SemaphoreHandle_t nrf_sdcard_semaphore)
{
    sdmmc_card_t * card = g_sdcard->card;
    sdmmc_host_t host = g_sdcard->host;
    
    const char mount_point[] = MOUNT_POINT;
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    free(g_sdcard);
    // TODO: Check if the host need to be free? 
    g_sdcard = NULL;
    xSemaphoreGive(nrf_sdcard_semaphore);
}

void sd_card_deinit(sdcard_config_t * sdcard_config)
{
    sdmmc_card_t * card = sdcard_config->card;
    sdmmc_host_t host = sdcard_config->host;
    
    const char mount_point[] = MOUNT_POINT;
    
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    spi_bus_free((spi_host_device_t)host.slot);
    free(sdcard_config);
}

void * sd_card_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    const char mount_point[] = MOUNT_POINT;
    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "[LVGL interface] Opening file");
    char fpath[256];
    sprintf(fpath, "/s/%s", path);
    FILE * fp;
    if (mode == LV_FS_MODE_WR) 
        fp = fopen(fpath, "w");
    else if(mode == LV_FS_MODE_RD)
        fp = fopen(fpath, "r");
    else if(mode == (LV_FS_MODE_RD|LV_FS_MODE_WR))
        fp = fopen(fpath, "r+");
    else
        fp = fopen(fpath, "r+");

    if (fp == NULL) {
        ESP_LOGE(TAG, "[LVGL interface] Failed to open file for writing");
        return fp;
    }
    fseek(fp, 0, SEEK_SET);
    ESP_LOGI(TAG, "[LVGL interface] Open file success");
    return fp;
}

lv_fs_res_t sd_card_close(lv_fs_drv_t * drv, void * file_p)
{
    fclose(file_p);
    return LV_FS_RES_OK;
}

lv_fs_res_t sd_card_read(lv_fs_drv_t * drv, void * f, void * ptr, uint32_t nmemb, uint32_t * br)
{
    *br = fread(ptr, 1, nmemb, f);
    return LV_FS_RES_OK;
}

lv_fs_res_t sd_card_write(lv_fs_drv_t * drv, void * f, const void * ptr, uint32_t nmemb, uint32_t * bw)
{
    *bw = fwrite(ptr, 1, nmemb, f);
    return LV_FS_RES_OK;
}

lv_fs_res_t sd_card_size(lv_fs_drv_t * drv, void * f, uint32_t * size_p)
{
    fseek(f, 0, SEEK_END);
    *size_p = ftell(f);
    return LV_FS_RES_OK;
}

void lv_interface_test()
{
/* static void * sd_card_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode) */
    /* FILE f; */
    FILE * f = sd_card_open(NULL, "pic_1.png", LV_FS_MODE_RD);
    /* FILE * f = sd_card_open(NULL,"s/FOO1.TXT", LV_FS_MODE_RD); */
    if (f == NULL)
    {
        printf("yuki: f is NULL\n");
        return;
    }
    
    uint32_t size = -1;
    sd_card_size(NULL, f, &size);
    
    printf("png_decoder_test size is %d", size);
    sd_card_close(NULL, f);
}

void sd_card_example(sdcard_config_t * sdcard_config)
{
    
    sdmmc_card_t * card = sdcard_config->card;
    sdmmc_host_t host = sdcard_config->host;

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    const char mount_point[] = MOUNT_POINT;
    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    /* FILE* f = fopen(MOUNT_POINT"/hello.txt", "w"); */
    FILE* f = fopen("/s/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello %s!\n", card->cid.name);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    // Check if destination file exists before renaming
    struct stat st;
    if (stat(MOUNT_POINT"/foo1.txt", &st) == 0) {
        // Delete it if it exists
        unlink(MOUNT_POINT"/foo1.txt");
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename(MOUNT_POINT"/hello.txt", MOUNT_POINT"/foo1.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen(MOUNT_POINT"/foo1.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char* pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    // All done, unmount partition and disable SDMMC or SPI peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    spi_bus_free(host.slot);
    free(sdcard_config);
}
