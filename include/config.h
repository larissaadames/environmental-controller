#ifndef CONFIG_H_
#define CONFIG_H_

#include "stdint.h"

void config_init(void);

void config_process_line(const char *line);

uint8_t config_get_temp_max(void);

#endif /* CONFIG_H_ */
