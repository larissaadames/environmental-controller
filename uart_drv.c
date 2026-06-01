#include "uart_drv.h"

#define UART_PIN_MASK (BIT2 | BIT3) // P1.2 = UCA0RXD, P1.3 = UCA0TXD

void uart_init(void)
{
    // Select the UART function on pins P1.2 and P1.3 ([DS]: SEL1:SEL0 = 0:1).
    P1->SEL0 |= UART_PIN_MASK;  // SEL0 = 1 on pins P1.2 and P1.3
    P1->SEL1 &= ~UART_PIN_MASK; // SEL1 = 0 on pins P1.2 and P1.3

    // Hold the eUSCI in software reset while configuring it (required by [TRM]).
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST;

    // Frame format + clock source. The 8-N-1 async defaults are all 0, so we
    // only set the clock source and keep the reset bit:
    //   UCPEN=0 no parity | UC7BIT=0 8 bits | UCSPB=0 1 stop | UCSYNC=0 async
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SSEL__SMCLK // BRCLK = SMCLK (12 MHz)
                      | EUSCI_A_CTLW0_SWRST;    // stays in reset

    // Baud rate 19200 @ 12 MHz with oversampling (derivation in uart_drv.h).
    EUSCI_A0->BRW = UART_UCBR_VALUE;                                // UCBRx = 39
    EUSCI_A0->MCTLW = (UART_UCBRS_VALUE << EUSCI_A_MCTLW_BRS_OFS)   // UCBRSx
                      | (UART_UCBRF_VALUE << EUSCI_A_MCTLW_BRF_OFS) // UCBRFx
                      | EUSCI_A_MCTLW_OS16;                         // UCOS16 = 1

    // Release the reset; the module now runs with the config above.
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;

    // Clear any pending RX flag before starting.
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;
}

void uart_enable_rx_interrupt(void)
{
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE; // enable RX interrupt (UCRXIE)

    // NVIC_EnableIRQ ORs into ISER, so it won't clobber other IRQs. Serviced in
    // EUSCIA0_IRQHandler, which the application must implement.
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}

int uart_tx_ready(void)
{
    return (EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG) != 0; // UCTXIFG: TXBUF empty
}

int uart_rx_ready(void)
{
    return (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) != 0; // UCRXIFG: byte in RXBUF
}

void uart_send_char(char c)
{
    while (!uart_tx_ready())
    {
    }

    EUSCI_A0->TXBUF = (uint16_t)c;
}

void uart_send_string(const char *str)
{
    while (*str != '\0')
    {
        uart_send_char(*str);
        str++;
    }
}

void uart_send_uint(uint16_t value)
{
    char tmp[5]; // 16-bit value is at most 5 digits (65535)
    int i = 0;

    if (value == 0)
    {
        uart_send_char('0');
        return;
    }

    while (value > 0) // build digits least-significant first
    {
        tmp[i] = (char)('0' + (value % 10U));
        value /= 10U;
        i++;
    }

    while (i > 0) // then send them reversed
    {
        i--;
        uart_send_char(tmp[i]);
    }
}

char uart_receive_char(void)
{
    while (!uart_rx_ready())
    {
    }

    return (char)EUSCI_A0->RXBUF; // reading RXBUF clears UCRXIFG
}
