#include "driverlib.h"
#include "temp_sensor.h"

// TMP006: I2C address per BoosterPack schematic (ADD0=ADD1=0)
#define SENSOR_I2C_ADDRESS 0x40
#define CONFIG_REG_ADDR    0x02
#define TAMBIENT_REG_ADDR  0x01

void temp_sensor_init(void)
{
    I2C_setSlaveAddress(EUSCI_B1_BASE, SENSOR_I2C_ADDRESS);
    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // MOD[14:12]=111 continuous conversion | CR[11:9]=010 -> 1 conv/sec
    uint16_t config = (0x7 << 12) | (0x2 << 9);

    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, CONFIG_REG_ADDR);
    I2C_masterSendMultiByteNext(EUSCI_B1_BASE, (config >> 8) & 0xFF);
    I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, config & 0xFF);
}

// Returns local die temperature in degrees Celsius.
// TAMBIENT register: bits 15:2 = signed value in units of 1/32 °C; bits 1:0 = 0.
// Dividing the raw 16-bit signed value by 128 (= 4 * 32) gives integer Celsius.
int16_t temp_sensor_read(void)
{
    uint16_t raw;

    I2C_setSlaveAddress(EUSCI_B1_BASE, SENSOR_I2C_ADDRESS);
    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    I2C_clearInterruptFlag(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    while (I2C_isBusBusy(EUSCI_B1_BASE));

    I2C_masterSendSingleByte(EUSCI_B1_BASE, TAMBIENT_REG_ADDR);
    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_STOP_INTERRUPT)));

    I2C_masterReceiveStart(EUSCI_B1_BASE);
    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0)));

    raw  = (uint16_t)(I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE) << 8);
    raw |= I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);

    return (int16_t)raw / 128;
}
