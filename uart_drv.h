#ifndef UART_DRV_H_
#define UART_DRV_H_

#include "msp.h"
#include "stdint.h"
#include "stdbool.h"
#include "sys_config.h"

// Configure pins, clock source and eUSCI_A0 for UART 8-N-1 at baudrate (polling).
void uart_init(uint32_t baudrate);

// Enable the RX interrupt (UCRXIE + NVIC); handle reception in EUSCIA0_IRQHandler.
void uart_enable_rx_interrupt(void);

void uart_send_char(char c);
void uart_send_string(const char *str);
void uart_send_uint(uint16_t value);

char uart_receive_char(void);

// Read one byte from RXBUF without blocking; call only after uart_rx_ready() returns true.
char uart_get_rx_byte(void);

bool uart_tx_ready(void);
bool uart_rx_ready(void);

#endif /* UART_DRV_H_ */
