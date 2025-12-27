/**
 * @file    init.c
 * @brief   系统初始化配置
 * @details 设备池、Shell、环境变量定义
 */

#include "main.h"
#include <env.h>

/*===========================================================================*/
/*                              Shell 配置                                    */
/*===========================================================================*/

shell Shell = {
    .Shell_Init = false,               /* Shell未初始化 */
    .c = 0,                            /* 初始化接收字符 */
    .Res_len = 0,                      /* 初始化接收长度 */
    .UART_NOTE = 0,                    /* 初始化串口节点 */
    .RunStae = 0,                      /* 初始化运行状态 */
    .Data_Receive = USART1_ReceiveChar /* 数据接收函数指针 */
};

Sysfpoint Shell_Sysfpoint;

/*===========================================================================*/
/*                              设备信息                                      */
/*===========================================================================*/

DeviceFamily STM32F103C8T6_Device = {
    .Architecture = "cortex-m4",
    .DeviceName = "STM32F407VE",
    .OS = "BareMetal",
    .Device = "STM32F4",
    .User = "Admin",
    .Password = "133990",
    .Version = "1.0.0"};

/*===========================================================================*/
/*                              设备池                                        */
/*===========================================================================*/

dev_info_t Dev_info_poor[] = {
    /* USART1 - 调试串口 */
    {
        .name = DEBUG_UART_NAME,
        .init = usart1_init,
        .enable = usart1_start,
        .disable = usart1_stop,
        .arg.ptr = (void *)&debug},

    /* USART3 - 通用串口 (PB10-TX, PB11-RX) */
    {
        .name = USART3_NAME,
        .init = usart3_init,
        .enable = usart3_start,
        .disable = usart3_stop,
        .arg.ptr = (void *)&uart3},

    /* I2C1 */
    {
        .name = "I2C1",
        .init = I2C1_Init,
        .enable = NULL,
        .disable = NULL,
        .arg.ptr = NULL},

    /* PWM */
    {
        .name = "PWM",
        .init = pwm_init,
        .enable = NULL,
        .disable = NULL,
        .arg.ptr = NULL},

    /* NVIC */
    {
        .name = "NVIC",
        .init = nvic_init,
        .enable = NULL,
        .disable = NULL,
        .arg.ptr = NULL},

    /* 板载LED */
    {
        .name = ONBOARD_LED_NAME,
        .init = led_init,
        .enable = led_on,
        .disable = led_off,
        .arg.ptr = NULL},

    /* TIM2 定时器 */
    {
        .name = "TIM2",
        .init = TIM_Init,
        .enable = NULL,
        .disable = NULL,
        .arg.argv = (void *[]){TIM2, (void *)10}},

    /* 设备池结束标志 */
    DEV_INFO_END};

/*===========================================================================*/
/*                              环境变量                                      */
/*===========================================================================*/

EnvVar env_vars[] = {
    {.name = "stop", .callback = stop},
    {.name = "pwm", .callback = pwm_set},
    {.name = "printf", .callback = env_printf},
    {NULL} /* 环境变量列表结束标志 */
};

/*===========================================================================*/
/*                              全局变量                                      */
/*===========================================================================*/

float pitch, roll, yaw;
float hmc_heading;
float altitude;