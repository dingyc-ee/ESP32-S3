#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"

#define MAX_SCAN_NUM    30

void wifi_scan_start(void)
{
    wifi_country_t ccode_config = {
        .cc = "CN",
        .schan = 1,
        .nchan = 13,
        .policy = WIFI_COUNTRY_POLICY_AUTO,
    };
    printf("WiFi scan start...\n");
    esp_wifi_set_country(&ccode_config);
    esp_wifi_scan_start(NULL, true);
}

void wifi_scan_result(void)
{
    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    uint16_t number = MAX_SCAN_NUM;
    wifi_ap_record_t ap_records[MAX_SCAN_NUM];
    memset(ap_records, 0x00, sizeof(ap_records));
    esp_wifi_scan_get_ap_records(&number, ap_records);
    printf("Scan %d AP\n", ap_count);
    printf("%-5s %-30s %-10s %-5s %-5s\n", "seq", "ssid", "channel", "rssi", "mac");
    for (int i = 0; i < number; i++) {
        printf("%-5d %-30s %-10d %-5d %02x:%02x:%02x:%02x:%02x:%02x\n", i, ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi,
            ap_records[i].bssid[0], ap_records[i].bssid[1], ap_records[i].bssid[2], ap_records[i].bssid[3], ap_records[i].bssid[4], ap_records[i].bssid[5]);
    }
    printf("WiFi scan done!\n");
}

void scan_start_task(void *argv)
{
    wifi_scan_start();
    vTaskDelete(NULL);
}

void scan_result_task(void *argv)
{
    wifi_scan_result();
    vTaskDelete(NULL);
}

void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
{
    switch (id) {
        case WIFI_EVENT_SCAN_DONE:
            ESP_LOGE("EVENT_HANDLE", "WIFI_EVENT_SCAN_DONE");
            xTaskCreate(scan_result_task, "scan_result_task", 1024 * 12, NULL, 1, NULL);
            break;
        case WIFI_EVENT_STA_START:
            ESP_LOGE("EVENT_HANDLE", "WIFI_EVENT_STA_START");
            xTaskCreate(scan_start_task, "scan_start_task", 1024 * 12, NULL, 1, NULL);
            break;
        default:
            break;
    }
}

void app_task(void *argv)
{
    ESP_LOGI("APP_TASK", "APP TASK创建完成");
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, run_on_event, NULL);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    ESP_LOGI("MAIN_TASK", "0. 初始化NVS存储");
    nvs_flash_init();

    ESP_LOGI("MAIN_TASK", "1. WiFi初始化阶段");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);

    xTaskCreate(app_task, "app_task", 1024 * 12, NULL, 1, NULL);

    ESP_LOGI("MAIN_TASK", "2. WiFi配置阶段");
    esp_wifi_set_mode(WIFI_MODE_STA);
    
    ESP_LOGI("MAIN_TASK", "3. WiFi启动阶段");
    esp_wifi_start();

    vTaskDelete(NULL);
}
