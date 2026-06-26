#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

#include <stdint.h>

void temp_sensor_init(void);
int16_t temp_sensor_read(void); // unit: degrees Celsius

#endif /* TEMP_SENSOR_H_ */
