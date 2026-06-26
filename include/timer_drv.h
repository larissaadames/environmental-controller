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

// Configure the selected timer in periodic 32-bit mode and start it; interrupt disabled.
void timer_init(timer_id_t id, uint32_t period_ms);

// Enable the timer interrupt (IE bit + NVIC); handle in the matching T32_INTx_IRQHandler.
void timer_enable_interrupt(timer_id_t id);

// Clear the pending interrupt flag for the selected timer; call at the top of the handler.
void timer_clear_interrupt(timer_id_t id);

#endif /* TIMER_DRV_H_ */
