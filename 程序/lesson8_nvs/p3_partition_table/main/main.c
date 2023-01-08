#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#define AP_MAX  2
#define TAG     "NVS"

typedef struct {
    char ssid[33];
    char passwd[64];
} ap_t;

void app_main(void)
{
    size_t length;
    nvs_handle_t handle;
    ap_t ap_write[AP_MAX] = {0};
    ap_t ap_read [AP_MAX] = {0};

    for (int i = 0; i < AP_MAX; i++) {
        strcpy(ap_write[i].ssid, "TPLINKHOST");
        strcpy(ap_write[i].passwd, "123456789");
    }

    nvs_flash_init_partition("mynvs");
    nvs_open_from_partition("mynvs", "storage", NVS_READWRITE, &handle);
    nvs_set_blob(handle, "ap_info", ap_write, sizeof(ap_write));
    length = sizeof(ap_read);
    nvs_get_blob(handle, "ap_info", ap_read, &length);
    ESP_LOGI(TAG, "AP information list:");
    for (int i = 0; i < AP_MAX; i++) {
        ESP_LOGI(TAG, "AP(%d): ssid:%s passwd:%s", i, ap_read[i].ssid, ap_read[i].passwd);
    }
    nvs_commit(handle);
    nvs_close(handle);
}
