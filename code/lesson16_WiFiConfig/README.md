# _WIFI自动配网_

要实现的功能：

* RGB灯颜色标识当前运行状态
* 自动保存WiFi信息，如SSID和密码
* 上电根据默认WiFi信息连接
* 长按BOOT按键进入自动配网，并保存成功配网的信息
  
### 颜色定义

| 颜色 | 状态 |
|------|------|
| 蓝色 | 未执行接入过程 |
| 红色 | 正在连接或接入失败 |
| 绿色 | 接入成功 |

### 自动保存WIFI信息流程

1. 初始化NVS，打开名为"wifi_storage"的句柄
2. 根据句柄，打开"sta_wifi_config"的blob块，从中读取wifi信息。读取成功，开始连接；读取失败，进入配网模式等待
3. 如果配网成功，将新的网络信息写入"sta_wifi_config"块中，执行重连操作

### 实测代码
```c
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_smartconfig.h"
#include "bsp_led.h"
#include "bsp_btn.h"
#include "flexible_button.h"

#define STA_CONNECTED   BIT0
#define SMART_CFG_DONE  BIT1

static nvs_handle_t  nvs_hd;
static wifi_config_t sta_wifi_conf;

static TaskHandle_t       app_task_hd    = NULL;
static EventGroupHandle_t event_group_hd = NULL;

static void btn_event_handler(int btn, int evt);
static void event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void app_task(void *arg);

void app_main(void)
{
    printf("1. 外设初始化\n");
    led_init();
    btn_init();
    btn_reg_evt_cb(btn_event_handler);
    led_set_color(0, 0, 20);

    printf("2. NVS初始化\n");
    nvs_flash_init();
    
    printf("3. WIFI初始化\n");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);

    printf("3. APP初始化\n");
    event_group_hd = xEventGroupCreate();
    xTaskCreate(app_task, "app_task", 1024 * 4, NULL, uxTaskPriorityGet(NULL), &app_task_hd); 


    printf("4. 配置WIFI\n");
    esp_wifi_set_mode(WIFI_MODE_STA);

    printf("5. 启动WIFI\n");
    led_set_color(20, 0, 0);
    esp_wifi_start();
    vTaskDelete(NULL);
}

static void btn_event_handler(int btn, int evt)
{
    if (FLEX_BTN_PRESS_LONG_START == evt) {
        printf("长按手动进入配网模式...\n");
        // 配网类型：ESPTouch and AirKiss
        esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);
        smartconfig_start_config_t smart_cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
        esp_smartconfig_start(&smart_cfg);
    }
}

static void event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if ((WIFI_EVENT == event_base) && (WIFI_EVENT_STA_START == event_id)) {
        esp_err_t err;
        size_t length = sizeof(wifi_config_t);
        nvs_open("wifi_storage", NVS_READWRITE, &nvs_hd);
        err = nvs_get_blob(nvs_hd, "sta_wifi_conf", &sta_wifi_conf, &length);
        if (ESP_OK == err) {
            printf("找到已保存的Wi-Fi信息 账号:%s 密码:%s 正在接入...\n", sta_wifi_conf.sta.ssid, sta_wifi_conf.sta.password);
            esp_wifi_set_config(WIFI_IF_STA, &sta_wifi_conf);
            esp_wifi_connect();
        }
        if (ESP_ERR_NVS_NOT_FOUND == err) {
            printf("未找到已保存的Wi-Fi信息, 开始自动配网...\n");
            // 配网类型：ESPTouch and AirKiss
            esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);
            smartconfig_start_config_t smart_cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
            esp_smartconfig_start(&smart_cfg);
        }
    }
    else if ((WIFI_EVENT == event_base) && (WIFI_EVENT_STA_DISCONNECTED == event_id)) {
        printf("Wi-Fi断开, 开始重连...\n");
        led_set_color(20, 0, 0);
        esp_wifi_connect();
    }
    else if ((IP_EVENT == event_base) && (IP_EVENT_STA_GOT_IP == event_id)) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("DHCP成功 IP地址:" IPSTR, IP2STR(&event->ip_info.ip));
        led_set_color(0, 20, 0);
        xEventGroupSetBits(event_group_hd, STA_CONNECTED);
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        printf("自动配网:扫描完成\n");;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        printf("自动配网:获取到信道\n");;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        bzero(&sta_wifi_conf, sizeof(wifi_config_t));
        memcpy(sta_wifi_conf.sta.ssid, evt->ssid, sizeof(sta_wifi_conf.sta.ssid));
        memcpy(sta_wifi_conf.sta.password, evt->password, sizeof(sta_wifi_conf.sta.password));
        sta_wifi_conf.sta.bssid_set = evt->bssid_set;
        if (sta_wifi_conf.sta.bssid_set == true) {
            memcpy(sta_wifi_conf.sta.bssid, evt->bssid, sizeof(sta_wifi_conf.sta.bssid));
        }
        printf("自动配网:账号:%s 密码:%s\n", sta_wifi_conf.sta.ssid, sta_wifi_conf.sta.password);
        nvs_set_blob(nvs_hd, "sta_wifi_conf", &sta_wifi_conf, sizeof(wifi_config_t));
        nvs_commit(nvs_hd);
        printf("更新保存Wi-Fi信息\n");
        esp_wifi_disconnect();
        esp_wifi_set_config(WIFI_IF_STA, &sta_wifi_conf);
        esp_wifi_connect();
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(event_group_hd, SMART_CFG_DONE);
    }
}

void app_task(void *arg)    
{
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL);
    esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL);

    while (true)
    {
        EventBits_t event_bit = xEventGroupWaitBits(event_group_hd, STA_CONNECTED | SMART_CFG_DONE, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bit & STA_CONNECTED) {
            printf("Wi-Fi连接成功\n");
        }
        if (event_bit & SMART_CFG_DONE) {
            printf("自动配网成功\n");
            esp_smartconfig_stop();
        }
    }
}
```

### 测试结果

* 裸板未保存WIFI配置启动

![裸板无WIFI配置启动](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section16_empty_cfg.jpg)

* 自动配网过程

![自动配网过程](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section16_smart_config.jpg)

* 保存WIFI配置，上电自动连接
  
![自动读取配置连接WIFI](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section16_saved_cfg.jpg)

* 长按手动配网

![手动配网](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section16_manual_smart_config.jpg)