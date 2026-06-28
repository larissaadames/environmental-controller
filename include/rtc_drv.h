#ifndef RTC_DRV_H_
#define RTC_DRV_H_

#include "stdint.h"
#include <stdbool.h>

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_datetime_t;

void rtc_init(void);

rtc_datetime_t rtc_now(void);

bool rtc_set(const rtc_datetime_t *dt);

#endif /* RTC_DRV_H_ */
