#include "battery.h"

void battery_init(void)
{

    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = CHARGE_SEL | STDBY_SEL,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    printf("yuki: io config\n");
    gpio_config(&io_config);
    printf("yuki: io config finish\n");
}

void battery_detect(void)
{
    gpio_set_pull_mode(CHARGE_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(STDBY_PIN, GPIO_PULLUP_ONLY);
    printf("PULL_UP: Charge: %d, Ready: %d\n", gpio_get_level(CHARGE_PIN), gpio_get_level(STDBY_PIN));
    
    gpio_set_pull_mode(CHARGE_PIN, GPIO_PULLDOWN_ONLY);
    gpio_set_pull_mode(STDBY_PIN, GPIO_PULLDOWN_ONLY);
    printf("PULL_DOWN: Charge: %d, Ready: %d\n", gpio_get_level(CHARGE_PIN), gpio_get_level(STDBY_PIN));
    
    gpio_set_pull_mode(CHARGE_PIN, GPIO_FLOATING);
    gpio_set_pull_mode(STDBY_PIN, GPIO_FLOATING);
    printf("PULL_FLOAT: Charge: %d, Ready: %d\n", gpio_get_level(CHARGE_PIN), gpio_get_level(STDBY_PIN));

    printf("\n");
}

void BatteryTask(void *pvParameter)
{
    battery_init();
    while(1)
    {
        vTaskDelay(100);
        battery_detect();
        /* printf("Charge: %d, Ready: %d\n", gpio_get_level(CHARGE_PIN), gpio_get_level(STDBY_PIN)); */
    }
}



