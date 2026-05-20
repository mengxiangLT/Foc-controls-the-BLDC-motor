#include "as5600_deng.h"
#include "as5600_i2c.h"
//#include "i2c_soft.h"  /* ???? I2C ??? I2C,???????? */
#include <math.h>
#include <stdlib.h>

/* 获取时间*/
extern uint32_t _micros(void);

/* ?? I2C ????(??????????)*/
extern uint8_t i2c_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
extern uint8_t i2c_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

#if 0
/* 磁传感器的精度是4096(12位,0-4095)*/
static uint16_t read_raw_angle(Sensor_AS5600 *sensor)
{
    uint16_t buf;
    uint16_t raw_value;
    int bit_resolution = 12;
    int bits_used_msb = 4;  /* 11-7 = 4 */
    float cpr;
    int lsb_used;
    uint8_t lsb_mask, msb_mask;
    
//    (void)sensor;  /* ??????? */
    
    /* ??2???? */
//    if(i2c_read_bytes(AS5600_ADDR, AS5600_RAW_ANGLE_H, buf, 2) != 0) {
//        return 0;
//    }
	  buf = as5600_read_angle_degree();
    
    cpr = 4096.0f;
    lsb_used = bit_resolution - bits_used_msb;
    lsb_mask = (uint8_t)((2 << lsb_used) - 1);
    msb_mask = (uint8_t)((2 << bits_used_msb) - 1);
    
    raw_value = ((buf & 0xFF00) >> 8 & lsb_mask);
    raw_value += (((buf & 0xFF) & msb_mask) << lsb_used);
    
    return raw_value;
}
#endif

/* 将磁传感器的角度值转换为弧度值(0-2PI)*/
double Sensor_AS5600_GetSensorAngle(Sensor_AS5600 *sensor)
{
	  float cpr = 4096.0f;  /* 2^12 */
    uint16_t raw = as5600_read_angle_degree();
    return (raw / cpr) * _2PI;
}

/* 初始化传感器的参数 */
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

/* 初始化传感器参数值 */
void Sensor_AS5600_SensorInit(Sensor_AS5600 *sensor)
{
    Sensor_AS5600_GetSensorAngle(sensor);
    sensor->vel_angle_prev = Sensor_AS5600_GetSensorAngle(sensor);
    sensor->vel_angle_prev_ts = _micros();
    
    Sensor_AS5600_GetSensorAngle(sensor);
    sensor->angle_prev = Sensor_AS5600_GetSensorAngle(sensor);
    sensor->angle_prev_ts = _micros();
}

/* 根据磁编码器获取的角度值，算出电机转过的圈数 */
void Sensor_AS5600_Update(Sensor_AS5600 *sensor)
{
    float val = Sensor_AS5600_GetSensorAngle(sensor);
    float d_angle;
    
    sensor->angle_prev_ts = _micros();
    d_angle = val - sensor->angle_prev;
    
    /* 当角度转过一圈（2PI/360°）的80%时，就认为完成了一圈 */
	  if(fabsf(d_angle) > (0.8f * _2PI)) {   
        if(d_angle > 0) {
            sensor->full_rotations--;
        } else {
            sensor->full_rotations++;
        }
    }
    sensor->angle_prev = val;
}

/* 获取当前转过的角度值(0-2PI)*/
float Sensor_AS5600_GetMechanicalAngle(Sensor_AS5600 *sensor)
{
    return sensor->angle_prev;
}

/* 将磁传感器检测到的转动圈数转换为角度的弧度值，再加上未转满一圈的部分
 *The number of rotations detected by the magnetic sensor is converted into radians, and the portion that did not complete one rotation is added.
*/
float Sensor_AS5600_GetAngle(Sensor_AS5600 *sensor)
{
    return (float)sensor->full_rotations * _2PI + sensor->angle_prev;
}

/* 获取转子的速度 */
float Sensor_AS5600_GetVelocity(Sensor_AS5600 *sensor)
{
    float Ts;
    float vel;
    
	/* 用当前时间减去之前的时间*10的-6次方，转换为秒*/
    Ts = (sensor->angle_prev_ts - sensor->vel_angle_prev_ts) * 1e-6f;
    
    /* 当时间小于等于零时，就直接赋值0.001s */
    if(Ts <= 0) Ts = 1e-3f;
    
	/* ((本时刻的圈数(full_rotations)-前一时刻的圈数(vel_full_rotations))*一圈对应的弧度(_2PI)+(当前角度-上次循环的角度))/过去的时间(Ts) */
    vel = ((float)(sensor->full_rotations - sensor->vel_full_rotations) * _2PI 
           + (sensor->angle_prev - sensor->vel_angle_prev)) / Ts;
    
    /* 将当前参数赋值给前一时刻的参数，用于下次周期的计算 */
    sensor->vel_angle_prev = sensor->angle_prev;
    sensor->vel_full_rotations = sensor->full_rotations;
    sensor->vel_angle_prev_ts = sensor->angle_prev_ts;
    
    return vel;
}

