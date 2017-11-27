#ifndef __UIP_TIMER_H__
#define __UIP_TIMER_H__
#include "util.h"

typedef unsigned long clock_time_t;
#define CLOCK_CONF_SECOND 1000

void uip_clock_init(void);
clock_time_t clock_time(void);

#ifdef CLOCK_CONF_SECOND
#define CLOCK_SECOND CLOCK_CONF_SECOND
#else
#define CLOCK_SECOND (clock_time_t)32
#endif


struct uip_timer {
  clock_time_t start;
  clock_time_t interval;
};

void uip_timer_set(struct uip_timer *t, clock_time_t interval);
void uip_timer_reset(struct uip_timer *t);
void uip_timer_restart(struct uip_timer *t);
int uip_timer_expired(struct uip_timer *t);

#endif /* __UIP_TIMER_H__ */

