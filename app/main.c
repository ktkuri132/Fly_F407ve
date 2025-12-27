#include "main.h"
#include <stdint.h>
#include <mpu6050/inv_mpu.h>
#include <hmc588/hmc588.h>
#include <bmp280/bmp280.h>
#include <config.h>
#include <env.h>

int main()
{
    Device_Registration(Dev_info_poor); // 初始化设备模型
    MCU_Shell_Init(&Shell,&STM32F103C8T6_Device); // 初始化Shell
    Sys_cmd_Init();                     // 初始化系统命令
    mpu_dmp_init();                     // 初始化MPU6050 DMP功能
    HMC5883L_Init();                    // 初始化HMC5883L磁力计
    // printf("%d\n", BMP280_Init());                     // 初始化BMP280气压计
    // printf("BMP280 Chip ID: 0x%02X\n", BMP280_ReadChipID());
    // altitude = BMP280_ReadPressure();
    // printf("Altitude: %.2f m\n", altitude);
    // pwm_set_all(3000, 3000, 3000, 3000); // 设置所有通道占空比为50%
    while (1)
    {
        Task_Switch_Tick_Handler(&Shell_Sysfpoint); // 任务切换处理
        irq_handle_runner(irq_handles); // 中断处理函数运行
    }
    return 0;
}
