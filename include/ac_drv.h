#ifndef AC_DRV_H_
#define AC_DRV_H_

#include <stdbool.h>

// Initialize the AC module (off by default).
void ac_init(void);

// Turn the AC on.
void ac_on(void);

// Turn the AC off.
void ac_off(void);

// Returns true if the AC is currently on.
bool ac_is_on(void);

#endif /* AC_DRV_H_ */
