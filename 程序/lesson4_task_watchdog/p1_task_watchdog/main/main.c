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
    }
}
