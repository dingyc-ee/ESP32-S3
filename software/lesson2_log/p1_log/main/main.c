#include <stdio.h>
#include "esp_log.h"

const char *TAG = "ding";

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);

    ESP_LOGE(TAG, "this is error message");
    ESP_LOGW(TAG, "this is warning message");
    ESP_LOGI(TAG, "this is information message");
    ESP_LOGD(TAG, "this is debug message");
    ESP_LOGV(TAG, "this is verbose message");
}
