#ifndef CONFIG_H_
#define CONFIG_H_

#include "stdint.h"

// Load default values. Call once at startup.
void config_init(void);

// Parse one "KEY=value" line, dispatch to its handler, and reply over the UART.
void config_process_line(const char *line);

// Current maximum temperature (degrees Celsius).
uint8_t config_get_temp_max(void);

#endif /* CONFIG_H_ */
