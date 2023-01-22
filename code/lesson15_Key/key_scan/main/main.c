#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp_gpio.h"

void button_process(int btn, int event)
{
    switch (event)
    {
    case FLEX_BTN_PRESS_CLICK:
        ESP_LOGI("IO", "单击");
        break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
        ESP_LOGI("IO", "双击");
        break;
    case FLEX_BTN_PRESS_SHORT_START:
        ESP_LOGI("IO", "短按");
        break;
    case FLEX_BTN_PRESS_LONG_START:
        ESP_LOGI("IO", "长按");
        break;
    default:
        break;
    }
}

void app_main(void)
{
    user_button_init();
    register_button_handler(button_process);

    vTaskDelete(NULL);
}
