#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "bsp_led.h"

static led_strip_t *led_hd = NULL;

void led_init(void)
{
    led_hd = led_strip_init(LED_RMT_CHANNEL, LED_PIN_IO, 1);
    led_hd->clear(led_hd, 50);
}

void led_set_color(uint8_t r, uint8_t g, uint8_t b)
{
    if (0 == r && 0 == g && 0 == b) {
        led_hd->clear(led_hd, 50);
    }
    else {
        led_hd->set_pixel(led_hd, 0, r, g, b);
        led_hd->refresh(led_hd, 100);
    }
}
