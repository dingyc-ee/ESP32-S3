### 测试代码
```c
#include <stdio.h>
#include "esp_log.h"
#include "esp_idf_version.h"
#include "esp_chip_info.h"
#include "esp_spi_flash.h"
#include "esp_mac.h"

#define TAG_HW      "硬件"
#define TAG_SW      "软件"

static char *get_esp_model_string(esp_chip_model_t model)
{
    char *esp_model = NULL;
    switch (model)
    {
    case CHIP_ESP32:
        esp_model = "ESP 32";
        break;
    case CHIP_ESP32S2:
        esp_model = "ESP32 S2";
        break;
    case CHIP_ESP32S3:
        esp_model = "ESP32 S3";
        break;
    case CHIP_ESP32C3:
        esp_model = "ESP32 C3";
        break;
    case CHIP_ESP32H2:
        esp_model = "ESP32 H2";
        break;
    default:
        esp_model = "ESP32 Unknown";
        break;
    }

    return esp_model;
}

void app_main(void)
{
    uint8_t mac[6];
    esp_chip_info_t chip_info;

    ESP_LOGI(TAG_SW, "ESP-IDF version:%s", esp_get_idf_version());

    esp_chip_info(&chip_info);
    ESP_LOGI(TAG_HW, "CPU core:%d", chip_info.cores);
    ESP_LOGI(TAG_HW, "CPU model:%s", get_esp_model_string(chip_info.model));
    ESP_LOGI(TAG_HW, "Embedded flash:%s", chip_info.features & CHIP_FEATURE_EMB_FLASH ? "Yes" : "No");
    ESP_LOGI(TAG_HW, "WiFi:%s", chip_info.features & CHIP_FEATURE_WIFI_BGN ? "Yes" : "No");
    ESP_LOGI(TAG_HW, "Bluetooth Low Energy:%s", chip_info.features & CHIP_FEATURE_BLE ? "Yes" : "No");
    ESP_LOGI(TAG_HW, "Bluetooth:%s", chip_info.features & CHIP_FEATURE_BT ? "Yes" : "No");
    ESP_LOGI(TAG_HW, "Embedded psram:%s", chip_info.features & CHIP_FEATURE_EMB_PSRAM ? "Yes" : "No");
    ESP_LOGI(TAG_HW, "Flash size:%d MB", spi_flash_get_chip_size() / 1024 / 1024);

    esp_efuse_mac_get_default(mac);
    ESP_LOGI(TAG_HW, "MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
```