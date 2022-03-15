#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  sys_outb(TIMER_CTRL, TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_);

  return util_sys_inb(TIMER_0 + timer, st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val val;

  switch (field) {
  case tsf_all:
    val.byte = st;
    break;
  case tsf_initial:
    val.in_mode = (st & TIMER_LSB_MSB) >> 4;
    break;
  case tsf_mode:
    val.count_mode = (st & 0xE) >> 1;
    break;
  case tsf_base:
    val.bcd = st & 1;
    break;
  }

  timer_print_config(timer, field, val);

  return 0;
}
