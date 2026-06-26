#ifndef RTC_DRV_H_
#define RTC_DRV_H_

#include "stdint.h"

// Calendar date/time, used to timestamp log events.
typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_datetime_t;

// Start the RTC_C calendar from a fixed initial date/time. Call once at startup.
void rtc_init(void);

// Current date/time read from the RTC calendar registers.
rtc_datetime_t rtc_now(void);

#endif /* RTC_DRV_H_ */
