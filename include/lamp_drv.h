#ifndef LAMP_DRV_H_
#define LAMP_DRV_H_

#include <stdbool.h>
#include <stdint.h>

void lamp_init(void);
void lamp_toggle(void);
void lamp_tick(void);
bool lamp_is_on(void);

uint32_t lamp_get_threshold(void); // unit: centilux
void lamp_set_threshold(uint32_t centilux);

#endif /* LAMP_DRV_H_ */
