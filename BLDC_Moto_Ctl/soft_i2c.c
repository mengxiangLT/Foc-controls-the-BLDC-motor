#include "soft_i2c.h"
#include "include.h"

// 使用更精确的微秒延时（基于SysTick）
static void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * 160;  // 160MHz主频
    while((DWT->CYCCNT - start) < cycles);
}

// 初始化DWT计时器（在main.c中调用一次）
void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


void SoftI2C_Init(SoftI2C_t* i2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(i2c->scl_port == GPIOB || i2c->sda_port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    if(i2c->scl_port == GPIOC || i2c->sda_port == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    if(i2c->scl_port == GPIOA || i2c->sda_port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }

    // 配置 SCL 和 SDA 为开漏输出
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin = i2c->scl_pin;
    HAL_GPIO_Init(i2c->scl_port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = i2c->sda_pin;
    HAL_GPIO_Init(i2c->sda_port, &GPIO_InitStruct);
    
    // 初始状态：SCL 和 SDA 都为高
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);
}

void SoftI2C_Start(SoftI2C_t* i2c)
{
    // SDA 先拉低，然后 SCL 拉低
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_SET);
    delay_us(3);
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_RESET);
    delay_us(3);
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_RESET);
    delay_us(3);
}

void SoftI2C_Stop(SoftI2C_t* i2c)
{
    // SCL 先拉高，然后 SDA 拉高
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_SET);
    delay_us(3);
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);
    delay_us(3);
}

uint8_t SoftI2C_WriteByte(SoftI2C_t* i2c, uint8_t data)
{
    // 发送 8 位数据
    for(int i = 0; i < 8; i++) {
        if(data & 0x80) {
            HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_RESET);
        }
        delay_us(2);
        HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_SET);
        delay_us(3);
        HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_RESET);
        delay_us(2);
        data <<= 1;
    }
    
    // 读取 ACK
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);
    delay_us(2);
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_SET);
    delay_us(3);
    uint8_t ack = HAL_GPIO_ReadPin(i2c->sda_port, i2c->sda_pin);
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_RESET);
    delay_us(2);
    
    return ack;  // 0 = ACK, 1 = NACK
}

uint8_t SoftI2C_ReadByte(SoftI2C_t* i2c, uint8_t ack)
{
    uint8_t data = 0;
    
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);
    
    for(int i = 0; i < 8; i++) {
        data <<= 1;
        HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_SET);
        delay_us(3);
        if(HAL_GPIO_ReadPin(i2c->sda_port, i2c->sda_pin)) {
            data |= 0x01;
        }
        HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_RESET);
        delay_us(2);
    }
    
    // 发送 ACK/NACK
    if(ack) {
        HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);  // NACK
    } else {
        HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_RESET); // ACK
    }
    delay_us(2);
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_SET);
    delay_us(3);
    HAL_GPIO_WritePin(i2c->scl_port, i2c->scl_pin, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(i2c->sda_port, i2c->sda_pin, GPIO_PIN_SET);
    
    return data;
}

uint16_t SoftI2C_ReadAS5600(SoftI2C_t* i2c)
{
    uint8_t buffer[2];
    
    // 写寄存器地址
    SoftI2C_Start(i2c);
    SoftI2C_WriteByte(i2c, 0x6C);  // AS5600 写地址
    SoftI2C_WriteByte(i2c, 0x0C);  // 角度寄存器
    SoftI2C_Stop(i2c);
    
    delay_us(5);
    
    // 读数据
    SoftI2C_Start(i2c);
    SoftI2C_WriteByte(i2c, 0x6D);  // AS5600 读地址
    buffer[0] = SoftI2C_ReadByte(i2c, 0);  // 高字节，发 ACK
    buffer[1] = SoftI2C_ReadByte(i2c, 1);  // 低字节，发 NACK
    SoftI2C_Stop(i2c);
    
    return ((uint16_t)buffer[0] << 8) | buffer[1];
}