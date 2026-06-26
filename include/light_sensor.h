#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

#include <stdint.h>

void light_sensor_init(void);
uint32_t light_sensor_read(void); // unit: centilux (0.01 lux)

#endif /* LIGHT_SENSOR_H_ */
