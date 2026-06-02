#ifndef AC_DRV_H_
#define AC_DRV_H_

// Initialize the AC module (off by default).
void ac_init(void);

// Turn the AC on.
void ac_on(void);

// Turn the AC off.
void ac_off(void);

// Returns non-zero if the AC is currently on.
int ac_is_on(void);

#endif /* AC_DRV_H_ */
