#ifndef _BSP_H_
#define _BSP_H_

#include <stdbool.h>

typedef enum
{
    BSP_BUTTON_1 = 0x1,
    BSP_BUTTON_2 = 0x2,
} bsp_buttons_t;

typedef enum
{
    BSP_DISPLAY_DISTANCE,
    BSP_DISPLAY_CALIBRATION,
    BSP_DISPLAY_POSITION,
} bsp_display_t;

typedef enum
{
    BSP_LED_GREEN = 0x1,
    BSP_LED_RED   = 0x2,
} bsp_led_t;

typedef enum
{
    BSP_EVENT_BUTTON_PUSHED,
    BSP_EVENT_ROTARY_ENC_UP,
    BSP_EVENT_ROTARY_ENC_DOWN,
    BSP_EVENT_POSITION_UPDATE,
    BSP_EVENT_TIMER_EXPIRED,
} bsp_event_t;

void bsp_init(void);

uint32_t bsp_buttons_get(void);

void bsp_relay_set(bool enable);

void bsp_led_set(bsp_led_t led, bool enable);

void bsp_led_toggle(bsp_led_t led);

uint32_t bsp_rotary_enc_get();

void bsp_display(bsp_display_t disp_type, uint32_t value);

uint32_t bsp_position_get();

void bsp_timer_set(uint32_t period_ms);

#endif // _BSP_H_
