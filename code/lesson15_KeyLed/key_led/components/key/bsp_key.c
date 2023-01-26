#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "bsp_key.h"

static flex_button_t  user_btn;
static button_handler user_btn_event_cb = NULL;

static uint8_t common_btn_read(void *arg)
{
    uint8_t value = 0;
    flex_button_t *btn = (flex_button_t *)arg;

    switch (btn->id)
    {
    case 0:
        value = gpio_get_level(BOOT_PIN_IO);
        break;
    default:
        break;
    }

    return value;
}

static void common_btn_evt_cb(void *arg)
{
    flex_button_t *btn = (flex_button_t *)arg;

    if (user_btn_event_cb) {
        user_btn_event_cb(btn->id, btn->event);
    }
}

static void btn_scan_task(void *arg)
{
    while (true) {
        // 频率为50Hz，循环周期20ms
        flex_button_scan();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

bool button_init(void)
{
    esp_err_t err;

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BOOT_PIN_IO,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    err = gpio_config(&io_conf);
    if (ESP_OK != err) {
        ESP_LOGE("KEY", "io init %s", esp_err_to_name(err));
        return false;
    }

    memset(&user_btn, 0x00, sizeof(flex_button_t));
    user_btn.id                     = BOOT_PIN_IO;
    user_btn.usr_button_read        = common_btn_read;
    user_btn.cb                     = common_btn_evt_cb;
    user_btn.pressed_logic_level    = 0;
    user_btn.short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500);
    user_btn.long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(3000);
    user_btn.long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(4500);

    flex_button_register(&user_btn);
    xTaskCreate(btn_scan_task, "flex_btn", 1024 * 4, NULL, uxTaskPriorityGet(NULL), NULL);
    ESP_LOGI("KEY", "io init");

    return true;
}

bool register_button_handler(button_handler cb)
{
    user_btn_event_cb = cb;
    return true;
}
