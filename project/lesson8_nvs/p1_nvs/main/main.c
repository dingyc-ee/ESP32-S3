#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"

const char *TAG = "NVS";

void app_main(void)
{
    esp_err_t err;
    nvs_handle_t handle;
    const char *name = "storage";
    const char *key  = "counter";
    int32_t counter_value = 0;  // value will default to 0, if not set yet in NVS

    err = nvs_flash_init();
    err = nvs_open(name, NVS_READWRITE, &handle);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "nvs_open %s fail", name);
    }
    err = nvs_get_i32(handle, key, &counter_value);
    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG, "restart_counter is %d", counter_value);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(TAG, "Key \"%s\" is not found", key);
            break;
        default:
            ESP_LOGE(TAG, "Error \"%s\" reading", esp_err_to_name(err));
            break;
    }
    counter_value++;
    nvs_set_i32(handle, key, counter_value);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "nvs_set_i32 key \"%s\" fail", key);
    }

    ESP_LOGI(TAG, "Update NVS...");
    err = nvs_commit(handle);
    ESP_LOGI(TAG, "%s\r\n", (err != ESP_OK) ? "Fail" : "Done");
    nvs_close(handle);
}
