/***
** Created by Aleksey Volkov on 18.03.2020.
***/

#ifndef STORAGE_H
#define STORAGE_H

/* 4 bytes aligned */
typedef struct {
  uint32_t magic;
  uint32_t version;
  uint32_t mode;
  uint32_t reserved;
} settings_t;

void storage_init();
void set_settings(uint32_t mode);

#endif //STORAGE_H
