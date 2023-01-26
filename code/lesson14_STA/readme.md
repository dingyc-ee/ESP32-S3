# _接AP_
### 1. 简介
必须要在WIFI初始化之前，先初始化`nvs`，否则会执行报错。
### 2. 默认EventLoop设置
接入AP会触发事件，这里直接使用的默认的EventLoop。在使用之前，要先把EventTask的任务栈增大，以免事件任务堆栈溢出。
![事件任务栈](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section14_default_event_task.jpg)
### 3. 接入代码
```c
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"

#define TAG     "WIFI"

#define RETRY_MAX       5
#define RGB_PIN         48

// WIFI_EVENT
#define STA_CONNECTED   BIT0
#define STA_FAIL        BIT1

static TaskHandle_t       app_task_hd    = NULL;
static EventGroupHandle_t event_group_hd = NULL;

static int retry_count = 0;

static void event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if ((WIFI_EVENT == event_base) && (WIFI_EVENT_STA_START == event_id)) {
        esp_wifi_connect();
    }
    else if ((WIFI_EVENT == event_base) && (WIFI_EVENT_STA_DISCONNECTED == event_id)) {
        if (retry_count < RETRY_MAX) {
            retry_count++;
            esp_wifi_connect();
        }
        else {
            xEventGroupSetBits(event_group_hd, STA_FAIL);
        }
    }
    else if ((IP_EVENT == event_base) && (IP_EVENT_STA_GOT_IP == event_id)) {
        retry_count = 0;
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(event_group_hd, STA_CONNECTED);
    }
}

void app_task(void *arg)    
{
    esp_event_handler_register("WIFI_EVENT", ESP_EVENT_ANY_ID, event_handler, NULL);
    esp_event_handler_register("IP_EVENT", ESP_EVENT_ANY_ID, event_handler, NULL);

    while (true)
    {
        EventBits_t event_bit = xEventGroupWaitBits(event_group_hd, STA_CONNECTED | STA_FAIL, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bit & STA_CONNECTED) {
            ESP_LOGI(TAG, "Connected!");
        }
        if (event_bit & STA_FAIL) {
            ESP_LOGE(TAG, "Error");
        }
    }
}

void app_main(void)
{
    nvs_flash_init();

    ESP_LOGI(TAG, "1. WiFi初始化阶段");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);
    ESP_LOGI(TAG, "1. APP初始化阶段");
    event_group_hd = xEventGroupCreate();
    xTaskCreate(app_task, "app_task", 1024 * 4, NULL, uxTaskPriorityGet(NULL), &app_task_hd); 

    ESP_LOGI(TAG, "2. WiFi配置阶段");
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t conf = {
        .sta = {
            .ssid = "CMCC-R1s-s4My",
            .password = "XXXXXXXX",
        },
    };
    esp_wifi_set_config(WIFI_IF_STA, &conf);

    ESP_LOGI(TAG, "3. WiFi启动阶段");
    esp_wifi_start();
    vTaskDelete(NULL);
}
```
执行结果：  
![WIFI接入结果](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section14_wifi_connect_result.jpg)


# WIFI事件处理


### 事件描述
<font color='green'>绿色部分表示SDK事件任务默认行为，</font><font color='red'>红色部分表示应用程序通常要执行的动作。</font>


| Event | Description |
|-------|-------------|
| WIFI_EVENT_STA_START | 调用函数 esp_wifi_start() 返回 ESP_OK，产生此事件。<br><font color='green'>事件任务初始化 LwIP 网络接口 (netif)。</font><br><font color='red'>通常，应用程序的事件回调函数需调用 esp_wifi_connect() 来连接已配置的 AP。</font> |
| WIFI_EVENT_STA_STOP | 调用函数 esp_wifi_stop() 到返回 ESP_OK，产生此事件。<br><font color='green'>事件任务将进行释放 IP 地址、终止 DHCP 服务、移除 Socket 并清除 LwIP netif。</font><br><font color='red'>应用程序的事件回调函数通常不需做任何响应。</font> |
| WIFI_EVENT_STA_CONNECTED | 调用函数 esp_wifi_connect() 返回 ESP_OK，且成功连接，产生此事件。<br><font color='green'>事件任务将启动 DHCP 客户端服务并开始获取 IP 地址。此时，Wi-Fi 驱动程序已准备就绪，可发送和接收数据。</font><br><font color='red'>如果您的应用程序不依赖于 LwIP，则此刻便可以开始应用程序开发工作。<br>但是，如果您的应用程序需基于 LwIP 进行，则还需等待 got ip 事件发生后才可开始。</font> |
| WIFI_EVENT_STA_DISCONNECTED | 1. 调用函数 esp_wifi_disconnect()、esp_wifi_stop() 或 esp_wifi_deinit()。<br>2. 调用函数 esp_wifi_connect()，但 Wi-Fi 驱动程序连接失败。<br>3. Wi-Fi 连接因为中断，例如：STA 连续丢失beacon、AP 脱离 station 连接、AP 验证模式改变。<br><font color='green'>事件任务的动作：关闭 STA 的 LwIP netif；通知 LwIP 任务清除所有Socket。</font><br><font color='red'>应用程序通常调用函数 esp_wifi_connect() 重新连接 Wi-Fi。</font> |
| IP_EVENT_STA_GOT_IP | DHCP 客户端成功或 IPV4 地址发生改变时，产生此事件。<br><font color='red'>应用程序一切就绪，可以开始任务（如：创建套接字）。</font> |



