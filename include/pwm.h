/***
** Created by Aleksey Volkov on 05.06.2020.
***/

#ifndef SSR_RELAY_PWM_H
#define SSR_RELAY_PWM_H

typedef enum {
  INDICATE_OFF, INDICATE_ON, INDICATE_SEARCHING, INDICATE_JOINED, INDICATE_MODE_0, INDICATE_MODE_1
} led_mode_t;

void leds_init();
void indicate_config_mode();
void indicate_config_mode_finish();
void indicate_mode(led_mode_t led_mode);

#endif //SSR_RELAY_PWM_H
