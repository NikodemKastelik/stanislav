#include <unity.h>
#include <string.h>

#include <mock_bsp.h>
#include "state_machine.c"

void setUp(void)
{
}

void tearDown(void)
{
}

static void init_expect(void)
{
    bsp_init_Expect();
    bsp_timer_set_Expect(LED_BLINK_PERIOD_MS);

    bsp_relay_set_Expect(false);
    bsp_led_set_Expect(BSP_LED_GREEN, true);
    bsp_led_set_Expect(BSP_LED_RED,   false);
}

void test_initialise(void)
{
    init_expect();
    state_machine_start();
}

void test_motor_start_and_stop(void)
{
    init_expect();
    state_machine_start();

    state_machine_event_add(BSP_EVENT_BUTTON_PUSHED);

    bsp_buttons_get_ExpectAndReturn(BSP_BUTTON_1);
    bsp_relay_set_Expect(true);
    bsp_led_set_Expect(BSP_LED_RED, false);
    state_machine_process();

    state_machine_event_add(BSP_EVENT_ROTARY_ENC_UP);
}
