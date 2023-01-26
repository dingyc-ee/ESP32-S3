#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include <stdbool.h>
#include "driver/gpio.h"
#include "flexible_button.h"

#define BOOT_PIN_IO     GPIO_NUM_0

typedef void (* button_handler)(int btn, int event);

bool button_init(void);
bool register_button_handler(button_handler cb);

#endif  /* __BSP_KEY_H__ */
