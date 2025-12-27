#include <main.h>
#include <shell/shell.h>
#include <irq/df_irq.h>
#include <mpu6050/inv_mpu.h>
#include <hmc588/hmc588.h>
#include <bmp280/bmp280.h>

int Serial_1_IRQHandlerCallback(int argc,void *argv[]){
    (void)argc;
    (void)argv;
    BIE_UART(Shell.Data_Receive(NULL,NULL), &Shell_Sysfpoint, &Shell, env_vars, &STM32F103C8T6_Device);
    return 0;
}

int Time_2_IRQHandlerCallback(int argc,void *argv[]){
    (void)argc;
    (void)argv;
    mpu_dmp_get_data(&pitch, &roll, &yaw); // 获取姿态数据
    hmc_heading = HMC5883L_GetHeading(); // 获取航向数据
    uart3.printf("Pitch: %.2f, Roll: %.2f, Yaw: %.2f, Heading: %.2f, Altitude: %.2f m\n",
                    pitch, roll, yaw, hmc_heading, altitude);
    return 0;
}