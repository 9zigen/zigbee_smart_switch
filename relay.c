/***
** Created by Aleksey Volkov on 05.06.2020.
***/

#include "bsp.h"
#include "include/relay.h"
#include "include/storage.h"
#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

APP_TIMER_DEF(relay_timer_id);
volatile relay_mode_t relay_mode = ON_OFF;
volatile uint8_t relay_status = 0;

static void relay_timer_handler(void *p_context)
{
  nrf_gpio_pin_clear(RELAY_PIN);
}

static void relay_timer_init()
{
  ret_code_t err_code;

  err_code = app_timer_create(&relay_timer_id, APP_TIMER_MODE_SINGLE_SHOT, relay_timer_handler);
  APP_ERROR_CHECK(err_code);
}

void relay_init()
{
  relay_timer_init();
  nrf_gpio_cfg_output(RELAY_PIN);
  relay_off();
}

void relay_on(uint8_t update_status)
{
  ret_code_t err_code;

  if (relay_mode == ON_DELAY_OFF)
  {
    err_code = app_timer_start(relay_timer_id, APP_TIMER_TICKS(1000), NULL);
    APP_ERROR_CHECK(err_code);
  }

  nrf_gpio_pin_set(RELAY_PIN);
  if (update_status)
  {
    relay_status = 1;
  }
}

void relay_off()
{
  nrf_gpio_pin_clear(RELAY_PIN);
  relay_status = 0;
}

void relay_toggle()
{
  nrf_gpio_pin_toggle(RELAY_PIN);
}

uint8_t relay_get_status()
{
  return relay_status;
}

void relay_set_status(uint8_t status)
{
  relay_status = status;
}

relay_mode_t relay_get_mode()
{
  return relay_mode;
}

void relay_set_mode(relay_mode_t mode)
{
  relay_mode = mode;
}

