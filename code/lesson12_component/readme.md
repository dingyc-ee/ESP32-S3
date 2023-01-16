# _Component_

ESP-IDF多文件管理。主要介绍了在ESP32工程中添加多个文件或目录的方法，在大型工程的开发中非常必要。

参考[官方文档链接](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.3/esp32s3/api-guides/build-system.html#id18)的组件CMakeLists文件内容。


## 新建最小工程
* 新建项目
![新建项目](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_new_project_1.jpg)
* 选择路径
![选择路径](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_new_project_2.jpg)
* 选择模板
![选择模板](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_new_project_3.jpg)
* 完成。此时的工程结构如下  
![原始工程结构](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_origin_component.jpg)


## 新建工程组件
* 新建组件  
![新建组件](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_origin_component.jpg)  
* 设置组件名(以debug为例)  
![设置组件名](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_set_component_name.jpg)
* 此时的工程结构  
![添加文件后的工程结构](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_add_component_1.jpg)  
#### _测试代码_
`组件CMakeLists.txt`
```cmake
idf_component_register(SRCS "debug.c"
                    INCLUDE_DIRS "include")
```
`debug.c`
```c
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

void DRV_DEBUG(char *fmt, ...)
{
    va_list args;
    vprintf("<<DBG-MSG>> ", args);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
```
`debug.h`
```c
#ifndef __DEBUG_H__
#define __DEBUG_H__

void DRV_DEBUG(char *fmt, ...);

#endif  /* __DEBUG_H__ */
```
`main.c`
```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "debug.h"

void app_main(void)
{
    int i = 0;

    while (true) {
        DRV_DEBUG("i = %d\n", i++);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
```
#### _测试效果_
![组件1测试结果](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_result_1.jpg)

## 另一种常见的组件方式
![组件布局2](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section12_new_component_2.jpg)