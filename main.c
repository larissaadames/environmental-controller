#include "msp.h"
#include "driverlib.h"
#include "uart_drv.h"
#include "cli.h"
#include "config.h"

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop the watchdog timer

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    uart_init();
    config_init();
    uart_enable_rx_interrupt();

    uart_send_string("System ready\r\n");

    while (1)
    {
        if (cli_poll())
        {
            config_process_line(cli_get_line());
            cli_clear();
        }
    }
}

void EUSCIA0_IRQHandler(void)
{
    if (uart_rx_ready())
    {
        char c = (char)EUSCI_A0->RXBUF; // read the received byte (clears UCRXIFG)
        cli_rx_byte(c);
    }
}
