#include "includes.h"


/*==================== ?????? ====================*/
float voltage_power_supply_speed = 0.0f;
float Ualpha_speed = 0.0f, Ubeta_speed = 0.0f;
float Ua_speed = 0.0f, Ub_speed = 0.0f, Uc_speed = 0.0f;
float zero_electric_angle_speed = 0.0f;
int Motor_PP = 7;
int Sensor_DIR = 1;

/* ??????? */
LowPassFilter M0_Vel_Flt;

/* PID ?? */
PIDController vel_loop_M0;
PIDController angle_loop_M0;

/* AS5600 ????? */
Sensor_AS5600 S0;

/* ??????? */
static float motor_target = 0.0f;
static char received_chars[256];
static int received_index = 0;

/*==================== ???? ====================*/
#define _CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define _3PI_2 4.71238898038f
#define PI 3.14159265359f


/* ?????? [0, 2PI] */
float _normalizeAngle_speed(float angle)
{
    float a = fmodf(angle, 2.0f * PI);
    return (a >= 0) ? a : (a + 2.0f * PI);
}

/* ????? */
float _electricalAngle_speed(void)
{
    return _normalizeAngle_speed((float)(Sensor_DIR * Motor_PP) * Sensor_AS5600_GetMechanicalAngle(&S0) - zero_electric_angle_speed);
}


/*==================== PID 控制 ====================*/
//速度闭环PID
void DFOC_M0_SET_VEL_PID(float P, float I, float D, float ramp)
{
    vel_loop_M0.P = P;
    vel_loop_M0.I = I;
    vel_loop_M0.D = D;
    vel_loop_M0.output_ramp = ramp;
}
//力位角度闭环PID
void DFOC_M0_SET_ANGLE_PID(float P, float I, float D, float ramp)
{
    angle_loop_M0.P = P;
    angle_loop_M0.I = I;
    angle_loop_M0.D = D;
    angle_loop_M0.output_ramp = ramp;
}

float DFOC_M0_VEL_PID(float error)
{
    return PIDController_Update(&vel_loop_M0, error);
}

float DFOC_M0_ANGLE_PID(float error)
{
    return PIDController_Update(&angle_loop_M0, error);
}

void setPwm_speed(float Ua, float Ub, float Uc)
{
    float dc_a, dc_b, dc_c;
    
    /* 限制速度值在有效范围内 */
    Ua = _CONSTRAIN(Ua, 0.0f, voltage_power_supply_speed);
    Ub = _CONSTRAIN(Ub, 0.0f, voltage_power_supply_speed);
    Uc = _CONSTRAIN(Uc, 0.0f, voltage_power_supply_speed);
    
    /* 限制电压值在有效范围内 */
    dc_a = _CONSTRAIN(Ua / voltage_power_supply_speed, 0.0f, 1.0f);
    dc_b = _CONSTRAIN(Ub / voltage_power_supply_speed, 0.0f, 1.0f);
    dc_c = _CONSTRAIN(Uc / voltage_power_supply_speed, 0.0f, 1.0f);
    
    /* ?? PWM (8??? 0-255) */
    Moto1_U_Set_Val((uint32_t)(dc_a * 255.0f));
    Moto1_V_Set_Val((uint32_t)(dc_b * 255.0f));
    Moto1_W_Set_Val((uint32_t)(dc_c * 255.0f));
//	  printf("\r\n dc_a = %f, dc_b = %f, dc_c = %f \r\n", dc_a, dc_b, dc_c);
}

void setTorque(float Uq, float angle_el)
{
    Sensor_AS5600_Update(&S0);  /* ????? */
    
    Uq = _CONSTRAIN(Uq, -voltage_power_supply_speed / 2.0f, voltage_power_supply_speed / 2.0f);
    
    angle_el = _normalizeAngle_speed(angle_el);
    
    /* ????? (Id=0 ??) */
    Ualpha_speed = -Uq * sinf(angle_el);
    Ubeta_speed =  Uq * cosf(angle_el);
    
    /* ?????? (SVPWM ???) */
    Ua_speed = Ualpha_speed + voltage_power_supply_speed / 2.0f;
    Ub_speed = (sqrtf(3.0f) * Ubeta_speed - Ualpha_speed) / 2.0f + voltage_power_supply_speed / 2.0f;
    Uc_speed = (-Ualpha_speed - sqrtf(3.0f) * Ubeta_speed) / 2.0f + voltage_power_supply_speed / 2.0f;
    
    setPwm_speed(Ua_speed, Ub_speed, Uc_speed);
}

/*==================== ????? ====================*/
void DFOC_Vbus(float power_supply)
{
	  float dt = 0.01f;
    float Tf = 0.1f;
    float alpha = dt / (Tf + dt);  /* ?????? */
	
    voltage_power_supply_speed = power_supply;
    
    /* ??? AS5600 ??? */
    Sensor_AS5600_Init(&S0, 0);
    Sensor_AS5600_SensorInit(&S0);
    
    /* ??? PID */
    PIDController_Init(&vel_loop_M0, 2.0f, 0.0f, 0.0f, 100000.0f, voltage_power_supply_speed / 2.0f);
    PIDController_Init(&angle_loop_M0, 2.0f, 0.0f, 0.0f, 100000.0f, 100.0f);
    
    /* ???????? */
#ifdef LOWPASS_TIME
    LowPassFilter_Init(&M0_Vel_Flt, 0.01f);
#else
		LowPassFilter_Init(&M0_Vel_Flt, alpha);
#endif
}

/* ??????? */
void DFOC_alignSensor(int _PP, int _DIR)
{
    Motor_PP = _PP;
    Sensor_DIR = _DIR;
    
    setTorque(3.0f, _3PI_2);  /* ?????? */
    delay_1ms(1000);
    
    Sensor_AS5600_Update(&S0);  /* ???? */
    zero_electric_angle_speed = _electricalAngle_speed();
    
    setTorque(0.0f, _3PI_2);    /* ???? */
}

/*==================== ????? ====================*/
float DFOC_M0_Angle(void)
{
    return (float)Sensor_DIR * Sensor_AS5600_GetAngle(&S0);
}

float DFOC_M0_Velocity(void)
{
    float vel_ori, vel_flit;
    vel_ori = Sensor_AS5600_GetVelocity(&S0);
    vel_flit = LowPassFilter_Update(&M0_Vel_Flt, (float)Sensor_DIR * vel_ori);
//	  printf("\r\n vel_ori = %f, vel_flit = %f \r\n", vel_ori, vel_flit);
    return vel_flit;
}

/*==================== ???? ====================*/
void DFOC_M0_set_Velocity_Angle(float Target)
{
    float angle_error = (Target - DFOC_M0_Angle()) * 180.0f / PI;
    float vel_target = DFOC_M0_ANGLE_PID(angle_error);
    float vel_error = vel_target - DFOC_M0_Velocity();
    float torque = DFOC_M0_VEL_PID(vel_error);
    
    setTorque(torque, _electricalAngle_speed());
}
//速度闭环
void DFOC_M0_setVelocity(float Target)
{
	  static uint16_t count = 0;
    float vel_error = (Target - DFOC_M0_Velocity()) * 180.0f / PI;
    float torque = DFOC_M0_VEL_PID(vel_error);
	  count++;
	  if(count>=100) {
//		    printf("\r\n count = %d,vel_error = %f, torque = %f \r\n", count, vel_error, torque);
			  count = 0;
		}
    setTorque(torque, _electricalAngle_speed());
}
//角度闭环
void DFOC_M0_set_Force_Angle(float Target)
{
    float angle_error = (Target - DFOC_M0_Angle()) * 180.0f / PI;
    float torque = DFOC_M0_ANGLE_PID(angle_error);
    
    setTorque(torque, _electricalAngle_speed());
}

void DFOC_M0_setTorque(float Target)
{
    setTorque(Target, _electricalAngle_speed());
}
