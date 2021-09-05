/***
** Created by Aleksey Volkov on 16.03.2020.
***/

#ifndef ZB_CUSTOM_BOARD_H
#define ZB_CUSTOM_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

/* Hardware */
//#define SSR_VERSION

#define RELAY_PIN      NRF_GPIO_PIN_MAP(0,28)

#define LEDS_NUMBER    2

#ifdef SSR_VERSION
#define LED_1_PIN      NRF_GPIO_PIN_MAP(0,10)
#else
#define LED_1_PIN      NRF_GPIO_PIN_MAP(0,17)
#define LED_2_PIN      NRF_GPIO_PIN_MAP(0,15)
#endif

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST { LED_1_PIN, LED_2_PIN }

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      LED_1_PIN
#define BSP_LED_1      LED_2_PIN

// There is only one button for the application
// as the second button is used for a RESET.
#define BUTTONS_NUMBER 1

#ifdef SSR_VERSION
#define BUTTON_1       NRF_GPIO_PIN_MAP(0,9)
#else
#define BUTTON_1       NRF_GPIO_PIN_MAP(0,24)
#endif

#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1

//#define BSP_SELF_PINRESET_PIN NRF_GPIO_PIN_MAP(0,19)

#define HWFC           true

#ifdef __cplusplus
}
#endif

#endif //ZB_CUSTOM_BOARD_H
