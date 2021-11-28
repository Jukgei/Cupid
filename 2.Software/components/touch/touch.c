#include "touch.h"

#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

void touch_init(void)
{
    ESP_ERROR_CHECK(touch_pad_init());

    touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V8, TOUCH_HVOLT_ATTEN_1V5);
    /* touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V5); */

    touch_pad_config(KEY_CONFIRM, 0);
    
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);

}

void TouchTask(void *pvParameter)
{
    touch_init();
    printf("yuki: before sleep\n ");
    touch_wakeup_init();

    uint16_t touch_value;
    uint16_t touch_filter_value;
    while(1)
    {
        
        touch_pad_read_raw_data(KEY_CONFIRM, &touch_value);
        touch_pad_read_filtered(KEY_CONFIRM, &touch_filter_value);
        printf("T%d:[%4d,%4d] ", 0, touch_value, touch_filter_value);
        printf("\n");
        vTaskDelay(20);
    }
}

void touch_wakeup_init(void)
{
    AllModuleLowPower();
    // Initialize touch pad peripheral.
    // The default fsm mode is software trigger mode.
    ESP_ERROR_CHECK(touch_pad_init());
    // If use touch pad wake up, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    /* touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER); */
    // Set reference voltage for charging/discharging
    // In this case, the high reference valtage will be 2.4V - 1V = 1.4V
    // The low reference voltage will be 0.5
    // The larger the range, the larger the pulse count value.
    /* touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V8, TOUCH_HVOLT_ATTEN_0V5); */
    //init RTC IO and mode for touch pad.
    /* touch_pad_config(KEY_CONFIRM, 450); */

    /* esp_sleep_enable_touchpad_wakeup(); */
    /* esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF); */

    /* esp_deep_sleep_start(); */
    printf("yuki: sleep\n");
}

void AllModuleLowPower()
{
    
    gpio_config_t o_config = {
        .mode = GPIO_MODE_DISABLE,
        /* .mode = GPIO_MODE_OUTPUT, */
        .pin_bit_mask = ((1ULL<<13)|(1ULL<<12)|(1ULL<<25)|(1ULL<<32)|(1ULL<<0)) , 
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&o_config);

    /* gpio_set_level(13, 1); */
    /* gpio_set_level(12, 1); */
    /* gpio_set_level(0, 0); */
}

