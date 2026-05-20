#include "as5600_i2c.h"

/*==================== AS5600 ???? ====================*/

/* ?? AS5600 ??? (???) - ?? eeprom_buffer_read ?? */
static uint8_t as5600_read_reg(uint8_t reg)
{
    uint8_t data = 0;
    
    /* ?? I2C ???? */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    
    /* ?? START ?? */
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* ??????(???) */
    i2c_master_addressing(I2C0, AS5600_ADDR, I2C_TRANSMITTER);
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
    /* ???????? */
    while(SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
    
    /* ??????????? */
    i2c_data_transmit(I2C0, reg);
    
    /* ?? BTC ?(????) */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    
    /* ???? START ?? */
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* ??????(???) */
    i2c_master_addressing(I2C0, AS5600_ADDR, I2C_RECEIVER);
    
    /* ?????1???,??????????? STOP */
    i2c_ack_config(I2C0, I2C_ACK_DISABLE);
    
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
    /* ?? STOP ?? */
    i2c_stop_on_bus(I2C0);
    
    /* ????????? */
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
    
    /* ???? */
    data = i2c_data_receive(I2C0);
    
    /* ?? STOP ???? */
    while(I2C_CTL0(I2C0) & 0x0200);
    
    /* ?????? */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
    
    return data;
}

/* ?? AS5600 16???? (2??) - ?? eeprom_buffer_read ?? */
static uint16_t as5600_read_reg16(uint8_t reg_h)
{
    uint8_t high, low;
    uint16_t data;
    
    /* ?? I2C ???? */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    
    /* ??????? NEXT(??2????) */
    i2c_ackpos_config(I2C0, I2C_ACKPOS_NEXT);
    
    /* ?? START ?? */
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* ??????(???) */
    i2c_master_addressing(I2C0, AS5600_ADDR, I2C_TRANSMITTER);
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
    /* ???????? */
    while(SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
    
    /* ?? I2C */
    i2c_enable(I2C0);
    
    /* ???????????(????) */
    i2c_data_transmit(I2C0, reg_h);
    
    /* ?? BTC ? */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    
    /* ???? START ?? */
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* ??????(???) */
    i2c_master_addressing(I2C0, AS5600_ADDR, I2C_RECEIVER);
    
    /* ??2???,?????? */
    i2c_ack_config(I2C0, I2C_ACK_DISABLE);
    
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
    /* ?? BTC ? */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    
    /* ?? STOP ?? */
    i2c_stop_on_bus(I2C0);
    
    /* ???8? */
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
    high = i2c_data_receive(I2C0);
    
    /* ???8? */
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
    low = i2c_data_receive(I2C0);
    
    /* ?? STOP ???? */
    while(I2C_CTL0(I2C0) & 0x0200);
    
    /* ????????????? */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
    i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
    
    data = ((uint16_t)high << 8) | low;
    return data & 0x0FFF;  /* AS5600 ??12????? */
}

/*==================== AS5600 ??? ====================*/
void as5600_i2c_init(void)
{
    /* ???? */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_I2C0);
    
    /* ??????????? */
    gpio_init(AS5600_SCL_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AS5600_SCL_PIN);
    gpio_init(AS5600_SDA_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AS5600_SDA_PIN);
    
    /* ?? I2C ?? - ???????? */
    i2c_clock_config(I2C0, 800000, I2C_DTCY_2);  /* 100kHz */
    
    /* ?? I2C ?? (??????????,??????) */
    /* ??:????? GD32 ????????,??????????? */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);
    
    /* ?? I2C */
    i2c_enable(I2C0);
    
    /* ???? */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

uint16_t as5600_read_raw_angle(void)
{
    return as5600_read_reg16(AS5600_ANGLE_H);
}

float as5600_read_angle_degree(void)
{
    uint16_t raw = as5600_read_raw_angle();
	  return (float)raw;
//    return (float)raw * 360.0f / 4096.0f;
}

uint8_t as5600_read_status(void)
{
    return as5600_read_reg(AS5600_STATUS);
}

uint8_t as5600_check_magnet(void)
{
    uint8_t status = as5600_read_status();
    return (status & 0x20) ? 1 : 0;  /* MD bit (bit5) */
}

