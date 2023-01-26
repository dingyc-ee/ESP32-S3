#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "main.h"

static uint8_t color = 0;
static uint8_t light = 20;

static void button_process(int btn, int event)
{
    switch (event)
    {
    case FLEX_BTN_PRESS_CLICK:
        ESP_LOGI("IO", "单击:切换颜色");
        color = (color + 1) % 4;
        break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
        ESP_LOGI("IO", "双击:切换亮度");
        light = ((light + 50) > 250) ? 50 : (light + 50);
        break;
    case FLEX_BTN_PRESS_SHORT_START:
        ESP_LOGI("IO", "短按:关灯");
        color = 0;
        light = 20;
        break;
    default:
        break;
    }

    if (0 == color || 0 == light) {
        led_set_color(0, 0, 0);
    }
    else {
        switch (color)
        {
        case 1:
            led_set_color(light, 0, 0);
            break;
        case 2:
            led_set_color(0, light, 0);
            break;
        case 3:
            led_set_color(0, 0, light);
            break;
        default:
            break;
        }
    }
}

void app_main(void)
{
    led_init();
    button_init();
    register_button_handler(button_process);
    ESP_LOGI("IO", "功能菜单 | 单击:切换颜色 | 双击:切换亮度 | 短按:关灯");

    vTaskDelete(NULL);
}
