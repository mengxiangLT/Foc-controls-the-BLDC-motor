#ifndef SOFT_I2C_H
#define SOFT_I2C_H

#include "includes.h"

typedef struct {
    GPIO_TypeDef* scl_port;
    uint16_t scl_pin;
    GPIO_TypeDef* sda_port;
    uint16_t sda_pin;
} SoftI2C_t;

extern SoftI2C_t soft_i2c_motor2;

void DWT_Init(void);
void SoftI2C_Init(SoftI2C_t* i2c);
void SoftI2C_Start(SoftI2C_t* i2c);
void SoftI2C_Stop(SoftI2C_t* i2c);
uint8_t SoftI2C_WriteByte(SoftI2C_t* i2c, uint8_t data);
uint8_t SoftI2C_ReadByte(SoftI2C_t* i2c, uint8_t ack);
uint16_t SoftI2C_ReadAS5600(SoftI2C_t* i2c);

#endif

