/***
** Created by Aleksey Volkov on 05.06.2020.
***/

#ifndef SSR_RELAY_RELAY_H
#define SSR_RELAY_RELAY_H

typedef enum {
  ON_OFF = 0, ON_DELAY_OFF
} relay_mode_t;

void relay_init();
void relay_on(uint8_t update_status);
void relay_off();
void relay_toggle();
uint8_t relay_get_status();
relay_mode_t relay_get_mode();
void relay_set_status(uint8_t status);
void relay_set_mode(relay_mode_t mode);

#endif //SSR_RELAY_RELAY_H
