#include "driverlib.h"
#include "light_sensor.h"

#define SENSOR_I2C_ADDRESS 0x44
#define CONFIG_REG_ADDR 0x01
#define RESULT_REG_ADDR 0x00

void light_sensor_init(void)
{
    I2C_setSlaveAddress(EUSCI_B1_BASE, SENSOR_I2C_ADDRESS);
    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // auto full-scale range | 800 ms integration | continuous conversion
    uint16_t config = (0xC << 12) | (0x1 << 11) | (0x2 << 9);

    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, CONFIG_REG_ADDR);
    I2C_masterSendMultiByteNext(EUSCI_B1_BASE, (config >> 8) & 0xFF);
    I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, config & 0xFF);
}

// Returns illuminance in centilux (0.01 lux).
// Raw value encodes: exponent (bits 15:12) and mantissa (bits 11:0).
// Result = (mantissa * 2^exponent) centilux.
uint32_t light_sensor_read(void)
{
    uint16_t raw;
    uint8_t exponent;
    uint16_t mantissa;
    uint32_t factor = 1;

    I2C_setSlaveAddress(EUSCI_B1_BASE, SENSOR_I2C_ADDRESS);
    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    I2C_clearInterruptFlag(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    while (I2C_isBusBusy(EUSCI_B1_BASE))
        ;

    I2C_masterSendSingleByte(EUSCI_B1_BASE, RESULT_REG_ADDR);
    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_STOP_INTERRUPT)))
        ;

    I2C_masterReceiveStart(EUSCI_B1_BASE);
    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0)))
        ;

    raw = (uint16_t)(I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE) << 8);
    raw |= I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);

    exponent = (uint8_t)(raw >> 12);
    mantissa = raw & 0x0FFF;
    while (exponent > 0)
    {
        factor *= 2;
        exponent--;
    }

    return (uint32_t)mantissa * factor;
}
