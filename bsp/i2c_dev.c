#include <config.h>
uint8_t mpu6050_i2c_write(uint8_t addr, uint8_t reg, uint16_t length, uint8_t *data)
{
    return Soft_IIC_Write_Len(&i2c_dev, addr, reg, length, data);
}

uint8_t mpu6050_i2c_read(uint8_t addr, uint8_t reg, uint16_t length, uint8_t *data)
{
    return Soft_IIC_Read_Len(&i2c_dev, addr, reg, length, data);
}
