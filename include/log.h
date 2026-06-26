#ifndef LOG_H_
#define LOG_H_

#include "stdint.h"
#include <stdbool.h>

typedef enum
{
    LOG_AC_ON = 0,
    LOG_AC_OFF = 1,
    LOG_LAMP_ON = 2,
    LOG_LAMP_OFF = 3
} log_event_t;

void log_init(void);

void log_event(log_event_t type);

void log_show_last(void);

void log_dump(void);

void log_clear(void);

#endif /* LOG_H_ */
