#include "gui.h"

lv_obj_t * ltr_label;
int cnt = 1;

void Gui_Task()
{

}

void lv_file_system_init()
{
    lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);
    
    drv.open_cb = sd_card_open;
    drv.letter = 's';
    drv.close_cb = sd_card_close;
    drv.read_cb = sd_card_read;
    drv.write_cb = sd_card_write;
    drv.size_cb = sd_card_size;
    /* if (drv.open_cb == NULL) */
    /*     printf("drv open_cb is NULL\n"); */

    lv_fs_drv_register(&drv);
}

void guiTask(void *pvParameter) 
{

    /* (void) pvParameter; */
    SemaphoreHandle_t nrf_sdcard_semaphore = (SemaphoreHandle_t )pvParameter; 
    xGuiSemaphore = xSemaphoreCreateMutex();
    
    /* lv_file_system_init(); */

    lv_init();
    
    // must be called after lv_init()
    /* lv_file_system_init(); */
    
    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();
    
    lv_color_t* buf1 = (lv_color_t * )heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t* buf2 = (lv_color_t * )heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create the demo application */
    /* create_demo_application(); */

    lv_png_init();
    /* LV_IMG_DECLARE(png_decoder_test); */
    lv_obj_t *img = lv_img_create(lv_scr_act(), NULL);
    /* lv_img_set_src(img, &png_decoder_test); */
    lv_img_set_src(img,"/s/pic_1.png");
    bool isDisplay = false;
    bool spi_already_release = false;
    /* sd_card_release_spi_bus(); */
    /* xSemaphoreGive(nrf_sdcard_semaphore); */
    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));
        if (isDisplay && spi_already_release == false)
        {
            sd_card_release_spi_bus(nrf_sdcard_semaphore);
            printf("yuki: first release\n");
            isDisplay = false;
            spi_already_release = true;
        }

        file_test(nrf_sdcard_semaphore, cnt);
        /* demo_data_update(cnt); */
        cnt++;
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            /* printf("yuki: ui tick\n"); */
            lv_task_handler();
            isDisplay = true;
            xSemaphoreGive(xGuiSemaphore);

       }
    }

    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    vTaskDelete(NULL);
}

void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}


void create_demo_application(void)
{
    /* ltr_label = lv_label_create(lv_scr_act(),NULL); */
    /* lv_label_set_recolor(ltr_label, true); */
    /* lv_label_set_text(ltr_label, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label "); */
    /* lv_obj_set_width(ltr_label, 310); */
    /* lv_obj_set_event_cb(ltr_label, demo_event_cb); */ 

    lv_obj_t * par = lv_obj_create(lv_scr_act(), NULL);
    /* lv_obj_set_size(par, 240, 135); */
    lv_obj_set_size(par, 135, 240);
    lv_color_t c = LV_COLOR_WHITE;
    /* c.full = ~c.full; */
    /* printf("color is 0x%x\n", c.full); */
    lv_obj_set_style_local_bg_color(par, LV_BAR_PART_BG, LV_STATE_DEFAULT, c);
    /* lv_obj_align(ltr_label, LV_ALIGN_TOP_LEFT, 5, 5); */
}

static void demo_event_cb(lv_obj_t * obj, lv_event_t e)
{
    int * cnt = (int * )lv_event_get_data(); 
    /* ESP_LOGI(TAG, "yuki: demo event cb cnt is %d", *cnt); */
    
    uint8_t bit_num = get_val_num_bit(*cnt);
    /* ESP_LOGI(TAG, "yuki: bit_num is  %d", bit_num); */
    char * ss = (char *)malloc(sizeof(char) * (bit_num + 1));
    memset(ss, 0, bit_num+1);
    while(*cnt != 0 )
    {
        /* ESP_LOGI(TAG, "yuki: cnt %d", *cnt); */
        char c = (*cnt % 10) + '0';
        ss[bit_num - 1] = c;
        bit_num -= 1;
        *cnt /= 10;
        /* ESP_LOGI(TAG, "yuki: cnt %d", *cnt); */
        /* ESP_LOGI(TAG, "yuki: bit_num %d", bit_num); */
    }
    
    lv_label_set_text(obj, ss);
    free(ss);
}

void demo_data_update(int value)
{
    /* ESP_LOGI(TAG, "yuki: send event %d", value); */
    lv_event_send(ltr_label, LV_EVENT_VALUE_CHANGED, &value);
}

void file_test(SemaphoreHandle_t nrf_sdcard_semaphore, uint32_t num)
{
    /* if (sd_card_mount(nrf_sdcard_semaphore, portMAX_DELAY) == true) */
    if (sd_card_mount(nrf_sdcard_semaphore, 1) == true)
    {
        /* printf("sd card get lock"); */
        FILE* f = fopen("/s/hello.txt", "a+");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return;
        }
        fprintf(f, "Hello %d!\n", num);
        fclose(f);
        ESP_LOGI(TAG, "File written");

        sd_card_release_spi_bus(nrf_sdcard_semaphore);
        /* printf("sd card release lock\n"); */
    }
    /* else */
    /*     printf("sd card cannot get lock\n"); */
}

uint8_t get_val_num_bit(int value)
{
    if (value == 0)
        return 1;

    uint8_t ret = 0;
    while(value != 0)
    {
        value /= 10;
        ret++;
    }
    return ret;
}


