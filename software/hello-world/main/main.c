#include <stdio.h>
#include "esp_log.h"

const char *TagLight = "交通灯";
const char *TagRoad  = "人行横道";

bool io_detect = true;

void app_main(void)
{
    if (true == io_detect) {
        esp_log_level_set(TagLight, ESP_LOG_VERBOSE);
    }

    ESP_LOGE(TagLight, "不亮了");
    ESP_LOGW(TagLight, "灯泡寿命还有%d%%", 5);
    ESP_LOGI(TagLight, "交通灯寿命还有%d%%", 75);
    ESP_LOGD(TagLight, "灯泡亮度1000 已使用200小时");
    ESP_LOGV(TagLight, "等死还有5丝厚度 氧化程度0.23 寿命正常");

    ESP_LOGE(TagRoad, "有人闯红灯");
    ESP_LOGW(TagRoad, "现在是红灯 禁止通行");
    ESP_LOGI(TagRoad, "20秒后 绿灯");
}
