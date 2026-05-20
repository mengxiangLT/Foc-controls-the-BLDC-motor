
#ifndef SENSOR_H
#define SENSOR_H

#include "includes.h"

extern  long  cpr;
extern  float angle_prev;

void MagneticSensor_Init1(void);
float getAngle1(void);
float getVelocity1(void);
void MagneticSensor_Init2(void);
float getAngle2(void);
float getVelocity2(void);

#endif


