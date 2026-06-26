#include "rtc_drv.h"
#include "driverlib.h"

void rtc_init(void)
{
    // The RTC is clocked by BCLK. Route the internal 32 kHz REFO oscillator to
    // it so the calendar runs without an external 32.768 kHz crystal (the
    // LaunchPad does not populate one by default).
    CS_initClockSignal(CS_BCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // TODO: the initial date/time is hardcoded. When a real time source is
    // added (e.g. a SET_TIME UART command or an external RTC), set the calendar
    // from it here instead of using this fixed value.
    const RTC_C_Calendar start =
        {
            0,    // seconds
            0,    // minutes
            12,   // hours
            0,    // dayOfWeek
            14,   // dayOfmonth
            6,    // month
            2026  // year
        };

    RTC_C_initCalendar(&start, RTC_C_FORMAT_BINARY);
    RTC_C_startClock();
}

rtc_datetime_t rtc_now(void)
{
    RTC_C_Calendar c = RTC_C_getCalendarTime();

    rtc_datetime_t dt;
    dt.seconds = (uint8_t)c.seconds;
    dt.minutes = (uint8_t)c.minutes;
    dt.hours = (uint8_t)c.hours;
    dt.day = (uint8_t)c.dayOfmonth;
    dt.month = (uint8_t)c.month;
    dt.year = (uint16_t)c.year;
    return dt;
}
