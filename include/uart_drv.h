#ifndef UART_DRV_H_
#define UART_DRV_H_

#include "msp.h"
#include "stdint.h"
#include "stdbool.h"
#include "sys_config.h"

void uart_init(uint32_t baudrate);

void uart_enable_rx_interrupt(void);

void uart_send_char(char c);
void uart_send_string(const char *str);
void uart_send_uint(uint16_t value);

char uart_receive_char(void);

char uart_get_rx_byte(void);

bool uart_tx_ready(void);
bool uart_rx_ready(void);

#endif /* UART_DRV_H_ */
