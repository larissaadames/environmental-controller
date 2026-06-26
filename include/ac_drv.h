#ifndef AC_DRV_H_
#define AC_DRV_H_

#include <stdbool.h>

void ac_init(void);

void ac_on(void);

void ac_off(void);

bool ac_is_on(void);

#endif /* AC_DRV_H_ */
