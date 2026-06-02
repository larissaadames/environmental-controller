#include "msp.h"
#include "driverlib.h"
#include "uart_drv.h"
#include "cli.h"
#include "config.h"
#include "timer_drv.h"
#include "ac_drv.h"
#include <stdlib.h>
#include <stdbool.h>

static volatile bool s_sensor_tick = false;

void handler_sensor_tick();

void main(void)
{
	// Setup
    WDTCTL = WDTPW | WDTHOLD; // Stop the watchdog timer
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    // INITs
    uart_init(19200);
    cli_init();
    config_init();
    ac_init();
    timer_init(TIMER_1, 10000); // 10s

    // IRQs
    uart_enable_rx_interrupt();
    timer_enable_interrupt(TIMER_1);

    uart_send_string("System ready\r\n");

    while (1)
    {
        if (cli_poll())
        {
            config_process_line(cli_get_line());
            cli_clear();
        }

        if (s_sensor_tick)
        {
            s_sensor_tick = false;
            handle_sensor_tick();
        }
    }
}

void EUSCIA0_IRQHandler(void)
{
    if (uart_rx_ready())
    {
        cli_rx_byte(uart_get_rx_byte());
    }
}

void T32_INT1_IRQHandler(void)
{
    timer_clear_interrupt(TIMER_1);
    s_sensor_tick = true;
}

void handler_sensor_tick()
{
    // TODO: Get temperature
    int temp = rand() % 41; // 0 - 40
    int max_temp = config_get_temp_max();

    // TODO: Get luminosity

    // Debug
    uart_send_string("Temp: ");
    uart_send_uint((uint16_t)temp);
    uart_send_string("C\r\n");

    if (temp <= max_temp && ac_is_on())
    {
        ac_off();
    }
    else if (temp > max_temp && !ac_is_on())
    {
        ac_on();
    }
}
