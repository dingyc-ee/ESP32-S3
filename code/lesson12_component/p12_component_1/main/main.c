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
