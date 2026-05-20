#ifndef __BLDC_MOTOR_H
#define __BLDC_MOTOR_H

#include "gd32f30x.h"


/* GD32 ???? I2C ??? */
#ifndef I2C_TRANSMITTER
#define I2C_TRANSMITTER   0
#endif

#ifndef I2C_RECEIVER
#define I2C_RECEIVER      1
#endif
/*==================== ???? ====================*/
/* MS8313 ???? - ?????????? */
#define MS8313_EN1_PORT     GPIOB
#define MS8313_EN1_PIN      GPIO_PIN_0
#define MS8313_IN1_PORT     GPIOB
#define MS8313_IN1_PIN      GPIO_PIN_1
//#define MS8313_EN2_PORT     GPIOB
//#define MS8313_EN2_PIN      GPIO_PIN_2
#define MS8313_IN2_PORT     GPIOB
#define MS8313_IN2_PIN      GPIO_PIN_13
//#define MS8313_EN3_PORT     GPIOB
//#define MS8313_EN3_PIN      GPIO_PIN_14
#define MS8313_IN3_PORT     GPIOB
#define MS8313_IN3_PIN      GPIO_PIN_15

/* AS5600 I2C ?? - ?????????? */
#define AS5600_SCL_PORT     GPIOB
#define AS5600_SCL_PIN      GPIO_PIN_6
#define AS5600_SDA_PORT     GPIOB
#define AS5600_SDA_PIN      GPIO_PIN_7

/* AS5600 I2C ?? (7???,????1?) */
#define AS5600_ADDR         0x36

/* AS5600 ????? */
#define AS5600_ANGLE_H      0x0E
#define AS5600_ANGLE_L      0x0F
#define AS5600_STATUS       0x0B
#define AS5600_AGC          0x1A
#define AS5600_MAGNITUDE_H  0x1B
#define AS5600_MAGNITUDE_L  0x1C

/*==================== ???? ====================*/
/* PID ?????? */
typedef struct {
    float kp;           /* ???? */
    float ki;           /* ???? */
    float kd;           /* ???? */
    float target;       /* ??? */
    float feedback;     /* ??? */
    float error;        /* ???? */
    float prev_error;   /* ????? */
    float integral;     /* ???? */
    float output;       /* ??? */
    float output_max;   /* ????? */
    float output_min;   /* ????? */
} PID_TypeDef;

/*==================== ???? ====================*/
/* ????? */
void bldc_motor_init(void);         /* ??????????? */
void ms8313_init(void);              /* ???MS8313???? */
void as5600_i2c_init(void);          /* ???I2C?? */

/* AS5600 ???? */
uint16_t as5600_read_raw_angle(void);    /* ?????? 0-4095 */
float as5600_read_angle_degree(void);    /* ???? 0-360? */
uint8_t as5600_read_status(void);        /* ???? */
uint8_t as5600_check_magnet(void);       /* ??????? */

/* MS8313 ???? */
void ms8313_set_output(uint8_t en1, uint8_t in1, 
                        uint8_t en2, uint8_t in2, 
                        uint8_t en3, uint8_t in3);  /* ??MS8313???? */
void ms8313_commutation(uint8_t step);     /* ???? */
void ms8313_stop(void);                    /* ?????? */

/* BLDC ???? */
void bldc_run(uint16_t pwm_duty);          /* ????,??PWM??? 0-1000 */
void bldc_run_speed(float target_rpm);     /* ?????? */
void bldc_run_position(uint16_t target_angle); /* ??????(???0-4095) */
void bldc_stop(void);                      /* ???? */

/* PID ?? */
void pid_init(PID_TypeDef *pid, float kp, float ki, float kd, 
              float output_max, float output_min);
float pid_compute(PID_TypeDef *pid, float target, float feedback);

/* ???? */
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
uint32_t get_systick_ms(void);             /* ???????? */

#endif /* __BLDC_MOTOR_H */

