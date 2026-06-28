#include "rtc_drv.h"
#include "driverlib.h"
#include "msp.h"
#include "uart_drv.h"

static bool is_leap_year(uint16_t year)
{
    return (year % 4U == 0U && year % 100U != 0U) || (year % 400U == 0U);
}

static uint8_t days_in_month(uint8_t month, uint16_t year)
{
    static const uint8_t days[12] = {31, 28, 31, 30, 31, 30,
                                     31, 31, 30, 31, 30, 31};

    if (month < 1U || month > 12U)
    {
        return 0;
    }

    if (month == 2U && is_leap_year(year))
    {
        return 29;
    }

    return days[month - 1U];
}

static bool datetime_is_valid(const rtc_datetime_t *dt)
{
    if (dt->day < 1U || dt->day > days_in_month(dt->month, dt->year))
    {
        return false;
    }

    return dt->hours <= 23U && dt->minutes <= 59U && dt->seconds <= 59U;
}

void rtc_init(void)
{
    // The RTC is clocked by BCLK. Route the internal 32 kHz REFO oscillator to
    // it so the calendar runs without an external 32.768 kHz crystal (the
    // LaunchPad does not populate one by default).
    CS_initClockSignal(CS_BCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    if (BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS))
    {
        uart_send_string("Resetting RTC, initializing to 12:00:00 on 28/06/2026\r\n");
        const RTC_C_Calendar start = {0, 0, 12, 0, 28, 6, 2026};
        RTC_C_initCalendar(&start, RTC_C_FORMAT_BINARY);
    }

    RTC_C_startClock();
}

bool rtc_set(const rtc_datetime_t *dt)
{
    if (!datetime_is_valid(dt))
    {
        return false;
    }

    const RTC_C_Calendar cal =
        {
            dt->seconds,
            dt->minutes,
            dt->hours,
            0, // dayOfWeek (not tracked)
            dt->day,
            dt->month,
            dt->year};

    RTC_C_initCalendar(&cal, RTC_C_FORMAT_BINARY);
    RTC_C_startClock();
    return true;
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
