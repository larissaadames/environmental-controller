#ifndef UART_DRV_H_
#define UART_DRV_H_

#include "msp.h"
#include "stdint.h"

//*****************************************************************************
//
// For SMCLK = 12 MHz and baud = 19200:
//   N = f_BRCLK / baud = 12000000 / 19200 = 625
//   Since N >= 16, use oversampling mode (UCOS16 = 1):
//     UCBRx  = INT(N / 16)                       = INT(625/16)   = 39
//     UCBRFx = INT( (N/16 - INT(N/16)) * 16 )    = INT(0.0625*16)= 1
//     UCBRSx = table value for the fractional part of N (0.0625) = 0x00
//
//*****************************************************************************
#define UART_SMCLK_HZ 12000000U // SMCLK frequency (Hz)
#define UART_BAUDRATE 19200U    // Desired baud rate (bps)
#define UART_UCBR_VALUE 39U     // UCA0BRW  (integer divisor)
#define UART_UCBRF_VALUE 1U     // UCBRFx   (1st modulation stage)
#define UART_UCBRS_VALUE 0x00U  // UCBRSx   (2nd modulation stage)

// Configure pins, clock source and eUSCI_A0 for UART 8-N-1 / 19200 bps (polling).
void uart_init(void);

// Enable the RX interrupt (UCRXIE + NVIC); handle reception in EUSCIA0_IRQHandler.
void uart_enable_rx_interrupt(void);

void uart_send_char(char c);
void uart_send_string(const char *str);
void uart_send_uint(uint16_t value);

char uart_receive_char(void);

int uart_tx_ready(void);
int uart_rx_ready(void);

#endif /* UART_DRV_H_ */
