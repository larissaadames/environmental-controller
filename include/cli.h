#ifndef CLI_H_
#define CLI_H_

#include "stdint.h"
#include <stdbool.h>

#define CLI_BUFFER_SIZE 32

void cli_init(void);

void cli_rx_byte(char c);

bool cli_poll(void);
void reset_ready(void);

const char *cli_get_line(void);

#endif /* CLI_H_ */
