#include <drv_usart.h>
#include <logger.h>
#include <hal_gpio.h>

#define USART_CONFIG_NEW_API 1

/*
static void usart_handler(drv_usart_evt_t evt, const uint8_t * buf)
{

}
*/

void bsp_init(void)
{
/*
    drv_usart_t usart_dev = DRV_USART_INSTANCE_GET(1);

#if USART_CONFIG_NEW_API
    drv_usart_cfg_t const cfg = DRV_USART_DEFAULT_CONFIG(HAL_GPIO_PIN_PA2,
                                                         HAL_GPIO_PIN_PA15);
#else
    hal_gpio_mode_cfg(GPIOA, 2,  HAL_GPIO_MODE_AF);
    hal_gpio_mode_cfg(GPIOA, 15, HAL_GPIO_MODE_AF);
    hal_gpio_af_cfg(GPIOA, 2, HAL_GPIO_AF_1);
    hal_gpio_af_cfg(GPIOA, 15, HAL_GPIO_AF_1);
    drv_usart_cfg_t const cfg = DRV_USART_DEFAULT_CONFIG;
#endif

    drv_usart_init(&usart_dev, &cfg, usart_handler);
*/
}
