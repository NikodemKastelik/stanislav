#include <hsm.h>
#include <ring_buffer.h>
#include <bsp.h>

#define __HSM_ARG_COUNT(_dummy, _0, _1, ...) _1
#define _HSM_ARG_COUNT(...)                  __HSM_ARG_COUNT(__VA_ARGS__, 1, 0)

#define _HSM_STATE_PARENT_1(parent_name) &state_ ## parent_name
#define _HSM_STATE_PARENT_0(...)         NULL

#define _HSM_STATE_PARENT_DETERMINE(arg_count, ...) _HSM_STATE_PARENT_ ## arg_count (__VA_ARGS__)

#define _HSM_STATE_DECLARE(arg_count, name, ...) \
    static bool state_ ## name ## _handler(hsm_event_t evt); \
    static const hsm_state_t state_ ## name = \
        {state_ ## name ## _handler, _HSM_STATE_PARENT_DETERMINE(arg_count, __VA_ARGS__)}

#define HSM_STATE_DECLARE(...) _HSM_STATE_DECLARE(_HSM_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define EVENT_QUEUE_SIZE 8

#define LED_BLINK_FREQ_HZ   (2)
#define LED_BLINK_PERIOD_MS (1000 / LED_BLINK_FREQ_HZ)

#define DISTANCE_OFFSET_MIN 10

HSM_STATE_DECLARE(powered_on);
HSM_STATE_DECLARE(device_initialise, powered_on);
HSM_STATE_DECLARE(distance_offset_control, powered_on);
HSM_STATE_DECLARE(calibration, powered_on);
HSM_STATE_DECLARE(motor_running, powered_on);
HSM_STATE_DECLARE(fault, powered_on);

static uint8_t m_queue_buffer[EVENT_QUEUE_SIZE * sizeof(hsm_event_t)];
static ringbuf_t m_queue;

static hsm_t m_hsm;
static bool m_led_green_blink;
static bool m_led_red_blink;
static uint32_t m_distance_configured;

static bool state_powered_on_handler(hsm_event_t evt)
{
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            hsm_transition(&m_hsm, &state_device_initialise);
            break;
        case BSP_EVENT_TIMER_EXPIRED:
            break;
        default:
            return false;
    }
    return true;
}

static bool state_device_initialise_handler(hsm_event_t evt)
{
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            m_distance_configured = DISTANCE_OFFSET_MIN;
            ringbuf_init(&m_queue, m_queue_buffer, sizeof(hsm_event_t), sizeof(m_queue_buffer));
            bsp_init();
            bsp_timer_set(LED_BLINK_PERIOD_MS);
            hsm_transition(&m_hsm, &state_distance_offset_control);
            break;
        default:
            return false;
    }
    return true;
}

static bool state_distance_offset_control_handler(hsm_event_t evt)
{
    uint32_t buttons_mask;
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            bsp_relay_set(false);
            bsp_led_set(BSP_LED_GREEN, true);
            bsp_led_set(BSP_LED_RED,   false);
            break;
        case BSP_EVENT_BUTTON_PUSHED:
            buttons_mask = bsp_buttons_get();
            if ((buttons_mask & (BSP_BUTTON_1 | BSP_BUTTON_2)) == (BSP_BUTTON_1 | BSP_BUTTON_2))
            {
                hsm_transition(&m_hsm, &state_calibration);
            }
            else if (buttons_mask & BSP_BUTTON_1)
            {
                hsm_transition(&m_hsm, &state_motor_running);
            }
            break;
        case BSP_EVENT_ROTARY_ENC_UP:
            m_distance_configured++;
            bsp_display(BSP_DISPLAY_DISTANCE, m_distance_configured);
            break;
        case BSP_EVENT_ROTARY_ENC_DOWN:
            m_distance_configured--;
            if (m_distance_configured < DISTANCE_OFFSET_MIN)
            {
                m_distance_configured = DISTANCE_OFFSET_MIN;
            }
            bsp_display(BSP_DISPLAY_DISTANCE, m_distance_configured);
            break;
        case HSM_EVENT_EXIT:
            break;
        default:
            return false;
    }
    return true;
}

static bool state_motor_running_handler(hsm_event_t evt)
{
    bool position_updated = false;
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            bsp_relay_set(true);
            m_led_green_blink = true;
            bsp_led_set(BSP_LED_RED, false);
            break;
        case BSP_EVENT_POSITION_UPDATE:
            position_updated = true;
            uint32_t current_position = bsp_position_get();
            bsp_display(BSP_DISPLAY_POSITION, current_position);
            if (current_position == m_distance_configured)
            {
                hsm_transition(&m_hsm, &state_distance_offset_control);
            }
            break;
        case BSP_EVENT_TIMER_EXPIRED:
            if (!position_updated)
            {
                hsm_transition(&m_hsm, &state_fault);
            }
            break;
        case HSM_EVENT_EXIT:
            m_led_green_blink = false;
            break;
        default:
            return false;
    }
    return true;
}

static bool state_calibration_handler(hsm_event_t evt)
{
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            break;
        case HSM_EVENT_EXIT:
            break;
        default:
            return false;
    }
    return true;
}

static bool state_fault_handler(hsm_event_t evt)
{
    switch (evt)
    {
        case HSM_EVENT_ENTRY:
            bsp_relay_set(false);
            m_led_red_blink = true;
            bsp_led_set(BSP_LED_GREEN, false);
            break;
        case HSM_EVENT_EXIT:
            m_led_red_blink = false;
            break;
        default:
            return false;
    }
    return true;
}

void state_machine_start(void)
{
    hsm_start(&m_hsm, &state_powered_on);
}

void state_machine_event_add(hsm_event_t event)
{
    ringbuf_put(&m_queue, &event);
}

void state_machine_process(void)
{
    hsm_event_t event;
    ringbuf_get(&m_queue, &event);
    hsm_dispatch(&m_hsm, event);
}
