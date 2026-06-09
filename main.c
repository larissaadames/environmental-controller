#include "msp.h"
#include "driverlib.h"
#include "uart_drv.h"
#include "cli.h"
#include "config.h"
#include "timer_drv.h"
#include "ac_drv.h"
#include "lamp_drv.h"
#include "led_drv.h"
#include "temp_sensor.h"
#include "light_sensor.h"
#include "sys_config.h"
#include <stdbool.h>

static volatile bool s_sensor_tick = false;

static void handler_sensor_tick(void);
static void i2c_init(void);
static void system_init(void);

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop the watchdog timer

    system_init();

    uart_send_string("System ready\r\n");

    while (1)
    {
        if (cli_poll())
        {
            config_process_line(cli_get_line());
            reset_ready();
        }

        if (s_sensor_tick)
        {
            s_sensor_tick = false;
            handler_sensor_tick();
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

void T32_INT2_IRQHandler(void)
{
    timer_clear_interrupt(TIMER_2);
    lamp_tick();
}

static void handler_sensor_tick(void)
{
    int16_t temp = temp_sensor_read();
    uint32_t light = light_sensor_read();
    int max_temp = config_get_temp_max();

    // Debug
    uart_send_string("Temp: ");
    uart_send_uint((uint16_t)temp);
    uart_send_string(" C\r\n");

    uart_send_string("Light: ");
    uart_send_uint(light);
    uart_send_string(" centilux\r\n");

    if (temp <= max_temp && ac_is_on())
    {
        ac_off();
    }
    else if (temp > max_temp && !ac_is_on())
    {
        ac_on();
    }

    bool real_lamp_is_on = light >= lamp_get_threshold();
    if (real_lamp_is_on != lamp_is_on())
    {
        lamp_toggle();
    }
}

static void i2c_init(void)
{
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    eUSCI_I2C_MasterConfig cfg;
    cfg.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    cfg.i2cClk = SMCLK_HZ;
    cfg.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    cfg.byteCounterThreshold = 0;
    cfg.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;

    I2C_disableModule(EUSCI_B1_BASE);
    I2C_initMaster(EUSCI_B1_BASE, &cfg);
    I2C_enableModule(EUSCI_B1_BASE);
}

static void system_init(void)
{
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    i2c_init();
    uart_init(19200);
    cli_init();
    config_init();
    ac_init();
    lamp_init();
    led_init();
    timer_init(TIMER_1, 10000);
    timer_init(TIMER_2, 1000);
    temp_sensor_init();
    light_sensor_init();
    uart_enable_rx_interrupt();
    timer_enable_interrupt(TIMER_1);
    timer_enable_interrupt(TIMER_2);
}
