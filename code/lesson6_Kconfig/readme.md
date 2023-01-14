# ESP32-Kconfig配置
#### 1. 新建Kconfig.projbuild文件
![新建Kconfig.projbuild文件](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-new.png)
####2. 写入配置项
1. 新建menu
```
menu "点灯配置"

endmenu
```
运行 idf.py menuconfig，结果如下
powershel
![powershell](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-powershell.png)
vscode
![vscode](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-vscode.png)
可以看到，power shell中文支持上有乱码，所以接下来选择英文
2. 配置menu，新增3个配置项
```
menu "WS2812"

	config LED_GPIO_NUM
		int "gpio pin"
		prompt "set esp32s3 board ws2812 GPIO"
		range 0 48
		default 48
		help
			This is help message
	
	config LED_RMT_CHANNEL
		int "channel"
		prompt "set esp32s3 ws2812 pwm channel"
		range 0 3
		default 0
		help
			This is help message
	
	config DELAY_TIME_MS
		int "period(ms)"
		prompt "set esp32s3 ws2812 blink period"
		range 0 2000
		default 1000
		help
			This is help message

endmenu
```
运行 idf.py menuconfig，结果如下
![powershell](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-powershell-result1.png)
进入配置项
![powershell](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-powershell-result2.png)
按？查看help帮助信息
![powershell](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-powershell-result3.png)
![powershell](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-powershell-result4.png)
修改闪烁周期值
![powershell](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-powershell-result5.png)
按enter 修改完成
![powershell](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-powershell-result6.png)
运行vscode，结果如下
![vscode](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-kconfig-vscode-result.png)
### 3. 编译
1. 查看sdkconfig文件，已经写入了3个宏定义
![sdkconfig](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-sdkconfig.png)
那么在C代码中就要替换这几个宏定义
```c
/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "BLINK";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/

static uint8_t s_led_state = 0;

static led_strip_t *pStrip_a;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        pStrip_a->refresh(pStrip_a, 100);
    } else {
        /* Set all LED off to clear all pixels */
        pStrip_a->clear(pStrip_a, 50);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(CONFIG_LED_RMT_CHANNEL, CONFIG_LED_GPIO_NUM, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led();

    while (1) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_DELAY_TIME_MS / portTICK_PERIOD_MS);
    }
}
```
运行结果：
![运行结果](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section6_esp32-sdkconfig-result1.png)
