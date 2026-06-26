#ifndef TIMER_DRV_H_
#define TIMER_DRV_H_

#include "msp.h"
#include "stdint.h"
#include "sys_config.h"

typedef enum
{
    TIMER_1 = 0,
    TIMER_2 = 1
} timer_id_t;

void timer_init(timer_id_t id, uint32_t period_ms);

void timer_enable_interrupt(timer_id_t id);

void timer_clear_interrupt(timer_id_t id);

#endif /* TIMER_DRV_H_ */
