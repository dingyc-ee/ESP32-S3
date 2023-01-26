#ifndef __BSP_BUTTON_H__
#define __BSP_BUTTON_H__

#include <esp_types.h>

typedef void (* btn_evt_cb)(int btn, int evt);

bool btn_init(void);
bool btn_reg_evt_cb(btn_evt_cb cb);

#endif  /* __BSP_BUTTON_H__ */
