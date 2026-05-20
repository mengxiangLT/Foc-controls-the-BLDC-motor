#include "bldc_motor.h"
#include <math.h>

/*==================== ???? ====================*/
static volatile uint32_t systick_counter = 0;

/* ????? - ?? */
static const uint8_t commutation_steps[6][6] = {
    {1, 1, 1, 0, 0, 0},  /* ??0: U????, V????, W?? */
    {1, 1, 0, 0, 1, 0},  /* ??1: U????, W????, V?? */
    {0, 0, 1, 1, 1, 0},  /* ??2: V????, W????, U?? */
    {0, 0, 1, 1, 0, 0},  /* ??3: V????, U????, W?? */
    {0, 0, 0, 0, 1, 1},  /* ??4: W????, U????, V?? */
    {1, 0, 0, 0, 1, 1}   /* ??5: W????, V????, U?? */
};

/* ???? */
static const uint16_t angle_thresholds[6] = {
    0,      /* 0° */
    683,    /* 60° = 4096/6 */
    1365,   /* 120° */
    2048,   /* 180° */
    2730,   /* 240° */
    3413    /* 300° */
};

/*==================== SysTick ====================*/
//void SysTick_Handler(void)
//{
//    systick_counter++;
//}

uint32_t get_systick_ms(void)
{
    return systick_counter;
}

/*==================== ???? ====================*/
void delay_us(uint32_t nus)
{
    uint32_t cnt = nus * (SystemCoreClock / 1000000) / 5;
    while(cnt--);
}

void delay_ms(uint32_t nms)
{
    uint32_t start = systick_counter;
    while((systick_counter - start) < nms);
}

/*==================== MS8313 ??? ====================*/
void ms8313_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
    gpio_init(MS8313_EN1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MS8313_EN1_PIN);
    gpio_init(MS8313_IN1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MS8313_IN1_PIN);
    gpio_init(MS8313_EN1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MS8313_EN1_PIN);
    gpio_init(MS8313_IN2_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MS8313_IN2_PIN);
    gpio_init(MS8313_EN1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MS8313_EN1_PIN);
    gpio_init(MS8313_IN3_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MS8313_IN3_PIN);
    
    /* ????:???? */
    gpio_bit_reset(MS8313_EN1_PORT, MS8313_EN1_PIN);
    gpio_bit_reset(MS8313_IN1_PORT, MS8313_IN1_PIN);
    gpio_bit_reset(MS8313_EN1_PORT, MS8313_EN1_PIN);
    gpio_bit_reset(MS8313_IN2_PORT, MS8313_IN2_PIN);
    gpio_bit_reset(MS8313_EN1_PORT, MS8313_EN1_PIN);
    gpio_bit_reset(MS8313_IN3_PORT, MS8313_IN3_PIN);
}

/*==================== MS8313 ?? ====================*/
void ms8313_set_output(uint8_t en1, uint8_t in1, 
                        uint8_t en2, uint8_t in2, 
                        uint8_t en3, uint8_t in3)
{
    if(en1) gpio_bit_set(MS8313_EN1_PORT, MS8313_EN1_PIN);
    else gpio_bit_reset(MS8313_EN1_PORT, MS8313_EN1_PIN);
    
    if(in1) gpio_bit_set(MS8313_IN1_PORT, MS8313_IN1_PIN);
    else gpio_bit_reset(MS8313_IN1_PORT, MS8313_IN1_PIN);
    
//    if(en2) gpio_bit_set(MS8313_EN2_PORT, MS8313_EN2_PIN);
//    else gpio_bit_reset(MS8313_EN2_PORT, MS8313_EN2_PIN);
    
    if(in2) gpio_bit_set(MS8313_IN2_PORT, MS8313_IN2_PIN);
    else gpio_bit_reset(MS8313_IN2_PORT, MS8313_IN2_PIN);
    
//    if(en3) gpio_bit_set(MS8313_EN3_PORT, MS8313_EN3_PIN);
//    else gpio_bit_reset(MS8313_EN3_PORT, MS8313_EN3_PIN);
    
    if(in3) gpio_bit_set(MS8313_IN3_PORT, MS8313_IN3_PIN);
    else gpio_bit_reset(MS8313_IN3_PORT, MS8313_IN3_PIN);
}

void ms8313_commutation(uint8_t step)
{
    const uint8_t *cfg;
    
    if(step > 5) step = 0;
    cfg = commutation_steps[step];
    ms8313_set_output(cfg[0], cfg[1], cfg[2], cfg[3], cfg[4], cfg[5]);
}

void ms8313_stop(void)
{
    ms8313_set_output(0, 0, 0, 0, 0, 0);
}

/*==================== I2C ???? ====================*/
/* ?? I2C ?? - ?? GD32 ??? */
static void i2c_write_byte(uint8_t data)
{
    i2c_data_transmit(I2C0, data);
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
}

static uint8_t i2c_read_byte(void)
{
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
    return i2c_data_receive(I2C0);
}

static void i2c_start(void)
{
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
}

static void i2c_send_addr(uint8_t addr, uint8_t dir)
{
    i2c_master_addressing(I2C0, addr, dir);
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
}

static void i2c_stop(void)
{
    i2c_stop_on_bus(I2C0);
}

/* AS5600 ????? */
static uint8_t as5600_read_reg(uint8_t reg)
{
    uint8_t data;
    
    i2c_start();
    i2c_send_addr(AS5600_ADDR, I2C_TRANSMITTER);
    i2c_write_byte(reg);
    
    i2c_start();
    i2c_send_addr(AS5600_ADDR, I2C_RECEIVER);
    data = i2c_read_byte();
    i2c_stop();
    
    return data;
}

/* AS5600 ??16???? */
static uint16_t as5600_read_reg16(uint8_t reg_h)
{
    uint8_t high, low;
    
    high = as5600_read_reg(reg_h);
    low = as5600_read_reg(reg_h + 1);
    
    return ((uint16_t)high << 8) | low;
}

/*==================== AS5600 ??? ====================*/
void as5600_i2c_init(void)
{
    /* ???? */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_I2C0);
    
    /* ???? */
    gpio_init(AS5600_SCL_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AS5600_SCL_PIN);
    gpio_init(AS5600_SDA_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AS5600_SDA_PIN);
    
    /* ?? I2C */
    i2c_deinit(I2C0);
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, AS5600_ADDR);
    i2c_enable(I2C0);
}

uint16_t as5600_read_raw_angle(void)
{
    return as5600_read_reg16(AS5600_ANGLE_H) & 0x0FFF;
}

float as5600_read_angle_degree(void)
{
    uint16_t raw = as5600_read_raw_angle();
    return (float)raw * 360.0f / 4096.0f;
}

uint8_t as5600_read_status(void)
{
    return as5600_read_reg(AS5600_STATUS);
}

uint8_t as5600_check_magnet(void)
{
    uint8_t status = as5600_read_status();
    return (status & 0x20) ? 1 : 0;
}

/*==================== PID ??? ====================*/
void pid_init(PID_TypeDef *pid, float kp, float ki, float kd, 
              float output_max, float output_min)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->target = 0;
    pid->feedback = 0;
    pid->error = 0;
    pid->prev_error = 0;
    pid->integral = 0;
    pid->output = 0;
    pid->output_max = output_max;
    pid->output_min = output_min;
}

float pid_compute(PID_TypeDef *pid, float target, float feedback)
{
    float delta_time;
    float derivative;
    
    delta_time = 0.01f;  /* ???? 10ms */
    
    pid->target = target;
    pid->feedback = feedback;
    pid->error = pid->target - pid->feedback;
    
    /* ??? */
    pid->integral += pid->error * delta_time;
    if(pid->integral > 100.0f) pid->integral = 100.0f;
    if(pid->integral < -100.0f) pid->integral = -100.0f;
    
    /* ??? */
    derivative = (pid->error - pid->prev_error) / delta_time;
    
    /* ?? */
    pid->output = pid->kp * pid->error + 
                  pid->ki * pid->integral + 
                  pid->kd * derivative;
    
    /* ?? */
    if(pid->output > pid->output_max) pid->output = pid->output_max;
    if(pid->output < pid->output_min) pid->output = pid->output_min;
    
    pid->prev_error = pid->error;
    
    return pid->output;
}

/*==================== BLDC ?? ====================*/
static uint8_t get_step_from_angle(uint16_t angle)
{
    uint8_t step;
    uint8_t i;
    
    step = 0;
    for(i = 1; i < 6; i++) {
        if(angle < angle_thresholds[i]) {
            step = i - 1;
            break;
        } else {
            step = 5;
        }
    }
    return step;
}

void bldc_run(uint16_t pwm_duty)
{
    uint16_t current_angle;
    uint8_t step;
    
    current_angle = as5600_read_raw_angle();
    step = get_step_from_angle(current_angle);
    ms8313_commutation(step);
    
    /* pwm_duty ??????? PWM ???? */
    (void)pwm_duty;  /* ??????? */
}

void bldc_run_speed(float target_rpm)
{
    static uint32_t last_time = 0;
    static uint16_t last_angle = 0;
    static PID_TypeDef speed_pid;
    static uint8_t pid_initialized = 0;
    
    uint32_t now;
    uint16_t current_angle;
    int16_t delta_angle;
    uint8_t step;
    float delta_time;
    float current_rpm;
    float pwm_output;
    uint16_t duty;
    
    /* ??? PID */
    if(!pid_initialized) {
        pid_init(&speed_pid, 0.5f, 0.1f, 0.05f, 1000.0f, -1000.0f);
        pid_initialized = 1;
    }
    
    now = get_systick_ms();
    current_angle = as5600_read_raw_angle();
    
    /* ?50ms?????? */
    if(now - last_time > 50) {
        delta_angle = (int16_t)current_angle - (int16_t)last_angle;
        if(delta_angle < 0) delta_angle += 4096;
        
        delta_time = (now - last_time) / 1000.0f;
        current_rpm = (delta_angle / 4096.0f) * 60.0f / delta_time;
        
        pwm_output = pid_compute(&speed_pid, target_rpm, current_rpm);
        
        duty = (uint16_t)fabsf(pwm_output);
        if(duty > 1000) duty = 1000;
        
        last_angle = current_angle;
        last_time = now;
    }
    
    /* ?? */
    step = get_step_from_angle(current_angle);
    ms8313_commutation(step);
}

void bldc_run_position(uint16_t target_angle)
{
    static PID_TypeDef pos_pid;
    static uint8_t pid_initialized = 0;
    
    uint16_t current_angle;
    int16_t error;
    float output;
    uint8_t step;
    uint16_t duty;
    
    if(!pid_initialized) {
        pid_init(&pos_pid, 1.2f, 0.05f, 0.2f, 1000.0f, -1000.0f);
        pid_initialized = 1;
    }
    
    current_angle = as5600_read_raw_angle();
    
    /* ???? */
    error = (int16_t)target_angle - (int16_t)current_angle;
    if(error > 2048) error -= 4096;
    if(error < -2048) error += 4096;
    
    output = pid_compute(&pos_pid, (float)target_angle, (float)current_angle);
    
    duty = (uint16_t)fabsf(output);
    if(duty > 1000) duty = 1000;
    
    step = get_step_from_angle(current_angle);
    ms8313_commutation(step);
    
    /* ??????????? */
    if(error < 5 && error > -5 && duty < 50) {
        ms8313_stop();
    }
}

void bldc_stop(void)
{
    ms8313_stop();
}

void bldc_motor_init(void)
{
    /* ?? SysTick ?? 1ms ?? */
    if(SysTick_Config(SystemCoreClock / 1000)) {
        while(1);
    }
    
    ms8313_init();
    as5600_i2c_init();
    
    delay_ms(100);
    
    if(!as5600_check_magnet()) {
        /* ??????,??????? */
    }
}

