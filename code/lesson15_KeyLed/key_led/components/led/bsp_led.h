#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include <stdint.h>
#include "driver/gpio.h"

#define LED_PIN_IO      GPIO_NUM_48
#define LED_RMT_CHANNEL 0

void led_init(void);
void led_set_color(uint8_t r, uint8_t g, uint8_t b);

#endif  /* __BSP_LED_H__ */