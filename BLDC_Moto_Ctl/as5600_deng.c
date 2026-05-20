#include "as5600_deng.h"
#include "as5600_i2c.h"
//#include "i2c_soft.h"  /* ???? I2C ??? I2C,???????? */
#include <math.h>
#include <stdlib.h>

/* ????????(????????)*/
extern uint32_t _micros(void);

/* ?? I2C ????(??????????)*/
extern uint8_t i2c_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
extern uint8_t i2c_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

#if 1
/* ??????(12?,0-4095)*/
static uint16_t read_raw_angle(Sensor_AS5600 *sensor)
{
    uint16_t buf;
    uint16_t raw_value;
    int bit_resolution = 12;
    int bits_used_msb = 4;  /* 11-7 = 4 */
    float cpr;
    int lsb_used;
    uint8_t lsb_mask, msb_mask;
    
    (void)sensor;  /* ??????? */
    
    /* ??2???? */
//    if(i2c_read_bytes(AS5600_ADDR, AS5600_RAW_ANGLE_H, buf, 2) != 0) {
//        return 0;
//    }
	  buf = (uint16_t)as5600_read_angle_degree();
    
    cpr = powf(2, bit_resolution);
    lsb_used = bit_resolution - bits_used_msb;
    lsb_mask = (uint8_t)((2 << lsb_used) - 1);
    msb_mask = (uint8_t)((2 << bits_used_msb) - 1);
    
    raw_value = ((buf & 0xFF00) >> 8 & lsb_mask);
    raw_value += (((buf & 0xFF) & msb_mask) << lsb_used);
    
    return raw_value;
}
#endif

/* ???????(?? 0-2PI)*/
double Sensor_AS5600_GetSensorAngle(Sensor_AS5600 *sensor)
{
    uint16_t raw = read_raw_angle(sensor);
    float cpr = 4096.0f;  /* 2^12 */
    return (raw / cpr) * _2PI;
//	  float cpr;
//	  cpr = as5600_read_angle_degree() * _2PI;
//	  return cpr;
}

/* ????????? */
void Sensor_AS5600_Init(Sensor_AS5600 *sensor, int Mot_Num)
{
    sensor->Mot_Num = Mot_Num;
    sensor->angle_prev = 0.0f;
    sensor->angle_prev_ts = 0;
    sensor->vel_angle_prev = 0.0f;
    sensor->vel_angle_prev_ts = 0;
    sensor->full_rotations = 0;
    sensor->vel_full_rotations = 0;
}

/* ???????? */
void Sensor_AS5600_SensorInit(Sensor_AS5600 *sensor)
{
    Sensor_AS5600_GetSensorAngle(sensor);
    sensor->vel_angle_prev = Sensor_AS5600_GetSensorAngle(sensor);
    sensor->vel_angle_prev_ts = _micros();
    
    Sensor_AS5600_GetSensorAngle(sensor);
    sensor->angle_prev = Sensor_AS5600_GetSensorAngle(sensor);
    sensor->angle_prev_ts = _micros();
}

/* ??????? */
void Sensor_AS5600_Update(Sensor_AS5600 *sensor)
{
    float val = Sensor_AS5600_GetSensorAngle(sensor);
    float d_angle;
    
    sensor->angle_prev_ts = _micros();
    d_angle = val - sensor->angle_prev;
    
    /* ????:???????? 0.8 * 2PI,???? */
    if(fabsf(d_angle) > (0.8f * _2PI)) {
        if(d_angle > 0) {
            sensor->full_rotations--;
        } else {
            sensor->full_rotations++;
        }
    }
    sensor->angle_prev = val;
}

/* ??????(0-2PI)*/
float Sensor_AS5600_GetMechanicalAngle(Sensor_AS5600 *sensor)
{
    return sensor->angle_prev;
}

/* ??????(???)*/
float Sensor_AS5600_GetAngle(Sensor_AS5600 *sensor)
{
    return (float)sensor->full_rotations * _2PI + sensor->angle_prev;
}

/* ????(??/?)*/
float Sensor_AS5600_GetVelocity(Sensor_AS5600 *sensor)
{
    float Ts;
    float vel;
    
    /* ??????(?)*/
    Ts = (sensor->angle_prev_ts - sensor->vel_angle_prev_ts) * 1e-6f;
    
    /* ????? */
    if(Ts <= 0) Ts = 1e-3f;
    
    /* ?? = (???? + ???? * 2PI) / ?? */
    vel = ((float)(sensor->full_rotations - sensor->vel_full_rotations) * _2PI 
           + (sensor->angle_prev - sensor->vel_angle_prev)) / Ts;
    
    /* ????????? */
    sensor->vel_angle_prev = sensor->angle_prev;
    sensor->vel_full_rotations = sensor->full_rotations;
    sensor->vel_angle_prev_ts = sensor->angle_prev_ts;
    
    return vel;
}

