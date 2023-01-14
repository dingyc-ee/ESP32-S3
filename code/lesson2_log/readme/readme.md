# ESP32 日志  
### UART配置  
1. 新建工程
查看->命令面板，输入esp-idf:new project，一路完成工程创建
2. 选择menuconfig，输入UART
![选择menuconfig](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig.png)
3. 输入UART
![配置UART串口参数](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig-uart.png)
4. 自定义波特率改为：460800
![设置monitor波特率，和硬件波特率](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig-uart-460800.png)

### 日志打印  
1. ESP32的日志打印函数
ESP_LOGx分成5个等级：error warning info debug verbose，使用前先定义字符串TAG
2. 测试代码
```c
#include <stdio.h>
#include "esp_log.h"

const char *TAG = "ding";

void app_main(void)
{
    ESP_LOGE(TAG, "this is error message");
    ESP_LOGW(TAG, "this is warning message");
    ESP_LOGI(TAG, "this is information message");
    ESP_LOGD(TAG, "this is debug message");
    ESP_LOGV(TAG, "this is verbose message");
}
```
3. 实际打印效果
Monitor
![实际效果展示](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig-uart-460800-print2.png)
SecureCRT
![实际效果展示](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig-uart-460800-print.png)


### 动态调整日志等级  
如果希望在使用时能够动态调整日志等级，需要先在menuconfig中配置最大的日志等级
1. 配置APP最大日志等级
选择menuconfig，输入LOG
![配置日志](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig-log.png)

设置最大日志等级为Verbose(详细)
![设置日志等级](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig-log-level.png)
2. 修改代码：在APP启动时日志等级改为最高Verbose
使用`esp_log_level_set()`接口
```c
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
```
3. 实际效果
![动态调整日志格式](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section2_esp32-menuconfig-log-level-verbose.png)