//https://www.qrp-labs.com/images/synth/demo3/i2c.h

#ifndef I2C_H
#define I2C_H

#include <inttypes.h>

void i2cInit();
uint8_t i2cSendRegister(uint8_t reg, uint8_t data);
uint8_t i2cReadRegister(uint8_t reg, uint8_t *data);

#endif //I2C_H
