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
    esp_event_handler_register_with(loop_handle, loop_base, EVENT_UP, ding_event_loop_handle, NULL);
    esp_event_handler_register_with(loop_handle, loop_base, EVENT_DOWN, ding_event_loop_handle, NULL);

    while (1) {
        esp_event_post_to(loop_handle, loop_base, EVENT_UP, "event_up", strlen("event_up") + 1, portMAX_DELAY);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        esp_event_post_to(loop_handle, loop_base, EVENT_DOWN, "event_down", strlen("event_down") + 1, portMAX_DELAY);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
