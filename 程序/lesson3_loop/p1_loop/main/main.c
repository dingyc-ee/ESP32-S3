#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *TAG = "COUNTER";

int count = 0;

void app_main(void)
{
    while (true) {
        ESP_LOGI(TAG, "count = %d", count++);
        /* 延时的毫秒 / 每个Tick的毫秒 = Tick数 */
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
