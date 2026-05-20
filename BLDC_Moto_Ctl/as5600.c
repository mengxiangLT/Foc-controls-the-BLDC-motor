#include "as5600.h"
#include "i2c.h"
//#include "soft_i2c.h"
#include "stdio.h"
#include "includes.h"

#define I2C_TIME_OUT_BASE   10
#define I2C_TIME_OUT_BYTE   1

#define AS5600_WR_ADDRESS 	0x6C
#define AS5600_RD_ADDRESS 	0x6D

#define AS5600_CPR      		4096
#define STATE_ADDRESS				0x0B
#define RAW_ANGLE 					0x0D

/*取绝对值*/
#define abs(x) ((x)>0?(x):-(x))

#define MAX_RETRIES 3

SoftI2C_t soft_i2c_motor2 = {
	.scl_port = GPIOC,
	.scl_pin = GPIO_PIN_15,
	.sda_port = GPIOC,
	.sda_pin = GPIO_PIN_14
};

static uint8_t soft_i2c_initialized = 0;
static uint16_t last_valid_raw2 = 0;
static uint8_t init_angle_read = 0;


float I2C_getRawCount(void);


static int i2cWrite(I2C_HandleTypeDef* hi2c,uint8_t dev_addr, uint8_t *pData, uint32_t count) {
//  int status;
//  int i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;
  
//  status = HAL_I2C_Master_Transmit(hi2c, dev_addr, pData, count, i2c_time_out);
	    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
	
	  /* send slave address to I2C bus */
	  i2c_master_addressing(I2C0, dev_addr, I2C_TRANSMITTER);
	    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
    
    /* send the EEPROM's internal address to write to : only one byte address */
    i2c_data_transmit(I2C0, write_address);
    
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    
    /* while there is data to be written */
    while(count--){  
        i2c_data_transmit(I2C0, *pData);
        
        /* point to the next byte to be written */
        pData++; 
        
        /* wait until BTC bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    
    /* wait until the stop condition is finished */
    while(I2C_CTL0(I2C0)&0x0200);
//  return status;
}

void i2cRead(uint8_t* p_buffer, uint8_t read_address, uint16_t number_of_byte)
{  
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));

    if(2 == number_of_byte){
        i2c_ackpos_config(I2C0,I2C_ACKPOS_NEXT);
    }
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while(SET != i2c_flag_get( I2C0 , I2C_FLAG_TBE));

    /* enable I2C0*/
    i2c_enable(I2C0);
    
    /* send the EEPROM's internal address to write to */
    i2c_data_transmit(I2C0, read_address);  
    
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_RECEIVER);

    if(number_of_byte < 3){
        /* disable acknowledge */
        i2c_ack_config(I2C0,I2C_ACK_DISABLE);
    }
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
    
    if(1 == number_of_byte){
        /* send a stop condition to I2C bus */
        i2c_stop_on_bus(I2C0);
    }
    
    /* while there is data to be read */
    while(number_of_byte){
        if(3 == number_of_byte){
            /* wait until BTC bit is set */
            while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));

            /* disable acknowledge */
            i2c_ack_config(I2C0,I2C_ACK_DISABLE);
        }
        if(2 == number_of_byte){
            /* wait until BTC bit is set */
            while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
            
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(I2C0);
        }
        
        /* wait until the RBNE bit is set and clear it */
        if(i2c_flag_get(I2C0, I2C_FLAG_RBNE)){
            /* read a byte from the EEPROM */
            *p_buffer = i2c_data_receive(I2C0);
            
            /* point to the next location where the byte read will be saved */
            p_buffer++; 
            
            /* decrement the read bytes counter */
            number_of_byte--;
        } 
    }
    
    /* wait until the stop condition is finished */
    while(I2C_CTL0(I2C0)&0x0200);
    
    /* enable acknowledge */
    i2c_ack_config(I2C0,I2C_ACK_ENABLE);

    i2c_ackpos_config(I2C0,I2C_ACKPOS_CURRENT);
}


//static int i2cRead(I2C_HandleTypeDef* hi2c,uint8_t dev_addr, uint8_t *pData, uint32_t count) {
//  int status;
//  int i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;
//  
//  status = HAL_I2C_Master_Receive(hi2c, (dev_addr | 1), pData, count, i2c_time_out);
//  return status;
//}

uint16_t bsp_as5600GetRawAngle1(void) {
  uint16_t raw_angle = 0;
  uint8_t buffer[2] = {0};
  uint8_t raw_angle_register = 0x0c;
  int retry = 0;

  for( retry = 0;retry < MAX_RETRIES;retry++)
  {
	  i2cWrite(&hi2c3,AS5600_WR_ADDRESS, &raw_angle_register, 1);
	  int state = i2cRead(&hi2c3,AS5600_RD_ADDRESS, buffer, 2);
	  if(state == HAL_OK) {
		  raw_angle = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];
		  break;
	  }
	  delay_1ms(1);
  }
	
  return raw_angle&0x0fff;
}

uint16_t bsp_as5600GetRawAngle2(void) 
{
    uint16_t raw_angle = 0;
    int retry;

    // 初始化软I2C
    if(!soft_i2c_initialized) {
        SoftI2C_Init(&soft_i2c_motor2);
        soft_i2c_initialized = 1;

        // 尝试多次读取，直到成功获取第一个有效值
        for(retry = 0; retry < 20; retry++) {
            raw_angle = SoftI2C_ReadAS5600(&soft_i2c_motor2);
            if(raw_angle != 0 && raw_angle != 0xFFFF && raw_angle < 4096) {
                last_valid_raw2 = raw_angle;
                init_angle_read = 1;
                printf("Soft I2C init success: raw=%d\r\n", raw_angle);
                break;
            }
            delay_1ms(10);
        }

        if(!init_angle_read) {
            printf("Soft I2C init FAILED! Check wiring.\r\n");
        }
    }

    // 正常读取
    if(init_angle_read) {
        raw_angle = SoftI2C_ReadAS5600(&soft_i2c_motor2);

        // 验证读取结果的有效性
        if(raw_angle != 0 && raw_angle != 0xFFFF && raw_angle < 4096) {
            last_valid_raw2 = raw_angle;
        } else {
            // 读取失败，使用上次有效值
            raw_angle = last_valid_raw2;
        }
    } else {
        raw_angle = last_valid_raw2;
    }

    return raw_angle & 0x0FFF;
}

float I2C_getRawCount1(void)//获取角度
{ 
  float angle=bsp_as5600GetRawAngle1()*0.0878f;
  angle = _normalizeAngle(angle);
  
  return angle;	
}

float I2C_getRawCount2(void)//获取角度
{ 
  float angle=bsp_as5600GetRawAngle2()*0.0878f;
  angle = _normalizeAngle(angle);
  
  return angle;	
}



