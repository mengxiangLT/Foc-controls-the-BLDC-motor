#ifndef __AS5600_H
#define __AS5600_H

#include <stdint.h>

#define _2PI 6.28318530718f

/* AS5600 I2C ?? */
//#define AS5600_ADDR 0x36

/* AS5600 ????? */
//#define AS5600_RAW_ANGLE_H 0x0C
//#define AS5600_RAW_ANGLE_L 0x0D
//#define AS5600_ANGLE_H      0x0E
//#define AS5600_ANGLE_L      0x0F
//#define AS5600_STATUS       0x0B

/* AS5600 ??? */
typedef struct {
    int Mot_Num;                    /* ???? */
    float angle_prev;               /* ?????? */
    uint32_t angle_prev_ts;         /* ?????????(??) */
    float vel_angle_prev;           /* ????????? */
    uint32_t vel_angle_prev_ts;     /* ????????? */
    int32_t full_rotations;         /* ????? */
    int32_t vel_full_rotations;     /* ??????? */
} Sensor_AS5600;

/* ???? */
void Sensor_AS5600_Init(Sensor_AS5600 *sensor, int Mot_Num);
void Sensor_AS5600_SensorInit(Sensor_AS5600 *sensor);
double Sensor_AS5600_GetSensorAngle(Sensor_AS5600 *sensor);
void Sensor_AS5600_Update(Sensor_AS5600 *sensor);
float Sensor_AS5600_GetMechanicalAngle(Sensor_AS5600 *sensor);
float Sensor_AS5600_GetAngle(Sensor_AS5600 *sensor);
float Sensor_AS5600_GetVelocity(Sensor_AS5600 *sensor);

#endif /* __AS5600_H */

