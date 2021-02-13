#ifndef __K_KEY_H
#define __K_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/* The following program is modified by the user according to the hardware device, otherwise the driver cannot run. */

/* Includes ------------------------------------------------------------------*/
#include <linux/types.h>
#include <linux/list.h>

#include "kinetis/core_common.h"

typedef void (*button_callback)(void *);

enum press_button_event {
    PRESS_DOWN = 0,
    PRESS_UP,
    PRESS_REPEAT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_RRESS_START,
    LONG_PRESS_HOLD,
    PRESSEVENT_NBR,
    NONE_PRESS
};

struct button {
    u16 ticks;
    u8  repeat : 4;
    u8  event : 4;
    u8  state : 3;
    u8  debounce_cnt : 3;
    u8  active_level : 1;
    u8  button_level : 1;
    u8 (*hal_button_level)(void);
    button_callback callback[PRESSEVENT_NBR];
    struct list_head list;
};

int button_add(struct button *button, u8(*pin_level)(void), u8 active_level,
    button_callback cb);
void button_drop(struct button *button);
void button_ticks(void);


/* The above procedure is modified by the user according to the hardware device, otherwise the driver cannot run. */

#ifdef __cplusplus
}
#endif

#endif /* __K_KEY_H */