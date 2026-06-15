#ifndef LOG_H_
#define LOG_H_

#include "stdint.h"

// Event types stored in the flash log. log_dump() reproduces the exact UART
// message associated with each type.
typedef enum
{
    LOG_AC_ON = 0,
    LOG_AC_OFF = 1,
    LOG_LAMP_ON = 2,
    LOG_LAMP_OFF = 3
} log_event_t;

// Prepare the flash log sectors and scan them to resume after the last record
// written before the last power-down. Call once at startup, after rtc_init().
void log_init(void);

// Append one event (type + current date/time from the RTC) to the flash log.
void log_event(log_event_t type);

// Print every stored event over the UART, oldest first.
void log_dump(void);

#endif /* LOG_H_ */
