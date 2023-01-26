#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include <esp_types.h>

void led_init(void);
void led_set_color(uint8_t r, uint8_t g, uint8_t b);

#endif  /* __BSP_LED_H__ */