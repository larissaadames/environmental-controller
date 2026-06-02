#include "timer_drv.h"

static Timer32_Type * const s_regs[] = { TIMER32_1, TIMER32_2 };
static const IRQn_Type      s_irqn[] = { T32_INT1_IRQn, T32_INT2_IRQn };

void timer_init(timer_id_t id, uint32_t period_ms)
{
    Timer32_Type *t = s_regs[id];

    // Periodic mode, 32-bit, prescaler /1; interrupt disabled until timer_enable_interrupt().
    t->CONTROL = (1 << 6)   // MODE: periodic
               | (0 << 5)   // IE:   disabled (set via timer_enable_interrupt)
               | (0 << 2)   // PRESCALE: /1
               | (1 << 1)   // SIZE: 32-bit
               | (0 << 0);  // ONESHOT: wrapping

    // Divide before multiply to avoid 32-bit overflow (see timer_drv.h).
    t->LOAD = (SMCLK_HZ / 1000U) * period_ms;

    t->CONTROL |= (1 << 7); // ENABLE: start counting
}

void timer_enable_interrupt(timer_id_t id)
{
    s_regs[id]->CONTROL |= (1 << 5); // IE: enable peripheral interrupt

    // NVIC_EnableIRQ ORs into ISER, so it won't clobber other IRQs. Serviced in
    // T32_INT1_IRQHandler / T32_INT2_IRQHandler, which the application must implement.
    NVIC_EnableIRQ(s_irqn[id]);
}

void timer_clear_interrupt(timer_id_t id)
{
    s_regs[id]->INTCLR = 1; // any write clears the interrupt flag
}
