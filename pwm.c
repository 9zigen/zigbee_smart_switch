/***
** Created by Aleksey Volkov on 05.06.2020.
***/

#include "include/pwm.h"
#include "nrfx_pwm.h"
#include "app_timer.h"
#include "include/custom_board.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

void leds_init()
{
  nrfx_pwm_config_t const config =
      {
          .output_pins =
              {
                  LED_2_PIN | NRFX_PWM_PIN_INVERTED, // channel 0
                  NRFX_PWM_PIN_NOT_USED,             // channel 1
                  NRFX_PWM_PIN_NOT_USED,             // channel 2
                  NRFX_PWM_PIN_NOT_USED,             // channel 3
              },
          .irq_priority = APP_IRQ_PRIORITY_LOWEST,
          .base_clock   = NRF_PWM_CLK_125kHz,
          .count_mode   = NRF_PWM_MODE_UP,
          .top_value    = 25000,
          .load_mode    = NRF_PWM_LOAD_COMMON,
          .step_mode    = NRF_PWM_STEP_AUTO
      };
  APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config, NULL));
}

void indicate_joined()
{
  NRF_LOG_INFO("indicate_joined");

  static uint16_t seq_values[] =
      {
          0x8000,
          0,
          0,
          0,
          0,
          0
      };

  nrf_pwm_sequence_t const seq =
      {
          .values.p_common = seq_values,
          .length          = NRF_PWM_VALUES_LENGTH(seq_values),
          .repeats         = 0,
          .end_delay       = 0
      };

  nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_STOP);
}

void indicate_searching()
{
  NRF_LOG_INFO("indicate_searching");

  static uint16_t seq_values[] =
      {
          0x8000,
          0,
          0x8000,
          0,
          0x8000,
          0
      };

  nrf_pwm_sequence_t const seq =
      {
          .values.p_common = seq_values,
          .length          = NRF_PWM_VALUES_LENGTH(seq_values),
          .repeats         = 0,
          .end_delay       = 2
      };

  nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP);
}

void indicate_config_mode()
{
  NRF_LOG_INFO("indicate_config_mode");

  static uint16_t seq_values[] =
      {
          0x8000,
          0,
          0x8000,
          0,
          0x8000,
          0
      };

  nrf_pwm_sequence_t const seq =
      {
          .values.p_common = seq_values,
          .length          = NRF_PWM_VALUES_LENGTH(seq_values),
          .repeats         = 2,
          .end_delay       = 0
      };

  nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP);
}

void indicate_config_mode_finish()
{
  NRF_LOG_INFO("indicate_config_mode_finish");

  // This array cannot be allocated on stack (hence "static") and it must
  // be in RAM (hence no "const", though its content is not changed).
  static uint16_t /*const*/ seq_values[] =
      {
          0x8000,
          0,
          0x8000,
          0,
          0x8000,
          0
      };
  nrf_pwm_sequence_t const seq =
      {
          .values.p_common = seq_values,
          .length          = NRF_PWM_VALUES_LENGTH(seq_values),
          .repeats         = 2,
          .end_delay       = 0
      };

  nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_STOP);
}

static void single_blink()
{
  NRF_LOG_INFO("single_blink");

  // This array cannot be allocated on stack (hence "static") and it must
  // be in RAM (hence no "const", though its content is not changed).
  static uint16_t /*const*/ seq_values[] =
      {
          0x8000,
          0,
          0,
          0,
          0,
          0
      };
  nrf_pwm_sequence_t const seq =
      {
          .values.p_common = seq_values,
          .length          = NRF_PWM_VALUES_LENGTH(seq_values),
          .repeats         = 0,
          .end_delay       = 4
      };

  nrfx_pwm_simple_playback(&m_pwm0, &seq, 3, NRFX_PWM_FLAG_LOOP);
}

static void double_blink()
{
  NRF_LOG_INFO("single_blink");

  // This array cannot be allocated on stack (hence "static") and it must
  // be in RAM (hence no "const", though its content is not changed).
  static uint16_t /*const*/ seq_values[] =
      {
          0x8000,
          0,
          0x8000,
          0,
          0,
          0
      };
  nrf_pwm_sequence_t const seq =
      {
          .values.p_common = seq_values,
          .length          = NRF_PWM_VALUES_LENGTH(seq_values),
          .repeats         = 0,
          .end_delay       = 4
      };

  nrfx_pwm_simple_playback(&m_pwm0, &seq, 3, NRFX_PWM_FLAG_LOOP);
}

void indicate_mode(led_mode_t led_mode)
{
  switch (led_mode)
  {
    case INDICATE_OFF:
      nrf_gpio_cfg_output(LED_1_PIN);
      nrf_gpio_pin_set(LED_1_PIN);
      break;

    case INDICATE_ON:
      nrf_gpio_cfg_output(LED_1_PIN);
      nrf_gpio_pin_clear(LED_1_PIN);
      break;

    case INDICATE_SEARCHING:
      indicate_searching();
      break;

    case INDICATE_JOINED:
      indicate_joined();
      break;

    case INDICATE_MODE_0:
      single_blink();
      break;

    case INDICATE_MODE_1:
      double_blink();
      break;
  }


}