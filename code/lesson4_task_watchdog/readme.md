# 任务看门狗

### 主任务死循环
在app_main任务中死循环
```c
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *TAG = "COUNTER";

int count = 0;

void app_main(void)
{
    while (true) {
        ESP_LOGI(TAG, "count = %d", count++);
    }
}
```
测试结果：触发了任务看门狗
![死循环触发任务看门狗](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section4_esp32-task-watchdog-timeout.png)
触发看门狗的原因：CPU一直在main任务中执行，IDLE空闲任务得不到执行。而IDLE空闲任务中会清除看门狗，因此5秒都一直没清除看门狗，定时器中断到来，触发软件看门狗。

###menuconfig配置软件看门狗
在menuconfig中输入watchdog
![配置任务看门狗](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section4_esp32-task-menuconfig-watchdog.png)
如果勾选了panic handler，则会执行panic动作。默认会打印寄存器然后复位，也可以设置成打印寄存器然后停下来
![panic动作](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section4_esp32-task-menuconfig-watchdog-panic.png)
