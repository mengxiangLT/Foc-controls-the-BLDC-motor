#ifndef __AS5600_I2C_H__
#define __AS5600_I2C_H__

#include "gd32f30x.h"

/* AS5600 I2C ?? - ?????????? */
#define AS5600_SCL_PORT     GPIOB
#define AS5600_SCL_PIN      GPIO_PIN_6
#define AS5600_SDA_PORT     GPIOB
#define AS5600_SDA_PIN      GPIO_PIN_7

/* AS5600 I2C ?? (7???,????1?) */
#define AS5600_ADDR         0x6C

/* AS5600 ????? */
#define AS5600_ANGLE_H      0x0E
#define AS5600_ANGLE_L      0x0F
#define AS5600_STATUS       0x0B
#define AS5600_AGC          0x1A
#define AS5600_MAGNITUDE_H  0x1B
#define AS5600_MAGNITUDE_L  0x1C


/* AS5600 ???? */
void as5600_i2c_init(void);
uint16_t as5600_read_raw_angle(void);    /* ?????? 0-4095 */
float as5600_read_angle_degree(void);    /* ???? 0-360? */
uint8_t as5600_read_status(void);        /* ???? */
uint8_t as5600_check_magnet(void);       /* ??????? */

#endif

