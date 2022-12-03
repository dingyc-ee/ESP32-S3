#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *Tag = "COUNTER";

int count;

void app_main(void)
{
    count = 100;
    ESP_LOGI(Tag, "Tick(ms) : %d", portTICK_PERIOD_MS);

    while (true) {
        ESP_LOGI(Tag, "Count value is %d", count++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
