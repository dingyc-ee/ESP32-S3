#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "flexible_button.h"
#include "bsp_btn.h"

#define BOOT_PIN_IO     GPIO_NUM_0

static flex_button_t  btn_hd;
static btn_evt_cb btn_cb = NULL;

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

    if (btn_cb) {
        btn_cb(btn->id, btn->event);
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

bool btn_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BOOT_PIN_IO,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    memset(&btn_hd, 0x00, sizeof(flex_button_t));
    btn_hd.id                     = BOOT_PIN_IO;
    btn_hd.usr_button_read        = common_btn_read;
    btn_hd.cb                     = common_btn_evt_cb;
    btn_hd.pressed_logic_level    = 0;
    btn_hd.short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500);
    btn_hd.long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(3000);
    btn_hd.long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(4500);

    flex_button_register(&btn_hd);
    xTaskCreate(btn_scan_task, "flex_btn", 1024 * 4, NULL, uxTaskPriorityGet(NULL), NULL);

    return true;
}

bool btn_reg_evt_cb(btn_evt_cb cb)
{
    btn_cb = cb;
    return true;
}
