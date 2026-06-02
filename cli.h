#ifndef CLI_H_
#define CLI_H_

#include "stdint.h"
#include <stdbool.h>

// Max line length including '\0'. Longer lines are truncated.
#define CLI_BUFFER_SIZE 32

void cli_init(void);

// Enqueue one received byte. Call from the UART RX ISR (fast, non-blocking).
void cli_rx_byte(char c);

// Drain buffered bytes, echo them, and assemble a line. Call from the main loop.
// Returns true when a complete line is ready.
bool cli_poll(void);

const char *cli_get_line(void); // the assembled '\0'-terminated line
void cli_clear(void);           // free the buffer for the next line

#endif /* CLI_H_ */
