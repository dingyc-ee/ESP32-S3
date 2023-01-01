### NVS Blob块存储
#### 1. 演示app_main任务栈溢出
![任务战溢出](picture/main_task_overflow.jpg)
#### 2. 设置app_main任务栈大小
打开menuconfig，输入main，如下图所示
![默认栈大小](picture/main_task_stack.jpg)
默认栈大小为3584字节，这里改为35840字节，重新编译
![默认栈大小](picture/main_task_stack_x10.jpg)
#### 3. Blob存储结果
```c
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#define AP_MAX  40
#define TAG     "NVS"

typedef struct {
    char ssid[33];
    char passwd[34];
} ap_t;

void app_main(void)
{
    size_t length;
    nvs_handle_t handle;
    ap_t ap_write[AP_MAX] = {0};
    ap_t ap_read [AP_MAX] = {0};

    for (int i = 0; i < AP_MAX; i++) {
        strcpy(ap_write[i].ssid, "TPLINKHOST");
        strcpy(ap_write[i].passwd, "4018el9U");
    }

    nvs_flash_init();
    nvs_open("storage", NVS_READWRITE, &handle);
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
```
![执行结果](picture/nvs_blob_result.jpg)