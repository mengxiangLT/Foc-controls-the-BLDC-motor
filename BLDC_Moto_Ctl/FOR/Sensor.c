#include "Sensor.h"
#include "for_utils.h"
#include <stdio.h>
#include <stdlib.h> 

// 传感器1独立变量
static long cpr1;
static long velocity_calc_timestamp1;
static long angle_data_prev1;
static float angle_prev1;
static float full_rotation_offset1;

// 传感器2独立变量
static long cpr2;
static long velocity_calc_timestamp2;
static long angle_data_prev2;
static float angle_prev2;
static float full_rotation_offset2;

uint16_t getRawCount1(void)
{
    return bsp_as5600GetRawAngle1() & 0x0FFF;
}

uint16_t getRawCount2(void)
{
    return bsp_as5600GetRawAngle2() & 0x0FFF;
}

void MagneticSensor_Init1(void)
{
    cpr1 = 4096;
    angle_data_prev1 = getRawCount1();
    full_rotation_offset1 = 0;
    velocity_calc_timestamp1 = _micros();
    angle_prev1 = getAngle1();
    printf("Sensor1 init: CPR=%ld\r\n", cpr1);
}

void MagneticSensor_Init2(void)
{
    cpr2 = 4096;
    angle_data_prev2 = getRawCount2();
    full_rotation_offset2 = 0;
    velocity_calc_timestamp2 = _micros();
    angle_prev2 = getAngle2();
    printf("Sensor2 init: CPR=%ld\r\n", cpr2);
}

float getAngle1(void)
{
    long angle_data, d_angle;
    
    angle_data = getRawCount1();
    d_angle = angle_data - angle_data_prev1;
    
    if(abs(d_angle) > (0.8f * cpr1)) {
        full_rotation_offset1 += (d_angle > 0) ? -_2PI : _2PI;
    }
    
    angle_data_prev1 = angle_data;
    
    // 防止溢出
    if(full_rotation_offset1 >= (_2PI * 2000)) {
        full_rotation_offset1 = 0;
        angle_prev1 = angle_prev1 - _2PI * 2000;
    }
    if(full_rotation_offset1 <= (-_2PI * 2000)) {
        full_rotation_offset1 = 0;
        angle_prev1 = angle_prev1 + _2PI * 2000;
    }
    
    return (full_rotation_offset1 + ((float)angle_data / cpr1 * _2PI));
}

float getAngle2(void)
{
    long angle_data, d_angle;
    
    angle_data = getRawCount2();
    d_angle = angle_data - angle_data_prev2;
    
    if(abs(d_angle) > (0.8f * cpr2)) {
        full_rotation_offset2 += (d_angle > 0) ? -_2PI : _2PI;
    }
    
    angle_data_prev2 = angle_data;
    
    // 防止溢出
    if(full_rotation_offset2 >= (_2PI * 2000)) {
        full_rotation_offset2 = 0;
        angle_prev2 = angle_prev2 - _2PI * 2000;
    }
    if(full_rotation_offset2 <= (-_2PI * 2000)) {
        full_rotation_offset2 = 0;
        angle_prev2 = angle_prev2 + _2PI * 2000;
    }
    
    return (full_rotation_offset2 + ((float)angle_data / cpr2 * _2PI));
}

float getVelocity1(void)
{
    unsigned long now_us;
    float Ts, angle_now, vel;

    now_us = _micros();
    Ts = (now_us - velocity_calc_timestamp1) * 1e-6f;
    if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

    angle_now = getAngle1();
    vel = (angle_now - angle_prev1) / Ts;

    angle_prev1 = angle_now;
    velocity_calc_timestamp1 = now_us;
    return vel;
}

float getVelocity2(void)
{
    unsigned long now_us;
    float Ts, angle_now, vel;

    now_us = _micros();
    Ts = (now_us - velocity_calc_timestamp2) * 1e-6f;
    if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

    angle_now = getAngle2();
    vel = (angle_now - angle_prev2) / Ts;

    angle_prev2 = angle_now;
    velocity_calc_timestamp2 = now_us;
    return vel;
}

