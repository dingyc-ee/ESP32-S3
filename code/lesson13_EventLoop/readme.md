# _EventLoop_  
### 1. EventLoop介绍  
在之前WIFI扫描章节中，已经介绍了Event的简单使用([使用Event处理WIFI扫描](https://github.com/dingyc-ee/ESP32-S3/tree/main/code/lesson11_event))。现在介绍下EventLoop的通用API。 
| 用户事件循环 | 默认事件循环 |  
| -| - |  
| esp_event_loop_create() | esp_event_loop_create_default() |  
| esp_event_handler_register_with() | esp_event_handler_register() |  
| esp_event_post_to() | esp_event_post() |  
* esp_event_loop_create  
源码分析：EventLoop实际上是一个消息队列+任务+事件链表，类似于WorkerThread的概念。创建EventLoop就是初始化对应的队列、互斥量和任务。同时初始化事件链表，
```c
esp_err_t esp_event_loop_create(const esp_event_loop_args_t* event_loop_args, esp_event_loop_handle_t* event_loop)
{
    esp_event_loop_instance_t* loop;
    esp_err_t err = ESP_ERR_NO_MEM; // most likely error

    loop = calloc(1, sizeof(*loop));
    if (loop == NULL) {
        ESP_LOGE(TAG, "alloc for event loop failed");
        return err;
    }

    loop->queue = xQueueCreate(event_loop_args->queue_size , sizeof(esp_event_post_instance_t));
    if (loop->queue == NULL) {
        ESP_LOGE(TAG, "create event loop queue failed");
        goto on_err;
    }

    loop->mutex = xSemaphoreCreateRecursiveMutex();
    if (loop->mutex == NULL) {
        ESP_LOGE(TAG, "create event loop mutex failed");
        goto on_err;
    }

    // 创建循环链表，注册事件会添加节点到链表上，删除事件会从链表上摘除节点
    SLIST_INIT(&(loop->loop_nodes));

    // Create the loop task if requested
    if (event_loop_args->task_name != NULL) {
        BaseType_t task_created = xTaskCreatePinnedToCore(esp_event_loop_run_task, event_loop_args->task_name,
                    event_loop_args->task_stack_size, (void*) loop,
                    event_loop_args->task_priority, &(loop->task), event_loop_args->task_core_id);

        if (task_created != pdPASS) {
            ESP_LOGE(TAG, "create task for loop failed");
            err = ESP_FAIL;
            goto on_err;
        }

        loop->name = event_loop_args->task_name;

        ESP_LOGD(TAG, "created task for loop %p", loop);
    } else {
        loop->name = "";
        loop->task = NULL;
    }

    loop->running_task = NULL;

    *event_loop = (esp_event_loop_handle_t) loop;

    ESP_LOGD(TAG, "created event loop %p", loop);

    return ESP_OK;
}
```
* esp_event_loop_run_task  
事件循环处理函数源码分析：永久等待消息队列。如果获取到消息，就在链表中寻找，如果找到匹配的base或id，就开始执行节点对应的回调函数。如果没找到匹配项，就答应错误日志。  
```c
static void esp_event_loop_run_task(void* args)
{
    esp_err_t err;
    esp_event_loop_handle_t loop = (esp_event_loop_handle_t) args;
    esp_event_post_instance_t post;

    ESP_LOGD(TAG, "running task for loop %p", loop);

    while(1) {
        while(xQueueReceive(loop->queue, &post, ticks_to_run) == pdTRUE) {
            ``// The event has already been unqueued, so ensure it gets executed.
            xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

            loop->running_task = xTaskGetCurrentTaskHandle();

            bool exec = false;

            esp_event_handler_node_t *handler, *temp_handler;
            esp_event_loop_node_t *loop_node, *temp_node;
            esp_event_base_node_t *base_node, *temp_base;
            esp_event_id_node_t *id_node, *temp_id_node;

            SLIST_FOREACH_SAFE(loop_node, &(loop->loop_nodes), next, temp_node) {
                // Execute loop level handlers
                SLIST_FOREACH_SAFE(handler, &(loop_node->handlers), next, temp_handler) {
                    handler_execute(loop, handler, post);
                    exec |= true;
                }

                SLIST_FOREACH_SAFE(base_node, &(loop_node->base_nodes), next, temp_base) {
                    if (base_node->base == post.base) {
                        // Execute base level handlers
                        SLIST_FOREACH_SAFE(handler, &(base_node->handlers), next, temp_handler) {
                            handler_execute(loop, handler, post);
                            exec |= true;
                        }

                        SLIST_FOREACH_SAFE(id_node, &(base_node->id_nodes), next, temp_id_node) {
                            if (id_node->id == post.id) {
                                // Execute id level handlers
                                SLIST_FOREACH_SAFE(handler, &(id_node->handlers), next, temp_handler) {
                                    handler_execute(loop, handler, post);
                                    exec |= true;
                                }
                                // Skip to next base node
                                break;
                            }
                        }
                    }
                }
            }

            esp_event_base_t base = post.base;
            int32_t id = post.id;

            post_instance_delete(&post);

            if (ticks_to_run != portMAX_DELAY) {
                end = xTaskGetTickCount();
                remaining_ticks -= end - marker;
                // If the ticks to run expired, return to the caller
                if (remaining_ticks <= 0) {
                    xSemaphoreGiveRecursive(loop->mutex);
                    break;
                } else {
                    marker = end;
                }
            }

            loop->running_task = NULL;

            xSemaphoreGiveRecursive(loop->mutex);

            if (!exec) {
                // No handlers were registered, not even loop/base level handlers
                ESP_LOGD(TAG, "no handlers have been registered for event %s:%d posted to loop %p", base, id, event_loop);
            }
        }
    }

    ESP_LOGE(TAG, "suspended task for loop %p", event_loop);
    vTaskSuspend(NULL);
}
```
* esp_event_loop_create_default  
源码分析：实际上是创建了sys_evt的任务，这个就非常熟悉了。
```c
esp_err_t esp_event_loop_create_default(void)
{
    esp_event_loop_args_t loop_args = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "sys_evt",
        .task_stack_size = ESP_TASKD_EVENT_STACK,
        .task_priority = ESP_TASKD_EVENT_PRIO,
        .task_core_id = 0
    };

    esp_err_t err;

    err = esp_event_loop_create(&loop_args, &s_default_loop);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}
```
* esp_event_handler_register_with  
源码分析：创建节点，包含的内容为base、id、handler回调函数，然后把节点添加进循环链表。如果`esp_event_loop_run_task()`获取到消息，就会在链表中查询base、id等参数，然后执行handler回调函数。
```c
esp_err_t esp_event_handler_register_with_internal(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                          int32_t event_id, esp_event_handler_t event_handler, void* event_handler_arg,
                                          esp_event_handler_instance_context_t** handler_ctx_arg, bool legacy)
{
    assert(event_loop);
    assert(event_handler);

    if (event_base == ESP_EVENT_ANY_BASE && event_id != ESP_EVENT_ANY_ID) {
        ESP_LOGE(TAG, "registering to any event base with specific id unsupported");
        return ESP_ERR_INVALID_ARG;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    if (event_base == ESP_EVENT_ANY_BASE) {
        event_base = esp_event_any_base;
    }

    esp_err_t err = ESP_OK;

    xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

    esp_event_loop_node_t *loop_node = NULL, *last_loop_node = NULL;

    SLIST_FOREACH(loop_node, &(loop->loop_nodes), next) {
        last_loop_node = loop_node;
    }

    bool is_loop_level_handler = (event_base == esp_event_any_base) && (event_id == ESP_EVENT_ANY_ID);

    if (!last_loop_node ||
       (last_loop_node && !SLIST_EMPTY(&(last_loop_node->base_nodes)) && is_loop_level_handler)) {
        loop_node = (esp_event_loop_node_t*) calloc(1, sizeof(*loop_node));

        if (!loop_node) {
            ESP_LOGE(TAG, "alloc for new loop node failed");
            err = ESP_ERR_NO_MEM;
            goto on_err;
        }

        SLIST_INIT(&(loop_node->handlers));
        SLIST_INIT(&(loop_node->base_nodes));

        err = loop_node_add_handler(loop_node, event_base, event_id, event_handler, event_handler_arg, handler_ctx_arg, legacy);

        if (err == ESP_OK) {
            if (!last_loop_node) {
                SLIST_INSERT_HEAD(&(loop->loop_nodes), loop_node, next);
            }
            else {
                SLIST_INSERT_AFTER(last_loop_node, loop_node, next);
            }
        } else {
            free(loop_node);
        }
    }
    else {
        err = loop_node_add_handler(last_loop_node, event_base, event_id, event_handler, event_handler_arg, handler_ctx_arg, legacy);
    }

on_err:
    xSemaphoreGiveRecursive(loop->mutex);
    return err;
}
```
* esp_event_post_to  
源码分析：就是向消息队列发送消息，没了。该消息会被其他线程的`esp_event_loop_run_task()`捕获，然后执行消息处理函数。
```c
esp_err_t esp_event_post_to(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id,
                            void* event_data, size_t event_data_size, TickType_t ticks_to_wait)
{
    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    esp_event_post_instance_t post;
    memset((void*)(&post), 0, sizeof(post));

    if (event_data != NULL && event_data_size != 0) {
        // Make persistent copy of event data on heap.
        void* event_data_copy = calloc(1, event_data_size);

        if (event_data_copy == NULL) {
            return ESP_ERR_NO_MEM;
        }

        memcpy(event_data_copy, event_data, event_data_size);
    }
    post.base = event_base;
    post.id = event_id;

    BaseType_t result = pdFALSE;

    // Find the task that currently executes the loop. It is safe to query loop->task since it is
    // not mutated since loop creation. ENSURE THIS REMAINS TRUE.
    if (loop->task == NULL) {
        // The loop has no dedicated task. Find out what task is currently running it.
        result = xSemaphoreTakeRecursive(loop->mutex, ticks_to_wait);

        if (result == pdTRUE) {
            if (loop->running_task != xTaskGetCurrentTaskHandle()) {
                xSemaphoreGiveRecursive(loop->mutex);
                result = xQueueSendToBack(loop->queue, &post, ticks_to_wait);
            } else {
                xSemaphoreGiveRecursive(loop->mutex);
                result = xQueueSendToBack(loop->queue, &post, 0);
            }
        }
    } else {
        // The loop has a dedicated task.
        if (loop->task != xTaskGetCurrentTaskHandle()) {
            result = xQueueSendToBack(loop->queue, &post, ticks_to_wait);
        } else {
            result = xQueueSendToBack(loop->queue, &post, 0);
        }
    }

    return ESP_OK;
}
```
### 2. 测试源码  
*注意：使用`esp_event_handler_register_with()`注册多个事件时，必须每个事件单独注册，不能采用或的方式，不然会无法执行。其原因是，事件任务是根据ID值相等来匹配具体事件的。*
```c
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define EVENT_UP    BIT0
#define EVENT_DOWN  BIT1

esp_event_base_t loop_base = "ding_event_loop_base";
esp_event_loop_handle_t loop_handle = NULL;

void ding_event_loop_handle(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    printf("event_base:%s event_id:%d, event_data:%s\n", event_base, event_id, (char *)event_data);
}

void app_main(void)
{
    esp_event_loop_args_t loop_args = {
        .queue_size = 20,
        .task_name = "ding_event_loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 4 * 1024,
        /**
         * @param xCoreID If the value is tskNO_AFFINITY, the created task is not
         * pinned to any CPU, and the scheduler can run it on any core available.
         * Values 0 or 1 indicate the index number of the CPU which the task should
         * be pinned to. Specifying values larger than (portNUM_PROCESSORS - 1) will
         * cause the function to fail.
         */
        .task_core_id = tskNO_AFFINITY,
    };

    esp_event_loop_create(&loop_args, &loop_handle);
    // 这里必须要每个事件单独注册，不能 EVENT_UP | EVENT_DOWN 这种形式
    esp_event_handler_register_with(loop_handle, loop_base, EVENT_UP, ding_event_loop_handle, NULL);
    esp_event_handler_register_with(loop_handle, loop_base, EVENT_DOWN, ding_event_loop_handle, NULL);

    while (1) {
        esp_event_post_to(loop_handle, loop_base, EVENT_UP, "event_up", strlen("event_up") + 1, portMAX_DELAY);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        esp_event_post_to(loop_handle, loop_base, EVENT_DOWN, "event_down", strlen("event_down") + 1, portMAX_DELAY);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
```
执行结果：  
![用户自定义事件循环](https://ding-aliyun.oss-cn-shenzhen.aliyuncs.com/esp32/section13_event_loop_result.jpg)  
